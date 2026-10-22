/* Implementation of CPU local variables generics */
#నిర్వచితం_కాకపోతే __CPULOCALS_H__
#నిర్వచించు __CPULOCALS_H__

#నిర్వచితం_కాకపోతే __ASSEMBLY__

#నిర్వచితమైతే CONFIG_SMP

/* SMP */

#నిర్వచించు CPULOCAL_ARRAY	[CONFIG_MAX_CPUS]

#నిర్వచించు get_cpu_var(cpu, name)		__cpu_local_vars[cpu].name
#నిర్వచించు get_cpu_var_ptr(cpu, name)	(&(get_cpu_var(cpu, name)))
#నిర్వచించు get_cpulocal_var(name)		get_cpu_var(cpuid, name)
#నిర్వచించు get_cpulocal_var_ptr(name)	get_cpu_var_ptr(cpuid, name)

/* FIXME - padd the structure so that items in the array do not share cacheline
 * with other cpus */

#లేకపోతే

/* single CPU */

#నిర్వచించు CPULOCAL_ARRAY

#నిర్వచించు get_cpulocal_var(name)		__cpu_local_vars.name
#నిర్వచించు get_cpulocal_var_ptr(name)	&(get_cpulocal_var(name))
#నిర్వచించు get_cpu_var(cpu, name)		get_cpulocal_var(name)
#నిర్వచించు get_cpu_var_ptr(cpu, name)	get_cpulocal_var_ptr(name)

#అయితే_ముగింపు

/*
 * The global cpu local variables in use
 */
బాహ్య నిర్మాణం __cpu_local_vars {

/* Process scheduling information and the kernel reentry count. */
	నిర్మాణం proc *proc_ptr;/* pointer to currently running process */
	నిర్మాణం proc *bill_ptr;/* process to bill for clock ticks */
	నిర్మాణం proc idle_proc;/* stub for an idle process */

/* 
 * signal whether pagefault is already being handled to detect recursive
 * pagefaults
 */
	పూర్ణాంకం pagefault_handled;

/*
 * which processpage tables are loaded right now. We need to know this because
 * some processes are loaded in each process pagetables and don't have their own
 * pagetables. Therefore we cannot use the proc_ptr pointer
 */
	నిర్మాణం proc * ptproc;

/* CPU private run queues */
	నిర్మాణం proc * నిరంతరం_q_తల[NR_SCHED_QUEUES]; /* ptrs to ready list headers */
	నిర్మాణం proc * నిరంతరం_q_తోక[NR_SCHED_QUEUES]; /* ptrs to ready list tails */
	పూర్ణాంకం cpu_is_idle; /* let the others know that you are idle */

	పూర్ణాంకం idle_interrupted; /* to interrupt busy-idle
						     while profiling */

	u64_t tsc_ctr_switch; /* when did we switched time accounting */

/* last values read from cpu when sending ooq msg to scheduler */
	u64_t cpu_చివరి_tsc;
	u64_t cpu_చివరి_idle;


	అక్షరం fpu_presence; /* whether the cpu has FPU or not */
	నిర్మాణం proc * fpu_owner; /* who owns the FPU of the local cpu */

} __cpu_local_vars CPULOCAL_ARRAY;

#అయితే_ముగింపు /* __ASSEMBLY__ */

#అయితే_ముగింపు /* __CPULOCALS_H__ */
