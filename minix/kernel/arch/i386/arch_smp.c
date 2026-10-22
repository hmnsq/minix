/* This file contains essentially the MP handling code of the Minix kernel.
 *
 * Changes:
 *   Apr 1,  2008     Added SMP support.
 */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _SMP

#ਅਨ੍ਤਰ੍ਭਾਵ <unistd.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <assert.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdlib.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <machine/cmos.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <machine/bios.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/spinlock.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/smp.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "apic.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "acpi.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/clock.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "glo.h"

ਖਾਲੀ trampoline(ਖਾਲੀ);

/*
 * arguments for trampoline. We need to pass the logical cpu id, gdt and idt.
 * They have to be in location which is reachable using absolute addressing in
 * 16-bit mode
 */
ਬਾਹਰੀ ਅਸਥਿਰ u32_t __ap_id, __ap_pt;
ਬਾਹਰੀ ਅਸਥਿਰ ਬਣਤਰ desctableptr_s __ap_gdt, __ap_idt;
ਬਾਹਰੀ u32_t __ap_gdt_tab, __ap_idt_tab;
ਬਾਹਰੀ ਖਾਲੀ * __trampoline_end;

ਬਾਹਰੀ u32_t busclock[CONFIG_MAX_CPUS];
ਬਾਹਰੀ ਪੂਰਨ panicking;

ਸਥਿਰ ਪੂਰਨ ਅਸਥਿਰ ap_cpu_ready;
ਸਥਿਰ ਪੂਰਨ ਅਸਥਿਰ cpu_ਥੱਲੇ;

/* there can be at most 255 local APIC ids, each fits in 8 bits */
ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ apicid2cpuid[255];
ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ cpuid2apicid[CONFIG_MAX_CPUS];

SPINLOCK_DEFINE(smp_cpu_ਤਾਲਾ)
SPINLOCK_DEFINE(dispq_ਤਾਲਾ)

ਸਥਿਰ ਖਾਲੀ smp_reinit_vars(ਖਾਲੀ);

/* These are initialized in protect.c */
ਬਾਹਰੀ ਬਣਤਰ segdesc_s gdt[GDT_ਮਾਪ];
ਬਾਹਰੀ ਬਣਤਰ gatedesc_s idt[IDT_ਮਾਪ];
ਬਾਹਰੀ ਬਣਤਰ tss_s tss[CONFIG_MAX_CPUS];
ਬਾਹਰੀ ਪੂਰਨ prot_init_ਪੂਰਾ;	/* Indicates they are ready */

ਸਥਿਰ phys_ਬਾਈਟਾਂ trampoline_ਅਧਾਰ;

ਸਥਿਰ u32_t ap_lin_addr(ਖਾਲੀ *vaddr)
{
	ਪ੍ਰਤਿਜ੍ਞਾ(trampoline_ਅਧਾਰ);
	ਵਾਪਸ (u32_t) vaddr - (u32_t) &trampoline + trampoline_ਅਧਾਰ;
}

/*
 * copies the 16-bit AP trampoline code to the first 1M of memory
 */
