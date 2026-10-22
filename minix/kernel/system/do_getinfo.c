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

#അന്തര്ഭാവ <string.h>

#അന്തര്ഭാവ "kernel/system.h"


#എങ്കിൽ USE_GETINFO

#അന്തര്ഭാവ <minix/u64.h>
#അന്തര്ഭാവ <sys/resource.h>

/*===========================================================================*
 *			        update_idle_time			     *
 *===========================================================================*/
സ്ഥിതം ശൂന്യം update_idle_time(ശൂന്യം)
{
	പൂർണ്ണസംഖ്യ i;
	ഘടന proc * idl = proc_addr(IDLE);

	idl->p_cycles = make64(0, 0);

	വേണ്ടി (i = 0; i < CONFIG_MAX_CPUS ; i++) {
		idl->p_cycles += get_cpu_var(i, idle_proc).p_cycles;
	}
}

/*===========================================================================*
 *			        do_getinfo				     *
 *===========================================================================*/
പൂർണ്ണസംഖ്യ do_getinfo(ഘടന proc * caller, message * m_ptr)
{
/* Request system information to be copied to caller's address space. This
 * call simply copies entire data structures to the caller.
 */
  size_t length;
  vir_bytes src_vir; 
  പൂർണ്ണസംഖ്യ nr_e, nr, r;
  പൂർണ്ണസംഖ്യ wipe_rnd_bin = -1;
  ഘടന proc *p;
  ഘടന rusage r_usage;

  /* Set source address and length based on request type. */
  തിരഞ്ഞെടുപ്പ് (m_ptr->m_lsys_krn_sys_getinfo.request) {
    അവസ്ഥ GET_MACHINE: {
        length = വലുപ്പം(ഘടന machine);
        src_vir = (vir_bytes) &machine;
        നിർത്തുക;
    }
    അവസ്ഥ GET_KINFO: {
        length = വലുപ്പം(ഘടന kinfo);
        src_vir = (vir_bytes) &kinfo;
        നിർത്തുക;
    }
    അവസ്ഥ GET_LOADINFO: {
        length = വലുപ്പം(ഘടന loadinfo);
        src_vir = (vir_bytes) &kloadinfo;
        നിർത്തുക;
    }
    അവസ്ഥ GET_CPUINFO: {
        length = വലുപ്പം(cpu_info);
        src_vir = (vir_bytes) &cpu_info;
        നിർത്തുക;
    }
    അവസ്ഥ GET_HZ: {
        length = വലുപ്പം(system_hz);
        src_vir = (vir_bytes) &system_hz;
        നിർത്തുക;
    }
    അവസ്ഥ GET_IMAGE: {
        length = വലുപ്പം(ഘടന boot_image) * NR_BOOT_PROCS;
        src_vir = (vir_bytes) image;
        നിർത്തുക;
    }
    അവസ്ഥ GET_IRQHOOKS: {
        length = വലുപ്പം(ഘടന irq_hook) * NR_IRQ_HOOKS;
        src_vir = (vir_bytes) irq_hooks;
        നിർത്തുക;
    }
    അവസ്ഥ GET_PROCTAB: {
	update_idle_time();
        length = വലുപ്പം(ഘടന proc) * (NR_PROCS + NR_TASKS);
        src_vir = (vir_bytes) proc;
        നിർത്തുക;
    }
    അവസ്ഥ GET_PRIVTAB: {
        length = വലുപ്പം(ഘടന priv) * (NR_SYS_PROCS);
        src_vir = (vir_bytes) priv;
        നിർത്തുക;
    }
    അവസ്ഥ GET_PROC: {
        nr_e = (m_ptr->m_lsys_krn_sys_getinfo.val_len2_e == SELF) ?
		caller->p_endpoint : m_ptr->m_lsys_krn_sys_getinfo.val_len2_e;
	എങ്കിൽ(!isokendpt(nr_e, &nr)) തിരികെ EINVAL; /* validate request */
        length = വലുപ്പം(ഘടന proc);
        src_vir = (vir_bytes) proc_addr(nr);
        നിർത്തുക;
    }
    അവസ്ഥ GET_PRIV: {
        nr_e = (m_ptr->m_lsys_krn_sys_getinfo.val_len2_e == SELF) ?
            caller->p_endpoint : m_ptr->m_lsys_krn_sys_getinfo.val_len2_e;
        എങ്കിൽ(!isokendpt(nr_e, &nr)) തിരികെ EINVAL; /* validate request */
        length = വലുപ്പം(ഘടന priv);
        src_vir = (vir_bytes) priv_addr(nr_to_id(nr));
        നിർത്തുക;
    }
    അവസ്ഥ GET_REGS: {
        nr_e = (m_ptr->m_lsys_krn_sys_getinfo.val_len2_e == SELF) ?
            caller->p_endpoint : m_ptr->m_lsys_krn_sys_getinfo.val_len2_e;
        എങ്കിൽ(!isokendpt(nr_e, &nr)) തിരികെ EINVAL; /* validate request */
        p = proc_addr(nr);
        length = വലുപ്പം(p->p_reg);
        src_vir = (vir_bytes) &p->p_reg;
        നിർത്തുക;
    }
    അവസ്ഥ GET_WHOAMI: {
	പൂർണ്ണസംഖ്യ len;
	m_ptr->m_krn_lsys_sys_getwhoami.endpt = caller->p_endpoint;
	len = MIN(വലുപ്പം(m_ptr->m_krn_lsys_sys_getwhoami.name),
		വലുപ്പം(caller->p_name))-1;
	strncpy(m_ptr->m_krn_lsys_sys_getwhoami.name, caller->p_name, len);
	m_ptr->m_krn_lsys_sys_getwhoami.name[len] = '\0';
	m_ptr->m_krn_lsys_sys_getwhoami.privflags = priv(caller)->s_flags;
        m_ptr->m_krn_lsys_sys_getwhoami.initflags = priv(caller)->s_init_flags;
	തിരികെ OK;
    }
    അവസ്ഥ GET_MONPARAMS: {
        src_vir = (vir_bytes) kinfo.param_buf;
	length = വലുപ്പം(kinfo.param_buf);
        നിർത്തുക;
    }
    അവസ്ഥ GET_RANDOMNESS: {		
        സ്ഥിതം ഘടന k_randomness copy;	/* copy to keep counters */
	പൂർണ്ണസംഖ്യ i;

        copy = krandom;
        വേണ്ടി (i= 0; i<RANDOM_SOURCES; i++) {
  		krandom.bin[i].r_size = 0;	/* invalidate random data */
  		krandom.bin[i].r_next = 0;
	}
    	length = വലുപ്പം(copy);
    	src_vir = (vir_bytes) &copy;
    	നിർത്തുക;
    }
    അവസ്ഥ GET_RANDOMNESS_BIN: {		
	പൂർണ്ണസംഖ്യ bin = m_ptr->m_lsys_krn_sys_getinfo.val_len2_e;

	എങ്കിൽ(bin < 0 || bin >= RANDOM_SOURCES) {
		printf("SYSTEM: GET_RANDOMNESS_BIN: %d out of range\n", bin);
		തിരികെ EINVAL;
	}

	എങ്കിൽ(krandom.bin[bin].r_size < RANDOM_ELEMENTS)
		തിരികെ ENOENT;

    	length = വലുപ്പം(krandom.bin[bin]);
    	src_vir = (vir_bytes) &krandom.bin[bin];

	wipe_rnd_bin = bin;

    	നിർത്തുക;
    }
    അവസ്ഥ GET_IRQACTIDS: {
        length = വലുപ്പം(irq_actids);
        src_vir = (vir_bytes) irq_actids;
        നിർത്തുക;
    }
    അവസ്ഥ GET_IDLETSC: {
	ഘടന proc * idl;
	update_idle_time();
	idl = proc_addr(IDLE);
        length = വലുപ്പം(idl->p_cycles);
        src_vir = (vir_bytes) &idl->p_cycles;
        നിർത്തുക;
    }
    അവസ്ഥ GET_CPUTICKS: {
	uint64_t ticks[MINIX_CPUSTATES];
	unsigned പൂർണ്ണസംഖ്യ cpu;
	cpu = (unsigned പൂർണ്ണസംഖ്യ)m_ptr->m_lsys_krn_sys_getinfo.val_len2_e;
	എങ്കിൽ (cpu >= CONFIG_MAX_CPUS)
		തിരികെ EINVAL;
	get_cpu_ticks(cpu, ticks);
	length = വലുപ്പം(ticks);
	src_vir = (vir_bytes)ticks;
	നിർത്തുക;
    }
    സ്ഥിരസ്ഥിതി:
	printf("do_getinfo: invalid request %d\n",
		m_ptr->m_lsys_krn_sys_getinfo.request);
        തിരികെ(EINVAL);
  }

  /* Try to make the actual copy for the requested data. */
  എങ്കിൽ (m_ptr->m_lsys_krn_sys_getinfo.val_len > 0 &&
	length > m_ptr->m_lsys_krn_sys_getinfo.val_len)
	തിരികെ (E2BIG);

  r = data_copy_vmcheck(caller, KERNEL, src_vir, caller->p_endpoint,
	m_ptr->m_lsys_krn_sys_getinfo.val_ptr, length);

  എങ്കിൽ(r != OK) തിരികെ r;

	എങ്കിൽ(wipe_rnd_bin >= 0 && wipe_rnd_bin < RANDOM_SOURCES) {
		krandom.bin[wipe_rnd_bin].r_size = 0;
		krandom.bin[wipe_rnd_bin].r_next = 0;
	}

  തിരികെ(OK);
}

#endif /* USE_GETINFO */

