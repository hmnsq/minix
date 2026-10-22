/* This file contains essentially the MP handling code of the Minix kernel.
 *
 * Changes:
 *   Apr 1,  2008     Added SMP support.
 */

#define _SMP

#அந்தர்பாவ <unistd.h>
#அந்தர்பாவ <assert.h>
#அந்தர்பாவ <stdlib.h>
#அந்தர்பாவ <string.h>
#அந்தர்பாவ <machine/cmos.h>
#அந்தர்பாவ <machine/bios.h>

#அந்தர்பாவ "kernel/spinlock.h"
#அந்தர்பாவ "kernel/smp.h"
#அந்தர்பாவ "apic.h"
#அந்தர்பாவ "acpi.h"
#அந்தர்பாவ "kernel/clock.h"

#அந்தர்பாவ "glo.h"

வெறுமை trampoline(வெறுமை);

/*
 * arguments for trampoline. We need to pass the logical cpu id, gdt and idt.
 * They have to be in location which is reachable using absolute addressing in
 * 16-bit mode
 */
வெளி நிலையற்ற u32_t __ap_id, __ap_pt;
வெளி நிலையற்ற கட்டமைப்பு desctableptr_s __ap_gdt, __ap_idt;
வெளி u32_t __ap_gdt_tab, __ap_idt_tab;
வெளி வெறுமை * __trampoline_end;

வெளி u32_t busclock[CONFIG_MAX_CPUS];
வெளி முழு panicking;

நிலையான முழு நிலையற்ற ap_cpu_ready;
நிலையான முழு நிலையற்ற cpu_down;

/* there can be at most 255 local APIC ids, each fits in 8 bits */
நிலையான unsigned எழுத்து apicid2cpuid[255];
unsigned எழுத்து cpuid2apicid[CONFIG_MAX_CPUS];

SPINLOCK_DEFINE(smp_cpu_lock)
SPINLOCK_DEFINE(dispq_lock)

நிலையான வெறுமை smp_reinit_vars(வெறுமை);

/* These are initialized in protect.c */
வெளி கட்டமைப்பு segdesc_s gdt[GDT_SIZE];
வெளி கட்டமைப்பு gatedesc_s idt[IDT_SIZE];
வெளி கட்டமைப்பு tss_s tss[CONFIG_MAX_CPUS];
வெளி முழு prot_init_done;	/* Indicates they are ready */

நிலையான phys_bytes trampoline_base;

நிலையான u32_t ap_lin_addr(வெறுமை *vaddr)
{
	ப்ரதிஜ்ஞா(trampoline_base);
	திரும்பு (u32_t) vaddr - (u32_t) &trampoline + trampoline_base;
}

/*
 * copies the 16-bit AP trampoline code to the first 1M of memory
 */
வெறுமை copy_trampoline(வெறுமை)
{
	unsigned tramp_size, tramp_start = (unsigned)&trampoline;;

	/* The trampoline code/data is made to be page-aligned. */
	ப்ரதிஜ்ஞா(!(tramp_start % I386_PAGE_SIZE));

	tramp_size = (unsigned) &__trampoline_end - tramp_start;
	trampoline_base = alloc_lowest(&kinfo, tramp_size);

	/* The memory allocator finds the lowest available memory.. 
	 * Verify it's low enough
	 */
	ப்ரதிஜ்ஞா(trampoline_base + tramp_size < (1 << 20));

	/* prepare gdt and idt for the new cpus; make copies
	 * of both the tables and the descriptors of them
	 * in their boot addressing environment.
	 */
	ப்ரதிஜ்ஞா(prot_init_done);
	memcpy(&__ap_gdt_tab, gdt, அளவு(gdt));
	memcpy(&__ap_idt_tab, gdt, அளவு(idt));
	__ap_gdt.base = ap_lin_addr(&__ap_gdt_tab);
	__ap_gdt.limit = அளவு(gdt)-1;
	__ap_idt.base = ap_lin_addr(&__ap_idt_tab);
	__ap_idt.limit = அளவு(idt)-1;

	phys_copy((phys_bytes) trampoline, trampoline_base, tramp_size);
}

