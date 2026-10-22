#ifndef __SMP_H__
#define __SMP_H__

#ifdef CONFIG_SMP

#ifndef __ASSEMBLY__

#અન્તર્ભાવ "kernel/kernel.h"
#અન્તર્ભાવ "arch_smp.h"
#અન્તર્ભાવ "spinlock.h"

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
શૂન્ય smp_init(શૂન્ય);

#define CPU_IS_BSP	1
#define CPU_IS_READY	2

સંરચના cpu {
	u32_t flags;
};

EXTERN સંરચના cpu cpus[CONFIG_MAX_CPUS];

#define cpu_set_flag(cpu, flag)	કરો { cpus[cpu].flags |= (flag); } જ્યારે(0)
#define cpu_clear_flag(cpu, flag) કરો { cpus[cpu].flags &= ~(flag); } જ્યારે(0)
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
	
શૂન્ય wait_for_APs_to_finish_booting(શૂન્ય);
શૂન્ય ap_boot_finished(unsigned cpu);
શૂન્ય smp_shutdown_aps(શૂન્ય );

/* IPI handlers */
શૂન્ય smp_ipi_halt_handler(શૂન્ય);
શૂન્ય smp_ipi_sched_handler(શૂન્ય);

શૂન્ય smp_schedule(unsigned cpu);
/* stop a processes on a different cpu */
શૂન્ય smp_schedule_stop_proc(સંરચના proc * p);
/* stop a process on a different cpu because its address space is being changed */
શૂન્ય smp_schedule_vminhibit(સંરચના proc * p);
/* stop the process and for saving its full context */
શૂન્ય smp_schedule_stop_proc_save_ctx(સંરચના proc * p);
/* migrate the full context of a process to the destination CPU */
શૂન્ય smp_schedule_migrate_proc(સંરચના proc * p, unsigned dest_cpu);

શૂન્ય arch_send_smp_schedule_ipi(unsigned cpu);
શૂન્ય arch_smp_halt_cpu(શૂન્ય);

/* deal with x-cpu scheduling event */
શૂન્ય smp_sched_handler(શૂન્ય);

#endif /* __ASSEMBLY__ */

#endif /* CONFIG_SMP */

#endif /* __SMP_H__ */
