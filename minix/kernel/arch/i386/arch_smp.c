/* This file contains essentially the MP handling code of the Minix kernel.
 *
 * Changes:
 *   Apr 1,  2008     Added SMP support.
 */

#घोषणा _SMP

#समावेश <unistd.h>
#समावेश <assert.h>
#समावेश <stdlib.h>
#समावेश <string.h>
#समावेश <machine/cmos.h>
#समावेश <machine/bios.h>

#समावेश "kernel/spinlock.h"
#समावेश "kernel/smp.h"
#समावेश "apic.h"
#समावेश "acpi.h"
#समावेश "kernel/clock.h"

#समावेश "glo.h"

व्योम trampoline(व्योम);

/*
 * arguments for trampoline. We need to pass the logical cpu id, gdt and idt.
 * They have to be in location which is reachable using absolute addressing in
 * 16-bit mode
 */
बाह्य अस्थिर u32_t __ap_id, __ap_pt;
बाह्य अस्थिर काष्ठा desctableptr_s __ap_gdt, __ap_idt;
बाह्य u32_t __ap_gdt_tab, __ap_idt_tab;
बाह्य व्योम * __trampoline_end;

बाह्य u32_t busclock[CONFIG_MAX_CPUS];
बाह्य पूर्णांक panicking;

अटल पूर्णांक अस्थिर ap_cpu_ready;
अटल पूर्णांक अस्थिर cpu_down;

/* there can be at most 255 local APIC ids, each fits in 8 bits */
अटल अचिन्हित अक्षर apicid2cpuid[255];
अचिन्हित अक्षर cpuid2apicid[CONFIG_MAX_CPUS];

SPINLOCK_DEFINE(smp_cpu_lock)
SPINLOCK_DEFINE(dispq_lock)

अटल व्योम smp_reinit_vars(व्योम);

/* These are initialized in protect.c */
बाह्य काष्ठा segdesc_s gdt[GDT_आकार];
बाह्य काष्ठा gatedesc_s idt[IDT_आकार];
बाह्य काष्ठा tss_s tss[CONFIG_MAX_CPUS];
बाह्य पूर्णांक prot_init_done;	/* Indicates they are ready */

अटल phys_bytes trampoline_base;

अटल u32_t ap_lin_addr(व्योम *vaddr)
{
	निश्चित(trampoline_base);
	वापस (u32_t) vaddr - (u32_t) &trampoline + trampoline_base;
}

/*
 * copies the 16-bit AP trampoline code to the first 1M of memory
 */
व्योम copy_trampoline(व्योम)
{
	अचिन्हित tramp_आकार, tramp_start = (अचिन्हित)&trampoline;;

	/* The trampoline code/data is made to be page-aligned. */
	निश्चित(!(tramp_start % I386_PAGE_आकार));

	tramp_आकार = (अचिन्हित) &__trampoline_end - tramp_start;
	trampoline_base = alloc_lowest(&kinfo, tramp_आकार);

	/* The memory allocator finds the lowest available memory.. 
	 * Verify it's low enough
	 */
	निश्चित(trampoline_base + tramp_आकार < (1 << 20));

	/* prepare gdt and idt for the new cpus; make copies
	 * of both the tables and the descriptors of them
	 * in their boot addressing environment.
	 */
	निश्चित(prot_init_done);
	स_नकल(&__ap_gdt_tab, gdt, माप(gdt));
	स_नकल(&__ap_idt_tab, gdt, माप(idt));
	__ap_gdt.base = ap_lin_addr(&__ap_gdt_tab);
	__ap_gdt.limit = माप(gdt)-1;
	__ap_idt.base = ap_lin_addr(&__ap_idt_tab);
	__ap_idt.limit = माप(idt)-1;

	phys_copy((phys_bytes) trampoline, trampoline_base, tramp_आकार);
}

बाह्य पूर्णांक booting_cpu;	/* tell protect.c what to do */

अटल व्योम smp_start_aps(व्योम)
{
	अचिन्हित cpu;
	u32_t biosresetvector;
	phys_bytes __ap_id_phys;
	काष्ठा proc *bootstrap_pt = get_cpulocal_var(ptproc);

	/* TODO hack around the alignment problem */

	phys_copy(0x467, (phys_bytes) &biosresetvector, माप(u32_t));

	/* set the bios shutdown code to 0xA */
	outb(RTC_सूचकांक, 0xF);
	outb(RTC_IO, 0xA);

	निश्चित(bootstrap_pt);
	निश्चित(bootstrap_pt->p_seg.p_cr3);
	__ap_pt  = bootstrap_pt->p_seg.p_cr3;
	निश्चित(__ap_pt);

	copy_trampoline();

	/* New locations for cpu id, pagetable root */
	__ap_id_phys = trampoline_base +
		(phys_bytes) &__ap_id - (phys_bytes)&trampoline;

	/* setup the warm reset vector */
	phys_copy((phys_bytes) &trampoline_base, 0x467, माप(u32_t));

	/* okay, we're ready to go.  boot all of the ap's now.  we loop through
	 * using the processor's apic id values.
	 */
	क्रम (cpu = 0; cpu < ncpus; cpu++) {
		ap_cpu_ready = -1;
		/* Don't send INIT/SIPI to boot cpu.  */
		अगर((apicid() == cpuid2apicid[cpu]) && 
				(apicid() == bsp_lapic_id)) {
			जारी;
		}

		__ap_id = booting_cpu = cpu;
		phys_copy((phys_bytes) &__ap_id, __ap_id_phys, माप(__ap_id));
		mfence();
		अगर (apic_send_init_ipi(cpu, trampoline_base) ||
				apic_send_startup_ipi(cpu, trampoline_base)) {
			म_लिखो("WARNING cannot boot cpu %d\n", cpu);
			जारी;
		}

		/* wait for 5 secs for the processors to boot */
		lapic_set_timer_one_shot(5000000); 

		जबतक (lapic_read(LAPIC_TIMER_CCR)) {
			अगर (ap_cpu_ready == cpu) {
				cpu_set_flag(cpu, CPU_IS_READY);
				अवरोध;
			}
		}
		अगर (ap_cpu_ready == -1) {
			म_लिखो("WARNING : CPU %d didn't boot\n", cpu);
		}
	}

	phys_copy((phys_bytes) &biosresetvector, 0x467, माप(u32_t));

	outb(RTC_सूचकांक, 0xF);
	outb(RTC_IO, 0);

	bsp_finish_booting();
	NOT_REACHABLE;
}

