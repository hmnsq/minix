#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ __SMP_H__
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ __SMP_H__

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_SMP

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ __ASSEMBLY__

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/kernel.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "arch_smp.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "spinlock.h"

/* number of CPUs (execution strands in the system */
EXTERN ਬਿਨਾਨਿਸ਼ਾਨੀ ncpus;
/* Number of virtual strands per physical core */
EXTERN ਬਿਨਾਨਿਸ਼ਾਨੀ ht_ਪ੍ਰਤੀ_core;
/* which cpu is bootstrapping */
EXTERN ਬਿਨਾਨਿਸ਼ਾਨੀ bsp_cpu_id;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ cpu_is_bsp(cpu)	(bsp_cpu_id == cpu)

/*
 * SMP initialization is largely architecture dependent and each architecture
 * must provide a method how to do it. If initiating SMP fails the function does
 * not report it. However it must put the system in such a state that it falls
 * back to a uniprocessor system. Although the uniprocessor configuration may be
 * suboptimal, the system must be able to run on the bootstrap processor as if
 * it was the only processor in the system
 */
ਖਾਲੀ smp_init(ਖਾਲੀ);

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CPU_IS_BSP	1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CPU_IS_READY	2

ਬਣਤਰ cpu {
	u32_t flags;
};

EXTERN ਬਣਤਰ cpu cpus[CONFIG_MAX_CPUS];

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ cpu_ਸੈੱਟ_flag(cpu, flag)	ਕਰੋ { cpus[cpu].flags |= (flag); } ਜਦੋਂ(0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ cpu_clear_flag(cpu, flag) ਕਰੋ { cpus[cpu].flags &= ~(flag); } ਜਦੋਂ(0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ cpu_test_flag(cpu, flag) (cpus[cpu].flags & (flag))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ cpu_is_ready(cpu) cpu_test_flag(cpu, CPU_IS_READY)

/*
 * Big Kernel Lock prevents more then one cpu executing the kernel code
 */
SPINLOCK_DECLARE(big_kernel_ਤਾਲਾ)
/*
 * to sync the booting APs
 */
SPINLOCK_DECLARE(boot_ਤਾਲਾ)
	
ਖਾਲੀ wait_for_APs_ਮੰਜ਼ਿਲ_ਅੰਤ_booting(ਖਾਲੀ);
ਖਾਲੀ ap_boot_ਮੁਕੰਮਲ(ਬਿਨਾਨਿਸ਼ਾਨੀ cpu);
ਖਾਲੀ smp_shutdown_aps(ਖਾਲੀ );

/* IPI handlers */
ਖਾਲੀ smp_ipi_halt_handler(ਖਾਲੀ);
ਖਾਲੀ smp_ipi_sched_handler(ਖਾਲੀ);

ਖਾਲੀ smp_ਸਮਾਂ_ਸਾਰਣੀ(ਬਿਨਾਨਿਸ਼ਾਨੀ cpu);
/* stop a processes on a different cpu */
ਖਾਲੀ smp_ਸਮਾਂ_ਸਾਰਣੀ_stop_proc(ਬਣਤਰ proc * p);
/* stop a process on a different cpu because its address space is being changed */
ਖਾਲੀ smp_ਸਮਾਂ_ਸਾਰਣੀ_vminhibit(ਬਣਤਰ proc * p);
/* stop the process and for saving its full context */
ਖਾਲੀ smp_ਸਮਾਂ_ਸਾਰਣੀ_stop_proc_save_ctx(ਬਣਤਰ proc * p);
/* migrate the full context of a process to the destination CPU */
ਖਾਲੀ smp_ਸਮਾਂ_ਸਾਰਣੀ_migrate_proc(ਬਣਤਰ proc * p, ਬਿਨਾਨਿਸ਼ਾਨੀ dest_cpu);

ਖਾਲੀ arch_send_smp_ਸਮਾਂ_ਸਾਰਣੀ_ipi(ਬਿਨਾਨਿਸ਼ਾਨੀ cpu);
ਖਾਲੀ arch_smp_halt_cpu(ਖਾਲੀ);

/* deal with x-cpu scheduling event */
ਖਾਲੀ smp_sched_handler(ਖਾਲੀ);

#ਜੇ_ਅੰਤ /* __ASSEMBLY__ */

#ਜੇ_ਅੰਤ /* CONFIG_SMP */

#ਜੇ_ਅੰਤ /* __SMP_H__ */
