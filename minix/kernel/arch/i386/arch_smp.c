/* This file contains essentially the MP handling code of the Minix kernel.
 *
 * Changes:
 *   Apr 1,  2008     Added SMP support.
 */

#define _SMP

#অন্তর্ভাৱ <unistd.h>
#অন্তর্ভাৱ <assert.h>
#অন্তর্ভাৱ <stdlib.h>
#অন্তর্ভাৱ <string.h>
#অন্তর্ভাৱ <machine/cmos.h>
#অন্তর্ভাৱ <machine/bios.h>

#অন্তর্ভাৱ "kernel/spinlock.h"
#অন্তর্ভাৱ "kernel/smp.h"
#অন্তর্ভাৱ "apic.h"
#অন্তর্ভাৱ "acpi.h"
#অন্তর্ভাৱ "kernel/clock.h"

#অন্তর্ভাৱ "glo.h"

শূন্য trampoline(শূন্য);

/*
 * arguments for trampoline. We need to pass the logical cpu id, gdt and idt.
 * They have to be in location which is reachable using absolute addressing in
 * 16-bit mode
 */
বাহ্য চঞ্চল u32_t __ap_id, __ap_pt;
বাহ্য চঞ্চল সংরচনা desctableptr_s __ap_gdt, __ap_idt;
বাহ্য u32_t __ap_gdt_tab, __ap_idt_tab;
বাহ্য শূন্য * __trampoline_end;

বাহ্য u32_t busclock[CONFIG_MAX_CPUS];
বাহ্য পূর্ণাঙ্ক panicking;

স্থির পূর্ণাঙ্ক চঞ্চল ap_cpu_ready;
স্থির পূর্ণাঙ্ক চঞ্চল cpu_down;

/* there can be at most 255 local APIC ids, each fits in 8 bits */
স্থির unsigned ৱর্ণ apicid2cpuid[255];
unsigned ৱর্ণ cpuid2apicid[CONFIG_MAX_CPUS];

SPINLOCK_DEFINE(smp_cpu_lock)
SPINLOCK_DEFINE(dispq_lock)

স্থির শূন্য smp_reinit_vars(শূন্য);

/* These are initialized in protect.c */
বাহ্য সংরচনা segdesc_s gdt[GDT_SIZE];
বাহ্য সংরচনা gatedesc_s idt[IDT_SIZE];
বাহ্য সংরচনা tss_s tss[CONFIG_MAX_CPUS];
বাহ্য পূর্ণাঙ্ক prot_init_done;	/* Indicates they are ready */

স্থির phys_bytes trampoline_base;

স্থির u32_t ap_lin_addr(শূন্য *vaddr)
{
	প্রতিজ্ঞা(trampoline_base);
	ঘূৰাই (u32_t) vaddr - (u32_t) &trampoline + trampoline_base;
}

/*
 * copies the 16-bit AP trampoline code to the first 1M of memory
 */
শূন্য copy_trampoline(শূন্য)
{
	unsigned tramp_size, tramp_start = (unsigned)&trampoline;;

	/* The trampoline code/data is made to be page-aligned. */
	প্রতিজ্ঞা(!(tramp_start % I386_PAGE_SIZE));

	tramp_size = (unsigned) &__trampoline_end - tramp_start;
	trampoline_base = alloc_lowest(&kinfo, tramp_size);

	/* The memory allocator finds the lowest available memory.. 
	 * Verify it's low enough
	 */
	প্রতিজ্ঞা(trampoline_base + tramp_size < (1 << 20));

	/* prepare gdt and idt for the new cpus; make copies
	 * of both the tables and the descriptors of them
	 * in their boot addressing environment.
	 */
	প্রতিজ্ঞা(prot_init_done);
	memcpy(&__ap_gdt_tab, gdt, পরিমাণ(gdt));
	memcpy(&__ap_idt_tab, gdt, পরিমাণ(idt));
	__ap_gdt.base = ap_lin_addr(&__ap_gdt_tab);
	__ap_gdt.limit = পরিমাণ(gdt)-1;
	__ap_idt.base = ap_lin_addr(&__ap_idt_tab);
	__ap_idt.limit = পরিমাণ(idt)-1;

	phys_copy((phys_bytes) trampoline, trampoline_base, tramp_size);
}

