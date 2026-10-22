#अन्तर्भाव "kernel/watchdog.h"
#अन्तर्भाव "glo.h"
#अन्तर्भाव <minix/minlib.h>
#अन्तर्भाव <minix/u64.h>

#अन्तर्भाव "apic.h"

#define CPUID_UNHALTED_CORE_CYCLES_AVAILABLE	0

/*
 * Intel architecture performance counters watchdog
 */

स्थिर संरचना arch_watchdog intel_arch_watchdog;
स्थिर संरचना arch_watchdog amd_watchdog;

स्थिर शून्य intel_arch_watchdog_init(अचर अचिन्हित cpu)
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
	जबसम्म (ex64hi(cpuf) || ex64lo(cpuf) > 0x7fffffffU)
		cpuf /= 2;
	cpuf = make64(-ex64lo(cpuf), ex64hi(cpuf));
	watchdog->resetval = watchdog->watchdog_resetval = cpuf;

	ia32_msr_write(INTEL_MSR_PERFMON_CRT0, 0, ex64lo(cpuf));

	ia32_msr_write(INTEL_MSR_PERFMON_SEL0, 0,
			val | INTEL_MSR_PERFMON_SEL0_ENABLE);

	/* unmask the performance counter interrupt */
	lapic_write(LAPIC_LVTPCR, APIC_ICR_DM_NMI);
}

स्थिर शून्य intel_arch_watchdog_reinit(अचर अचिन्हित cpu)
{
	lapic_write(LAPIC_LVTPCR, APIC_ICR_DM_NMI);
	ia32_msr_write(INTEL_MSR_PERFMON_CRT0, 0, ex64lo(watchdog->resetval));
}

पूर्णाङ्क arch_watchdog_init(शून्य)
{
	u32_t eax, ebx, ecx, edx;
	अचिन्हित cpu = cpuid;

	यदि (!lapic_addr) {
		म_लिखो("ERROR : Cannot use NMI watchdog if APIC is not enabled\n");
		फर्काऊ -1;
	}

	यदि (cpu_info[cpu].vendor == CPU_VENDOR_INTEL) {
		eax = 0xA;

		_cpuid(&eax, &ebx, &ecx, &edx);

		/* FIXME currently we support only watchdog based on the intel
		 * architectural performance counters. Some Intel CPUs don't have this
		 * feature
		 */
		यदि (ebx & (1 << CPUID_UNHALTED_CORE_CYCLES_AVAILABLE))
			फर्काऊ -1;
		यदि (!((((eax >> 8)) & 0xff) > 0))
			फर्काऊ -1;

		watchdog = &intel_arch_watchdog;
	} अन्यथा यदि (cpu_info[cpu].vendor == CPU_VENDOR_AMD) {
		यदि (cpu_info[cpu].family != 6 &&
				cpu_info[cpu].family != 15 &&
				cpu_info[cpu].family != 16 &&
				cpu_info[cpu].family != 17)
			फर्काऊ -1;
		अन्यथा
			watchdog = &amd_watchdog;
	} अन्यथा
		फर्काऊ -1;

	/* Setup PC overflow as NMI for watchdog, it is masked for now */
	lapic_write(LAPIC_LVTPCR, APIC_ICR_INT_MASK | APIC_ICR_DM_NMI);
	(शून्य) lapic_read(LAPIC_LVTPCR);

	/* double check if LAPIC is enabled */
	यदि (lapic_addr && watchdog->init) {
		watchdog->init(cpuid);
	}

	फर्काऊ 0;
}

शून्य arch_watchdog_stop(शून्य)
{
}

शून्य arch_watchdog_lockup(अचर संरचना nmi_frame * frame)
{
	म_लिखो("KERNEL LOCK UP\n"
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
			frame->eax,
			frame->ecx,
			frame->edx,
			frame->ebx,
			frame->ebp,
			frame->esi,
			frame->edi,
			frame->gs,
			frame->fs,
			frame->es,
			frame->ds,
			frame->pc,
			frame->cs,
			frame->eflags
			);
	panic("Kernel lockup");
}

पूर्णाङ्क i386_watchdog_start(शून्य)
{
	यदि (arch_watchdog_init()) {
		म_लिखो("WARNING watchdog initialization "
				"failed! Disabled\n");
		watchdog_enabled = 0;
		फर्काऊ -1;
	}
	अन्यथा
		BOOT_VERBOSE(म_लिखो("Watchdog enabled\n"););

	फर्काऊ 0;
}

स्थिर पूर्णाङ्क intel_arch_watchdog_profile_init(अचर अचिन्हित freq)
{
	u64_t cpuf;

	/* FIXME works only if all CPUs have the same freq */
	cpuf = cpu_get_freq(cpuid);
	cpuf /= freq;

	/*
	 * if freq is too low and the cpu freq too high we may get in a range of
	 * insane value which cannot be handled by the 31bit CPU perf counter
	 */
	यदि (ex64hi(cpuf) != 0 || ex64lo(cpuf) > 0x7fffffffU) {
		म_लिखो("ERROR : nmi watchdog ticks exceed 31bits, use higher frequency\n");
		फर्काऊ EINVAL;
	}

	cpuf = make64(-ex64lo(cpuf), ex64hi(cpuf));
	watchdog->profile_resetval = cpuf;

	फर्काऊ OK;
}

स्थिर संरचना arch_watchdog intel_arch_watchdog = {
	/*.init = */		intel_arch_watchdog_init,
	/*.reinit = */		intel_arch_watchdog_reinit,
	/*.profile_init = */	intel_arch_watchdog_profile_init
};

#define AMD_MSR_EVENT_SEL0		0xc0010000
#define AMD_MSR_EVENT_CTR0		0xc0010004
#define AMD_MSR_EVENT_SEL0_ENABLE	(1 << 22)

स्थिर शून्य amd_watchdog_init(अचर अचिन्हित cpu)
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

स्थिर शून्य amd_watchdog_reinit(अचर अचिन्हित cpu)
{
	lapic_write(LAPIC_LVTPCR, APIC_ICR_DM_NMI);
	ia32_msr_write(AMD_MSR_EVENT_CTR0,
		       ex64hi(watchdog->resetval), ex64lo(watchdog->resetval));
}

स्थिर पूर्णाङ्क amd_watchdog_profile_init(अचर अचिन्हित freq)
{
	u64_t cpuf;

	/* FIXME works only if all CPUs have the same freq */
	cpuf = cpu_get_freq(cpuid);
	cpuf = -cpuf / freq;

	watchdog->profile_resetval = cpuf;

	फर्काऊ OK;
}

स्थिर संरचना arch_watchdog amd_watchdog = {
	/*.init = */		amd_watchdog_init,
	/*.reinit = */		amd_watchdog_reinit,
	/*.profile_init = */	amd_watchdog_profile_init
};
