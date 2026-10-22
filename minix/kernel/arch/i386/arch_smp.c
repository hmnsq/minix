/* This file contains essentially the MP handling code of the Minix kernel.
 *
 * Changes:
 *   Apr 1,  2008     Added SMP support.
 */

#నిర్వచించు _SMP

#అన్తర్భావ <unistd.h>
#అన్తర్భావ <assert.h>
#అన్తర్భావ <stdlib.h>
#అన్తర్భావ <string.h>
#అన్తర్భావ <machine/cmos.h>
#అన్తర్భావ <machine/bios.h>

#అన్తర్భావ "kernel/spinlock.h"
#అన్తర్భావ "kernel/smp.h"
#అన్తర్భావ "apic.h"
#అన్తర్భావ "acpi.h"
#అన్తర్భావ "kernel/clock.h"

#అన్తర్భావ "glo.h"

శూన్య trampoline(శూన్య);

/*
 * arguments for trampoline. We need to pass the logical cpu id, gdt and idt.
 * They have to be in location which is reachable using absolute addressing in
 * 16-bit mode
 */
బాహ్య అస్థిర u32_t __ap_id, __ap_pt;
బాహ్య అస్థిర నిర్మాణం desctableptr_s __ap_gdt, __ap_idt;
బాహ్య u32_t __ap_gdt_tab, __ap_idt_tab;
బాహ్య శూన్య * __trampoline_end;

బాహ్య u32_t busclock[CONFIG_MAX_CPUS];
బాహ్య పూర్ణాంకం panicking;

స్థిర పూర్ణాంకం అస్థిర ap_cpu_ready;
స్థిర పూర్ణాంకం అస్థిర cpu_కిందికి;

/* there can be at most 255 local APIC ids, each fits in 8 bits */
స్థిర గుర్తులేని అక్షరం apicid2cpuid[255];
గుర్తులేని అక్షరం cpuid2apicid[CONFIG_MAX_CPUS];

SPINLOCK_DEFINE(smp_cpu_తాళం)
SPINLOCK_DEFINE(dispq_తాళం)

స్థిర శూన్య smp_reinit_vars(శూన్య);

/* These are initialized in protect.c */
బాహ్య నిర్మాణం segdesc_s gdt[GDT_కొలత];
బాహ్య నిర్మాణం gatedesc_s idt[IDT_కొలత];
బాహ్య నిర్మాణం tss_s tss[CONFIG_MAX_CPUS];
బాహ్య పూర్ణాంకం prot_init_పూర్తి;	/* Indicates they are ready */

స్థిర phys_బైట్లు trampoline_పునాది;

స్థిర u32_t ap_lin_addr(శూన్య *vaddr)
{
	ప్రతిజ్ఞా(trampoline_పునాది);
	తిరిగి (u32_t) vaddr - (u32_t) &trampoline + trampoline_పునాది;
}

/*
 * copies the 16-bit AP trampoline code to the first 1M of memory
 */
శూన్య copy_trampoline(శూన్య)
{
	గుర్తులేని tramp_కొలత, tramp_ప్రారంభం = (గుర్తులేని)&trampoline;;

	/* The trampoline code/data is made to be page-aligned. */
	ప్రతిజ్ఞా(!(tramp_ప్రారంభం % I386_పుట_కొలత));

	tramp_కొలత = (గుర్తులేని) &__trampoline_end - tramp_ప్రారంభం;
	trampoline_పునాది = alloc_అత్యల్ప(&kinfo, tramp_కొలత);

	/* The memory allocator finds the lowest available memory.. 
	 * Verify it's low enough
	 */
	ప్రతిజ్ఞా(trampoline_పునాది + tramp_కొలత < (1 << 20));

	/* prepare gdt and idt for the new cpus; make copies
	 * of both the tables and the descriptors of them
	 * in their boot addressing environment.
	 */
	ప్రతిజ్ఞా(prot_init_పూర్తి);
	జ్ఞాపకం_నకలు(&__ap_gdt_tab, gdt, పరిమాణం(gdt));
	జ్ఞాపకం_నకలు(&__ap_idt_tab, gdt, పరిమాణం(idt));
	__ap_gdt.పునాది = ap_lin_addr(&__ap_gdt_tab);
	__ap_gdt.హద్దు_2 = పరిమాణం(gdt)-1;
	__ap_idt.పునాది = ap_lin_addr(&__ap_idt_tab);
	__ap_idt.హద్దు_2 = పరిమాణం(idt)-1;

	phys_copy((phys_బైట్లు) trampoline, trampoline_పునాది, tramp_కొలత);
}

బాహ్య పూర్ణాంకం booting_cpu;	/* tell protect.c what to do */

