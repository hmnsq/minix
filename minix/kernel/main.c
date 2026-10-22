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
#ضمن <string.h>
#ضمن <stdlib.h>
#ضمن <assert.h>
#ضمن <minix/endpoint.h>
#ضمن <machine/vmparam.h>
#ضمن <minix/u64.h>
#ضمن <minix/board.h>
#ضمن <sys/reboot.h>
#ضمن "clock.h"
#ضمن "direct_utils.h"
#ضمن "hw_intr.h"
#ضمن "arch_proto.h"

#جيڪڏهن_وصف_ٿيل CONFIG_SMP
#ضمن "smp.h"
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل USE_WATCHDOG
#ضمن "watchdog.h"
#جيڪڏهن_پورو
#ضمن "spinlock.h"

/* dummy for linking */
حرف *** _penviron;

/* Prototype declarations for PRIVATE functions. */
ساكن فراغ announce(فراغ);

فراغ bsp_پڄاڻي_booting(فراغ)
{
  صحيح i;
#إذا SPROFILE
  sprofiling = 0;      /* we're not profiling until instructed to */
#جيڪڏهن_پورو /* SPROFILE */

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
  لكل (i=0; i < NR_BOOT_PROCS - NR_TASKS; i++) {
	RTS_UNSET(proc_addr(i), RTS_PROC_STOP);
  }
  /*
   * Enable timer interrupts and clock task on the boot CPU.  First reset the
   * CPU accounting values, as the timer initialization (indirectly) uses them.
   */
  چڪرن_accounting_init();

  إذا (boot_cpu_init_timer(system_hz)) {
	  panic("FATAL : failed to initialize timer interrupts, "
			  "cannot continue without any clock source!");
  }

  fpu_init();

/* Warnings for sanity checks that take time. These warnings are printed
 * so it's a clear warning no full release should be done with them
 * enabled.
 */
#إذا DEBUG_SCHED_CHECK
  FIXME("DEBUG_SCHED_CHECK enabled");
#جيڪڏهن_پورو
#إذا DEBUG_VMASSERT
  FIXME("DEBUG_VMASSERT enabled");
#جيڪڏهن_پورو
#إذا DEBUG_PROC_CHECK
  FIXME("PROC check enabled");
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل CONFIG_SMP
  cpu_سيٽ_flag(bsp_cpu_id, CPU_IS_READY);
  machine.processors_ڳڻپ = ncpus;
  machine.bsp_id = bsp_cpu_id;
#وإلا
  machine.processors_ڳڻپ = 1;
  machine.bsp_id = 0;
#جيڪڏهن_پورو
  

  /* Kernel may no longer use bits of memory as VM will be running soon */
  kernel_may_alloc = 0;

  switch_منزل_user();
  NOT_REACHABLE;
}


/*===========================================================================*
 *			kmain 	                             		*
 *===========================================================================*/
