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


#إذا USE_GETINFO

#ضمن <minix/u64.h>
#ضمن <sys/resource.h>

/*===========================================================================*
 *			        update_idle_time			     *
 *===========================================================================*/
ساكن فراغ update_idle_وقت(فراغ)
{
	صحيح i;
	بنية proc * idl = proc_addr(IDLE);

	idl->p_چڪرن = make64(0, 0);

	لكل (i = 0; i < CONFIG_MAX_CPUS ; i++) {
		idl->p_چڪرن += get_cpu_var(i, idle_proc).p_چڪرن;
	}
}

/*===========================================================================*
 *			        do_getinfo				     *
 *===========================================================================*/
صحيح do_getinfo(بنية proc * caller, پيغام * m_ptr)
{
/* Request system information to be copied to caller's address space. This
 * call simply copies entire data structures to the caller.
 */
  size_t ڊيگهائي;
  vir_بائيٽون src_vir; 
  صحيح nr_e, nr, r;
  صحيح wipe_rnd_bin = -1;
  بنية proc *p;
  بنية rusage r_usage;

  /* Set source address and length based on request type. */
  بدل (m_ptr->m_lsys_krn_sys_getinfo.request) {
    حالة GET_MACHINE: {
        ڊيگهائي = حجم(بنية machine);
        src_vir = (vir_بائيٽون) &machine;
        اكسر;
    }
    حالة GET_KINFO: {
        ڊيگهائي = حجم(بنية kinfo);
        src_vir = (vir_بائيٽون) &kinfo;
        اكسر;
    }
    حالة GET_LOADINFO: {
        ڊيگهائي = حجم(بنية loadinfo);
        src_vir = (vir_بائيٽون) &kloadinfo;
        اكسر;
    }
    حالة GET_CPUINFO: {
        ڊيگهائي = حجم(cpu_info);
        src_vir = (vir_بائيٽون) &cpu_info;
        اكسر;
    }
    حالة GET_HZ: {
        ڊيگهائي = حجم(system_hz);
        src_vir = (vir_بائيٽون) &system_hz;
        اكسر;
    }
    حالة GET_IMAGE: {
        ڊيگهائي = حجم(بنية boot_image) * NR_BOOT_PROCS;
        src_vir = (vir_بائيٽون) image;
        اكسر;
    }
    حالة GET_IRQHOOKS: {
        ڊيگهائي = حجم(بنية irq_hook) * NR_IRQ_HOOKS;
        src_vir = (vir_بائيٽون) irq_hooks;
        اكسر;
    }
    حالة GET_PROCTAB: {
	update_idle_وقت();
        ڊيگهائي = حجم(بنية proc) * (NR_PROCS + NR_TASKS);
        src_vir = (vir_بائيٽون) proc;
        اكسر;
    }
    حالة GET_PRIVTAB: {
        ڊيگهائي = حجم(بنية priv) * (NR_SYS_PROCS);
        src_vir = (vir_بائيٽون) priv;
        اكسر;
    }
    حالة GET_PROC: {
        nr_e = (m_ptr->m_lsys_krn_sys_getinfo.val_len2_e == SELF) ?
		caller->p_endpoint : m_ptr->m_lsys_krn_sys_getinfo.val_len2_e;
	إذا(!isokendpt(nr_e, &nr)) أرجع EINVAL; /* validate request */
        ڊيگهائي = حجم(بنية proc);
        src_vir = (vir_بائيٽون) proc_addr(nr);
        اكسر;
    }
    حالة GET_PRIV: {
        nr_e = (m_ptr->m_lsys_krn_sys_getinfo.val_len2_e == SELF) ?
            caller->p_endpoint : m_ptr->m_lsys_krn_sys_getinfo.val_len2_e;
        إذا(!isokendpt(nr_e, &nr)) أرجع EINVAL; /* validate request */
        ڊيگهائي = حجم(بنية priv);
        src_vir = (vir_بائيٽون) priv_addr(nr_منزل_id(nr));
        اكسر;
    }
    حالة GET_REGS: {
        nr_e = (m_ptr->m_lsys_krn_sys_getinfo.val_len2_e == SELF) ?
            caller->p_endpoint : m_ptr->m_lsys_krn_sys_getinfo.val_len2_e;
        إذا(!isokendpt(nr_e, &nr)) أرجع EINVAL; /* validate request */
        p = proc_addr(nr);
        ڊيگهائي = حجم(p->p_reg);
        src_vir = (vir_بائيٽون) &p->p_reg;
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
	أرجع OK;
    }
    حالة GET_MONPARAMS: {
        src_vir = (vir_بائيٽون) kinfo.param_buf;
	ڊيگهائي = حجم(kinfo.param_buf);
        اكسر;
    }
    حالة GET_RANDOMNESS: {		
        ساكن بنية k_randomness copy;	/* copy to keep counters */
	صحيح i;

        copy = krandom;
        لكل (i= 0; i<RANDOM_SOURCES; i++) {
  		krandom.bin[i].r_سائيز = 0;	/* invalidate random data */
  		krandom.bin[i].r_ايندڙ = 0;
	}
    	ڊيگهائي = حجم(copy);
    	src_vir = (vir_بائيٽون) &copy;
    	اكسر;
    }
    حالة GET_RANDOMNESS_BIN: {		
	صحيح bin = m_ptr->m_lsys_krn_sys_getinfo.val_len2_e;

	إذا(bin < 0 || bin >= RANDOM_SOURCES) {
		لکو("SYSTEM: GET_RANDOMNESS_BIN: %d out of range\n", bin);
		أرجع EINVAL;
	}

	إذا(krandom.bin[bin].r_سائيز < RANDOM_ELEMENTS)
		أرجع ENOENT;

    	ڊيگهائي = حجم(krandom.bin[bin]);
    	src_vir = (vir_بائيٽون) &krandom.bin[bin];

	wipe_rnd_bin = bin;

    	اكسر;
    }
    حالة GET_IRQACTIDS: {
        ڊيگهائي = حجم(irq_actids);
        src_vir = (vir_بائيٽون) irq_actids;
        اكسر;
    }
    حالة GET_IDLETSC: {
	بنية proc * idl;
	update_idle_وقت();
	idl = proc_addr(IDLE);
        ڊيگهائي = حجم(idl->p_چڪرن);
        src_vir = (vir_بائيٽون) &idl->p_چڪرن;
        اكسر;
    }
    حالة GET_CPUTICKS: {
	uint64_t ticks[MINIX_CPUSTATES];
	بنانشان صحيح cpu;
	cpu = (بنانشان صحيح)m_ptr->m_lsys_krn_sys_getinfo.val_len2_e;
	إذا (cpu >= CONFIG_MAX_CPUS)
		أرجع EINVAL;
	get_cpu_ticks(cpu, ticks);
	ڊيگهائي = حجم(ticks);
	src_vir = (vir_بائيٽون)ticks;
	اكسر;
    }
    افتراضي:
	لکو("do_getinfo: invalid request %d\n",
		m_ptr->m_lsys_krn_sys_getinfo.request);
        أرجع(EINVAL);
  }

  /* Try to make the actual copy for the requested data. */
  إذا (m_ptr->m_lsys_krn_sys_getinfo.val_len > 0 &&
	ڊيگهائي > m_ptr->m_lsys_krn_sys_getinfo.val_len)
	أرجع (E2BIG);

  r = معلومات_copy_vmcheck(caller, KERNEL, src_vir, caller->p_endpoint,
	m_ptr->m_lsys_krn_sys_getinfo.val_ptr, ڊيگهائي);

  إذا(r != OK) أرجع r;

	إذا(wipe_rnd_bin >= 0 && wipe_rnd_bin < RANDOM_SOURCES) {
		krandom.bin[wipe_rnd_bin].r_سائيز = 0;
		krandom.bin[wipe_rnd_bin].r_ايندڙ = 0;
	}

  أرجع(OK);
}

#جيڪڏهن_پورو /* USE_GETINFO */

