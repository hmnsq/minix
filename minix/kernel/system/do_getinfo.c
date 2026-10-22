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

#ಅನ್ತರ್ಭಾವ <string.h>

#ಅನ್ತರ್ಭಾವ "kernel/system.h"


#ಯದಿ USE_GETINFO

#ಅನ್ತರ್ಭಾವ <minix/u64.h>
#ಅನ್ತರ್ಭಾವ <sys/resource.h>

/*===========================================================================*
 *			        update_idle_time			     *
 *===========================================================================*/
ಸ್ಥಿರ ಶೂನ್ಯ update_idle_time(ಶೂನ್ಯ)
{
	ಪೂರ್ಣಾಙ್ಕ i;
	ಸಂರಚನಾ proc * idl = proc_addr(IDLE);

	idl->p_cycles = make64(0, 0);

	ಪ್ರತಿ (i = 0; i < CONFIG_MAX_CPUS ; i++) {
		idl->p_cycles += get_cpu_var(i, idle_proc).p_cycles;
	}
}

/*===========================================================================*
 *			        do_getinfo				     *
 *===========================================================================*/
ಪೂರ್ಣಾಙ್ಕ do_getinfo(ಸಂರಚನಾ proc * caller, message * m_ptr)
{
/* Request system information to be copied to caller's address space. This
 * call simply copies entire data structures to the caller.
 */
  size_t length;
  vir_bytes src_vir; 
  ಪೂರ್ಣಾಙ್ಕ nr_e, nr, r;
  ಪೂರ್ಣಾಙ್ಕ wipe_rnd_bin = -1;
  ಸಂರಚನಾ proc *p;
  ಸಂರಚನಾ rusage r_usage;

  /* Set source address and length based on request type. */
  ವಿಕಲ್ಪ (m_ptr->m_lsys_krn_sys_getinfo.request) {
    ಪಕ್ಷ GET_MACHINE: {
        length = ಪರಿಮಾಣ(ಸಂರಚನಾ machine);
        src_vir = (vir_bytes) &machine;
        ಭಙ್ಗ;
    }
    ಪಕ್ಷ GET_KINFO: {
        length = ಪರಿಮಾಣ(ಸಂರಚನಾ kinfo);
        src_vir = (vir_bytes) &kinfo;
        ಭಙ್ಗ;
    }
    ಪಕ್ಷ GET_LOADINFO: {
        length = ಪರಿಮಾಣ(ಸಂರಚನಾ loadinfo);
        src_vir = (vir_bytes) &kloadinfo;
        ಭಙ್ಗ;
    }
    ಪಕ್ಷ GET_CPUINFO: {
        length = ಪರಿಮಾಣ(cpu_info);
        src_vir = (vir_bytes) &cpu_info;
        ಭಙ್ಗ;
    }
    ಪಕ್ಷ GET_HZ: {
        length = ಪರಿಮಾಣ(system_hz);
        src_vir = (vir_bytes) &system_hz;
        ಭಙ್ಗ;
    }
    ಪಕ್ಷ GET_IMAGE: {
        length = ಪರಿಮಾಣ(ಸಂರಚನಾ boot_image) * NR_BOOT_PROCS;
        src_vir = (vir_bytes) image;
        ಭಙ್ಗ;
    }
    ಪಕ್ಷ GET_IRQHOOKS: {
        length = ಪರಿಮಾಣ(ಸಂರಚನಾ irq_hook) * NR_IRQ_HOOKS;
        src_vir = (vir_bytes) irq_hooks;
        ಭಙ್ಗ;
    }
    ಪಕ್ಷ GET_PROCTAB: {
	update_idle_time();
        length = ಪರಿಮಾಣ(ಸಂರಚನಾ proc) * (NR_PROCS + NR_TASKS);
        src_vir = (vir_bytes) proc;
        ಭಙ್ಗ;
    }
    ಪಕ್ಷ GET_PRIVTAB: {
        length = ಪರಿಮಾಣ(ಸಂರಚನಾ priv) * (NR_SYS_PROCS);
        src_vir = (vir_bytes) priv;
        ಭಙ್ಗ;
    }
    ಪಕ್ಷ GET_PROC: {
        nr_e = (m_ptr->m_lsys_krn_sys_getinfo.val_len2_e == SELF) ?
		caller->p_endpoint : m_ptr->m_lsys_krn_sys_getinfo.val_len2_e;
	ಯದಿ(!isokendpt(nr_e, &nr)) ಪ್ರತ್ಯಾಗಮ EINVAL; /* validate request */
        length = ಪರಿಮಾಣ(ಸಂರಚನಾ proc);
        src_vir = (vir_bytes) proc_addr(nr);
        ಭಙ್ಗ;
    }
    ಪಕ್ಷ GET_PRIV: {
        nr_e = (m_ptr->m_lsys_krn_sys_getinfo.val_len2_e == SELF) ?
            caller->p_endpoint : m_ptr->m_lsys_krn_sys_getinfo.val_len2_e;
        ಯದಿ(!isokendpt(nr_e, &nr)) ಪ್ರತ್ಯಾಗಮ EINVAL; /* validate request */
        length = ಪರಿಮಾಣ(ಸಂರಚನಾ priv);
        src_vir = (vir_bytes) priv_addr(nr_to_id(nr));
        ಭಙ್ಗ;
    }
    ಪಕ್ಷ GET_REGS: {
        nr_e = (m_ptr->m_lsys_krn_sys_getinfo.val_len2_e == SELF) ?
            caller->p_endpoint : m_ptr->m_lsys_krn_sys_getinfo.val_len2_e;
        ಯದಿ(!isokendpt(nr_e, &nr)) ಪ್ರತ್ಯಾಗಮ EINVAL; /* validate request */
        p = proc_addr(nr);
        length = ಪರಿಮಾಣ(p->p_reg);
        src_vir = (vir_bytes) &p->p_reg;
        ಭಙ್ಗ;
    }
    ಪಕ್ಷ GET_WHOAMI: {
	ಪೂರ್ಣಾಙ್ಕ len;
	m_ptr->m_krn_lsys_sys_getwhoami.endpt = caller->p_endpoint;
	len = MIN(ಪರಿಮಾಣ(m_ptr->m_krn_lsys_sys_getwhoami.name),
		ಪರಿಮಾಣ(caller->p_name))-1;
	strncpy(m_ptr->m_krn_lsys_sys_getwhoami.name, caller->p_name, len);
	m_ptr->m_krn_lsys_sys_getwhoami.name[len] = '\0';
	m_ptr->m_krn_lsys_sys_getwhoami.privflags = priv(caller)->s_flags;
        m_ptr->m_krn_lsys_sys_getwhoami.initflags = priv(caller)->s_init_flags;
	ಪ್ರತ್ಯಾಗಮ OK;
    }
    ಪಕ್ಷ GET_MONPARAMS: {
        src_vir = (vir_bytes) kinfo.param_buf;
	length = ಪರಿಮಾಣ(kinfo.param_buf);
        ಭಙ್ಗ;
    }
    ಪಕ್ಷ GET_RANDOMNESS: {		
        ಸ್ಥಿರ ಸಂರಚನಾ k_randomness copy;	/* copy to keep counters */
	ಪೂರ್ಣಾಙ್ಕ i;

        copy = krandom;
        ಪ್ರತಿ (i= 0; i<RANDOM_SOURCES; i++) {
  		krandom.bin[i].r_size = 0;	/* invalidate random data */
  		krandom.bin[i].r_next = 0;
	}
    	length = ಪರಿಮಾಣ(copy);
    	src_vir = (vir_bytes) &copy;
    	ಭಙ್ಗ;
    }
    ಪಕ್ಷ GET_RANDOMNESS_BIN: {		
	ಪೂರ್ಣಾಙ್ಕ bin = m_ptr->m_lsys_krn_sys_getinfo.val_len2_e;

	ಯದಿ(bin < 0 || bin >= RANDOM_SOURCES) {
		printf("SYSTEM: GET_RANDOMNESS_BIN: %d out of range\n", bin);
		ಪ್ರತ್ಯಾಗಮ EINVAL;
	}

	ಯದಿ(krandom.bin[bin].r_size < RANDOM_ELEMENTS)
		ಪ್ರತ್ಯಾಗಮ ENOENT;

    	length = ಪರಿಮಾಣ(krandom.bin[bin]);
    	src_vir = (vir_bytes) &krandom.bin[bin];

	wipe_rnd_bin = bin;

    	ಭಙ್ಗ;
    }
    ಪಕ್ಷ GET_IRQACTIDS: {
        length = ಪರಿಮಾಣ(irq_actids);
        src_vir = (vir_bytes) irq_actids;
        ಭಙ್ಗ;
    }
    ಪಕ್ಷ GET_IDLETSC: {
	ಸಂರಚನಾ proc * idl;
	update_idle_time();
	idl = proc_addr(IDLE);
        length = ಪರಿಮಾಣ(idl->p_cycles);
        src_vir = (vir_bytes) &idl->p_cycles;
        ಭಙ್ಗ;
    }
    ಪಕ್ಷ GET_CPUTICKS: {
	uint64_t ticks[MINIX_CPUSTATES];
	unsigned ಪೂರ್ಣಾಙ್ಕ cpu;
	cpu = (unsigned ಪೂರ್ಣಾಙ್ಕ)m_ptr->m_lsys_krn_sys_getinfo.val_len2_e;
	ಯದಿ (cpu >= CONFIG_MAX_CPUS)
		ಪ್ರತ್ಯಾಗಮ EINVAL;
	get_cpu_ticks(cpu, ticks);
	length = ಪರಿಮಾಣ(ticks);
	src_vir = (vir_bytes)ticks;
	ಭಙ್ಗ;
    }
    ಸ್ವಭಾವ:
	printf("do_getinfo: invalid request %d\n",
		m_ptr->m_lsys_krn_sys_getinfo.request);
        ಪ್ರತ್ಯಾಗಮ(EINVAL);
  }

  /* Try to make the actual copy for the requested data. */
  ಯದಿ (m_ptr->m_lsys_krn_sys_getinfo.val_len > 0 &&
	length > m_ptr->m_lsys_krn_sys_getinfo.val_len)
	ಪ್ರತ್ಯಾಗಮ (E2BIG);

  r = data_copy_vmcheck(caller, KERNEL, src_vir, caller->p_endpoint,
	m_ptr->m_lsys_krn_sys_getinfo.val_ptr, length);

  ಯದಿ(r != OK) ಪ್ರತ್ಯಾಗಮ r;

	ಯದಿ(wipe_rnd_bin >= 0 && wipe_rnd_bin < RANDOM_SOURCES) {
		krandom.bin[wipe_rnd_bin].r_size = 0;
		krandom.bin[wipe_rnd_bin].r_next = 0;
	}

  ಪ್ರತ್ಯಾಗಮ(OK);
}

#endif /* USE_GETINFO */

