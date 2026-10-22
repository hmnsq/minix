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

#அந்தர்பாவ <string.h>

#அந்தர்பாவ "kernel/system.h"


#எனில் USE_GETINFO

#அந்தர்பாவ <minix/u64.h>
#அந்தர்பாவ <sys/resource.h>

/*===========================================================================*
 *			        update_idle_time			     *
 *===========================================================================*/
நிலையான வெறுமை update_idle_time(வெறுமை)
{
	முழு i;
	கட்டமைப்பு proc * idl = proc_addr(IDLE);

	idl->p_cycles = make64(0, 0);

	க்கு (i = 0; i < CONFIG_MAX_CPUS ; i++) {
		idl->p_cycles += get_cpu_var(i, idle_proc).p_cycles;
	}
}

/*===========================================================================*
 *			        do_getinfo				     *
 *===========================================================================*/
முழு do_getinfo(கட்டமைப்பு proc * caller, message * m_ptr)
{
/* Request system information to be copied to caller's address space. This
 * call simply copies entire data structures to the caller.
 */
  size_t length;
  vir_bytes src_vir; 
  முழு nr_e, nr, r;
  முழு wipe_rnd_bin = -1;
  கட்டமைப்பு proc *p;
  கட்டமைப்பு rusage r_usage;

  /* Set source address and length based on request type. */
  தேர்வு (m_ptr->m_lsys_krn_sys_getinfo.request) {
    நிலை GET_MACHINE: {
        length = அளவு(கட்டமைப்பு machine);
        src_vir = (vir_bytes) &machine;
        நிறுத்து;
    }
    நிலை GET_KINFO: {
        length = அளவு(கட்டமைப்பு kinfo);
        src_vir = (vir_bytes) &kinfo;
        நிறுத்து;
    }
    நிலை GET_LOADINFO: {
        length = அளவு(கட்டமைப்பு loadinfo);
        src_vir = (vir_bytes) &kloadinfo;
        நிறுத்து;
    }
    நிலை GET_CPUINFO: {
        length = அளவு(cpu_info);
        src_vir = (vir_bytes) &cpu_info;
        நிறுத்து;
    }
    நிலை GET_HZ: {
        length = அளவு(system_hz);
        src_vir = (vir_bytes) &system_hz;
        நிறுத்து;
    }
    நிலை GET_IMAGE: {
        length = அளவு(கட்டமைப்பு boot_image) * NR_BOOT_PROCS;
        src_vir = (vir_bytes) image;
        நிறுத்து;
    }
    நிலை GET_IRQHOOKS: {
        length = அளவு(கட்டமைப்பு irq_hook) * NR_IRQ_HOOKS;
        src_vir = (vir_bytes) irq_hooks;
        நிறுத்து;
    }
    நிலை GET_PROCTAB: {
	update_idle_time();
        length = அளவு(கட்டமைப்பு proc) * (NR_PROCS + NR_TASKS);
        src_vir = (vir_bytes) proc;
        நிறுத்து;
    }
    நிலை GET_PRIVTAB: {
        length = அளவு(கட்டமைப்பு priv) * (NR_SYS_PROCS);
        src_vir = (vir_bytes) priv;
        நிறுத்து;
    }
    நிலை GET_PROC: {
        nr_e = (m_ptr->m_lsys_krn_sys_getinfo.val_len2_e == SELF) ?
		caller->p_endpoint : m_ptr->m_lsys_krn_sys_getinfo.val_len2_e;
	எனில்(!isokendpt(nr_e, &nr)) திரும்பு EINVAL; /* validate request */
        length = அளவு(கட்டமைப்பு proc);
        src_vir = (vir_bytes) proc_addr(nr);
        நிறுத்து;
    }
    நிலை GET_PRIV: {
        nr_e = (m_ptr->m_lsys_krn_sys_getinfo.val_len2_e == SELF) ?
            caller->p_endpoint : m_ptr->m_lsys_krn_sys_getinfo.val_len2_e;
        எனில்(!isokendpt(nr_e, &nr)) திரும்பு EINVAL; /* validate request */
        length = அளவு(கட்டமைப்பு priv);
        src_vir = (vir_bytes) priv_addr(nr_to_id(nr));
        நிறுத்து;
    }
    நிலை GET_REGS: {
        nr_e = (m_ptr->m_lsys_krn_sys_getinfo.val_len2_e == SELF) ?
            caller->p_endpoint : m_ptr->m_lsys_krn_sys_getinfo.val_len2_e;
        எனில்(!isokendpt(nr_e, &nr)) திரும்பு EINVAL; /* validate request */
        p = proc_addr(nr);
        length = அளவு(p->p_reg);
        src_vir = (vir_bytes) &p->p_reg;
        நிறுத்து;
    }
    நிலை GET_WHOAMI: {
	முழு len;
	m_ptr->m_krn_lsys_sys_getwhoami.endpt = caller->p_endpoint;
	len = MIN(அளவு(m_ptr->m_krn_lsys_sys_getwhoami.name),
		அளவு(caller->p_name))-1;
	strncpy(m_ptr->m_krn_lsys_sys_getwhoami.name, caller->p_name, len);
	m_ptr->m_krn_lsys_sys_getwhoami.name[len] = '\0';
	m_ptr->m_krn_lsys_sys_getwhoami.privflags = priv(caller)->s_flags;
        m_ptr->m_krn_lsys_sys_getwhoami.initflags = priv(caller)->s_init_flags;
	திரும்பு OK;
    }
    நிலை GET_MONPARAMS: {
        src_vir = (vir_bytes) kinfo.param_buf;
	length = அளவு(kinfo.param_buf);
        நிறுத்து;
    }
    நிலை GET_RANDOMNESS: {		
        நிலையான கட்டமைப்பு k_randomness copy;	/* copy to keep counters */
	முழு i;

        copy = krandom;
        க்கு (i= 0; i<RANDOM_SOURCES; i++) {
  		krandom.bin[i].r_size = 0;	/* invalidate random data */
  		krandom.bin[i].r_next = 0;
	}
    	length = அளவு(copy);
    	src_vir = (vir_bytes) &copy;
    	நிறுத்து;
    }
    நிலை GET_RANDOMNESS_BIN: {		
	முழு bin = m_ptr->m_lsys_krn_sys_getinfo.val_len2_e;

	எனில்(bin < 0 || bin >= RANDOM_SOURCES) {
		printf("SYSTEM: GET_RANDOMNESS_BIN: %d out of range\n", bin);
		திரும்பு EINVAL;
	}

	எனில்(krandom.bin[bin].r_size < RANDOM_ELEMENTS)
		திரும்பு ENOENT;

    	length = அளவு(krandom.bin[bin]);
    	src_vir = (vir_bytes) &krandom.bin[bin];

	wipe_rnd_bin = bin;

    	நிறுத்து;
    }
    நிலை GET_IRQACTIDS: {
        length = அளவு(irq_actids);
        src_vir = (vir_bytes) irq_actids;
        நிறுத்து;
    }
    நிலை GET_IDLETSC: {
	கட்டமைப்பு proc * idl;
	update_idle_time();
	idl = proc_addr(IDLE);
        length = அளவு(idl->p_cycles);
        src_vir = (vir_bytes) &idl->p_cycles;
        நிறுத்து;
    }
    நிலை GET_CPUTICKS: {
	uint64_t ticks[MINIX_CPUSTATES];
	unsigned முழு cpu;
	cpu = (unsigned முழு)m_ptr->m_lsys_krn_sys_getinfo.val_len2_e;
	எனில் (cpu >= CONFIG_MAX_CPUS)
		திரும்பு EINVAL;
	get_cpu_ticks(cpu, ticks);
	length = அளவு(ticks);
	src_vir = (vir_bytes)ticks;
	நிறுத்து;
    }
    இயல்பு:
	printf("do_getinfo: invalid request %d\n",
		m_ptr->m_lsys_krn_sys_getinfo.request);
        திரும்பு(EINVAL);
  }

  /* Try to make the actual copy for the requested data. */
  எனில் (m_ptr->m_lsys_krn_sys_getinfo.val_len > 0 &&
	length > m_ptr->m_lsys_krn_sys_getinfo.val_len)
	திரும்பு (E2BIG);

  r = data_copy_vmcheck(caller, KERNEL, src_vir, caller->p_endpoint,
	m_ptr->m_lsys_krn_sys_getinfo.val_ptr, length);

  எனில்(r != OK) திரும்பு r;

	எனில்(wipe_rnd_bin >= 0 && wipe_rnd_bin < RANDOM_SOURCES) {
		krandom.bin[wipe_rnd_bin].r_size = 0;
		krandom.bin[wipe_rnd_bin].r_next = 0;
	}

  திரும்பு(OK);
}

#endif /* USE_GETINFO */

