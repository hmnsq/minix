#ضمن "kernel/watchdog.h"
#ضمن "glo.h"
#ضمن <minix/minlib.h>
#ضمن <minix/u64.h>

#ضمن "apic.h"

#وصف_ڪريو CPUID_UNHALTED_CORE_چڪرن_موجود	0

/*
 * Intel architecture performance counters watchdog
 */

ساكن بنية arch_watchdog intel_arch_watchdog;
ساكن بنية arch_watchdog amd_watchdog;

ساكن فراغ intel_arch_watchdog_init(ثابت بنانشان cpu)
{
	u64_t cpuf;
	u32_t val;

	ia32_msr_write(INTEL_MSR_PERFMON_CRT0, 0, 0);

	/* Int, OS, USR, Core ccyles */
	val = 1 << 20 | 1 << 17 | 1 << 16 | 0x3c;
	ia32_msr_write(INTEL_MSR_PERFMON_SEL0, 0, val);

	/*
	 * should give as a tick approx. every 0.5-1s, the perf counter has only
	 * lowest 31 bits writable :(
	 */
	cpuf = cpu_get_freq(cpu);
	طالما (ex64hi(cpuf) || ex64lo(cpuf) > 0x7fffffffU)
		cpuf /= 2;
	cpuf = make64(-ex64lo(cpuf), ex64hi(cpuf));
	watchdog->resetval = watchdog->watchdog_resetval = cpuf;

	ia32_msr_write(INTEL_MSR_PERFMON_CRT0, 0, ex64lo(cpuf));

	ia32_msr_write(INTEL_MSR_PERFMON_SEL0, 0,
			val | INTEL_MSR_PERFMON_SEL0_ENABLE);

	/* unmask the performance counter interrupt */
	lapic_write(LAPIC_LVTPCR, APIC_ICR_DM_NMI);
}

ساكن فراغ intel_arch_watchdog_reinit(ثابت بنانشان cpu)
{
	lapic_write(LAPIC_LVTPCR, APIC_ICR_DM_NMI);
	ia32_msr_write(INTEL_MSR_PERFMON_CRT0, 0, ex64lo(watchdog->resetval));
}

صحيح arch_watchdog_init(فراغ)
{
	u32_t eax, ebx, ecx, edx;
	بنانشان cpu = cpuid;

	إذا (!lapic_addr) {
		لکو("ERROR : Cannot use NMI watchdog if APIC is not enabled\n");
		أرجع -1;
	}

	إذا (cpu_info[cpu].vendor == CPU_VENDOR_INTEL) {
		eax = 0xA;

		_cpuid(&eax, &ebx, &ecx, &edx);

		/* FIXME currently we support only watchdog based on the intel
		 * architectural performance counters. Some Intel CPUs don't have this
		 * feature
		 */
		إذا (ebx & (1 << CPUID_UNHALTED_CORE_چڪرن_موجود))
			أرجع -1;
		إذا (!((((eax >> 8)) & 0xff) > 0))
			أرجع -1;

		watchdog = &intel_arch_watchdog;
	} وإلا إذا (cpu_info[cpu].vendor == CPU_VENDOR_AMD) {
		إذا (cpu_info[cpu].family != 6 &&
				cpu_info[cpu].family != 15 &&
				cpu_info[cpu].family != 16 &&
				cpu_info[cpu].family != 17)
			أرجع -1;
		وإلا
			watchdog = &amd_watchdog;
	} وإلا
		أرجع -1;

	/* Setup PC overflow as NMI for watchdog, it is masked for now */
	lapic_write(LAPIC_LVTPCR, APIC_ICR_INT_نقاب | APIC_ICR_DM_NMI);
	(فراغ) lapic_read(LAPIC_LVTPCR);

	/* double check if LAPIC is enabled */
	إذا (lapic_addr && watchdog->init) {
		watchdog->init(cpuid);
	}

	أرجع 0;
}

فراغ arch_watchdog_stop(فراغ)
{
}

