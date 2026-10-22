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
#شامل <string.h>
#شامل <stdlib.h>
#شامل <assert.h>
#شامل <minix/endpoint.h>
#شامل <machine/vmparam.h>
#شامل <minix/u64.h>
#شامل <minix/board.h>
#شامل <sys/reboot.h>
#شامل "clock.h"
#شامل "direct_utils.h"
#شامل "hw_intr.h"
#شامل "arch_proto.h"

#اگر_تعریف_شدہ CONFIG_SMP
#شامل "smp.h"
#اگر_ختم
#اگر_تعریف_شدہ USE_WATCHDOG
#شامل "watchdog.h"
#اگر_ختم
#شامل "spinlock.h"

/* dummy for linking */
حرف *** _penviron;

/* Prototype declarations for PRIVATE functions. */
ساکن خالی announce(خالی);

خالی bsp_اختتام_booting(خالی)
{
  صحیح i;
#اگر SPROFILE
  sprofiling = 0;      /* we're not profiling until instructed to */
#اگر_ختم /* SPROFILE */

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
  برائے (i=0; i < NR_BOOT_PROCS - NR_TASKS; i++) {
	RTS_UNSET(proc_addr(i), RTS_PROC_STOP);
  }
  /*
   * Enable timer interrupts and clock task on the boot CPU.  First reset the
   * CPU accounting values, as the timer initialization (indirectly) uses them.
   */
  ادوار_accounting_init();

  اگر (boot_cpu_init_timer(system_hz)) {
	  panic("FATAL : failed to initialize timer interrupts, "
			  "cannot continue without any clock source!");
  }

  fpu_init();

/* Warnings for sanity checks that take time. These warnings are printed
 * so it's a clear warning no full release should be done with them
 * enabled.
 */
#اگر DEBUG_SCHED_CHECK
  FIXME("DEBUG_SCHED_CHECK enabled");
#اگر_ختم
#اگر DEBUG_VMASSERT
  FIXME("DEBUG_VMASSERT enabled");
#اگر_ختم
#اگر DEBUG_PROC_CHECK
  FIXME("PROC check enabled");
#اگر_ختم

#اگر_تعریف_شدہ CONFIG_SMP
  cpu_سیٹ_flag(bsp_cpu_id, CPU_IS_READY);
  machine.processors_گنتی = ncpus;
  machine.bsp_id = bsp_cpu_id;
#ورنہ
  machine.processors_گنتی = 1;
  machine.bsp_id = 0;
#اگر_ختم
  

  /* Kernel may no longer use bits of memory as VM will be running soon */
  kernel_may_alloc = 0;

  switch_مقصد_user();
  NOT_REACHABLE;
}


/*===========================================================================*
 *			kmain 	                             		*
 *===========================================================================*/
