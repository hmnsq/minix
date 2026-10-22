#अन्तर्भाव <assert.h>

#अन्तर्भाव "smp.h"
#अन्तर्भाव "interrupt.h"
#अन्तर्भाव "clock.h"

unsigned ncpus;
unsigned ht_per_core;
unsigned bsp_cpu_id;

संरचना cpu cpus[CONFIG_MAX_CPUS];

/* info passed to another cpu along with a sched ipi */
संरचना sched_ipi_data {
	चञ्चल u32_t	flags;
	चञ्चल u32_t	data;
};

स्थिर संरचना sched_ipi_data  sched_ipi_data[CONFIG_MAX_CPUS];

#define SCHED_IPI_STOP_PROC	1
#define SCHED_IPI_VM_INHIBIT	2
#define SCHED_IPI_SAVE_CTX	4

स्थिर चञ्चल unsigned ap_cpus_booted;

SPINLOCK_DEFINE(big_kernel_lock)
SPINLOCK_DEFINE(boot_lock)

शून्य wait_for_APs_to_finish_booting(शून्य)
{
	unsigned n = 0;
	पूर्णाङ्क i;

	/* check how many cpus are actually alive */
	अत्थाय (i = 0 ; i < ncpus ; i++) {
		सचे (cpu_test_flag(i, CPU_IS_READY))
			n++;
	}
	सचे (n != ncpus)
		printf("WARNING only %d out of %d cpus booted\n", n, ncpus);

	/* we must let the other CPUs to run in kernel mode first */
	BKL_UNLOCK();
	याव (ap_cpus_booted != (n - 1))
		arch_pause();
	/* now we have to take the lock again as we continue execution */
	BKL_LOCK();
}

शून्य ap_boot_finished(unsigned cpu)
{
	ap_cpus_booted++;
}

शून्य smp_ipi_halt_handler(शून्य)
{
	ipi_ack();
	stop_local_timer();
	arch_smp_halt_cpu();
}

शून्य smp_schedule(unsigned cpu)
{
	arch_send_smp_schedule_ipi(cpu);
}

शून्य smp_sched_handler(शून्य);

/*
 * tell another cpu about a task to do and return only after the cpu acks that
 * the task is finished. Also wait before it finishes task sent by another cpu
 * to the same one.
 */
स्थिर शून्य smp_schedule_sync(संरचना proc * p, unsigned task)
{
	unsigned cpu = p->p_cpu;
	unsigned mycpu = cpuid;

	प्रतिज्ञा(cpu != mycpu);
	/*
	 * if some other cpu made a request to the same cpu, wait until it is
	 * done before proceeding
	 */
	सचे (sched_ipi_data[cpu].flags != 0) {
		BKL_UNLOCK();
		याव (sched_ipi_data[cpu].flags != 0) {
			सचे (sched_ipi_data[mycpu].flags) {
				BKL_LOCK();
				smp_sched_handler();
				BKL_UNLOCK();
			}
		}
		BKL_LOCK();
	}

	sched_ipi_data[cpu].data = (u32_t) p;
	sched_ipi_data[cpu].flags |= task;
	__insn_barrier();
	arch_send_smp_schedule_ipi(cpu);

	/* wait until the destination cpu finishes its job */
	BKL_UNLOCK();
	याव (sched_ipi_data[cpu].flags != 0) {
		सचे (sched_ipi_data[mycpu].flags) {
			BKL_LOCK();
			smp_sched_handler();
			BKL_UNLOCK();
		}
	}
	BKL_LOCK();
}

शून्य smp_schedule_stop_proc(संरचना proc * p)
{
	सचे (proc_is_runnable(p))
		smp_schedule_sync(p, SCHED_IPI_STOP_PROC);
	अञ्ञथा
		RTS_SET(p, RTS_PROC_STOP);
	प्रतिज्ञा(RTS_ISSET(p, RTS_PROC_STOP));
}

शून्य smp_schedule_vminhibit(संरचना proc * p)
{
	सचे (proc_is_runnable(p))
		smp_schedule_sync(p, SCHED_IPI_VM_INHIBIT);
	अञ्ञथा
		RTS_SET(p, RTS_VMINHIBIT);
	प्रतिज्ञा(RTS_ISSET(p, RTS_VMINHIBIT));
}

शून्य smp_schedule_stop_proc_save_ctx(संरचना proc * p)
{
	/*
	 * stop the processes and force the complete context of the process to
	 * be saved (i.e. including FPU state and such)
	 */
	smp_schedule_sync(p, SCHED_IPI_STOP_PROC | SCHED_IPI_SAVE_CTX);
	प्रतिज्ञा(RTS_ISSET(p, RTS_PROC_STOP));
}

शून्य smp_schedule_migrate_proc(संरचना proc * p, unsigned dest_cpu)
{
	/*
	 * stop the processes and force the complete context of the process to
	 * be saved (i.e. including FPU state and such)
	 */
	smp_schedule_sync(p, SCHED_IPI_STOP_PROC | SCHED_IPI_SAVE_CTX);
	प्रतिज्ञा(RTS_ISSET(p, RTS_PROC_STOP));
	
	/* assign the new cpu and let the process run again */
	p->p_cpu = dest_cpu;
	RTS_UNSET(p, RTS_PROC_STOP);
}

शून्य smp_sched_handler(शून्य)
{
	unsigned flgs;
	unsigned cpu = cpuid;

	flgs = sched_ipi_data[cpu].flags;

	सचे (flgs) {
		संरचना proc * p;
		p = (संरचना proc *)sched_ipi_data[cpu].data;

		सचे (flgs & SCHED_IPI_STOP_PROC) {
			RTS_SET(p, RTS_PROC_STOP);
		}
		सचे (flgs & SCHED_IPI_SAVE_CTX) {
			/* all context has been saved already, FPU remains */
			सचे (proc_used_fpu(p) &&
					get_cpulocal_var(fpu_owner) == p) {
				disable_fpu_exception();
				save_local_fpu(p, FALSE /*retain*/);
				/* we're preparing to migrate somewhere else */
				release_fpu(p);
			}
		}
		सचे (flgs & SCHED_IPI_VM_INHIBIT) {
			RTS_SET(p, RTS_VMINHIBIT);
		}
	}

	__insn_barrier();
	sched_ipi_data[cpu].flags = 0;
}

/*
 * This function gets always called only after smp_sched_handler() has been
 * already called. It only serves the purpose of acknowledging the IPI and
 * preempting the current process if the CPU was not idle.
 */
शून्य smp_ipi_sched_handler(शून्य)
{
	संरचना proc * curr;

	ipi_ack();

	curr = get_cpulocal_var(proc_ptr);
	सचे (curr->p_endpoint != IDLE) {
		RTS_SET(curr, RTS_PREEMPTED);
	}
}

