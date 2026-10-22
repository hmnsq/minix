/* This file contains essentially the MP handling code of the Minix kernel.
 *
 * Changes:
 *   Apr 1,  2008     Added SMP support.
 */

#define _SMP

#ಅನ್ತರ್ಭಾವ <unistd.h>
#ಅನ್ತರ್ಭಾವ <assert.h>
#ಅನ್ತರ್ಭಾವ <stdlib.h>
#ಅನ್ತರ್ಭಾವ <string.h>
#ಅನ್ತರ್ಭಾವ <machine/cmos.h>
#ಅನ್ತರ್ಭಾವ <machine/bios.h>

#ಅನ್ತರ್ಭಾವ "kernel/spinlock.h"
#ಅನ್ತರ್ಭಾವ "kernel/smp.h"
#ಅನ್ತರ್ಭಾವ "apic.h"
#ಅನ್ತರ್ಭಾವ "acpi.h"
#ಅನ್ತರ್ಭಾವ "kernel/clock.h"

#ಅನ್ತರ್ಭಾವ "glo.h"

ಶೂನ್ಯ trampoline(ಶೂನ್ಯ);

/*
 * arguments for trampoline. We need to pass the logical cpu id, gdt and idt.
 * They have to be in location which is reachable using absolute addressing in
 * 16-bit mode
 */
ಬಾಹ್ಯ ಚಞ್ಚಲ u32_t __ap_id, __ap_pt;
ಬಾಹ್ಯ ಚಞ್ಚಲ ಸಂರಚನಾ desctableptr_s __ap_gdt, __ap_idt;
ಬಾಹ್ಯ u32_t __ap_gdt_tab, __ap_idt_tab;
ಬಾಹ್ಯ ಶೂನ್ಯ * __trampoline_end;

ಬಾಹ್ಯ u32_t busclock[CONFIG_MAX_CPUS];
ಬಾಹ್ಯ ಪೂರ್ಣಾಙ್ಕ panicking;

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ ಚಞ್ಚಲ ap_cpu_ready;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ ಚಞ್ಚಲ cpu_down;

/* there can be at most 255 local APIC ids, each fits in 8 bits */
ಸ್ಥಿರ unsigned ವರ್ಣ apicid2cpuid[255];
unsigned ವರ್ಣ cpuid2apicid[CONFIG_MAX_CPUS];

SPINLOCK_DEFINE(smp_cpu_lock)
SPINLOCK_DEFINE(dispq_lock)

ಸ್ಥಿರ ಶೂನ್ಯ smp_reinit_vars(ಶೂನ್ಯ);

/* These are initialized in protect.c */
ಬಾಹ್ಯ ಸಂರಚನಾ segdesc_s gdt[GDT_SIZE];
ಬಾಹ್ಯ ಸಂರಚನಾ gatedesc_s idt[IDT_SIZE];
ಬಾಹ್ಯ ಸಂರಚನಾ tss_s tss[CONFIG_MAX_CPUS];
ಬಾಹ್ಯ ಪೂರ್ಣಾಙ್ಕ prot_init_done;	/* Indicates they are ready */

ಸ್ಥಿರ phys_bytes trampoline_base;

ಸ್ಥಿರ u32_t ap_lin_addr(ಶೂನ್ಯ *vaddr)
{
	ಪ್ರತಿಜ್ಞಾ(trampoline_base);
	ಪ್ರತ್ಯಾಗಮ (u32_t) vaddr - (u32_t) &trampoline + trampoline_base;
}

/*
 * copies the 16-bit AP trampoline code to the first 1M of memory
 */
ಶೂನ್ಯ copy_trampoline(ಶೂನ್ಯ)
{
	unsigned tramp_size, tramp_start = (unsigned)&trampoline;;

	/* The trampoline code/data is made to be page-aligned. */
	ಪ್ರತಿಜ್ಞಾ(!(tramp_start % I386_PAGE_SIZE));

	tramp_size = (unsigned) &__trampoline_end - tramp_start;
	trampoline_base = alloc_lowest(&kinfo, tramp_size);

	/* The memory allocator finds the lowest available memory.. 
	 * Verify it's low enough
	 */
	ಪ್ರತಿಜ್ಞಾ(trampoline_base + tramp_size < (1 << 20));

	/* prepare gdt and idt for the new cpus; make copies
	 * of both the tables and the descriptors of them
	 * in their boot addressing environment.
	 */
	ಪ್ರತಿಜ್ಞಾ(prot_init_done);
	memcpy(&__ap_gdt_tab, gdt, ಪರಿಮಾಣ(gdt));
	memcpy(&__ap_idt_tab, gdt, ಪರಿಮಾಣ(idt));
	__ap_gdt.base = ap_lin_addr(&__ap_gdt_tab);
	__ap_gdt.limit = ಪರಿಮಾಣ(gdt)-1;
	__ap_idt.base = ap_lin_addr(&__ap_idt_tab);
	__ap_idt.limit = ಪರಿಮಾಣ(idt)-1;

	phys_copy((phys_bytes) trampoline, trampoline_base, tramp_size);
}