خالی kmain(kinfo_t *local_cbi)
{
/* Start the ball rolling. */
  ساخت boot_image *ip;	/* boot image pointer */
  اندراج ساخت proc *rp;	/* process pointer */
  اندراج صحیح i, j;
  ساکن صحیح bss_test;

  /* bss sanity check */
  أكد(bss_test == 0);
  bss_test = 1;

  /* save a global copy of the boot parameters */
  حافظہ_نقل(&kinfo, local_cbi, حجم(kinfo));
  حافظہ_نقل(&kmess, kinfo.kmess, حجم(kmess));

   /* We have done this exercise in pre_init so we expect this code
      to simply work! */
   machine.board_id = get_board_id_by_name(env_get(BOARDVARNAME));
#اگر_تعریف_شدہ __arm__
  /* We want to initialize serial before we do any output */
  arch_ser_init();
#اگر_ختم
  /* We can talk now */
  DEBUGBASIC(("MINIX booting\n"));

  /* Kernel may use bits of main memory before VM is started */
  kernel_may_alloc = 1;

  أكد(حجم(kinfo.boot_procs) == حجم(image));
  حافظہ_نقل(kinfo.boot_procs, image, حجم(kinfo.boot_procs));

  cstart();

  BKL_تالا();
 
   DEBUGEXTRA(("main()\n"));

  /* Clear the process table. Anounce each slot as empty and set up mappings
   * for proc_addr() and proc_nr() macros. Do the same for the table with
   * privilege structures for the system processes and the ipc filter pool.
   */
  proc_init();
  IPCF_POOL_INIT();

   اگر(NR_BOOT_MODULES != kinfo.mbi.mi_mods_گنتی)
   	panic("expecting %d boot processes/modules, found %d",
		NR_BOOT_MODULES, kinfo.mbi.mi_mods_گنتی);

  /* Set up proc table entries for processes in boot image. */
  برائے (i=0; i < NR_BOOT_PROCS; ++i) {
	صحیح schedulable_proc;
	proc_nr_t proc_nr;
	صحیح ipc_مقصد_m, kcalls;
	sys_map_t map;

	ip = &image[i];				/* process' attributes */
	DEBUGEXTRA(("initializing %s... ", ip->proc_name));
	rp = proc_addr(ip->proc_nr);		/* get process pointer */
	ip->endpoint = rp->p_endpoint;		/* ipc endpoint */
	rp->p_cpu_وقت_بایاں = 0;
	اگر(i < NR_TASKS)			/* name (tasks only) */
		strlcpy(rp->p_name, ip->proc_name, حجم(rp->p_name));

	اگر(i >= NR_TASKS) {
		/* Remember this so it can be passed to VM */
		multiboot_module_t *mb_mod = &kinfo.module_list[i - NR_TASKS];
		ip->آغاز_addr = mb_mod->mod_آغاز;
		ip->len = mb_mod->mod_انتہا - mb_mod->mod_آغاز;
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
	اگر(schedulable_proc) {
	    /* Assign privilege structure. Force a static privilege id. */
            (خالی) get_priv(rp, static_priv_id(proc_nr));

            /* Privileges for kernel tasks. */
	    اگر(proc_nr == VM_PROC_NR) {
                priv(rp)->s_flags = VM_F;
                priv(rp)->s_trap_نقاب = SRV_T;
		ipc_مقصد_m = SRV_M;
		kcalls = SRV_KC;
                priv(rp)->s_sig_mgr = SELF;
                rp->p_ترجیح = SRV_Q;
                rp->p_وقتی_حصہ_سائز_ms = SRV_QT;
	    }
	    ورنہ اگر(iskerneln(proc_nr)) {
                /* Privilege flags. */
                priv(rp)->s_flags = (proc_nr == IDLE ? IDL_F : TSK_F);
                /* Init flags. */
                priv(rp)->s_init_flags = TSK_I;
                /* Allowed traps. */
                priv(rp)->s_trap_نقاب = (proc_nr == CLOCK 
                    || proc_nr == SYSTEM  ? CSK_T : TSK_T);
                ipc_مقصد_m = TSK_M;                  /* allowed targets */
                kcalls = TSK_KC;                   /* allowed kernel calls */
            }
            /* Privileges for the root system process. */
            ورنہ {
	    	أكد(isrootsysn(proc_nr));
                priv(rp)->s_flags= RSYS_F;        /* privilege flags */
                priv(rp)->s_init_flags = SRV_I;   /* init flags */
                priv(rp)->s_trap_نقاب= SRV_T;     /* allowed traps */
                ipc_مقصد_m = SRV_M;                 /* allowed targets */
                kcalls = SRV_KC;                  /* allowed kernel calls */
                priv(rp)->s_sig_mgr = SRV_SM;     /* signal manager */
                rp->p_ترجیح = SRV_Q;	          /* priority queue */
                rp->p_وقتی_حصہ_سائز_ms = SRV_QT;   /* quantum size */
            }

            /* Fill in target mask. */
            memset(&map, 0, حجم(map));

            اگر (ipc_مقصد_m == ALL_M) {
                برائے(j = 0; j < NR_SYS_PROCS; j++)
                    سیٹ_sys_ثنائی_ہندسہ(map, j);
            }

            fill_sendto_نقاب(rp, &map);

            /* Fill in kernel call mask. */
            برائے(j = 0; j < SYS_CALL_نقاب_سائز; j++) {
                priv(rp)->s_k_call_نقاب[j] = (kcalls == NO_C ? 0 : (~0));
            }
	}
	ورنہ {
	    /* Don't let the process run for now. */
            RTS_سیٹ(rp, RTS_NO_PRIV | RTS_NO_وقتی_حصہ);
	}

	/* Arch-specific state initialization. */
	arch_boot_proc(ip, rp);

	/* scheduling functions depend on proc_ptr pointing somewhere. */
	اگر(!get_cpulocal_var(proc_ptr))
		get_cpulocal_var(proc_ptr) = rp;

	/* Process isn't scheduled until VM has set up a pagetable for it. */
	اگر(rp->p_nr != VM_PROC_NR && rp->p_nr >= 0) {
		rp->p_rts_flags |= RTS_VMINHIBIT;
		rp->p_rts_flags |= RTS_BOOTINHIBIT;
	}

	rp->p_rts_flags |= RTS_PROC_STOP;
	rp->p_rts_flags &= ~RTS_خانہ_FREE;
	DEBUGEXTRA(("done\n"));
  }

  /* update boot procs info for VM */
  حافظہ_نقل(kinfo.boot_procs, image, حجم(kinfo.boot_procs));

#تعریف_کرو IPCNAME(n) { \
	أكد((n) >= 0 && (n) <= IPCNO_HIGHEST); \
	أكد(!ipc_call_نام[n]);	\
	ipc_call_نام[n] = #n; \
}

  arch_post_init();

  IPCNAME(SEND);
  IPCNAME(RECEIVE);
  IPCNAME(SENDREC);
  IPCNAME(NOTIFY);
  IPCNAME(SENDNB);
  IPCNAME(SENDA);

  /* System and processes initialization */
  حافظہ_init();
  DEBUGEXTRA(("system_init()... "));
  system_init();
  DEBUGEXTRA(("done\n"));

  /* The bootstrap phase is over, so we can add the physical
   * memory used for it to the free list.
   */
  جوڑو_memmap(&kinfo, kinfo.bootstrap_آغاز, kinfo.bootstrap_len);

#اگر_تعریف_شدہ CONFIG_SMP
  اگر (config_no_apic) {
	  DEBUGBASIC(("APIC disabled, disables SMP, using legacy PIC\n"));
	  smp_single_cpu_fallback();
  } ورنہ اگر (config_no_smp) {
	  DEBUGBASIC(("SMP disabled, using legacy PIC\n"));
	  smp_single_cpu_fallback();
  } ورنہ {
	  smp_init();
	  /*
	   * if smp_init() returns it means that it failed and we try to finish
	   * single CPU booting
	   */
	  bsp_اختتام_booting();
  }
#ورنہ
  /* 
   * if configured for a single CPU, we are already on the kernel stack which we
   * are going to use everytime we execute kernel code. We finish booting and we
   * never return here
   */
  bsp_اختتام_booting();
#اگر_ختم

  NOT_REACHABLE;
}

