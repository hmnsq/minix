#جے_بنا_تعریف __SMP_H__
#تعریف_کرو __SMP_H__

#جے_تعریف_شدہ CONFIG_SMP

#جے_بنا_تعریف __ASSEMBLY__

#ضمن "kernel/kernel.h"
#ضمن "arch_smp.h"
#ضمن "spinlock.h"

/* number of CPUs (execution strands in the system */
EXTERN بنانشانی ncpus;
/* Number of virtual strands per physical core */
EXTERN بنانشانی ht_فی_core;
/* which cpu is bootstrapping */
EXTERN بنانشانی bsp_cpu_id;

#تعریف_کرو cpu_is_bsp(cpu)	(bsp_cpu_id == cpu)

/*
 * SMP initialization is largely architecture dependent and each architecture
 * must provide a method how to do it. If initiating SMP fails the function does
 * not report it. However it must put the system in such a state that it falls
 * back to a uniprocessor system. Although the uniprocessor configuration may be
 * suboptimal, the system must be able to run on the bootstrap processor as if
 * it was the only processor in the system
 */
فراغ smp_init(فراغ);

#تعریف_کرو CPU_IS_BSP	1
#تعریف_کرو CPU_IS_READY	2

بنية cpu {
	u32_t flags;
};

EXTERN بنية cpu cpus[CONFIG_MAX_CPUS];

#تعریف_کرو cpu_سیٹ_flag(cpu, flag)	افعل { cpus[cpu].flags |= (flag); } طالما(0)
#تعریف_کرو cpu_clear_flag(cpu, flag) افعل { cpus[cpu].flags &= ~(flag); } طالما(0)
#تعریف_کرو cpu_test_flag(cpu, flag) (cpus[cpu].flags & (flag))
#تعریف_کرو cpu_is_ready(cpu) cpu_test_flag(cpu, CPU_IS_READY)

/*
 * Big Kernel Lock prevents more then one cpu executing the kernel code
 */
SPINLOCK_DECLARE(big_kernel_جندرا)
/*
 * to sync the booting APs
 */
SPINLOCK_DECLARE(boot_جندرا)
	
فراغ wait_for_APs_منزل_انت_booting(فراغ);
فراغ ap_boot_مکمل(بنانشانی cpu);
فراغ smp_shutdown_aps(فراغ );

/* IPI handlers */
فراغ smp_ipi_halt_handler(فراغ);
فراغ smp_ipi_sched_handler(فراغ);

فراغ smp_ویلا_سارنی(بنانشانی cpu);
/* stop a processes on a different cpu */
فراغ smp_ویلا_سارنی_stop_proc(بنية proc * p);
/* stop a process on a different cpu because its address space is being changed */
فراغ smp_ویلا_سارنی_vminhibit(بنية proc * p);
/* stop the process and for saving its full context */
فراغ smp_ویلا_سارنی_stop_proc_save_ctx(بنية proc * p);
/* migrate the full context of a process to the destination CPU */
فراغ smp_ویلا_سارنی_migrate_proc(بنية proc * p, بنانشانی dest_cpu);

فراغ arch_send_smp_ویلا_سارنی_ipi(بنانشانی cpu);
فراغ arch_smp_halt_cpu(فراغ);

/* deal with x-cpu scheduling event */
فراغ smp_sched_handler(فراغ);

#جے_انت /* __ASSEMBLY__ */

#جے_انت /* CONFIG_SMP */

#جے_انت /* __SMP_H__ */
