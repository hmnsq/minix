/* The kernel call implemented in this file:
 *   m_type:	SYS_GETINFO
 *
 * The parameters for this kernel call are:
 *   m_lsys_krn_sys_getinfo.request	(what info to get)
 *   m_lsys_krn_sys_getinfo.val_ptr 	(where to put it)
 *   m_lsys_krn_sys_getinfo.val_len 	(maximum length expected, optional)
 *   m_lsys_krn_sys_getinfo.val_ptr2	(second, optional pointer)
 *   m_lsys_krn_sys_getinfo.val_len2_e	(second length or process nr)
 *
 * Upon return of the GETWHOAMI request the following parameters are used:
 *   m_krn_lsys_sys_getwhoami.endpt	(the caller endpoint)
 *   m_krn_lsys_sys_getwhoami.privflags	(the caller priviledes)
 *   m_krn_lsys_sys_getwhoami.initflags (the caller initflags)
 *   m_krn_lsys_sys_getwhoami.name	(the caller process name)
 *
 */

#অন্তর্ভাব <string.h>

#অন্তর্ভাব "kernel/system.h"


#যদি USE_GETINFO

#অন্তর্ভাব <minix/u64.h>
#অন্তর্ভাব <sys/resource.h>

/*===========================================================================*
 *			        update_idle_time			     *
 *===========================================================================*/
স্থির শূন্য update_idle_time(শূন্য)
{
	পূর্ণাঙ্ক i;
	সংরচনা proc * idl = proc_addr(IDLE);

	idl->p_cycles = make64(0, 0);

	প্রতি (i = 0; i < CONFIG_MAX_CPUS ; i++) {
		idl->p_cycles += get_cpu_var(i, idle_proc).p_cycles;
	}
}

/*===========================================================================*
 *			        do_getinfo				     *
 *===========================================================================*/
