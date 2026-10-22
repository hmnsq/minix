#شامل <assert.h>

#شامل "smp.h"
#شامل "interrupt.h"
#شامل "clock.h"

غیرعلامتی ncpus;
غیرعلامتی ht_فی_core;
غیرعلامتی bsp_cpu_id;

ساخت cpu cpus[CONFIG_MAX_CPUS];

/* info passed to another cpu along with a sched ipi */
ساخت sched_ipi_ڈیٹا {
	متلون u32_t	flags;
	متلون u32_t	ڈیٹا;
};

ساکن ساخت sched_ipi_ڈیٹا  sched_ipi_ڈیٹا[CONFIG_MAX_CPUS];

#تعریف_کرو SCHED_IPI_STOP_PROC	1
#تعریف_کرو SCHED_IPI_VM_INHIBIT	2
#تعریف_کرو SCHED_IPI_SAVE_CTX	4

ساکن متلون غیرعلامتی ap_cpus_booted;

SPINLOCK_DEFINE(big_kernel_تالا)
SPINLOCK_DEFINE(boot_تالا)

خالی wait_for_APs_مقصد_اختتام_booting(خالی)
{
	غیرعلامتی n = 0;
	صحیح i;

	/* check how many cpus are actually alive */
	برائے (i = 0 ; i < ncpus ; i++) {
		اگر (cpu_test_flag(i, CPU_IS_READY))
			n++;
	}
	اگر (n != ncpus)
		لکھو("WARNING only %d out of %d cpus booted\n", n, ncpus);

	/* we must let the other CPUs to run in kernel mode first */
	BKL_UNLOCK();
	جب (ap_cpus_booted != (n - 1))
		arch_pause();
	/* now we have to take the lock again as we continue execution */
	BKL_تالا();
}

خالی ap_boot_ختم_شدہ(غیرعلامتی cpu)
{
	ap_cpus_booted++;
}

خالی smp_ipi_halt_handler(خالی)
{
	ipi_ack();
	stop_local_timer();
	arch_smp_halt_cpu();
}

خالی smp_جدول_وقت(غیرعلامتی cpu)
{
	arch_send_smp_جدول_وقت_ipi(cpu);
}

خالی smp_sched_handler(خالی);

/*
 * tell another cpu about a task to do and return only after the cpu acks that
 * the task is finished. Also wait before it finishes task sent by another cpu
 * to the same one.
 */
ساکن خالی smp_جدول_وقت_sync(ساخت proc * p, غیرعلامتی task)
{
	غیرعلامتی cpu = p->p_cpu;
	غیرعلامتی mycpu = cpuid;

	أكد(cpu != mycpu);
	/*
	 * if some other cpu made a request to the same cpu, wait until it is
	 * done before proceeding
	 */
	اگر (sched_ipi_ڈیٹا[cpu].flags != 0) {
		BKL_UNLOCK();
		جب (sched_ipi_ڈیٹا[cpu].flags != 0) {
			اگر (sched_ipi_ڈیٹا[mycpu].flags) {
				BKL_تالا();
				smp_sched_handler();
				BKL_UNLOCK();
			}
		}
		BKL_تالا();
	}

	sched_ipi_ڈیٹا[cpu].ڈیٹا = (u32_t) p;
	sched_ipi_ڈیٹا[cpu].flags |= task;
	__insn_barrier();
	arch_send_smp_جدول_وقت_ipi(cpu);

	/* wait until the destination cpu finishes its job */
	BKL_UNLOCK();
	جب (sched_ipi_ڈیٹا[cpu].flags != 0) {
		اگر (sched_ipi_ڈیٹا[mycpu].flags) {
			BKL_تالا();
			smp_sched_handler();
			BKL_UNLOCK();
		}
	}
	BKL_تالا();
}

خالی smp_جدول_وقت_stop_proc(ساخت proc * p)
{
	اگر (proc_is_runnable(p))
		smp_جدول_وقت_sync(p, SCHED_IPI_STOP_PROC);
	ورنہ
		RTS_سیٹ(p, RTS_PROC_STOP);
	أكد(RTS_ISSET(p, RTS_PROC_STOP));
}

خالی smp_جدول_وقت_vminhibit(ساخت proc * p)
{
	اگر (proc_is_runnable(p))
		smp_جدول_وقت_sync(p, SCHED_IPI_VM_INHIBIT);
	ورنہ
		RTS_سیٹ(p, RTS_VMINHIBIT);
	أكد(RTS_ISSET(p, RTS_VMINHIBIT));
}

خالی smp_جدول_وقت_stop_proc_save_ctx(ساخت proc * p)
{
	/*
	 * stop the processes and force the complete context of the process to
	 * be saved (i.e. including FPU state and such)
	 */
	smp_جدول_وقت_sync(p, SCHED_IPI_STOP_PROC | SCHED_IPI_SAVE_CTX);
	أكد(RTS_ISSET(p, RTS_PROC_STOP));
}

خالی smp_جدول_وقت_migrate_proc(ساخت proc * p, غیرعلامتی dest_cpu)
{
	/*
	 * stop the processes and force the complete context of the process to
	 * be saved (i.e. including FPU state and such)
	 */
	smp_جدول_وقت_sync(p, SCHED_IPI_STOP_PROC | SCHED_IPI_SAVE_CTX);
	أكد(RTS_ISSET(p, RTS_PROC_STOP));
	
	/* assign the new cpu and let the process run again */
	p->p_cpu = dest_cpu;
	RTS_UNSET(p, RTS_PROC_STOP);
}

خالی smp_sched_handler(خالی)
{
	غیرعلامتی flgs;
	غیرعلامتی cpu = cpuid;

	flgs = sched_ipi_ڈیٹا[cpu].flags;

	اگر (flgs) {
		ساخت proc * p;
		p = (ساخت proc *)sched_ipi_ڈیٹا[cpu].ڈیٹا;

		اگر (flgs & SCHED_IPI_STOP_PROC) {
			RTS_سیٹ(p, RTS_PROC_STOP);
		}
		اگر (flgs & SCHED_IPI_SAVE_CTX) {
			/* all context has been saved already, FPU remains */
			اگر (proc_مستعمل_fpu(p) &&
					get_cpulocal_var(fpu_owner) == p) {
				disable_fpu_exception();
				save_local_fpu(p, FALSE /*retain*/);
				/* we're preparing to migrate somewhere else */
				release_fpu(p);
			}
		}
		اگر (flgs & SCHED_IPI_VM_INHIBIT) {
			RTS_سیٹ(p, RTS_VMINHIBIT);
		}
	}

	__insn_barrier();
	sched_ipi_ڈیٹا[cpu].flags = 0;
}

/*
 * This function gets always called only after smp_sched_handler() has been
 * already called. It only serves the purpose of acknowledging the IPI and
 * preempting the current process if the CPU was not idle.
 */
خالی smp_ipi_sched_handler(خالی)
{
	ساخت proc * curr;

	ipi_ack();

	curr = get_cpulocal_var(proc_ptr);
	اگر (curr->p_endpoint != IDLE) {
		RTS_سیٹ(curr, RTS_PREEMPTED);
	}
}

