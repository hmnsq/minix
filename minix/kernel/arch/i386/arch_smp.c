/* This file contains essentially the MP handling code of the Minix kernel.
 *
 * Changes:
 *   Apr 1,  2008     Added SMP support.
 */

#define _SMP

#ଅନ୍ତର୍ଭାଵ <unistd.h>
#ଅନ୍ତର୍ଭାଵ <assert.h>
#ଅନ୍ତର୍ଭାଵ <stdlib.h>
#ଅନ୍ତର୍ଭାଵ <string.h>
#ଅନ୍ତର୍ଭାଵ <machine/cmos.h>
#ଅନ୍ତର୍ଭାଵ <machine/bios.h>

#ଅନ୍ତର୍ଭାଵ "kernel/spinlock.h"
#ଅନ୍ତର୍ଭାଵ "kernel/smp.h"
#ଅନ୍ତର୍ଭାଵ "apic.h"
#ଅନ୍ତର୍ଭାଵ "acpi.h"
#ଅନ୍ତର୍ଭାଵ "kernel/clock.h"

#ଅନ୍ତର୍ଭାଵ "glo.h"

ଶୂନ୍ୟ trampoline(ଶୂନ୍ୟ);

/*
 * arguments for trampoline. We need to pass the logical cpu id, gdt and idt.
 * They have to be in location which is reachable using absolute addressing in
 * 16-bit mode
 */
ବାହ୍ୟ ଅସ୍ଥିର u32_t __ap_id, __ap_pt;
ବାହ୍ୟ ଅସ୍ଥିର ସଂରଚନା desctableptr_s __ap_gdt, __ap_idt;
ବାହ୍ୟ u32_t __ap_gdt_tab, __ap_idt_tab;
ବାହ୍ୟ ଶୂନ୍ୟ * __trampoline_end;

ବାହ୍ୟ u32_t busclock[CONFIG_MAX_CPUS];
ବାହ୍ୟ ପୂର୍ଣ୍ଣସଂଖ୍ୟା panicking;

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅସ୍ଥିର ap_cpu_ready;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅସ୍ଥିର cpu_down;

/* there can be at most 255 local APIC ids, each fits in 8 bits */
ସ୍ଥିର unsigned ଅକ୍ଷର apicid2cpuid[255];
unsigned ଅକ୍ଷର cpuid2apicid[CONFIG_MAX_CPUS];

SPINLOCK_DEFINE(smp_cpu_lock)
SPINLOCK_DEFINE(dispq_lock)

ସ୍ଥିର ଶୂନ୍ୟ smp_reinit_vars(ଶୂନ୍ୟ);

/* These are initialized in protect.c */
ବାହ୍ୟ ସଂରଚନା segdesc_s gdt[GDT_SIZE];
ବାହ୍ୟ ସଂରଚନା gatedesc_s idt[IDT_SIZE];
ବାହ୍ୟ ସଂରଚନା tss_s tss[CONFIG_MAX_CPUS];
ବାହ୍ୟ ପୂର୍ଣ୍ଣସଂଖ୍ୟା prot_init_done;	/* Indicates they are ready */

ସ୍ଥିର phys_bytes trampoline_base;

ସ୍ଥିର u32_t ap_lin_addr(ଶୂନ୍ୟ *vaddr)
{
	ପ୍ରତିଜ୍ଞା(trampoline_base);
	ଫେରନ୍ତୁ (u32_t) vaddr - (u32_t) &trampoline + trampoline_base;
}

/*
 * copies the 16-bit AP trampoline code to the first 1M of memory
 */
ଶୂନ୍ୟ copy_trampoline(ଶୂନ୍ୟ)
{
	unsigned tramp_size, tramp_start = (unsigned)&trampoline;;

	/* The trampoline code/data is made to be page-aligned. */
	ପ୍ରତିଜ୍ଞା(!(tramp_start % I386_PAGE_SIZE));

	tramp_size = (unsigned) &__trampoline_end - tramp_start;
	trampoline_base = alloc_lowest(&kinfo, tramp_size);

	/* The memory allocator finds the lowest available memory.. 
	 * Verify it's low enough
	 */
	ପ୍ରତିଜ୍ଞା(trampoline_base + tramp_size < (1 << 20));

	/* prepare gdt and idt for the new cpus; make copies
	 * of both the tables and the descriptors of them
	 * in their boot addressing environment.
	 */
	ପ୍ରତିଜ୍ଞା(prot_init_done);
	memcpy(&__ap_gdt_tab, gdt, ଆକାର(gdt));
	memcpy(&__ap_idt_tab, gdt, ଆକାର(idt));
	__ap_gdt.base = ap_lin_addr(&__ap_gdt_tab);
	__ap_gdt.limit = ଆକାର(gdt)-1;
	__ap_idt.base = ap_lin_addr(&__ap_idt_tab);
	__ap_idt.limit = ଆକାର(idt)-1;

	phys_copy((phys_bytes) trampoline, trampoline_base, tramp_size);
}

