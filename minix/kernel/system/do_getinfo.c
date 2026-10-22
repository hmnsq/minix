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

#अन्तर्भाव <string.h>

#अन्तर्भाव "kernel/system.h"


#जर USE_GETINFO

#अन्तर्भाव <minix/u64.h>
#अन्तर्भाव <sys/resource.h>

/*===========================================================================*
 *			        update_idle_time			     *
 *===========================================================================*/
स्थिर शून्य update_idle_time(शून्य)
{
	पूर्णांक i;
	संरचना proc * idl = proc_addr(IDLE);

	idl->p_cycles = make64(0, 0);

	साठी (i = 0; i < CONFIG_MAX_CPUS ; i++) {
		idl->p_cycles += get_cpu_var(i, idle_proc).p_cycles;
	}
}

/*===========================================================================*
 *			        do_getinfo				     *
 *===========================================================================*/
पूर्णांक do_getinfo(संरचना proc * caller, message * m_ptr)
{
/* Request system information to be copied to caller's address space. This
 * call simply copies entire data structures to the caller.
 */
  माप_प्रकार length;
  vir_bytes src_vir; 
  पूर्णांक nr_e, nr, r;
  पूर्णांक wipe_rnd_bin = -1;
  संरचना proc *p;
  संरचना rusage r_usage;

  /* Set source address and length based on request type. */
  निवड (m_ptr->m_lsys_krn_sys_getinfo.request) {
    स्थिती GET_MACHINE: {
        length = आकार(संरचना machine);
        src_vir = (vir_bytes) &machine;
        थांबा;
    }
    स्थिती GET_KINFO: {
        length = आकार(संरचना kinfo);
        src_vir = (vir_bytes) &kinfo;
        थांबा;
    }
    स्थिती GET_LOADINFO: {
        length = आकार(संरचना loadinfo);
        src_vir = (vir_bytes) &kloadinfo;
        थांबा;
    }
    स्थिती GET_CPUINFO: {
        length = आकार(cpu_info);
        src_vir = (vir_bytes) &cpu_info;
        थांबा;
    }
    स्थिती GET_HZ: {
        length = आकार(system_hz);
        src_vir = (vir_bytes) &system_hz;
        थांबा;
    }
    स्थिती GET_IMAGE: {
        length = आकार(संरचना boot_image) * NR_BOOT_PROCS;
        src_vir = (vir_bytes) image;
        थांबा;
    }
    स्थिती GET_IRQHOOKS: {
        length = आकार(संरचना irq_hook) * NR_IRQ_HOOKS;
        src_vir = (vir_bytes) irq_hooks;
        थांबा;
    }
    स्थिती GET_PROCTAB: {
	update_idle_time();
        length = आकार(संरचना proc) * (NR_PROCS + NR_TASKS);
        src_vir = (vir_bytes) proc;
        थांबा;
    }
    स्थिती GET_PRIVTAB: {
        length = आकार(संरचना priv) * (NR_SYS_PROCS);
        src_vir = (vir_bytes) priv;
        थांबा;
    }
    स्थिती GET_PROC: {
        nr_e = (m_ptr->m_lsys_krn_sys_getinfo.val_len2_e == SELF) ?
		caller->p_endpoint : m_ptr->m_lsys_krn_sys_getinfo.val_len2_e;
	जर(!isokendpt(nr_e, &nr)) परत EINVAL; /* validate request */
        length = आकार(संरचना proc);
        src_vir = (vir_bytes) proc_addr(nr);
        थांबा;
    }
    स्थिती GET_PRIV: {
        nr_e = (m_ptr->m_lsys_krn_sys_getinfo.val_len2_e == SELF) ?
            caller->p_endpoint : m_ptr->m_lsys_krn_sys_getinfo.val_len2_e;
        जर(!isokendpt(nr_e, &nr)) परत EINVAL; /* validate request */
        length = आकार(संरचना priv);
        src_vir = (vir_bytes) priv_addr(nr_to_id(nr));
        थांबा;
    }
    स्थिती GET_REGS: {
        nr_e = (m_ptr->m_lsys_krn_sys_getinfo.val_len2_e == SELF) ?
            caller->p_endpoint : m_ptr->m_lsys_krn_sys_getinfo.val_len2_e;
        जर(!isokendpt(nr_e, &nr)) परत EINVAL; /* validate request */
        p = proc_addr(nr);
        length = आकार(p->p_reg);
        src_vir = (vir_bytes) &p->p_reg;
        थांबा;
    }
    स्थिती GET_WHOAMI: {
	पूर्णांक len;
	m_ptr->m_krn_lsys_sys_getwhoami.endpt = caller->p_endpoint;
	len = MIN(आकार(m_ptr->m_krn_lsys_sys_getwhoami.name),
		आकार(caller->p_name))-1;
	म_नकलन(m_ptr->m_krn_lsys_sys_getwhoami.name, caller->p_name, len);
	m_ptr->m_krn_lsys_sys_getwhoami.name[len] = '\0';
	m_ptr->m_krn_lsys_sys_getwhoami.privflags = priv(caller)->s_flags;
        m_ptr->m_krn_lsys_sys_getwhoami.initflags = priv(caller)->s_init_flags;
	परत OK;
    }
    स्थिती GET_MONPARAMS: {
        src_vir = (vir_bytes) kinfo.param_buf;
	length = आकार(kinfo.param_buf);
        थांबा;
    }
    स्थिती GET_RANDOMNESS: {		
        स्थिर संरचना k_randomness copy;	/* copy to keep counters */
	पूर्णांक i;

        copy = krandom;
        साठी (i= 0; i<RANDOM_SOURCES; i++) {
  		krandom.bin[i].r_size = 0;	/* invalidate random data */
  		krandom.bin[i].r_next = 0;
	}
    	length = आकार(copy);
    	src_vir = (vir_bytes) &copy;
    	थांबा;
    }
    स्थिती GET_RANDOMNESS_BIN: {		
	पूर्णांक bin = m_ptr->m_lsys_krn_sys_getinfo.val_len2_e;

	जर(bin < 0 || bin >= RANDOM_SOURCES) {
		म_लिखो("SYSTEM: GET_RANDOMNESS_BIN: %d out of range\n", bin);
		परत EINVAL;
	}

	जर(krandom.bin[bin].r_size < RANDOM_ELEMENTS)
		परत ENOENT;

    	length = आकार(krandom.bin[bin]);
    	src_vir = (vir_bytes) &krandom.bin[bin];

	wipe_rnd_bin = bin;

    	थांबा;
    }
    स्थिती GET_IRQACTIDS: {
        length = आकार(irq_actids);
        src_vir = (vir_bytes) irq_actids;
        थांबा;
    }
    स्थिती GET_IDLETSC: {
	संरचना proc * idl;
	update_idle_time();
	idl = proc_addr(IDLE);
        length = आकार(idl->p_cycles);
        src_vir = (vir_bytes) &idl->p_cycles;
        थांबा;
    }
    स्थिती GET_CPUTICKS: {
	uint64_t ticks[MINIX_CPUSTATES];
	अचिन्हित पूर्णांक cpu;
	cpu = (अचिन्हित पूर्णांक)m_ptr->m_lsys_krn_sys_getinfo.val_len2_e;
	जर (cpu >= CONFIG_MAX_CPUS)
		परत EINVAL;
	get_cpu_ticks(cpu, ticks);
	length = आकार(ticks);
	src_vir = (vir_bytes)ticks;
	थांबा;
    }
    पूर्वनिर्धारित:
	म_लिखो("do_getinfo: invalid request %d\n",
		m_ptr->m_lsys_krn_sys_getinfo.request);
        परत(EINVAL);
  }

  /* Try to make the actual copy for the requested data. */
  जर (m_ptr->m_lsys_krn_sys_getinfo.val_len > 0 &&
	length > m_ptr->m_lsys_krn_sys_getinfo.val_len)
	परत (E2BIG);

  r = data_copy_vmcheck(caller, KERNEL, src_vir, caller->p_endpoint,
	m_ptr->m_lsys_krn_sys_getinfo.val_ptr, length);

  जर(r != OK) परत r;

	जर(wipe_rnd_bin >= 0 && wipe_rnd_bin < RANDOM_SOURCES) {
		krandom.bin[wipe_rnd_bin].r_size = 0;
		krandom.bin[wipe_rnd_bin].r_next = 0;
	}

  परत(OK);
}

#endif /* USE_GETINFO */

