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

#અન્તર્ભાવ <string.h>

#અન્તર્ભાવ "kernel/system.h"


#જો USE_GETINFO

#અન્તર્ભાવ <minix/u64.h>
#અન્તર્ભાવ <sys/resource.h>

/*===========================================================================*
 *			        update_idle_time			     *
 *===========================================================================*/
સ્થિર શૂન્ય update_idle_time(શૂન્ય)
{
	પૂર્ણાંક i;
	સંરચના proc * idl = proc_addr(IDLE);

	idl->p_cycles = make64(0, 0);

	માટે (i = 0; i < CONFIG_MAX_CPUS ; i++) {
		idl->p_cycles += get_cpu_var(i, idle_proc).p_cycles;
	}
}

/*===========================================================================*
 *			        do_getinfo				     *
 *===========================================================================*/
પૂર્ણાંક do_getinfo(સંરચના proc * caller, message * m_ptr)
{
/* Request system information to be copied to caller's address space. This
 * call simply copies entire data structures to the caller.
 */
  size_t length;
  vir_bytes src_vir; 
  પૂર્ણાંક nr_e, nr, r;
  પૂર્ણાંક wipe_rnd_bin = -1;
  સંરચના proc *p;
  સંરચના rusage r_usage;

  /* Set source address and length based on request type. */
  પસંદગી (m_ptr->m_lsys_krn_sys_getinfo.request) {
    સ્થિતિ GET_MACHINE: {
        length = કદ(સંરચના machine);
        src_vir = (vir_bytes) &machine;
        તોડો;
    }
    સ્થિતિ GET_KINFO: {
        length = કદ(સંરચના kinfo);
        src_vir = (vir_bytes) &kinfo;
        તોડો;
    }
    સ્થિતિ GET_LOADINFO: {
        length = કદ(સંરચના loadinfo);
        src_vir = (vir_bytes) &kloadinfo;
        તોડો;
    }
    સ્થિતિ GET_CPUINFO: {
        length = કદ(cpu_info);
        src_vir = (vir_bytes) &cpu_info;
        તોડો;
    }
    સ્થિતિ GET_HZ: {
        length = કદ(system_hz);
        src_vir = (vir_bytes) &system_hz;
        તોડો;
    }
    સ્થિતિ GET_IMAGE: {
        length = કદ(સંરચના boot_image) * NR_BOOT_PROCS;
        src_vir = (vir_bytes) image;
        તોડો;
    }
    સ્થિતિ GET_IRQHOOKS: {
        length = કદ(સંરચના irq_hook) * NR_IRQ_HOOKS;
        src_vir = (vir_bytes) irq_hooks;
        તોડો;
    }
    સ્થિતિ GET_PROCTAB: {
	update_idle_time();
        length = કદ(સંરચના proc) * (NR_PROCS + NR_TASKS);
        src_vir = (vir_bytes) proc;
        તોડો;
    }
    સ્થિતિ GET_PRIVTAB: {
        length = કદ(સંરચના priv) * (NR_SYS_PROCS);
        src_vir = (vir_bytes) priv;
        તોડો;
    }
    સ્થિતિ GET_PROC: {
        nr_e = (m_ptr->m_lsys_krn_sys_getinfo.val_len2_e == SELF) ?
		caller->p_endpoint : m_ptr->m_lsys_krn_sys_getinfo.val_len2_e;
	જો(!isokendpt(nr_e, &nr)) પાછા EINVAL; /* validate request */
        length = કદ(સંરચના proc);
        src_vir = (vir_bytes) proc_addr(nr);
        તોડો;
    }
    સ્થિતિ GET_PRIV: {
        nr_e = (m_ptr->m_lsys_krn_sys_getinfo.val_len2_e == SELF) ?
            caller->p_endpoint : m_ptr->m_lsys_krn_sys_getinfo.val_len2_e;
        જો(!isokendpt(nr_e, &nr)) પાછા EINVAL; /* validate request */
        length = કદ(સંરચના priv);
        src_vir = (vir_bytes) priv_addr(nr_to_id(nr));
        તોડો;
    }
    સ્થિતિ GET_REGS: {
        nr_e = (m_ptr->m_lsys_krn_sys_getinfo.val_len2_e == SELF) ?
            caller->p_endpoint : m_ptr->m_lsys_krn_sys_getinfo.val_len2_e;
        જો(!isokendpt(nr_e, &nr)) પાછા EINVAL; /* validate request */
        p = proc_addr(nr);
        length = કદ(p->p_reg);
        src_vir = (vir_bytes) &p->p_reg;
        તોડો;
    }
    સ્થિતિ GET_WHOAMI: {
	પૂર્ણાંક len;
	m_ptr->m_krn_lsys_sys_getwhoami.endpt = caller->p_endpoint;
	len = MIN(કદ(m_ptr->m_krn_lsys_sys_getwhoami.name),
		કદ(caller->p_name))-1;
	strncpy(m_ptr->m_krn_lsys_sys_getwhoami.name, caller->p_name, len);
	m_ptr->m_krn_lsys_sys_getwhoami.name[len] = '\0';
	m_ptr->m_krn_lsys_sys_getwhoami.privflags = priv(caller)->s_flags;
        m_ptr->m_krn_lsys_sys_getwhoami.initflags = priv(caller)->s_init_flags;
	પાછા OK;
    }
    સ્થિતિ GET_MONPARAMS: {
        src_vir = (vir_bytes) kinfo.param_buf;
	length = કદ(kinfo.param_buf);
        તોડો;
    }
    સ્થિતિ GET_RANDOMNESS: {		
        સ્થિર સંરચના k_randomness copy;	/* copy to keep counters */
	પૂર્ણાંક i;

        copy = krandom;
        માટે (i= 0; i<RANDOM_SOURCES; i++) {
  		krandom.bin[i].r_size = 0;	/* invalidate random data */
  		krandom.bin[i].r_next = 0;
	}
    	length = કદ(copy);
    	src_vir = (vir_bytes) &copy;
    	તોડો;
    }
    સ્થિતિ GET_RANDOMNESS_BIN: {		
	પૂર્ણાંક bin = m_ptr->m_lsys_krn_sys_getinfo.val_len2_e;

	જો(bin < 0 || bin >= RANDOM_SOURCES) {
		printf("SYSTEM: GET_RANDOMNESS_BIN: %d out of range\n", bin);
		પાછા EINVAL;
	}

	જો(krandom.bin[bin].r_size < RANDOM_ELEMENTS)
		પાછા ENOENT;

    	length = કદ(krandom.bin[bin]);
    	src_vir = (vir_bytes) &krandom.bin[bin];

	wipe_rnd_bin = bin;

    	તોડો;
    }
    સ્થિતિ GET_IRQACTIDS: {
        length = કદ(irq_actids);
        src_vir = (vir_bytes) irq_actids;
        તોડો;
    }
    સ્થિતિ GET_IDLETSC: {
	સંરચના proc * idl;
	update_idle_time();
	idl = proc_addr(IDLE);
        length = કદ(idl->p_cycles);
        src_vir = (vir_bytes) &idl->p_cycles;
        તોડો;
    }
    સ્થિતિ GET_CPUTICKS: {
	uint64_t ticks[MINIX_CPUSTATES];
	unsigned પૂર્ણાંક cpu;
	cpu = (unsigned પૂર્ણાંક)m_ptr->m_lsys_krn_sys_getinfo.val_len2_e;
	જો (cpu >= CONFIG_MAX_CPUS)
		પાછા EINVAL;
	get_cpu_ticks(cpu, ticks);
	length = કદ(ticks);
	src_vir = (vir_bytes)ticks;
	તોડો;
    }
    મૂળભૂત:
	printf("do_getinfo: invalid request %d\n",
		m_ptr->m_lsys_krn_sys_getinfo.request);
        પાછા(EINVAL);
  }

  /* Try to make the actual copy for the requested data. */
  જો (m_ptr->m_lsys_krn_sys_getinfo.val_len > 0 &&
	length > m_ptr->m_lsys_krn_sys_getinfo.val_len)
	પાછા (E2BIG);

  r = data_copy_vmcheck(caller, KERNEL, src_vir, caller->p_endpoint,
	m_ptr->m_lsys_krn_sys_getinfo.val_ptr, length);

  જો(r != OK) પાછા r;

	જો(wipe_rnd_bin >= 0 && wipe_rnd_bin < RANDOM_SOURCES) {
		krandom.bin[wipe_rnd_bin].r_size = 0;
		krandom.bin[wipe_rnd_bin].r_next = 0;
	}

  પાછા(OK);
}

#endif /* USE_GETINFO */

