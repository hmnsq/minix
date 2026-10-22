/* Implementation of CPU local variables generics */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ __CPULOCALS_H__
#ವ್ಯಾಖ್ಯಾನಿಸು __CPULOCALS_H__

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ __ASSEMBLY__

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_SMP

/* SMP */

#ವ್ಯಾಖ್ಯಾನಿಸು CPULOCAL_ARRAY	[CONFIG_MAX_CPUS]

#ವ್ಯಾಖ್ಯಾನಿಸು get_cpu_var(cpu, name)		__cpu_local_vars[cpu].name
#ವ್ಯಾಖ್ಯಾನಿಸು get_cpu_var_ptr(cpu, name)	(&(get_cpu_var(cpu, name)))
#ವ್ಯಾಖ್ಯಾನಿಸು get_cpulocal_var(name)		get_cpu_var(cpuid, name)
#ವ್ಯಾಖ್ಯಾನಿಸು get_cpulocal_var_ptr(name)	get_cpu_var_ptr(cpuid, name)

/* FIXME - padd the structure so that items in the array do not share cacheline
 * with other cpus */

#ಇಲ್ಲದಿದ್ದರೆ

/* single CPU */

#ವ್ಯಾಖ್ಯಾನಿಸು CPULOCAL_ARRAY

#ವ್ಯಾಖ್ಯಾನಿಸು get_cpulocal_var(name)		__cpu_local_vars.name
#ವ್ಯಾಖ್ಯಾನಿಸು get_cpulocal_var_ptr(name)	&(get_cpulocal_var(name))
#ವ್ಯಾಖ್ಯಾನಿಸು get_cpu_var(cpu, name)		get_cpulocal_var(name)
#ವ್ಯಾಖ್ಯಾನಿಸು get_cpu_var_ptr(cpu, name)	get_cpulocal_var_ptr(name)

#ಆದರೆ_ಅಂತ್ಯ

/*
 * The global cpu local variables in use
 */
ಬಾಹ್ಯ ರಚನೆ __cpu_local_vars {

/* Process scheduling information and the kernel reentry count. */
	ರಚನೆ proc *proc_ptr;/* pointer to currently running process */
	ರಚನೆ proc *bill_ptr;/* process to bill for clock ticks */
	ರಚನೆ proc idle_proc;/* stub for an idle process */

/* 
 * signal whether pagefault is already being handled to detect recursive
 * pagefaults
 */
	ಪೂರ್ಣಾಂಕ pagefault_handled;

/*
 * which processpage tables are loaded right now. We need to know this because
 * some processes are loaded in each process pagetables and don't have their own
 * pagetables. Therefore we cannot use the proc_ptr pointer
 */
	ರಚನೆ proc * ptproc;

/* CPU private run queues */
	ರಚನೆ proc * ಸತತ_q_ತಲೆ[NR_SCHED_QUEUES]; /* ptrs to ready list headers */
	ರಚನೆ proc * ಸತತ_q_ಬಾಲ[NR_SCHED_QUEUES]; /* ptrs to ready list tails */
	ಪೂರ್ಣಾಂಕ cpu_is_idle; /* let the others know that you are idle */

	ಪೂರ್ಣಾಂಕ idle_interrupted; /* to interrupt busy-idle
						     while profiling */

	u64_t tsc_ctr_switch; /* when did we switched time accounting */

/* last values read from cpu when sending ooq msg to scheduler */
	u64_t cpu_ಕೊನೆಯ_tsc;
	u64_t cpu_ಕೊನೆಯ_idle;


	ಅಕ್ಷರ fpu_presence; /* whether the cpu has FPU or not */
	ರಚನೆ proc * fpu_owner; /* who owns the FPU of the local cpu */

} __cpu_local_vars CPULOCAL_ARRAY;

#ಆದರೆ_ಅಂತ್ಯ /* __ASSEMBLY__ */

#ಆದರೆ_ಅಂತ್ಯ /* __CPULOCALS_H__ */