ਖਾਲੀ copy_trampoline(ਖਾਲੀ)
{
	ਬਿਨਾਨਿਸ਼ਾਨੀ tramp_ਮਾਪ, tramp_ਸ਼ੁਰੂ = (ਬਿਨਾਨਿਸ਼ਾਨੀ)&trampoline;;

	/* The trampoline code/data is made to be page-aligned. */
	ਪ੍ਰਤਿਜ੍ਞਾ(!(tramp_ਸ਼ੁਰੂ % I386_ਪੰਨਾ_ਮਾਪ));

	tramp_ਮਾਪ = (ਬਿਨਾਨਿਸ਼ਾਨੀ) &__trampoline_end - tramp_ਸ਼ੁਰੂ;
	trampoline_ਅਧਾਰ = alloc_ਸਭ_ਤੋਂ_ਨੀਵਾਂ(&kinfo, tramp_ਮਾਪ);

	/* The memory allocator finds the lowest available memory.. 
	 * Verify it's low enough
	 */
	ਪ੍ਰਤਿਜ੍ਞਾ(trampoline_ਅਧਾਰ + tramp_ਮਾਪ < (1 << 20));

	/* prepare gdt and idt for the new cpus; make copies
	 * of both the tables and the descriptors of them
	 * in their boot addressing environment.
	 */
	ਪ੍ਰਤਿਜ੍ਞਾ(prot_init_ਪੂਰਾ);
	ਸਮ੍ਰਿਤੀ_ਨਕਲ(&__ap_gdt_tab, gdt, ਆਕਾਰ(gdt));
	ਸਮ੍ਰਿਤੀ_ਨਕਲ(&__ap_idt_tab, gdt, ਆਕਾਰ(idt));
	__ap_gdt.ਅਧਾਰ = ap_lin_addr(&__ap_gdt_tab);
	__ap_gdt.ਹੱਦਬੰਦੀ_2 = ਆਕਾਰ(gdt)-1;
	__ap_idt.ਅਧਾਰ = ap_lin_addr(&__ap_idt_tab);
	__ap_idt.ਹੱਦਬੰਦੀ_2 = ਆਕਾਰ(idt)-1;

	phys_copy((phys_ਬਾਈਟਾਂ) trampoline, trampoline_ਅਧਾਰ, tramp_ਮਾਪ);
}

ਬਾਹਰੀ ਪੂਰਨ booting_cpu;	/* tell protect.c what to do */

ਸਥਿਰ ਖਾਲੀ smp_ਸ਼ੁਰੂ_aps(ਖਾਲੀ)
{
	ਬਿਨਾਨਿਸ਼ਾਨੀ cpu;
	u32_t biosresetvector;
	phys_ਬਾਈਟਾਂ __ap_id_phys;
	ਬਣਤਰ proc *bootstrap_pt = get_cpulocal_var(ptproc);

	/* TODO hack around the alignment problem */

	phys_copy(0x467, (phys_ਬਾਈਟਾਂ) &biosresetvector, ਆਕਾਰ(u32_t));

	/* set the bios shutdown code to 0xA */
	outb(RTC_ਸੂਚਕਾਂਕ, 0xF);
	outb(RTC_IO, 0xA);

	ਪ੍ਰਤਿਜ੍ਞਾ(bootstrap_pt);
	ਪ੍ਰਤਿਜ੍ਞਾ(bootstrap_pt->p_seg.p_cr3);
	__ap_pt  = bootstrap_pt->p_seg.p_cr3;
	ਪ੍ਰਤਿਜ੍ਞਾ(__ap_pt);

	copy_trampoline();

	/* New locations for cpu id, pagetable root */
	__ap_id_phys = trampoline_ਅਧਾਰ +
		(phys_ਬਾਈਟਾਂ) &__ap_id - (phys_ਬਾਈਟਾਂ)&trampoline;

	/* setup the warm reset vector */
	phys_copy((phys_ਬਾਈਟਾਂ) &trampoline_ਅਧਾਰ, 0x467, ਆਕਾਰ(u32_t));

	/* okay, we're ready to go.  boot all of the ap's now.  we loop through
	 * using the processor's apic id values.
	 */
	ਲਈ (cpu = 0; cpu < ncpus; cpu++) {
		ap_cpu_ready = -1;
		/* Don't send INIT/SIPI to boot cpu.  */
		ਜੇ((apicid() == cpuid2apicid[cpu]) && 
				(apicid() == bsp_lapic_id)) {
			ਜਾਰੀ;
		}

		__ap_id = booting_cpu = cpu;
		phys_copy((phys_ਬਾਈਟਾਂ) &__ap_id, __ap_id_phys, ਆਕਾਰ(__ap_id));
		mfence();
		ਜੇ (apic_send_init_ipi(cpu, trampoline_ਅਧਾਰ) ||
				apic_send_startup_ipi(cpu, trampoline_ਅਧਾਰ)) {
			ਲਿਖੋ("WARNING cannot boot cpu %d\n", cpu);
			ਜਾਰੀ;
		}

		/* wait for 5 secs for the processors to boot */
		lapic_ਸੈੱਟ_timer_one_shot(5000000); 

		ਜਦੋਂ (lapic_read(LAPIC_TIMER_CCR)) {
			ਜੇ (ap_cpu_ready == cpu) {
				cpu_ਸੈੱਟ_flag(cpu, CPU_IS_READY);
				ਰੋਕੋ;
			}
		}
		ਜੇ (ap_cpu_ready == -1) {
			ਲਿਖੋ("WARNING : CPU %d didn't boot\n", cpu);
		}
	}

	phys_copy((phys_ਬਾਈਟਾਂ) &biosresetvector, 0x467, ਆਕਾਰ(u32_t));

	outb(RTC_ਸੂਚਕਾਂਕ, 0xF);
	outb(RTC_IO, 0);

	bsp_ਅੰਤ_booting();
	NOT_REACHABLE;
}