ಬಾಹ್ಯ ಪೂರ್ಣಾಙ್ಕ booting_cpu;	/* tell protect.c what to do */

ಸ್ಥಿರ ಶೂನ್ಯ smp_start_aps(ಶೂನ್ಯ)
{
	unsigned cpu;
	u32_t biosresetvector;
	phys_bytes __ap_id_phys;
	ಸಂರಚನಾ proc *bootstrap_pt = get_cpulocal_var(ptproc);

	/* TODO hack around the alignment problem */

	phys_copy(0x467, (phys_bytes) &biosresetvector, ಪರಿಮಾಣ(u32_t));

	/* set the bios shutdown code to 0xA */
	outb(RTC_INDEX, 0xF);
	outb(RTC_IO, 0xA);

	ಪ್ರತಿಜ್ಞಾ(bootstrap_pt);
	ಪ್ರತಿಜ್ಞಾ(bootstrap_pt->p_seg.p_cr3);
	__ap_pt  = bootstrap_pt->p_seg.p_cr3;
	ಪ್ರತಿಜ್ಞಾ(__ap_pt);

	copy_trampoline();

	/* New locations for cpu id, pagetable root */
	__ap_id_phys = trampoline_base +
		(phys_bytes) &__ap_id - (phys_bytes)&trampoline;

	/* setup the warm reset vector */
	phys_copy((phys_bytes) &trampoline_base, 0x467, ಪರಿಮಾಣ(u32_t));

	/* okay, we're ready to go.  boot all of the ap's now.  we loop through
	 * using the processor's apic id values.
	 */
	ಪ್ರತಿ (cpu = 0; cpu < ncpus; cpu++) {
		ap_cpu_ready = -1;
		/* Don't send INIT/SIPI to boot cpu.  */
		ಯದಿ((apicid() == cpuid2apicid[cpu]) && 
				(apicid() == bsp_lapic_id)) {
			ಅನುವರ್ತ;
		}

		__ap_id = booting_cpu = cpu;
		phys_copy((phys_bytes) &__ap_id, __ap_id_phys, ಪರಿಮಾಣ(__ap_id));
		mfence();
		ಯದಿ (apic_send_init_ipi(cpu, trampoline_base) ||
				apic_send_startup_ipi(cpu, trampoline_base)) {
			printf("WARNING cannot boot cpu %d\n", cpu);
			ಅನುವರ್ತ;
		}

		/* wait for 5 secs for the processors to boot */
		lapic_set_timer_one_shot(5000000); 

		ಯಾವತ್ (lapic_read(LAPIC_TIMER_CCR)) {
			ಯದಿ (ap_cpu_ready == cpu) {
				cpu_set_flag(cpu, CPU_IS_READY);
				ಭಙ್ಗ;
			}
		}
		ಯದಿ (ap_cpu_ready == -1) {
			printf("WARNING : CPU %d didn't boot\n", cpu);
		}
	}

	phys_copy((phys_bytes) &biosresetvector, 0x467, ಪರಿಮಾಣ(u32_t));

	outb(RTC_INDEX, 0xF);
	outb(RTC_IO, 0);

	bsp_finish_booting();
	NOT_REACHABLE;
}

ಶೂನ್ಯ smp_halt_cpu (ಶೂನ್ಯ) 
{
	NOT_IMPLEMENTED;
}

ಶೂನ್ಯ smp_shutdown_aps(ಶೂನ್ಯ)
{
	unsigned cpu;

	ಯದಿ (ncpus == 1)
		ಗಚ್ಛ exit_shutdown_aps;

	/* we must let the other cpus enter the kernel mode */
	BKL_UNLOCK();

	ಪ್ರತಿ (cpu = 0; cpu < ncpus; cpu++) {
		ಯದಿ (cpu == cpuid)
			ಅನುವರ್ತ;
		ಯದಿ (!cpu_test_flag(cpu, CPU_IS_READY)) {
			printf("CPU %d didn't boot\n", cpu);
			ಅನುವರ್ತ;
		}

		cpu_down = -1;
		barrier();
		apic_send_ipi(APIC_SMP_CPU_HALT_VECTOR, cpu, APIC_IPI_DEST);
		/* wait for the cpu to be down */
		ಯಾವತ್ (cpu_down != cpu);
		printf("CPU %d is down\n", cpu);
		cpu_clear_flag(cpu, CPU_IS_READY);
	}

exit_shutdown_aps:
	ioapic_disable_all();

	lapic_disable();

	ncpus = 1; /* hopefully !!! */
	lapic_addr = lapic_eoi_addr = 0;
	ಪ್ರತ್ಯಾಗಮ;
}