পূর্ণাঙ্ক do_getinfo(সংরচনা proc * caller, message * m_ptr)
{
/* Request system information to be copied to caller's address space. This
 * call simply copies entire data structures to the caller.
 */
  size_t length;
  vir_bytes src_vir; 
  পূর্ণাঙ্ক nr_e, nr, r;
  পূর্ণাঙ্ক wipe_rnd_bin = -1;
  সংরচনা proc *p;
  সংরচনা rusage r_usage;

  /* Set source address and length based on request type. */
  বিকল্প (m_ptr->m_lsys_krn_sys_getinfo.request) {
    পক্ষ GET_MACHINE: {
        length = পরিমাণ(সংরচনা machine);
        src_vir = (vir_bytes) &machine;
        ভঙ্গ;
    }
    পক্ষ GET_KINFO: {
        length = পরিমাণ(সংরচনা kinfo);
        src_vir = (vir_bytes) &kinfo;
        ভঙ্গ;
    }
    পক্ষ GET_LOADINFO: {
        length = পরিমাণ(সংরচনা loadinfo);
        src_vir = (vir_bytes) &kloadinfo;
        ভঙ্গ;
    }
    পক্ষ GET_CPUINFO: {
        length = পরিমাণ(cpu_info);
        src_vir = (vir_bytes) &cpu_info;
        ভঙ্গ;
    }
    পক্ষ GET_HZ: {
        length = পরিমাণ(system_hz);
        src_vir = (vir_bytes) &system_hz;
        ভঙ্গ;
    }
    পক্ষ GET_IMAGE: {
        length = পরিমাণ(সংরচনা boot_image) * NR_BOOT_PROCS;
        src_vir = (vir_bytes) image;
        ভঙ্গ;
    }
    পক্ষ GET_IRQHOOKS: {
        length = পরিমাণ(সংরচনা irq_hook) * NR_IRQ_HOOKS;
        src_vir = (vir_bytes) irq_hooks;
        ভঙ্গ;
    }
    পক্ষ GET_PROCTAB: {
	update_idle_time();
        length = পরিমাণ(সংরচনা proc) * (NR_PROCS + NR_TASKS);
        src_vir = (vir_bytes) proc;
        ভঙ্গ;
    }
    পক্ষ GET_PRIVTAB: {
        length = পরিমাণ(সংরচনা priv) * (NR_SYS_PROCS);
        src_vir = (vir_bytes) priv;
        ভঙ্গ;
    }
    পক্ষ GET_PROC: {
        nr_e = (m_ptr->m_lsys_krn_sys_getinfo.val_len2_e == SELF) ?
		caller->p_endpoint : m_ptr->m_lsys_krn_sys_getinfo.val_len2_e;
	যদি(!isokendpt(nr_e, &nr)) প্রত্যাগম EINVAL; /* validate request */
        length = পরিমাণ(সংরচনা proc);
        src_vir = (vir_bytes) proc_addr(nr);
        ভঙ্গ;
    }
    পক্ষ GET_PRIV: {
        nr_e = (m_ptr->m_lsys_krn_sys_getinfo.val_len2_e == SELF) ?
            caller->p_endpoint : m_ptr->m_lsys_krn_sys_getinfo.val_len2_e;
        যদি(!isokendpt(nr_e, &nr)) প্রত্যাগম EINVAL; /* validate request */
        length = পরিমাণ(সংরচনা priv);
        src_vir = (vir_bytes) priv_addr(nr_to_id(nr));
        ভঙ্গ;
    }
    পক্ষ GET_REGS: {
        nr_e = (m_ptr->m_lsys_krn_sys_getinfo.val_len2_e == SELF) ?
            caller->p_endpoint : m_ptr->m_lsys_krn_sys_getinfo.val_len2_e;
        যদি(!isokendpt(nr_e, &nr)) প্রত্যাগম EINVAL; /* validate request */
        p = proc_addr(nr);
        length = পরিমাণ(p->p_reg);
        src_vir = (vir_bytes) &p->p_reg;
        ভঙ্গ;
    }
    পক্ষ GET_WHOAMI: {
	পূর্ণাঙ্ক len;
	m_ptr->m_krn_lsys_sys_getwhoami.endpt = caller->p_endpoint;
	len = MIN(পরিমাণ(m_ptr->m_krn_lsys_sys_getwhoami.name),
		পরিমাণ(caller->p_name))-1;
	strncpy(m_ptr->m_krn_lsys_sys_getwhoami.name, caller->p_name, len);
	m_ptr->m_krn_lsys_sys_getwhoami.name[len] = '\0';
	m_ptr->m_krn_lsys_sys_getwhoami.privflags = priv(caller)->s_flags;
        m_ptr->m_krn_lsys_sys_getwhoami.initflags = priv(caller)->s_init_flags;
	প্রত্যাগম OK;
    }
    পক্ষ GET_MONPARAMS: {
        src_vir = (vir_bytes) kinfo.param_buf;
	length = পরিমাণ(kinfo.param_buf);
        ভঙ্গ;
    }
    পক্ষ GET_RANDOMNESS: {		
        স্থির সংরচনা k_randomness copy;	/* copy to keep counters */
	পূর্ণাঙ্ক i;

        copy = krandom;
        প্রতি (i= 0; i<RANDOM_SOURCES; i++) {
  		krandom.bin[i].r_size = 0;	/* invalidate random data */
  		krandom.bin[i].r_next = 0;
	}
    	length = পরিমাণ(copy);
    	src_vir = (vir_bytes) &copy;
    	ভঙ্গ;
    }
    পক্ষ GET_RANDOMNESS_BIN: {		
	পূর্ণাঙ্ক bin = m_ptr->m_lsys_krn_sys_getinfo.val_len2_e;

	যদি(bin < 0 || bin >= RANDOM_SOURCES) {
		printf("SYSTEM: GET_RANDOMNESS_BIN: %d out of range\n", bin);
		প্রত্যাগম EINVAL;
	}

	যদি(krandom.bin[bin].r_size < RANDOM_ELEMENTS)
		প্রত্যাগম ENOENT;

    	length = পরিমাণ(krandom.bin[bin]);
    	src_vir = (vir_bytes) &krandom.bin[bin];

	wipe_rnd_bin = bin;

    	ভঙ্গ;
    }
    পক্ষ GET_IRQACTIDS: {
        length = পরিমাণ(irq_actids);
        src_vir = (vir_bytes) irq_actids;
        ভঙ্গ;
    }
    পক্ষ GET_IDLETSC: {
	সংরচনা proc * idl;
	update_idle_time();
	idl = proc_addr(IDLE);
        length = পরিমাণ(idl->p_cycles);
        src_vir = (vir_bytes) &idl->p_cycles;
        ভঙ্গ;
    }
    পক্ষ GET_CPUTICKS: {
	uint64_t ticks[MINIX_CPUSTATES];
	unsigned পূর্ণাঙ্ক cpu;
	cpu = (unsigned পূর্ণাঙ্ক)m_ptr->m_lsys_krn_sys_getinfo.val_len2_e;
	যদি (cpu >= CONFIG_MAX_CPUS)
		প্রত্যাগম EINVAL;
	get_cpu_ticks(cpu, ticks);
	length = পরিমাণ(ticks);
	src_vir = (vir_bytes)ticks;
	ভঙ্গ;
    }
    স্বভাব:
	printf("do_getinfo: invalid request %d\n",
		m_ptr->m_lsys_krn_sys_getinfo.request);
        প্রত্যাগম(EINVAL);
  }

  /* Try to make the actual copy for the requested data. */
  যদি (m_ptr->m_lsys_krn_sys_getinfo.val_len > 0 &&
	length > m_ptr->m_lsys_krn_sys_getinfo.val_len)
	প্রত্যাগম (E2BIG);

  r = data_copy_vmcheck(caller, KERNEL, src_vir, caller->p_endpoint,
	m_ptr->m_lsys_krn_sys_getinfo.val_ptr, length);

  যদি(r != OK) প্রত্যাগম r;

	যদি(wipe_rnd_bin >= 0 && wipe_rnd_bin < RANDOM_SOURCES) {
		krandom.bin[wipe_rnd_bin].r_size = 0;
		krandom.bin[wipe_rnd_bin].r_next = 0;
	}

  প্রত্যাগম(OK);
}

#endif /* USE_GETINFO */

