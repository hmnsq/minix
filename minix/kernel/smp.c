#ਅਨ੍ਤਰ੍ਭਾਵ <assert.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "smp.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "interrupt.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "clock.h"

ਬਿਨਾਨਿਸ਼ਾਨੀ ncpus;
ਬਿਨਾਨਿਸ਼ਾਨੀ ht_ਪ੍ਰਤੀ_core;
ਬਿਨਾਨਿਸ਼ਾਨੀ bsp_cpu_id;

ਬਣਤਰ cpu cpus[CONFIG_MAX_CPUS];

/* info passed to another cpu along with a sched ipi */
ਬਣਤਰ sched_ipi_ਅੰਕੜੇ {
	ਅਸਥਿਰ u32_t	flags;
	ਅਸਥਿਰ u32_t	ਅੰਕੜੇ;
};

ਸਥਿਰ ਬਣਤਰ sched_ipi_ਅੰਕੜੇ  sched_ipi_ਅੰਕੜੇ[CONFIG_MAX_CPUS];

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SCHED_IPI_STOP_PROC	1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SCHED_IPI_VM_INHIBIT	2
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SCHED_IPI_SAVE_CTX	4

ਸਥਿਰ ਅਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ap_cpus_booted;

SPINLOCK_DEFINE(big_kernel_ਤਾਲਾ)
SPINLOCK_DEFINE(boot_ਤਾਲਾ)

ਖਾਲੀ wait_for_APs_ਮੰਜ਼ਿਲ_ਅੰਤ_booting(ਖਾਲੀ)
{
	ਬਿਨਾਨਿਸ਼ਾਨੀ n = 0;
	ਪੂਰਨ i;

	/* check how many cpus are actually alive */
	ਲਈ (i = 0 ; i < ncpus ; i++) {
		ਜੇ (cpu_test_flag(i, CPU_IS_READY))
			n++;
	}
	ਜੇ (n != ncpus)
		ਲਿਖੋ("WARNING only %d out of %d cpus booted\n", n, ncpus);

	/* we must let the other CPUs to run in kernel mode first */
	BKL_UNLOCK();
	ਜਦੋਂ (ap_cpus_booted != (n - 1))
		arch_pause();
	/* now we have to take the lock again as we continue execution */
	BKL_ਤਾਲਾ();
}

ਖਾਲੀ ap_boot_ਮੁਕੰਮਲ(ਬਿਨਾਨਿਸ਼ਾਨੀ cpu)
{
	ap_cpus_booted++;
}

ਖਾਲੀ smp_ipi_halt_handler(ਖਾਲੀ)
{
	ipi_ack();
	stop_local_timer();
	arch_smp_halt_cpu();
}

ਖਾਲੀ smp_ਸਮਾਂ_ਸਾਰਣੀ(ਬਿਨਾਨਿਸ਼ਾਨੀ cpu)
{
	arch_send_smp_ਸਮਾਂ_ਸਾਰਣੀ_ipi(cpu);
}

ਖਾਲੀ smp_sched_handler(ਖਾਲੀ);

/*
 * tell another cpu about a task to do and return only after the cpu acks that
 * the task is finished. Also wait before it finishes task sent by another cpu
 * to the same one.
 */
ਸਥਿਰ ਖਾਲੀ smp_ਸਮਾਂ_ਸਾਰਣੀ_sync(ਬਣਤਰ proc * p, ਬਿਨਾਨਿਸ਼ਾਨੀ task)
{
	ਬਿਨਾਨਿਸ਼ਾਨੀ cpu = p->p_cpu;
	ਬਿਨਾਨਿਸ਼ਾਨੀ mycpu = cpuid;

	ਪ੍ਰਤਿਜ੍ਞਾ(cpu != mycpu);
	/*
	 * if some other cpu made a request to the same cpu, wait until it is
	 * done before proceeding
	 */
	ਜੇ (sched_ipi_ਅੰਕੜੇ[cpu].flags != 0) {
		BKL_UNLOCK();
		ਜਦੋਂ (sched_ipi_ਅੰਕੜੇ[cpu].flags != 0) {
			ਜੇ (sched_ipi_ਅੰਕੜੇ[mycpu].flags) {
				BKL_ਤਾਲਾ();
				smp_sched_handler();
				BKL_UNLOCK();
			}
		}
		BKL_ਤਾਲਾ();
	}

	sched_ipi_ਅੰਕੜੇ[cpu].ਅੰਕੜੇ = (u32_t) p;
	sched_ipi_ਅੰਕੜੇ[cpu].flags |= task;
	__insn_barrier();
	arch_send_smp_ਸਮਾਂ_ਸਾਰਣੀ_ipi(cpu);

	/* wait until the destination cpu finishes its job */
	BKL_UNLOCK();
	ਜਦੋਂ (sched_ipi_ਅੰਕੜੇ[cpu].flags != 0) {
		ਜੇ (sched_ipi_ਅੰਕੜੇ[mycpu].flags) {
			BKL_ਤਾਲਾ();
			smp_sched_handler();
			BKL_UNLOCK();
		}
	}
	BKL_ਤਾਲਾ();
}

