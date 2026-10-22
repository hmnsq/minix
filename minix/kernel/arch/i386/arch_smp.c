/* This file contains essentially the MP handling code of the Minix kernel.
 *
 * Changes:
 *   Apr 1,  2008     Added SMP support.
 */

#ವ್ಯಾಖ್ಯಾನಿಸು _SMP

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
ಬಾಹ್ಯ ಅಸ್ಥಿರ u32_t __ap_id, __ap_pt;
ಬಾಹ್ಯ ಅಸ್ಥಿರ ರಚನೆ desctableptr_s __ap_gdt, __ap_idt;
ಬಾಹ್ಯ u32_t __ap_gdt_tab, __ap_idt_tab;
ಬಾಹ್ಯ ಶೂನ್ಯ * __trampoline_end;

ಬಾಹ್ಯ u32_t busclock[CONFIG_MAX_CPUS];
ಬಾಹ್ಯ ಪೂರ್ಣಾಂಕ panicking;

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಅಸ್ಥಿರ ap_cpu_ready;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಅಸ್ಥಿರ cpu_ಕೆಳಕ್ಕೆ;

/* there can be at most 255 local APIC ids, each fits in 8 bits */
ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ apicid2cpuid[255];
ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ cpuid2apicid[CONFIG_MAX_CPUS];

SPINLOCK_DEFINE(smp_cpu_ಬೀಗ)
SPINLOCK_DEFINE(dispq_ಬೀಗ)

ಸ್ಥಿರ ಶೂನ್ಯ smp_reinit_vars(ಶೂನ್ಯ);

/* These are initialized in protect.c */
ಬಾಹ್ಯ ರಚನೆ segdesc_s gdt[GDT_ಗಾತ್ರ];
ಬಾಹ್ಯ ರಚನೆ gatedesc_s idt[IDT_ಗಾತ್ರ];
ಬಾಹ್ಯ ರಚನೆ tss_s tss[CONFIG_MAX_CPUS];
ಬಾಹ್ಯ ಪೂರ್ಣಾಂಕ prot_init_ಮುಗಿದ;	/* Indicates they are ready */

ಸ್ಥಿರ phys_ಬೈಟುಗಳು trampoline_ತಳ;

ಸ್ಥಿರ u32_t ap_lin_addr(ಶೂನ್ಯ *vaddr)
{
	ಪ್ರತಿಜ್ಞಾ(trampoline_ತಳ);
	ಹಿಂತಿರುಗಿ (u32_t) vaddr - (u32_t) &trampoline + trampoline_ತಳ;
}

/*
 * copies the 16-bit AP trampoline code to the first 1M of memory
 */
ಶೂನ್ಯ copy_trampoline(ಶೂನ್ಯ)
{
	ಚಿಹ್ನೆರಹಿತ tramp_ಗಾತ್ರ, tramp_ಆರಂಭ = (ಚಿಹ್ನೆರಹಿತ)&trampoline;;

	/* The trampoline code/data is made to be page-aligned. */
	ಪ್ರತಿಜ್ಞಾ(!(tramp_ಆರಂಭ % I386_ಪುಟ_ಗಾತ್ರ));

	tramp_ಗಾತ್ರ = (ಚಿಹ್ನೆರಹಿತ) &__trampoline_end - tramp_ಆರಂಭ;
	trampoline_ತಳ = alloc_ಅತಿಕಡಿಮೆ(&kinfo, tramp_ಗಾತ್ರ);

	/* The memory allocator finds the lowest available memory.. 
	 * Verify it's low enough
	 */
	ಪ್ರತಿಜ್ಞಾ(trampoline_ತಳ + tramp_ಗಾತ್ರ < (1 << 20));

	/* prepare gdt and idt for the new cpus; make copies
	 * of both the tables and the descriptors of them
	 * in their boot addressing environment.
	 */
	ಪ್ರತಿಜ್ಞಾ(prot_init_ಮುಗಿದ);
	ಸ್ಮರಣೆ_ನಕಲು(&__ap_gdt_tab, gdt, ಗಾತ್ರ(gdt));
	ಸ್ಮರಣೆ_ನಕಲು(&__ap_idt_tab, gdt, ಗಾತ್ರ(idt));
	__ap_gdt.ತಳ = ap_lin_addr(&__ap_gdt_tab);
	__ap_gdt.ಎಲ್ಲೆ_2 = ಗಾತ್ರ(gdt)-1;
	__ap_idt.ತಳ = ap_lin_addr(&__ap_idt_tab);
	__ap_idt.ಎಲ್ಲೆ_2 = ಗಾತ್ರ(idt)-1;

	phys_copy((phys_ಬೈಟುಗಳು) trampoline, trampoline_ತಳ, tramp_ಗಾತ್ರ);
}

