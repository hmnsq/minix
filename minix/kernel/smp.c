#অন্তর্ভাৱ <assert.h>

#অন্তর্ভাৱ "smp.h"
#অন্তর্ভাৱ "interrupt.h"
#অন্তর্ভাৱ "clock.h"

unsigned ncpus;
unsigned ht_per_core;
unsigned bsp_cpu_id;

সংরচনা cpu cpus[CONFIG_MAX_CPUS];

/* info passed to another cpu along with a sched ipi */
সংরচনা sched_ipi_data {
	চঞ্চল u32_t	flags;
	চঞ্চল u32_t	data;
};

স্থির সংরচনা sched_ipi_data  sched_ipi_data[CONFIG_MAX_CPUS];

#define SCHED_IPI_STOP_PROC	1
#define SCHED_IPI_VM_INHIBIT	2
#define SCHED_IPI_SAVE_CTX	4

স্থির চঞ্চল unsigned ap_cpus_booted;

SPINLOCK_DEFINE(big_kernel_lock)
SPINLOCK_DEFINE(boot_lock)

শূন্য wait_for_APs_to_finish_booting(শূন্য)
{
	unsigned n = 0;
	পূর্ণাঙ্ক i;

	/* check how many cpus are actually alive */
	বাবে (i = 0 ; i < ncpus ; i++) {
		যদি (cpu_test_flag(i, CPU_IS_READY))
			n++;
	}
	যদি (n != ncpus)
		printf("WARNING only %d out of %d cpus booted\n", n, ncpus);

	/* we must let the other CPUs to run in kernel mode first */
	BKL_UNLOCK();
	যেতিয়ালৈ (ap_cpus_booted != (n - 1))
		arch_pause();
	/* now we have to take the lock again as we continue execution */
	BKL_LOCK();
}

শূন্য ap_boot_finished(unsigned cpu)
{
	ap_cpus_booted++;
}

শূন্য smp_ipi_halt_handler(শূন্য)
{
	ipi_ack();
	stop_local_timer();
	arch_smp_halt_cpu();
}

শূন্য smp_schedule(unsigned cpu)
{
	arch_send_smp_schedule_ipi(cpu);
}

শূন্য smp_sched_handler(শূন্য);

/*
 * tell another cpu about a task to do and return only after the cpu acks that
 * the task is finished. Also wait before it finishes task sent by another cpu
 * to the same one.
 */
স্থির শূন্য smp_schedule_sync(সংরচনা proc * p, unsigned task)
{
	unsigned cpu = p->p_cpu;
	unsigned mycpu = cpuid;

	প্রতিজ্ঞা(cpu != mycpu);
	/*
	 * if some other cpu made a request to the same cpu, wait until it is
	 * done before proceeding
	 */
	যদি (sched_ipi_data[cpu].flags != 0) {
		BKL_UNLOCK();
		যেতিয়ালৈ (sched_ipi_data[cpu].flags != 0) {
			যদি (sched_ipi_data[mycpu].flags) {
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
	যেতিয়ালৈ (sched_ipi_data[cpu].flags != 0) {
		যদি (sched_ipi_data[mycpu].flags) {
			BKL_LOCK();
			smp_sched_handler();
			BKL_UNLOCK();
		}
	}
	BKL_LOCK();
}

শূন্য smp_schedule_stop_proc(সংরচনা proc * p)
{
	যদি (proc_is_runnable(p))
		smp_schedule_sync(p, SCHED_IPI_STOP_PROC);
	অন্যথা
		RTS_SET(p, RTS_PROC_STOP);
	প্রতিজ্ঞা(RTS_ISSET(p, RTS_PROC_STOP));
}

শূন্য smp_schedule_vminhibit(সংরচনা proc * p)
{
	যদি (proc_is_runnable(p))
		smp_schedule_sync(p, SCHED_IPI_VM_INHIBIT);
	অন্যথা
		RTS_SET(p, RTS_VMINHIBIT);
	প্রতিজ্ঞা(RTS_ISSET(p, RTS_VMINHIBIT));
}

শূন্য smp_schedule_stop_proc_save_ctx(সংরচনা proc * p)
{
	/*
	 * stop the processes and force the complete context of the process to
	 * be saved (i.e. including FPU state and such)
	 */
	smp_schedule_sync(p, SCHED_IPI_STOP_PROC | SCHED_IPI_SAVE_CTX);
	প্রতিজ্ঞা(RTS_ISSET(p, RTS_PROC_STOP));
}

শূন্য smp_schedule_migrate_proc(সংরচনা proc * p, unsigned dest_cpu)
{
	/*
	 * stop the processes and force the complete context of the process to
	 * be saved (i.e. including FPU state and such)
	 */
	smp_schedule_sync(p, SCHED_IPI_STOP_PROC | SCHED_IPI_SAVE_CTX);
	প্রতিজ্ঞা(RTS_ISSET(p, RTS_PROC_STOP));
	
	/* assign the new cpu and let the process run again */
	p->p_cpu = dest_cpu;
	RTS_UNSET(p, RTS_PROC_STOP);
}

শূন্য smp_sched_handler(শূন্য)
{
	unsigned flgs;
	unsigned cpu = cpuid;

	flgs = sched_ipi_data[cpu].flags;

	যদি (flgs) {
		সংরচনা proc * p;
		p = (সংরচনা proc *)sched_ipi_data[cpu].data;

		যদি (flgs & SCHED_IPI_STOP_PROC) {
			RTS_SET(p, RTS_PROC_STOP);
		}
		যদি (flgs & SCHED_IPI_SAVE_CTX) {
			/* all context has been saved already, FPU remains */
			যদি (proc_used_fpu(p) &&
					get_cpulocal_var(fpu_owner) == p) {
				disable_fpu_exception();
				save_local_fpu(p, FALSE /*retain*/);
				/* we're preparing to migrate somewhere else */
				release_fpu(p);
			}
		}
		যদি (flgs & SCHED_IPI_VM_INHIBIT) {
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
শূন্য smp_ipi_sched_handler(শূন্য)
{
	সংরচনা proc * curr;

	ipi_ack();

	curr = get_cpulocal_var(proc_ptr);
	যদি (curr->p_endpoint != IDLE) {
		RTS_SET(curr, RTS_PREEMPTED);
	}
}

