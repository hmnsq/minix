#అన్తర్భావ "kernel/watchdog.h"
#అన్తర్భావ "glo.h"
#అన్తర్భావ <minix/minlib.h>
#అన్తర్భావ <minix/u64.h>

#అన్తర్భావ "apic.h"

#నిర్వచించు CPUID_UNHALTED_CORE_చక్రాలు_అందుబాటు	0

/*
 * Intel architecture performance counters watchdog
 */

స్థిర నిర్మాణం arch_watchdog intel_arch_watchdog;
స్థిర నిర్మాణం arch_watchdog amd_watchdog;

స్థిర శూన్య intel_arch_watchdog_init(స్థిరరాశి గుర్తులేని cpu)
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
	అయితే (ex64hi(cpuf) || ex64lo(cpuf) > 0x7fffffffU)
		cpuf /= 2;
	cpuf = make64(-ex64lo(cpuf), ex64hi(cpuf));
	watchdog->resetval = watchdog->watchdog_resetval = cpuf;

	ia32_msr_write(INTEL_MSR_PERFMON_CRT0, 0, ex64lo(cpuf));

	ia32_msr_write(INTEL_MSR_PERFMON_SEL0, 0,
			val | INTEL_MSR_PERFMON_SEL0_ENABLE);

	/* unmask the performance counter interrupt */
	lapic_write(LAPIC_LVTPCR, APIC_ICR_DM_NMI);
}

స్థిర శూన్య intel_arch_watchdog_reinit(స్థిరరాశి గుర్తులేని cpu)
{
	lapic_write(LAPIC_LVTPCR, APIC_ICR_DM_NMI);
	ia32_msr_write(INTEL_MSR_PERFMON_CRT0, 0, ex64lo(watchdog->resetval));
}

పూర్ణాంకం arch_watchdog_init(శూన్య)
{
	u32_t eax, ebx, ecx, edx;
	గుర్తులేని cpu = cpuid;

	ఒకవేళ (!lapic_addr) {
		రాయి("ERROR : Cannot use NMI watchdog if APIC is not enabled\n");
		తిరిగి -1;
	}

	ఒకవేళ (cpu_info[cpu].vendor == CPU_VENDOR_INTEL) {
		eax = 0xA;

		_cpuid(&eax, &ebx, &ecx, &edx);

		/* FIXME currently we support only watchdog based on the intel
		 * architectural performance counters. Some Intel CPUs don't have this
		 * feature
		 */
		ఒకవేళ (ebx & (1 << CPUID_UNHALTED_CORE_చక్రాలు_అందుబాటు))
			తిరిగి -1;
		ఒకవేళ (!((((eax >> 8)) & 0xff) > 0))
			తిరిగి -1;

		watchdog = &intel_arch_watchdog;
	} లేకపోతే ఒకవేళ (cpu_info[cpu].vendor == CPU_VENDOR_AMD) {
		ఒకవేళ (cpu_info[cpu].family != 6 &&
				cpu_info[cpu].family != 15 &&
				cpu_info[cpu].family != 16 &&
				cpu_info[cpu].family != 17)
			తిరిగి -1;
		లేకపోతే
			watchdog = &amd_watchdog;
	} లేకపోతే
		తిరిగి -1;

	/* Setup PC overflow as NMI for watchdog, it is masked for now */
	lapic_write(LAPIC_LVTPCR, APIC_ICR_INT_ముసుగు | APIC_ICR_DM_NMI);
	(శూన్య) lapic_read(LAPIC_LVTPCR);

	/* double check if LAPIC is enabled */
	ఒకవేళ (lapic_addr && watchdog->init) {
		watchdog->init(cpuid);
	}

	తిరిగి 0;
}

శూన్య arch_watchdog_stop(శూన్య)
{
}

శూన్య arch_watchdog_lockup(స్థిరరాశి నిర్మాణం nmi_చట్రం * చట్రం)
{
	రాయి("KERNEL LOCK UP\n"
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
			చట్రం->eax,
			చట్రం->ecx,
			చట్రం->edx,
			చట్రం->ebx,
			చట్రం->ebp,
			చట్రం->esi,
			చట్రం->edi,
			చట్రం->gs,
			చట్రం->fs,
			చట్రం->es,
			చట్రం->ds,
			చట్రం->pc,
			చట్రం->cs,
			చట్రం->eflags
			);
	panic("Kernel lockup");
}

పూర్ణాంకం i386_watchdog_ప్రారంభం(శూన్య)
{
	ఒకవేళ (arch_watchdog_init()) {
		రాయి("WARNING watchdog initialization "
				"failed! Disabled\n");
		watchdog_enabled = 0;
		తిరిగి -1;
	}
	లేకపోతే
		BOOT_VERBOSE(రాయి("Watchdog enabled\n"););

	తిరిగి 0;
}

స్థిర పూర్ణాంకం intel_arch_watchdog_profile_init(స్థిరరాశి గుర్తులేని freq)
{
	u64_t cpuf;

	/* FIXME works only if all CPUs have the same freq */
	cpuf = cpu_get_freq(cpuid);
	cpuf /= freq;

	/*
	 * if freq is too low and the cpu freq too high we may get in a range of
	 * insane value which cannot be handled by the 31bit CPU perf counter
	 */
	ఒకవేళ (ex64hi(cpuf) != 0 || ex64lo(cpuf) > 0x7fffffffU) {
		రాయి("ERROR : nmi watchdog ticks exceed 31bits, use higher frequency\n");
		తిరిగి EINVAL;
	}

	cpuf = make64(-ex64lo(cpuf), ex64hi(cpuf));
	watchdog->profile_resetval = cpuf;

	తిరిగి OK;
}

స్థిర నిర్మాణం arch_watchdog intel_arch_watchdog = {
	/*.init = */		intel_arch_watchdog_init,
	/*.reinit = */		intel_arch_watchdog_reinit,
	/*.profile_init = */	intel_arch_watchdog_profile_init
};

#నిర్వచించు AMD_MSR_EVENT_SEL0		0xc0010000
#నిర్వచించు AMD_MSR_EVENT_CTR0		0xc0010004
#నిర్వచించు AMD_MSR_EVENT_SEL0_ENABLE	(1 << 22)

స్థిర శూన్య amd_watchdog_init(స్థిరరాశి గుర్తులేని cpu)
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

స్థిర శూన్య amd_watchdog_reinit(స్థిరరాశి గుర్తులేని cpu)
{
	lapic_write(LAPIC_LVTPCR, APIC_ICR_DM_NMI);
	ia32_msr_write(AMD_MSR_EVENT_CTR0,
		       ex64hi(watchdog->resetval), ex64lo(watchdog->resetval));
}

స్థిర పూర్ణాంకం amd_watchdog_profile_init(స్థిరరాశి గుర్తులేని freq)
{
	u64_t cpuf;

	/* FIXME works only if all CPUs have the same freq */
	cpuf = cpu_get_freq(cpuid);
	cpuf = -cpuf / freq;

	watchdog->profile_resetval = cpuf;

	తిరిగి OK;
}

స్థిర నిర్మాణం arch_watchdog amd_watchdog = {
	/*.init = */		amd_watchdog_init,
	/*.reinit = */		amd_watchdog_reinit,
	/*.profile_init = */	amd_watchdog_profile_init
};
