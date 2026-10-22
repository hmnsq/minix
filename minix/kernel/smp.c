#ಅನ್ತರ್ಭಾವ <assert.h>

#ಅನ್ತರ್ಭಾವ "smp.h"
#ಅನ್ತರ್ಭಾವ "interrupt.h"
#ಅನ್ತರ್ಭಾವ "clock.h"

ಚಿಹ್ನೆರಹಿತ ncpus;
ಚಿಹ್ನೆರಹಿತ ht_ಪ್ರತಿ_core;
ಚಿಹ್ನೆರಹಿತ bsp_cpu_id;

ರಚನೆ cpu cpus[CONFIG_MAX_CPUS];

/* info passed to another cpu along with a sched ipi */
ರಚನೆ sched_ipi_ದತ್ತಾಂಶ {
	ಅಸ್ಥಿರ u32_t	flags;
	ಅಸ್ಥಿರ u32_t	ದತ್ತಾಂಶ;
};

ಸ್ಥಿರ ರಚನೆ sched_ipi_ದತ್ತಾಂಶ  sched_ipi_ದತ್ತಾಂಶ[CONFIG_MAX_CPUS];

#ವ್ಯಾಖ್ಯಾನಿಸು SCHED_IPI_STOP_PROC	1
#ವ್ಯಾಖ್ಯಾನಿಸು SCHED_IPI_VM_INHIBIT	2
#ವ್ಯಾಖ್ಯಾನಿಸು SCHED_IPI_SAVE_CTX	4

ಸ್ಥಿರ ಅಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ap_cpus_booted;

SPINLOCK_DEFINE(big_kernel_ಬೀಗ)
SPINLOCK_DEFINE(boot_ಬೀಗ)

ಶೂನ್ಯ wait_for_APs_ಗಮ್ಯ_ಮುಕ್ತಾಯ_booting(ಶೂನ್ಯ)
{
	ಚಿಹ್ನೆರಹಿತ n = 0;
	ಪೂರ್ಣಾಂಕ i;

	/* check how many cpus are actually alive */
	ಗಾಗಿ (i = 0 ; i < ncpus ; i++) {
		ಆದರೆ (cpu_test_flag(i, CPU_IS_READY))
			n++;
	}
	ಆದರೆ (n != ncpus)
		ಬರೆ("WARNING only %d out of %d cpus booted\n", n, ncpus);

	/* we must let the other CPUs to run in kernel mode first */
	BKL_UNLOCK();
	ಆಗ (ap_cpus_booted != (n - 1))
		arch_pause();
	/* now we have to take the lock again as we continue execution */
	BKL_ಬೀಗ();
}

ಶೂನ್ಯ ap_boot_ಪೂರ್ಣಗೊಂಡ(ಚಿಹ್ನೆರಹಿತ cpu)
{
	ap_cpus_booted++;
}

ಶೂನ್ಯ smp_ipi_halt_handler(ಶೂನ್ಯ)
{
	ipi_ack();
	stop_local_timer();
	arch_smp_halt_cpu();
}

ಶೂನ್ಯ smp_ವೇಳಾಪಟ್ಟಿ(ಚಿಹ್ನೆರಹಿತ cpu)
{
	arch_send_smp_ವೇಳಾಪಟ್ಟಿ_ipi(cpu);
}

ಶೂನ್ಯ smp_sched_handler(ಶೂನ್ಯ);

/*
 * tell another cpu about a task to do and return only after the cpu acks that
 * the task is finished. Also wait before it finishes task sent by another cpu
 * to the same one.
 */
ಸ್ಥಿರ ಶೂನ್ಯ smp_ವೇಳಾಪಟ್ಟಿ_sync(ರಚನೆ proc * p, ಚಿಹ್ನೆರಹಿತ task)
{
	ಚಿಹ್ನೆರಹಿತ cpu = p->p_cpu;
	ಚಿಹ್ನೆರಹಿತ mycpu = cpuid;

	ಪ್ರತಿಜ್ಞಾ(cpu != mycpu);
	/*
	 * if some other cpu made a request to the same cpu, wait until it is
	 * done before proceeding
	 */
	ಆದರೆ (sched_ipi_ದತ್ತಾಂಶ[cpu].flags != 0) {
		BKL_UNLOCK();
		ಆಗ (sched_ipi_ದತ್ತಾಂಶ[cpu].flags != 0) {
			ಆದರೆ (sched_ipi_ದತ್ತಾಂಶ[mycpu].flags) {
				BKL_ಬೀಗ();
				smp_sched_handler();
				BKL_UNLOCK();
			}
		}
		BKL_ಬೀಗ();
	}

	sched_ipi_ದತ್ತಾಂಶ[cpu].ದತ್ತಾಂಶ = (u32_t) p;
	sched_ipi_ದತ್ತಾಂಶ[cpu].flags |= task;
	__insn_barrier();
	arch_send_smp_ವೇಳಾಪಟ್ಟಿ_ipi(cpu);

	/* wait until the destination cpu finishes its job */
	BKL_UNLOCK();
	ಆಗ (sched_ipi_ದತ್ತಾಂಶ[cpu].flags != 0) {
		ಆದರೆ (sched_ipi_ದತ್ತಾಂಶ[mycpu].flags) {
			BKL_ಬೀಗ();
			smp_sched_handler();
			BKL_UNLOCK();
		}
	}
	BKL_ಬೀಗ();
}

