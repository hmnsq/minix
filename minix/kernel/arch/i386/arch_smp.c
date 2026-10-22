/* This file contains essentially the MP handling code of the Minix kernel.
 *
 * Changes:
 *   Apr 1,  2008     Added SMP support.
 */

#define _SMP

#અન્તર્ભાવ <unistd.h>
#અન્તર્ભાવ <assert.h>
#અન્તર્ભાવ <stdlib.h>
#અન્તર્ભાવ <string.h>
#અન્તર્ભાવ <machine/cmos.h>
#અન્તર્ભાવ <machine/bios.h>

#અન્તર્ભાવ "kernel/spinlock.h"
#અન્તર્ભાવ "kernel/smp.h"
#અન્તર્ભાવ "apic.h"
#અન્તર્ભાવ "acpi.h"
#અન્તર્ભાવ "kernel/clock.h"

#અન્તર્ભાવ "glo.h"

શૂન્ય trampoline(શૂન્ય);

/*
 * arguments for trampoline. We need to pass the logical cpu id, gdt and idt.
 * They have to be in location which is reachable using absolute addressing in
 * 16-bit mode
 */
બાહ્ય અસ્થિર u32_t __ap_id, __ap_pt;
બાહ્ય અસ્થિર સંરચના desctableptr_s __ap_gdt, __ap_idt;
બાહ્ય u32_t __ap_gdt_tab, __ap_idt_tab;
બાહ્ય શૂન્ય * __trampoline_end;

બાહ્ય u32_t busclock[CONFIG_MAX_CPUS];
બાહ્ય પૂર્ણાંક panicking;

સ્થિર પૂર્ણાંક અસ્થિર ap_cpu_ready;
સ્થિર પૂર્ણાંક અસ્થિર cpu_down;

/* there can be at most 255 local APIC ids, each fits in 8 bits */
સ્થિર unsigned અક્ષર apicid2cpuid[255];
unsigned અક્ષર cpuid2apicid[CONFIG_MAX_CPUS];

SPINLOCK_DEFINE(smp_cpu_lock)
SPINLOCK_DEFINE(dispq_lock)

સ્થિર શૂન્ય smp_reinit_vars(શૂન્ય);

/* These are initialized in protect.c */
બાહ્ય સંરચના segdesc_s gdt[GDT_SIZE];
બાહ્ય સંરચના gatedesc_s idt[IDT_SIZE];
બાહ્ય સંરચના tss_s tss[CONFIG_MAX_CPUS];
બાહ્ય પૂર્ણાંક prot_init_done;	/* Indicates they are ready */

સ્થિર phys_bytes trampoline_base;

સ્થિર u32_t ap_lin_addr(શૂન્ય *vaddr)
{
	પ્રતિજ્ઞા(trampoline_base);
	પાછા (u32_t) vaddr - (u32_t) &trampoline + trampoline_base;
}

/*
 * copies the 16-bit AP trampoline code to the first 1M of memory
 */
શૂન્ય copy_trampoline(શૂન્ય)
{
	unsigned tramp_size, tramp_start = (unsigned)&trampoline;;

	/* The trampoline code/data is made to be page-aligned. */
	પ્રતિજ્ઞા(!(tramp_start % I386_PAGE_SIZE));

	tramp_size = (unsigned) &__trampoline_end - tramp_start;
	trampoline_base = alloc_lowest(&kinfo, tramp_size);

	/* The memory allocator finds the lowest available memory.. 
	 * Verify it's low enough
	 */
	પ્રતિજ્ઞા(trampoline_base + tramp_size < (1 << 20));

	/* prepare gdt and idt for the new cpus; make copies
	 * of both the tables and the descriptors of them
	 * in their boot addressing environment.
	 */
	પ્રતિજ્ઞા(prot_init_done);
	memcpy(&__ap_gdt_tab, gdt, કદ(gdt));
	memcpy(&__ap_idt_tab, gdt, કદ(idt));
	__ap_gdt.base = ap_lin_addr(&__ap_gdt_tab);
	__ap_gdt.limit = કદ(gdt)-1;
	__ap_idt.base = ap_lin_addr(&__ap_idt_tab);
	__ap_idt.limit = કદ(idt)-1;

	phys_copy((phys_bytes) trampoline, trampoline_base, tramp_size);
}

