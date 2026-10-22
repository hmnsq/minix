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
#অন্তর্ভাৱ <string.h>
#অন্তর্ভাৱ <stdlib.h>
#অন্তর্ভাৱ <assert.h>
#অন্তর্ভাৱ <minix/endpoint.h>
#অন্তর্ভাৱ <machine/vmparam.h>
#অন্তর্ভাৱ <minix/u64.h>
#অন্তর্ভাৱ <minix/board.h>
#অন্তর্ভাৱ <sys/reboot.h>
#অন্তর্ভাৱ "clock.h"
#অন্তর্ভাৱ "direct_utils.h"
#অন্তর্ভাৱ "hw_intr.h"
#অন্তর্ভাৱ "arch_proto.h"

#ifdef CONFIG_SMP
#অন্তর্ভাৱ "smp.h"
#endif
#ifdef USE_WATCHDOG
#অন্তর্ভাৱ "watchdog.h"
#endif
#অন্তর্ভাৱ "spinlock.h"

/* dummy for linking */
ৱর্ণ *** _penviron;

/* Prototype declarations for PRIVATE functions. */
স্থির শূন্য announce(শূন্য);

শূন্য bsp_finish_booting(শূন্য)
{
  পূর্ণাঙ্ক i;
#যদি SPROFILE
  sprofiling = 0;      /* we're not profiling until instructed to */
#endif /* SPROFILE */

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
  বাবে (i=0; i < NR_BOOT_PROCS - NR_TASKS; i++) {
	RTS_UNSET(proc_addr(i), RTS_PROC_STOP);
  }
  /*
   * Enable timer interrupts and clock task on the boot CPU.  First reset the
   * CPU accounting values, as the timer initialization (indirectly) uses them.
   */
  cycles_accounting_init();

  যদি (boot_cpu_init_timer(system_hz)) {
	  panic("FATAL : failed to initialize timer interrupts, "
			  "cannot continue without any clock source!");
  }

  fpu_init();

/* Warnings for sanity checks that take time. These warnings are printed
 * so it's a clear warning no full release should be done with them
 * enabled.
 */
#যদি DEBUG_SCHED_CHECK
  FIXME("DEBUG_SCHED_CHECK enabled");
#endif
#যদি DEBUG_VMASSERT
  FIXME("DEBUG_VMASSERT enabled");
#endif
#যদি DEBUG_PROC_CHECK
  FIXME("PROC check enabled");
#endif

#ifdef CONFIG_SMP
  cpu_set_flag(bsp_cpu_id, CPU_IS_READY);
  machine.processors_গুনতি = ncpus;
  machine.bsp_id = bsp_cpu_id;
#অন্যথা
  machine.processors_গুনতি = 1;
  machine.bsp_id = 0;
#endif
  

  /* Kernel may no longer use bits of memory as VM will be running soon */
  kernel_may_alloc = 0;

  switch_to_user();
  NOT_REACHABLE;
}


/*===========================================================================*
 *			kmain 	                             		*
 *===========================================================================*/
