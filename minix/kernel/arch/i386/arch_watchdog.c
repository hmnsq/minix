#ಅನ್ತರ್ಭಾವ "kernel/watchdog.h"
#ಅನ್ತರ್ಭಾವ "glo.h"
#ಅನ್ತರ್ಭಾವ <minix/minlib.h>
#ಅನ್ತರ್ಭಾವ <minix/u64.h>

#ಅನ್ತರ್ಭಾವ "apic.h"

#ವ್ಯಾಖ್ಯಾನಿಸು CPUID_UNHALTED_CORE_ಚಕ್ರಗಳು_ಲಭ್ಯ	0

/*
 * Intel architecture performance counters watchdog
 */

ಸ್ಥಿರ ರಚನೆ arch_watchdog intel_arch_watchdog;
ಸ್ಥಿರ ರಚನೆ arch_watchdog amd_watchdog;

ಸ್ಥಿರ ಶೂನ್ಯ intel_arch_watchdog_init(ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ cpu)
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
	ಆಗ (ex64hi(cpuf) || ex64lo(cpuf) > 0x7fffffffU)
		cpuf /= 2;
	cpuf = make64(-ex64lo(cpuf), ex64hi(cpuf));
	watchdog->resetval = watchdog->watchdog_resetval = cpuf;

	ia32_msr_write(INTEL_MSR_PERFMON_CRT0, 0, ex64lo(cpuf));

	ia32_msr_write(INTEL_MSR_PERFMON_SEL0, 0,
			val | INTEL_MSR_PERFMON_SEL0_ENABLE);

	/* unmask the performance counter interrupt */
	lapic_write(LAPIC_LVTPCR, APIC_ICR_DM_NMI);
}

ಸ್ಥಿರ ಶೂನ್ಯ intel_arch_watchdog_reinit(ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ cpu)
{
	lapic_write(LAPIC_LVTPCR, APIC_ICR_DM_NMI);
	ia32_msr_write(INTEL_MSR_PERFMON_CRT0, 0, ex64lo(watchdog->resetval));
}

