/* This file contains essentially the MP handling code of the Minix kernel.
 *
 * Changes:
 *   Apr 1,  2008     Added SMP support.
 */

#define _SMP

#شامل <unistd.h>
#شامل <assert.h>
#شامل <stdlib.h>
#شامل <string.h>
#شامل <machine/cmos.h>
#شامل <machine/bios.h>

#شامل "kernel/spinlock.h"
#شامل "kernel/smp.h"
#شامل "apic.h"
#شامل "acpi.h"
#شامل "kernel/clock.h"

#شامل "glo.h"

تهی trampoline(تهی);

/*
 * arguments for trampoline. We need to pass the logical cpu id, gdt and idt.
 * They have to be in location which is reachable using absolute addressing in
 * 16-bit mode
 */
بیرونی فرار u32_t __ap_id, __ap_pt;
بیرونی فرار ساختار desctableptr_s __ap_gdt, __ap_idt;
بیرونی u32_t __ap_gdt_tab, __ap_idt_tab;
بیرونی تهی * __trampoline_end;

بیرونی u32_t busclock[CONFIG_MAX_CPUS];
بیرونی صحیح panicking;

ایستا صحیح فرار ap_cpu_ready;
ایستا صحیح فرار cpu_down;

/* there can be at most 255 local APIC ids, each fits in 8 bits */
ایستا unsigned نویسه apicid2cpuid[255];
unsigned نویسه cpuid2apicid[CONFIG_MAX_CPUS];

SPINLOCK_DEFINE(smp_cpu_lock)
SPINLOCK_DEFINE(dispq_lock)

ایستا تهی smp_reinit_vars(تهی);

/* These are initialized in protect.c */
بیرونی ساختار segdesc_s gdt[GDT_SIZE];
بیرونی ساختار gatedesc_s idt[IDT_SIZE];
بیرونی ساختار tss_s tss[CONFIG_MAX_CPUS];
بیرونی صحیح prot_init_done;	/* Indicates they are ready */

ایستا phys_bytes trampoline_base;

ایستا u32_t ap_lin_addr(تهی *vaddr)
{
	أكد(trampoline_base);
	بازگشت (u32_t) vaddr - (u32_t) &trampoline + trampoline_base;
}

/*
 * copies the 16-bit AP trampoline code to the first 1M of memory
 */
تهی copy_trampoline(تهی)
{
	unsigned tramp_size, tramp_start = (unsigned)&trampoline;;

	/* The trampoline code/data is made to be page-aligned. */
	أكد(!(tramp_start % I386_PAGE_SIZE));

	tramp_size = (unsigned) &__trampoline_end - tramp_start;
	trampoline_base = alloc_lowest(&kinfo, tramp_size);

	/* The memory allocator finds the lowest available memory.. 
	 * Verify it's low enough
	 */
	أكد(trampoline_base + tramp_size < (1 << 20));

	/* prepare gdt and idt for the new cpus; make copies
	 * of both the tables and the descriptors of them
	 * in their boot addressing environment.
	 */
	أكد(prot_init_done);
	memcpy(&__ap_gdt_tab, gdt, اندازه(gdt));
	memcpy(&__ap_idt_tab, gdt, اندازه(idt));
	__ap_gdt.base = ap_lin_addr(&__ap_gdt_tab);
	__ap_gdt.limit = اندازه(gdt)-1;
	__ap_idt.base = ap_lin_addr(&__ap_idt_tab);
	__ap_idt.limit = اندازه(idt)-1;

	phys_copy((phys_bytes) trampoline, trampoline_base, tramp_size);
}

بیرونی صحیح booting_cpu;	/* tell protect.c what to do */

ایستا تهی smp_start_aps(تهی)
{
	unsigned cpu;
	u32_t biosresetvector;
	phys_bytes __ap_id_phys;
	ساختار proc *bootstrap_pt = get_cpulocal_var(ptproc);

	/* TODO hack around the alignment problem */

	phys_copy(0x467, (phys_bytes) &biosresetvector, اندازه(u32_t));

	/* set the bios shutdown code to 0xA */
	outb(RTC_INDEX, 0xF);
	outb(RTC_IO, 0xA);

	أكد(bootstrap_pt);
	أكد(bootstrap_pt->p_seg.p_cr3);
	__ap_pt  = bootstrap_pt->p_seg.p_cr3;
	أكد(__ap_pt);

	copy_trampoline();

	/* New locations for cpu id, pagetable root */
	__ap_id_phys = trampoline_base +
		(phys_bytes) &__ap_id - (phys_bytes)&trampoline;

	/* setup the warm reset vector */
	phys_copy((phys_bytes) &trampoline_base, 0x467, اندازه(u32_t));

	/* okay, we're ready to go.  boot all of the ap's now.  we loop through
	 * using the processor's apic id values.
	 */
	برای (cpu = 0; cpu < ncpus; cpu++) {
		ap_cpu_ready = -1;
		/* Don't send INIT/SIPI to boot cpu.  */
		اگر((apicid() == cpuid2apicid[cpu]) && 
				(apicid() == bsp_lapic_id)) {
			ادامه;
		}

		__ap_id = booting_cpu = cpu;
		phys_copy((phys_bytes) &__ap_id, __ap_id_phys, اندازه(__ap_id));
		mfence();
		اگر (apic_send_init_ipi(cpu, trampoline_base) ||
				apic_send_startup_ipi(cpu, trampoline_base)) {
			printf("WARNING cannot boot cpu %d\n", cpu);
			ادامه;
		}

		/* wait for 5 secs for the processors to boot */
		lapic_set_timer_one_shot(5000000); 

		وقتی (lapic_read(LAPIC_TIMER_CCR)) {
			اگر (ap_cpu_ready == cpu) {
				cpu_set_flag(cpu, CPU_IS_READY);
				بشکن;
			}
		}
		اگر (ap_cpu_ready == -1) {
			printf("WARNING : CPU %d didn't boot\n", cpu);
		}
	}

	phys_copy((phys_bytes) &biosresetvector, 0x467, اندازه(u32_t));

	outb(RTC_INDEX, 0xF);
	outb(RTC_IO, 0);

	bsp_finish_booting();
	NOT_REACHABLE;
}

