/* Implementation of CPU local variables generics */
#यदि_अपरिभाषितम् __CPULOCALS_H__
#परिभाषय __CPULOCALS_H__

#यदि_अपरिभाषितम् __ASSEMBLY__

#यदि_परिभाषितम् CONFIG_SMP

/* SMP */

#परिभाषय CPULOCAL_ARRAY	[CONFIG_MAX_CPUS]

#परिभाषय get_cpu_var(cpu, name)		__cpu_local_vars[cpu].name
#परिभाषय get_cpu_var_ptr(cpu, name)	(&(get_cpu_var(cpu, name)))
#परिभाषय get_cpulocal_var(name)		get_cpu_var(cpuid, name)
#परिभाषय get_cpulocal_var_ptr(name)	get_cpu_var_ptr(cpuid, name)

/* FIXME - padd the structure so that items in the array do not share cacheline
 * with other cpus */

#अन्यथा

/* single CPU */

#परिभाषय CPULOCAL_ARRAY

#परिभाषय get_cpulocal_var(name)		__cpu_local_vars.name
#परिभाषय get_cpulocal_var_ptr(name)	&(get_cpulocal_var(name))
#परिभाषय get_cpu_var(cpu, name)		get_cpulocal_var(name)
#परिभाषय get_cpu_var_ptr(cpu, name)	get_cpulocal_var_ptr(name)

#यदि_अन्तः

/*
 * The global cpu local variables in use
 */
बाह्य संरचना __cpu_local_vars {

/* Process scheduling information and the kernel reentry count. */
	संरचना proc *proc_ptr;/* pointer to currently running process */
	संरचना proc *bill_ptr;/* process to bill for clock ticks */
	संरचना proc idle_proc;/* stub for an idle process */

/* 
 * signal whether pagefault is already being handled to detect recursive
 * pagefaults
 */
	पूर्णाङ्क pagefault_handled;

/*
 * which processpage tables are loaded right now. We need to know this because
 * some processes are loaded in each process pagetables and don't have their own
 * pagetables. Therefore we cannot use the proc_ptr pointer
 */
	संरचना proc * ptproc;

/* CPU private run queues */
	संरचना proc * सततम्_q_शीर्ष[NR_SCHED_QUEUES]; /* ptrs to ready list headers */
	संरचना proc * सततम्_q_पुच्छ[NR_SCHED_QUEUES]; /* ptrs to ready list tails */
	पूर्णाङ्क cpu_is_idle; /* let the others know that you are idle */

	पूर्णाङ्क idle_interrupted; /* to interrupt busy-idle
						     while profiling */

	u64_t tsc_ctr_switch; /* when did we switched time accounting */

/* last values read from cpu when sending ooq msg to scheduler */
	u64_t cpu_चरमः_tsc;
	u64_t cpu_चरमः_idle;


	वर्ण fpu_presence; /* whether the cpu has FPU or not */
	संरचना proc * fpu_owner; /* who owns the FPU of the local cpu */

} __cpu_local_vars CPULOCAL_ARRAY;

#यदि_अन्तः /* __ASSEMBLY__ */

#यदि_अन्तः /* __CPULOCALS_H__ */