فراغ kmain(kinfo_t *local_cbi)
{
/* Start the ball rolling. */
  بنية boot_image *ip;	/* boot image pointer */
  سجل بنية proc *rp;	/* process pointer */
  سجل صحيح i, j;
  ساكن صحيح bss_test;

  /* bss sanity check */
  أكد(bss_test == 0);
  bss_test = 1;

  /* save a global copy of the boot parameters */
  يادگيري_نقل(&kinfo, local_cbi, حجم(kinfo));
  يادگيري_نقل(&kmess, kinfo.kmess, حجم(kmess));

   /* We have done this exercise in pre_init so we expect this code
      to simply work! */
   machine.board_id = get_board_id_by_name(env_get(BOARDVARNAME));
#جيڪڏهن_وصف_ٿيل __arm__
  /* We want to initialize serial before we do any output */
  arch_ser_init();
#جيڪڏهن_پورو
  /* We can talk now */
  DEBUGBASIC(("MINIX booting\n"));

  /* Kernel may use bits of main memory before VM is started */
  kernel_may_alloc = 1;

  أكد(حجم(kinfo.boot_procs) == حجم(image));
  يادگيري_نقل(kinfo.boot_procs, image, حجم(kinfo.boot_procs));

  cstart();

  BKL_تالو();
 
   DEBUGEXTRA(("main()\n"));

  /* Clear the process table. Anounce each slot as empty and set up mappings
   * for proc_addr() and proc_nr() macros. Do the same for the table with
   * privilege structures for the system processes and the ipc filter pool.
   */
  proc_init();
  IPCF_POOL_INIT();

   إذا(NR_BOOT_MODULES != kinfo.mbi.mi_mods_ڳڻپ)
   	panic("expecting %d boot processes/modules, found %d",
		NR_BOOT_MODULES, kinfo.mbi.mi_mods_ڳڻپ);

  /* Set up proc table entries for processes in boot image. */
  لكل (i=0; i < NR_BOOT_PROCS; ++i) {
	صحيح schedulable_proc;
	proc_nr_t proc_nr;
	صحيح ipc_منزل_m, kcalls;
	sys_map_t map;

	ip = &image[i];				/* process' attributes */
	DEBUGEXTRA(("initializing %s... ", ip->proc_name));
	rp = proc_addr(ip->proc_nr);		/* get process pointer */
	ip->endpoint = rp->p_endpoint;		/* ipc endpoint */
	rp->p_cpu_وقت_کاٻو = 0;
	إذا(i < NR_TASKS)			/* name (tasks only) */
		strlcpy(rp->p_name, ip->proc_name, حجم(rp->p_name));

	إذا(i >= NR_TASKS) {
		/* Remember this so it can be passed to VM */
		multiboot_module_t *mb_mod = &kinfo.module_list[i - NR_TASKS];
		ip->شروعات_addr = mb_mod->mod_شروعات;
		ip->len = mb_mod->mod_پڇاڙي - mb_mod->mod_شروعات;
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
	إذا(schedulable_proc) {
	    /* Assign privilege structure. Force a static privilege id. */
            (فراغ) get_priv(rp, static_priv_id(proc_nr));

            /* Privileges for kernel tasks. */
	    إذا(proc_nr == VM_PROC_NR) {
                priv(rp)->s_flags = VM_F;
                priv(rp)->s_trap_نقاب = SRV_T;
		ipc_منزل_m = SRV_M;
		kcalls = SRV_KC;
                priv(rp)->s_sig_mgr = SELF;
                rp->p_ترجيح = SRV_Q;
                rp->p_وقتي_حصو_سائيز_ms = SRV_QT;
	    }
	    وإلا إذا(iskerneln(proc_nr)) {
                /* Privilege flags. */
                priv(rp)->s_flags = (proc_nr == IDLE ? IDL_F : TSK_F);
                /* Init flags. */
                priv(rp)->s_init_flags = TSK_I;
                /* Allowed traps. */
                priv(rp)->s_trap_نقاب = (proc_nr == CLOCK 
                    || proc_nr == SYSTEM  ? CSK_T : TSK_T);
                ipc_منزل_m = TSK_M;                  /* allowed targets */
                kcalls = TSK_KC;                   /* allowed kernel calls */
            }
            /* Privileges for the root system process. */
            وإلا {
	    	أكد(isrootsysn(proc_nr));
                priv(rp)->s_flags= RSYS_F;        /* privilege flags */
                priv(rp)->s_init_flags = SRV_I;   /* init flags */
                priv(rp)->s_trap_نقاب= SRV_T;     /* allowed traps */
                ipc_منزل_m = SRV_M;                 /* allowed targets */
                kcalls = SRV_KC;                  /* allowed kernel calls */
                priv(rp)->s_sig_mgr = SRV_SM;     /* signal manager */
                rp->p_ترجيح = SRV_Q;	          /* priority queue */
                rp->p_وقتي_حصو_سائيز_ms = SRV_QT;   /* quantum size */
            }

            /* Fill in target mask. */
            memset(&map, 0, حجم(map));

            إذا (ipc_منزل_m == ALL_M) {
                لكل(j = 0; j < NR_SYS_PROCS; j++)
                    سيٽ_sys_ٻٽو_انگ(map, j);
            }

            fill_sendto_نقاب(rp, &map);

            /* Fill in kernel call mask. */
            لكل(j = 0; j < SYS_CALL_نقاب_سائيز; j++) {
                priv(rp)->s_k_call_نقاب[j] = (kcalls == NO_C ? 0 : (~0));
            }
	}
	وإلا {
	    /* Don't let the process run for now. */
            RTS_سيٽ(rp, RTS_NO_PRIV | RTS_NO_وقتي_حصو);
	}

	/* Arch-specific state initialization. */
	arch_boot_proc(ip, rp);

	/* scheduling functions depend on proc_ptr pointing somewhere. */
	إذا(!get_cpulocal_var(proc_ptr))
		get_cpulocal_var(proc_ptr) = rp;

	/* Process isn't scheduled until VM has set up a pagetable for it. */
	إذا(rp->p_nr != VM_PROC_NR && rp->p_nr >= 0) {
		rp->p_rts_flags |= RTS_VMINHIBIT;
		rp->p_rts_flags |= RTS_BOOTINHIBIT;
	}

	rp->p_rts_flags |= RTS_PROC_STOP;
	rp->p_rts_flags &= ~RTS_جڳهه_FREE;
	DEBUGEXTRA(("done\n"));
  }

  /* update boot procs info for VM */
  يادگيري_نقل(kinfo.boot_procs, image, حجم(kinfo.boot_procs));

#وصف_ڪريو IPCNAME(n) { \
	أكد((n) >= 0 && (n) <= IPCNO_HIGHEST); \
	أكد(!ipc_call_نالا[n]);	\
	ipc_call_نالا[n] = #n; \
}

  arch_post_init();

  IPCNAME(SEND);
  IPCNAME(RECEIVE);
  IPCNAME(SENDREC);
  IPCNAME(NOTIFY);
  IPCNAME(SENDNB);
  IPCNAME(SENDA);

  /* System and processes initialization */
  يادگيري_init();
  DEBUGEXTRA(("system_init()... "));
  system_init();
  DEBUGEXTRA(("done\n"));

  /* The bootstrap phase is over, so we can add the physical
   * memory used for it to the free list.
   */
  جوڙيو_memmap(&kinfo, kinfo.bootstrap_شروعات, kinfo.bootstrap_len);

#جيڪڏهن_وصف_ٿيل CONFIG_SMP
  إذا (config_no_apic) {
	  DEBUGBASIC(("APIC disabled, disables SMP, using legacy PIC\n"));
	  smp_single_cpu_fallback();
  } وإلا إذا (config_no_smp) {
	  DEBUGBASIC(("SMP disabled, using legacy PIC\n"));
	  smp_single_cpu_fallback();
  } وإلا {
	  smp_init();
	  /*
	   * if smp_init() returns it means that it failed and we try to finish
	   * single CPU booting
	   */
	  bsp_پڄاڻي_booting();
  }
#وإلا
  /* 
   * if configured for a single CPU, we are already on the kernel stack which we
   * are going to use everytime we execute kernel code. We finish booting and we
   * never return here
   */
  bsp_پڄاڻي_booting();
#جيڪڏهن_پورو

  NOT_REACHABLE;
}

