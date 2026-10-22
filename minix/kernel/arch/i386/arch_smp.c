/* This file contains essentially the MP handling code of the Minix kernel.
 *
 * Changes:
 *   Apr 1,  2008     Added SMP support.
 */

#تعریف_کرو _SMP

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

خالی trampoline(خالی);

/*
 * arguments for trampoline. We need to pass the logical cpu id, gdt and idt.
 * They have to be in location which is reachable using absolute addressing in
 * 16-bit mode
 */
خارجی متلون u32_t __ap_id, __ap_pt;
خارجی متلون ساخت desctableptr_s __ap_gdt, __ap_idt;
خارجی u32_t __ap_gdt_tab, __ap_idt_tab;
خارجی خالی * __trampoline_end;

خارجی u32_t busclock[CONFIG_MAX_CPUS];
خارجی صحیح panicking;

ساکن صحیح متلون ap_cpu_ready;
ساکن صحیح متلون cpu_نیچے_کو;

/* there can be at most 255 local APIC ids, each fits in 8 bits */
ساکن غیرعلامتی حرف apicid2cpuid[255];
غیرعلامتی حرف cpuid2apicid[CONFIG_MAX_CPUS];

SPINLOCK_DEFINE(smp_cpu_تالا)
SPINLOCK_DEFINE(dispq_تالا)

ساکن خالی smp_reinit_vars(خالی);

/* These are initialized in protect.c */
خارجی ساخت segdesc_s gdt[GDT_سائز];
خارجی ساخت gatedesc_s idt[IDT_سائز];
خارجی ساخت tss_s tss[CONFIG_MAX_CPUS];
خارجی صحیح prot_init_مکمل;	/* Indicates they are ready */

ساکن phys_بائٹیں trampoline_بنیاد;

ساکن u32_t ap_lin_addr(خالی *vaddr)
{
	أكد(trampoline_بنیاد);
	واپس (u32_t) vaddr - (u32_t) &trampoline + trampoline_بنیاد;
}

/*
 * copies the 16-bit AP trampoline code to the first 1M of memory
 */
خالی copy_trampoline(خالی)
{
	غیرعلامتی tramp_سائز, tramp_آغاز = (غیرعلامتی)&trampoline;;

	/* The trampoline code/data is made to be page-aligned. */
	أكد(!(tramp_آغاز % I386_صفحہ_سائز));

	tramp_سائز = (غیرعلامتی) &__trampoline_end - tramp_آغاز;
	trampoline_بنیاد = alloc_سب_سے_نیچا(&kinfo, tramp_سائز);

	/* The memory allocator finds the lowest available memory.. 
	 * Verify it's low enough
	 */
	أكد(trampoline_بنیاد + tramp_سائز < (1 << 20));

	/* prepare gdt and idt for the new cpus; make copies
	 * of both the tables and the descriptors of them
	 * in their boot addressing environment.
	 */
	أكد(prot_init_مکمل);
	حافظہ_نقل(&__ap_gdt_tab, gdt, حجم(gdt));
	حافظہ_نقل(&__ap_idt_tab, gdt, حجم(idt));
	__ap_gdt.بنیاد = ap_lin_addr(&__ap_gdt_tab);
	__ap_gdt.حد_بندی_2 = حجم(gdt)-1;
	__ap_idt.بنیاد = ap_lin_addr(&__ap_idt_tab);
	__ap_idt.حد_بندی_2 = حجم(idt)-1;

	phys_copy((phys_بائٹیں) trampoline, trampoline_بنیاد, tramp_سائز);
}

خارجی صحیح booting_cpu;	/* tell protect.c what to do */

ساکن خالی smp_آغاز_aps(خالی)
{
	غیرعلامتی cpu;
	u32_t biosresetvector;
	phys_بائٹیں __ap_id_phys;
	ساخت proc *bootstrap_pt = get_cpulocal_var(ptproc);

	/* TODO hack around the alignment problem */

	phys_copy(0x467, (phys_بائٹیں) &biosresetvector, حجم(u32_t));

	/* set the bios shutdown code to 0xA */
	outb(RTC_اشاریہ, 0xF);
	outb(RTC_IO, 0xA);

	أكد(bootstrap_pt);
	أكد(bootstrap_pt->p_seg.p_cr3);
	__ap_pt  = bootstrap_pt->p_seg.p_cr3;
	أكد(__ap_pt);

	copy_trampoline();

	/* New locations for cpu id, pagetable root */
	__ap_id_phys = trampoline_بنیاد +
		(phys_بائٹیں) &__ap_id - (phys_بائٹیں)&trampoline;

	/* setup the warm reset vector */
	phys_copy((phys_بائٹیں) &trampoline_بنیاد, 0x467, حجم(u32_t));

	/* okay, we're ready to go.  boot all of the ap's now.  we loop through
	 * using the processor's apic id values.
	 */
	برائے (cpu = 0; cpu < ncpus; cpu++) {
		ap_cpu_ready = -1;
		/* Don't send INIT/SIPI to boot cpu.  */
		اگر((apicid() == cpuid2apicid[cpu]) && 
				(apicid() == bsp_lapic_id)) {
			جاری;
		}

		__ap_id = booting_cpu = cpu;
		phys_copy((phys_بائٹیں) &__ap_id, __ap_id_phys, حجم(__ap_id));
		mfence();
		اگر (apic_send_init_ipi(cpu, trampoline_بنیاد) ||
				apic_send_startup_ipi(cpu, trampoline_بنیاد)) {
			لکھو("WARNING cannot boot cpu %d\n", cpu);
			جاری;
		}

		/* wait for 5 secs for the processors to boot */
		lapic_سیٹ_timer_one_shot(5000000); 

		جب (lapic_read(LAPIC_TIMER_CCR)) {
			اگر (ap_cpu_ready == cpu) {
				cpu_سیٹ_flag(cpu, CPU_IS_READY);
				توڑو;
			}
		}
		اگر (ap_cpu_ready == -1) {
			لکھو("WARNING : CPU %d didn't boot\n", cpu);
		}
	}

	phys_copy((phys_بائٹیں) &biosresetvector, 0x467, حجم(u32_t));

	outb(RTC_اشاریہ, 0xF);
	outb(RTC_IO, 0);

	bsp_اختتام_booting();
	NOT_REACHABLE;
}