শূন্য kmain(kinfo_t *local_cbi)
{
/* Start the ball rolling. */
  সংরচনা boot_image *ip;	/* boot image pointer */
  পঞ্জিকা সংরচনা proc *rp;	/* process pointer */
  পঞ্জিকা পূর্ণাঙ্ক i, j;
  স্থির পূর্ণাঙ্ক bss_test;

  /* bss sanity check */
  প্রতিজ্ঞা(bss_test == 0);
  bss_test = 1;

  /* save a global copy of the boot parameters */
  memcpy(&kinfo, local_cbi, পরিমাণ(kinfo));
  memcpy(&kmess, kinfo.kmess, পরিমাণ(kmess));

   /* We have done this exercise in pre_init so we expect this code
      to simply work! */
   machine.board_id = get_board_id_by_name(env_get(BOARDVARNAME));
#ifdef __arm__
  /* We want to initialize serial before we do any output */
  arch_ser_init();
#endif
  /* We can talk now */
  DEBUGBASIC(("MINIX booting\n"));

  /* Kernel may use bits of main memory before VM is started */
  kernel_may_alloc = 1;

  প্রতিজ্ঞা(পরিমাণ(kinfo.boot_procs) == পরিমাণ(image));
  memcpy(kinfo.boot_procs, image, পরিমাণ(kinfo.boot_procs));

  cstart();

  BKL_LOCK();
 
   DEBUGEXTRA(("main()\n"));

  /* Clear the process table. Anounce each slot as empty and set up mappings
   * for proc_addr() and proc_nr() macros. Do the same for the table with
   * privilege structures for the system processes and the ipc filter pool.
   */
  proc_init();
  IPCF_POOL_INIT();

   যদি(NR_BOOT_MODULES != kinfo.mbi.mi_mods_গুনতি)
   	panic("expecting %d boot processes/modules, found %d",
		NR_BOOT_MODULES, kinfo.mbi.mi_mods_গুনতি);

  /* Set up proc table entries for processes in boot image. */
  বাবে (i=0; i < NR_BOOT_PROCS; ++i) {
	পূর্ণাঙ্ক schedulable_proc;
	proc_nr_t proc_nr;
	পূর্ণাঙ্ক ipc_to_m, kcalls;
	sys_map_t map;

	ip = &image[i];				/* process' attributes */
	DEBUGEXTRA(("initializing %s... ", ip->proc_name));
	rp = proc_addr(ip->proc_nr);		/* get process pointer */
	ip->endpoint = rp->p_endpoint;		/* ipc endpoint */
	rp->p_cpu_time_বাম = 0;
	যদি(i < NR_TASKS)			/* name (tasks only) */
		strlcpy(rp->p_name, ip->proc_name, পরিমাণ(rp->p_name));

	যদি(i >= NR_TASKS) {
		/* Remember this so it can be passed to VM */
		multiboot_module_t *mb_mod = &kinfo.module_list[i - NR_TASKS];
		ip->start_addr = mb_mod->mod_start;
		ip->len = mb_mod->mod_end - mb_mod->mod_start;
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
	যদি(schedulable_proc) {
	    /* Assign privilege structure. Force a static privilege id. */
            (শূন্য) get_priv(rp, static_priv_id(proc_nr));

            /* Privileges for kernel tasks. */
	    যদি(proc_nr == VM_PROC_NR) {
                priv(rp)->s_flags = VM_F;
                priv(rp)->s_trap_mask = SRV_T;
		ipc_to_m = SRV_M;
		kcalls = SRV_KC;
                priv(rp)->s_sig_mgr = SELF;
                rp->p_priority = SRV_Q;
                rp->p_quantum_size_ms = SRV_QT;
	    }
	    অন্যথা যদি(iskerneln(proc_nr)) {
                /* Privilege flags. */
                priv(rp)->s_flags = (proc_nr == IDLE ? IDL_F : TSK_F);
                /* Init flags. */
                priv(rp)->s_init_flags = TSK_I;
                /* Allowed traps. */
                priv(rp)->s_trap_mask = (proc_nr == CLOCK 
                    || proc_nr == SYSTEM  ? CSK_T : TSK_T);
                ipc_to_m = TSK_M;                  /* allowed targets */
                kcalls = TSK_KC;                   /* allowed kernel calls */
            }
            /* Privileges for the root system process. */
            অন্যথা {
	    	প্রতিজ্ঞা(isrootsysn(proc_nr));
                priv(rp)->s_flags= RSYS_F;        /* privilege flags */
                priv(rp)->s_init_flags = SRV_I;   /* init flags */
                priv(rp)->s_trap_mask= SRV_T;     /* allowed traps */
                ipc_to_m = SRV_M;                 /* allowed targets */
                kcalls = SRV_KC;                  /* allowed kernel calls */
                priv(rp)->s_sig_mgr = SRV_SM;     /* signal manager */
                rp->p_priority = SRV_Q;	          /* priority queue */
                rp->p_quantum_size_ms = SRV_QT;   /* quantum size */
            }

            /* Fill in target mask. */
            memset(&map, 0, পরিমাণ(map));

            যদি (ipc_to_m == ALL_M) {
                বাবে(j = 0; j < NR_SYS_PROCS; j++)
                    set_sys_bit(map, j);
            }

            fill_sendto_mask(rp, &map);

            /* Fill in kernel call mask. */
            বাবে(j = 0; j < SYS_CALL_MASK_SIZE; j++) {
                priv(rp)->s_k_call_mask[j] = (kcalls == NO_C ? 0 : (~0));
            }
	}
	অন্যথা {
	    /* Don't let the process run for now. */
            RTS_SET(rp, RTS_NO_PRIV | RTS_NO_QUANTUM);
	}

	/* Arch-specific state initialization. */
	arch_boot_proc(ip, rp);

	/* scheduling functions depend on proc_ptr pointing somewhere. */
	যদি(!get_cpulocal_var(proc_ptr))
		get_cpulocal_var(proc_ptr) = rp;

	/* Process isn't scheduled until VM has set up a pagetable for it. */
	যদি(rp->p_nr != VM_PROC_NR && rp->p_nr >= 0) {
		rp->p_rts_flags |= RTS_VMINHIBIT;
		rp->p_rts_flags |= RTS_BOOTINHIBIT;
	}

	rp->p_rts_flags |= RTS_PROC_STOP;
	rp->p_rts_flags &= ~RTS_SLOT_FREE;
	DEBUGEXTRA(("done\n"));
  }

  /* update boot procs info for VM */
  memcpy(kinfo.boot_procs, image, পরিমাণ(kinfo.boot_procs));

#define IPCNAME(n) { \
	প্রতিজ্ঞা((n) >= 0 && (n) <= IPCNO_HIGHEST); \
	প্রতিজ্ঞা(!ipc_call_names[n]);	\
	ipc_call_names[n] = #n; \
}

  arch_post_init();

  IPCNAME(SEND);
  IPCNAME(RECEIVE);
  IPCNAME(SENDREC);
  IPCNAME(NOTIFY);
  IPCNAME(SENDNB);
  IPCNAME(SENDA);

  /* System and processes initialization */
  memory_init();
  DEBUGEXTRA(("system_init()... "));
  system_init();
  DEBUGEXTRA(("done\n"));

  /* The bootstrap phase is over, so we can add the physical
   * memory used for it to the free list.
   */
  add_memmap(&kinfo, kinfo.bootstrap_start, kinfo.bootstrap_len);

#ifdef CONFIG_SMP
  যদি (config_no_apic) {
	  DEBUGBASIC(("APIC disabled, disables SMP, using legacy PIC\n"));
	  smp_single_cpu_fallback();
  } অন্যথা যদি (config_no_smp) {
	  DEBUGBASIC(("SMP disabled, using legacy PIC\n"));
	  smp_single_cpu_fallback();
  } অন্যথা {
	  smp_init();
	  /*
	   * if smp_init() returns it means that it failed and we try to finish
	   * single CPU booting
	   */
	  bsp_finish_booting();
  }
#অন্যথা
  /* 
   * if configured for a single CPU, we are already on the kernel stack which we
   * are going to use everytime we execute kernel code. We finish booting and we
   * never return here
   */
  bsp_finish_booting();
#endif

  NOT_REACHABLE;
}