ಪೂರ್ಣಾಂಕ arch_watchdog_init(ಶೂನ್ಯ)
{
	u32_t eax, ebx, ecx, edx;
	ಚಿಹ್ನೆರಹಿತ cpu = cpuid;

	ಆದರೆ (!lapic_addr) {
		ಬರೆ("ERROR : Cannot use NMI watchdog if APIC is not enabled\n");
		ಹಿಂತಿರುಗಿ -1;
	}

	ಆದರೆ (cpu_info[cpu].vendor == CPU_VENDOR_INTEL) {
		eax = 0xA;

		_cpuid(&eax, &ebx, &ecx, &edx);

		/* FIXME currently we support only watchdog based on the intel
		 * architectural performance counters. Some Intel CPUs don't have this
		 * feature
		 */
		ಆದರೆ (ebx & (1 << CPUID_UNHALTED_CORE_ಚಕ್ರಗಳು_ಲಭ್ಯ))
			ಹಿಂತಿರುಗಿ -1;
		ಆದರೆ (!((((eax >> 8)) & 0xff) > 0))
			ಹಿಂತಿರುಗಿ -1;

		watchdog = &intel_arch_watchdog;
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (cpu_info[cpu].vendor == CPU_VENDOR_AMD) {
		ಆದರೆ (cpu_info[cpu].family != 6 &&
				cpu_info[cpu].family != 15 &&
				cpu_info[cpu].family != 16 &&
				cpu_info[cpu].family != 17)
			ಹಿಂತಿರುಗಿ -1;
		ಇಲ್ಲದಿದ್ದರೆ
			watchdog = &amd_watchdog;
	} ಇಲ್ಲದಿದ್ದರೆ
		ಹಿಂತಿರುಗಿ -1;

	/* Setup PC overflow as NMI for watchdog, it is masked for now */
	lapic_write(LAPIC_LVTPCR, APIC_ICR_INT_ಮುಖವಾಡ | APIC_ICR_DM_NMI);
	(ಶೂನ್ಯ) lapic_read(LAPIC_LVTPCR);

	/* double check if LAPIC is enabled */
	ಆದರೆ (lapic_addr && watchdog->init) {
		watchdog->init(cpuid);
	}

	ಹಿಂತಿರುಗಿ 0;
}

ಶೂನ್ಯ arch_watchdog_stop(ಶೂನ್ಯ)
{
}

ಶೂನ್ಯ arch_watchdog_lockup(ಸ್ಥಿರಾಂಕ ರಚನೆ nmi_ಚೌಕಟ್ಟು * ಚೌಕಟ್ಟು)
{
	ಬರೆ("KERNEL LOCK UP\n"
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
			ಚೌಕಟ್ಟು->eax,
			ಚೌಕಟ್ಟು->ecx,
			ಚೌಕಟ್ಟು->edx,
			ಚೌಕಟ್ಟು->ebx,
			ಚೌಕಟ್ಟು->ebp,
			ಚೌಕಟ್ಟು->esi,
			ಚೌಕಟ್ಟು->edi,
			ಚೌಕಟ್ಟು->gs,
			ಚೌಕಟ್ಟು->fs,
			ಚೌಕಟ್ಟು->es,
			ಚೌಕಟ್ಟು->ds,
			ಚೌಕಟ್ಟು->pc,
			ಚೌಕಟ್ಟು->cs,
			ಚೌಕಟ್ಟು->eflags
			);
	panic("Kernel lockup");
}

ಪೂರ್ಣಾಂಕ i386_watchdog_ಆರಂಭ(ಶೂನ್ಯ)
{
	ಆದರೆ (arch_watchdog_init()) {
		ಬರೆ("WARNING watchdog initialization "
				"failed! Disabled\n");
		watchdog_enabled = 0;
		ಹಿಂತಿರುಗಿ -1;
	}
	ಇಲ್ಲದಿದ್ದರೆ
		BOOT_VERBOSE(ಬರೆ("Watchdog enabled\n"););

	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ intel_arch_watchdog_profile_init(ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ freq)
{
	u64_t cpuf;

	/* FIXME works only if all CPUs have the same freq */
	cpuf = cpu_get_freq(cpuid);
	cpuf /= freq;

	/*
	 * if freq is too low and the cpu freq too high we may get in a range of
	 * insane value which cannot be handled by the 31bit CPU perf counter
	 */
	ಆದರೆ (ex64hi(cpuf) != 0 || ex64lo(cpuf) > 0x7fffffffU) {
		ಬರೆ("ERROR : nmi watchdog ticks exceed 31bits, use higher frequency\n");
		ಹಿಂತಿರುಗಿ EINVAL;
	}

	cpuf = make64(-ex64lo(cpuf), ex64hi(cpuf));
	watchdog->profile_resetval = cpuf;

	ಹಿಂತಿರುಗಿ OK;
}

ಸ್ಥಿರ ರಚನೆ arch_watchdog intel_arch_watchdog = {
	/*.init = */		intel_arch_watchdog_init,
	/*.reinit = */		intel_arch_watchdog_reinit,
	/*.profile_init = */	intel_arch_watchdog_profile_init
};

#ವ್ಯಾಖ್ಯಾನಿಸು AMD_MSR_EVENT_SEL0		0xc0010000
#ವ್ಯಾಖ್ಯಾನಿಸು AMD_MSR_EVENT_CTR0		0xc0010004
#ವ್ಯಾಖ್ಯಾನಿಸು AMD_MSR_EVENT_SEL0_ENABLE	(1 << 22)

ಸ್ಥಿರ ಶೂನ್ಯ amd_watchdog_init(ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ cpu)
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

ಸ್ಥಿರ ಶೂನ್ಯ amd_watchdog_reinit(ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ cpu)
{
	lapic_write(LAPIC_LVTPCR, APIC_ICR_DM_NMI);
	ia32_msr_write(AMD_MSR_EVENT_CTR0,
		       ex64hi(watchdog->resetval), ex64lo(watchdog->resetval));
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ amd_watchdog_profile_init(ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ freq)
{
	u64_t cpuf;

	/* FIXME works only if all CPUs have the same freq */
	cpuf = cpu_get_freq(cpuid);
	cpuf = -cpuf / freq;

	watchdog->profile_resetval = cpuf;

	ಹಿಂತಿರುಗಿ OK;
}

ಸ್ಥಿರ ರಚನೆ arch_watchdog amd_watchdog = {
	/*.init = */		amd_watchdog_init,
	/*.reinit = */		amd_watchdog_reinit,
	/*.profile_init = */	amd_watchdog_profile_init
};
