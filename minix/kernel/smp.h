#अगर_अघोषित __SMP_H__
#घोषणा __SMP_H__

#अगर_घोषित CONFIG_SMP

#अगर_अघोषित __ASSEMBLY__

#समावेश "kernel/kernel.h"
#समावेश "arch_smp.h"
#समावेश "spinlock.h"

/* number of CPUs (execution strands in the system */
EXTERN अचिन्हित ncpus;
/* Number of virtual strands per physical core */
EXTERN अचिन्हित ht_per_core;
/* which cpu is bootstrapping */
EXTERN अचिन्हित bsp_cpu_id;

#घोषणा cpu_is_bsp(cpu)	(bsp_cpu_id == cpu)

/*
 * SMP initialization is largely architecture dependent and each architecture
 * must provide a method how to do it. If initiating SMP fails the function does
 * not report it. However it must put the system in such a state that it falls
 * back to a uniprocessor system. Although the uniprocessor configuration may be
 * suboptimal, the system must be able to run on the bootstrap processor as if
 * it was the only processor in the system
 */
व्योम smp_init(व्योम);

#घोषणा CPU_IS_BSP	1
#घोषणा CPU_IS_READY	2

काष्ठा cpu {
	u32_t flags;
};

EXTERN काष्ठा cpu cpus[CONFIG_MAX_CPUS];

#घोषणा cpu_set_flag(cpu, flag)	करो { cpus[cpu].flags |= (flag); } जबतक(0)
#घोषणा cpu_clear_flag(cpu, flag) करो { cpus[cpu].flags &= ~(flag); } जबतक(0)
#घोषणा cpu_test_flag(cpu, flag) (cpus[cpu].flags & (flag))
#घोषणा cpu_is_ready(cpu) cpu_test_flag(cpu, CPU_IS_READY)

/*
 * Big Kernel Lock prevents more then one cpu executing the kernel code
 */
SPINLOCK_DECLARE(big_kernel_lock)
/*
 * to sync the booting APs
 */
SPINLOCK_DECLARE(boot_lock)
	
व्योम wait_for_APs_to_finish_booting(व्योम);
व्योम ap_boot_finished(अचिन्हित cpu);
व्योम smp_shutdown_aps(व्योम );

/* IPI handlers */
व्योम smp_ipi_halt_handler(व्योम);
व्योम smp_ipi_sched_handler(व्योम);

व्योम smp_schedule(अचिन्हित cpu);
/* stop a processes on a different cpu */
व्योम smp_schedule_stop_proc(काष्ठा proc * p);
/* stop a process on a different cpu because its address space is being changed */
व्योम smp_schedule_vminhibit(काष्ठा proc * p);
/* stop the process and for saving its full context */
व्योम smp_schedule_stop_proc_save_ctx(काष्ठा proc * p);
/* migrate the full context of a process to the destination CPU */
व्योम smp_schedule_migrate_proc(काष्ठा proc * p, अचिन्हित dest_cpu);

व्योम arch_send_smp_schedule_ipi(अचिन्हित cpu);
व्योम arch_smp_halt_cpu(व्योम);

/* deal with x-cpu scheduling event */
व्योम smp_sched_handler(व्योम);

#पूर्ण_अगर /* __ASSEMBLY__ */

#पूर्ण_अगर /* CONFIG_SMP */

#पूर्ण_अगर /* __SMP_H__ */
