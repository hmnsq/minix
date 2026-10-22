#यदि_अपरिभाषितम् __SMP_H__
#परिभाषय __SMP_H__

#यदि_परिभाषितम् CONFIG_SMP

#यदि_अपरिभाषितम् __ASSEMBLY__

#अन्तर्भाव "kernel/kernel.h"
#अन्तर्भाव "arch_smp.h"
#अन्तर्भाव "spinlock.h"

/* number of CPUs (execution strands in the system */
EXTERN अचिह्नित ncpus;
/* Number of virtual strands per physical core */
EXTERN अचिह्नित ht_प्रत्येकम्_core;
/* which cpu is bootstrapping */
EXTERN अचिह्नित bsp_cpu_id;

#परिभाषय cpu_is_bsp(cpu)	(bsp_cpu_id == cpu)

/*
 * SMP initialization is largely architecture dependent and each architecture
 * must provide a method how to do it. If initiating SMP fails the function does
 * not report it. However it must put the system in such a state that it falls
 * back to a uniprocessor system. Although the uniprocessor configuration may be
 * suboptimal, the system must be able to run on the bootstrap processor as if
 * it was the only processor in the system
 */
शून्य smp_init(शून्य);

#परिभाषय CPU_IS_BSP	1
#परिभाषय CPU_IS_READY	2

संरचना cpu {
	u32_t flags;
};

EXTERN संरचना cpu cpus[CONFIG_MAX_CPUS];

#परिभाषय cpu_समुच्चयः_flag(cpu, flag)	कुरु { cpus[cpu].flags |= (flag); } यावत्(0)
#परिभाषय cpu_clear_flag(cpu, flag) कुरु { cpus[cpu].flags &= ~(flag); } यावत्(0)
#परिभाषय cpu_test_flag(cpu, flag) (cpus[cpu].flags & (flag))
#परिभाषय cpu_is_ready(cpu) cpu_test_flag(cpu, CPU_IS_READY)

/*
 * Big Kernel Lock prevents more then one cpu executing the kernel code
 */
SPINLOCK_DECLARE(big_kernel_तालकम्)
/*
 * to sync the booting APs
 */
SPINLOCK_DECLARE(boot_तालकम्)
	
शून्य wait_for_APs_गन्तव्यम्_finish_booting(शून्य);
शून्य ap_boot_परिसमाप्तम्(अचिह्नित cpu);
शून्य smp_shutdown_aps(शून्य );

/* IPI handlers */
शून्य smp_ipi_halt_handler(शून्य);
शून्य smp_ipi_sched_handler(शून्य);

शून्य smp_कालसारणी(अचिह्नित cpu);
/* stop a processes on a different cpu */
शून्य smp_कालसारणी_stop_proc(संरचना proc * p);
/* stop a process on a different cpu because its address space is being changed */
शून्य smp_कालसारणी_vminhibit(संरचना proc * p);
/* stop the process and for saving its full context */
शून्य smp_कालसारणी_stop_proc_save_ctx(संरचना proc * p);
/* migrate the full context of a process to the destination CPU */
शून्य smp_कालसारणी_migrate_proc(संरचना proc * p, अचिह्नित dest_cpu);

शून्य arch_send_smp_कालसारणी_ipi(अचिह्नित cpu);
शून्य arch_smp_halt_cpu(शून्य);

/* deal with x-cpu scheduling event */
शून्य smp_sched_handler(शून्य);

#यदि_अन्तः /* __ASSEMBLY__ */

#यदि_अन्तः /* CONFIG_SMP */

#यदि_अन्तः /* __SMP_H__ */
