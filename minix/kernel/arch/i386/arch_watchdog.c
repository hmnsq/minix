#অন্তর্ভাব "kernel/watchdog.h"
#অন্তর্ভাব "glo.h"
#অন্তর্ভাব <minix/minlib.h>
#অন্তর্ভাব <minix/u64.h>

#অন্তর্ভাব "apic.h"

#define CPUID_UNHALTED_CORE_CYCLES_AVAILABLE	0

/*
 * Intel architecture performance counters watchdog
 */

স্থির কাঠামো arch_watchdog intel_arch_watchdog;
স্থির কাঠামো arch_watchdog amd_watchdog;

স্থির শূন্য intel_arch_watchdog_init(ধ্রুবক unsigned cpu)
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
	যখন (ex64hi(cpuf) || ex64lo(cpuf) > 0x7fffffffU)
		cpuf /= 2;
	cpuf = make64(-ex64lo(cpuf), ex64hi(cpuf));
	watchdog->resetval = watchdog->watchdog_resetval = cpuf;

	ia32_msr_write(INTEL_MSR_PERFMON_CRT0, 0, ex64lo(cpuf));

	ia32_msr_write(INTEL_MSR_PERFMON_SEL0, 0,
			val | INTEL_MSR_PERFMON_SEL0_ENABLE);

	/* unmask the performance counter interrupt */
	lapic_write(LAPIC_LVTPCR, APIC_ICR_DM_NMI);
}

স্থির শূন্য intel_arch_watchdog_reinit(ধ্রুবক unsigned cpu)
{
	lapic_write(LAPIC_LVTPCR, APIC_ICR_DM_NMI);
	ia32_msr_write(INTEL_MSR_PERFMON_CRT0, 0, ex64lo(watchdog->resetval));
}

পূর্ণসংখ্যা arch_watchdog_init(শূন্য)
{
	u32_t eax, ebx, ecx, edx;
	unsigned cpu = cpuid;

	যদি (!lapic_addr) {
		printf("ERROR : Cannot use NMI watchdog if APIC is not enabled\n");
		ফেরত -1;
	}

	যদি (cpu_info[cpu].vendor == CPU_VENDOR_INTEL) {
		eax = 0xA;

		_cpuid(&eax, &ebx, &ecx, &edx);

		/* FIXME currently we support only watchdog based on the intel
		 * architectural performance counters. Some Intel CPUs don't have this
		 * feature
		 */
		যদি (ebx & (1 << CPUID_UNHALTED_CORE_CYCLES_AVAILABLE))
			ফেরত -1;
		যদি (!((((eax >> 8)) & 0xff) > 0))
			ফেরত -1;

		watchdog = &intel_arch_watchdog;
	} অন্যথা যদি (cpu_info[cpu].vendor == CPU_VENDOR_AMD) {
		যদি (cpu_info[cpu].family != 6 &&
				cpu_info[cpu].family != 15 &&
				cpu_info[cpu].family != 16 &&
				cpu_info[cpu].family != 17)
			ফেরত -1;
		অন্যথা
			watchdog = &amd_watchdog;
	} অন্যথা
		ফেরত -1;

	/* Setup PC overflow as NMI for watchdog, it is masked for now */
	lapic_write(LAPIC_LVTPCR, APIC_ICR_INT_MASK | APIC_ICR_DM_NMI);
	(শূন্য) lapic_read(LAPIC_LVTPCR);

	/* double check if LAPIC is enabled */
	যদি (lapic_addr && watchdog->init) {
		watchdog->init(cpuid);
	}

	ফেরত 0;
}

শূন্য arch_watchdog_stop(শূন্য)
{
}

শূন্য arch_watchdog_lockup(ধ্রুবক কাঠামো nmi_frame * frame)
{
	printf("KERNEL LOCK UP\n"
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

পূর্ণসংখ্যা i386_watchdog_start(শূন্য)
{
	যদি (arch_watchdog_init()) {
		printf("WARNING watchdog initialization "
				"failed! Disabled\n");
		watchdog_enabled = 0;
		ফেরত -1;
	}
	অন্যথা
		BOOT_VERBOSE(printf("Watchdog enabled\n"););

	ফেরত 0;
}

স্থির পূর্ণসংখ্যা intel_arch_watchdog_profile_init(ধ্রুবক unsigned freq)
{
	u64_t cpuf;

	/* FIXME works only if all CPUs have the same freq */
	cpuf = cpu_get_freq(cpuid);
	cpuf /= freq;

	/*
	 * if freq is too low and the cpu freq too high we may get in a range of
	 * insane value which cannot be handled by the 31bit CPU perf counter
	 */
	যদি (ex64hi(cpuf) != 0 || ex64lo(cpuf) > 0x7fffffffU) {
		printf("ERROR : nmi watchdog ticks exceed 31bits, use higher frequency\n");
		ফেরত EINVAL;
	}

	cpuf = make64(-ex64lo(cpuf), ex64hi(cpuf));
	watchdog->profile_resetval = cpuf;

	ফেরত OK;
}

স্থির কাঠামো arch_watchdog intel_arch_watchdog = {
	/*.init = */		intel_arch_watchdog_init,
	/*.reinit = */		intel_arch_watchdog_reinit,
	/*.profile_init = */	intel_arch_watchdog_profile_init
};

#define AMD_MSR_EVENT_SEL0		0xc0010000
#define AMD_MSR_EVENT_CTR0		0xc0010004
#define AMD_MSR_EVENT_SEL0_ENABLE	(1 << 22)

স্থির শূন্য amd_watchdog_init(ধ্রুবক unsigned cpu)
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

স্থির শূন্য amd_watchdog_reinit(ধ্রুবক unsigned cpu)
{
	lapic_write(LAPIC_LVTPCR, APIC_ICR_DM_NMI);
	ia32_msr_write(AMD_MSR_EVENT_CTR0,
		       ex64hi(watchdog->resetval), ex64lo(watchdog->resetval));
}

স্থির পূর্ণসংখ্যা amd_watchdog_profile_init(ধ্রুবক unsigned freq)
{
	u64_t cpuf;

	/* FIXME works only if all CPUs have the same freq */
	cpuf = cpu_get_freq(cpuid);
	cpuf = -cpuf / freq;

	watchdog->profile_resetval = cpuf;

	ফেরত OK;
}

স্থির কাঠামো arch_watchdog amd_watchdog = {
	/*.init = */		amd_watchdog_init,
	/*.reinit = */		amd_watchdog_reinit,
	/*.profile_init = */	amd_watchdog_profile_init
};