ಬಾಹ್ಯ ಪೂರ್ಣಾಂಕ booting_cpu;	/* tell protect.c what to do */

ಸ್ಥಿರ ಶೂನ್ಯ smp_ಆರಂಭ_aps(ಶೂನ್ಯ)
{
	ಚಿಹ್ನೆರಹಿತ cpu;
	u32_t biosresetvector;
	phys_ಬೈಟುಗಳು __ap_id_phys;
	ರಚನೆ proc *bootstrap_pt = get_cpulocal_var(ptproc);

	/* TODO hack around the alignment problem */

	phys_copy(0x467, (phys_ಬೈಟುಗಳು) &biosresetvector, ಗಾತ್ರ(u32_t));

	/* set the bios shutdown code to 0xA */
	outb(RTC_ಸೂಚ್ಯಂಕ, 0xF);
	outb(RTC_IO, 0xA);

	ಪ್ರತಿಜ್ಞಾ(bootstrap_pt);
	ಪ್ರತಿಜ್ಞಾ(bootstrap_pt->p_seg.p_cr3);
	__ap_pt  = bootstrap_pt->p_seg.p_cr3;
	ಪ್ರತಿಜ್ಞಾ(__ap_pt);

	copy_trampoline();

	/* New locations for cpu id, pagetable root */
	__ap_id_phys = trampoline_ತಳ +
		(phys_ಬೈಟುಗಳು) &__ap_id - (phys_ಬೈಟುಗಳು)&trampoline;

	/* setup the warm reset vector */
	phys_copy((phys_ಬೈಟುಗಳು) &trampoline_ತಳ, 0x467, ಗಾತ್ರ(u32_t));

	/* okay, we're ready to go.  boot all of the ap's now.  we loop through
	 * using the processor's apic id values.
	 */
	ಗಾಗಿ (cpu = 0; cpu < ncpus; cpu++) {
		ap_cpu_ready = -1;
		/* Don't send INIT/SIPI to boot cpu.  */
		ಆದರೆ((apicid() == cpuid2apicid[cpu]) && 
				(apicid() == bsp_lapic_id)) {
			ಮುಂದುವರಿ;
		}

		__ap_id = booting_cpu = cpu;
		phys_copy((phys_ಬೈಟುಗಳು) &__ap_id, __ap_id_phys, ಗಾತ್ರ(__ap_id));
		mfence();
		ಆದರೆ (apic_send_init_ipi(cpu, trampoline_ತಳ) ||
				apic_send_startup_ipi(cpu, trampoline_ತಳ)) {
			ಬರೆ("WARNING cannot boot cpu %d\n", cpu);
			ಮುಂದುವರಿ;
		}

		/* wait for 5 secs for the processors to boot */
		lapic_ಗಣ_timer_one_shot(5000000); 

		ಆಗ (lapic_read(LAPIC_TIMER_CCR)) {
			ಆದರೆ (ap_cpu_ready == cpu) {
				cpu_ಗಣ_flag(cpu, CPU_IS_READY);
				ನಿಲ್ಲಿಸು;
			}
		}
		ಆದರೆ (ap_cpu_ready == -1) {
			ಬರೆ("WARNING : CPU %d didn't boot\n", cpu);
		}
	}

	phys_copy((phys_ಬೈಟುಗಳು) &biosresetvector, 0x467, ಗಾತ್ರ(u32_t));

	outb(RTC_ಸೂಚ್ಯಂಕ, 0xF);
	outb(RTC_IO, 0);

	bsp_ಮುಕ್ತಾಯ_booting();
	NOT_REACHABLE;
}