ਖਾਲੀ smp_ਸਮਾਂ_ਸਾਰਣੀ_stop_proc(ਬਣਤਰ proc * p)
{
	ਜੇ (proc_is_runnable(p))
		smp_ਸਮਾਂ_ਸਾਰਣੀ_sync(p, SCHED_IPI_STOP_PROC);
	ਨਹੀਂਤਾਂ
		RTS_ਸੈੱਟ(p, RTS_PROC_STOP);
	ਪ੍ਰਤਿਜ੍ਞਾ(RTS_ISSET(p, RTS_PROC_STOP));
}

ਖਾਲੀ smp_ਸਮਾਂ_ਸਾਰਣੀ_vminhibit(ਬਣਤਰ proc * p)
{
	ਜੇ (proc_is_runnable(p))
		smp_ਸਮਾਂ_ਸਾਰਣੀ_sync(p, SCHED_IPI_VM_INHIBIT);
	ਨਹੀਂਤਾਂ
		RTS_ਸੈੱਟ(p, RTS_VMINHIBIT);
	ਪ੍ਰਤਿਜ੍ਞਾ(RTS_ISSET(p, RTS_VMINHIBIT));
}

ਖਾਲੀ smp_ਸਮਾਂ_ਸਾਰਣੀ_stop_proc_save_ctx(ਬਣਤਰ proc * p)
{
	/*
	 * stop the processes and force the complete context of the process to
	 * be saved (i.e. including FPU state and such)
	 */
	smp_ਸਮਾਂ_ਸਾਰਣੀ_sync(p, SCHED_IPI_STOP_PROC | SCHED_IPI_SAVE_CTX);
	ਪ੍ਰਤਿਜ੍ਞਾ(RTS_ISSET(p, RTS_PROC_STOP));
}

ਖਾਲੀ smp_ਸਮਾਂ_ਸਾਰਣੀ_migrate_proc(ਬਣਤਰ proc * p, ਬਿਨਾਨਿਸ਼ਾਨੀ dest_cpu)
{
	/*
	 * stop the processes and force the complete context of the process to
	 * be saved (i.e. including FPU state and such)
	 */
	smp_ਸਮਾਂ_ਸਾਰਣੀ_sync(p, SCHED_IPI_STOP_PROC | SCHED_IPI_SAVE_CTX);
	ਪ੍ਰਤਿਜ੍ਞਾ(RTS_ISSET(p, RTS_PROC_STOP));
	
	/* assign the new cpu and let the process run again */
	p->p_cpu = dest_cpu;
	RTS_UNSET(p, RTS_PROC_STOP);
}

ਖਾਲੀ smp_sched_handler(ਖਾਲੀ)
{
	ਬਿਨਾਨਿਸ਼ਾਨੀ flgs;
	ਬਿਨਾਨਿਸ਼ਾਨੀ cpu = cpuid;

	flgs = sched_ipi_ਅੰਕੜੇ[cpu].flags;

	ਜੇ (flgs) {
		ਬਣਤਰ proc * p;
		p = (ਬਣਤਰ proc *)sched_ipi_ਅੰਕੜੇ[cpu].ਅੰਕੜੇ;

		ਜੇ (flgs & SCHED_IPI_STOP_PROC) {
			RTS_ਸੈੱਟ(p, RTS_PROC_STOP);
		}
		ਜੇ (flgs & SCHED_IPI_SAVE_CTX) {
			/* all context has been saved already, FPU remains */
			ਜੇ (proc_ਵਰਤਿਆ_fpu(p) &&
					get_cpulocal_var(fpu_owner) == p) {
				disable_fpu_exception();
				save_local_fpu(p, FALSE /*retain*/);
				/* we're preparing to migrate somewhere else */
				release_fpu(p);
			}
		}
		ਜੇ (flgs & SCHED_IPI_VM_INHIBIT) {
			RTS_ਸੈੱਟ(p, RTS_VMINHIBIT);
		}
	}

	__insn_barrier();
	sched_ipi_ਅੰਕੜੇ[cpu].flags = 0;
}

/*
 * This function gets always called only after smp_sched_handler() has been
 * already called. It only serves the purpose of acknowledging the IPI and
 * preempting the current process if the CPU was not idle.
 */
ਖਾਲੀ smp_ipi_sched_handler(ਖਾਲੀ)
{
	ਬਣਤਰ proc * curr;

	ipi_ack();

	curr = get_cpulocal_var(proc_ptr);
	ਜੇ (curr->p_endpoint != IDLE) {
		RTS_ਸੈੱਟ(curr, RTS_PREEMPTED);
	}
}