/*===========================================================================*
 *				announce				     *
 *===========================================================================*/
ساكن فراغ announce(فراغ)
{
  /* Display the MINIX startup banner. */
  لکو("\nMINIX %s. "
#جيڪڏهن_وصف_ٿيل PAE
"(PAE) "
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل _VCS_REVISION
	"(" _VCS_REVISION ")\n"
#جيڪڏهن_پورو
      "Copyright 2016, Vrije Universiteit, Amsterdam, The Netherlands\n",
      OS_RELEASE);
  لکو("MINIX is open source software, see http://www.minix3.org\n");
}

/*===========================================================================*
 *				prepare_shutdown			     *
 *===========================================================================*/
فراغ prepare_shutdown(ثابت صحيح how)
{
/* This function prepares to shutdown MINIX. */
  ساكن minix_timer_t shutdown_timer;

  /* Continue after 1 second, to give processes a chance to get scheduled to 
   * do shutdown work.  Set a watchog timer to call shutdown(). The timer 
   * argument passes the shutdown status. 
   */
  لکو("MINIX will now be shut down ...\n");
  سيٽ_kernel_timer(&shutdown_timer, get_monotonic() + system_hz,
      minix_shutdown, how);
}

/*===========================================================================*
 *				shutdown 				     *
 *===========================================================================*/
