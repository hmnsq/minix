/* This file contains the main program of MINIX as well as its shutdown code.
 * The routine main() initializes the system and starts the ball rolling by
 * setting up the process table, interrupt vectors, and scheduling each task 
 * to run to initialize itself.
 * The routine shutdown() does the opposite and brings down MINIX. 
 *
 * The entries into this file are:
 *   main:	    	MINIX main program
 *   prepare_shutdown:	prepare to take MINIX down
 */
#అన్తర్భావ <string.h>
#అన్తర్భావ <stdlib.h>
#అన్తర్భావ <assert.h>
#అన్తర్భావ <minix/endpoint.h>
#అన్తర్భావ <machine/vmparam.h>
#అన్తర్భావ <minix/u64.h>
#అన్తర్భావ <minix/board.h>
#అన్తర్భావ <sys/reboot.h>
#అన్తర్భావ "clock.h"
#అన్తర్భావ "direct_utils.h"
#అన్తర్భావ "hw_intr.h"
#అన్తర్భావ "arch_proto.h"

#నిర్వచితమైతే CONFIG_SMP
#అన్తర్భావ "smp.h"
#అయితే_ముగింపు
#నిర్వచితమైతే USE_WATCHDOG
#అన్తర్భావ "watchdog.h"
#అయితే_ముగింపు
#అన్తర్భావ "spinlock.h"

/* dummy for linking */
అక్షరం *** _penviron;

/* Prototype declarations for PRIVATE functions. */
స్థిర శూన్య announce(శూన్య);

శూన్య bsp_ముగింపు_booting(శూన్య)
{
  పూర్ణాంకం i;
#ఒకవేళ SPROFILE
  sprofiling = 0;      /* we're not profiling until instructed to */
#అయితే_ముగింపు /* SPROFILE */

  cpu_identify();

  vm_running = 0;
  krandom.random_sources = RANDOM_SOURCES;
  krandom.random_elements = RANDOM_ELEMENTS;

  /* MINIX is now ready. All boot image processes are on the ready queue.
   * Return to the assembly code to start running the current process. 
   */
  
  /* it should point somewhere */
  get_cpulocal_var(bill_ptr) = get_cpulocal_var_ptr(idle_proc);
  get_cpulocal_var(proc_ptr) = get_cpulocal_var_ptr(idle_proc);
  announce();				/* print MINIX startup banner */

  /*
   * we have access to the cpu local run queue, only now schedule the processes.
   * We ignore the slots for the former kernel tasks
   */
  కొరకు (i=0; i < NR_BOOT_PROCS - NR_TASKS; i++) {
	RTS_UNSET(proc_addr(i), RTS_PROC_STOP);
  }
  /*
   * Enable timer interrupts and clock task on the boot CPU.  First reset the
   * CPU accounting values, as the timer initialization (indirectly) uses them.
   */
  చక్రాలు_accounting_init();

  ఒకవేళ (boot_cpu_init_timer(system_hz)) {
	  panic("FATAL : failed to initialize timer interrupts, "
			  "cannot continue without any clock source!");
  }

  fpu_init();

/* Warnings for sanity checks that take time. These warnings are printed
 * so it's a clear warning no full release should be done with them
 * enabled.
 */
#ఒకవేళ DEBUG_SCHED_CHECK
  FIXME("DEBUG_SCHED_CHECK enabled");
#అయితే_ముగింపు
#ఒకవేళ DEBUG_VMASSERT
  FIXME("DEBUG_VMASSERT enabled");
#అయితే_ముగింపు
#ఒకవేళ DEBUG_PROC_CHECK
  FIXME("PROC check enabled");
#అయితే_ముగింపు

#నిర్వచితమైతే CONFIG_SMP
  cpu_సమితి_flag(bsp_cpu_id, CPU_IS_READY);
  machine.processors_లెక్క = ncpus;
  machine.bsp_id = bsp_cpu_id;
#లేకపోతే
  machine.processors_లెక్క = 1;
  machine.bsp_id = 0;
#అయితే_ముగింపు
  

  /* Kernel may no longer use bits of memory as VM will be running soon */
  kernel_may_alloc = 0;

  switch_గమ్యం_user();
  NOT_REACHABLE;
}


