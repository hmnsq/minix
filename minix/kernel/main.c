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
#ಅನ್ತರ್ಭಾವ <string.h>
#ಅನ್ತರ್ಭಾವ <stdlib.h>
#ಅನ್ತರ್ಭಾವ <assert.h>
#ಅನ್ತರ್ಭಾವ <minix/endpoint.h>
#ಅನ್ತರ್ಭಾವ <machine/vmparam.h>
#ಅನ್ತರ್ಭಾವ <minix/u64.h>
#ಅನ್ತರ್ಭಾವ <minix/board.h>
#ಅನ್ತರ್ಭಾವ <sys/reboot.h>
#ಅನ್ತರ್ಭಾವ "clock.h"
#ಅನ್ತರ್ಭಾವ "direct_utils.h"
#ಅನ್ತರ್ಭಾವ "hw_intr.h"
#ಅನ್ತರ್ಭಾವ "arch_proto.h"

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_SMP
#ಅನ್ತರ್ಭಾವ "smp.h"
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_WATCHDOG
#ಅನ್ತರ್ಭಾವ "watchdog.h"
#ಆದರೆ_ಅಂತ್ಯ
#ಅನ್ತರ್ಭಾವ "spinlock.h"

/* dummy for linking */
ಅಕ್ಷರ *** _penviron;

/* Prototype declarations for PRIVATE functions. */
ಸ್ಥಿರ ಶೂನ್ಯ announce(ಶೂನ್ಯ);

ಶೂನ್ಯ bsp_ಮುಕ್ತಾಯ_booting(ಶೂನ್ಯ)
{
  ಪೂರ್ಣಾಂಕ i;
#ಆದರೆ SPROFILE
  sprofiling = 0;      /* we're not profiling until instructed to */
#ಆದರೆ_ಅಂತ್ಯ /* SPROFILE */

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
  ಗಾಗಿ (i=0; i < NR_BOOT_PROCS - NR_TASKS; i++) {
	RTS_UNSET(proc_addr(i), RTS_PROC_STOP);
  }
  /*
   * Enable timer interrupts and clock task on the boot CPU.  First reset the
   * CPU accounting values, as the timer initialization (indirectly) uses them.
   */
  ಚಕ್ರಗಳು_accounting_init();

  ಆದರೆ (boot_cpu_init_timer(system_hz)) {
	  panic("FATAL : failed to initialize timer interrupts, "
			  "cannot continue without any clock source!");
  }

  fpu_init();

/* Warnings for sanity checks that take time. These warnings are printed
 * so it's a clear warning no full release should be done with them
 * enabled.
 */
#ಆದರೆ DEBUG_SCHED_CHECK
  FIXME("DEBUG_SCHED_CHECK enabled");
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ DEBUG_VMASSERT
  FIXME("DEBUG_VMASSERT enabled");
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ DEBUG_PROC_CHECK
  FIXME("PROC check enabled");
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_SMP
  cpu_ಗಣ_flag(bsp_cpu_id, CPU_IS_READY);
  machine.processors_ಎಣಿಕೆ = ncpus;
  machine.bsp_id = bsp_cpu_id;
#ಇಲ್ಲದಿದ್ದರೆ
  machine.processors_ಎಣಿಕೆ = 1;
  machine.bsp_id = 0;
#ಆದರೆ_ಅಂತ್ಯ
  

  /* Kernel may no longer use bits of memory as VM will be running soon */
  kernel_may_alloc = 0;

  switch_ಗಮ್ಯ_user();
  NOT_REACHABLE;
}


/*===========================================================================*
 *			kmain 	                             		*
 *===========================================================================*/
