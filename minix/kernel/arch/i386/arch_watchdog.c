#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/watchdog.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "glo.h"
#ਅਨ੍ਤਰ੍ਭਾਵ <minix/minlib.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <minix/u64.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "apic.h"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CPUID_UNHALTED_CORE_ਚੱਕਰਾਂ_ਉਪਲਬਧ	0

/*
 * Intel architecture performance counters watchdog
 */

ਸਥਿਰ ਬਣਤਰ arch_watchdog intel_arch_watchdog;
ਸਥਿਰ ਬਣਤਰ arch_watchdog amd_watchdog;

ਸਥਿਰ ਖਾਲੀ intel_arch_watchdog_init(ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ cpu)
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
	ਜਦੋਂ (ex64hi(cpuf) || ex64lo(cpuf) > 0x7fffffffU)
		cpuf /= 2;
	cpuf = make64(-ex64lo(cpuf), ex64hi(cpuf));
	watchdog->resetval = watchdog->watchdog_resetval = cpuf;

	ia32_msr_write(INTEL_MSR_PERFMON_CRT0, 0, ex64lo(cpuf));

	ia32_msr_write(INTEL_MSR_PERFMON_SEL0, 0,
			val | INTEL_MSR_PERFMON_SEL0_ENABLE);

	/* unmask the performance counter interrupt */
	lapic_write(LAPIC_LVTPCR, APIC_ICR_DM_NMI);
}

ਸਥਿਰ ਖਾਲੀ intel_arch_watchdog_reinit(ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ cpu)
{
	lapic_write(LAPIC_LVTPCR, APIC_ICR_DM_NMI);
	ia32_msr_write(INTEL_MSR_PERFMON_CRT0, 0, ex64lo(watchdog->resetval));
}

ਪੂਰਨ arch_watchdog_init(ਖਾਲੀ)
{
	u32_t eax, ebx, ecx, edx;
	ਬਿਨਾਨਿਸ਼ਾਨੀ cpu = cpuid;

	ਜੇ (!lapic_addr) {
		ਲਿਖੋ("ERROR : Cannot use NMI watchdog if APIC is not enabled\n");
		ਵਾਪਸ -1;
	}

	ਜੇ (cpu_info[cpu].vendor == CPU_VENDOR_INTEL) {
		eax = 0xA;

		_cpuid(&eax, &ebx, &ecx, &edx);

		/* FIXME currently we support only watchdog based on the intel
		 * architectural performance counters. Some Intel CPUs don't have this
		 * feature
		 */
		ਜੇ (ebx & (1 << CPUID_UNHALTED_CORE_ਚੱਕਰਾਂ_ਉਪਲਬਧ))
			ਵਾਪਸ -1;
		ਜੇ (!((((eax >> 8)) & 0xff) > 0))
			ਵਾਪਸ -1;

		watchdog = &intel_arch_watchdog;
	} ਨਹੀਂਤਾਂ ਜੇ (cpu_info[cpu].vendor == CPU_VENDOR_AMD) {
		ਜੇ (cpu_info[cpu].family != 6 &&
				cpu_info[cpu].family != 15 &&
				cpu_info[cpu].family != 16 &&
				cpu_info[cpu].family != 17)
			ਵਾਪਸ -1;
		ਨਹੀਂਤਾਂ
			watchdog = &amd_watchdog;
	} ਨਹੀਂਤਾਂ
		ਵਾਪਸ -1;

	/* Setup PC overflow as NMI for watchdog, it is masked for now */
	lapic_write(LAPIC_LVTPCR, APIC_ICR_INT_ਨਕਾਬ | APIC_ICR_DM_NMI);
	(ਖਾਲੀ) lapic_read(LAPIC_LVTPCR);

	/* double check if LAPIC is enabled */
	ਜੇ (lapic_addr && watchdog->init) {
		watchdog->init(cpuid);
	}

	ਵਾਪਸ 0;
}

