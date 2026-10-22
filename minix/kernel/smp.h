#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ __SMP_H__
#ವ್ಯಾಖ್ಯಾನಿಸು __SMP_H__

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_SMP

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ __ASSEMBLY__

#ಅನ್ತರ್ಭಾವ "kernel/kernel.h"
#ಅನ್ತರ್ಭಾವ "arch_smp.h"
#ಅನ್ತರ್ಭಾವ "spinlock.h"

/* number of CPUs (execution strands in the system */
EXTERN ಚಿಹ್ನೆರಹಿತ ncpus;
/* Number of virtual strands per physical core */
EXTERN ಚಿಹ್ನೆರಹಿತ ht_ಪ್ರತಿ_core;
/* which cpu is bootstrapping */
EXTERN ಚಿಹ್ನೆರಹಿತ bsp_cpu_id;

#ವ್ಯಾಖ್ಯಾನಿಸು cpu_is_bsp(cpu)	(bsp_cpu_id == cpu)

/*
 * SMP initialization is largely architecture dependent and each architecture
 * must provide a method how to do it. If initiating SMP fails the function does
 * not report it. However it must put the system in such a state that it falls
 * back to a uniprocessor system. Although the uniprocessor configuration may be
 * suboptimal, the system must be able to run on the bootstrap processor as if
 * it was the only processor in the system
 */
ಶೂನ್ಯ smp_init(ಶೂನ್ಯ);

#ವ್ಯಾಖ್ಯಾನಿಸು CPU_IS_BSP	1
#ವ್ಯಾಖ್ಯಾನಿಸು CPU_IS_READY	2

ರಚನೆ cpu {
	u32_t flags;
};

EXTERN ರಚನೆ cpu cpus[CONFIG_MAX_CPUS];

#ವ್ಯಾಖ್ಯಾನಿಸು cpu_ಗಣ_flag(cpu, flag)	ಮಾಡು { cpus[cpu].flags |= (flag); } ಆಗ(0)
#ವ್ಯಾಖ್ಯಾನಿಸು cpu_clear_flag(cpu, flag) ಮಾಡು { cpus[cpu].flags &= ~(flag); } ಆಗ(0)
#ವ್ಯಾಖ್ಯಾನಿಸು cpu_test_flag(cpu, flag) (cpus[cpu].flags & (flag))
#ವ್ಯಾಖ್ಯಾನಿಸು cpu_is_ready(cpu) cpu_test_flag(cpu, CPU_IS_READY)

/*
 * Big Kernel Lock prevents more then one cpu executing the kernel code
 */
SPINLOCK_DECLARE(big_kernel_ಬೀಗ)
/*
 * to sync the booting APs
 */
SPINLOCK_DECLARE(boot_ಬೀಗ)
	
ಶೂನ್ಯ wait_for_APs_ಗಮ್ಯ_ಮುಕ್ತಾಯ_booting(ಶೂನ್ಯ);
ಶೂನ್ಯ ap_boot_ಪೂರ್ಣಗೊಂಡ(ಚಿಹ್ನೆರಹಿತ cpu);
ಶೂನ್ಯ smp_shutdown_aps(ಶೂನ್ಯ );

/* IPI handlers */
ಶೂನ್ಯ smp_ipi_halt_handler(ಶೂನ್ಯ);
ಶೂನ್ಯ smp_ipi_sched_handler(ಶೂನ್ಯ);

ಶೂನ್ಯ smp_ವೇಳಾಪಟ್ಟಿ(ಚಿಹ್ನೆರಹಿತ cpu);
/* stop a processes on a different cpu */
ಶೂನ್ಯ smp_ವೇಳಾಪಟ್ಟಿ_stop_proc(ರಚನೆ proc * p);
/* stop a process on a different cpu because its address space is being changed */
ಶೂನ್ಯ smp_ವೇಳಾಪಟ್ಟಿ_vminhibit(ರಚನೆ proc * p);
/* stop the process and for saving its full context */
ಶೂನ್ಯ smp_ವೇಳಾಪಟ್ಟಿ_stop_proc_save_ctx(ರಚನೆ proc * p);
/* migrate the full context of a process to the destination CPU */
ಶೂನ್ಯ smp_ವೇಳಾಪಟ್ಟಿ_migrate_proc(ರಚನೆ proc * p, ಚಿಹ್ನೆರಹಿತ dest_cpu);

ಶೂನ್ಯ arch_send_smp_ವೇಳಾಪಟ್ಟಿ_ipi(ಚಿಹ್ನೆರಹಿತ cpu);
ಶೂನ್ಯ arch_smp_halt_cpu(ಶೂನ್ಯ);

/* deal with x-cpu scheduling event */
ಶೂನ್ಯ smp_sched_handler(ಶೂನ್ಯ);

#ಆದರೆ_ಅಂತ್ಯ /* __ASSEMBLY__ */

#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_SMP */

#ಆದರೆ_ಅಂತ್ಯ /* __SMP_H__ */
