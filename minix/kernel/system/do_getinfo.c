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

#অন্তর্ভাব <string.h>

#অন্তর্ভাব "kernel/system.h"


#যদি USE_GETINFO

#অন্তর্ভাব <minix/u64.h>
#অন্তর্ভাব <sys/resource.h>

/*===========================================================================*
 *			        update_idle_time			     *
 *===========================================================================*/
স্থির শূন্য update_idle_time(শূন্য)
{
	পূর্ণসংখ্যা i;
	কাঠামো proc * idl = proc_addr(IDLE);

	idl->p_cycles = make64(0, 0);

	জন্য (i = 0; i < CONFIG_MAX_CPUS ; i++) {
		idl->p_cycles += get_cpu_var(i, idle_proc).p_cycles;
	}
}

/*===========================================================================*
 *			        do_getinfo				     *
 *===========================================================================*/
পূর্ণসংখ্যা do_getinfo(কাঠামো proc * caller, message * m_ptr)
{
/* Request system information to be copied to caller's address space. This
 * call simply copies entire data structures to the caller.
 */
  size_t দৈর্ঘ্য;
  vir_bytes src_vir; 
  পূর্ণসংখ্যা nr_e, nr, r;
  পূর্ণসংখ্যা wipe_rnd_bin = -1;
  কাঠামো proc *p;
  কাঠামো rusage r_usage;

  /* Set source address and length based on request type. */
  নির্বাচন (m_ptr->m_lsys_krn_sys_getinfo.request) {
    ক্ষেত্র GET_MACHINE: {
        দৈর্ঘ্য = আকার(কাঠামো machine);
        src_vir = (vir_bytes) &machine;
        বিরতি;
    }
    ক্ষেত্র GET_KINFO: {
        দৈর্ঘ্য = আকার(কাঠামো kinfo);
        src_vir = (vir_bytes) &kinfo;
        বিরতি;
    }
    ক্ষেত্র GET_LOADINFO: {
        দৈর্ঘ্য = আকার(কাঠামো loadinfo);
        src_vir = (vir_bytes) &kloadinfo;
        বিরতি;
    }
    ক্ষেত্র GET_CPUINFO: {
        দৈর্ঘ্য = আকার(cpu_info);
        src_vir = (vir_bytes) &cpu_info;
        বিরতি;
    }
    ক্ষেত্র GET_HZ: {
        দৈর্ঘ্য = আকার(system_hz);
        src_vir = (vir_bytes) &system_hz;
        বিরতি;
    }
    ক্ষেত্র GET_IMAGE: {
        দৈর্ঘ্য = আকার(কাঠামো boot_image) * NR_BOOT_PROCS;
        src_vir = (vir_bytes) image;
        বিরতি;
    }
    ক্ষেত্র GET_IRQHOOKS: {
        দৈর্ঘ্য = আকার(কাঠামো irq_hook) * NR_IRQ_HOOKS;
        src_vir = (vir_bytes) irq_hooks;
        বিরতি;
    }
    ক্ষেত্র GET_PROCTAB: {
	update_idle_time();
        দৈর্ঘ্য = আকার(কাঠামো proc) * (NR_PROCS + NR_TASKS);
        src_vir = (vir_bytes) proc;
        বিরতি;
    }
    ক্ষেত্র GET_PRIVTAB: {
        দৈর্ঘ্য = আকার(কাঠামো priv) * (NR_SYS_PROCS);
        src_vir = (vir_bytes) priv;
        বিরতি;
    }
    ক্ষেত্র GET_PROC: {
        nr_e = (m_ptr->m_lsys_krn_sys_getinfo.val_len2_e == SELF) ?
		caller->p_endpoint : m_ptr->m_lsys_krn_sys_getinfo.val_len2_e;
	যদি(!isokendpt(nr_e, &nr)) ফেরত EINVAL; /* validate request */
        দৈর্ঘ্য = আকার(কাঠামো proc);
        src_vir = (vir_bytes) proc_addr(nr);
        বিরতি;
    }
    ক্ষেত্র GET_PRIV: {
        nr_e = (m_ptr->m_lsys_krn_sys_getinfo.val_len2_e == SELF) ?
            caller->p_endpoint : m_ptr->m_lsys_krn_sys_getinfo.val_len2_e;
        যদি(!isokendpt(nr_e, &nr)) ফেরত EINVAL; /* validate request */
        দৈর্ঘ্য = আকার(কাঠামো priv);
        src_vir = (vir_bytes) priv_addr(nr_to_id(nr));
        বিরতি;
    }
    ক্ষেত্র GET_REGS: {
        nr_e = (m_ptr->m_lsys_krn_sys_getinfo.val_len2_e == SELF) ?
            caller->p_endpoint : m_ptr->m_lsys_krn_sys_getinfo.val_len2_e;
        যদি(!isokendpt(nr_e, &nr)) ফেরত EINVAL; /* validate request */
        p = proc_addr(nr);
        দৈর্ঘ্য = আকার(p->p_reg);
        src_vir = (vir_bytes) &p->p_reg;
        বিরতি;
    }
    ক্ষেত্র GET_WHOAMI: {
	পূর্ণসংখ্যা len;
	m_ptr->m_krn_lsys_sys_getwhoami.endpt = caller->p_endpoint;
	len = MIN(আকার(m_ptr->m_krn_lsys_sys_getwhoami.name),
		আকার(caller->p_name))-1;
	strncpy(m_ptr->m_krn_lsys_sys_getwhoami.name, caller->p_name, len);
	m_ptr->m_krn_lsys_sys_getwhoami.name[len] = '\0';
	m_ptr->m_krn_lsys_sys_getwhoami.privflags = priv(caller)->s_flags;
        m_ptr->m_krn_lsys_sys_getwhoami.initflags = priv(caller)->s_init_flags;
	ফেরত OK;
    }
    ক্ষেত্র GET_MONPARAMS: {
        src_vir = (vir_bytes) kinfo.param_buf;
	দৈর্ঘ্য = আকার(kinfo.param_buf);
        বিরতি;
    }
    ক্ষেত্র GET_RANDOMNESS: {		
        স্থির কাঠামো k_randomness copy;	/* copy to keep counters */
	পূর্ণসংখ্যা i;

        copy = krandom;
        জন্য (i= 0; i<RANDOM_SOURCES; i++) {
  		krandom.bin[i].r_আকার = 0;	/* invalidate random data */
  		krandom.bin[i].r_পরবর্তী = 0;
	}
    	দৈর্ঘ্য = আকার(copy);
    	src_vir = (vir_bytes) &copy;
    	বিরতি;
    }
    ক্ষেত্র GET_RANDOMNESS_BIN: {		
	পূর্ণসংখ্যা bin = m_ptr->m_lsys_krn_sys_getinfo.val_len2_e;

	যদি(bin < 0 || bin >= RANDOM_SOURCES) {
		printf("SYSTEM: GET_RANDOMNESS_BIN: %d out of range\n", bin);
		ফেরত EINVAL;
	}

	যদি(krandom.bin[bin].r_আকার < RANDOM_ELEMENTS)
		ফেরত ENOENT;

    	দৈর্ঘ্য = আকার(krandom.bin[bin]);
    	src_vir = (vir_bytes) &krandom.bin[bin];

	wipe_rnd_bin = bin;

    	বিরতি;
    }
    ক্ষেত্র GET_IRQACTIDS: {
        দৈর্ঘ্য = আকার(irq_actids);
        src_vir = (vir_bytes) irq_actids;
        বিরতি;
    }
    ক্ষেত্র GET_IDLETSC: {
	কাঠামো proc * idl;
	update_idle_time();
	idl = proc_addr(IDLE);
        দৈর্ঘ্য = আকার(idl->p_cycles);
        src_vir = (vir_bytes) &idl->p_cycles;
        বিরতি;
    }
    ক্ষেত্র GET_CPUTICKS: {
	uint64_t ticks[MINIX_CPUSTATES];
	unsigned পূর্ণসংখ্যা cpu;
	cpu = (unsigned পূর্ণসংখ্যা)m_ptr->m_lsys_krn_sys_getinfo.val_len2_e;
	যদি (cpu >= CONFIG_MAX_CPUS)
		ফেরত EINVAL;
	get_cpu_ticks(cpu, ticks);
	দৈর্ঘ্য = আকার(ticks);
	src_vir = (vir_bytes)ticks;
	বিরতি;
    }
    পূর্বনির্ধারিত:
	printf("do_getinfo: invalid request %d\n",
		m_ptr->m_lsys_krn_sys_getinfo.request);
        ফেরত(EINVAL);
  }

  /* Try to make the actual copy for the requested data. */
  যদি (m_ptr->m_lsys_krn_sys_getinfo.val_len > 0 &&
	দৈর্ঘ্য > m_ptr->m_lsys_krn_sys_getinfo.val_len)
	ফেরত (E2BIG);

  r = তথ্য_copy_vmcheck(caller, KERNEL, src_vir, caller->p_endpoint,
	m_ptr->m_lsys_krn_sys_getinfo.val_ptr, দৈর্ঘ্য);

  যদি(r != OK) ফেরত r;

	যদি(wipe_rnd_bin >= 0 && wipe_rnd_bin < RANDOM_SOURCES) {
		krandom.bin[wipe_rnd_bin].r_আকার = 0;
		krandom.bin[wipe_rnd_bin].r_পরবর্তী = 0;
	}

  ফেরত(OK);
}

#endif /* USE_GETINFO */