స్థిర శూన్య smp_ప్రారంభం_aps(శూన్య)
{
	గుర్తులేని cpu;
	u32_t biosresetvector;
	phys_బైట్లు __ap_id_phys;
	నిర్మాణం proc *bootstrap_pt = get_cpulocal_var(ptproc);

	/* TODO hack around the alignment problem */

	phys_copy(0x467, (phys_బైట్లు) &biosresetvector, పరిమాణం(u32_t));

	/* set the bios shutdown code to 0xA */
	outb(RTC_సూచీ, 0xF);
	outb(RTC_IO, 0xA);

	ప్రతిజ్ఞా(bootstrap_pt);
	ప్రతిజ్ఞా(bootstrap_pt->p_seg.p_cr3);
	__ap_pt  = bootstrap_pt->p_seg.p_cr3;
	ప్రతిజ్ఞా(__ap_pt);

	copy_trampoline();

	/* New locations for cpu id, pagetable root */
	__ap_id_phys = trampoline_పునాది +
		(phys_బైట్లు) &__ap_id - (phys_బైట్లు)&trampoline;

	/* setup the warm reset vector */
	phys_copy((phys_బైట్లు) &trampoline_పునాది, 0x467, పరిమాణం(u32_t));

	/* okay, we're ready to go.  boot all of the ap's now.  we loop through
	 * using the processor's apic id values.
	 */
	కొరకు (cpu = 0; cpu < ncpus; cpu++) {
		ap_cpu_ready = -1;
		/* Don't send INIT/SIPI to boot cpu.  */
		ఒకవేళ((apicid() == cpuid2apicid[cpu]) && 
				(apicid() == bsp_lapic_id)) {
			కొనసాగు;
		}

		__ap_id = booting_cpu = cpu;
		phys_copy((phys_బైట్లు) &__ap_id, __ap_id_phys, పరిమాణం(__ap_id));
		mfence();
		ఒకవేళ (apic_send_init_ipi(cpu, trampoline_పునాది) ||
				apic_send_startup_ipi(cpu, trampoline_పునాది)) {
			రాయి("WARNING cannot boot cpu %d\n", cpu);
			కొనసాగు;
		}

		/* wait for 5 secs for the processors to boot */
		lapic_సమితి_timer_one_shot(5000000); 

		అయితే (lapic_read(LAPIC_TIMER_CCR)) {
			ఒకవేళ (ap_cpu_ready == cpu) {
				cpu_సమితి_flag(cpu, CPU_IS_READY);
				ఆపు;
			}
		}
		ఒకవేళ (ap_cpu_ready == -1) {
			రాయి("WARNING : CPU %d didn't boot\n", cpu);
		}
	}

	phys_copy((phys_బైట్లు) &biosresetvector, 0x467, పరిమాణం(u32_t));

	outb(RTC_సూచీ, 0xF);
	outb(RTC_IO, 0);

	bsp_ముగింపు_booting();
	NOT_REACHABLE;
}

శూన్య smp_halt_cpu (శూన్య) 
{
	NOT_IMPLEMENTED;
}

శూన్య smp_shutdown_aps(శూన్య)
{
	గుర్తులేని cpu;

	ఒకవేళ (ncpus == 1)
		వెళ్ళు exit_shutdown_aps;

	/* we must let the other cpus enter the kernel mode */
	BKL_UNLOCK();

	కొరకు (cpu = 0; cpu < ncpus; cpu++) {
		ఒకవేళ (cpu == cpuid)
			కొనసాగు;
		ఒకవేళ (!cpu_test_flag(cpu, CPU_IS_READY)) {
			రాయి("CPU %d didn't boot\n", cpu);
			కొనసాగు;
		}

		cpu_కిందికి = -1;
		barrier();
		apic_send_ipi(APIC_SMP_CPU_HALT_సదిశ, cpu, APIC_IPI_DEST);
		/* wait for the cpu to be down */
		అయితే (cpu_కిందికి != cpu);
		రాయి("CPU %d is down\n", cpu);
		cpu_clear_flag(cpu, CPU_IS_READY);
	}

exit_shutdown_aps:
	ioapic_disable_all();

	lapic_disable();

	ncpus = 1; /* hopefully !!! */
	lapic_addr = lapic_eoi_addr = 0;
	తిరిగి;
}