বাহ্য পূর্ণাঙ্ক booting_cpu;	/* tell protect.c what to do */

স্থির শূন্য smp_start_aps(শূন্য)
{
	unsigned cpu;
	u32_t biosresetvector;
	phys_bytes __ap_id_phys;
	সংরচনা proc *bootstrap_pt = get_cpulocal_var(ptproc);

	/* TODO hack around the alignment problem */

	phys_copy(0x467, (phys_bytes) &biosresetvector, পরিমাণ(u32_t));

	/* set the bios shutdown code to 0xA */
	outb(RTC_INDEX, 0xF);
	outb(RTC_IO, 0xA);

	প্রতিজ্ঞা(bootstrap_pt);
	প্রতিজ্ঞা(bootstrap_pt->p_seg.p_cr3);
	__ap_pt  = bootstrap_pt->p_seg.p_cr3;
	প্রতিজ্ঞা(__ap_pt);

	copy_trampoline();

	/* New locations for cpu id, pagetable root */
	__ap_id_phys = trampoline_base +
		(phys_bytes) &__ap_id - (phys_bytes)&trampoline;

	/* setup the warm reset vector */
	phys_copy((phys_bytes) &trampoline_base, 0x467, পরিমাণ(u32_t));

	/* okay, we're ready to go.  boot all of the ap's now.  we loop through
	 * using the processor's apic id values.
	 */
	বাবে (cpu = 0; cpu < ncpus; cpu++) {
		ap_cpu_ready = -1;
		/* Don't send INIT/SIPI to boot cpu.  */
		যদি((apicid() == cpuid2apicid[cpu]) && 
				(apicid() == bsp_lapic_id)) {
			অনুৱর্ত;
		}

		__ap_id = booting_cpu = cpu;
		phys_copy((phys_bytes) &__ap_id, __ap_id_phys, পরিমাণ(__ap_id));
		mfence();
		যদি (apic_send_init_ipi(cpu, trampoline_base) ||
				apic_send_startup_ipi(cpu, trampoline_base)) {
			printf("WARNING cannot boot cpu %d\n", cpu);
			অনুৱর্ত;
		}

		/* wait for 5 secs for the processors to boot */
		lapic_set_timer_one_shot(5000000); 

		যেতিয়ালৈ (lapic_read(LAPIC_TIMER_CCR)) {
			যদি (ap_cpu_ready == cpu) {
				cpu_set_flag(cpu, CPU_IS_READY);
				ভঙ্গ;
			}
		}
		যদি (ap_cpu_ready == -1) {
			printf("WARNING : CPU %d didn't boot\n", cpu);
		}
	}

	phys_copy((phys_bytes) &biosresetvector, 0x467, পরিমাণ(u32_t));

	outb(RTC_INDEX, 0xF);
	outb(RTC_IO, 0);

	bsp_finish_booting();
	NOT_REACHABLE;
}

শূন্য smp_halt_cpu (শূন্য) 
{
	NOT_IMPLEMENTED;
}

শূন্য smp_shutdown_aps(শূন্য)
{
	unsigned cpu;

	যদি (ncpus == 1)
		গচ্ছ exit_shutdown_aps;

	/* we must let the other cpus enter the kernel mode */
	BKL_UNLOCK();

	বাবে (cpu = 0; cpu < ncpus; cpu++) {
		যদি (cpu == cpuid)
			অনুৱর্ত;
		যদি (!cpu_test_flag(cpu, CPU_IS_READY)) {
			printf("CPU %d didn't boot\n", cpu);
			অনুৱর্ত;
		}

		cpu_down = -1;
		barrier();
		apic_send_ipi(APIC_SMP_CPU_HALT_VECTOR, cpu, APIC_IPI_DEST);
		/* wait for the cpu to be down */
		যেতিয়ালৈ (cpu_down != cpu);
		printf("CPU %d is down\n", cpu);
		cpu_clear_flag(cpu, CPU_IS_READY);
	}

exit_shutdown_aps:
	ioapic_disable_all();

	lapic_disable();

	ncpus = 1; /* hopefully !!! */
	lapic_addr = lapic_eoi_addr = 0;
	ঘূৰাই;
}