فراغ arch_watchdog_lockup(ثابت بنية nmi_چوکٺو * چوکٺو)
{
	لکو("KERNEL LOCK UP\n"
			"eax    0x%08x\n"
			"ecx    0x%08x\n"
			"edx    0x%08x\n"
			"ebx    0x%08x\n"
			"ebp    0x%08x\n"
			"esi    0x%08x\n"
			"edi    0x%08x\n"
			"gs     0x%08x\n"
			"fs     0x%08x\n"
			"es     0x%08x\n"
			"ds     0x%08x\n"
			"pc     0x%08x\n"
			"cs     0x%08x\n"
			"eflags 0x%08x\n",
			چوکٺو->eax,
			چوکٺو->ecx,
			چوکٺو->edx,
			چوکٺو->ebx,
			چوکٺو->ebp,
			چوکٺو->esi,
			چوکٺو->edi,
			چوکٺو->gs,
			چوکٺو->fs,
			چوکٺو->es,
			چوکٺو->ds,
			چوکٺو->pc,
			چوکٺو->cs,
			چوکٺو->eflags
			);
	panic("Kernel lockup");
}

صحيح i386_watchdog_شروعات(فراغ)
{
	إذا (arch_watchdog_init()) {
		لکو("WARNING watchdog initialization "
				"failed! Disabled\n");
		watchdog_enabled = 0;
		أرجع -1;
	}
	وإلا
		BOOT_VERBOSE(لکو("Watchdog enabled\n"););

	أرجع 0;
}

ساكن صحيح intel_arch_watchdog_profile_init(ثابت بنانشان freq)
{
	u64_t cpuf;

	/* FIXME works only if all CPUs have the same freq */
	cpuf = cpu_get_freq(cpuid);
	cpuf /= freq;

	/*
	 * if freq is too low and the cpu freq too high we may get in a range of
	 * insane value which cannot be handled by the 31bit CPU perf counter
	 */
	إذا (ex64hi(cpuf) != 0 || ex64lo(cpuf) > 0x7fffffffU) {
		لکو("ERROR : nmi watchdog ticks exceed 31bits, use higher frequency\n");
		أرجع EINVAL;
	}

	cpuf = make64(-ex64lo(cpuf), ex64hi(cpuf));
	watchdog->profile_resetval = cpuf;

	أرجع OK;
}

ساكن بنية arch_watchdog intel_arch_watchdog = {
	/*.init = */		intel_arch_watchdog_init,
	/*.reinit = */		intel_arch_watchdog_reinit,
	/*.profile_init = */	intel_arch_watchdog_profile_init
};

#وصف_ڪريو AMD_MSR_EVENT_SEL0		0xc0010000
#وصف_ڪريو AMD_MSR_EVENT_CTR0		0xc0010004
#وصف_ڪريو AMD_MSR_EVENT_SEL0_ENABLE	(1 << 22)

ساكن فراغ amd_watchdog_init(ثابت بنانشان cpu)
{
	u64_t cpuf;
	u32_t val;

	ia32_msr_write(AMD_MSR_EVENT_CTR0, 0, 0);

	/* Int, OS, USR, Cycles cpu is running */
	val = 1 << 20 | 1 << 17 | 1 << 16 | 0x76;
	ia32_msr_write(AMD_MSR_EVENT_SEL0, 0, val);

	cpuf = -cpu_get_freq(cpu);
	watchdog->resetval = watchdog->watchdog_resetval = cpuf;

	ia32_msr_write(AMD_MSR_EVENT_CTR0,
		       ex64hi(watchdog->resetval), ex64lo(watchdog->resetval));

	ia32_msr_write(AMD_MSR_EVENT_SEL0, 0,
			val | AMD_MSR_EVENT_SEL0_ENABLE);

	/* unmask the performance counter interrupt */
	lapic_write(LAPIC_LVTPCR, APIC_ICR_DM_NMI);
}

ساكن فراغ amd_watchdog_reinit(ثابت بنانشان cpu)
{
	lapic_write(LAPIC_LVTPCR, APIC_ICR_DM_NMI);
	ia32_msr_write(AMD_MSR_EVENT_CTR0,
		       ex64hi(watchdog->resetval), ex64lo(watchdog->resetval));
}

ساكن صحيح amd_watchdog_profile_init(ثابت بنانشان freq)
{
	u64_t cpuf;

	/* FIXME works only if all CPUs have the same freq */
	cpuf = cpu_get_freq(cpuid);
	cpuf = -cpuf / freq;

	watchdog->profile_resetval = cpuf;

	أرجع OK;
}

ساكن بنية arch_watchdog amd_watchdog = {
	/*.init = */		amd_watchdog_init,
	/*.reinit = */		amd_watchdog_reinit,
	/*.profile_init = */	amd_watchdog_profile_init
};