స్థిర శూన్య ap_ముగింపు_booting(శూన్య)
{
	గుర్తులేని cpu = cpuid;

	/* inform the world of our presence. */
	ap_cpu_ready = cpu;

	/*
	 * Finish processor initialisation.  CPUs must be excluded from running.
	 * lapic timer calibration locks and unlocks the BKL because of the
	 * nested interrupts used for calibration. Therefore BKL is not good
	 * enough, the boot_lock must be held.
	 */
	spinlock_తాళం(&boot_తాళం);
	BKL_తాళం();

	రాయి("CPU %d is up\n", cpu);

	cpu_identify();

	lapic_enable(cpu);
	fpu_init();

	ఒకవేళ (app_cpu_init_timer(system_hz)) {
		panic("FATAL : failed to initialize timer interrupts CPU %d, "
				"cannot continue without any clock source!", cpu);
	}

	/* FIXME assign CPU local idle structure */
	get_cpulocal_var(proc_ptr) = get_cpulocal_var_ptr(idle_proc);
	get_cpulocal_var(bill_ptr) = get_cpulocal_var_ptr(idle_proc);

	ap_boot_ముగిసిన(cpu);
	spinlock_unlock(&boot_తాళం);

	switch_గమ్యం_user();
	NOT_REACHABLE;
}

శూన్య smp_ap_boot(శూన్య)
{
	switch_k_దొంతర((అక్షరం *)get_k_దొంతర_శిఖరం(__ap_id) -
			X86_దొంతర_శిఖరం_RESERVED, ap_ముగింపు_booting);
}

స్థిర శూన్య smp_reinit_vars(శూన్య)
{
	lapic_addr = lapic_eoi_addr = 0;
	ioapic_enabled = 0;

	ncpus = 1;
}

స్థిర శూన్య tss_init_all(శూన్య)
{
	గుర్తులేని cpu;

	కొరకు(cpu = 0; cpu < ncpus ; cpu++)
		tss_init(cpu, get_k_దొంతర_శిఖరం(cpu)); 
}

స్థిర పూర్ణాంకం discover_cpus(శూన్య)
{
	నిర్మాణం acpi_madt_lapic * cpu;

	అయితే (ncpus < CONFIG_MAX_CPUS && (cpu = acpi_get_lapic_తదుపరి())) {
		apicid2cpuid[cpu->apic_id] = ncpus;
		cpuid2apicid[ncpus] = cpu->apic_id;
		రాయి("CPU %3d local APIC id %3d\n", ncpus, cpu->apic_id);
		ncpus++;
	}

	తిరిగి ncpus;
}

శూన్య smp_init (శూన్య)
{
	/* read the MP configuration */
	ఒకవేళ (!discover_cpus()) {
		ncpus = 1;
		వెళ్ళు uniproc_fallback;
	}

	lapic_addr = LOCAL_APIC_DEF_ADDR;
	ioapic_enabled = 0;

	tss_init_all();

	/* 
	 * we still run on the boot stack and we cannot use cpuid as its value
	 * wasn't set yet. apicid2cpuid initialized in mps_init()
	 */
	bsp_cpu_id = apicid2cpuid[apicid()];

	ఒకవేళ (!lapic_enable(bsp_cpu_id)) {
		రాయి("ERROR : failed to initialize BSP Local APIC\n");
		వెళ్ళు uniproc_fallback;
	}

	bsp_lapic_id = apicid();
	
	acpi_init();

	ఒకవేళ (!detect_ioapics()) {
		lapic_disable();
		lapic_addr = 0x0;
		వెళ్ళు uniproc_fallback;
	}

	ioapic_enable_all();
	
	ఒకవేళ (ioapic_enabled)
		machine.apic_enabled = 1;

	/* set smp idt entries. */ 
	apic_idt_init(0); /* Not a reset ! */
	idt_reload();

	BOOT_VERBOSE(రాయి("SMP initialized\n"));

	switch_k_దొంతర((అక్షరం *)get_k_దొంతర_శిఖరం(bsp_cpu_id) -
			X86_దొంతర_శిఖరం_RESERVED, smp_ప్రారంభం_aps);
	
	తిరిగి;

uniproc_fallback:
	apic_idt_init(1); /* Reset to PIC idt ! */
	idt_reload();
	smp_reinit_vars (); /* revert to a single proc system. */
	intr_init(0); /* no auto eoi */
	రాయి("WARNING : SMP initialization failed\n");
}
	
శూన్య arch_smp_halt_cpu(శూన్య)
{
	/* say that we are down */
	cpu_కిందికి = cpuid;
	barrier();
	/* unlock the BKL and don't continue */
	BKL_UNLOCK();
	కొరకు(;;);
}

శూన్య arch_send_smp_కాలపట్టిక_ipi(గుర్తులేని cpu)
{
	apic_send_ipi(APIC_SMP_SCHED_PROC_సదిశ, cpu, APIC_IPI_DEST);
}