ಶೂನ್ಯ kmain(kinfo_t *local_cbi)
{
/* Start the ball rolling. */
  ರಚನೆ boot_image *ip;	/* boot image pointer */
  ನೋಂದಣಿ ರಚನೆ proc *rp;	/* process pointer */
  ನೋಂದಣಿ ಪೂರ್ಣಾಂಕ i, j;
  ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ bss_test;

  /* bss sanity check */
  ಪ್ರತಿಜ್ಞಾ(bss_test == 0);
  bss_test = 1;

  /* save a global copy of the boot parameters */
  ಸ್ಮರಣೆ_ನಕಲು(&kinfo, local_cbi, ಗಾತ್ರ(kinfo));
  ಸ್ಮರಣೆ_ನಕಲು(&kmess, kinfo.kmess, ಗಾತ್ರ(kmess));

   /* We have done this exercise in pre_init so we expect this code
      to simply work! */
   machine.board_id = get_board_id_by_name(env_get(BOARDVARNAME));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __arm__
  /* We want to initialize serial before we do any output */
  arch_ser_init();
#ಆದರೆ_ಅಂತ್ಯ
  /* We can talk now */
  DEBUGBASIC(("MINIX booting\n"));

  /* Kernel may use bits of main memory before VM is started */
  kernel_may_alloc = 1;

  ಪ್ರತಿಜ್ಞಾ(ಗಾತ್ರ(kinfo.boot_procs) == ಗಾತ್ರ(image));
  ಸ್ಮರಣೆ_ನಕಲು(kinfo.boot_procs, image, ಗಾತ್ರ(kinfo.boot_procs));

  cstart();

  BKL_ಬೀಗ();
 
   DEBUGEXTRA(("main()\n"));

  /* Clear the process table. Anounce each slot as empty and set up mappings
   * for proc_addr() and proc_nr() macros. Do the same for the table with
   * privilege structures for the system processes and the ipc filter pool.
   */
  proc_init();
  IPCF_POOL_INIT();

   ಆದರೆ(NR_BOOT_MODULES != kinfo.mbi.mi_mods_ಎಣಿಕೆ)
   	panic("expecting %d boot processes/modules, found %d",
		NR_BOOT_MODULES, kinfo.mbi.mi_mods_ಎಣಿಕೆ);

  /* Set up proc table entries for processes in boot image. */
  ಗಾಗಿ (i=0; i < NR_BOOT_PROCS; ++i) {
	ಪೂರ್ಣಾಂಕ schedulable_proc;
	proc_nr_t proc_nr;
	ಪೂರ್ಣಾಂಕ ipc_ಗಮ್ಯ_m, kcalls;
	sys_map_t map;

	ip = &image[i];				/* process' attributes */
	DEBUGEXTRA(("initializing %s... ", ip->proc_name));
	rp = proc_addr(ip->proc_nr);		/* get process pointer */
	ip->endpoint = rp->p_endpoint;		/* ipc endpoint */
	rp->p_cpu_ಸಮಯ_ಎಡ = 0;
	ಆದರೆ(i < NR_TASKS)			/* name (tasks only) */
		strlcpy(rp->p_name, ip->proc_name, ಗಾತ್ರ(rp->p_name));

	ಆದರೆ(i >= NR_TASKS) {
		/* Remember this so it can be passed to VM */
		multiboot_module_t *mb_mod = &kinfo.module_list[i - NR_TASKS];
		ip->ಆರಂಭ_addr = mb_mod->mod_ಆರಂಭ;
		ip->len = mb_mod->mod_ಕೊನೆ - mb_mod->mod_ಆರಂಭ;
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
	ಆದರೆ(schedulable_proc) {
	    /* Assign privilege structure. Force a static privilege id. */
            (ಶೂನ್ಯ) get_priv(rp, static_priv_id(proc_nr));

            /* Privileges for kernel tasks. */
	    ಆದರೆ(proc_nr == VM_PROC_NR) {
                priv(rp)->s_flags = VM_F;
                priv(rp)->s_trap_ಮುಖವಾಡ = SRV_T;
		ipc_ಗಮ್ಯ_m = SRV_M;
		kcalls = SRV_KC;
                priv(rp)->s_sig_mgr = SELF;
                rp->p_ಆದ್ಯತೆ = SRV_Q;
                rp->p_ಕಾಲಾಂಶ_ಗಾತ್ರ_ms = SRV_QT;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(iskerneln(proc_nr)) {
                /* Privilege flags. */
                priv(rp)->s_flags = (proc_nr == IDLE ? IDL_F : TSK_F);
                /* Init flags. */
                priv(rp)->s_init_flags = TSK_I;
                /* Allowed traps. */
                priv(rp)->s_trap_ಮುಖವಾಡ = (proc_nr == CLOCK 
                    || proc_nr == SYSTEM  ? CSK_T : TSK_T);
                ipc_ಗಮ್ಯ_m = TSK_M;                  /* allowed targets */
                kcalls = TSK_KC;                   /* allowed kernel calls */
            }
            /* Privileges for the root system process. */
            ಇಲ್ಲದಿದ್ದರೆ {
	    	ಪ್ರತಿಜ್ಞಾ(isrootsysn(proc_nr));
                priv(rp)->s_flags= RSYS_F;        /* privilege flags */
                priv(rp)->s_init_flags = SRV_I;   /* init flags */
                priv(rp)->s_trap_ಮುಖವಾಡ= SRV_T;     /* allowed traps */
                ipc_ಗಮ್ಯ_m = SRV_M;                 /* allowed targets */
                kcalls = SRV_KC;                  /* allowed kernel calls */
                priv(rp)->s_sig_mgr = SRV_SM;     /* signal manager */
                rp->p_ಆದ್ಯತೆ = SRV_Q;	          /* priority queue */
                rp->p_ಕಾಲಾಂಶ_ಗಾತ್ರ_ms = SRV_QT;   /* quantum size */
            }

            /* Fill in target mask. */
            memset(&map, 0, ಗಾತ್ರ(map));

            ಆದರೆ (ipc_ಗಮ್ಯ_m == ALL_M) {
                ಗಾಗಿ(j = 0; j < NR_SYS_PROCS; j++)
                    ಗಣ_sys_ಬಿಟ್(map, j);
            }

            fill_sendto_ಮುಖವಾಡ(rp, &map);

            /* Fill in kernel call mask. */
            ಗಾಗಿ(j = 0; j < SYS_CALL_ಮುಖವಾಡ_ಗಾತ್ರ; j++) {
                priv(rp)->s_k_call_ಮುಖವಾಡ[j] = (kcalls == NO_C ? 0 : (~0));
            }
	}
	ಇಲ್ಲದಿದ್ದರೆ {
	    /* Don't let the process run for now. */
            RTS_ಗಣ(rp, RTS_NO_PRIV | RTS_NO_ಕಾಲಾಂಶ);
	}

	/* Arch-specific state initialization. */
	arch_boot_proc(ip, rp);

	/* scheduling functions depend on proc_ptr pointing somewhere. */
	ಆದರೆ(!get_cpulocal_var(proc_ptr))
		get_cpulocal_var(proc_ptr) = rp;

	/* Process isn't scheduled until VM has set up a pagetable for it. */
	ಆದರೆ(rp->p_nr != VM_PROC_NR && rp->p_nr >= 0) {
		rp->p_rts_flags |= RTS_VMINHIBIT;
		rp->p_rts_flags |= RTS_BOOTINHIBIT;
	}

	rp->p_rts_flags |= RTS_PROC_STOP;
	rp->p_rts_flags &= ~RTS_ಸ್ಥಾನ_FREE;
	DEBUGEXTRA(("done\n"));
  }

  /* update boot procs info for VM */
  ಸ್ಮರಣೆ_ನಕಲು(kinfo.boot_procs, image, ಗಾತ್ರ(kinfo.boot_procs));

#ವ್ಯಾಖ್ಯಾನಿಸು IPCNAME(n) { \
	ಪ್ರತಿಜ್ಞಾ((n) >= 0 && (n) <= IPCNO_HIGHEST); \
	ಪ್ರತಿಜ್ಞಾ(!ipc_call_ಹೆಸರುಗಳು[n]);	\
	ipc_call_ಹೆಸರುಗಳು[n] = #n; \
}

  arch_post_init();

  IPCNAME(SEND);
  IPCNAME(RECEIVE);
  IPCNAME(SENDREC);
  IPCNAME(NOTIFY);
  IPCNAME(SENDNB);
  IPCNAME(SENDA);

  /* System and processes initialization */
  ಸ್ಮರಣೆ_init();
  DEBUGEXTRA(("system_init()... "));
  system_init();
  DEBUGEXTRA(("done\n"));

  /* The bootstrap phase is over, so we can add the physical
   * memory used for it to the free list.
   */
  ಕೂಡಿಸು_memmap(&kinfo, kinfo.bootstrap_ಆರಂಭ, kinfo.bootstrap_len);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_SMP
  ಆದರೆ (config_no_apic) {
	  DEBUGBASIC(("APIC disabled, disables SMP, using legacy PIC\n"));
	  smp_single_cpu_fallback();
  } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (config_no_smp) {
	  DEBUGBASIC(("SMP disabled, using legacy PIC\n"));
	  smp_single_cpu_fallback();
  } ಇಲ್ಲದಿದ್ದರೆ {
	  smp_init();
	  /*
	   * if smp_init() returns it means that it failed and we try to finish
	   * single CPU booting
	   */
	  bsp_ಮುಕ್ತಾಯ_booting();
  }
#ಇಲ್ಲದಿದ್ದರೆ
  /* 
   * if configured for a single CPU, we are already on the kernel stack which we
   * are going to use everytime we execute kernel code. We finish booting and we
   * never return here
   */
  bsp_ಮುಕ್ತಾಯ_booting();
#ಆದರೆ_ಅಂತ್ಯ

  NOT_REACHABLE;
}

/*===========================================================================*
 *				announce				     *
 *===========================================================================*/
ಸ್ಥಿರ ಶೂನ್ಯ announce(ಶೂನ್ಯ)
{
  /* Display the MINIX startup banner. */
  ಬರೆ("\nMINIX %s. "
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ PAE
"(PAE) "
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ _VCS_REVISION
	"(" _VCS_REVISION ")\n"
#ಆದರೆ_ಅಂತ್ಯ
      "Copyright 2016, Vrije Universiteit, Amsterdam, The Netherlands\n",
      OS_RELEASE);
  ಬರೆ("MINIX is open source software, see http://www.minix3.org\n");
}

/*===========================================================================*
 *				prepare_shutdown			     *
 *===========================================================================*/
ಶೂನ್ಯ prepare_shutdown(ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ how)
{
/* This function prepares to shutdown MINIX. */
  ಸ್ಥಿರ minix_timer_t shutdown_timer;

  /* Continue after 1 second, to give processes a chance to get scheduled to 
   * do shutdown work.  Set a watchog timer to call shutdown(). The timer 
   * argument passes the shutdown status. 
   */
  ಬರೆ("MINIX will now be shut down ...\n");
  ಗಣ_kernel_timer(&shutdown_timer, get_monotonic() + system_hz,
      minix_shutdown, how);
}

/*===========================================================================*
 *				shutdown 				     *
 *===========================================================================*/
ಶೂನ್ಯ minix_shutdown(ಪೂರ್ಣಾಂಕ how)
{
/* This function is called from prepare_shutdown or stop_sequence to bring 
 * down MINIX.
 */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_SMP
  /* 
   * FIXME
   *
   * we will need to stop timers on all cpus if SMP is enabled and put them in
   * such a state that we can perform the whole boot process once restarted from
   * monitor again
   */
  ಆದರೆ (ncpus > 1)
	  smp_shutdown_aps();
#ಆದರೆ_ಅಂತ್ಯ
  hw_intr_disable_all();
  stop_local_timer();

  /* Show shutdown message */
  ನೇರ_cls();
  ಆದರೆ((how & RB_POWERDOWN) == RB_POWERDOWN)
	ನೇರ_print("MINIX has halted and will now power off.\n");
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(how & RB_HALT)
	ನೇರ_print("MINIX has halted. "
		     "It is safe to turn off your computer.\n");
  ಇಲ್ಲದಿದ್ದರೆ
	ನೇರ_print("MINIX will now reset.\n");
  arch_shutdown(how);
}

/*===========================================================================*
 *				cstart					     *
 *===========================================================================*/
ಶೂನ್ಯ cstart(ಶೂನ್ಯ)
{
/* Perform system initializations prior to calling main(). Most settings are
 * determined with help of the environment strings passed by MINIX' loader.
 */
  ನೋಂದಣಿ ಅಕ್ಷರ *ಮೌಲ್ಯ;				/* value in key=value pair */

  /* low-level initialization */
  prot_init();

  /* determine verbosity */
  ಆದರೆ ((ಮೌಲ್ಯ = env_get(VERBOSEBOOTVARNAME)))
	  verboseboot = atoi(ಮೌಲ್ಯ);

  /* Initialize clock variables. */
  init_clock();

  /* Get memory parameters. */
  ಮೌಲ್ಯ = env_get("ac_layout");
  ಆದರೆ(ಮೌಲ್ಯ && atoi(ಮೌಲ್ಯ)) {
        kinfo.user_sp = (vir_ಬೈಟುಗಳು) USR_STACKTOP_COMPACT;
        kinfo.user_ಕೊನೆ = (vir_ಬೈಟುಗಳು) USR_DATATOP_COMPACT;
  }

  DEBUGEXTRA(("cstart\n"));

  /* Record miscellaneous information for user-space servers. */
  kinfo.nr_procs = NR_PROCS;
  kinfo.nr_tasks = NR_TASKS;
  strlcpy(kinfo.release, OS_RELEASE, ಗಾತ್ರ(kinfo.release));
  strlcpy(kinfo.ಆವೃತ್ತಿ, OS_ಆವೃತ್ತಿ, ಗಾತ್ರ(kinfo.ಆವೃತ್ತಿ));

  /* Initialize various user-mapped structures. */
  memset(&arm_frclock, 0, ಗಾತ್ರ(arm_frclock));

  memset(&kuserinfo, 0, ಗಾತ್ರ(kuserinfo));
  kuserinfo.kui_ಗಾತ್ರ = ಗಾತ್ರ(kuserinfo);
  kuserinfo.kui_user_sp = kinfo.user_sp;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_APIC
  ಮೌಲ್ಯ = env_get("no_apic");
  ಆದರೆ(ಮೌಲ್ಯ)
	config_no_apic = atoi(ಮೌಲ್ಯ);
  ಇಲ್ಲದಿದ್ದರೆ
	config_no_apic = 1;
  ಮೌಲ್ಯ = env_get("apic_timer_x");
  ಆದರೆ(ಮೌಲ್ಯ)
	config_apic_timer_x = atoi(ಮೌಲ್ಯ);
  ಇಲ್ಲದಿದ್ದರೆ
	config_apic_timer_x = 1;
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_WATCHDOG
  ಮೌಲ್ಯ = env_get("watchdog");
  ಆದರೆ (ಮೌಲ್ಯ)
	  watchdog_enabled = atoi(ಮೌಲ್ಯ);
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_SMP
  ಆದರೆ (config_no_apic)
	  config_no_smp = 1;
  ಮೌಲ್ಯ = env_get("no_smp");
  ಆದರೆ(ಮೌಲ್ಯ)
	config_no_smp = atoi(ಮೌಲ್ಯ);
  ಇಲ್ಲದಿದ್ದರೆ
	config_no_smp = 0;
#ಆದರೆ_ಅಂತ್ಯ
  DEBUGEXTRA(("intr_init(0)\n"));

  intr_init(0);

  arch_init();
}

/*===========================================================================*
 *				get_value				     *
 *===========================================================================*/

ಅಕ್ಷರ *get_ಮೌಲ್ಯ(
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *params,			/* boot monitor parameters */
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name			/* key to look up */
)
{
/* Get environment value - kernel version of getenv to avoid setting up the
 * usual environment array.
 */
  ನೋಂದಣಿ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *namep;
  ನೋಂದಣಿ ಅಕ್ಷರ *envp;

  ಗಾಗಿ (envp = (ಅಕ್ಷರ *) params; *envp != 0;) {
	ಗಾಗಿ (namep = name; *namep != 0 && *namep == *envp; namep++, envp++)
		;
	ಆದರೆ (*namep == '\0' && *envp == '=') ಹಿಂತಿರುಗಿ(envp + 1);
	ಆಗ (*envp++ != 0)
		;
  }
  ಹಿಂತಿರುಗಿ(ಖಾಲಿ);
}

/*===========================================================================*
 *				env_get				     	*
 *===========================================================================*/
ಅಕ್ಷರ *env_get(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name)
{
	ಹಿಂತಿರುಗಿ get_ಮೌಲ್ಯ(kinfo.param_buf, name);
}

ಶೂನ್ಯ cpu_print_freq(ಚಿಹ್ನೆರಹಿತ cpu)
{
        u64_t freq;

        freq = cpu_get_freq(cpu);
        DEBUGBASIC(("CPU %d freq %lu MHz\n", cpu, (ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)(freq / 1000000)));
}

ಪೂರ್ಣಾಂಕ is_fpu(ಶೂನ್ಯ)
{
        ಹಿಂತಿರುಗಿ get_cpulocal_var(fpu_presence);
}

