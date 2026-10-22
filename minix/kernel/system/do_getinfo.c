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


#यदि USE_GETINFO

#अन्तर्भाव <minix/u64.h>
#अन्तर्भाव <sys/resource.h>

/*===========================================================================*
 *			        update_idle_time			     *
 *===========================================================================*/
स्थिर शून्य update_idle_time(शून्य)
{
	पूर्णाङ्क i;
	संरचना proc * idl = proc_addr(IDLE);

	idl->p_cycles = make64(0, 0);

	प्रति (i = 0; i < CONFIG_MAX_CPUS ; i++) {
		idl->p_cycles += get_cpu_var(i, idle_proc).p_cycles;
	}
}

/*===========================================================================*
 *			        do_getinfo				     *
 *===========================================================================*/
पूर्णाङ्क do_getinfo(संरचना proc * caller, message * m_ptr)
{
/* Request system information to be copied to caller's address space. This
 * call simply copies entire data structures to the caller.
 */
  size_t length;
  vir_bytes src_vir; 
  पूर्णाङ्क nr_e, nr, r;
  पूर्णाङ्क wipe_rnd_bin = -1;
  संरचना proc *p;
  संरचना rusage r_usage;

  /* Set source address and length based on request type. */
  विकल्प (m_ptr->m_lsys_krn_sys_getinfo.request) {
    पक्ष GET_MACHINE: {
        length = परिमाण(संरचना machine);
        src_vir = (vir_bytes) &machine;
        भङ्ग;
    }
    पक्ष GET_KINFO: {
        length = परिमाण(संरचना kinfo);
        src_vir = (vir_bytes) &kinfo;
        भङ्ग;
    }
    पक्ष GET_LOADINFO: {
        length = परिमाण(संरचना loadinfo);
        src_vir = (vir_bytes) &kloadinfo;
        भङ्ग;
    }
    पक्ष GET_CPUINFO: {
        length = परिमाण(cpu_info);
        src_vir = (vir_bytes) &cpu_info;
        भङ्ग;
    }
    पक्ष GET_HZ: {
        length = परिमाण(system_hz);
        src_vir = (vir_bytes) &system_hz;
        भङ्ग;
    }
    पक्ष GET_IMAGE: {
        length = परिमाण(संरचना boot_image) * NR_BOOT_PROCS;
        src_vir = (vir_bytes) image;
        भङ्ग;
    }
    पक्ष GET_IRQHOOKS: {
        length = परिमाण(संरचना irq_hook) * NR_IRQ_HOOKS;
        src_vir = (vir_bytes) irq_hooks;
        भङ्ग;
    }
    पक्ष GET_PROCTAB: {
	update_idle_time();
        length = परिमाण(संरचना proc) * (NR_PROCS + NR_TASKS);
        src_vir = (vir_bytes) proc;
        भङ्ग;
    }
    पक्ष GET_PRIVTAB: {
        length = परिमाण(संरचना priv) * (NR_SYS_PROCS);
        src_vir = (vir_bytes) priv;
        भङ्ग;
    }
    पक्ष GET_PROC: {
        nr_e = (m_ptr->m_lsys_krn_sys_getinfo.val_len2_e == SELF) ?
		caller->p_endpoint : m_ptr->m_lsys_krn_sys_getinfo.val_len2_e;
	यदि(!isokendpt(nr_e, &nr)) प्रत्यागम EINVAL; /* validate request */
        length = परिमाण(संरचना proc);
        src_vir = (vir_bytes) proc_addr(nr);
        भङ्ग;
    }
    पक्ष GET_PRIV: {
        nr_e = (m_ptr->m_lsys_krn_sys_getinfo.val_len2_e == SELF) ?
            caller->p_endpoint : m_ptr->m_lsys_krn_sys_getinfo.val_len2_e;
        यदि(!isokendpt(nr_e, &nr)) प्रत्यागम EINVAL; /* validate request */
        length = परिमाण(संरचना priv);
        src_vir = (vir_bytes) priv_addr(nr_to_id(nr));
        भङ्ग;
    }
    पक्ष GET_REGS: {
        nr_e = (m_ptr->m_lsys_krn_sys_getinfo.val_len2_e == SELF) ?
            caller->p_endpoint : m_ptr->m_lsys_krn_sys_getinfo.val_len2_e;
        यदि(!isokendpt(nr_e, &nr)) प्रत्यागम EINVAL; /* validate request */
        p = proc_addr(nr);
        length = परिमाण(p->p_reg);
        src_vir = (vir_bytes) &p->p_reg;
        भङ्ग;
    }
    पक्ष GET_WHOAMI: {
	पूर्णाङ्क len;
	m_ptr->m_krn_lsys_sys_getwhoami.endpt = caller->p_endpoint;
	len = MIN(परिमाण(m_ptr->m_krn_lsys_sys_getwhoami.name),
		परिमाण(caller->p_name))-1;
	strncpy(m_ptr->m_krn_lsys_sys_getwhoami.name, caller->p_name, len);
	m_ptr->m_krn_lsys_sys_getwhoami.name[len] = '\0';
	m_ptr->m_krn_lsys_sys_getwhoami.privflags = priv(caller)->s_flags;
        m_ptr->m_krn_lsys_sys_getwhoami.initflags = priv(caller)->s_init_flags;
	प्रत्यागम OK;
    }
    पक्ष GET_MONPARAMS: {
        src_vir = (vir_bytes) kinfo.param_buf;
	length = परिमाण(kinfo.param_buf);
        भङ्ग;
    }
    पक्ष GET_RANDOMNESS: {		
        स्थिर संरचना k_randomness copy;	/* copy to keep counters */
	पूर्णाङ्क i;

        copy = krandom;
        प्रति (i= 0; i<RANDOM_SOURCES; i++) {
  		krandom.bin[i].r_size = 0;	/* invalidate random data */
  		krandom.bin[i].r_next = 0;
	}
    	length = परिमाण(copy);
    	src_vir = (vir_bytes) &copy;
    	भङ्ग;
    }
    पक्ष GET_RANDOMNESS_BIN: {		
	पूर्णाङ्क bin = m_ptr->m_lsys_krn_sys_getinfo.val_len2_e;

	यदि(bin < 0 || bin >= RANDOM_SOURCES) {
		printf("SYSTEM: GET_RANDOMNESS_BIN: %d out of range\n", bin);
		प्रत्यागम EINVAL;
	}

	यदि(krandom.bin[bin].r_size < RANDOM_ELEMENTS)
		प्रत्यागम ENOENT;

    	length = परिमाण(krandom.bin[bin]);
    	src_vir = (vir_bytes) &krandom.bin[bin];

	wipe_rnd_bin = bin;

    	भङ्ग;
    }
    पक्ष GET_IRQACTIDS: {
        length = परिमाण(irq_actids);
        src_vir = (vir_bytes) irq_actids;
        भङ्ग;
    }
    पक्ष GET_IDLETSC: {
	संरचना proc * idl;
	update_idle_time();
	idl = proc_addr(IDLE);
        length = परिमाण(idl->p_cycles);
        src_vir = (vir_bytes) &idl->p_cycles;
        भङ्ग;
    }
    पक्ष GET_CPUTICKS: {
	uint64_t ticks[MINIX_CPUSTATES];
	unsigned पूर्णाङ्क cpu;
	cpu = (unsigned पूर्णाङ्क)m_ptr->m_lsys_krn_sys_getinfo.val_len2_e;
	यदि (cpu >= CONFIG_MAX_CPUS)
		प्रत्यागम EINVAL;
	get_cpu_ticks(cpu, ticks);
	length = परिमाण(ticks);
	src_vir = (vir_bytes)ticks;
	भङ्ग;
    }
    स्वभाव:
	printf("do_getinfo: invalid request %d\n",
		m_ptr->m_lsys_krn_sys_getinfo.request);
        प्रत्यागम(EINVAL);
  }

  /* Try to make the actual copy for the requested data. */
  यदि (m_ptr->m_lsys_krn_sys_getinfo.val_len > 0 &&
	length > m_ptr->m_lsys_krn_sys_getinfo.val_len)
	प्रत्यागम (E2BIG);

  r = data_copy_vmcheck(caller, KERNEL, src_vir, caller->p_endpoint,
	m_ptr->m_lsys_krn_sys_getinfo.val_ptr, length);

  यदि(r != OK) प्रत्यागम r;

	यदि(wipe_rnd_bin >= 0 && wipe_rnd_bin < RANDOM_SOURCES) {
		krandom.bin[wipe_rnd_bin].r_size = 0;
		krandom.bin[wipe_rnd_bin].r_next = 0;
	}

  प्रत्यागम(OK);
}

#endif /* USE_GETINFO */