/*===========================================================================*
 *				announce				     *
 *===========================================================================*/
ساکن خالی announce(خالی)
{
  /* Display the MINIX startup banner. */
  لکھو("\nMINIX %s. "
#اگر_تعریف_شدہ PAE
"(PAE) "
#اگر_ختم
#اگر_تعریف_شدہ _VCS_REVISION
	"(" _VCS_REVISION ")\n"
#اگر_ختم
      "Copyright 2016, Vrije Universiteit, Amsterdam, The Netherlands\n",
      OS_RELEASE);
  لکھو("MINIX is open source software, see http://www.minix3.org\n");
}

/*===========================================================================*
 *				prepare_shutdown			     *
 *===========================================================================*/
خالی prepare_shutdown(ثابت صحیح how)
{
/* This function prepares to shutdown MINIX. */
  ساکن minix_timer_t shutdown_timer;

  /* Continue after 1 second, to give processes a chance to get scheduled to 
   * do shutdown work.  Set a watchog timer to call shutdown(). The timer 
   * argument passes the shutdown status. 
   */
  لکھو("MINIX will now be shut down ...\n");
  سیٹ_kernel_timer(&shutdown_timer, get_monotonic() + system_hz,
      minix_shutdown, how);
}

/*===========================================================================*
 *				shutdown 				     *
 *===========================================================================*/