ಶೂನ್ಯ smp_halt_cpu (ಶೂನ್ಯ) 
{
	NOT_IMPLEMENTED;
}

ಶೂನ್ಯ smp_shutdown_aps(ಶೂನ್ಯ)
{
	ಚಿಹ್ನೆರಹಿತ cpu;

	ಆದರೆ (ncpus == 1)
		ಹೋಗು exit_shutdown_aps;

	/* we must let the other cpus enter the kernel mode */
	BKL_UNLOCK();

	ಗಾಗಿ (cpu = 0; cpu < ncpus; cpu++) {
		ಆದರೆ (cpu == cpuid)
			ಮುಂದುವರಿ;
		ಆದರೆ (!cpu_test_flag(cpu, CPU_IS_READY)) {
			ಬರೆ("CPU %d didn't boot\n", cpu);
			ಮುಂದುವರಿ;
		}

		cpu_ಕೆಳಕ್ಕೆ = -1;
		barrier();
		apic_send_ipi(APIC_SMP_CPU_HALT_ಸದಿಶ, cpu, APIC_IPI_DEST);
		/* wait for the cpu to be down */
		ಆಗ (cpu_ಕೆಳಕ್ಕೆ != cpu);
		ಬರೆ("CPU %d is down\n", cpu);
		cpu_clear_flag(cpu, CPU_IS_READY);
	}

exit_shutdown_aps:
	ioapic_disable_all();

	lapic_disable();

	ncpus = 1; /* hopefully !!! */
	lapic_addr = lapic_eoi_addr = 0;
	ಹಿಂತಿರುಗಿ;
}

ಸ್ಥಿರ ಶೂನ್ಯ ap_ಮುಕ್ತಾಯ_booting(ಶೂನ್ಯ)
{
	ಚಿಹ್ನೆರಹಿತ cpu = cpuid;

	/* inform the world of our presence. */
	ap_cpu_ready = cpu;

	/*
	 * Finish processor initialisation.  CPUs must be excluded from running.
	 * lapic timer calibration locks and unlocks the BKL because of the
	 * nested interrupts used for calibration. Therefore BKL is not good
	 * enough, the boot_lock must be held.
	 */
	spinlock_ಬೀಗ(&boot_ಬೀಗ);
	BKL_ಬೀಗ();

	ಬರೆ("CPU %d is up\n", cpu);

	cpu_identify();

	lapic_enable(cpu);
	fpu_init();

	ಆದರೆ (app_cpu_init_timer(system_hz)) {
		panic("FATAL : failed to initialize timer interrupts CPU %d, "
				"cannot continue without any clock source!", cpu);
	}

	/* FIXME assign CPU local idle structure */
	get_cpulocal_var(proc_ptr) = get_cpulocal_var_ptr(idle_proc);
	get_cpulocal_var(bill_ptr) = get_cpulocal_var_ptr(idle_proc);

	ap_boot_ಪೂರ್ಣಗೊಂಡ(cpu);
	spinlock_unlock(&boot_ಬೀಗ);

	switch_ಗಮ್ಯ_user();
	NOT_REACHABLE;
}

ಶೂನ್ಯ smp_ap_boot(ಶೂನ್ಯ)
{
	switch_k_ರಾಶಿ((ಅಕ್ಷರ *)get_k_ರಾಶಿ_ತುದಿ(__ap_id) -
			X86_ರಾಶಿ_ತುದಿ_RESERVED, ap_ಮುಕ್ತಾಯ_booting);
}