ବାହ୍ୟ ପୂର୍ଣ୍ଣସଂଖ୍ୟା booting_cpu;	/* tell protect.c what to do */

ସ୍ଥିର ଶୂନ୍ୟ smp_start_aps(ଶୂନ୍ୟ)
{
	unsigned cpu;
	u32_t biosresetvector;
	phys_bytes __ap_id_phys;
	ସଂରଚନା proc *bootstrap_pt = get_cpulocal_var(ptproc);

	/* TODO hack around the alignment problem */

	phys_copy(0x467, (phys_bytes) &biosresetvector, ଆକାର(u32_t));

	/* set the bios shutdown code to 0xA */
	outb(RTC_INDEX, 0xF);
	outb(RTC_IO, 0xA);

	ପ୍ରତିଜ୍ଞା(bootstrap_pt);
	ପ୍ରତିଜ୍ଞା(bootstrap_pt->p_seg.p_cr3);
	__ap_pt  = bootstrap_pt->p_seg.p_cr3;
	ପ୍ରତିଜ୍ଞା(__ap_pt);

	copy_trampoline();

	/* New locations for cpu id, pagetable root */
	__ap_id_phys = trampoline_base +
		(phys_bytes) &__ap_id - (phys_bytes)&trampoline;

	/* setup the warm reset vector */
	phys_copy((phys_bytes) &trampoline_base, 0x467, ଆକାର(u32_t));

	/* okay, we're ready to go.  boot all of the ap's now.  we loop through
	 * using the processor's apic id values.
	 */
	ପାଇଁ (cpu = 0; cpu < ncpus; cpu++) {
		ap_cpu_ready = -1;
		/* Don't send INIT/SIPI to boot cpu.  */
		ଯଦି((apicid() == cpuid2apicid[cpu]) && 
				(apicid() == bsp_lapic_id)) {
			ଜାରି;
		}

		__ap_id = booting_cpu = cpu;
		phys_copy((phys_bytes) &__ap_id, __ap_id_phys, ଆକାର(__ap_id));
		mfence();
		ଯଦି (apic_send_init_ipi(cpu, trampoline_base) ||
				apic_send_startup_ipi(cpu, trampoline_base)) {
			printf("WARNING cannot boot cpu %d\n", cpu);
			ଜାରି;
		}

		/* wait for 5 secs for the processors to boot */
		lapic_set_timer_one_shot(5000000); 

		ଯେତେବେଳେ (lapic_read(LAPIC_TIMER_CCR)) {
			ଯଦି (ap_cpu_ready == cpu) {
				cpu_set_flag(cpu, CPU_IS_READY);
				ଭାଙ୍ଗ;
			}
		}
		ଯଦି (ap_cpu_ready == -1) {
			printf("WARNING : CPU %d didn't boot\n", cpu);
		}
	}

	phys_copy((phys_bytes) &biosresetvector, 0x467, ଆକାର(u32_t));

	outb(RTC_INDEX, 0xF);
	outb(RTC_IO, 0);

	bsp_finish_booting();
	NOT_REACHABLE;
}

ଶୂନ୍ୟ smp_halt_cpu (ଶୂନ୍ୟ) 
{
	NOT_IMPLEMENTED;
}

ଶୂନ୍ୟ smp_shutdown_aps(ଶୂନ୍ୟ)
{
	unsigned cpu;

	ଯଦି (ncpus == 1)
		ଯାଅ exit_shutdown_aps;

	/* we must let the other cpus enter the kernel mode */
	BKL_UNLOCK();

	ପାଇଁ (cpu = 0; cpu < ncpus; cpu++) {
		ଯଦି (cpu == cpuid)
			ଜାରି;
		ଯଦି (!cpu_test_flag(cpu, CPU_IS_READY)) {
			printf("CPU %d didn't boot\n", cpu);
			ଜାରି;
		}

		cpu_down = -1;
		barrier();
		apic_send_ipi(APIC_SMP_CPU_HALT_VECTOR, cpu, APIC_IPI_DEST);
		/* wait for the cpu to be down */
		ଯେତେବେଳେ (cpu_down != cpu);
		printf("CPU %d is down\n", cpu);
		cpu_clear_flag(cpu, CPU_IS_READY);
	}

exit_shutdown_aps:
	ioapic_disable_all();

	lapic_disable();

	ncpus = 1; /* hopefully !!! */
	lapic_addr = lapic_eoi_addr = 0;
	ଫେରନ୍ତୁ;
}