வெளி முழு booting_cpu;	/* tell protect.c what to do */

நிலையான வெறுமை smp_start_aps(வெறுமை)
{
	unsigned cpu;
	u32_t biosresetvector;
	phys_bytes __ap_id_phys;
	கட்டமைப்பு proc *bootstrap_pt = get_cpulocal_var(ptproc);

	/* TODO hack around the alignment problem */

	phys_copy(0x467, (phys_bytes) &biosresetvector, அளவு(u32_t));

	/* set the bios shutdown code to 0xA */
	outb(RTC_INDEX, 0xF);
	outb(RTC_IO, 0xA);

	ப்ரதிஜ்ஞா(bootstrap_pt);
	ப்ரதிஜ்ஞா(bootstrap_pt->p_seg.p_cr3);
	__ap_pt  = bootstrap_pt->p_seg.p_cr3;
	ப்ரதிஜ்ஞா(__ap_pt);

	copy_trampoline();

	/* New locations for cpu id, pagetable root */
	__ap_id_phys = trampoline_base +
		(phys_bytes) &__ap_id - (phys_bytes)&trampoline;

	/* setup the warm reset vector */
	phys_copy((phys_bytes) &trampoline_base, 0x467, அளவு(u32_t));

	/* okay, we're ready to go.  boot all of the ap's now.  we loop through
	 * using the processor's apic id values.
	 */
	க்கு (cpu = 0; cpu < ncpus; cpu++) {
		ap_cpu_ready = -1;
		/* Don't send INIT/SIPI to boot cpu.  */
		எனில்((apicid() == cpuid2apicid[cpu]) && 
				(apicid() == bsp_lapic_id)) {
			தொடர்;
		}

		__ap_id = booting_cpu = cpu;
		phys_copy((phys_bytes) &__ap_id, __ap_id_phys, அளவு(__ap_id));
		mfence();
		எனில் (apic_send_init_ipi(cpu, trampoline_base) ||
				apic_send_startup_ipi(cpu, trampoline_base)) {
			printf("WARNING cannot boot cpu %d\n", cpu);
			தொடர்;
		}

		/* wait for 5 secs for the processors to boot */
		lapic_set_timer_one_shot(5000000); 

		போது (lapic_read(LAPIC_TIMER_CCR)) {
			எனில் (ap_cpu_ready == cpu) {
				cpu_set_flag(cpu, CPU_IS_READY);
				நிறுத்து;
			}
		}
		எனில் (ap_cpu_ready == -1) {
			printf("WARNING : CPU %d didn't boot\n", cpu);
		}
	}

	phys_copy((phys_bytes) &biosresetvector, 0x467, அளவு(u32_t));

	outb(RTC_INDEX, 0xF);
	outb(RTC_IO, 0);

	bsp_finish_booting();
	NOT_REACHABLE;
}

வெறுமை smp_halt_cpu (வெறுமை) 
{
	NOT_IMPLEMENTED;
}

வெறுமை smp_shutdown_aps(வெறுமை)
{
	unsigned cpu;

	எனில் (ncpus == 1)
		செல் exit_shutdown_aps;

	/* we must let the other cpus enter the kernel mode */
	BKL_UNLOCK();

	க்கு (cpu = 0; cpu < ncpus; cpu++) {
		எனில் (cpu == cpuid)
			தொடர்;
		எனில் (!cpu_test_flag(cpu, CPU_IS_READY)) {
			printf("CPU %d didn't boot\n", cpu);
			தொடர்;
		}

		cpu_down = -1;
		barrier();
		apic_send_ipi(APIC_SMP_CPU_HALT_VECTOR, cpu, APIC_IPI_DEST);
		/* wait for the cpu to be down */
		போது (cpu_down != cpu);
		printf("CPU %d is down\n", cpu);
		cpu_clear_flag(cpu, CPU_IS_READY);
	}

exit_shutdown_aps:
	ioapic_disable_all();

	lapic_disable();

	ncpus = 1; /* hopefully !!! */
	lapic_addr = lapic_eoi_addr = 0;
	திரும்பு;
}