/*===========================================================================*
 *				announce				     *
 *===========================================================================*/
স্থির শূন্য announce(শূন্য)
{
  /* Display the MINIX startup banner. */
  printf("\nMINIX %s. "
#ifdef PAE
"(PAE) "
#endif
#ifdef _VCS_REVISION
	"(" _VCS_REVISION ")\n"
#endif
      "Copyright 2016, Vrije Universiteit, Amsterdam, The Netherlands\n",
      OS_RELEASE);
  printf("MINIX is open source software, see http://www.minix3.org\n");
}

/*===========================================================================*
 *				prepare_shutdown			     *
 *===========================================================================*/
শূন্য prepare_shutdown(ধ্রুৱ পূর্ণাঙ্ক how)
{
/* This function prepares to shutdown MINIX. */
  স্থির minix_timer_t shutdown_timer;

  /* Continue after 1 second, to give processes a chance to get scheduled to 
   * do shutdown work.  Set a watchog timer to call shutdown(). The timer 
   * argument passes the shutdown status. 
   */
  printf("MINIX will now be shut down ...\n");
  set_kernel_timer(&shutdown_timer, get_monotonic() + system_hz,
      minix_shutdown, how);
}

/*===========================================================================*
 *				shutdown 				     *
 *===========================================================================*/
শূন্য minix_shutdown(পূর্ণাঙ্ক how)
{
/* This function is called from prepare_shutdown or stop_sequence to bring 
 * down MINIX.
 */

#ifdef CONFIG_SMP
  /* 
   * FIXME
   *
   * we will need to stop timers on all cpus if SMP is enabled and put them in
   * such a state that we can perform the whole boot process once restarted from
   * monitor again
   */
  যদি (ncpus > 1)
	  smp_shutdown_aps();
#endif
  hw_intr_disable_all();
  stop_local_timer();

  /* Show shutdown message */
  direct_cls();
  যদি((how & RB_POWERDOWN) == RB_POWERDOWN)
	direct_print("MINIX has halted and will now power off.\n");
  অন্যথা যদি(how & RB_HALT)
	direct_print("MINIX has halted. "
		     "It is safe to turn off your computer.\n");
  অন্যথা
	direct_print("MINIX will now reset.\n");
  arch_shutdown(how);
}

