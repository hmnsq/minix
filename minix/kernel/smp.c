#ضمن <assert.h>

#ضمن "smp.h"
#ضمن "interrupt.h"
#ضمن "clock.h"

بیعلامت ncpus;
بیعلامت ht_در_هر_core;
بیعلامت bsp_cpu_id;

بنية cpu cpus[CONFIG_MAX_CPUS];

/* info passed to another cpu along with a sched ipi */
بنية sched_ipi_داده {
	متطاير u32_t	flags;
	متطاير u32_t	داده;
};

ساكن بنية sched_ipi_داده  sched_ipi_داده[CONFIG_MAX_CPUS];

#تعریف_کن SCHED_IPI_STOP_PROC	1
#تعریف_کن SCHED_IPI_VM_INHIBIT	2
#تعریف_کن SCHED_IPI_SAVE_CTX	4

ساكن متطاير بیعلامت ap_cpus_booted;

SPINLOCK_DEFINE(big_kernel_قفل)
SPINLOCK_DEFINE(boot_قفل)

فراغ wait_for_APs_مقصد_پایان_booting(فراغ)
{
	بیعلامت n = 0;
	صحيح i;

	/* check how many cpus are actually alive */
	لكل (i = 0 ; i < ncpus ; i++) {
		إذا (cpu_test_flag(i, CPU_IS_READY))
			n++;
	}
	إذا (n != ncpus)
		چاپ_کن("WARNING only %d out of %d cpus booted\n", n, ncpus);

	/* we must let the other CPUs to run in kernel mode first */
	BKL_UNLOCK();
	طالما (ap_cpus_booted != (n - 1))
		arch_pause();
	/* now we have to take the lock again as we continue execution */
	BKL_قفل();
}

فراغ ap_boot_پایانیافته(بیعلامت cpu)
{
	ap_cpus_booted++;
}

فراغ smp_ipi_halt_handler(فراغ)
{
	ipi_ack();
	stop_local_timer();
	arch_smp_halt_cpu();
}

فراغ smp_زمانبندی(بیعلامت cpu)
{
	arch_send_smp_زمانبندی_ipi(cpu);
}

فراغ smp_sched_handler(فراغ);

/*
 * tell another cpu about a task to do and return only after the cpu acks that
 * the task is finished. Also wait before it finishes task sent by another cpu
 * to the same one.
 */
ساكن فراغ smp_زمانبندی_sync(بنية proc * p, بیعلامت task)
{
	بیعلامت cpu = p->p_cpu;
	بیعلامت mycpu = cpuid;

	أكد(cpu != mycpu);
	/*
	 * if some other cpu made a request to the same cpu, wait until it is
	 * done before proceeding
	 */
	إذا (sched_ipi_داده[cpu].flags != 0) {
		BKL_UNLOCK();
		طالما (sched_ipi_داده[cpu].flags != 0) {
			إذا (sched_ipi_داده[mycpu].flags) {
				BKL_قفل();
				smp_sched_handler();
				BKL_UNLOCK();
			}
		}
		BKL_قفل();
	}

	sched_ipi_داده[cpu].داده = (u32_t) p;
	sched_ipi_داده[cpu].flags |= task;
	__insn_barrier();
	arch_send_smp_زمانبندی_ipi(cpu);

	/* wait until the destination cpu finishes its job */
	BKL_UNLOCK();
	طالما (sched_ipi_داده[cpu].flags != 0) {
		إذا (sched_ipi_داده[mycpu].flags) {
			BKL_قفل();
			smp_sched_handler();
			BKL_UNLOCK();
		}
	}
	BKL_قفل();
}

فراغ smp_زمانبندی_stop_proc(بنية proc * p)
{
	إذا (proc_is_runnable(p))
		smp_زمانبندی_sync(p, SCHED_IPI_STOP_PROC);
	وإلا
		RTS_مجموعه(p, RTS_PROC_STOP);
	أكد(RTS_ISSET(p, RTS_PROC_STOP));
}

فراغ smp_زمانبندی_vminhibit(بنية proc * p)
{
	إذا (proc_is_runnable(p))
		smp_زمانبندی_sync(p, SCHED_IPI_VM_INHIBIT);
	وإلا
		RTS_مجموعه(p, RTS_VMINHIBIT);
	أكد(RTS_ISSET(p, RTS_VMINHIBIT));
}

فراغ smp_زمانبندی_stop_proc_save_ctx(بنية proc * p)
{
	/*
	 * stop the processes and force the complete context of the process to
	 * be saved (i.e. including FPU state and such)
	 */
	smp_زمانبندی_sync(p, SCHED_IPI_STOP_PROC | SCHED_IPI_SAVE_CTX);
	أكد(RTS_ISSET(p, RTS_PROC_STOP));
}

فراغ smp_زمانبندی_migrate_proc(بنية proc * p, بیعلامت dest_cpu)
{
	/*
	 * stop the processes and force the complete context of the process to
	 * be saved (i.e. including FPU state and such)
	 */
	smp_زمانبندی_sync(p, SCHED_IPI_STOP_PROC | SCHED_IPI_SAVE_CTX);
	أكد(RTS_ISSET(p, RTS_PROC_STOP));
	
	/* assign the new cpu and let the process run again */
	p->p_cpu = dest_cpu;
	RTS_UNSET(p, RTS_PROC_STOP);
}

فراغ smp_sched_handler(فراغ)
{
	بیعلامت flgs;
	بیعلامت cpu = cpuid;

	flgs = sched_ipi_داده[cpu].flags;

	إذا (flgs) {
		بنية proc * p;
		p = (بنية proc *)sched_ipi_داده[cpu].داده;

		إذا (flgs & SCHED_IPI_STOP_PROC) {
			RTS_مجموعه(p, RTS_PROC_STOP);
		}
		إذا (flgs & SCHED_IPI_SAVE_CTX) {
			/* all context has been saved already, FPU remains */
			إذا (proc_استفاده_شده_fpu(p) &&
					get_cpulocal_var(fpu_owner) == p) {
				disable_fpu_exception();
				save_local_fpu(p, FALSE /*retain*/);
				/* we're preparing to migrate somewhere else */
				release_fpu(p);
			}
		}
		إذا (flgs & SCHED_IPI_VM_INHIBIT) {
			RTS_مجموعه(p, RTS_VMINHIBIT);
		}
	}

	__insn_barrier();
	sched_ipi_داده[cpu].flags = 0;
}

/*
 * This function gets always called only after smp_sched_handler() has been
 * already called. It only serves the purpose of acknowledging the IPI and
 * preempting the current process if the CPU was not idle.
 */
فراغ smp_ipi_sched_handler(فراغ)
{
	بنية proc * curr;

	ipi_ack();

	curr = get_cpulocal_var(proc_ptr);
	إذا (curr->p_endpoint != IDLE) {
		RTS_مجموعه(curr, RTS_PREEMPTED);
	}
}

