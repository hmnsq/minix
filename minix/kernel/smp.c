#ಅನ್ತರ್ಭಾವ <assert.h>

#ಅನ್ತರ್ಭಾವ "smp.h"
#ಅನ್ತರ್ಭಾವ "interrupt.h"
#ಅನ್ತರ್ಭಾವ "clock.h"

unsigned ncpus;
unsigned ht_per_core;
unsigned bsp_cpu_id;

ಸಂರಚನಾ cpu cpus[CONFIG_MAX_CPUS];

/* info passed to another cpu along with a sched ipi */
ಸಂರಚನಾ sched_ipi_data {
	ಚಞ್ಚಲ u32_t	flags;
	ಚಞ್ಚಲ u32_t	data;
};

ಸ್ಥಿರ ಸಂರಚನಾ sched_ipi_data  sched_ipi_data[CONFIG_MAX_CPUS];

#define SCHED_IPI_STOP_PROC	1
#define SCHED_IPI_VM_INHIBIT	2
#define SCHED_IPI_SAVE_CTX	4

ಸ್ಥಿರ ಚಞ್ಚಲ unsigned ap_cpus_booted;

SPINLOCK_DEFINE(big_kernel_lock)
SPINLOCK_DEFINE(boot_lock)

ಶೂನ್ಯ wait_for_APs_to_finish_booting(ಶೂನ್ಯ)
{
	unsigned n = 0;
	ಪೂರ್ಣಾಙ್ಕ i;

	/* check how many cpus are actually alive */
	ಪ್ರತಿ (i = 0 ; i < ncpus ; i++) {
		ಯದಿ (cpu_test_flag(i, CPU_IS_READY))
			n++;
	}
	ಯದಿ (n != ncpus)
		printf("WARNING only %d out of %d cpus booted\n", n, ncpus);

	/* we must let the other CPUs to run in kernel mode first */
	BKL_UNLOCK();
	ಯಾವತ್ (ap_cpus_booted != (n - 1))
		arch_pause();
	/* now we have to take the lock again as we continue execution */
	BKL_LOCK();
}

ಶೂನ್ಯ ap_boot_finished(unsigned cpu)
{
	ap_cpus_booted++;
}

ಶೂನ್ಯ smp_ipi_halt_handler(ಶೂನ್ಯ)
{
	ipi_ack();
	stop_local_timer();
	arch_smp_halt_cpu();
}

ಶೂನ್ಯ smp_schedule(unsigned cpu)
{
	arch_send_smp_schedule_ipi(cpu);
}

ಶೂನ್ಯ smp_sched_handler(ಶೂನ್ಯ);

/*
 * tell another cpu about a task to do and return only after the cpu acks that
 * the task is finished. Also wait before it finishes task sent by another cpu
 * to the same one.
 */
ಸ್ಥಿರ ಶೂನ್ಯ smp_schedule_sync(ಸಂರಚನಾ proc * p, unsigned task)
{
	unsigned cpu = p->p_cpu;
	unsigned mycpu = cpuid;

	ಪ್ರತಿಜ್ಞಾ(cpu != mycpu);
	/*
	 * if some other cpu made a request to the same cpu, wait until it is
	 * done before proceeding
	 */
	ಯದಿ (sched_ipi_data[cpu].flags != 0) {
		BKL_UNLOCK();
		ಯಾವತ್ (sched_ipi_data[cpu].flags != 0) {
			ಯದಿ (sched_ipi_data[mycpu].flags) {
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
	ಯಾವತ್ (sched_ipi_data[cpu].flags != 0) {
		ಯದಿ (sched_ipi_data[mycpu].flags) {
			BKL_LOCK();
			smp_sched_handler();
			BKL_UNLOCK();
		}
	}
	BKL_LOCK();
}

ಶೂನ್ಯ smp_schedule_stop_proc(ಸಂರಚನಾ proc * p)
{
	ಯದಿ (proc_is_runnable(p))
		smp_schedule_sync(p, SCHED_IPI_STOP_PROC);
	ಅನ್ಯಥಾ
		RTS_SET(p, RTS_PROC_STOP);
	ಪ್ರತಿಜ್ಞಾ(RTS_ISSET(p, RTS_PROC_STOP));
}

ಶೂನ್ಯ smp_schedule_vminhibit(ಸಂರಚನಾ proc * p)
{
	ಯದಿ (proc_is_runnable(p))
		smp_schedule_sync(p, SCHED_IPI_VM_INHIBIT);
	ಅನ್ಯಥಾ
		RTS_SET(p, RTS_VMINHIBIT);
	ಪ್ರತಿಜ್ಞಾ(RTS_ISSET(p, RTS_VMINHIBIT));
}

ಶೂನ್ಯ smp_schedule_stop_proc_save_ctx(ಸಂರಚನಾ proc * p)
{
	/*
	 * stop the processes and force the complete context of the process to
	 * be saved (i.e. including FPU state and such)
	 */
	smp_schedule_sync(p, SCHED_IPI_STOP_PROC | SCHED_IPI_SAVE_CTX);
	ಪ್ರತಿಜ್ಞಾ(RTS_ISSET(p, RTS_PROC_STOP));
}

ಶೂನ್ಯ smp_schedule_migrate_proc(ಸಂರಚನಾ proc * p, unsigned dest_cpu)
{
	/*
	 * stop the processes and force the complete context of the process to
	 * be saved (i.e. including FPU state and such)
	 */
	smp_schedule_sync(p, SCHED_IPI_STOP_PROC | SCHED_IPI_SAVE_CTX);
	ಪ್ರತಿಜ್ಞಾ(RTS_ISSET(p, RTS_PROC_STOP));
	
	/* assign the new cpu and let the process run again */
	p->p_cpu = dest_cpu;
	RTS_UNSET(p, RTS_PROC_STOP);
}

ಶೂನ್ಯ smp_sched_handler(ಶೂನ್ಯ)
{
	unsigned flgs;
	unsigned cpu = cpuid;

	flgs = sched_ipi_data[cpu].flags;

	ಯದಿ (flgs) {
		ಸಂರಚನಾ proc * p;
		p = (ಸಂರಚನಾ proc *)sched_ipi_data[cpu].data;

		ಯದಿ (flgs & SCHED_IPI_STOP_PROC) {
			RTS_SET(p, RTS_PROC_STOP);
		}
		ಯದಿ (flgs & SCHED_IPI_SAVE_CTX) {
			/* all context has been saved already, FPU remains */
			ಯದಿ (proc_used_fpu(p) &&
					get_cpulocal_var(fpu_owner) == p) {
				disable_fpu_exception();
				save_local_fpu(p, FALSE /*retain*/);
				/* we're preparing to migrate somewhere else */
				release_fpu(p);
			}
		}
		ಯದಿ (flgs & SCHED_IPI_VM_INHIBIT) {
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
ಶೂನ್ಯ smp_ipi_sched_handler(ಶೂನ್ಯ)
{
	ಸಂರಚನಾ proc * curr;

	ipi_ack();

	curr = get_cpulocal_var(proc_ptr);
	ಯದಿ (curr->p_endpoint != IDLE) {
		RTS_SET(curr, RTS_PREEMPTED);
	}
}