व्योम smp_halt_cpu (व्योम) 
{
	NOT_IMPLEMENTED;
}

व्योम smp_shutdown_aps(व्योम)
{
	अचिन्हित cpu;

	अगर (ncpus == 1)
		जाओ exit_shutdown_aps;

	/* we must let the other cpus enter the kernel mode */
	BKL_UNLOCK();

	क्रम (cpu = 0; cpu < ncpus; cpu++) {
		अगर (cpu == cpuid)
			जारी;
		अगर (!cpu_test_flag(cpu, CPU_IS_READY)) {
			म_लिखो("CPU %d didn't boot\n", cpu);
			जारी;
		}

		cpu_down = -1;
		barrier();
		apic_send_ipi(APIC_SMP_CPU_HALT_VECTOR, cpu, APIC_IPI_DEST);
		/* wait for the cpu to be down */
		जबतक (cpu_down != cpu);
		म_लिखो("CPU %d is down\n", cpu);
		cpu_clear_flag(cpu, CPU_IS_READY);
	}

exit_shutdown_aps:
	ioapic_disable_all();

	lapic_disable();

	ncpus = 1; /* hopefully !!! */
	lapic_addr = lapic_eoi_addr = 0;
	वापस;
}

अटल व्योम ap_finish_booting(व्योम)
{
	अचिन्हित cpu = cpuid;

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

	म_लिखो("CPU %d is up\n", cpu);

	cpu_identify();

	lapic_enable(cpu);
	fpu_init();

	अगर (app_cpu_init_timer(system_hz)) {
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

व्योम smp_ap_boot(व्योम)
{
	switch_k_ढेर((अक्षर *)get_k_ढेर_शीर्ष(__ap_id) -
			X86_ढेर_शीर्ष_RESERVED, ap_finish_booting);
}

अटल व्योम smp_reinit_vars(व्योम)
{
	lapic_addr = lapic_eoi_addr = 0;
	ioapic_enabled = 0;

	ncpus = 1;
}

अटल व्योम tss_init_all(व्योम)
{
	अचिन्हित cpu;

	क्रम(cpu = 0; cpu < ncpus ; cpu++)
		tss_init(cpu, get_k_ढेर_शीर्ष(cpu)); 
}

अटल पूर्णांक discover_cpus(व्योम)
{
	काष्ठा acpi_madt_lapic * cpu;

	जबतक (ncpus < CONFIG_MAX_CPUS && (cpu = acpi_get_lapic_अग्र())) {
		apicid2cpuid[cpu->apic_id] = ncpus;
		cpuid2apicid[ncpus] = cpu->apic_id;
		म_लिखो("CPU %3d local APIC id %3d\n", ncpus, cpu->apic_id);
		ncpus++;
	}

	वापस ncpus;
}

व्योम smp_init (व्योम)
{
	/* read the MP configuration */
	अगर (!discover_cpus()) {
		ncpus = 1;
		जाओ uniproc_fallback;
	}

	lapic_addr = LOCAL_APIC_DEF_ADDR;
	ioapic_enabled = 0;

	tss_init_all();

	/* 
	 * we still run on the boot stack and we cannot use cpuid as its value
	 * wasn't set yet. apicid2cpuid initialized in mps_init()
	 */
	bsp_cpu_id = apicid2cpuid[apicid()];

	अगर (!lapic_enable(bsp_cpu_id)) {
		म_लिखो("ERROR : failed to initialize BSP Local APIC\n");
		जाओ uniproc_fallback;
	}

	bsp_lapic_id = apicid();
	
	acpi_init();

	अगर (!detect_ioapics()) {
		lapic_disable();
		lapic_addr = 0x0;
		जाओ uniproc_fallback;
	}

	ioapic_enable_all();
	
	अगर (ioapic_enabled)
		machine.apic_enabled = 1;

	/* set smp idt entries. */ 
	apic_idt_init(0); /* Not a reset ! */
	idt_reload();

	BOOT_VERBOSE(म_लिखो("SMP initialized\n"));

	switch_k_ढेर((अक्षर *)get_k_ढेर_शीर्ष(bsp_cpu_id) -
			X86_ढेर_शीर्ष_RESERVED, smp_start_aps);
	
	वापस;

uniproc_fallback:
	apic_idt_init(1); /* Reset to PIC idt ! */
	idt_reload();
	smp_reinit_vars (); /* revert to a single proc system. */
	intr_init(0); /* no auto eoi */
	म_लिखो("WARNING : SMP initialization failed\n");
}
	
व्योम arch_smp_halt_cpu(व्योम)
{
	/* say that we are down */
	cpu_down = cpuid;
	barrier();
	/* unlock the BKL and don't continue */
	BKL_UNLOCK();
	क्रम(;;);
}

व्योम arch_send_smp_schedule_ipi(अचिन्हित cpu)
{
	apic_send_ipi(APIC_SMP_SCHED_PROC_VECTOR, cpu, APIC_IPI_DEST);
}