/*===========================================================================*
 *				cstart					     *
 *===========================================================================*/
শূন্য cstart(শূন্য)
{
/* Perform system initializations prior to calling main(). Most settings are
 * determined with help of the environment strings passed by MINIX' loader.
 */
  পঞ্জিকা ৱর্ণ *মান;				/* value in key=value pair */

  /* low-level initialization */
  prot_init();

  /* determine verbosity */
  যদি ((মান = env_get(VERBOSEBOOTVARNAME)))
	  verboseboot = atoi(মান);

  /* Initialize clock variables. */
  init_clock();

  /* Get memory parameters. */
  মান = env_get("ac_layout");
  যদি(মান && atoi(মান)) {
        kinfo.user_sp = (vir_bytes) USR_STACKTOP_COMPACT;
        kinfo.user_end = (vir_bytes) USR_DATATOP_COMPACT;
  }

  DEBUGEXTRA(("cstart\n"));

  /* Record miscellaneous information for user-space servers. */
  kinfo.nr_procs = NR_PROCS;
  kinfo.nr_tasks = NR_TASKS;
  strlcpy(kinfo.release, OS_RELEASE, পরিমাণ(kinfo.release));
  strlcpy(kinfo.version, OS_VERSION, পরিমাণ(kinfo.version));

  /* Initialize various user-mapped structures. */
  memset(&arm_frclock, 0, পরিমাণ(arm_frclock));

  memset(&kuserinfo, 0, পরিমাণ(kuserinfo));
  kuserinfo.kui_size = পরিমাণ(kuserinfo);
  kuserinfo.kui_user_sp = kinfo.user_sp;

#ifdef USE_APIC
  মান = env_get("no_apic");
  যদি(মান)
	config_no_apic = atoi(মান);
  অন্যথা
	config_no_apic = 1;
  মান = env_get("apic_timer_x");
  যদি(মান)
	config_apic_timer_x = atoi(মান);
  অন্যথা
	config_apic_timer_x = 1;
#endif

#ifdef USE_WATCHDOG
  মান = env_get("watchdog");
  যদি (মান)
	  watchdog_enabled = atoi(মান);
#endif

#ifdef CONFIG_SMP
  যদি (config_no_apic)
	  config_no_smp = 1;
  মান = env_get("no_smp");
  যদি(মান)
	config_no_smp = atoi(মান);
  অন্যথা
	config_no_smp = 0;
#endif
  DEBUGEXTRA(("intr_init(0)\n"));

  intr_init(0);

  arch_init();
}

/*===========================================================================*
 *				get_value				     *
 *===========================================================================*/

ৱর্ণ *get_মান(
  ধ্রুৱ ৱর্ণ *params,			/* boot monitor parameters */
  ধ্রুৱ ৱর্ণ *name			/* key to look up */
)
{
/* Get environment value - kernel version of getenv to avoid setting up the
 * usual environment array.
 */
  পঞ্জিকা ধ্রুৱ ৱর্ণ *namep;
  পঞ্জিকা ৱর্ণ *envp;

  বাবে (envp = (ৱর্ণ *) params; *envp != 0;) {
	বাবে (namep = name; *namep != 0 && *namep == *envp; namep++, envp++)
		;
	যদি (*namep == '\0' && *envp == '=') ঘূৰাই(envp + 1);
	যেতিয়ালৈ (*envp++ != 0)
		;
  }
  ঘূৰাই(রিক্ত);
}

/*===========================================================================*
 *				env_get				     	*
 *===========================================================================*/
ৱর্ণ *env_get(ধ্রুৱ ৱর্ণ *name)
{
	ঘূৰাই get_মান(kinfo.param_buf, name);
}

শূন্য cpu_print_freq(unsigned cpu)
{
        u64_t freq;

        freq = cpu_get_freq(cpu);
        DEBUGBASIC(("CPU %d freq %lu MHz\n", cpu, (unsigned long)(freq / 1000000)));
}

পূর্ণাঙ্ক is_fpu(শূন্য)
{
        ঘূৰাই get_cpulocal_var(fpu_presence);
}