/*===========================================================================*
 *			kmain 	                             		*
 *===========================================================================*/
శూన్య kmain(kinfo_t *local_cbi)
{
/* Start the ball rolling. */
  నిర్మాణం boot_image *ip;	/* boot image pointer */
  నమోదు నిర్మాణం proc *rp;	/* process pointer */
  నమోదు పూర్ణాంకం i, j;
  స్థిర పూర్ణాంకం bss_test;

  /* bss sanity check */
  ప్రతిజ్ఞా(bss_test == 0);
  bss_test = 1;

  /* save a global copy of the boot parameters */
  జ్ఞాపకం_నకలు(&kinfo, local_cbi, పరిమాణం(kinfo));
  జ్ఞాపకం_నకలు(&kmess, kinfo.kmess, పరిమాణం(kmess));

   /* We have done this exercise in pre_init so we expect this code
      to simply work! */
   machine.board_id = get_board_id_by_name(env_get(BOARDVARNAME));
#నిర్వచితమైతే __arm__
  /* We want to initialize serial before we do any output */
  arch_ser_init();
#అయితే_ముగింపు
  /* We can talk now */
  DEBUGBASIC(("MINIX booting\n"));

  /* Kernel may use bits of main memory before VM is started */
  kernel_may_alloc = 1;

  ప్రతిజ్ఞా(పరిమాణం(kinfo.boot_procs) == పరిమాణం(image));
  జ్ఞాపకం_నకలు(kinfo.boot_procs, image, పరిమాణం(kinfo.boot_procs));

  cstart();

  BKL_తాళం();
 
   DEBUGEXTRA(("main()\n"));

  /* Clear the process table. Anounce each slot as empty and set up mappings
   * for proc_addr() and proc_nr() macros. Do the same for the table with
   * privilege structures for the system processes and the ipc filter pool.
   */
  proc_init();
  IPCF_POOL_INIT();

   ఒకవేళ(NR_BOOT_MODULES != kinfo.mbi.mi_mods_లెక్క)
   	panic("expecting %d boot processes/modules, found %d",
		NR_BOOT_MODULES, kinfo.mbi.mi_mods_లెక్క);

  /* Set up proc table entries for processes in boot image. */
  కొరకు (i=0; i < NR_BOOT_PROCS; ++i) {
	పూర్ణాంకం schedulable_proc;
	proc_nr_t proc_nr;
	పూర్ణాంకం ipc_గమ్యం_m, kcalls;
	sys_map_t map;

	ip = &image[i];				/* process' attributes */
	DEBUGEXTRA(("initializing %s... ", ip->proc_name));
	rp = proc_addr(ip->proc_nr);		/* get process pointer */
	ip->endpoint = rp->p_endpoint;		/* ipc endpoint */
	rp->p_cpu_సమయం_ఎడమ = 0;
	ఒకవేళ(i < NR_TASKS)			/* name (tasks only) */
		strlcpy(rp->p_name, ip->proc_name, పరిమాణం(rp->p_name));

	ఒకవేళ(i >= NR_TASKS) {
		/* Remember this so it can be passed to VM */
		multiboot_module_t *mb_mod = &kinfo.module_list[i - NR_TASKS];
		ip->ప్రారంభం_addr = mb_mod->mod_ప్రారంభం;
		ip->len = mb_mod->mod_చివర - mb_mod->mod_ప్రారంభం;
	}
	
	reset_proc_accounting(rp);

	/* See if this process is immediately schedulable.
	 * In that case, set its privileges now and allow it to run.
	 * Only kernel tasks and the root system process get to run immediately.
	 * All the other system processes are inhibited from running by the
	 * RTS_NO_PRIV flag. They can only be scheduled once the root system
	 * process has set their privileges.
	 */
	proc_nr = proc_nr(rp);
	schedulable_proc = (iskerneln(proc_nr) || isrootsysn(proc_nr) ||
		proc_nr == VM_PROC_NR);
	ఒకవేళ(schedulable_proc) {
	    /* Assign privilege structure. Force a static privilege id. */
            (శూన్య) get_priv(rp, static_priv_id(proc_nr));

            /* Privileges for kernel tasks. */
	    ఒకవేళ(proc_nr == VM_PROC_NR) {
                priv(rp)->s_flags = VM_F;
                priv(rp)->s_trap_ముసుగు = SRV_T;
		ipc_గమ్యం_m = SRV_M;
		kcalls = SRV_KC;
                priv(rp)->s_sig_mgr = SELF;
                rp->p_ప్రాధాన్యత = SRV_Q;
                rp->p_కాలాంశం_కొలత_ms = SRV_QT;
	    }
	    లేకపోతే ఒకవేళ(iskerneln(proc_nr)) {
                /* Privilege flags. */
                priv(rp)->s_flags = (proc_nr == IDLE ? IDL_F : TSK_F);
                /* Init flags. */
                priv(rp)->s_init_flags = TSK_I;
                /* Allowed traps. */
                priv(rp)->s_trap_ముసుగు = (proc_nr == CLOCK 
                    || proc_nr == SYSTEM  ? CSK_T : TSK_T);
                ipc_గమ్యం_m = TSK_M;                  /* allowed targets */
                kcalls = TSK_KC;                   /* allowed kernel calls */
            }
            /* Privileges for the root system process. */
            లేకపోతే {
	    	ప్రతిజ్ఞా(isrootsysn(proc_nr));
                priv(rp)->s_flags= RSYS_F;        /* privilege flags */
                priv(rp)->s_init_flags = SRV_I;   /* init flags */
                priv(rp)->s_trap_ముసుగు= SRV_T;     /* allowed traps */
                ipc_గమ్యం_m = SRV_M;                 /* allowed targets */
                kcalls = SRV_KC;                  /* allowed kernel calls */
                priv(rp)->s_sig_mgr = SRV_SM;     /* signal manager */
                rp->p_ప్రాధాన్యత = SRV_Q;	          /* priority queue */
                rp->p_కాలాంశం_కొలత_ms = SRV_QT;   /* quantum size */
            }

            /* Fill in target mask. */
            memset(&map, 0, పరిమాణం(map));

            ఒకవేళ (ipc_గమ్యం_m == ALL_M) {
                కొరకు(j = 0; j < NR_SYS_PROCS; j++)
                    సమితి_sys_బిట్టు(map, j);
            }

            fill_sendto_ముసుగు(rp, &map);

            /* Fill in kernel call mask. */
            కొరకు(j = 0; j < SYS_CALL_ముసుగు_కొలత; j++) {
                priv(rp)->s_k_call_ముసుగు[j] = (kcalls == NO_C ? 0 : (~0));
            }
	}
	లేకపోతే {
	    /* Don't let the process run for now. */
            RTS_సమితి(rp, RTS_NO_PRIV | RTS_NO_కాలాంశం);
	}

	/* Arch-specific state initialization. */
	arch_boot_proc(ip, rp);

	/* scheduling functions depend on proc_ptr pointing somewhere. */
	ఒకవేళ(!get_cpulocal_var(proc_ptr))
		get_cpulocal_var(proc_ptr) = rp;

	/* Process isn't scheduled until VM has set up a pagetable for it. */
	ఒకవేళ(rp->p_nr != VM_PROC_NR && rp->p_nr >= 0) {
		rp->p_rts_flags |= RTS_VMINHIBIT;
		rp->p_rts_flags |= RTS_BOOTINHIBIT;
	}

	rp->p_rts_flags |= RTS_PROC_STOP;
	rp->p_rts_flags &= ~RTS_స్థానం_FREE;
	DEBUGEXTRA(("done\n"));
  }

  /* update boot procs info for VM */
  జ్ఞాపకం_నకలు(kinfo.boot_procs, image, పరిమాణం(kinfo.boot_procs));

#నిర్వచించు IPCNAME(n) { \
	ప్రతిజ్ఞా((n) >= 0 && (n) <= IPCNO_HIGHEST); \
	ప్రతిజ్ఞా(!ipc_call_పేర్లు[n]);	\
	ipc_call_పేర్లు[n] = #n; \
}

  arch_post_init();

  IPCNAME(SEND);
  IPCNAME(RECEIVE);
  IPCNAME(SENDREC);
  IPCNAME(NOTIFY);
  IPCNAME(SENDNB);
  IPCNAME(SENDA);

  /* System and processes initialization */
  జ్ఞాపకం_init();
  DEBUGEXTRA(("system_init()... "));
  system_init();
  DEBUGEXTRA(("done\n"));

  /* The bootstrap phase is over, so we can add the physical
   * memory used for it to the free list.
   */
  కూడు_memmap(&kinfo, kinfo.bootstrap_ప్రారంభం, kinfo.bootstrap_len);

