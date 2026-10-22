/* Implementation of CPU local variables generics */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ __CPULOCALS_H__
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ __CPULOCALS_H__

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ __ASSEMBLY__

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_SMP

/* SMP */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CPULOCAL_ARRAY	[CONFIG_MAX_CPUS]

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ get_cpu_var(cpu, name)		__cpu_local_vars[cpu].name
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ get_cpu_var_ptr(cpu, name)	(&(get_cpu_var(cpu, name)))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ get_cpulocal_var(name)		get_cpu_var(cpuid, name)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ get_cpulocal_var_ptr(name)	get_cpu_var_ptr(cpuid, name)

/* FIXME - padd the structure so that items in the array do not share cacheline
 * with other cpus */

#ਨਹੀਂਤਾਂ

/* single CPU */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CPULOCAL_ARRAY

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ get_cpulocal_var(name)		__cpu_local_vars.name
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ get_cpulocal_var_ptr(name)	&(get_cpulocal_var(name))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ get_cpu_var(cpu, name)		get_cpulocal_var(name)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ get_cpu_var_ptr(cpu, name)	get_cpulocal_var_ptr(name)

#ਜੇ_ਅੰਤ

/*
 * The global cpu local variables in use
 */
ਬਾਹਰੀ ਬਣਤਰ __cpu_local_vars {

/* Process scheduling information and the kernel reentry count. */
	ਬਣਤਰ proc *proc_ptr;/* pointer to currently running process */
	ਬਣਤਰ proc *bill_ptr;/* process to bill for clock ticks */
	ਬਣਤਰ proc idle_proc;/* stub for an idle process */

/* 
 * signal whether pagefault is already being handled to detect recursive
 * pagefaults
 */
	ਪੂਰਨ pagefault_handled;

/*
 * which processpage tables are loaded right now. We need to know this because
 * some processes are loaded in each process pagetables and don't have their own
 * pagetables. Therefore we cannot use the proc_ptr pointer
 */
	ਬਣਤਰ proc * ptproc;

/* CPU private run queues */
	ਬਣਤਰ proc * ਲੜੀਵਾਰ_q_ਸਿਰ[NR_SCHED_QUEUES]; /* ptrs to ready list headers */
	ਬਣਤਰ proc * ਲੜੀਵਾਰ_q_ਪੂਛ[NR_SCHED_QUEUES]; /* ptrs to ready list tails */
	ਪੂਰਨ cpu_is_idle; /* let the others know that you are idle */

	ਪੂਰਨ idle_interrupted; /* to interrupt busy-idle
						     while profiling */

	u64_t tsc_ctr_switch; /* when did we switched time accounting */

/* last values read from cpu when sending ooq msg to scheduler */
	u64_t cpu_ਆਖਰੀ_tsc;
	u64_t cpu_ਆਖਰੀ_idle;


	ਅੱਖਰ fpu_presence; /* whether the cpu has FPU or not */
	ਬਣਤਰ proc * fpu_owner; /* who owns the FPU of the local cpu */

} __cpu_local_vars CPULOCAL_ARRAY;

#ਜੇ_ਅੰਤ /* __ASSEMBLY__ */

#ਜੇ_ਅੰਤ /* __CPULOCALS_H__ */