ସ୍ଥିର ଶୂନ୍ୟ ap_finish_booting(ଶୂନ୍ୟ)
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

	ଯଦି (app_cpu_init_timer(system_hz)) {
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

ଶୂନ୍ୟ smp_ap_boot(ଶୂନ୍ୟ)
{
	switch_k_ଗଦା((ଅକ୍ଷର *)get_k_ଗଦା_ଶୀର୍ଷ(__ap_id) -
			X86_ଗଦା_ଶୀର୍ଷ_RESERVED, ap_finish_booting);
}

ସ୍ଥିର ଶୂନ୍ୟ smp_reinit_vars(ଶୂନ୍ୟ)
{
	lapic_addr = lapic_eoi_addr = 0;
	ioapic_enabled = 0;

	ncpus = 1;
}

ସ୍ଥିର ଶୂନ୍ୟ tss_init_all(ଶୂନ୍ୟ)
{
	unsigned cpu;

	ପାଇଁ(cpu = 0; cpu < ncpus ; cpu++)
		tss_init(cpu, get_k_ଗଦା_ଶୀର୍ଷ(cpu)); 
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା discover_cpus(ଶୂନ୍ୟ)
{
	ସଂରଚନା acpi_madt_lapic * cpu;

	ଯେତେବେଳେ (ncpus < CONFIG_MAX_CPUS && (cpu = acpi_get_lapic_next())) {
		apicid2cpuid[cpu->apic_id] = ncpus;
		cpuid2apicid[ncpus] = cpu->apic_id;
		printf("CPU %3d local APIC id %3d\n", ncpus, cpu->apic_id);
		ncpus++;
	}

	ଫେରନ୍ତୁ ncpus;
}

ଶୂନ୍ୟ smp_init (ଶୂନ୍ୟ)
{
	/* read the MP configuration */
	ଯଦି (!discover_cpus()) {
		ncpus = 1;
		ଯାଅ uniproc_fallback;
	}

	lapic_addr = LOCAL_APIC_DEF_ADDR;
	ioapic_enabled = 0;

	tss_init_all();

	/* 
	 * we still run on the boot stack and we cannot use cpuid as its value
	 * wasn't set yet. apicid2cpuid initialized in mps_init()
	 */
	bsp_cpu_id = apicid2cpuid[apicid()];

	ଯଦି (!lapic_enable(bsp_cpu_id)) {
		printf("ERROR : failed to initialize BSP Local APIC\n");
		ଯାଅ uniproc_fallback;
	}

	bsp_lapic_id = apicid();
	
	acpi_init();

	ଯଦି (!detect_ioapics()) {
		lapic_disable();
		lapic_addr = 0x0;
		ଯାଅ uniproc_fallback;
	}

	ioapic_enable_all();
	
	ଯଦି (ioapic_enabled)
		machine.apic_enabled = 1;

	/* set smp idt entries. */ 
	apic_idt_init(0); /* Not a reset ! */
	idt_reload();

	BOOT_VERBOSE(printf("SMP initialized\n"));

	switch_k_ଗଦା((ଅକ୍ଷର *)get_k_ଗଦା_ଶୀର୍ଷ(bsp_cpu_id) -
			X86_ଗଦା_ଶୀର୍ଷ_RESERVED, smp_start_aps);
	
	ଫେରନ୍ତୁ;

uniproc_fallback:
	apic_idt_init(1); /* Reset to PIC idt ! */
	idt_reload();
	smp_reinit_vars (); /* revert to a single proc system. */
	intr_init(0); /* no auto eoi */
	printf("WARNING : SMP initialization failed\n");
}
	
ଶୂନ୍ୟ arch_smp_halt_cpu(ଶୂନ୍ୟ)
{
	/* say that we are down */
	cpu_down = cpuid;
	barrier();
	/* unlock the BKL and don't continue */
	BKL_UNLOCK();
	ପାଇଁ(;;);
}

ଶୂନ୍ୟ arch_send_smp_schedule_ipi(unsigned cpu)
{
	apic_send_ipi(APIC_SMP_SCHED_PROC_VECTOR, cpu, APIC_IPI_DEST);
}