ਖਾਲੀ arch_watchdog_stop(ਖਾਲੀ)
{
}

ਖਾਲੀ arch_watchdog_lockup(ਅਚਲ ਬਣਤਰ nmi_ਚੌਖਟਾ * ਚੌਖਟਾ)
{
	ਲਿਖੋ("KERNEL LOCK UP\n"
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
			ਚੌਖਟਾ->eax,
			ਚੌਖਟਾ->ecx,
			ਚੌਖਟਾ->edx,
			ਚੌਖਟਾ->ebx,
			ਚੌਖਟਾ->ebp,
			ਚੌਖਟਾ->esi,
			ਚੌਖਟਾ->edi,
			ਚੌਖਟਾ->gs,
			ਚੌਖਟਾ->fs,
			ਚੌਖਟਾ->es,
			ਚੌਖਟਾ->ds,
			ਚੌਖਟਾ->pc,
			ਚੌਖਟਾ->cs,
			ਚੌਖਟਾ->eflags
			);
	panic("Kernel lockup");
}

ਪੂਰਨ i386_watchdog_ਸ਼ੁਰੂ(ਖਾਲੀ)
{
	ਜੇ (arch_watchdog_init()) {
		ਲਿਖੋ("WARNING watchdog initialization "
				"failed! Disabled\n");
		watchdog_enabled = 0;
		ਵਾਪਸ -1;
	}
	ਨਹੀਂਤਾਂ
		BOOT_VERBOSE(ਲਿਖੋ("Watchdog enabled\n"););

	ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ intel_arch_watchdog_profile_init(ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ freq)
{
	u64_t cpuf;

	/* FIXME works only if all CPUs have the same freq */
	cpuf = cpu_get_freq(cpuid);
	cpuf /= freq;

	/*
	 * if freq is too low and the cpu freq too high we may get in a range of
	 * insane value which cannot be handled by the 31bit CPU perf counter
	 */
	ਜੇ (ex64hi(cpuf) != 0 || ex64lo(cpuf) > 0x7fffffffU) {
		ਲਿਖੋ("ERROR : nmi watchdog ticks exceed 31bits, use higher frequency\n");
		ਵਾਪਸ EINVAL;
	}

	cpuf = make64(-ex64lo(cpuf), ex64hi(cpuf));
	watchdog->profile_resetval = cpuf;

	ਵਾਪਸ OK;
}

ਸਥਿਰ ਬਣਤਰ arch_watchdog intel_arch_watchdog = {
	/*.init = */		intel_arch_watchdog_init,
	/*.reinit = */		intel_arch_watchdog_reinit,
	/*.profile_init = */	intel_arch_watchdog_profile_init
};

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AMD_MSR_EVENT_SEL0		0xc0010000
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AMD_MSR_EVENT_CTR0		0xc0010004
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AMD_MSR_EVENT_SEL0_ENABLE	(1 << 22)

ਸਥਿਰ ਖਾਲੀ amd_watchdog_init(ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ cpu)
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

ਸਥਿਰ ਖਾਲੀ amd_watchdog_reinit(ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ cpu)
{
	lapic_write(LAPIC_LVTPCR, APIC_ICR_DM_NMI);
	ia32_msr_write(AMD_MSR_EVENT_CTR0,
		       ex64hi(watchdog->resetval), ex64lo(watchdog->resetval));
}

ਸਥਿਰ ਪੂਰਨ amd_watchdog_profile_init(ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ freq)
{
	u64_t cpuf;

	/* FIXME works only if all CPUs have the same freq */
	cpuf = cpu_get_freq(cpuid);
	cpuf = -cpuf / freq;

	watchdog->profile_resetval = cpuf;

	ਵਾਪਸ OK;
}

ਸਥਿਰ ਬਣਤਰ arch_watchdog amd_watchdog = {
	/*.init = */		amd_watchdog_init,
	/*.reinit = */		amd_watchdog_reinit,
	/*.profile_init = */	amd_watchdog_profile_init
};