خالی smp_halt_cpu (خالی) 
{
	NOT_IMPLEMENTED;
}

خالی smp_shutdown_aps(خالی)
{
	غیرعلامتی cpu;

	اگر (ncpus == 1)
		جاؤ exit_shutdown_aps;

	/* we must let the other cpus enter the kernel mode */
	BKL_UNLOCK();

	برائے (cpu = 0; cpu < ncpus; cpu++) {
		اگر (cpu == cpuid)
			جاری;
		اگر (!cpu_test_flag(cpu, CPU_IS_READY)) {
			لکھو("CPU %d didn't boot\n", cpu);
			جاری;
		}

		cpu_نیچے_کو = -1;
		barrier();
		apic_send_ipi(APIC_SMP_CPU_HALT_سمتیہ, cpu, APIC_IPI_DEST);
		/* wait for the cpu to be down */
		جب (cpu_نیچے_کو != cpu);
		لکھو("CPU %d is down\n", cpu);
		cpu_clear_flag(cpu, CPU_IS_READY);
	}

exit_shutdown_aps:
	ioapic_disable_all();

	lapic_disable();

	ncpus = 1; /* hopefully !!! */
	lapic_addr = lapic_eoi_addr = 0;
	واپس;
}

ساکن خالی ap_اختتام_booting(خالی)
{
	غیرعلامتی cpu = cpuid;

	/* inform the world of our presence. */
	ap_cpu_ready = cpu;

	/*
	 * Finish processor initialisation.  CPUs must be excluded from running.
	 * lapic timer calibration locks and unlocks the BKL because of the
	 * nested interrupts used for calibration. Therefore BKL is not good
	 * enough, the boot_lock must be held.
	 */
	spinlock_تالا(&boot_تالا);
	BKL_تالا();

	لکھو("CPU %d is up\n", cpu);

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

	ap_boot_ختم_شدہ(cpu);
	spinlock_unlock(&boot_تالا);

	switch_مقصد_user();
	NOT_REACHABLE;
}

خالی smp_ap_boot(خالی)
{
	switch_k_ڈھیر((حرف *)get_k_ڈھیر_چوٹی(__ap_id) -
			X86_ڈھیر_چوٹی_RESERVED, ap_اختتام_booting);
}

ساکن خالی smp_reinit_vars(خالی)
{
	lapic_addr = lapic_eoi_addr = 0;
	ioapic_enabled = 0;

	ncpus = 1;
}

ساکن خالی tss_init_all(خالی)
{
	غیرعلامتی cpu;

	برائے(cpu = 0; cpu < ncpus ; cpu++)
		tss_init(cpu, get_k_ڈھیر_چوٹی(cpu)); 
}

ساکن صحیح discover_cpus(خالی)
{
	ساخت acpi_madt_lapic * cpu;

	جب (ncpus < CONFIG_MAX_CPUS && (cpu = acpi_get_lapic_اگلا())) {
		apicid2cpuid[cpu->apic_id] = ncpus;
		cpuid2apicid[ncpus] = cpu->apic_id;
		لکھو("CPU %3d local APIC id %3d\n", ncpus, cpu->apic_id);
		ncpus++;
	}

	واپس ncpus;
}

خالی smp_init (خالی)
{
	/* read the MP configuration */
	اگر (!discover_cpus()) {
		ncpus = 1;
		جاؤ uniproc_fallback;
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
		لکھو("ERROR : failed to initialize BSP Local APIC\n");
		جاؤ uniproc_fallback;
	}

	bsp_lapic_id = apicid();
	
	acpi_init();

	اگر (!detect_ioapics()) {
		lapic_disable();
		lapic_addr = 0x0;
		جاؤ uniproc_fallback;
	}

	ioapic_enable_all();
	
	اگر (ioapic_enabled)
		machine.apic_enabled = 1;

	/* set smp idt entries. */ 
	apic_idt_init(0); /* Not a reset ! */
	idt_reload();

	BOOT_VERBOSE(لکھو("SMP initialized\n"));

	switch_k_ڈھیر((حرف *)get_k_ڈھیر_چوٹی(bsp_cpu_id) -
			X86_ڈھیر_چوٹی_RESERVED, smp_آغاز_aps);
	
	واپس;

uniproc_fallback:
	apic_idt_init(1); /* Reset to PIC idt ! */
	idt_reload();
	smp_reinit_vars (); /* revert to a single proc system. */
	intr_init(0); /* no auto eoi */
	لکھو("WARNING : SMP initialization failed\n");
}
	
خالی arch_smp_halt_cpu(خالی)
{
	/* say that we are down */
	cpu_نیچے_کو = cpuid;
	barrier();
	/* unlock the BKL and don't continue */
	BKL_UNLOCK();
	برائے(;;);
}

خالی arch_send_smp_جدول_وقت_ipi(غیرعلامتی cpu)
{
	apic_send_ipi(APIC_SMP_SCHED_PROC_سمتیہ, cpu, APIC_IPI_DEST);
}