فراغ minix_shutdown(صحيح how)
{
/* This function is called from prepare_shutdown or stop_sequence to bring 
 * down MINIX.
 */

#جيڪڏهن_وصف_ٿيل CONFIG_SMP
  /* 
   * FIXME
   *
   * we will need to stop timers on all cpus if SMP is enabled and put them in
   * such a state that we can perform the whole boot process once restarted from
   * monitor again
   */
  إذا (ncpus > 1)
	  smp_shutdown_aps();
#جيڪڏهن_پورو
  hw_intr_disable_all();
  stop_local_timer();

  /* Show shutdown message */
  سڌو_cls();
  إذا((how & RB_POWERDOWN) == RB_POWERDOWN)
	سڌو_print("MINIX has halted and will now power off.\n");
  وإلا إذا(how & RB_HALT)
	سڌو_print("MINIX has halted. "
		     "It is safe to turn off your computer.\n");
  وإلا
	سڌو_print("MINIX will now reset.\n");
  arch_shutdown(how);
}

/*===========================================================================*
 *				cstart					     *
 *===========================================================================*/
فراغ cstart(فراغ)
{
/* Perform system initializations prior to calling main(). Most settings are
 * determined with help of the environment strings passed by MINIX' loader.
 */
  سجل حرف *قدر;				/* value in key=value pair */

  /* low-level initialization */
  prot_init();

  /* determine verbosity */
  إذا ((قدر = env_get(VERBOSEBOOTVARNAME)))
	  verboseboot = atoi(قدر);

  /* Initialize clock variables. */
  init_clock();

  /* Get memory parameters. */
  قدر = env_get("ac_layout");
  إذا(قدر && atoi(قدر)) {
        kinfo.user_sp = (vir_بائيٽون) USR_STACKTOP_COMPACT;
        kinfo.user_پڇاڙي = (vir_بائيٽون) USR_DATATOP_COMPACT;
  }

  DEBUGEXTRA(("cstart\n"));

  /* Record miscellaneous information for user-space servers. */
  kinfo.nr_procs = NR_PROCS;
  kinfo.nr_tasks = NR_TASKS;
  strlcpy(kinfo.release, OS_RELEASE, حجم(kinfo.release));
  strlcpy(kinfo.نسخو, OS_نسخو, حجم(kinfo.نسخو));

  /* Initialize various user-mapped structures. */
  memset(&arm_frclock, 0, حجم(arm_frclock));

  memset(&kuserinfo, 0, حجم(kuserinfo));
  kuserinfo.kui_سائيز = حجم(kuserinfo);
  kuserinfo.kui_user_sp = kinfo.user_sp;

#جيڪڏهن_وصف_ٿيل USE_APIC
  قدر = env_get("no_apic");
  إذا(قدر)
	config_no_apic = atoi(قدر);
  وإلا
	config_no_apic = 1;
  قدر = env_get("apic_timer_x");
  إذا(قدر)
	config_apic_timer_x = atoi(قدر);
  وإلا
	config_apic_timer_x = 1;
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل USE_WATCHDOG
  قدر = env_get("watchdog");
  إذا (قدر)
	  watchdog_enabled = atoi(قدر);
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل CONFIG_SMP
  إذا (config_no_apic)
	  config_no_smp = 1;
  قدر = env_get("no_smp");
  إذا(قدر)
	config_no_smp = atoi(قدر);
  وإلا
	config_no_smp = 0;
#جيڪڏهن_پورو
  DEBUGEXTRA(("intr_init(0)\n"));

  intr_init(0);

  arch_init();
}

/*===========================================================================*
 *				get_value				     *
 *===========================================================================*/

حرف *get_قدر(
  ثابت حرف *params,			/* boot monitor parameters */
  ثابت حرف *name			/* key to look up */
)
{
/* Get environment value - kernel version of getenv to avoid setting up the
 * usual environment array.
 */
  سجل ثابت حرف *namep;
  سجل حرف *envp;

  لكل (envp = (حرف *) params; *envp != 0;) {
	لكل (namep = name; *namep != 0 && *namep == *envp; namep++, envp++)
		;
	إذا (*namep == '\0' && *envp == '=') أرجع(envp + 1);
	طالما (*envp++ != 0)
		;
  }
  أرجع(عدم);
}

/*===========================================================================*
 *				env_get				     	*
 *===========================================================================*/
حرف *env_get(ثابت حرف *name)
{
	أرجع get_قدر(kinfo.param_buf, name);
}

فراغ cpu_print_freq(بنانشان cpu)
{
        u64_t freq;

        freq = cpu_get_freq(cpu);
        DEBUGBASIC(("CPU %d freq %lu MHz\n", cpu, (بنانشان ڊگهو)(freq / 1000000)));
}

صحيح is_fpu(فراغ)
{
        أرجع get_cpulocal_var(fpu_presence);
}