স্থির শূন্য ap_finish_booting(শূন্য)
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

	যদি (app_cpu_init_timer(system_hz)) {
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

শূন্য smp_ap_boot(শূন্য)
{
	switch_k_স্তূপ((ৱর্ণ *)get_k_স্তূপ_শীৰ্ষ(__ap_id) -
			X86_স্তূপ_শীৰ্ষ_RESERVED, ap_finish_booting);
}

স্থির শূন্য smp_reinit_vars(শূন্য)
{
	lapic_addr = lapic_eoi_addr = 0;
	ioapic_enabled = 0;

	ncpus = 1;
}

স্থির শূন্য tss_init_all(শূন্য)
{
	unsigned cpu;

	বাবে(cpu = 0; cpu < ncpus ; cpu++)
		tss_init(cpu, get_k_স্তূপ_শীৰ্ষ(cpu)); 
}

স্থির পূর্ণাঙ্ক discover_cpus(শূন্য)
{
	সংরচনা acpi_madt_lapic * cpu;

	যেতিয়ালৈ (ncpus < CONFIG_MAX_CPUS && (cpu = acpi_get_lapic_next())) {
		apicid2cpuid[cpu->apic_id] = ncpus;
		cpuid2apicid[ncpus] = cpu->apic_id;
		printf("CPU %3d local APIC id %3d\n", ncpus, cpu->apic_id);
		ncpus++;
	}

	ঘূৰাই ncpus;
}

শূন্য smp_init (শূন্য)
{
	/* read the MP configuration */
	যদি (!discover_cpus()) {
		ncpus = 1;
		গচ্ছ uniproc_fallback;
	}

	lapic_addr = LOCAL_APIC_DEF_ADDR;
	ioapic_enabled = 0;

	tss_init_all();

	/* 
	 * we still run on the boot stack and we cannot use cpuid as its value
	 * wasn't set yet. apicid2cpuid initialized in mps_init()
	 */
	bsp_cpu_id = apicid2cpuid[apicid()];

	যদি (!lapic_enable(bsp_cpu_id)) {
		printf("ERROR : failed to initialize BSP Local APIC\n");
		গচ্ছ uniproc_fallback;
	}

	bsp_lapic_id = apicid();
	
	acpi_init();

	যদি (!detect_ioapics()) {
		lapic_disable();
		lapic_addr = 0x0;
		গচ্ছ uniproc_fallback;
	}

	ioapic_enable_all();
	
	যদি (ioapic_enabled)
		machine.apic_enabled = 1;

	/* set smp idt entries. */ 
	apic_idt_init(0); /* Not a reset ! */
	idt_reload();

	BOOT_VERBOSE(printf("SMP initialized\n"));

	switch_k_স্তূপ((ৱর্ণ *)get_k_স্তূপ_শীৰ্ষ(bsp_cpu_id) -
			X86_স্তূপ_শীৰ্ষ_RESERVED, smp_start_aps);
	
	ঘূৰাই;

uniproc_fallback:
	apic_idt_init(1); /* Reset to PIC idt ! */
	idt_reload();
	smp_reinit_vars (); /* revert to a single proc system. */
	intr_init(0); /* no auto eoi */
	printf("WARNING : SMP initialization failed\n");
}
	
শূন্য arch_smp_halt_cpu(শূন্য)
{
	/* say that we are down */
	cpu_down = cpuid;
	barrier();
	/* unlock the BKL and don't continue */
	BKL_UNLOCK();
	বাবে(;;);
}

শূন্য arch_send_smp_schedule_ipi(unsigned cpu)
{
	apic_send_ipi(APIC_SMP_SCHED_PROC_VECTOR, cpu, APIC_IPI_DEST);
}
