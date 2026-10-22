#அந்தர்பாவ <assert.h>

#அந்தர்பாவ "smp.h"
#அந்தர்பாவ "interrupt.h"
#அந்தர்பாவ "clock.h"

unsigned ncpus;
unsigned ht_per_core;
unsigned bsp_cpu_id;

கட்டமைப்பு cpu cpus[CONFIG_MAX_CPUS];

/* info passed to another cpu along with a sched ipi */
கட்டமைப்பு sched_ipi_data {
	நிலையற்ற u32_t	flags;
	நிலையற்ற u32_t	data;
};

நிலையான கட்டமைப்பு sched_ipi_data  sched_ipi_data[CONFIG_MAX_CPUS];

#define SCHED_IPI_STOP_PROC	1
#define SCHED_IPI_VM_INHIBIT	2
#define SCHED_IPI_SAVE_CTX	4

நிலையான நிலையற்ற unsigned ap_cpus_booted;

SPINLOCK_DEFINE(big_kernel_lock)
SPINLOCK_DEFINE(boot_lock)

வெறுமை wait_for_APs_to_finish_booting(வெறுமை)
{
	unsigned n = 0;
	முழு i;

	/* check how many cpus are actually alive */
	க்கு (i = 0 ; i < ncpus ; i++) {
		எனில் (cpu_test_flag(i, CPU_IS_READY))
			n++;
	}
	எனில் (n != ncpus)
		printf("WARNING only %d out of %d cpus booted\n", n, ncpus);

	/* we must let the other CPUs to run in kernel mode first */
	BKL_UNLOCK();
	போது (ap_cpus_booted != (n - 1))
		arch_pause();
	/* now we have to take the lock again as we continue execution */
	BKL_LOCK();
}

வெறுமை ap_boot_finished(unsigned cpu)
{
	ap_cpus_booted++;
}

வெறுமை smp_ipi_halt_handler(வெறுமை)
{
	ipi_ack();
	stop_local_timer();
	arch_smp_halt_cpu();
}

வெறுமை smp_schedule(unsigned cpu)
{
	arch_send_smp_schedule_ipi(cpu);
}

வெறுமை smp_sched_handler(வெறுமை);

/*
 * tell another cpu about a task to do and return only after the cpu acks that
 * the task is finished. Also wait before it finishes task sent by another cpu
 * to the same one.
 */
நிலையான வெறுமை smp_schedule_sync(கட்டமைப்பு proc * p, unsigned task)
{
	unsigned cpu = p->p_cpu;
	unsigned mycpu = cpuid;

	ப்ரதிஜ்ஞா(cpu != mycpu);
	/*
	 * if some other cpu made a request to the same cpu, wait until it is
	 * done before proceeding
	 */
	எனில் (sched_ipi_data[cpu].flags != 0) {
		BKL_UNLOCK();
		போது (sched_ipi_data[cpu].flags != 0) {
			எனில் (sched_ipi_data[mycpu].flags) {
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
	போது (sched_ipi_data[cpu].flags != 0) {
		எனில் (sched_ipi_data[mycpu].flags) {
			BKL_LOCK();
			smp_sched_handler();
			BKL_UNLOCK();
		}
	}
	BKL_LOCK();
}

வெறுமை smp_schedule_stop_proc(கட்டமைப்பு proc * p)
{
	எனில் (proc_is_runnable(p))
		smp_schedule_sync(p, SCHED_IPI_STOP_PROC);
	இல்லையெனில்
		RTS_SET(p, RTS_PROC_STOP);
	ப்ரதிஜ்ஞா(RTS_ISSET(p, RTS_PROC_STOP));
}

வெறுமை smp_schedule_vminhibit(கட்டமைப்பு proc * p)
{
	எனில் (proc_is_runnable(p))
		smp_schedule_sync(p, SCHED_IPI_VM_INHIBIT);
	இல்லையெனில்
		RTS_SET(p, RTS_VMINHIBIT);
	ப்ரதிஜ்ஞா(RTS_ISSET(p, RTS_VMINHIBIT));
}

வெறுமை smp_schedule_stop_proc_save_ctx(கட்டமைப்பு proc * p)
{
	/*
	 * stop the processes and force the complete context of the process to
	 * be saved (i.e. including FPU state and such)
	 */
	smp_schedule_sync(p, SCHED_IPI_STOP_PROC | SCHED_IPI_SAVE_CTX);
	ப்ரதிஜ்ஞா(RTS_ISSET(p, RTS_PROC_STOP));
}

வெறுமை smp_schedule_migrate_proc(கட்டமைப்பு proc * p, unsigned dest_cpu)
{
	/*
	 * stop the processes and force the complete context of the process to
	 * be saved (i.e. including FPU state and such)
	 */
	smp_schedule_sync(p, SCHED_IPI_STOP_PROC | SCHED_IPI_SAVE_CTX);
	ப்ரதிஜ்ஞா(RTS_ISSET(p, RTS_PROC_STOP));
	
	/* assign the new cpu and let the process run again */
	p->p_cpu = dest_cpu;
	RTS_UNSET(p, RTS_PROC_STOP);
}

வெறுமை smp_sched_handler(வெறுமை)
{
	unsigned flgs;
	unsigned cpu = cpuid;

	flgs = sched_ipi_data[cpu].flags;

	எனில் (flgs) {
		கட்டமைப்பு proc * p;
		p = (கட்டமைப்பு proc *)sched_ipi_data[cpu].data;

		எனில் (flgs & SCHED_IPI_STOP_PROC) {
			RTS_SET(p, RTS_PROC_STOP);
		}
		எனில் (flgs & SCHED_IPI_SAVE_CTX) {
			/* all context has been saved already, FPU remains */
			எனில் (proc_used_fpu(p) &&
					get_cpulocal_var(fpu_owner) == p) {
				disable_fpu_exception();
				save_local_fpu(p, FALSE /*retain*/);
				/* we're preparing to migrate somewhere else */
				release_fpu(p);
			}
		}
		எனில் (flgs & SCHED_IPI_VM_INHIBIT) {
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
வெறுமை smp_ipi_sched_handler(வெறுமை)
{
	கட்டமைப்பு proc * curr;

	ipi_ack();

	curr = get_cpulocal_var(proc_ptr);
	எனில் (curr->p_endpoint != IDLE) {
		RTS_SET(curr, RTS_PREEMPTED);
	}
}

