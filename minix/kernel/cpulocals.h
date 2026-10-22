/* Implementation of CPU local variables generics */
#اگر_تعریف_نشده __CPULOCALS_H__
#تعریف_کن __CPULOCALS_H__

#اگر_تعریف_نشده __ASSEMBLY__

#اگر_تعریف_شده CONFIG_SMP

/* SMP */

#تعریف_کن CPULOCAL_ARRAY	[CONFIG_MAX_CPUS]

#تعریف_کن get_cpu_var(cpu, name)		__cpu_local_vars[cpu].name
#تعریف_کن get_cpu_var_ptr(cpu, name)	(&(get_cpu_var(cpu, name)))
#تعریف_کن get_cpulocal_var(name)		get_cpu_var(cpuid, name)
#تعریف_کن get_cpulocal_var_ptr(name)	get_cpu_var_ptr(cpuid, name)

/* FIXME - padd the structure so that items in the array do not share cacheline
 * with other cpus */

#وإلا

/* single CPU */

#تعریف_کن CPULOCAL_ARRAY

#تعریف_کن get_cpulocal_var(name)		__cpu_local_vars.name
#تعریف_کن get_cpulocal_var_ptr(name)	&(get_cpulocal_var(name))
#تعریف_کن get_cpu_var(cpu, name)		get_cpulocal_var(name)
#تعریف_کن get_cpu_var_ptr(cpu, name)	get_cpulocal_var_ptr(name)

#پایان_اگر

/*
 * The global cpu local variables in use
 */
خارجي بنية __cpu_local_vars {

/* Process scheduling information and the kernel reentry count. */
	بنية proc *proc_ptr;/* pointer to currently running process */
	بنية proc *bill_ptr;/* process to bill for clock ticks */
	بنية proc idle_proc;/* stub for an idle process */

/* 
 * signal whether pagefault is already being handled to detect recursive
 * pagefaults
 */
	صحيح pagefault_handled;

/*
 * which processpage tables are loaded right now. We need to know this because
 * some processes are loaded in each process pagetables and don't have their own
 * pagetables. Therefore we cannot use the proc_ptr pointer
 */
	بنية proc * ptproc;

/* CPU private run queues */
	بنية proc * پیاپی_q_سر[NR_SCHED_QUEUES]; /* ptrs to ready list headers */
	بنية proc * پیاپی_q_دم[NR_SCHED_QUEUES]; /* ptrs to ready list tails */
	صحيح cpu_is_idle; /* let the others know that you are idle */

	صحيح idle_interrupted; /* to interrupt busy-idle
						     while profiling */

	u64_t tsc_ctr_switch; /* when did we switched time accounting */

/* last values read from cpu when sending ooq msg to scheduler */
	u64_t cpu_آخرین_tsc;
	u64_t cpu_آخرین_idle;


	حرف fpu_presence; /* whether the cpu has FPU or not */
	بنية proc * fpu_owner; /* who owns the FPU of the local cpu */

} __cpu_local_vars CPULOCAL_ARRAY;

#پایان_اگر /* __ASSEMBLY__ */

#پایان_اگر /* __CPULOCALS_H__ */
