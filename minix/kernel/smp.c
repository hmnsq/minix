#అన్తర్భావ <assert.h>

#అన్తర్భావ "smp.h"
#అన్తర్భావ "interrupt.h"
#అన్తర్భావ "clock.h"

గుర్తులేని ncpus;
గుర్తులేని ht_చొప్పున_core;
గుర్తులేని bsp_cpu_id;

నిర్మాణం cpu cpus[CONFIG_MAX_CPUS];

/* info passed to another cpu along with a sched ipi */
నిర్మాణం sched_ipi_దత్తాంశం {
	అస్థిర u32_t	flags;
	అస్థిర u32_t	దత్తాంశం;
};

స్థిర నిర్మాణం sched_ipi_దత్తాంశం  sched_ipi_దత్తాంశం[CONFIG_MAX_CPUS];

#నిర్వచించు SCHED_IPI_STOP_PROC	1
#నిర్వచించు SCHED_IPI_VM_INHIBIT	2
#నిర్వచించు SCHED_IPI_SAVE_CTX	4

స్థిర అస్థిర గుర్తులేని ap_cpus_booted;

SPINLOCK_DEFINE(big_kernel_తాళం)
SPINLOCK_DEFINE(boot_తాళం)

శూన్య wait_for_APs_గమ్యం_ముగింపు_booting(శూన్య)
{
	గుర్తులేని n = 0;
	పూర్ణాంకం i;

	/* check how many cpus are actually alive */
	కొరకు (i = 0 ; i < ncpus ; i++) {
		ఒకవేళ (cpu_test_flag(i, CPU_IS_READY))
			n++;
	}
	ఒకవేళ (n != ncpus)
		రాయి("WARNING only %d out of %d cpus booted\n", n, ncpus);

	/* we must let the other CPUs to run in kernel mode first */
	BKL_UNLOCK();
	అయితే (ap_cpus_booted != (n - 1))
		arch_pause();
	/* now we have to take the lock again as we continue execution */
	BKL_తాళం();
}

శూన్య ap_boot_ముగిసిన(గుర్తులేని cpu)
{
	ap_cpus_booted++;
}

శూన్య smp_ipi_halt_handler(శూన్య)
{
	ipi_ack();
	stop_local_timer();
	arch_smp_halt_cpu();
}

శూన్య smp_కాలపట్టిక(గుర్తులేని cpu)
{
	arch_send_smp_కాలపట్టిక_ipi(cpu);
}

శూన్య smp_sched_handler(శూన్య);

/*
 * tell another cpu about a task to do and return only after the cpu acks that
 * the task is finished. Also wait before it finishes task sent by another cpu
 * to the same one.
 */
స్థిర శూన్య smp_కాలపట్టిక_sync(నిర్మాణం proc * p, గుర్తులేని task)
{
	గుర్తులేని cpu = p->p_cpu;
	గుర్తులేని mycpu = cpuid;

	ప్రతిజ్ఞా(cpu != mycpu);
	/*
	 * if some other cpu made a request to the same cpu, wait until it is
	 * done before proceeding
	 */
	ఒకవేళ (sched_ipi_దత్తాంశం[cpu].flags != 0) {
		BKL_UNLOCK();
		అయితే (sched_ipi_దత్తాంశం[cpu].flags != 0) {
			ఒకవేళ (sched_ipi_దత్తాంశం[mycpu].flags) {
				BKL_తాళం();
				smp_sched_handler();
				BKL_UNLOCK();
			}
		}
		BKL_తాళం();
	}

	sched_ipi_దత్తాంశం[cpu].దత్తాంశం = (u32_t) p;
	sched_ipi_దత్తాంశం[cpu].flags |= task;
	__insn_barrier();
	arch_send_smp_కాలపట్టిక_ipi(cpu);

	/* wait until the destination cpu finishes its job */
	BKL_UNLOCK();
	అయితే (sched_ipi_దత్తాంశం[cpu].flags != 0) {
		ఒకవేళ (sched_ipi_దత్తాంశం[mycpu].flags) {
			BKL_తాళం();
			smp_sched_handler();
			BKL_UNLOCK();
		}
	}
	BKL_తాళం();
}

