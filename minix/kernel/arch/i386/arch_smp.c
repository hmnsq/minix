/* This file contains essentially the MP handling code of the Minix kernel.
 *
 * Changes:
 *   Apr 1,  2008     Added SMP support.
 */

#define _SMP

#ضمن <unistd.h>
#ضمن <assert.h>
#ضمن <stdlib.h>
#ضمن <string.h>
#ضمن <machine/cmos.h>
#ضمن <machine/bios.h>

#ضمن "kernel/spinlock.h"
#ضمن "kernel/smp.h"
#ضمن "apic.h"
#ضمن "acpi.h"
#ضمن "kernel/clock.h"

#ضمن "glo.h"

فراغ trampoline(فراغ);

/*
 * arguments for trampoline. We need to pass the logical cpu id, gdt and idt.
 * They have to be in location which is reachable using absolute addressing in
 * 16-bit mode
 */
خارجي متطاير u32_t __ap_id, __ap_pt;
خارجي متطاير بنية desctableptr_s __ap_gdt, __ap_idt;
خارجي u32_t __ap_gdt_tab, __ap_idt_tab;
خارجي فراغ * __trampoline_end;

خارجي u32_t busclock[CONFIG_MAX_CPUS];
خارجي صحيح panicking;

ساكن صحيح متطاير ap_cpu_ready;
ساكن صحيح متطاير cpu_down;

/* there can be at most 255 local APIC ids, each fits in 8 bits */
ساكن unsigned حرف apicid2cpuid[255];
unsigned حرف cpuid2apicid[CONFIG_MAX_CPUS];

SPINLOCK_DEFINE(smp_cpu_lock)
SPINLOCK_DEFINE(dispq_lock)

ساكن فراغ smp_reinit_vars(فراغ);

/* These are initialized in protect.c */
خارجي بنية segdesc_s gdt[GDT_SIZE];
خارجي بنية gatedesc_s idt[IDT_SIZE];
خارجي بنية tss_s tss[CONFIG_MAX_CPUS];
خارجي صحيح prot_init_done;	/* Indicates they are ready */

ساكن phys_bytes trampoline_base;

ساكن u32_t ap_lin_addr(فراغ *vaddr)
{
	أكد(trampoline_base);
	أرجع (u32_t) vaddr - (u32_t) &trampoline + trampoline_base;
}

/*
 * copies the 16-bit AP trampoline code to the first 1M of memory
 */
فراغ copy_trampoline(فراغ)
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
	memcpy(&__ap_gdt_tab, gdt, حجم(gdt));
	memcpy(&__ap_idt_tab, gdt, حجم(idt));
	__ap_gdt.base = ap_lin_addr(&__ap_gdt_tab);
	__ap_gdt.limit = حجم(gdt)-1;
	__ap_idt.base = ap_lin_addr(&__ap_idt_tab);
	__ap_idt.limit = حجم(idt)-1;

	phys_copy((phys_bytes) trampoline, trampoline_base, tramp_size);
}

خارجي صحيح booting_cpu;	/* tell protect.c what to do */

ساكن فراغ smp_start_aps(فراغ)
{
	unsigned cpu;
	u32_t biosresetvector;
	phys_bytes __ap_id_phys;
	بنية proc *bootstrap_pt = get_cpulocal_var(ptproc);

	/* TODO hack around the alignment problem */

	phys_copy(0x467, (phys_bytes) &biosresetvector, حجم(u32_t));

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
	phys_copy((phys_bytes) &trampoline_base, 0x467, حجم(u32_t));

	/* okay, we're ready to go.  boot all of the ap's now.  we loop through
	 * using the processor's apic id values.
	 */
	لكل (cpu = 0; cpu < ncpus; cpu++) {
		ap_cpu_ready = -1;
		/* Don't send INIT/SIPI to boot cpu.  */
		𐤀𐤌((apicid() == cpuid2apicid[cpu]) && 
				(apicid() == bsp_lapic_id)) {
			استمر;
		}

		__ap_id = booting_cpu = cpu;
		phys_copy((phys_bytes) &__ap_id, __ap_id_phys, حجم(__ap_id));
		mfence();
		𐤀𐤌 (apic_send_init_ipi(cpu, trampoline_base) ||
				apic_send_startup_ipi(cpu, trampoline_base)) {
			printf("WARNING cannot boot cpu %d\n", cpu);
			استمر;
		}

		/* wait for 5 secs for the processors to boot */
		lapic_set_timer_one_shot(5000000); 

		طالما (lapic_read(LAPIC_TIMER_CCR)) {
			𐤀𐤌 (ap_cpu_ready == cpu) {
				cpu_set_flag(cpu, CPU_IS_READY);
				اكسر;
			}
		}
		𐤀𐤌 (ap_cpu_ready == -1) {
			printf("WARNING : CPU %d didn't boot\n", cpu);
		}
	}

	phys_copy((phys_bytes) &biosresetvector, 0x467, حجم(u32_t));

	outb(RTC_INDEX, 0xF);
	outb(RTC_IO, 0);

	bsp_finish_booting();
	NOT_REACHABLE;
}