ಶೂನ್ಯ smp_ವೇಳಾಪಟ್ಟಿ_stop_proc(ರಚನೆ proc * p)
{
	ಆದರೆ (proc_is_runnable(p))
		smp_ವೇಳಾಪಟ್ಟಿ_sync(p, SCHED_IPI_STOP_PROC);
	ಇಲ್ಲದಿದ್ದರೆ
		RTS_ಗಣ(p, RTS_PROC_STOP);
	ಪ್ರತಿಜ್ಞಾ(RTS_ISSET(p, RTS_PROC_STOP));
}

ಶೂನ್ಯ smp_ವೇಳಾಪಟ್ಟಿ_vminhibit(ರಚನೆ proc * p)
{
	ಆದರೆ (proc_is_runnable(p))
		smp_ವೇಳಾಪಟ್ಟಿ_sync(p, SCHED_IPI_VM_INHIBIT);
	ಇಲ್ಲದಿದ್ದರೆ
		RTS_ಗಣ(p, RTS_VMINHIBIT);
	ಪ್ರತಿಜ್ಞಾ(RTS_ISSET(p, RTS_VMINHIBIT));
}

ಶೂನ್ಯ smp_ವೇಳಾಪಟ್ಟಿ_stop_proc_save_ctx(ರಚನೆ proc * p)
{
	/*
	 * stop the processes and force the complete context of the process to
	 * be saved (i.e. including FPU state and such)
	 */
	smp_ವೇಳಾಪಟ್ಟಿ_sync(p, SCHED_IPI_STOP_PROC | SCHED_IPI_SAVE_CTX);
	ಪ್ರತಿಜ್ಞಾ(RTS_ISSET(p, RTS_PROC_STOP));
}

ಶೂನ್ಯ smp_ವೇಳಾಪಟ್ಟಿ_migrate_proc(ರಚನೆ proc * p, ಚಿಹ್ನೆರಹಿತ dest_cpu)
{
	/*
	 * stop the processes and force the complete context of the process to
	 * be saved (i.e. including FPU state and such)
	 */
	smp_ವೇಳಾಪಟ್ಟಿ_sync(p, SCHED_IPI_STOP_PROC | SCHED_IPI_SAVE_CTX);
	ಪ್ರತಿಜ್ಞಾ(RTS_ISSET(p, RTS_PROC_STOP));
	
	/* assign the new cpu and let the process run again */
	p->p_cpu = dest_cpu;
	RTS_UNSET(p, RTS_PROC_STOP);
}

ಶೂನ್ಯ smp_sched_handler(ಶೂನ್ಯ)
{
	ಚಿಹ್ನೆರಹಿತ flgs;
	ಚಿಹ್ನೆರಹಿತ cpu = cpuid;

	flgs = sched_ipi_ದತ್ತಾಂಶ[cpu].flags;

	ಆದರೆ (flgs) {
		ರಚನೆ proc * p;
		p = (ರಚನೆ proc *)sched_ipi_ದತ್ತಾಂಶ[cpu].ದತ್ತಾಂಶ;

		ಆದರೆ (flgs & SCHED_IPI_STOP_PROC) {
			RTS_ಗಣ(p, RTS_PROC_STOP);
		}
		ಆದರೆ (flgs & SCHED_IPI_SAVE_CTX) {
			/* all context has been saved already, FPU remains */
			ಆದರೆ (proc_ಬಳಸಿದ_fpu(p) &&
					get_cpulocal_var(fpu_owner) == p) {
				disable_fpu_exception();
				save_local_fpu(p, FALSE /*retain*/);
				/* we're preparing to migrate somewhere else */
				release_fpu(p);
			}
		}
		ಆದರೆ (flgs & SCHED_IPI_VM_INHIBIT) {
			RTS_ಗಣ(p, RTS_VMINHIBIT);
		}
	}

	__insn_barrier();
	sched_ipi_ದತ್ತಾಂಶ[cpu].flags = 0;
}

/*
 * This function gets always called only after smp_sched_handler() has been
 * already called. It only serves the purpose of acknowledging the IPI and
 * preempting the current process if the CPU was not idle.
 */
ಶೂನ್ಯ smp_ipi_sched_handler(ಶೂನ್ಯ)
{
	ರಚನೆ proc * curr;

	ipi_ack();

	curr = get_cpulocal_var(proc_ptr);
	ಆದರೆ (curr->p_endpoint != IDLE) {
		RTS_ಗಣ(curr, RTS_PREEMPTED);
	}
}

