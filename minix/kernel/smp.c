#અન્તર્ભાવ <assert.h>

#અન્તર્ભાવ "smp.h"
#અન્તર્ભાવ "interrupt.h"
#અન્તર્ભાવ "clock.h"

unsigned ncpus;
unsigned ht_per_core;
unsigned bsp_cpu_id;

સંરચના cpu cpus[CONFIG_MAX_CPUS];

/* info passed to another cpu along with a sched ipi */
સંરચના sched_ipi_data {
	અસ્થિર u32_t	flags;
	અસ્થિર u32_t	data;
};

સ્થિર સંરચના sched_ipi_data  sched_ipi_data[CONFIG_MAX_CPUS];

#define SCHED_IPI_STOP_PROC	1
#define SCHED_IPI_VM_INHIBIT	2
#define SCHED_IPI_SAVE_CTX	4

સ્થિર અસ્થિર unsigned ap_cpus_booted;

SPINLOCK_DEFINE(big_kernel_lock)
SPINLOCK_DEFINE(boot_lock)

શૂન્ય wait_for_APs_to_finish_booting(શૂન્ય)
{
	unsigned n = 0;
	પૂર્ણાંક i;

	/* check how many cpus are actually alive */
	માટે (i = 0 ; i < ncpus ; i++) {
		જો (cpu_test_flag(i, CPU_IS_READY))
			n++;
	}
	જો (n != ncpus)
		printf("WARNING only %d out of %d cpus booted\n", n, ncpus);

	/* we must let the other CPUs to run in kernel mode first */
	BKL_UNLOCK();
	જ્યારે (ap_cpus_booted != (n - 1))
		arch_pause();
	/* now we have to take the lock again as we continue execution */
	BKL_LOCK();
}

શૂન્ય ap_boot_finished(unsigned cpu)
{
	ap_cpus_booted++;
}

શૂન્ય smp_ipi_halt_handler(શૂન્ય)
{
	ipi_ack();
	stop_local_timer();
	arch_smp_halt_cpu();
}

શૂન્ય smp_schedule(unsigned cpu)
{
	arch_send_smp_schedule_ipi(cpu);
}

શૂન્ય smp_sched_handler(શૂન્ય);

/*
 * tell another cpu about a task to do and return only after the cpu acks that
 * the task is finished. Also wait before it finishes task sent by another cpu
 * to the same one.
 */
સ્થિર શૂન્ય smp_schedule_sync(સંરચના proc * p, unsigned task)
{
	unsigned cpu = p->p_cpu;
	unsigned mycpu = cpuid;

	પ્રતિજ્ઞા(cpu != mycpu);
	/*
	 * if some other cpu made a request to the same cpu, wait until it is
	 * done before proceeding
	 */
	જો (sched_ipi_data[cpu].flags != 0) {
		BKL_UNLOCK();
		જ્યારે (sched_ipi_data[cpu].flags != 0) {
			જો (sched_ipi_data[mycpu].flags) {
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
	જ્યારે (sched_ipi_data[cpu].flags != 0) {
		જો (sched_ipi_data[mycpu].flags) {
			BKL_LOCK();
			smp_sched_handler();
			BKL_UNLOCK();
		}
	}
	BKL_LOCK();
}

શૂન્ય smp_schedule_stop_proc(સંરચના proc * p)
{
	જો (proc_is_runnable(p))
		smp_schedule_sync(p, SCHED_IPI_STOP_PROC);
	નહીંતર
		RTS_SET(p, RTS_PROC_STOP);
	પ્રતિજ્ઞા(RTS_ISSET(p, RTS_PROC_STOP));
}

શૂન્ય smp_schedule_vminhibit(સંરચના proc * p)
{
	જો (proc_is_runnable(p))
		smp_schedule_sync(p, SCHED_IPI_VM_INHIBIT);
	નહીંતર
		RTS_SET(p, RTS_VMINHIBIT);
	પ્રતિજ્ઞા(RTS_ISSET(p, RTS_VMINHIBIT));
}

શૂન્ય smp_schedule_stop_proc_save_ctx(સંરચના proc * p)
{
	/*
	 * stop the processes and force the complete context of the process to
	 * be saved (i.e. including FPU state and such)
	 */
	smp_schedule_sync(p, SCHED_IPI_STOP_PROC | SCHED_IPI_SAVE_CTX);
	પ્રતિજ્ઞા(RTS_ISSET(p, RTS_PROC_STOP));
}

શૂન્ય smp_schedule_migrate_proc(સંરચના proc * p, unsigned dest_cpu)
{
	/*
	 * stop the processes and force the complete context of the process to
	 * be saved (i.e. including FPU state and such)
	 */
	smp_schedule_sync(p, SCHED_IPI_STOP_PROC | SCHED_IPI_SAVE_CTX);
	પ્રતિજ્ઞા(RTS_ISSET(p, RTS_PROC_STOP));
	
	/* assign the new cpu and let the process run again */
	p->p_cpu = dest_cpu;
	RTS_UNSET(p, RTS_PROC_STOP);
}

શૂન્ય smp_sched_handler(શૂન્ય)
{
	unsigned flgs;
	unsigned cpu = cpuid;

	flgs = sched_ipi_data[cpu].flags;

	જો (flgs) {
		સંરચના proc * p;
		p = (સંરચના proc *)sched_ipi_data[cpu].data;

		જો (flgs & SCHED_IPI_STOP_PROC) {
			RTS_SET(p, RTS_PROC_STOP);
		}
		જો (flgs & SCHED_IPI_SAVE_CTX) {
			/* all context has been saved already, FPU remains */
			જો (proc_used_fpu(p) &&
					get_cpulocal_var(fpu_owner) == p) {
				disable_fpu_exception();
				save_local_fpu(p, FALSE /*retain*/);
				/* we're preparing to migrate somewhere else */
				release_fpu(p);
			}
		}
		જો (flgs & SCHED_IPI_VM_INHIBIT) {
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
શૂન્ય smp_ipi_sched_handler(શૂન્ય)
{
	સંરચના proc * curr;

	ipi_ack();

	curr = get_cpulocal_var(proc_ptr);
	જો (curr->p_endpoint != IDLE) {
		RTS_SET(curr, RTS_PREEMPTED);
	}
}

