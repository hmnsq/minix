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
#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdlib.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <assert.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <minix/endpoint.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <machine/vmparam.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <minix/u64.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <minix/board.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/reboot.h>
#ਅਨ੍ਤਰ੍ਭਾਵ "clock.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "direct_utils.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hw_intr.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "arch_proto.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_SMP
#ਅਨ੍ਤਰ੍ਭਾਵ "smp.h"
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_WATCHDOG
#ਅਨ੍ਤਰ੍ਭਾਵ "watchdog.h"
#ਜੇ_ਅੰਤ
#ਅਨ੍ਤਰ੍ਭਾਵ "spinlock.h"

/* dummy for linking */
ਅੱਖਰ *** _penviron;

/* Prototype declarations for PRIVATE functions. */
ਸਥਿਰ ਖਾਲੀ announce(ਖਾਲੀ);

ਖਾਲੀ bsp_ਅੰਤ_booting(ਖਾਲੀ)
{
  ਪੂਰਨ i;
#ਜੇ SPROFILE
  sprofiling = 0;      /* we're not profiling until instructed to */
#ਜੇ_ਅੰਤ /* SPROFILE */

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
  ਲਈ (i=0; i < NR_BOOT_PROCS - NR_TASKS; i++) {
	RTS_UNSET(proc_addr(i), RTS_PROC_STOP);
  }
  /*
   * Enable timer interrupts and clock task on the boot CPU.  First reset the
   * CPU accounting values, as the timer initialization (indirectly) uses them.
   */
  ਚੱਕਰਾਂ_accounting_init();

  ਜੇ (boot_cpu_init_timer(system_hz)) {
	  panic("FATAL : failed to initialize timer interrupts, "
			  "cannot continue without any clock source!");
  }

  fpu_init();

/* Warnings for sanity checks that take time. These warnings are printed
 * so it's a clear warning no full release should be done with them
 * enabled.
 */
#ਜੇ DEBUG_SCHED_CHECK
  FIXME("DEBUG_SCHED_CHECK enabled");
#ਜੇ_ਅੰਤ
#ਜੇ DEBUG_VMASSERT
  FIXME("DEBUG_VMASSERT enabled");
#ਜੇ_ਅੰਤ
#ਜੇ DEBUG_PROC_CHECK
  FIXME("PROC check enabled");
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_SMP
  cpu_ਸੈੱਟ_flag(bsp_cpu_id, CPU_IS_READY);
  machine.processors_ਗਿਣਤੀ = ncpus;
  machine.bsp_id = bsp_cpu_id;
#ਨਹੀਂਤਾਂ
  machine.processors_ਗਿਣਤੀ = 1;
  machine.bsp_id = 0;
#ਜੇ_ਅੰਤ
  

  /* Kernel may no longer use bits of memory as VM will be running soon */
  kernel_may_alloc = 0;

  switch_ਮੰਜ਼ਿਲ_user();
  NOT_REACHABLE;
}


/*===========================================================================*
 *			kmain 	                             		*
 *===========================================================================*/