બાહ્ય પૂર્ણાંક booting_cpu;	/* tell protect.c what to do */

સ્થિર શૂન્ય smp_start_aps(શૂન્ય)
{
	unsigned cpu;
	u32_t biosresetvector;
	phys_bytes __ap_id_phys;
	સંરચના proc *bootstrap_pt = get_cpulocal_var(ptproc);

	/* TODO hack around the alignment problem */

	phys_copy(0x467, (phys_bytes) &biosresetvector, કદ(u32_t));

	/* set the bios shutdown code to 0xA */
	outb(RTC_INDEX, 0xF);
	outb(RTC_IO, 0xA);

	પ્રતિજ્ઞા(bootstrap_pt);
	પ્રતિજ્ઞા(bootstrap_pt->p_seg.p_cr3);
	__ap_pt  = bootstrap_pt->p_seg.p_cr3;
	પ્રતિજ્ઞા(__ap_pt);

	copy_trampoline();

	/* New locations for cpu id, pagetable root */
	__ap_id_phys = trampoline_base +
		(phys_bytes) &__ap_id - (phys_bytes)&trampoline;

	/* setup the warm reset vector */
	phys_copy((phys_bytes) &trampoline_base, 0x467, કદ(u32_t));

	/* okay, we're ready to go.  boot all of the ap's now.  we loop through
	 * using the processor's apic id values.
	 */
	માટે (cpu = 0; cpu < ncpus; cpu++) {
		ap_cpu_ready = -1;
		/* Don't send INIT/SIPI to boot cpu.  */
		જો((apicid() == cpuid2apicid[cpu]) && 
				(apicid() == bsp_lapic_id)) {
			ચાલુ;
		}

		__ap_id = booting_cpu = cpu;
		phys_copy((phys_bytes) &__ap_id, __ap_id_phys, કદ(__ap_id));
		mfence();
		જો (apic_send_init_ipi(cpu, trampoline_base) ||
				apic_send_startup_ipi(cpu, trampoline_base)) {
			printf("WARNING cannot boot cpu %d\n", cpu);
			ચાલુ;
		}

		/* wait for 5 secs for the processors to boot */
		lapic_set_timer_one_shot(5000000); 

		જ્યારે (lapic_read(LAPIC_TIMER_CCR)) {
			જો (ap_cpu_ready == cpu) {
				cpu_set_flag(cpu, CPU_IS_READY);
				તોડો;
			}
		}
		જો (ap_cpu_ready == -1) {
			printf("WARNING : CPU %d didn't boot\n", cpu);
		}
	}

	phys_copy((phys_bytes) &biosresetvector, 0x467, કદ(u32_t));

	outb(RTC_INDEX, 0xF);
	outb(RTC_IO, 0);

	bsp_finish_booting();
	NOT_REACHABLE;
}

શૂન્ય smp_halt_cpu (શૂન્ય) 
{
	NOT_IMPLEMENTED;
}

શૂન્ય smp_shutdown_aps(શૂન્ય)
{
	unsigned cpu;

	જો (ncpus == 1)
		જાઓ exit_shutdown_aps;

	/* we must let the other cpus enter the kernel mode */
	BKL_UNLOCK();

	માટે (cpu = 0; cpu < ncpus; cpu++) {
		જો (cpu == cpuid)
			ચાલુ;
		જો (!cpu_test_flag(cpu, CPU_IS_READY)) {
			printf("CPU %d didn't boot\n", cpu);
			ચાલુ;
		}

		cpu_down = -1;
		barrier();
		apic_send_ipi(APIC_SMP_CPU_HALT_VECTOR, cpu, APIC_IPI_DEST);
		/* wait for the cpu to be down */
		જ્યારે (cpu_down != cpu);
		printf("CPU %d is down\n", cpu);
		cpu_clear_flag(cpu, CPU_IS_READY);
	}

exit_shutdown_aps:
	ioapic_disable_all();

	lapic_disable();

	ncpus = 1; /* hopefully !!! */
	lapic_addr = lapic_eoi_addr = 0;
	પાછા;
}