#నిర్వచితమైతే CONFIG_SMP
  ఒకవేళ (config_no_apic) {
	  DEBUGBASIC(("APIC disabled, disables SMP, using legacy PIC\n"));
	  smp_single_cpu_fallback();
  } లేకపోతే ఒకవేళ (config_no_smp) {
	  DEBUGBASIC(("SMP disabled, using legacy PIC\n"));
	  smp_single_cpu_fallback();
  } లేకపోతే {
	  smp_init();
	  /*
	   * if smp_init() returns it means that it failed and we try to finish
	   * single CPU booting
	   */
	  bsp_ముగింపు_booting();
  }
#లేకపోతే
  /* 
   * if configured for a single CPU, we are already on the kernel stack which we
   * are going to use everytime we execute kernel code. We finish booting and we
   * never return here
   */
  bsp_ముగింపు_booting();
#అయితే_ముగింపు

  NOT_REACHABLE;
}

/*===========================================================================*
 *				announce				     *
 *===========================================================================*/
స్థిర శూన్య announce(శూన్య)
{
  /* Display the MINIX startup banner. */
  రాయి("\nMINIX %s. "
#నిర్వచితమైతే PAE
"(PAE) "
#అయితే_ముగింపు
#నిర్వచితమైతే _VCS_REVISION
	"(" _VCS_REVISION ")\n"
#అయితే_ముగింపు
      "Copyright 2016, Vrije Universiteit, Amsterdam, The Netherlands\n",
      OS_RELEASE);
  రాయి("MINIX is open source software, see http://www.minix3.org\n");
}