فراغ smp_halt_cpu (فراغ) 
{
	NOT_IMPLEMENTED;
}

فراغ smp_shutdown_aps(فراغ)
{
	unsigned cpu;

	𐤀𐤌 (ncpus == 1)
		اذهب exit_shutdown_aps;

	/* we must let the other cpus enter the kernel mode */
	BKL_UNLOCK();

	لكل (cpu = 0; cpu < ncpus; cpu++) {
		𐤀𐤌 (cpu == cpuid)
			استمر;
		𐤀𐤌 (!cpu_test_flag(cpu, CPU_IS_READY)) {
			printf("CPU %d didn't boot\n", cpu);
			استمر;
		}

		cpu_down = -1;
		barrier();
		apic_send_ipi(APIC_SMP_CPU_HALT_VECTOR, cpu, APIC_IPI_DEST);
		/* wait for the cpu to be down */
		طالما (cpu_down != cpu);
		printf("CPU %d is down\n", cpu);
		cpu_clear_flag(cpu, CPU_IS_READY);
	}

exit_shutdown_aps:
	ioapic_disable_all();

	lapic_disable();

	ncpus = 1; /* hopefully !!! */
	lapic_addr = lapic_eoi_addr = 0;
	أرجع;
}

ساكن فراغ ap_finish_booting(فراغ)
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

	𐤀𐤌 (app_cpu_init_timer(system_hz)) {
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

فراغ smp_ap_boot(فراغ)
{
	switch_k_stack((حرف *)get_k_stack_top(__ap_id) -
			X86_STACK_TOP_RESERVED, ap_finish_booting);
}

ساكن فراغ smp_reinit_vars(فراغ)
{
	lapic_addr = lapic_eoi_addr = 0;
	ioapic_enabled = 0;

	ncpus = 1;
}

ساكن فراغ tss_init_all(فراغ)
{
	unsigned cpu;

	لكل(cpu = 0; cpu < ncpus ; cpu++)
		tss_init(cpu, get_k_stack_top(cpu)); 
}

ساكن صحيح discover_cpus(فراغ)
{
	بنية acpi_madt_lapic * cpu;

	طالما (ncpus < CONFIG_MAX_CPUS && (cpu = acpi_get_lapic_next())) {
		apicid2cpuid[cpu->apic_id] = ncpus;
		cpuid2apicid[ncpus] = cpu->apic_id;
		printf("CPU %3d local APIC id %3d\n", ncpus, cpu->apic_id);
		ncpus++;
	}

	أرجع ncpus;
}

فراغ smp_init (فراغ)
{
	/* read the MP configuration */
	𐤀𐤌 (!discover_cpus()) {
		ncpus = 1;
		اذهب uniproc_fallback;
	}

	lapic_addr = LOCAL_APIC_DEF_ADDR;
	ioapic_enabled = 0;

	tss_init_all();

	/* 
	 * we still run on the boot stack and we cannot use cpuid as its value
	 * wasn't set yet. apicid2cpuid initialized in mps_init()
	 */
	bsp_cpu_id = apicid2cpuid[apicid()];

	𐤀𐤌 (!lapic_enable(bsp_cpu_id)) {
		printf("ERROR : failed to initialize BSP Local APIC\n");
		اذهب uniproc_fallback;
	}

	bsp_lapic_id = apicid();
	
	acpi_init();

	𐤀𐤌 (!detect_ioapics()) {
		lapic_disable();
		lapic_addr = 0x0;
		اذهب uniproc_fallback;
	}

	ioapic_enable_all();
	
	𐤀𐤌 (ioapic_enabled)
		machine.apic_enabled = 1;

	/* set smp idt entries. */ 
	apic_idt_init(0); /* Not a reset ! */
	idt_reload();

	BOOT_VERBOSE(printf("SMP initialized\n"));

	switch_k_stack((حرف *)get_k_stack_top(bsp_cpu_id) -
			X86_STACK_TOP_RESERVED, smp_start_aps);
	
	أرجع;

uniproc_fallback:
	apic_idt_init(1); /* Reset to PIC idt ! */
	idt_reload();
	smp_reinit_vars (); /* revert to a single proc system. */
	intr_init(0); /* no auto eoi */
	printf("WARNING : SMP initialization failed\n");
}
	
فراغ arch_smp_halt_cpu(فراغ)
{
	/* say that we are down */
	cpu_down = cpuid;
	barrier();
	/* unlock the BKL and don't continue */
	BKL_UNLOCK();
	لكل(;;);
}

فراغ arch_send_smp_schedule_ipi(unsigned cpu)
{
	apic_send_ipi(APIC_SMP_SCHED_PROC_VECTOR, cpu, APIC_IPI_DEST);
}