સ્થિર શૂન્ય ap_finish_booting(શૂન્ય)
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

	જો (app_cpu_init_timer(system_hz)) {
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

શૂન્ય smp_ap_boot(શૂન્ય)
{
	switch_k_ઢગલો((અક્ષર *)get_k_ઢગલો_ટોચ(__ap_id) -
			X86_ઢગલો_ટોચ_RESERVED, ap_finish_booting);
}

સ્થિર શૂન્ય smp_reinit_vars(શૂન્ય)
{
	lapic_addr = lapic_eoi_addr = 0;
	ioapic_enabled = 0;

	ncpus = 1;
}

સ્થિર શૂન્ય tss_init_all(શૂન્ય)
{
	unsigned cpu;

	માટે(cpu = 0; cpu < ncpus ; cpu++)
		tss_init(cpu, get_k_ઢગલો_ટોચ(cpu)); 
}

સ્થિર પૂર્ણાંક discover_cpus(શૂન્ય)
{
	સંરચના acpi_madt_lapic * cpu;

	જ્યારે (ncpus < CONFIG_MAX_CPUS && (cpu = acpi_get_lapic_next())) {
		apicid2cpuid[cpu->apic_id] = ncpus;
		cpuid2apicid[ncpus] = cpu->apic_id;
		printf("CPU %3d local APIC id %3d\n", ncpus, cpu->apic_id);
		ncpus++;
	}

	પાછા ncpus;
}

શૂન્ય smp_init (શૂન્ય)
{
	/* read the MP configuration */
	જો (!discover_cpus()) {
		ncpus = 1;
		જાઓ uniproc_fallback;
	}

	lapic_addr = LOCAL_APIC_DEF_ADDR;
	ioapic_enabled = 0;

	tss_init_all();

	/* 
	 * we still run on the boot stack and we cannot use cpuid as its value
	 * wasn't set yet. apicid2cpuid initialized in mps_init()
	 */
	bsp_cpu_id = apicid2cpuid[apicid()];

	જો (!lapic_enable(bsp_cpu_id)) {
		printf("ERROR : failed to initialize BSP Local APIC\n");
		જાઓ uniproc_fallback;
	}

	bsp_lapic_id = apicid();
	
	acpi_init();

	જો (!detect_ioapics()) {
		lapic_disable();
		lapic_addr = 0x0;
		જાઓ uniproc_fallback;
	}

	ioapic_enable_all();
	
	જો (ioapic_enabled)
		machine.apic_enabled = 1;

	/* set smp idt entries. */ 
	apic_idt_init(0); /* Not a reset ! */
	idt_reload();

	BOOT_VERBOSE(printf("SMP initialized\n"));

	switch_k_ઢગલો((અક્ષર *)get_k_ઢગલો_ટોચ(bsp_cpu_id) -
			X86_ઢગલો_ટોચ_RESERVED, smp_start_aps);
	
	પાછા;

uniproc_fallback:
	apic_idt_init(1); /* Reset to PIC idt ! */
	idt_reload();
	smp_reinit_vars (); /* revert to a single proc system. */
	intr_init(0); /* no auto eoi */
	printf("WARNING : SMP initialization failed\n");
}
	
શૂન્ય arch_smp_halt_cpu(શૂન્ય)
{
	/* say that we are down */
	cpu_down = cpuid;
	barrier();
	/* unlock the BKL and don't continue */
	BKL_UNLOCK();
	માટે(;;);
}

શૂન્ય arch_send_smp_schedule_ipi(unsigned cpu)
{
	apic_send_ipi(APIC_SMP_SCHED_PROC_VECTOR, cpu, APIC_IPI_DEST);
}
