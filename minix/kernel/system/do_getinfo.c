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

#ଅନ୍ତର୍ଭାଵ <string.h>

#ଅନ୍ତର୍ଭାଵ "kernel/system.h"


#ଯଦି USE_GETINFO

#ଅନ୍ତର୍ଭାଵ <minix/u64.h>
#ଅନ୍ତର୍ଭାଵ <sys/resource.h>

/*===========================================================================*
 *			        update_idle_time			     *
 *===========================================================================*/
ସ୍ଥିର ଶୂନ୍ୟ update_idle_time(ଶୂନ୍ୟ)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
	ସଂରଚନା proc * idl = proc_addr(IDLE);

	idl->p_cycles = make64(0, 0);

	ପାଇଁ (i = 0; i < CONFIG_MAX_CPUS ; i++) {
		idl->p_cycles += get_cpu_var(i, idle_proc).p_cycles;
	}
}

/*===========================================================================*
 *			        do_getinfo				     *
 *===========================================================================*/
ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_getinfo(ସଂରଚନା proc * caller, message * m_ptr)
{
/* Request system information to be copied to caller's address space. This
 * call simply copies entire data structures to the caller.
 */
  size_t length;
  vir_bytes src_vir; 
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା nr_e, nr, r;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା wipe_rnd_bin = -1;
  ସଂରଚନା proc *p;
  ସଂରଚନା rusage r_usage;

  /* Set source address and length based on request type. */
  ବାଛ (m_ptr->m_lsys_krn_sys_getinfo.request) {
    ସ୍ଥିତି GET_MACHINE: {
        length = ଆକାର(ସଂରଚନା machine);
        src_vir = (vir_bytes) &machine;
        ଭାଙ୍ଗ;
    }
    ସ୍ଥିତି GET_KINFO: {
        length = ଆକାର(ସଂରଚନା kinfo);
        src_vir = (vir_bytes) &kinfo;
        ଭାଙ୍ଗ;
    }
    ସ୍ଥିତି GET_LOADINFO: {
        length = ଆକାର(ସଂରଚନା loadinfo);
        src_vir = (vir_bytes) &kloadinfo;
        ଭାଙ୍ଗ;
    }
    ସ୍ଥିତି GET_CPUINFO: {
        length = ଆକାର(cpu_info);
        src_vir = (vir_bytes) &cpu_info;
        ଭାଙ୍ଗ;
    }
    ସ୍ଥିତି GET_HZ: {
        length = ଆକାର(system_hz);
        src_vir = (vir_bytes) &system_hz;
        ଭାଙ୍ଗ;
    }
    ସ୍ଥିତି GET_IMAGE: {
        length = ଆକାର(ସଂରଚନା boot_image) * NR_BOOT_PROCS;
        src_vir = (vir_bytes) image;
        ଭାଙ୍ଗ;
    }
    ସ୍ଥିତି GET_IRQHOOKS: {
        length = ଆକାର(ସଂରଚନା irq_hook) * NR_IRQ_HOOKS;
        src_vir = (vir_bytes) irq_hooks;
        ଭାଙ୍ଗ;
    }
    ସ୍ଥିତି GET_PROCTAB: {
	update_idle_time();
        length = ଆକାର(ସଂରଚନା proc) * (NR_PROCS + NR_TASKS);
        src_vir = (vir_bytes) proc;
        ଭାଙ୍ଗ;
    }
    ସ୍ଥିତି GET_PRIVTAB: {
        length = ଆକାର(ସଂରଚନା priv) * (NR_SYS_PROCS);
        src_vir = (vir_bytes) priv;
        ଭାଙ୍ଗ;
    }
    ସ୍ଥିତି GET_PROC: {
        nr_e = (m_ptr->m_lsys_krn_sys_getinfo.val_len2_e == SELF) ?
		caller->p_endpoint : m_ptr->m_lsys_krn_sys_getinfo.val_len2_e;
	ଯଦି(!isokendpt(nr_e, &nr)) ଫେରନ୍ତୁ EINVAL; /* validate request */
        length = ଆକାର(ସଂରଚନା proc);
        src_vir = (vir_bytes) proc_addr(nr);
        ଭାଙ୍ଗ;
    }
    ସ୍ଥିତି GET_PRIV: {
        nr_e = (m_ptr->m_lsys_krn_sys_getinfo.val_len2_e == SELF) ?
            caller->p_endpoint : m_ptr->m_lsys_krn_sys_getinfo.val_len2_e;
        ଯଦି(!isokendpt(nr_e, &nr)) ଫେରନ୍ତୁ EINVAL; /* validate request */
        length = ଆକାର(ସଂରଚନା priv);
        src_vir = (vir_bytes) priv_addr(nr_to_id(nr));
        ଭାଙ୍ଗ;
    }
    ସ୍ଥିତି GET_REGS: {
        nr_e = (m_ptr->m_lsys_krn_sys_getinfo.val_len2_e == SELF) ?
            caller->p_endpoint : m_ptr->m_lsys_krn_sys_getinfo.val_len2_e;
        ଯଦି(!isokendpt(nr_e, &nr)) ଫେରନ୍ତୁ EINVAL; /* validate request */
        p = proc_addr(nr);
        length = ଆକାର(p->p_reg);
        src_vir = (vir_bytes) &p->p_reg;
        ଭାଙ୍ଗ;
    }
    ସ୍ଥିତି GET_WHOAMI: {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା len;
	m_ptr->m_krn_lsys_sys_getwhoami.endpt = caller->p_endpoint;
	len = MIN(ଆକାର(m_ptr->m_krn_lsys_sys_getwhoami.name),
		ଆକାର(caller->p_name))-1;
	strncpy(m_ptr->m_krn_lsys_sys_getwhoami.name, caller->p_name, len);
	m_ptr->m_krn_lsys_sys_getwhoami.name[len] = '\0';
	m_ptr->m_krn_lsys_sys_getwhoami.privflags = priv(caller)->s_flags;
        m_ptr->m_krn_lsys_sys_getwhoami.initflags = priv(caller)->s_init_flags;
	ଫେରନ୍ତୁ OK;
    }
    ସ୍ଥିତି GET_MONPARAMS: {
        src_vir = (vir_bytes) kinfo.param_buf;
	length = ଆକାର(kinfo.param_buf);
        ଭାଙ୍ଗ;
    }
    ସ୍ଥିତି GET_RANDOMNESS: {		
        ସ୍ଥିର ସଂରଚନା k_randomness copy;	/* copy to keep counters */
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

        copy = krandom;
        ପାଇଁ (i= 0; i<RANDOM_SOURCES; i++) {
  		krandom.bin[i].r_size = 0;	/* invalidate random data */
  		krandom.bin[i].r_next = 0;
	}
    	length = ଆକାର(copy);
    	src_vir = (vir_bytes) &copy;
    	ଭାଙ୍ଗ;
    }
    ସ୍ଥିତି GET_RANDOMNESS_BIN: {		
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା bin = m_ptr->m_lsys_krn_sys_getinfo.val_len2_e;

	ଯଦି(bin < 0 || bin >= RANDOM_SOURCES) {
		printf("SYSTEM: GET_RANDOMNESS_BIN: %d out of range\n", bin);
		ଫେରନ୍ତୁ EINVAL;
	}

	ଯଦି(krandom.bin[bin].r_size < RANDOM_ELEMENTS)
		ଫେରନ୍ତୁ ENOENT;

    	length = ଆକାର(krandom.bin[bin]);
    	src_vir = (vir_bytes) &krandom.bin[bin];

	wipe_rnd_bin = bin;

    	ଭାଙ୍ଗ;
    }
    ସ୍ଥିତି GET_IRQACTIDS: {
        length = ଆକାର(irq_actids);
        src_vir = (vir_bytes) irq_actids;
        ଭାଙ୍ଗ;
    }
    ସ୍ଥିତି GET_IDLETSC: {
	ସଂରଚନା proc * idl;
	update_idle_time();
	idl = proc_addr(IDLE);
        length = ଆକାର(idl->p_cycles);
        src_vir = (vir_bytes) &idl->p_cycles;
        ଭାଙ୍ଗ;
    }
    ସ୍ଥିତି GET_CPUTICKS: {
	uint64_t ticks[MINIX_CPUSTATES];
	unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା cpu;
	cpu = (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)m_ptr->m_lsys_krn_sys_getinfo.val_len2_e;
	ଯଦି (cpu >= CONFIG_MAX_CPUS)
		ଫେରନ୍ତୁ EINVAL;
	get_cpu_ticks(cpu, ticks);
	length = ଆକାର(ticks);
	src_vir = (vir_bytes)ticks;
	ଭାଙ୍ଗ;
    }
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	printf("do_getinfo: invalid request %d\n",
		m_ptr->m_lsys_krn_sys_getinfo.request);
        ଫେରନ୍ତୁ(EINVAL);
  }

  /* Try to make the actual copy for the requested data. */
  ଯଦି (m_ptr->m_lsys_krn_sys_getinfo.val_len > 0 &&
	length > m_ptr->m_lsys_krn_sys_getinfo.val_len)
	ଫେରନ୍ତୁ (E2BIG);

  r = data_copy_vmcheck(caller, KERNEL, src_vir, caller->p_endpoint,
	m_ptr->m_lsys_krn_sys_getinfo.val_ptr, length);

  ଯଦି(r != OK) ଫେରନ୍ତୁ r;

	ଯଦି(wipe_rnd_bin >= 0 && wipe_rnd_bin < RANDOM_SOURCES) {
		krandom.bin[wipe_rnd_bin].r_size = 0;
		krandom.bin[wipe_rnd_bin].r_next = 0;
	}

  ଫେରନ୍ତୁ(OK);
}

#endif /* USE_GETINFO */