ਖਾਲੀ smp_halt_cpu (ਖਾਲੀ) 
{
	NOT_IMPLEMENTED;
}

ਖਾਲੀ smp_shutdown_aps(ਖਾਲੀ)
{
	ਬਿਨਾਨਿਸ਼ਾਨੀ cpu;

	ਜੇ (ncpus == 1)
		ਜਾਓ exit_shutdown_aps;

	/* we must let the other cpus enter the kernel mode */
	BKL_UNLOCK();

	ਲਈ (cpu = 0; cpu < ncpus; cpu++) {
		ਜੇ (cpu == cpuid)
			ਜਾਰੀ;
		ਜੇ (!cpu_test_flag(cpu, CPU_IS_READY)) {
			ਲਿਖੋ("CPU %d didn't boot\n", cpu);
			ਜਾਰੀ;
		}

		cpu_ਥੱਲੇ = -1;
		barrier();
		apic_send_ipi(APIC_SMP_CPU_HALT_ਸਦਿਸ਼, cpu, APIC_IPI_DEST);
		/* wait for the cpu to be down */
		ਜਦੋਂ (cpu_ਥੱਲੇ != cpu);
		ਲਿਖੋ("CPU %d is down\n", cpu);
		cpu_clear_flag(cpu, CPU_IS_READY);
	}

exit_shutdown_aps:
	ioapic_disable_all();

	lapic_disable();

	ncpus = 1; /* hopefully !!! */
	lapic_addr = lapic_eoi_addr = 0;
	ਵਾਪਸ;
}

ਸਥਿਰ ਖਾਲੀ ap_ਅੰਤ_booting(ਖਾਲੀ)
{
	ਬਿਨਾਨਿਸ਼ਾਨੀ cpu = cpuid;

	/* inform the world of our presence. */
	ap_cpu_ready = cpu;

	/*
	 * Finish processor initialisation.  CPUs must be excluded from running.
	 * lapic timer calibration locks and unlocks the BKL because of the
	 * nested interrupts used for calibration. Therefore BKL is not good
	 * enough, the boot_lock must be held.
	 */
	spinlock_ਤਾਲਾ(&boot_ਤਾਲਾ);
	BKL_ਤਾਲਾ();

	ਲਿਖੋ("CPU %d is up\n", cpu);

	cpu_identify();

	lapic_enable(cpu);
	fpu_init();

	ਜੇ (app_cpu_init_timer(system_hz)) {
		panic("FATAL : failed to initialize timer interrupts CPU %d, "
				"cannot continue without any clock source!", cpu);
	}

	/* FIXME assign CPU local idle structure */
	get_cpulocal_var(proc_ptr) = get_cpulocal_var_ptr(idle_proc);
	get_cpulocal_var(bill_ptr) = get_cpulocal_var_ptr(idle_proc);

	ap_boot_ਮੁਕੰਮਲ(cpu);
	spinlock_unlock(&boot_ਤਾਲਾ);

	switch_ਮੰਜ਼ਿਲ_user();
	NOT_REACHABLE;
}