நிலையான வெறுமை ap_finish_booting(வெறுமை)
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

	எனில் (app_cpu_init_timer(system_hz)) {
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

வெறுமை smp_ap_boot(வெறுமை)
{
	switch_k_அடுக்கு((எழுத்து *)get_k_அடுக்கு_உச்சி(__ap_id) -
			X86_அடுக்கு_உச்சி_RESERVED, ap_finish_booting);
}

நிலையான வெறுமை smp_reinit_vars(வெறுமை)
{
	lapic_addr = lapic_eoi_addr = 0;
	ioapic_enabled = 0;

	ncpus = 1;
}

நிலையான வெறுமை tss_init_all(வெறுமை)
{
	unsigned cpu;

	க்கு(cpu = 0; cpu < ncpus ; cpu++)
		tss_init(cpu, get_k_அடுக்கு_உச்சி(cpu)); 
}

நிலையான முழு discover_cpus(வெறுமை)
{
	கட்டமைப்பு acpi_madt_lapic * cpu;

	போது (ncpus < CONFIG_MAX_CPUS && (cpu = acpi_get_lapic_next())) {
		apicid2cpuid[cpu->apic_id] = ncpus;
		cpuid2apicid[ncpus] = cpu->apic_id;
		printf("CPU %3d local APIC id %3d\n", ncpus, cpu->apic_id);
		ncpus++;
	}

	திரும்பு ncpus;
}

வெறுமை smp_init (வெறுமை)
{
	/* read the MP configuration */
	எனில் (!discover_cpus()) {
		ncpus = 1;
		செல் uniproc_fallback;
	}

	lapic_addr = LOCAL_APIC_DEF_ADDR;
	ioapic_enabled = 0;

	tss_init_all();

	/* 
	 * we still run on the boot stack and we cannot use cpuid as its value
	 * wasn't set yet. apicid2cpuid initialized in mps_init()
	 */
	bsp_cpu_id = apicid2cpuid[apicid()];

	எனில் (!lapic_enable(bsp_cpu_id)) {
		printf("ERROR : failed to initialize BSP Local APIC\n");
		செல் uniproc_fallback;
	}

	bsp_lapic_id = apicid();
	
	acpi_init();

	எனில் (!detect_ioapics()) {
		lapic_disable();
		lapic_addr = 0x0;
		செல் uniproc_fallback;
	}

	ioapic_enable_all();
	
	எனில் (ioapic_enabled)
		machine.apic_enabled = 1;

	/* set smp idt entries. */ 
	apic_idt_init(0); /* Not a reset ! */
	idt_reload();

	BOOT_VERBOSE(printf("SMP initialized\n"));

	switch_k_அடுக்கு((எழுத்து *)get_k_அடுக்கு_உச்சி(bsp_cpu_id) -
			X86_அடுக்கு_உச்சி_RESERVED, smp_start_aps);
	
	திரும்பு;

uniproc_fallback:
	apic_idt_init(1); /* Reset to PIC idt ! */
	idt_reload();
	smp_reinit_vars (); /* revert to a single proc system. */
	intr_init(0); /* no auto eoi */
	printf("WARNING : SMP initialization failed\n");
}
	
வெறுமை arch_smp_halt_cpu(வெறுமை)
{
	/* say that we are down */
	cpu_down = cpuid;
	barrier();
	/* unlock the BKL and don't continue */
	BKL_UNLOCK();
	க்கு(;;);
}

வெறுமை arch_send_smp_schedule_ipi(unsigned cpu)
{
	apic_send_ipi(APIC_SMP_SCHED_PROC_VECTOR, cpu, APIC_IPI_DEST);
}
