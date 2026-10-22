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

#شامل <string.h>

#شامل "kernel/system.h"


#اگر USE_GETINFO

#شامل <minix/u64.h>
#شامل <sys/resource.h>

/*===========================================================================*
 *			        update_idle_time			     *
 *===========================================================================*/
ایستا تهی update_idle_time(تهی)
{
	صحیح i;
	ساختار proc * idl = proc_addr(IDLE);

	idl->p_cycles = make64(0, 0);

	برای (i = 0; i < CONFIG_MAX_CPUS ; i++) {
		idl->p_cycles += get_cpu_var(i, idle_proc).p_cycles;
	}
}

/*===========================================================================*
 *			        do_getinfo				     *
 *===========================================================================*/
صحیح do_getinfo(ساختار proc * caller, message * m_ptr)
{
/* Request system information to be copied to caller's address space. This
 * call simply copies entire data structures to the caller.
 */
  size_t length;
  vir_bytes src_vir; 
  صحیح nr_e, nr, r;
  صحیح wipe_rnd_bin = -1;
  ساختار proc *p;
  ساختار rusage r_usage;

  /* Set source address and length based on request type. */
  انتخاب (m_ptr->m_lsys_krn_sys_getinfo.request) {
    حالت GET_MACHINE: {
        length = اندازه(ساختار machine);
        src_vir = (vir_bytes) &machine;
        بشکن;
    }
    حالت GET_KINFO: {
        length = اندازه(ساختار kinfo);
        src_vir = (vir_bytes) &kinfo;
        بشکن;
    }
    حالت GET_LOADINFO: {
        length = اندازه(ساختار loadinfo);
        src_vir = (vir_bytes) &kloadinfo;
        بشکن;
    }
    حالت GET_CPUINFO: {
        length = اندازه(cpu_info);
        src_vir = (vir_bytes) &cpu_info;
        بشکن;
    }
    حالت GET_HZ: {
        length = اندازه(system_hz);
        src_vir = (vir_bytes) &system_hz;
        بشکن;
    }
    حالت GET_IMAGE: {
        length = اندازه(ساختار boot_image) * NR_BOOT_PROCS;
        src_vir = (vir_bytes) image;
        بشکن;
    }
    حالت GET_IRQHOOKS: {
        length = اندازه(ساختار irq_hook) * NR_IRQ_HOOKS;
        src_vir = (vir_bytes) irq_hooks;
        بشکن;
    }
    حالت GET_PROCTAB: {
	update_idle_time();
        length = اندازه(ساختار proc) * (NR_PROCS + NR_TASKS);
        src_vir = (vir_bytes) proc;
        بشکن;
    }
    حالت GET_PRIVTAB: {
        length = اندازه(ساختار priv) * (NR_SYS_PROCS);
        src_vir = (vir_bytes) priv;
        بشکن;
    }
    حالت GET_PROC: {
        nr_e = (m_ptr->m_lsys_krn_sys_getinfo.val_len2_e == SELF) ?
		caller->p_endpoint : m_ptr->m_lsys_krn_sys_getinfo.val_len2_e;
	اگر(!isokendpt(nr_e, &nr)) بازگشت EINVAL; /* validate request */
        length = اندازه(ساختار proc);
        src_vir = (vir_bytes) proc_addr(nr);
        بشکن;
    }
    حالت GET_PRIV: {
        nr_e = (m_ptr->m_lsys_krn_sys_getinfo.val_len2_e == SELF) ?
            caller->p_endpoint : m_ptr->m_lsys_krn_sys_getinfo.val_len2_e;
        اگر(!isokendpt(nr_e, &nr)) بازگشت EINVAL; /* validate request */
        length = اندازه(ساختار priv);
        src_vir = (vir_bytes) priv_addr(nr_to_id(nr));
        بشکن;
    }
    حالت GET_REGS: {
        nr_e = (m_ptr->m_lsys_krn_sys_getinfo.val_len2_e == SELF) ?
            caller->p_endpoint : m_ptr->m_lsys_krn_sys_getinfo.val_len2_e;
        اگر(!isokendpt(nr_e, &nr)) بازگشت EINVAL; /* validate request */
        p = proc_addr(nr);
        length = اندازه(p->p_reg);
        src_vir = (vir_bytes) &p->p_reg;
        بشکن;
    }
    حالت GET_WHOAMI: {
	صحیح len;
	m_ptr->m_krn_lsys_sys_getwhoami.endpt = caller->p_endpoint;
	len = MIN(اندازه(m_ptr->m_krn_lsys_sys_getwhoami.name),
		اندازه(caller->p_name))-1;
	strncpy(m_ptr->m_krn_lsys_sys_getwhoami.name, caller->p_name, len);
	m_ptr->m_krn_lsys_sys_getwhoami.name[len] = '\0';
	m_ptr->m_krn_lsys_sys_getwhoami.privflags = priv(caller)->s_flags;
        m_ptr->m_krn_lsys_sys_getwhoami.initflags = priv(caller)->s_init_flags;
	بازگشت OK;
    }
    حالت GET_MONPARAMS: {
        src_vir = (vir_bytes) kinfo.param_buf;
	length = اندازه(kinfo.param_buf);
        بشکن;
    }
    حالت GET_RANDOMNESS: {		
        ایستا ساختار k_randomness copy;	/* copy to keep counters */
	صحیح i;

        copy = krandom;
        برای (i= 0; i<RANDOM_SOURCES; i++) {
  		krandom.bin[i].r_size = 0;	/* invalidate random data */
  		krandom.bin[i].r_next = 0;
	}
    	length = اندازه(copy);
    	src_vir = (vir_bytes) &copy;
    	بشکن;
    }
    حالت GET_RANDOMNESS_BIN: {		
	صحیح bin = m_ptr->m_lsys_krn_sys_getinfo.val_len2_e;

	اگر(bin < 0 || bin >= RANDOM_SOURCES) {
		printf("SYSTEM: GET_RANDOMNESS_BIN: %d out of range\n", bin);
		بازگشت EINVAL;
	}

	اگر(krandom.bin[bin].r_size < RANDOM_ELEMENTS)
		بازگشت ENOENT;

    	length = اندازه(krandom.bin[bin]);
    	src_vir = (vir_bytes) &krandom.bin[bin];

	wipe_rnd_bin = bin;

    	بشکن;
    }
    حالت GET_IRQACTIDS: {
        length = اندازه(irq_actids);
        src_vir = (vir_bytes) irq_actids;
        بشکن;
    }
    حالت GET_IDLETSC: {
	ساختار proc * idl;
	update_idle_time();
	idl = proc_addr(IDLE);
        length = اندازه(idl->p_cycles);
        src_vir = (vir_bytes) &idl->p_cycles;
        بشکن;
    }
    حالت GET_CPUTICKS: {
	uint64_t ticks[MINIX_CPUSTATES];
	unsigned صحیح cpu;
	cpu = (unsigned صحیح)m_ptr->m_lsys_krn_sys_getinfo.val_len2_e;
	اگر (cpu >= CONFIG_MAX_CPUS)
		بازگشت EINVAL;
	get_cpu_ticks(cpu, ticks);
	length = اندازه(ticks);
	src_vir = (vir_bytes)ticks;
	بشکن;
    }
    پیشفرض:
	printf("do_getinfo: invalid request %d\n",
		m_ptr->m_lsys_krn_sys_getinfo.request);
        بازگشت(EINVAL);
  }

  /* Try to make the actual copy for the requested data. */
  اگر (m_ptr->m_lsys_krn_sys_getinfo.val_len > 0 &&
	length > m_ptr->m_lsys_krn_sys_getinfo.val_len)
	بازگشت (E2BIG);

  r = data_copy_vmcheck(caller, KERNEL, src_vir, caller->p_endpoint,
	m_ptr->m_lsys_krn_sys_getinfo.val_ptr, length);

  اگر(r != OK) بازگشت r;

	اگر(wipe_rnd_bin >= 0 && wipe_rnd_bin < RANDOM_SOURCES) {
		krandom.bin[wipe_rnd_bin].r_size = 0;
		krandom.bin[wipe_rnd_bin].r_next = 0;
	}

  بازگشت(OK);
}

#endif /* USE_GETINFO */