శూన్య smp_కాలపట్టిక_stop_proc(నిర్మాణం proc * p)
{
	ఒకవేళ (proc_is_runnable(p))
		smp_కాలపట్టిక_sync(p, SCHED_IPI_STOP_PROC);
	లేకపోతే
		RTS_సమితి(p, RTS_PROC_STOP);
	ప్రతిజ్ఞా(RTS_ISSET(p, RTS_PROC_STOP));
}

శూన్య smp_కాలపట్టిక_vminhibit(నిర్మాణం proc * p)
{
	ఒకవేళ (proc_is_runnable(p))
		smp_కాలపట్టిక_sync(p, SCHED_IPI_VM_INHIBIT);
	లేకపోతే
		RTS_సమితి(p, RTS_VMINHIBIT);
	ప్రతిజ్ఞా(RTS_ISSET(p, RTS_VMINHIBIT));
}

శూన్య smp_కాలపట్టిక_stop_proc_save_ctx(నిర్మాణం proc * p)
{
	/*
	 * stop the processes and force the complete context of the process to
	 * be saved (i.e. including FPU state and such)
	 */
	smp_కాలపట్టిక_sync(p, SCHED_IPI_STOP_PROC | SCHED_IPI_SAVE_CTX);
	ప్రతిజ్ఞా(RTS_ISSET(p, RTS_PROC_STOP));
}

శూన్య smp_కాలపట్టిక_migrate_proc(నిర్మాణం proc * p, గుర్తులేని dest_cpu)
{
	/*
	 * stop the processes and force the complete context of the process to
	 * be saved (i.e. including FPU state and such)
	 */
	smp_కాలపట్టిక_sync(p, SCHED_IPI_STOP_PROC | SCHED_IPI_SAVE_CTX);
	ప్రతిజ్ఞా(RTS_ISSET(p, RTS_PROC_STOP));
	
	/* assign the new cpu and let the process run again */
	p->p_cpu = dest_cpu;
	RTS_UNSET(p, RTS_PROC_STOP);
}

శూన్య smp_sched_handler(శూన్య)
{
	గుర్తులేని flgs;
	గుర్తులేని cpu = cpuid;

	flgs = sched_ipi_దత్తాంశం[cpu].flags;

	ఒకవేళ (flgs) {
		నిర్మాణం proc * p;
		p = (నిర్మాణం proc *)sched_ipi_దత్తాంశం[cpu].దత్తాంశం;

		ఒకవేళ (flgs & SCHED_IPI_STOP_PROC) {
			RTS_సమితి(p, RTS_PROC_STOP);
		}
		ఒకవేళ (flgs & SCHED_IPI_SAVE_CTX) {
			/* all context has been saved already, FPU remains */
			ఒకవేళ (proc_వాడిన_fpu(p) &&
					get_cpulocal_var(fpu_owner) == p) {
				disable_fpu_exception();
				save_local_fpu(p, FALSE /*retain*/);
				/* we're preparing to migrate somewhere else */
				release_fpu(p);
			}
		}
		ఒకవేళ (flgs & SCHED_IPI_VM_INHIBIT) {
			RTS_సమితి(p, RTS_VMINHIBIT);
		}
	}

	__insn_barrier();
	sched_ipi_దత్తాంశం[cpu].flags = 0;
}

/*
 * This function gets always called only after smp_sched_handler() has been
 * already called. It only serves the purpose of acknowledging the IPI and
 * preempting the current process if the CPU was not idle.
 */
శూన్య smp_ipi_sched_handler(శూన్య)
{
	నిర్మాణం proc * curr;

	ipi_ack();

	curr = get_cpulocal_var(proc_ptr);
	ఒకవేళ (curr->p_endpoint != IDLE) {
		RTS_సమితి(curr, RTS_PREEMPTED);
	}
}