/*===========================================================================*
 *				prepare_shutdown			     *
 *===========================================================================*/
శూన్య prepare_shutdown(స్థిరరాశి పూర్ణాంకం how)
{
/* This function prepares to shutdown MINIX. */
  స్థిర minix_timer_t shutdown_timer;

  /* Continue after 1 second, to give processes a chance to get scheduled to 
   * do shutdown work.  Set a watchog timer to call shutdown(). The timer 
   * argument passes the shutdown status. 
   */
  రాయి("MINIX will now be shut down ...\n");
  సమితి_kernel_timer(&shutdown_timer, get_monotonic() + system_hz,
      minix_shutdown, how);
}

/*===========================================================================*
 *				shutdown 				     *
 *===========================================================================*/
శూన్య minix_shutdown(పూర్ణాంకం how)
{
/* This function is called from prepare_shutdown or stop_sequence to bring 
 * down MINIX.
 */

#నిర్వచితమైతే CONFIG_SMP
  /* 
   * FIXME
   *
   * we will need to stop timers on all cpus if SMP is enabled and put them in
   * such a state that we can perform the whole boot process once restarted from
   * monitor again
   */
  ఒకవేళ (ncpus > 1)
	  smp_shutdown_aps();
#అయితే_ముగింపు
  hw_intr_disable_all();
  stop_local_timer();

  /* Show shutdown message */
  ప్రత్యక్ష_cls();
  ఒకవేళ((how & RB_POWERDOWN) == RB_POWERDOWN)
	ప్రత్యక్ష_print("MINIX has halted and will now power off.\n");
  లేకపోతే ఒకవేళ(how & RB_HALT)
	ప్రత్యక్ష_print("MINIX has halted. "
		     "It is safe to turn off your computer.\n");
  లేకపోతే
	ప్రత్యక్ష_print("MINIX will now reset.\n");
  arch_shutdown(how);
}

/*===========================================================================*
 *				cstart					     *
 *===========================================================================*/