ਖਾਲੀ smp_ap_boot(ਖਾਲੀ)
{
	switch_k_ਢੇਰ((ਅੱਖਰ *)get_k_ਢੇਰ_ਸਿਖਰ(__ap_id) -
			X86_ਢੇਰ_ਸਿਖਰ_RESERVED, ap_ਅੰਤ_booting);
}

ਸਥਿਰ ਖਾਲੀ smp_reinit_vars(ਖਾਲੀ)
{
	lapic_addr = lapic_eoi_addr = 0;
	ioapic_enabled = 0;

	ncpus = 1;
}

ਸਥਿਰ ਖਾਲੀ tss_init_all(ਖਾਲੀ)
{
	ਬਿਨਾਨਿਸ਼ਾਨੀ cpu;

	ਲਈ(cpu = 0; cpu < ncpus ; cpu++)
		tss_init(cpu, get_k_ਢੇਰ_ਸਿਖਰ(cpu)); 
}

ਸਥਿਰ ਪੂਰਨ discover_cpus(ਖਾਲੀ)
{
	ਬਣਤਰ acpi_madt_lapic * cpu;

	ਜਦੋਂ (ncpus < CONFIG_MAX_CPUS && (cpu = acpi_get_lapic_ਅਗਲਾ())) {
		apicid2cpuid[cpu->apic_id] = ncpus;
		cpuid2apicid[ncpus] = cpu->apic_id;
		ਲਿਖੋ("CPU %3d local APIC id %3d\n", ncpus, cpu->apic_id);
		ncpus++;
	}

	ਵਾਪਸ ncpus;
}

ਖਾਲੀ smp_init (ਖਾਲੀ)
{
	/* read the MP configuration */
	ਜੇ (!discover_cpus()) {
		ncpus = 1;
		ਜਾਓ uniproc_fallback;
	}

	lapic_addr = LOCAL_APIC_DEF_ADDR;
	ioapic_enabled = 0;

	tss_init_all();

	/* 
	 * we still run on the boot stack and we cannot use cpuid as its value
	 * wasn't set yet. apicid2cpuid initialized in mps_init()
	 */
	bsp_cpu_id = apicid2cpuid[apicid()];

	ਜੇ (!lapic_enable(bsp_cpu_id)) {
		ਲਿਖੋ("ERROR : failed to initialize BSP Local APIC\n");
		ਜਾਓ uniproc_fallback;
	}

	bsp_lapic_id = apicid();
	
	acpi_init();

	ਜੇ (!detect_ioapics()) {
		lapic_disable();
		lapic_addr = 0x0;
		ਜਾਓ uniproc_fallback;
	}

	ioapic_enable_all();
	
	ਜੇ (ioapic_enabled)
		machine.apic_enabled = 1;

	/* set smp idt entries. */ 
	apic_idt_init(0); /* Not a reset ! */
	idt_reload();

	BOOT_VERBOSE(ਲਿਖੋ("SMP initialized\n"));

	switch_k_ਢੇਰ((ਅੱਖਰ *)get_k_ਢੇਰ_ਸਿਖਰ(bsp_cpu_id) -
			X86_ਢੇਰ_ਸਿਖਰ_RESERVED, smp_ਸ਼ੁਰੂ_aps);
	
	ਵਾਪਸ;

uniproc_fallback:
	apic_idt_init(1); /* Reset to PIC idt ! */
	idt_reload();
	smp_reinit_vars (); /* revert to a single proc system. */
	intr_init(0); /* no auto eoi */
	ਲਿਖੋ("WARNING : SMP initialization failed\n");
}
	
ਖਾਲੀ arch_smp_halt_cpu(ਖਾਲੀ)
{
	/* say that we are down */
	cpu_ਥੱਲੇ = cpuid;
	barrier();
	/* unlock the BKL and don't continue */
	BKL_UNLOCK();
	ਲਈ(;;);
}

ਖਾਲੀ arch_send_smp_ਸਮਾਂ_ਸਾਰਣੀ_ipi(ਬਿਨਾਨਿਸ਼ਾਨੀ cpu)
{
	apic_send_ipi(APIC_SMP_SCHED_PROC_ਸਦਿਸ਼, cpu, APIC_IPI_DEST);
}
