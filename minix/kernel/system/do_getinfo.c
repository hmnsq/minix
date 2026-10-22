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

#ضمن <string.h>

#ضمن "kernel/system.h"


#אם USE_GETINFO

#ضمن <minix/u64.h>
#ضمن <sys/resource.h>

/*===========================================================================*
 *			        update_idle_time			     *
 *===========================================================================*/
ساكن فراغ update_idle_time(فراغ)
{
	صحيح i;
	بنية proc * idl = proc_addr(IDLE);

	idl->p_cycles = make64(0, 0);

	עבור (i = 0; i < CONFIG_MAX_CPUS ; i++) {
		idl->p_cycles += get_cpu_var(i, idle_proc).p_cycles;
	}
}

/*===========================================================================*
 *			        do_getinfo				     *
 *===========================================================================*/
صحيح do_getinfo(بنية proc * caller, message * m_ptr)
{
/* Request system information to be copied to caller's address space. This
 * call simply copies entire data structures to the caller.
 */
  size_t length;
  vir_bytes src_vir; 
  صحيح nr_e, nr, r;
  صحيح wipe_rnd_bin = -1;
  بنية proc *p;
  بنية rusage r_usage;

  /* Set source address and length based on request type. */
  بدل (m_ptr->m_lsys_krn_sys_getinfo.request) {
    حالة GET_MACHINE: {
        length = حجم(بنية machine);
        src_vir = (vir_bytes) &machine;
        اكسر;
    }
    حالة GET_KINFO: {
        length = حجم(بنية kinfo);
        src_vir = (vir_bytes) &kinfo;
        اكسر;
    }
    حالة GET_LOADINFO: {
        length = حجم(بنية loadinfo);
        src_vir = (vir_bytes) &kloadinfo;
        اكسر;
    }
    حالة GET_CPUINFO: {
        length = حجم(cpu_info);
        src_vir = (vir_bytes) &cpu_info;
        اكسر;
    }
    حالة GET_HZ: {
        length = حجم(system_hz);
        src_vir = (vir_bytes) &system_hz;
        اكسر;
    }
    حالة GET_IMAGE: {
        length = حجم(بنية boot_image) * NR_BOOT_PROCS;
        src_vir = (vir_bytes) image;
        اكسر;
    }
    حالة GET_IRQHOOKS: {
        length = حجم(بنية irq_hook) * NR_IRQ_HOOKS;
        src_vir = (vir_bytes) irq_hooks;
        اكسر;
    }
    حالة GET_PROCTAB: {
	update_idle_time();
        length = حجم(بنية proc) * (NR_PROCS + NR_TASKS);
        src_vir = (vir_bytes) proc;
        اكسر;
    }
    حالة GET_PRIVTAB: {
        length = حجم(بنية priv) * (NR_SYS_PROCS);
        src_vir = (vir_bytes) priv;
        اكسر;
    }
    حالة GET_PROC: {
        nr_e = (m_ptr->m_lsys_krn_sys_getinfo.val_len2_e == SELF) ?
		caller->p_endpoint : m_ptr->m_lsys_krn_sys_getinfo.val_len2_e;
	אם(!isokendpt(nr_e, &nr)) החזר EINVAL; /* validate request */
        length = حجم(بنية proc);
        src_vir = (vir_bytes) proc_addr(nr);
        اكسر;
    }
    حالة GET_PRIV: {
        nr_e = (m_ptr->m_lsys_krn_sys_getinfo.val_len2_e == SELF) ?
            caller->p_endpoint : m_ptr->m_lsys_krn_sys_getinfo.val_len2_e;
        אם(!isokendpt(nr_e, &nr)) החזר EINVAL; /* validate request */
        length = حجم(بنية priv);
        src_vir = (vir_bytes) priv_addr(nr_to_id(nr));
        اكسر;
    }
    حالة GET_REGS: {
        nr_e = (m_ptr->m_lsys_krn_sys_getinfo.val_len2_e == SELF) ?
            caller->p_endpoint : m_ptr->m_lsys_krn_sys_getinfo.val_len2_e;
        אם(!isokendpt(nr_e, &nr)) החזר EINVAL; /* validate request */
        p = proc_addr(nr);
        length = حجم(p->p_reg);
        src_vir = (vir_bytes) &p->p_reg;
        اكسر;
    }
    حالة GET_WHOAMI: {
	صحيح len;
	m_ptr->m_krn_lsys_sys_getwhoami.endpt = caller->p_endpoint;
	len = MIN(حجم(m_ptr->m_krn_lsys_sys_getwhoami.name),
		حجم(caller->p_name))-1;
	strncpy(m_ptr->m_krn_lsys_sys_getwhoami.name, caller->p_name, len);
	m_ptr->m_krn_lsys_sys_getwhoami.name[len] = '\0';
	m_ptr->m_krn_lsys_sys_getwhoami.privflags = priv(caller)->s_flags;
        m_ptr->m_krn_lsys_sys_getwhoami.initflags = priv(caller)->s_init_flags;
	החזר OK;
    }
    حالة GET_MONPARAMS: {
        src_vir = (vir_bytes) kinfo.param_buf;
	length = حجم(kinfo.param_buf);
        اكسر;
    }
    حالة GET_RANDOMNESS: {		
        ساكن بنية k_randomness copy;	/* copy to keep counters */
	صحيح i;

        copy = krandom;
        עבור (i= 0; i<RANDOM_SOURCES; i++) {
  		krandom.bin[i].r_size = 0;	/* invalidate random data */
  		krandom.bin[i].r_next = 0;
	}
    	length = حجم(copy);
    	src_vir = (vir_bytes) &copy;
    	اكسر;
    }
    حالة GET_RANDOMNESS_BIN: {		
	صحيح bin = m_ptr->m_lsys_krn_sys_getinfo.val_len2_e;

	אם(bin < 0 || bin >= RANDOM_SOURCES) {
		printf("SYSTEM: GET_RANDOMNESS_BIN: %d out of range\n", bin);
		החזר EINVAL;
	}

	אם(krandom.bin[bin].r_size < RANDOM_ELEMENTS)
		החזר ENOENT;

    	length = حجم(krandom.bin[bin]);
    	src_vir = (vir_bytes) &krandom.bin[bin];

	wipe_rnd_bin = bin;

    	اكسر;
    }
    حالة GET_IRQACTIDS: {
        length = حجم(irq_actids);
        src_vir = (vir_bytes) irq_actids;
        اكسر;
    }
    حالة GET_IDLETSC: {
	بنية proc * idl;
	update_idle_time();
	idl = proc_addr(IDLE);
        length = حجم(idl->p_cycles);
        src_vir = (vir_bytes) &idl->p_cycles;
        اكسر;
    }
    حالة GET_CPUTICKS: {
	uint64_t ticks[MINIX_CPUSTATES];
	unsigned صحيح cpu;
	cpu = (unsigned صحيح)m_ptr->m_lsys_krn_sys_getinfo.val_len2_e;
	אם (cpu >= CONFIG_MAX_CPUS)
		החזר EINVAL;
	get_cpu_ticks(cpu, ticks);
	length = حجم(ticks);
	src_vir = (vir_bytes)ticks;
	اكسر;
    }
    افتراضي:
	printf("do_getinfo: invalid request %d\n",
		m_ptr->m_lsys_krn_sys_getinfo.request);
        החזר(EINVAL);
  }

  /* Try to make the actual copy for the requested data. */
  אם (m_ptr->m_lsys_krn_sys_getinfo.val_len > 0 &&
	length > m_ptr->m_lsys_krn_sys_getinfo.val_len)
	החזר (E2BIG);

  r = data_copy_vmcheck(caller, KERNEL, src_vir, caller->p_endpoint,
	m_ptr->m_lsys_krn_sys_getinfo.val_ptr, length);

  אם(r != OK) החזר r;

	אם(wipe_rnd_bin >= 0 && wipe_rnd_bin < RANDOM_SOURCES) {
		krandom.bin[wipe_rnd_bin].r_size = 0;
		krandom.bin[wipe_rnd_bin].r_next = 0;
	}

  החזר(OK);
}

#endif /* USE_GETINFO */

