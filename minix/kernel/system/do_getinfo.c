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

#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/system.h"


#ਜੇ USE_GETINFO

#ਅਨ੍ਤਰ੍ਭਾਵ <minix/u64.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/resource.h>

/*===========================================================================*
 *			        update_idle_time			     *
 *===========================================================================*/
ਸਥਿਰ ਖਾਲੀ update_idle_ਸਮਾਂ(ਖਾਲੀ)
{
	ਪੂਰਨ i;
	ਬਣਤਰ proc * idl = proc_addr(IDLE);

	idl->p_ਚੱਕਰਾਂ = make64(0, 0);

	ਲਈ (i = 0; i < CONFIG_MAX_CPUS ; i++) {
		idl->p_ਚੱਕਰਾਂ += get_cpu_var(i, idle_proc).p_ਚੱਕਰਾਂ;
	}
}

/*===========================================================================*
 *			        do_getinfo				     *
 *===========================================================================*/
ਪੂਰਨ do_getinfo(ਬਣਤਰ proc * caller, ਸੁਨੇਹਾ * m_ptr)
{
/* Request system information to be copied to caller's address space. This
 * call simply copies entire data structures to the caller.
 */
  size_t ਲੰਮਾਈ;
  vir_ਬਾਈਟਾਂ src_vir; 
  ਪੂਰਨ nr_e, nr, r;
  ਪੂਰਨ wipe_rnd_bin = -1;
  ਬਣਤਰ proc *p;
  ਬਣਤਰ rusage r_usage;

  /* Set source address and length based on request type. */
  ਚੋਣ (m_ptr->m_lsys_krn_sys_getinfo.request) {
    ਹਾਲਤ GET_MACHINE: {
        ਲੰਮਾਈ = ਆਕਾਰ(ਬਣਤਰ machine);
        src_vir = (vir_ਬਾਈਟਾਂ) &machine;
        ਰੋਕੋ;
    }
    ਹਾਲਤ GET_KINFO: {
        ਲੰਮਾਈ = ਆਕਾਰ(ਬਣਤਰ kinfo);
        src_vir = (vir_ਬਾਈਟਾਂ) &kinfo;
        ਰੋਕੋ;
    }
    ਹਾਲਤ GET_LOADINFO: {
        ਲੰਮਾਈ = ਆਕਾਰ(ਬਣਤਰ loadinfo);
        src_vir = (vir_ਬਾਈਟਾਂ) &kloadinfo;
        ਰੋਕੋ;
    }
    ਹਾਲਤ GET_CPUINFO: {
        ਲੰਮਾਈ = ਆਕਾਰ(cpu_info);
        src_vir = (vir_ਬਾਈਟਾਂ) &cpu_info;
        ਰੋਕੋ;
    }
    ਹਾਲਤ GET_HZ: {
        ਲੰਮਾਈ = ਆਕਾਰ(system_hz);
        src_vir = (vir_ਬਾਈਟਾਂ) &system_hz;
        ਰੋਕੋ;
    }
    ਹਾਲਤ GET_IMAGE: {
        ਲੰਮਾਈ = ਆਕਾਰ(ਬਣਤਰ boot_image) * NR_BOOT_PROCS;
        src_vir = (vir_ਬਾਈਟਾਂ) image;
        ਰੋਕੋ;
    }
    ਹਾਲਤ GET_IRQHOOKS: {
        ਲੰਮਾਈ = ਆਕਾਰ(ਬਣਤਰ irq_hook) * NR_IRQ_HOOKS;
        src_vir = (vir_ਬਾਈਟਾਂ) irq_hooks;
        ਰੋਕੋ;
    }
    ਹਾਲਤ GET_PROCTAB: {
	update_idle_ਸਮਾਂ();
        ਲੰਮਾਈ = ਆਕਾਰ(ਬਣਤਰ proc) * (NR_PROCS + NR_TASKS);
        src_vir = (vir_ਬਾਈਟਾਂ) proc;
        ਰੋਕੋ;
    }
    ਹਾਲਤ GET_PRIVTAB: {
        ਲੰਮਾਈ = ਆਕਾਰ(ਬਣਤਰ priv) * (NR_SYS_PROCS);
        src_vir = (vir_ਬਾਈਟਾਂ) priv;
        ਰੋਕੋ;
    }
    ਹਾਲਤ GET_PROC: {
        nr_e = (m_ptr->m_lsys_krn_sys_getinfo.val_len2_e == SELF) ?
		caller->p_endpoint : m_ptr->m_lsys_krn_sys_getinfo.val_len2_e;
	ਜੇ(!isokendpt(nr_e, &nr)) ਵਾਪਸ EINVAL; /* validate request */
        ਲੰਮਾਈ = ਆਕਾਰ(ਬਣਤਰ proc);
        src_vir = (vir_ਬਾਈਟਾਂ) proc_addr(nr);
        ਰੋਕੋ;
    }
    ਹਾਲਤ GET_PRIV: {
        nr_e = (m_ptr->m_lsys_krn_sys_getinfo.val_len2_e == SELF) ?
            caller->p_endpoint : m_ptr->m_lsys_krn_sys_getinfo.val_len2_e;
        ਜੇ(!isokendpt(nr_e, &nr)) ਵਾਪਸ EINVAL; /* validate request */
        ਲੰਮਾਈ = ਆਕਾਰ(ਬਣਤਰ priv);
        src_vir = (vir_ਬਾਈਟਾਂ) priv_addr(nr_ਮੰਜ਼ਿਲ_id(nr));
        ਰੋਕੋ;
    }
    ਹਾਲਤ GET_REGS: {
        nr_e = (m_ptr->m_lsys_krn_sys_getinfo.val_len2_e == SELF) ?
            caller->p_endpoint : m_ptr->m_lsys_krn_sys_getinfo.val_len2_e;
        ਜੇ(!isokendpt(nr_e, &nr)) ਵਾਪਸ EINVAL; /* validate request */
        p = proc_addr(nr);
        ਲੰਮਾਈ = ਆਕਾਰ(p->p_reg);
        src_vir = (vir_ਬਾਈਟਾਂ) &p->p_reg;
        ਰੋਕੋ;
    }
    ਹਾਲਤ GET_WHOAMI: {
	ਪੂਰਨ len;
	m_ptr->m_krn_lsys_sys_getwhoami.endpt = caller->p_endpoint;
	len = MIN(ਆਕਾਰ(m_ptr->m_krn_lsys_sys_getwhoami.name),
		ਆਕਾਰ(caller->p_name))-1;
	strncpy(m_ptr->m_krn_lsys_sys_getwhoami.name, caller->p_name, len);
	m_ptr->m_krn_lsys_sys_getwhoami.name[len] = '\0';
	m_ptr->m_krn_lsys_sys_getwhoami.privflags = priv(caller)->s_flags;
        m_ptr->m_krn_lsys_sys_getwhoami.initflags = priv(caller)->s_init_flags;
	ਵਾਪਸ OK;
    }
    ਹਾਲਤ GET_MONPARAMS: {
        src_vir = (vir_ਬਾਈਟਾਂ) kinfo.param_buf;
	ਲੰਮਾਈ = ਆਕਾਰ(kinfo.param_buf);
        ਰੋਕੋ;
    }
    ਹਾਲਤ GET_RANDOMNESS: {		
        ਸਥਿਰ ਬਣਤਰ k_randomness copy;	/* copy to keep counters */
	ਪੂਰਨ i;

        copy = krandom;
        ਲਈ (i= 0; i<RANDOM_SOURCES; i++) {
  		krandom.bin[i].r_ਮਾਪ = 0;	/* invalidate random data */
  		krandom.bin[i].r_ਅਗਲਾ = 0;
	}
    	ਲੰਮਾਈ = ਆਕਾਰ(copy);
    	src_vir = (vir_ਬਾਈਟਾਂ) &copy;
    	ਰੋਕੋ;
    }
    ਹਾਲਤ GET_RANDOMNESS_BIN: {		
	ਪੂਰਨ bin = m_ptr->m_lsys_krn_sys_getinfo.val_len2_e;

	ਜੇ(bin < 0 || bin >= RANDOM_SOURCES) {
		ਲਿਖੋ("SYSTEM: GET_RANDOMNESS_BIN: %d out of range\n", bin);
		ਵਾਪਸ EINVAL;
	}

	ਜੇ(krandom.bin[bin].r_ਮਾਪ < RANDOM_ELEMENTS)
		ਵਾਪਸ ENOENT;

    	ਲੰਮਾਈ = ਆਕਾਰ(krandom.bin[bin]);
    	src_vir = (vir_ਬਾਈਟਾਂ) &krandom.bin[bin];

	wipe_rnd_bin = bin;

    	ਰੋਕੋ;
    }
    ਹਾਲਤ GET_IRQACTIDS: {
        ਲੰਮਾਈ = ਆਕਾਰ(irq_actids);
        src_vir = (vir_ਬਾਈਟਾਂ) irq_actids;
        ਰੋਕੋ;
    }
    ਹਾਲਤ GET_IDLETSC: {
	ਬਣਤਰ proc * idl;
	update_idle_ਸਮਾਂ();
	idl = proc_addr(IDLE);
        ਲੰਮਾਈ = ਆਕਾਰ(idl->p_ਚੱਕਰਾਂ);
        src_vir = (vir_ਬਾਈਟਾਂ) &idl->p_ਚੱਕਰਾਂ;
        ਰੋਕੋ;
    }
    ਹਾਲਤ GET_CPUTICKS: {
	uint64_t ticks[MINIX_CPUSTATES];
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ cpu;
	cpu = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)m_ptr->m_lsys_krn_sys_getinfo.val_len2_e;
	ਜੇ (cpu >= CONFIG_MAX_CPUS)
		ਵਾਪਸ EINVAL;
	get_cpu_ticks(cpu, ticks);
	ਲੰਮਾਈ = ਆਕਾਰ(ticks);
	src_vir = (vir_ਬਾਈਟਾਂ)ticks;
	ਰੋਕੋ;
    }
    ਮੂਲ:
	ਲਿਖੋ("do_getinfo: invalid request %d\n",
		m_ptr->m_lsys_krn_sys_getinfo.request);
        ਵਾਪਸ(EINVAL);
  }

  /* Try to make the actual copy for the requested data. */
  ਜੇ (m_ptr->m_lsys_krn_sys_getinfo.val_len > 0 &&
	ਲੰਮਾਈ > m_ptr->m_lsys_krn_sys_getinfo.val_len)
	ਵਾਪਸ (E2BIG);

  r = ਅੰਕੜੇ_copy_vmcheck(caller, KERNEL, src_vir, caller->p_endpoint,
	m_ptr->m_lsys_krn_sys_getinfo.val_ptr, ਲੰਮਾਈ);

  ਜੇ(r != OK) ਵਾਪਸ r;

	ਜੇ(wipe_rnd_bin >= 0 && wipe_rnd_bin < RANDOM_SOURCES) {
		krandom.bin[wipe_rnd_bin].r_ਮਾਪ = 0;
		krandom.bin[wipe_rnd_bin].r_ਅਗਲਾ = 0;
	}

  ਵਾਪਸ(OK);
}

#ਜੇ_ਅੰਤ /* USE_GETINFO */