ਖਾਲੀ kmain(kinfo_t *local_cbi)
{
/* Start the ball rolling. */
  ਬਣਤਰ boot_image *ip;	/* boot image pointer */
  ਰਜਿਸਟਰ ਬਣਤਰ proc *rp;	/* process pointer */
  ਰਜਿਸਟਰ ਪੂਰਨ i, j;
  ਸਥਿਰ ਪੂਰਨ bss_test;

  /* bss sanity check */
  ਪ੍ਰਤਿਜ੍ਞਾ(bss_test == 0);
  bss_test = 1;

  /* save a global copy of the boot parameters */
  ਸਮ੍ਰਿਤੀ_ਨਕਲ(&kinfo, local_cbi, ਆਕਾਰ(kinfo));
  ਸਮ੍ਰਿਤੀ_ਨਕਲ(&kmess, kinfo.kmess, ਆਕਾਰ(kmess));

   /* We have done this exercise in pre_init so we expect this code
      to simply work! */
   machine.board_id = get_board_id_by_name(env_get(BOARDVARNAME));
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __arm__
  /* We want to initialize serial before we do any output */
  arch_ser_init();
#ਜੇ_ਅੰਤ
  /* We can talk now */
  DEBUGBASIC(("MINIX booting\n"));

  /* Kernel may use bits of main memory before VM is started */
  kernel_may_alloc = 1;

  ਪ੍ਰਤਿਜ੍ਞਾ(ਆਕਾਰ(kinfo.boot_procs) == ਆਕਾਰ(image));
  ਸਮ੍ਰਿਤੀ_ਨਕਲ(kinfo.boot_procs, image, ਆਕਾਰ(kinfo.boot_procs));

  cstart();

  BKL_ਤਾਲਾ();
 
   DEBUGEXTRA(("main()\n"));

  /* Clear the process table. Anounce each slot as empty and set up mappings
   * for proc_addr() and proc_nr() macros. Do the same for the table with
   * privilege structures for the system processes and the ipc filter pool.
   */
  proc_init();
  IPCF_POOL_INIT();

   ਜੇ(NR_BOOT_MODULES != kinfo.mbi.mi_mods_ਗਿਣਤੀ)
   	panic("expecting %d boot processes/modules, found %d",
		NR_BOOT_MODULES, kinfo.mbi.mi_mods_ਗਿਣਤੀ);

  /* Set up proc table entries for processes in boot image. */
  ਲਈ (i=0; i < NR_BOOT_PROCS; ++i) {
	ਪੂਰਨ schedulable_proc;
	proc_nr_t proc_nr;
	ਪੂਰਨ ipc_ਮੰਜ਼ਿਲ_m, kcalls;
	sys_map_t map;

	ip = &image[i];				/* process' attributes */
	DEBUGEXTRA(("initializing %s... ", ip->proc_name));
	rp = proc_addr(ip->proc_nr);		/* get process pointer */
	ip->endpoint = rp->p_endpoint;		/* ipc endpoint */
	rp->p_cpu_ਸਮਾਂ_ਖੱਬਾ = 0;
	ਜੇ(i < NR_TASKS)			/* name (tasks only) */
		strlcpy(rp->p_name, ip->proc_name, ਆਕਾਰ(rp->p_name));

	ਜੇ(i >= NR_TASKS) {
		/* Remember this so it can be passed to VM */
		multiboot_module_t *mb_mod = &kinfo.module_list[i - NR_TASKS];
		ip->ਸ਼ੁਰੂ_addr = mb_mod->mod_ਸ਼ੁਰੂ;
		ip->len = mb_mod->mod_ਸਿਰਾ - mb_mod->mod_ਸ਼ੁਰੂ;
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
	ਜੇ(schedulable_proc) {
	    /* Assign privilege structure. Force a static privilege id. */
            (ਖਾਲੀ) get_priv(rp, static_priv_id(proc_nr));

            /* Privileges for kernel tasks. */
	    ਜੇ(proc_nr == VM_PROC_NR) {
                priv(rp)->s_flags = VM_F;
                priv(rp)->s_trap_ਨਕਾਬ = SRV_T;
		ipc_ਮੰਜ਼ਿਲ_m = SRV_M;
		kcalls = SRV_KC;
                priv(rp)->s_sig_mgr = SELF;
                rp->p_ਤਰਜੀਹ = SRV_Q;
                rp->p_ਸਮਾਂ_ਹਿੱਸਾ_ਮਾਪ_ms = SRV_QT;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ(iskerneln(proc_nr)) {
                /* Privilege flags. */
                priv(rp)->s_flags = (proc_nr == IDLE ? IDL_F : TSK_F);
                /* Init flags. */
                priv(rp)->s_init_flags = TSK_I;
                /* Allowed traps. */
                priv(rp)->s_trap_ਨਕਾਬ = (proc_nr == CLOCK 
                    || proc_nr == SYSTEM  ? CSK_T : TSK_T);
                ipc_ਮੰਜ਼ਿਲ_m = TSK_M;                  /* allowed targets */
                kcalls = TSK_KC;                   /* allowed kernel calls */
            }
            /* Privileges for the root system process. */
            ਨਹੀਂਤਾਂ {
	    	ਪ੍ਰਤਿਜ੍ਞਾ(isrootsysn(proc_nr));
                priv(rp)->s_flags= RSYS_F;        /* privilege flags */
                priv(rp)->s_init_flags = SRV_I;   /* init flags */
                priv(rp)->s_trap_ਨਕਾਬ= SRV_T;     /* allowed traps */
                ipc_ਮੰਜ਼ਿਲ_m = SRV_M;                 /* allowed targets */
                kcalls = SRV_KC;                  /* allowed kernel calls */
                priv(rp)->s_sig_mgr = SRV_SM;     /* signal manager */
                rp->p_ਤਰਜੀਹ = SRV_Q;	          /* priority queue */
                rp->p_ਸਮਾਂ_ਹਿੱਸਾ_ਮਾਪ_ms = SRV_QT;   /* quantum size */
            }

            /* Fill in target mask. */
            memset(&map, 0, ਆਕਾਰ(map));

            ਜੇ (ipc_ਮੰਜ਼ਿਲ_m == ALL_M) {
                ਲਈ(j = 0; j < NR_SYS_PROCS; j++)
                    ਸੈੱਟ_sys_ਦੋਆਧਾਰੀ_ਅੰਕ(map, j);
            }

            fill_sendto_ਨਕਾਬ(rp, &map);

            /* Fill in kernel call mask. */
            ਲਈ(j = 0; j < SYS_CALL_ਨਕਾਬ_ਮਾਪ; j++) {
                priv(rp)->s_k_call_ਨਕਾਬ[j] = (kcalls == NO_C ? 0 : (~0));
            }
	}
	ਨਹੀਂਤਾਂ {
	    /* Don't let the process run for now. */
            RTS_ਸੈੱਟ(rp, RTS_NO_PRIV | RTS_NO_ਸਮਾਂ_ਹਿੱਸਾ);
	}

	/* Arch-specific state initialization. */
	arch_boot_proc(ip, rp);

	/* scheduling functions depend on proc_ptr pointing somewhere. */
	ਜੇ(!get_cpulocal_var(proc_ptr))
		get_cpulocal_var(proc_ptr) = rp;

	/* Process isn't scheduled until VM has set up a pagetable for it. */
	ਜੇ(rp->p_nr != VM_PROC_NR && rp->p_nr >= 0) {
		rp->p_rts_flags |= RTS_VMINHIBIT;
		rp->p_rts_flags |= RTS_BOOTINHIBIT;
	}

	rp->p_rts_flags |= RTS_PROC_STOP;
	rp->p_rts_flags &= ~RTS_ਥਾਂ_FREE;
	DEBUGEXTRA(("done\n"));
  }

  /* update boot procs info for VM */
  ਸਮ੍ਰਿਤੀ_ਨਕਲ(kinfo.boot_procs, image, ਆਕਾਰ(kinfo.boot_procs));

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IPCNAME(n) { \
	ਪ੍ਰਤਿਜ੍ਞਾ((n) >= 0 && (n) <= IPCNO_HIGHEST); \
	ਪ੍ਰਤਿਜ੍ਞਾ(!ipc_call_ਨਾਂ[n]);	\
	ipc_call_ਨਾਂ[n] = #n; \
}

  arch_post_init();

  IPCNAME(SEND);
  IPCNAME(RECEIVE);
  IPCNAME(SENDREC);
  IPCNAME(NOTIFY);
  IPCNAME(SENDNB);
  IPCNAME(SENDA);

  /* System and processes initialization */
  ਸਮ੍ਰਿਤੀ_init();
  DEBUGEXTRA(("system_init()... "));
  system_init();
  DEBUGEXTRA(("done\n"));

  /* The bootstrap phase is over, so we can add the physical
   * memory used for it to the free list.
   */
  ਜੋੜੋ_memmap(&kinfo, kinfo.bootstrap_ਸ਼ੁਰੂ, kinfo.bootstrap_len);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_SMP
  ਜੇ (config_no_apic) {
	  DEBUGBASIC(("APIC disabled, disables SMP, using legacy PIC\n"));
	  smp_single_cpu_fallback();
  } ਨਹੀਂਤਾਂ ਜੇ (config_no_smp) {
	  DEBUGBASIC(("SMP disabled, using legacy PIC\n"));
	  smp_single_cpu_fallback();
  } ਨਹੀਂਤਾਂ {
	  smp_init();
	  /*
	   * if smp_init() returns it means that it failed and we try to finish
	   * single CPU booting
	   */
	  bsp_ਅੰਤ_booting();
  }
#ਨਹੀਂਤਾਂ
  /* 
   * if configured for a single CPU, we are already on the kernel stack which we
   * are going to use everytime we execute kernel code. We finish booting and we
   * never return here
   */
  bsp_ਅੰਤ_booting();
#ਜੇ_ਅੰਤ

  NOT_REACHABLE;
}

/*===========================================================================*
 *				announce				     *
 *===========================================================================*/
ਸਥਿਰ ਖਾਲੀ announce(ਖਾਲੀ)
{
  /* Display the MINIX startup banner. */
  ਲਿਖੋ("\nMINIX %s. "
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ PAE
"(PAE) "
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ _VCS_REVISION
	"(" _VCS_REVISION ")\n"
#ਜੇ_ਅੰਤ
      "Copyright 2016, Vrije Universiteit, Amsterdam, The Netherlands\n",
      OS_RELEASE);
  ਲਿਖੋ("MINIX is open source software, see http://www.minix3.org\n");
}

/*===========================================================================*
 *				prepare_shutdown			     *
 *===========================================================================*/
ਖਾਲੀ prepare_shutdown(ਅਚਲ ਪੂਰਨ how)
{
/* This function prepares to shutdown MINIX. */
  ਸਥਿਰ minix_timer_t shutdown_timer;

  /* Continue after 1 second, to give processes a chance to get scheduled to 
   * do shutdown work.  Set a watchog timer to call shutdown(). The timer 
   * argument passes the shutdown status. 
   */
  ਲਿਖੋ("MINIX will now be shut down ...\n");
  ਸੈੱਟ_kernel_timer(&shutdown_timer, get_monotonic() + system_hz,
      minix_shutdown, how);
}

/*===========================================================================*
 *				shutdown 				     *
 *===========================================================================*/
ਖਾਲੀ minix_shutdown(ਪੂਰਨ how)
{
/* This function is called from prepare_shutdown or stop_sequence to bring 
 * down MINIX.
 */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_SMP
  /* 
   * FIXME
   *
   * we will need to stop timers on all cpus if SMP is enabled and put them in
   * such a state that we can perform the whole boot process once restarted from
   * monitor again
   */
  ਜੇ (ncpus > 1)
	  smp_shutdown_aps();
#ਜੇ_ਅੰਤ
  hw_intr_disable_all();
  stop_local_timer();

  /* Show shutdown message */
  ਸਿੱਧਾ_cls();
  ਜੇ((how & RB_POWERDOWN) == RB_POWERDOWN)
	ਸਿੱਧਾ_print("MINIX has halted and will now power off.\n");
  ਨਹੀਂਤਾਂ ਜੇ(how & RB_HALT)
	ਸਿੱਧਾ_print("MINIX has halted. "
		     "It is safe to turn off your computer.\n");
  ਨਹੀਂਤਾਂ
	ਸਿੱਧਾ_print("MINIX will now reset.\n");
  arch_shutdown(how);
}

/*===========================================================================*
 *				cstart					     *
 *===========================================================================*/
ਖਾਲੀ cstart(ਖਾਲੀ)
{
/* Perform system initializations prior to calling main(). Most settings are
 * determined with help of the environment strings passed by MINIX' loader.
 */
  ਰਜਿਸਟਰ ਅੱਖਰ *ਮੁੱਲ;				/* value in key=value pair */

  /* low-level initialization */
  prot_init();

  /* determine verbosity */
  ਜੇ ((ਮੁੱਲ = env_get(VERBOSEBOOTVARNAME)))
	  verboseboot = atoi(ਮੁੱਲ);

  /* Initialize clock variables. */
  init_clock();

  /* Get memory parameters. */
  ਮੁੱਲ = env_get("ac_layout");
  ਜੇ(ਮੁੱਲ && atoi(ਮੁੱਲ)) {
        kinfo.user_sp = (vir_ਬਾਈਟਾਂ) USR_STACKTOP_COMPACT;
        kinfo.user_ਸਿਰਾ = (vir_ਬਾਈਟਾਂ) USR_DATATOP_COMPACT;
  }

  DEBUGEXTRA(("cstart\n"));

  /* Record miscellaneous information for user-space servers. */
  kinfo.nr_procs = NR_PROCS;
  kinfo.nr_tasks = NR_TASKS;
  strlcpy(kinfo.release, OS_RELEASE, ਆਕਾਰ(kinfo.release));
  strlcpy(kinfo.ਸੰਸਕਰਣ, OS_ਸੰਸਕਰਣ, ਆਕਾਰ(kinfo.ਸੰਸਕਰਣ));

  /* Initialize various user-mapped structures. */
  memset(&arm_frclock, 0, ਆਕਾਰ(arm_frclock));

  memset(&kuserinfo, 0, ਆਕਾਰ(kuserinfo));
  kuserinfo.kui_ਮਾਪ = ਆਕਾਰ(kuserinfo);
  kuserinfo.kui_user_sp = kinfo.user_sp;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_APIC
  ਮੁੱਲ = env_get("no_apic");
  ਜੇ(ਮੁੱਲ)
	config_no_apic = atoi(ਮੁੱਲ);
  ਨਹੀਂਤਾਂ
	config_no_apic = 1;
  ਮੁੱਲ = env_get("apic_timer_x");
  ਜੇ(ਮੁੱਲ)
	config_apic_timer_x = atoi(ਮੁੱਲ);
  ਨਹੀਂਤਾਂ
	config_apic_timer_x = 1;
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_WATCHDOG
  ਮੁੱਲ = env_get("watchdog");
  ਜੇ (ਮੁੱਲ)
	  watchdog_enabled = atoi(ਮੁੱਲ);
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_SMP
  ਜੇ (config_no_apic)
	  config_no_smp = 1;
  ਮੁੱਲ = env_get("no_smp");
  ਜੇ(ਮੁੱਲ)
	config_no_smp = atoi(ਮੁੱਲ);
  ਨਹੀਂਤਾਂ
	config_no_smp = 0;
#ਜੇ_ਅੰਤ
  DEBUGEXTRA(("intr_init(0)\n"));

  intr_init(0);

  arch_init();
}

/*===========================================================================*
 *				get_value				     *
 *===========================================================================*/

ਅੱਖਰ *get_ਮੁੱਲ(
  ਅਚਲ ਅੱਖਰ *params,			/* boot monitor parameters */
  ਅਚਲ ਅੱਖਰ *name			/* key to look up */
)
{
/* Get environment value - kernel version of getenv to avoid setting up the
 * usual environment array.
 */
  ਰਜਿਸਟਰ ਅਚਲ ਅੱਖਰ *namep;
  ਰਜਿਸਟਰ ਅੱਖਰ *envp;

  ਲਈ (envp = (ਅੱਖਰ *) params; *envp != 0;) {
	ਲਈ (namep = name; *namep != 0 && *namep == *envp; namep++, envp++)
		;
	ਜੇ (*namep == '\0' && *envp == '=') ਵਾਪਸ(envp + 1);
	ਜਦੋਂ (*envp++ != 0)
		;
  }
  ਵਾਪਸ(ਨਿਰਾ);
}

/*===========================================================================*
 *				env_get				     	*
 *===========================================================================*/
ਅੱਖਰ *env_get(ਅਚਲ ਅੱਖਰ *name)
{
	ਵਾਪਸ get_ਮੁੱਲ(kinfo.param_buf, name);
}

ਖਾਲੀ cpu_print_freq(ਬਿਨਾਨਿਸ਼ਾਨੀ cpu)
{
        u64_t freq;

        freq = cpu_get_freq(cpu);
        DEBUGBASIC(("CPU %d freq %lu MHz\n", cpu, (ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)(freq / 1000000)));
}

ਪੂਰਨ is_fpu(ਖਾਲੀ)
{
        ਵਾਪਸ get_cpulocal_var(fpu_presence);
}