శూన్య cstart(శూన్య)
{
/* Perform system initializations prior to calling main(). Most settings are
 * determined with help of the environment strings passed by MINIX' loader.
 */
  నమోదు అక్షరం *విలువ;				/* value in key=value pair */

  /* low-level initialization */
  prot_init();

  /* determine verbosity */
  ఒకవేళ ((విలువ = env_get(VERBOSEBOOTVARNAME)))
	  verboseboot = atoi(విలువ);

  /* Initialize clock variables. */
  init_clock();

  /* Get memory parameters. */
  విలువ = env_get("ac_layout");
  ఒకవేళ(విలువ && atoi(విలువ)) {
        kinfo.user_sp = (vir_బైట్లు) USR_STACKTOP_COMPACT;
        kinfo.user_చివర = (vir_బైట్లు) USR_DATATOP_COMPACT;
  }

  DEBUGEXTRA(("cstart\n"));

  /* Record miscellaneous information for user-space servers. */
  kinfo.nr_procs = NR_PROCS;
  kinfo.nr_tasks = NR_TASKS;
  strlcpy(kinfo.release, OS_RELEASE, పరిమాణం(kinfo.release));
  strlcpy(kinfo.సంస్కరణ, OS_సంస్కరణ, పరిమాణం(kinfo.సంస్కరణ));

  /* Initialize various user-mapped structures. */
  memset(&arm_frclock, 0, పరిమాణం(arm_frclock));

  memset(&kuserinfo, 0, పరిమాణం(kuserinfo));
  kuserinfo.kui_కొలత = పరిమాణం(kuserinfo);
  kuserinfo.kui_user_sp = kinfo.user_sp;

#నిర్వచితమైతే USE_APIC
  విలువ = env_get("no_apic");
  ఒకవేళ(విలువ)
	config_no_apic = atoi(విలువ);
  లేకపోతే
	config_no_apic = 1;
  విలువ = env_get("apic_timer_x");
  ఒకవేళ(విలువ)
	config_apic_timer_x = atoi(విలువ);
  లేకపోతే
	config_apic_timer_x = 1;
#అయితే_ముగింపు

#నిర్వచితమైతే USE_WATCHDOG
  విలువ = env_get("watchdog");
  ఒకవేళ (విలువ)
	  watchdog_enabled = atoi(విలువ);
#అయితే_ముగింపు

#నిర్వచితమైతే CONFIG_SMP
  ఒకవేళ (config_no_apic)
	  config_no_smp = 1;
  విలువ = env_get("no_smp");
  ఒకవేళ(విలువ)
	config_no_smp = atoi(విలువ);
  లేకపోతే
	config_no_smp = 0;
#అయితే_ముగింపు
  DEBUGEXTRA(("intr_init(0)\n"));

  intr_init(0);

  arch_init();
}

/*===========================================================================*
 *				get_value				     *
 *===========================================================================*/

అక్షరం *get_విలువ(
  స్థిరరాశి అక్షరం *params,			/* boot monitor parameters */
  స్థిరరాశి అక్షరం *name			/* key to look up */
)
{
/* Get environment value - kernel version of getenv to avoid setting up the
 * usual environment array.
 */
  నమోదు స్థిరరాశి అక్షరం *namep;
  నమోదు అక్షరం *envp;

  కొరకు (envp = (అక్షరం *) params; *envp != 0;) {
	కొరకు (namep = name; *namep != 0 && *namep == *envp; namep++, envp++)
		;
	ఒకవేళ (*namep == '\0' && *envp == '=') తిరిగి(envp + 1);
	అయితే (*envp++ != 0)
		;
  }
  తిరిగి(ఖాళీ);
}

/*===========================================================================*
 *				env_get				     	*
 *===========================================================================*/
అక్షరం *env_get(స్థిరరాశి అక్షరం *name)
{
	తిరిగి get_విలువ(kinfo.param_buf, name);
}

శూన్య cpu_print_freq(గుర్తులేని cpu)
{
        u64_t freq;

        freq = cpu_get_freq(cpu);
        DEBUGBASIC(("CPU %d freq %lu MHz\n", cpu, (గుర్తులేని దీర్ఘ)(freq / 1000000)));
}

పూర్ణాంకం is_fpu(శూన్య)
{
        తిరిగి get_cpulocal_var(fpu_presence);
}

