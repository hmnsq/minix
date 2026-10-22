/* This file contains essentially the MP handling code of the Minix kernel.
 *
 * Changes:
 *   Apr 1,  2008     Added SMP support.
 */

#define _SMP

#अन्तर्भाव <unistd.h>
#अन्तर्भाव <assert.h>
#अन्तर्भाव <stdlib.h>
#अन्तर्भाव <string.h>
#अन्तर्भाव <machine/cmos.h>
#अन्तर्भाव <machine/bios.h>

#अन्तर्भाव "kernel/spinlock.h"
#अन्तर्भाव "kernel/smp.h"
#अन्तर्भाव "apic.h"
#अन्तर्भाव "acpi.h"
#अन्तर्भाव "kernel/clock.h"

#अन्तर्भाव "glo.h"

शून्य trampoline(शून्य);

/*
 * arguments for trampoline. We need to pass the logical cpu id, gdt and idt.
 * They have to be in location which is reachable using absolute addressing in
 * 16-bit mode
 */
बाह्य चञ्चल u32_t __ap_id, __ap_pt;
बाह्य चञ्चल संरचना desctableptr_s __ap_gdt, __ap_idt;
बाह्य u32_t __ap_gdt_tab, __ap_idt_tab;
बाह्य शून्य * __trampoline_end;

बाह्य u32_t busclock[CONFIG_MAX_CPUS];
बाह्य पूर्णाङ्क panicking;

स्थिर पूर्णाङ्क चञ्चल ap_cpu_ready;
स्थिर पूर्णाङ्क चञ्चल cpu_down;

/* there can be at most 255 local APIC ids, each fits in 8 bits */
स्थिर unsigned वर्ण apicid2cpuid[255];
unsigned वर्ण cpuid2apicid[CONFIG_MAX_CPUS];

SPINLOCK_DEFINE(smp_cpu_lock)
SPINLOCK_DEFINE(dispq_lock)

स्थिर शून्य smp_reinit_vars(शून्य);

/* These are initialized in protect.c */
बाह्य संरचना segdesc_s gdt[GDT_SIZE];
बाह्य संरचना gatedesc_s idt[IDT_SIZE];
बाह्य संरचना tss_s tss[CONFIG_MAX_CPUS];
बाह्य पूर्णाङ्क prot_init_done;	/* Indicates they are ready */

स्थिर phys_bytes trampoline_base;

स्थिर u32_t ap_lin_addr(शून्य *vaddr)
{
	प्रतिज्ञा(trampoline_base);
	प्रत्यागम (u32_t) vaddr - (u32_t) &trampoline + trampoline_base;
}

/*
 * copies the 16-bit AP trampoline code to the first 1M of memory
 */
शून्य copy_trampoline(शून्य)
{
	unsigned tramp_size, tramp_start = (unsigned)&trampoline;;

	/* The trampoline code/data is made to be page-aligned. */
	प्रतिज्ञा(!(tramp_start % I386_PAGE_SIZE));

	tramp_size = (unsigned) &__trampoline_end - tramp_start;
	trampoline_base = alloc_lowest(&kinfo, tramp_size);

	/* The memory allocator finds the lowest available memory.. 
	 * Verify it's low enough
	 */
	प्रतिज्ञा(trampoline_base + tramp_size < (1 << 20));

	/* prepare gdt and idt for the new cpus; make copies
	 * of both the tables and the descriptors of them
	 * in their boot addressing environment.
	 */
	प्रतिज्ञा(prot_init_done);
	memcpy(&__ap_gdt_tab, gdt, परिमाण(gdt));
	memcpy(&__ap_idt_tab, gdt, परिमाण(idt));
	__ap_gdt.base = ap_lin_addr(&__ap_gdt_tab);
	__ap_gdt.limit = परिमाण(gdt)-1;
	__ap_idt.base = ap_lin_addr(&__ap_idt_tab);
	__ap_idt.limit = परिमाण(idt)-1;

	phys_copy((phys_bytes) trampoline, trampoline_base, tramp_size);
}

बाह्य पूर्णाङ्क booting_cpu;	/* tell protect.c what to do */

स्थिर शून्य smp_start_aps(शून्य)
{
	unsigned cpu;
	u32_t biosresetvector;
	phys_bytes __ap_id_phys;
	संरचना proc *bootstrap_pt = get_cpulocal_var(ptproc);

	/* TODO hack around the alignment problem */

	phys_copy(0x467, (phys_bytes) &biosresetvector, परिमाण(u32_t));

	/* set the bios shutdown code to 0xA */
	outb(RTC_INDEX, 0xF);
	outb(RTC_IO, 0xA);

	प्रतिज्ञा(bootstrap_pt);
	प्रतिज्ञा(bootstrap_pt->p_seg.p_cr3);
	__ap_pt  = bootstrap_pt->p_seg.p_cr3;
	प्रतिज्ञा(__ap_pt);

	copy_trampoline();

	/* New locations for cpu id, pagetable root */
	__ap_id_phys = trampoline_base +
		(phys_bytes) &__ap_id - (phys_bytes)&trampoline;

	/* setup the warm reset vector */
	phys_copy((phys_bytes) &trampoline_base, 0x467, परिमाण(u32_t));

	/* okay, we're ready to go.  boot all of the ap's now.  we loop through
	 * using the processor's apic id values.
	 */
	प्रति (cpu = 0; cpu < ncpus; cpu++) {
		ap_cpu_ready = -1;
		/* Don't send INIT/SIPI to boot cpu.  */
		यदि((apicid() == cpuid2apicid[cpu]) && 
				(apicid() == bsp_lapic_id)) {
			अनुवर्त;
		}

		__ap_id = booting_cpu = cpu;
		phys_copy((phys_bytes) &__ap_id, __ap_id_phys, परिमाण(__ap_id));
		mfence();
		यदि (apic_send_init_ipi(cpu, trampoline_base) ||
				apic_send_startup_ipi(cpu, trampoline_base)) {
			printf("WARNING cannot boot cpu %d\n", cpu);
			अनुवर्त;
		}

		/* wait for 5 secs for the processors to boot */
		lapic_set_timer_one_shot(5000000); 

		यावत् (lapic_read(LAPIC_TIMER_CCR)) {
			यदि (ap_cpu_ready == cpu) {
				cpu_set_flag(cpu, CPU_IS_READY);
				भङ्ग;
			}
		}
		यदि (ap_cpu_ready == -1) {
			printf("WARNING : CPU %d didn't boot\n", cpu);
		}
	}

	phys_copy((phys_bytes) &biosresetvector, 0x467, परिमाण(u32_t));

	outb(RTC_INDEX, 0xF);
	outb(RTC_IO, 0);

	bsp_finish_booting();
	NOT_REACHABLE;
}

