#ifndef __SMP_H__
#define __SMP_H__

#ifdef CONFIG_SMP

#ifndef __ASSEMBLY__

#അന്തര്ഭാവ "kernel/kernel.h"
#അന്തര്ഭാവ "arch_smp.h"
#അന്തര്ഭാവ "spinlock.h"

/* number of CPUs (execution strands in the system */
EXTERN unsigned ncpus;
/* Number of virtual strands per physical core */
EXTERN unsigned ht_per_core;
/* which cpu is bootstrapping */
EXTERN unsigned bsp_cpu_id;

#define cpu_is_bsp(cpu)	(bsp_cpu_id == cpu)

/*
 * SMP initialization is largely architecture dependent and each architecture
 * must provide a method how to do it. If initiating SMP fails the function does
 * not report it. However it must put the system in such a state that it falls
 * back to a uniprocessor system. Although the uniprocessor configuration may be
 * suboptimal, the system must be able to run on the bootstrap processor as if
 * it was the only processor in the system
 */
ശൂന്യം smp_init(ശൂന്യം);

#define CPU_IS_BSP	1
#define CPU_IS_READY	2

ഘടന cpu {
	u32_t flags;
};

EXTERN ഘടന cpu cpus[CONFIG_MAX_CPUS];

#define cpu_set_flag(cpu, flag)	ചെയ്യുക { cpus[cpu].flags |= (flag); } ആകുമ്പോൾ(0)
#define cpu_clear_flag(cpu, flag) ചെയ്യുക { cpus[cpu].flags &= ~(flag); } ആകുമ്പോൾ(0)
#define cpu_test_flag(cpu, flag) (cpus[cpu].flags & (flag))
#define cpu_is_ready(cpu) cpu_test_flag(cpu, CPU_IS_READY)

/*
 * Big Kernel Lock prevents more then one cpu executing the kernel code
 */
SPINLOCK_DECLARE(big_kernel_lock)
/*
 * to sync the booting APs
 */
SPINLOCK_DECLARE(boot_lock)
	
ശൂന്യം wait_for_APs_to_finish_booting(ശൂന്യം);
ശൂന്യം ap_boot_finished(unsigned cpu);
ശൂന്യം smp_shutdown_aps(ശൂന്യം );

/* IPI handlers */
ശൂന്യം smp_ipi_halt_handler(ശൂന്യം);
ശൂന്യം smp_ipi_sched_handler(ശൂന്യം);

ശൂന്യം smp_schedule(unsigned cpu);
/* stop a processes on a different cpu */
ശൂന്യം smp_schedule_stop_proc(ഘടന proc * p);
/* stop a process on a different cpu because its address space is being changed */
ശൂന്യം smp_schedule_vminhibit(ഘടന proc * p);
/* stop the process and for saving its full context */
ശൂന്യം smp_schedule_stop_proc_save_ctx(ഘടന proc * p);
/* migrate the full context of a process to the destination CPU */
ശൂന്യം smp_schedule_migrate_proc(ഘടന proc * p, unsigned dest_cpu);

ശൂന്യം arch_send_smp_schedule_ipi(unsigned cpu);
ശൂന്യം arch_smp_halt_cpu(ശൂന്യം);

/* deal with x-cpu scheduling event */
ശൂന്യം smp_sched_handler(ശൂന്യം);

#endif /* __ASSEMBLY__ */

#endif /* CONFIG_SMP */

#endif /* __SMP_H__ */