ಸ್ಥಿರ ಶೂನ್ಯ ap_finish_booting(ಶೂನ್ಯ)
{
	unsigned cpu = cpuid;

	/* inform the world of our presence. */
	ap_cpu_ready = cpu;

	/*
	 * Finish processor initialisation.  CPUs must be excluded from running.
	 * lapic timer calibration locks and unlocks the BKL because of the
	 * nested interrupts used for calibration. Therefore BKL is not good
	 * enough, the boot_lock must be held.
	 */
	spinlock_lock(&boot_lock);
	BKL_LOCK();

	printf("CPU %d is up\n", cpu);

	cpu_identify();

	lapic_enable(cpu);
	fpu_init();

	ಯದಿ (app_cpu_init_timer(system_hz)) {
		panic("FATAL : failed to initialize timer interrupts CPU %d, "
				"cannot continue without any clock source!", cpu);
	}

	/* FIXME assign CPU local idle structure */
	get_cpulocal_var(proc_ptr) = get_cpulocal_var_ptr(idle_proc);
	get_cpulocal_var(bill_ptr) = get_cpulocal_var_ptr(idle_proc);

	ap_boot_finished(cpu);
	spinlock_unlock(&boot_lock);

	switch_to_user();
	NOT_REACHABLE;
}

ಶೂನ್ಯ smp_ap_boot(ಶೂನ್ಯ)
{
	switch_k_ರಾಶಿ((ವರ್ಣ *)get_k_ರಾಶಿ_ತುದಿ(__ap_id) -
			X86_ರಾಶಿ_ತುದಿ_RESERVED, ap_finish_booting);
}

ಸ್ಥಿರ ಶೂನ್ಯ smp_reinit_vars(ಶೂನ್ಯ)
{
	lapic_addr = lapic_eoi_addr = 0;
	ioapic_enabled = 0;

	ncpus = 1;
}

ಸ್ಥಿರ ಶೂನ್ಯ tss_init_all(ಶೂನ್ಯ)
{
	unsigned cpu;

	ಪ್ರತಿ(cpu = 0; cpu < ncpus ; cpu++)
		tss_init(cpu, get_k_ರಾಶಿ_ತುದಿ(cpu)); 
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ discover_cpus(ಶೂನ್ಯ)
{
	ಸಂರಚನಾ acpi_madt_lapic * cpu;

	ಯಾವತ್ (ncpus < CONFIG_MAX_CPUS && (cpu = acpi_get_lapic_next())) {
		apicid2cpuid[cpu->apic_id] = ncpus;
		cpuid2apicid[ncpus] = cpu->apic_id;
		printf("CPU %3d local APIC id %3d\n", ncpus, cpu->apic_id);
		ncpus++;
	}

	ಪ್ರತ್ಯಾಗಮ ncpus;
}

ಶೂನ್ಯ smp_init (ಶೂನ್ಯ)
{
	/* read the MP configuration */
	ಯದಿ (!discover_cpus()) {
		ncpus = 1;
		ಗಚ್ಛ uniproc_fallback;
	}

	lapic_addr = LOCAL_APIC_DEF_ADDR;
	ioapic_enabled = 0;

	tss_init_all();

	/* 
	 * we still run on the boot stack and we cannot use cpuid as its value
	 * wasn't set yet. apicid2cpuid initialized in mps_init()
	 */
	bsp_cpu_id = apicid2cpuid[apicid()];

	ಯದಿ (!lapic_enable(bsp_cpu_id)) {
		printf("ERROR : failed to initialize BSP Local APIC\n");
		ಗಚ್ಛ uniproc_fallback;
	}

	bsp_lapic_id = apicid();
	
	acpi_init();

	ಯದಿ (!detect_ioapics()) {
		lapic_disable();
		lapic_addr = 0x0;
		ಗಚ್ಛ uniproc_fallback;
	}

	ioapic_enable_all();
	
	ಯದಿ (ioapic_enabled)
		machine.apic_enabled = 1;

	/* set smp idt entries. */ 
	apic_idt_init(0); /* Not a reset ! */
	idt_reload();

	BOOT_VERBOSE(printf("SMP initialized\n"));

	switch_k_ರಾಶಿ((ವರ್ಣ *)get_k_ರಾಶಿ_ತುದಿ(bsp_cpu_id) -
			X86_ರಾಶಿ_ತುದಿ_RESERVED, smp_start_aps);
	
	ಪ್ರತ್ಯಾಗಮ;

uniproc_fallback:
	apic_idt_init(1); /* Reset to PIC idt ! */
	idt_reload();
	smp_reinit_vars (); /* revert to a single proc system. */
	intr_init(0); /* no auto eoi */
	printf("WARNING : SMP initialization failed\n");
}
	
ಶೂನ್ಯ arch_smp_halt_cpu(ಶೂನ್ಯ)
{
	/* say that we are down */
	cpu_down = cpuid;
	barrier();
	/* unlock the BKL and don't continue */
	BKL_UNLOCK();
	ಪ್ರತಿ(;;);
}

ಶೂನ್ಯ arch_send_smp_schedule_ipi(unsigned cpu)
{
	apic_send_ipi(APIC_SMP_SCHED_PROC_VECTOR, cpu, APIC_IPI_DEST);
}