ಸ್ಥಿರ ಶೂನ್ಯ smp_reinit_vars(ಶೂನ್ಯ)
{
	lapic_addr = lapic_eoi_addr = 0;
	ioapic_enabled = 0;

	ncpus = 1;
}

ಸ್ಥಿರ ಶೂನ್ಯ tss_init_all(ಶೂನ್ಯ)
{
	ಚಿಹ್ನೆರಹಿತ cpu;

	ಗಾಗಿ(cpu = 0; cpu < ncpus ; cpu++)
		tss_init(cpu, get_k_ರಾಶಿ_ತುದಿ(cpu)); 
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ discover_cpus(ಶೂನ್ಯ)
{
	ರಚನೆ acpi_madt_lapic * cpu;

	ಆಗ (ncpus < CONFIG_MAX_CPUS && (cpu = acpi_get_lapic_ಮುಂದಿನ())) {
		apicid2cpuid[cpu->apic_id] = ncpus;
		cpuid2apicid[ncpus] = cpu->apic_id;
		ಬರೆ("CPU %3d local APIC id %3d\n", ncpus, cpu->apic_id);
		ncpus++;
	}

	ಹಿಂತಿರುಗಿ ncpus;
}

ಶೂನ್ಯ smp_init (ಶೂನ್ಯ)
{
	/* read the MP configuration */
	ಆದರೆ (!discover_cpus()) {
		ncpus = 1;
		ಹೋಗು uniproc_fallback;
	}

	lapic_addr = LOCAL_APIC_DEF_ADDR;
	ioapic_enabled = 0;

	tss_init_all();

	/* 
	 * we still run on the boot stack and we cannot use cpuid as its value
	 * wasn't set yet. apicid2cpuid initialized in mps_init()
	 */
	bsp_cpu_id = apicid2cpuid[apicid()];

	ಆದರೆ (!lapic_enable(bsp_cpu_id)) {
		ಬರೆ("ERROR : failed to initialize BSP Local APIC\n");
		ಹೋಗು uniproc_fallback;
	}

	bsp_lapic_id = apicid();
	
	acpi_init();

	ಆದರೆ (!detect_ioapics()) {
		lapic_disable();
		lapic_addr = 0x0;
		ಹೋಗು uniproc_fallback;
	}

	ioapic_enable_all();
	
	ಆದರೆ (ioapic_enabled)
		machine.apic_enabled = 1;

	/* set smp idt entries. */ 
	apic_idt_init(0); /* Not a reset ! */
	idt_reload();

	BOOT_VERBOSE(ಬರೆ("SMP initialized\n"));

	switch_k_ರಾಶಿ((ಅಕ್ಷರ *)get_k_ರಾಶಿ_ತುದಿ(bsp_cpu_id) -
			X86_ರಾಶಿ_ತುದಿ_RESERVED, smp_ಆರಂಭ_aps);
	
	ಹಿಂತಿರುಗಿ;

uniproc_fallback:
	apic_idt_init(1); /* Reset to PIC idt ! */
	idt_reload();
	smp_reinit_vars (); /* revert to a single proc system. */
	intr_init(0); /* no auto eoi */
	ಬರೆ("WARNING : SMP initialization failed\n");
}
	
ಶೂನ್ಯ arch_smp_halt_cpu(ಶೂನ್ಯ)
{
	/* say that we are down */
	cpu_ಕೆಳಕ್ಕೆ = cpuid;
	barrier();
	/* unlock the BKL and don't continue */
	BKL_UNLOCK();
	ಗಾಗಿ(;;);
}

ಶೂನ್ಯ arch_send_smp_ವೇಳಾಪಟ್ಟಿ_ipi(ಚಿಹ್ನೆರಹಿತ cpu)
{
	apic_send_ipi(APIC_SMP_SCHED_PROC_ಸದಿಶ, cpu, APIC_IPI_DEST);
}