تهی smp_halt_cpu (تهی) 
{
	NOT_IMPLEMENTED;
}

تهی smp_shutdown_aps(تهی)
{
	unsigned cpu;

	اگر (ncpus == 1)
		برو exit_shutdown_aps;

	/* we must let the other cpus enter the kernel mode */
	BKL_UNLOCK();

	برای (cpu = 0; cpu < ncpus; cpu++) {
		اگر (cpu == cpuid)
			ادامه;
		اگر (!cpu_test_flag(cpu, CPU_IS_READY)) {
			printf("CPU %d didn't boot\n", cpu);
			ادامه;
		}

		cpu_down = -1;
		barrier();
		apic_send_ipi(APIC_SMP_CPU_HALT_VECTOR, cpu, APIC_IPI_DEST);
		/* wait for the cpu to be down */
		وقتی (cpu_down != cpu);
		printf("CPU %d is down\n", cpu);
		cpu_clear_flag(cpu, CPU_IS_READY);
	}

exit_shutdown_aps:
	ioapic_disable_all();

	lapic_disable();

	ncpus = 1; /* hopefully !!! */
	lapic_addr = lapic_eoi_addr = 0;
	بازگشت;
}

ایستا تهی ap_finish_booting(تهی)
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

	اگر (app_cpu_init_timer(system_hz)) {
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

تهی smp_ap_boot(تهی)
{
	switch_k_پشته((نویسه *)get_k_پشته_بالاترین(__ap_id) -
			X86_پشته_بالاترین_RESERVED, ap_finish_booting);
}

ایستا تهی smp_reinit_vars(تهی)
{
	lapic_addr = lapic_eoi_addr = 0;
	ioapic_enabled = 0;

	ncpus = 1;
}

ایستا تهی tss_init_all(تهی)
{
	unsigned cpu;

	برای(cpu = 0; cpu < ncpus ; cpu++)
		tss_init(cpu, get_k_پشته_بالاترین(cpu)); 
}

ایستا صحیح discover_cpus(تهی)
{
	ساختار acpi_madt_lapic * cpu;

	وقتی (ncpus < CONFIG_MAX_CPUS && (cpu = acpi_get_lapic_next())) {
		apicid2cpuid[cpu->apic_id] = ncpus;
		cpuid2apicid[ncpus] = cpu->apic_id;
		printf("CPU %3d local APIC id %3d\n", ncpus, cpu->apic_id);
		ncpus++;
	}

	بازگشت ncpus;
}

تهی smp_init (تهی)
{
	/* read the MP configuration */
	اگر (!discover_cpus()) {
		ncpus = 1;
		برو uniproc_fallback;
	}

	lapic_addr = LOCAL_APIC_DEF_ADDR;
	ioapic_enabled = 0;

	tss_init_all();

	/* 
	 * we still run on the boot stack and we cannot use cpuid as its value
	 * wasn't set yet. apicid2cpuid initialized in mps_init()
	 */
	bsp_cpu_id = apicid2cpuid[apicid()];

	اگر (!lapic_enable(bsp_cpu_id)) {
		printf("ERROR : failed to initialize BSP Local APIC\n");
		برو uniproc_fallback;
	}

	bsp_lapic_id = apicid();
	
	acpi_init();

	اگر (!detect_ioapics()) {
		lapic_disable();
		lapic_addr = 0x0;
		برو uniproc_fallback;
	}

	ioapic_enable_all();
	
	اگر (ioapic_enabled)
		machine.apic_enabled = 1;

	/* set smp idt entries. */ 
	apic_idt_init(0); /* Not a reset ! */
	idt_reload();

	BOOT_VERBOSE(printf("SMP initialized\n"));

	switch_k_پشته((نویسه *)get_k_پشته_بالاترین(bsp_cpu_id) -
			X86_پشته_بالاترین_RESERVED, smp_start_aps);
	
	بازگشت;

uniproc_fallback:
	apic_idt_init(1); /* Reset to PIC idt ! */
	idt_reload();
	smp_reinit_vars (); /* revert to a single proc system. */
	intr_init(0); /* no auto eoi */
	printf("WARNING : SMP initialization failed\n");
}
	
تهی arch_smp_halt_cpu(تهی)
{
	/* say that we are down */
	cpu_down = cpuid;
	barrier();
	/* unlock the BKL and don't continue */
	BKL_UNLOCK();
	برای(;;);
}

تهی arch_send_smp_schedule_ipi(unsigned cpu)
{
	apic_send_ipi(APIC_SMP_SCHED_PROC_VECTOR, cpu, APIC_IPI_DEST);
}
