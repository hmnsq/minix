#अन्तर्भाव <assert.h>

#अन्तर्भाव "smp.h"
#अन्तर्भाव "interrupt.h"
#अन्तर्भाव "clock.h"

अचिह्नित ncpus;
अचिह्नित ht_प्रत्येकम्_core;
अचिह्नित bsp_cpu_id;

संरचना cpu cpus[CONFIG_MAX_CPUS];

/* info passed to another cpu along with a sched ipi */
संरचना sched_ipi_दत्त {
	चञ्चल u32_t	flags;
	चञ्चल u32_t	दत्त;
};

स्थिर संरचना sched_ipi_दत्त  sched_ipi_दत्त[CONFIG_MAX_CPUS];

#परिभाषय SCHED_IPI_STOP_PROC	1
#परिभाषय SCHED_IPI_VM_INHIBIT	2
#परिभाषय SCHED_IPI_SAVE_CTX	4

स्थिर चञ्चल अचिह्नित ap_cpus_booted;

SPINLOCK_DEFINE(big_kernel_तालकम्)
SPINLOCK_DEFINE(boot_तालकम्)

शून्य wait_for_APs_गन्तव्यम्_finish_booting(शून्य)
{
	अचिह्नित n = 0;
	पूर्णाङ्क i;

	/* check how many cpus are actually alive */
	प्रति (i = 0 ; i < ncpus ; i++) {
		यदि (cpu_test_flag(i, CPU_IS_READY))
			n++;
	}
	यदि (n != ncpus)
		प्रारूप_लिख("WARNING only %d out of %d cpus booted\n", n, ncpus);

	/* we must let the other CPUs to run in kernel mode first */
	BKL_UNLOCK();
	यावत् (ap_cpus_booted != (n - 1))
		arch_pause();
	/* now we have to take the lock again as we continue execution */
	BKL_तालकम्();
}

शून्य ap_boot_परिसमाप्तम्(अचिह्नित cpu)
{
	ap_cpus_booted++;
}

शून्य smp_ipi_halt_handler(शून्य)
{
	ipi_ack();
	stop_local_timer();
	arch_smp_halt_cpu();
}

शून्य smp_कालसारणी(अचिह्नित cpu)
{
	arch_send_smp_कालसारणी_ipi(cpu);
}

शून्य smp_sched_handler(शून्य);

/*
 * tell another cpu about a task to do and return only after the cpu acks that
 * the task is finished. Also wait before it finishes task sent by another cpu
 * to the same one.
 */
स्थिर शून्य smp_कालसारणी_sync(संरचना proc * p, अचिह्नित task)
{
	अचिह्नित cpu = p->p_cpu;
	अचिह्नित mycpu = cpuid;

	प्रतिज्ञा(cpu != mycpu);
	/*
	 * if some other cpu made a request to the same cpu, wait until it is
	 * done before proceeding
	 */
	यदि (sched_ipi_दत्त[cpu].flags != 0) {
		BKL_UNLOCK();
		यावत् (sched_ipi_दत्त[cpu].flags != 0) {
			यदि (sched_ipi_दत्त[mycpu].flags) {
				BKL_तालकम्();
				smp_sched_handler();
				BKL_UNLOCK();
			}
		}
		BKL_तालकम्();
	}

	sched_ipi_दत्त[cpu].दत्त = (u32_t) p;
	sched_ipi_दत्त[cpu].flags |= task;
	__insn_barrier();
	arch_send_smp_कालसारणी_ipi(cpu);

	/* wait until the destination cpu finishes its job */
	BKL_UNLOCK();
	यावत् (sched_ipi_दत्त[cpu].flags != 0) {
		यदि (sched_ipi_दत्त[mycpu].flags) {
			BKL_तालकम्();
			smp_sched_handler();
			BKL_UNLOCK();
		}
	}
	BKL_तालकम्();
}

शून्य smp_कालसारणी_stop_proc(संरचना proc * p)
{
	यदि (proc_is_runnable(p))
		smp_कालसारणी_sync(p, SCHED_IPI_STOP_PROC);
	अन्यथा
		RTS_समुच्चयः(p, RTS_PROC_STOP);
	प्रतिज्ञा(RTS_ISSET(p, RTS_PROC_STOP));
}

शून्य smp_कालसारणी_vminhibit(संरचना proc * p)
{
	यदि (proc_is_runnable(p))
		smp_कालसारणी_sync(p, SCHED_IPI_VM_INHIBIT);
	अन्यथा
		RTS_समुच्चयः(p, RTS_VMINHIBIT);
	प्रतिज्ञा(RTS_ISSET(p, RTS_VMINHIBIT));
}

शून्य smp_कालसारणी_stop_proc_save_ctx(संरचना proc * p)
{
	/*
	 * stop the processes and force the complete context of the process to
	 * be saved (i.e. including FPU state and such)
	 */
	smp_कालसारणी_sync(p, SCHED_IPI_STOP_PROC | SCHED_IPI_SAVE_CTX);
	प्रतिज्ञा(RTS_ISSET(p, RTS_PROC_STOP));
}

शून्य smp_कालसारणी_migrate_proc(संरचना proc * p, अचिह्नित dest_cpu)
{
	/*
	 * stop the processes and force the complete context of the process to
	 * be saved (i.e. including FPU state and such)
	 */
	smp_कालसारणी_sync(p, SCHED_IPI_STOP_PROC | SCHED_IPI_SAVE_CTX);
	प्रतिज्ञा(RTS_ISSET(p, RTS_PROC_STOP));
	
	/* assign the new cpu and let the process run again */
	p->p_cpu = dest_cpu;
	RTS_UNSET(p, RTS_PROC_STOP);
}

शून्य smp_sched_handler(शून्य)
{
	अचिह्नित flgs;
	अचिह्नित cpu = cpuid;

	flgs = sched_ipi_दत्त[cpu].flags;

	यदि (flgs) {
		संरचना proc * p;
		p = (संरचना proc *)sched_ipi_दत्त[cpu].दत्त;

		यदि (flgs & SCHED_IPI_STOP_PROC) {
			RTS_समुच्चयः(p, RTS_PROC_STOP);
		}
		यदि (flgs & SCHED_IPI_SAVE_CTX) {
			/* all context has been saved already, FPU remains */
			यदि (proc_प्रयुक्तम्_fpu(p) &&
					get_cpulocal_var(fpu_owner) == p) {
				disable_fpu_exception();
				save_local_fpu(p, FALSE /*retain*/);
				/* we're preparing to migrate somewhere else */
				release_fpu(p);
			}
		}
		यदि (flgs & SCHED_IPI_VM_INHIBIT) {
			RTS_समुच्चयः(p, RTS_VMINHIBIT);
		}
	}

	__insn_barrier();
	sched_ipi_दत्त[cpu].flags = 0;
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
	यदि (curr->p_endpoint != IDLE) {
		RTS_समुच्चयः(curr, RTS_PREEMPTED);
	}
}