خالی minix_shutdown(صحیح how)
{
/* This function is called from prepare_shutdown or stop_sequence to bring 
 * down MINIX.
 */

#اگر_تعریف_شدہ CONFIG_SMP
  /* 
   * FIXME
   *
   * we will need to stop timers on all cpus if SMP is enabled and put them in
   * such a state that we can perform the whole boot process once restarted from
   * monitor again
   */
  اگر (ncpus > 1)
	  smp_shutdown_aps();
#اگر_ختم
  hw_intr_disable_all();
  stop_local_timer();

  /* Show shutdown message */
  براہ_راست_cls();
  اگر((how & RB_POWERDOWN) == RB_POWERDOWN)
	براہ_راست_print("MINIX has halted and will now power off.\n");
  ورنہ اگر(how & RB_HALT)
	براہ_راست_print("MINIX has halted. "
		     "It is safe to turn off your computer.\n");
  ورنہ
	براہ_راست_print("MINIX will now reset.\n");
  arch_shutdown(how);
}

/*===========================================================================*
 *				cstart					     *
 *===========================================================================*/
خالی cstart(خالی)
{
/* Perform system initializations prior to calling main(). Most settings are
 * determined with help of the environment strings passed by MINIX' loader.
 */
  اندراج حرف *قدر;				/* value in key=value pair */

  /* low-level initialization */
  prot_init();

  /* determine verbosity */
  اگر ((قدر = env_get(VERBOSEBOOTVARNAME)))
	  verboseboot = atoi(قدر);

  /* Initialize clock variables. */
  init_clock();

  /* Get memory parameters. */
  قدر = env_get("ac_layout");
  اگر(قدر && atoi(قدر)) {
        kinfo.user_sp = (vir_بائٹیں) USR_STACKTOP_COMPACT;
        kinfo.user_انتہا = (vir_بائٹیں) USR_DATATOP_COMPACT;
  }

  DEBUGEXTRA(("cstart\n"));

  /* Record miscellaneous information for user-space servers. */
  kinfo.nr_procs = NR_PROCS;
  kinfo.nr_tasks = NR_TASKS;
  strlcpy(kinfo.release, OS_RELEASE, حجم(kinfo.release));
  strlcpy(kinfo.نسخہ, OS_نسخہ, حجم(kinfo.نسخہ));

  /* Initialize various user-mapped structures. */
  memset(&arm_frclock, 0, حجم(arm_frclock));

  memset(&kuserinfo, 0, حجم(kuserinfo));
  kuserinfo.kui_سائز = حجم(kuserinfo);
  kuserinfo.kui_user_sp = kinfo.user_sp;

#اگر_تعریف_شدہ USE_APIC
  قدر = env_get("no_apic");
  اگر(قدر)
	config_no_apic = atoi(قدر);
  ورنہ
	config_no_apic = 1;
  قدر = env_get("apic_timer_x");
  اگر(قدر)
	config_apic_timer_x = atoi(قدر);
  ورنہ
	config_apic_timer_x = 1;
#اگر_ختم

#اگر_تعریف_شدہ USE_WATCHDOG
  قدر = env_get("watchdog");
  اگر (قدر)
	  watchdog_enabled = atoi(قدر);
#اگر_ختم

#اگر_تعریف_شدہ CONFIG_SMP
  اگر (config_no_apic)
	  config_no_smp = 1;
  قدر = env_get("no_smp");
  اگر(قدر)
	config_no_smp = atoi(قدر);
  ورنہ
	config_no_smp = 0;
#اگر_ختم
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
  اندراج ثابت حرف *namep;
  اندراج حرف *envp;

  برائے (envp = (حرف *) params; *envp != 0;) {
	برائے (namep = name; *namep != 0 && *namep == *envp; namep++, envp++)
		;
	اگر (*namep == '\0' && *envp == '=') واپس(envp + 1);
	جب (*envp++ != 0)
		;
  }
  واپس(تہی);
}

/*===========================================================================*
 *				env_get				     	*
 *===========================================================================*/
حرف *env_get(ثابت حرف *name)
{
	واپس get_قدر(kinfo.param_buf, name);
}

خالی cpu_print_freq(غیرعلامتی cpu)
{
        u64_t freq;

        freq = cpu_get_freq(cpu);
        DEBUGBASIC(("CPU %d freq %lu MHz\n", cpu, (غیرعلامتی طویل)(freq / 1000000)));
}

صحیح is_fpu(خالی)
{
        واپس get_cpulocal_var(fpu_presence);
}

