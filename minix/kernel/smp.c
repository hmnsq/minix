#समावेश <assert.h>

#समावेश "smp.h"
#समावेश "interrupt.h"
#समावेश "clock.h"

अचिन्हित ncpus;
अचिन्हित ht_per_core;
अचिन्हित bsp_cpu_id;

काष्ठा cpu cpus[CONFIG_MAX_CPUS];

/* info passed to another cpu along with a sched ipi */
काष्ठा sched_ipi_तथ्य {
	अस्थिर u32_t	flags;
	अस्थिर u32_t	तथ्य;
};

अटल काष्ठा sched_ipi_तथ्य  sched_ipi_तथ्य[CONFIG_MAX_CPUS];

#घोषणा SCHED_IPI_STOP_PROC	1
#घोषणा SCHED_IPI_VM_INHIBIT	2
#घोषणा SCHED_IPI_SAVE_CTX	4

अटल अस्थिर अचिन्हित ap_cpus_booted;

SPINLOCK_DEFINE(big_kernel_lock)
SPINLOCK_DEFINE(boot_lock)

व्योम wait_for_APs_to_finish_booting(व्योम)
{
	अचिन्हित n = 0;
	पूर्णांक i;

	/* check how many cpus are actually alive */
	क्रम (i = 0 ; i < ncpus ; i++) {
		अगर (cpu_test_flag(i, CPU_IS_READY))
			n++;
	}
	अगर (n != ncpus)
		म_लिखो("WARNING only %d out of %d cpus booted\n", n, ncpus);

	/* we must let the other CPUs to run in kernel mode first */
	BKL_UNLOCK();
	जबतक (ap_cpus_booted != (n - 1))
		arch_pause();
	/* now we have to take the lock again as we continue execution */
	BKL_LOCK();
}

व्योम ap_boot_finished(अचिन्हित cpu)
{
	ap_cpus_booted++;
}

व्योम smp_ipi_halt_handler(व्योम)
{
	ipi_ack();
	stop_local_timer();
	arch_smp_halt_cpu();
}

व्योम smp_schedule(अचिन्हित cpu)
{
	arch_send_smp_schedule_ipi(cpu);
}

व्योम smp_sched_handler(व्योम);

/*
 * tell another cpu about a task to do and return only after the cpu acks that
 * the task is finished. Also wait before it finishes task sent by another cpu
 * to the same one.
 */
अटल व्योम smp_schedule_sync(काष्ठा proc * p, अचिन्हित task)
{
	अचिन्हित cpu = p->p_cpu;
	अचिन्हित mycpu = cpuid;

	निश्चित(cpu != mycpu);
	/*
	 * if some other cpu made a request to the same cpu, wait until it is
	 * done before proceeding
	 */
	अगर (sched_ipi_तथ्य[cpu].flags != 0) {
		BKL_UNLOCK();
		जबतक (sched_ipi_तथ्य[cpu].flags != 0) {
			अगर (sched_ipi_तथ्य[mycpu].flags) {
				BKL_LOCK();
				smp_sched_handler();
				BKL_UNLOCK();
			}
		}
		BKL_LOCK();
	}

	sched_ipi_तथ्य[cpu].तथ्य = (u32_t) p;
	sched_ipi_तथ्य[cpu].flags |= task;
	__insn_barrier();
	arch_send_smp_schedule_ipi(cpu);

	/* wait until the destination cpu finishes its job */
	BKL_UNLOCK();
	जबतक (sched_ipi_तथ्य[cpu].flags != 0) {
		अगर (sched_ipi_तथ्य[mycpu].flags) {
			BKL_LOCK();
			smp_sched_handler();
			BKL_UNLOCK();
		}
	}
	BKL_LOCK();
}

व्योम smp_schedule_stop_proc(काष्ठा proc * p)
{
	अगर (proc_is_runnable(p))
		smp_schedule_sync(p, SCHED_IPI_STOP_PROC);
	अन्यथा
		RTS_SET(p, RTS_PROC_STOP);
	निश्चित(RTS_ISSET(p, RTS_PROC_STOP));
}

व्योम smp_schedule_vminhibit(काष्ठा proc * p)
{
	अगर (proc_is_runnable(p))
		smp_schedule_sync(p, SCHED_IPI_VM_INHIBIT);
	अन्यथा
		RTS_SET(p, RTS_VMINHIBIT);
	निश्चित(RTS_ISSET(p, RTS_VMINHIBIT));
}

व्योम smp_schedule_stop_proc_save_ctx(काष्ठा proc * p)
{
	/*
	 * stop the processes and force the complete context of the process to
	 * be saved (i.e. including FPU state and such)
	 */
	smp_schedule_sync(p, SCHED_IPI_STOP_PROC | SCHED_IPI_SAVE_CTX);
	निश्चित(RTS_ISSET(p, RTS_PROC_STOP));
}

व्योम smp_schedule_migrate_proc(काष्ठा proc * p, अचिन्हित dest_cpu)
{
	/*
	 * stop the processes and force the complete context of the process to
	 * be saved (i.e. including FPU state and such)
	 */
	smp_schedule_sync(p, SCHED_IPI_STOP_PROC | SCHED_IPI_SAVE_CTX);
	निश्चित(RTS_ISSET(p, RTS_PROC_STOP));
	
	/* assign the new cpu and let the process run again */
	p->p_cpu = dest_cpu;
	RTS_UNSET(p, RTS_PROC_STOP);
}

व्योम smp_sched_handler(व्योम)
{
	अचिन्हित flgs;
	अचिन्हित cpu = cpuid;

	flgs = sched_ipi_तथ्य[cpu].flags;

	अगर (flgs) {
		काष्ठा proc * p;
		p = (काष्ठा proc *)sched_ipi_तथ्य[cpu].तथ्य;

		अगर (flgs & SCHED_IPI_STOP_PROC) {
			RTS_SET(p, RTS_PROC_STOP);
		}
		अगर (flgs & SCHED_IPI_SAVE_CTX) {
			/* all context has been saved already, FPU remains */
			अगर (proc_used_fpu(p) &&
					get_cpulocal_var(fpu_owner) == p) {
				disable_fpu_exception();
				save_local_fpu(p, FALSE /*retain*/);
				/* we're preparing to migrate somewhere else */
				release_fpu(p);
			}
		}
		अगर (flgs & SCHED_IPI_VM_INHIBIT) {
			RTS_SET(p, RTS_VMINHIBIT);
		}
	}

	__insn_barrier();
	sched_ipi_तथ्य[cpu].flags = 0;
}

/*
 * This function gets always called only after smp_sched_handler() has been
 * already called. It only serves the purpose of acknowledging the IPI and
 * preempting the current process if the CPU was not idle.
 */
व्योम smp_ipi_sched_handler(व्योम)
{
	काष्ठा proc * curr;

	ipi_ack();

	curr = get_cpulocal_var(proc_ptr);
	अगर (curr->p_endpoint != IDLE) {
		RTS_SET(curr, RTS_PREEMPTED);
	}
}

