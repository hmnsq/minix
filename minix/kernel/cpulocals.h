/* Implementation of CPU local variables generics */
#ifndef __CPULOCALS_H__
#define __CPULOCALS_H__

#ifndef __ASSEMBLY__

#ifdef CONFIG_SMP

/* SMP */

#define CPULOCAL_ARRAY	[CONFIG_MAX_CPUS]

#define get_cpu_var(cpu, name)		__cpu_local_vars[cpu].name
#define get_cpu_var_ptr(cpu, name)	(&(get_cpu_var(cpu, name)))
#define get_cpulocal_var(name)		get_cpu_var(cpuid, name)
#define get_cpulocal_var_ptr(name)	get_cpu_var_ptr(cpuid, name)

/* FIXME - padd the structure so that items in the array do not share cacheline
 * with other cpus */

#অন্যথা

/* single CPU */

#define CPULOCAL_ARRAY

#define get_cpulocal_var(name)		__cpu_local_vars.name
#define get_cpulocal_var_ptr(name)	&(get_cpulocal_var(name))
#define get_cpu_var(cpu, name)		get_cpulocal_var(name)
#define get_cpu_var_ptr(cpu, name)	get_cpulocal_var_ptr(name)

#endif

/*
 * The global cpu local variables in use
 */
বাহ্যিক কাঠামো __cpu_local_vars {

/* Process scheduling information and the kernel reentry count. */
	কাঠামো proc *proc_ptr;/* pointer to currently running process */
	কাঠামো proc *bill_ptr;/* process to bill for clock ticks */
	কাঠামো proc idle_proc;/* stub for an idle process */

/* 
 * signal whether pagefault is already being handled to detect recursive
 * pagefaults
 */
	পূর্ণসংখ্যা pagefault_handled;

/*
 * which processpage tables are loaded right now. We need to know this because
 * some processes are loaded in each process pagetables and don't have their own
 * pagetables. Therefore we cannot use the proc_ptr pointer
 */
	কাঠামো proc * ptproc;

/* CPU private run queues */
	কাঠামো proc * run_q_শীর্ষ[NR_SCHED_QUEUES]; /* ptrs to ready list headers */
	কাঠামো proc * run_q_লেজ[NR_SCHED_QUEUES]; /* ptrs to ready list tails */
	পূর্ণসংখ্যা cpu_is_idle; /* let the others know that you are idle */

	পূর্ণসংখ্যা idle_interrupted; /* to interrupt busy-idle
						     while profiling */

	u64_t tsc_ctr_switch; /* when did we switched time accounting */

/* last values read from cpu when sending ooq msg to scheduler */
	u64_t cpu_last_tsc;
	u64_t cpu_last_idle;


	অক্ষর fpu_presence; /* whether the cpu has FPU or not */
	কাঠামো proc * fpu_owner; /* who owns the FPU of the local cpu */

} __cpu_local_vars CPULOCAL_ARRAY;

#endif /* __ASSEMBLY__ */

#endif /* __CPULOCALS_H__ */