शून्य smp_halt_cpu (शून्य) 
{
	NOT_IMPLEMENTED;
}

शून्य smp_shutdown_aps(शून्य)
{
	unsigned cpu;

	यदि (ncpus == 1)
		गच्छ exit_shutdown_aps;

	/* we must let the other cpus enter the kernel mode */
	BKL_UNLOCK();

	प्रति (cpu = 0; cpu < ncpus; cpu++) {
		यदि (cpu == cpuid)
			अनुवर्त;
		यदि (!cpu_test_flag(cpu, CPU_IS_READY)) {
			printf("CPU %d didn't boot\n", cpu);
			अनुवर्त;
		}

		cpu_down = -1;
		barrier();
		apic_send_ipi(APIC_SMP_CPU_HALT_VECTOR, cpu, APIC_IPI_DEST);
		/* wait for the cpu to be down */
		यावत् (cpu_down != cpu);
		printf("CPU %d is down\n", cpu);
		cpu_clear_flag(cpu, CPU_IS_READY);
	}

exit_shutdown_aps:
	ioapic_disable_all();

	lapic_disable();

	ncpus = 1; /* hopefully !!! */
	lapic_addr = lapic_eoi_addr = 0;
	प्रत्यागम;
}

स्थिर शून्य ap_finish_booting(शून्य)
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

	यदि (app_cpu_init_timer(system_hz)) {
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

शून्य smp_ap_boot(शून्य)
{
	switch_k_चळत((वर्ण *)get_k_चळत_शीर्ष(__ap_id) -
			X86_चळत_शीर्ष_RESERVED, ap_finish_booting);
}

स्थिर शून्य smp_reinit_vars(शून्य)
{
	lapic_addr = lapic_eoi_addr = 0;
	ioapic_enabled = 0;

	ncpus = 1;
}

स्थिर शून्य tss_init_all(शून्य)
{
	unsigned cpu;

	प्रति(cpu = 0; cpu < ncpus ; cpu++)
		tss_init(cpu, get_k_चळत_शीर्ष(cpu)); 
}

स्थिर पूर्णाङ्क discover_cpus(शून्य)
{
	संरचना acpi_madt_lapic * cpu;

	यावत् (ncpus < CONFIG_MAX_CPUS && (cpu = acpi_get_lapic_next())) {
		apicid2cpuid[cpu->apic_id] = ncpus;
		cpuid2apicid[ncpus] = cpu->apic_id;
		printf("CPU %3d local APIC id %3d\n", ncpus, cpu->apic_id);
		ncpus++;
	}

	प्रत्यागम ncpus;
}

शून्य smp_init (शून्य)
{
	/* read the MP configuration */
	यदि (!discover_cpus()) {
		ncpus = 1;
		गच्छ uniproc_fallback;
	}

	lapic_addr = LOCAL_APIC_DEF_ADDR;
	ioapic_enabled = 0;

	tss_init_all();

	/* 
	 * we still run on the boot stack and we cannot use cpuid as its value
	 * wasn't set yet. apicid2cpuid initialized in mps_init()
	 */
	bsp_cpu_id = apicid2cpuid[apicid()];

	यदि (!lapic_enable(bsp_cpu_id)) {
		printf("ERROR : failed to initialize BSP Local APIC\n");
		गच्छ uniproc_fallback;
	}

	bsp_lapic_id = apicid();
	
	acpi_init();

	यदि (!detect_ioapics()) {
		lapic_disable();
		lapic_addr = 0x0;
		गच्छ uniproc_fallback;
	}

	ioapic_enable_all();
	
	यदि (ioapic_enabled)
		machine.apic_enabled = 1;

	/* set smp idt entries. */ 
	apic_idt_init(0); /* Not a reset ! */
	idt_reload();

	BOOT_VERBOSE(printf("SMP initialized\n"));

	switch_k_चळत((वर्ण *)get_k_चळत_शीर्ष(bsp_cpu_id) -
			X86_चळत_शीर्ष_RESERVED, smp_start_aps);
	
	प्रत्यागम;

uniproc_fallback:
	apic_idt_init(1); /* Reset to PIC idt ! */
	idt_reload();
	smp_reinit_vars (); /* revert to a single proc system. */
	intr_init(0); /* no auto eoi */
	printf("WARNING : SMP initialization failed\n");
}
	
शून्य arch_smp_halt_cpu(शून्य)
{
	/* say that we are down */
	cpu_down = cpuid;
	barrier();
	/* unlock the BKL and don't continue */
	BKL_UNLOCK();
	प्रति(;;);
}

शून्य arch_send_smp_schedule_ipi(unsigned cpu)
{
	apic_send_ipi(APIC_SMP_SCHED_PROC_VECTOR, cpu, APIC_IPI_DEST);
}
