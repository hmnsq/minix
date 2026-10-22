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

#అన్తర్భావ <string.h>

#అన్తర్భావ "kernel/system.h"


#ఒకవేళ USE_GETINFO

#అన్తర్భావ <minix/u64.h>
#అన్తర్భావ <sys/resource.h>

/*===========================================================================*
 *			        update_idle_time			     *
 *===========================================================================*/
స్థిర శూన్య update_idle_సమయం(శూన్య)
{
	పూర్ణాంకం i;
	నిర్మాణం proc * idl = proc_addr(IDLE);

	idl->p_చక్రాలు = make64(0, 0);

	కొరకు (i = 0; i < CONFIG_MAX_CPUS ; i++) {
		idl->p_చక్రాలు += get_cpu_var(i, idle_proc).p_చక్రాలు;
	}
}

/*===========================================================================*
 *			        do_getinfo				     *
 *===========================================================================*/
పూర్ణాంకం do_getinfo(నిర్మాణం proc * caller, సందేశం * m_ptr)
{
/* Request system information to be copied to caller's address space. This
 * call simply copies entire data structures to the caller.
 */
  size_t నిడివి;
  vir_బైట్లు src_vir; 
  పూర్ణాంకం nr_e, nr, r;
  పూర్ణాంకం wipe_rnd_bin = -1;
  నిర్మాణం proc *p;
  నిర్మాణం rusage r_usage;

  /* Set source address and length based on request type. */
  ఎంపిక (m_ptr->m_lsys_krn_sys_getinfo.request) {
    స్థితి GET_MACHINE: {
        నిడివి = పరిమాణం(నిర్మాణం machine);
        src_vir = (vir_బైట్లు) &machine;
        ఆపు;
    }
    స్థితి GET_KINFO: {
        నిడివి = పరిమాణం(నిర్మాణం kinfo);
        src_vir = (vir_బైట్లు) &kinfo;
        ఆపు;
    }
    స్థితి GET_LOADINFO: {
        నిడివి = పరిమాణం(నిర్మాణం loadinfo);
        src_vir = (vir_బైట్లు) &kloadinfo;
        ఆపు;
    }
    స్థితి GET_CPUINFO: {
        నిడివి = పరిమాణం(cpu_info);
        src_vir = (vir_బైట్లు) &cpu_info;
        ఆపు;
    }
    స్థితి GET_HZ: {
        నిడివి = పరిమాణం(system_hz);
        src_vir = (vir_బైట్లు) &system_hz;
        ఆపు;
    }
    స్థితి GET_IMAGE: {
        నిడివి = పరిమాణం(నిర్మాణం boot_image) * NR_BOOT_PROCS;
        src_vir = (vir_బైట్లు) image;
        ఆపు;
    }
    స్థితి GET_IRQHOOKS: {
        నిడివి = పరిమాణం(నిర్మాణం irq_hook) * NR_IRQ_HOOKS;
        src_vir = (vir_బైట్లు) irq_hooks;
        ఆపు;
    }
    స్థితి GET_PROCTAB: {
	update_idle_సమయం();
        నిడివి = పరిమాణం(నిర్మాణం proc) * (NR_PROCS + NR_TASKS);
        src_vir = (vir_బైట్లు) proc;
        ఆపు;
    }
    స్థితి GET_PRIVTAB: {
        నిడివి = పరిమాణం(నిర్మాణం priv) * (NR_SYS_PROCS);
        src_vir = (vir_బైట్లు) priv;
        ఆపు;
    }
    స్థితి GET_PROC: {
        nr_e = (m_ptr->m_lsys_krn_sys_getinfo.val_len2_e == SELF) ?
		caller->p_endpoint : m_ptr->m_lsys_krn_sys_getinfo.val_len2_e;
	ఒకవేళ(!isokendpt(nr_e, &nr)) తిరిగి EINVAL; /* validate request */
        నిడివి = పరిమాణం(నిర్మాణం proc);
        src_vir = (vir_బైట్లు) proc_addr(nr);
        ఆపు;
    }
    స్థితి GET_PRIV: {
        nr_e = (m_ptr->m_lsys_krn_sys_getinfo.val_len2_e == SELF) ?
            caller->p_endpoint : m_ptr->m_lsys_krn_sys_getinfo.val_len2_e;
        ఒకవేళ(!isokendpt(nr_e, &nr)) తిరిగి EINVAL; /* validate request */
        నిడివి = పరిమాణం(నిర్మాణం priv);
        src_vir = (vir_బైట్లు) priv_addr(nr_గమ్యం_id(nr));
        ఆపు;
    }
    స్థితి GET_REGS: {
        nr_e = (m_ptr->m_lsys_krn_sys_getinfo.val_len2_e == SELF) ?
            caller->p_endpoint : m_ptr->m_lsys_krn_sys_getinfo.val_len2_e;
        ఒకవేళ(!isokendpt(nr_e, &nr)) తిరిగి EINVAL; /* validate request */
        p = proc_addr(nr);
        నిడివి = పరిమాణం(p->p_reg);
        src_vir = (vir_బైట్లు) &p->p_reg;
        ఆపు;
    }
    స్థితి GET_WHOAMI: {
	పూర్ణాంకం len;
	m_ptr->m_krn_lsys_sys_getwhoami.endpt = caller->p_endpoint;
	len = MIN(పరిమాణం(m_ptr->m_krn_lsys_sys_getwhoami.name),
		పరిమాణం(caller->p_name))-1;
	strncpy(m_ptr->m_krn_lsys_sys_getwhoami.name, caller->p_name, len);
	m_ptr->m_krn_lsys_sys_getwhoami.name[len] = '\0';
	m_ptr->m_krn_lsys_sys_getwhoami.privflags = priv(caller)->s_flags;
        m_ptr->m_krn_lsys_sys_getwhoami.initflags = priv(caller)->s_init_flags;
	తిరిగి OK;
    }
    స్థితి GET_MONPARAMS: {
        src_vir = (vir_బైట్లు) kinfo.param_buf;
	నిడివి = పరిమాణం(kinfo.param_buf);
        ఆపు;
    }
    స్థితి GET_RANDOMNESS: {		
        స్థిర నిర్మాణం k_randomness copy;	/* copy to keep counters */
	పూర్ణాంకం i;

        copy = krandom;
        కొరకు (i= 0; i<RANDOM_SOURCES; i++) {
  		krandom.bin[i].r_కొలత = 0;	/* invalidate random data */
  		krandom.bin[i].r_తదుపరి = 0;
	}
    	నిడివి = పరిమాణం(copy);
    	src_vir = (vir_బైట్లు) &copy;
    	ఆపు;
    }
    స్థితి GET_RANDOMNESS_BIN: {		
	పూర్ణాంకం bin = m_ptr->m_lsys_krn_sys_getinfo.val_len2_e;

	ఒకవేళ(bin < 0 || bin >= RANDOM_SOURCES) {
		రాయి("SYSTEM: GET_RANDOMNESS_BIN: %d out of range\n", bin);
		తిరిగి EINVAL;
	}

	ఒకవేళ(krandom.bin[bin].r_కొలత < RANDOM_ELEMENTS)
		తిరిగి ENOENT;

    	నిడివి = పరిమాణం(krandom.bin[bin]);
    	src_vir = (vir_బైట్లు) &krandom.bin[bin];

	wipe_rnd_bin = bin;

    	ఆపు;
    }
    స్థితి GET_IRQACTIDS: {
        నిడివి = పరిమాణం(irq_actids);
        src_vir = (vir_బైట్లు) irq_actids;
        ఆపు;
    }
    స్థితి GET_IDLETSC: {
	నిర్మాణం proc * idl;
	update_idle_సమయం();
	idl = proc_addr(IDLE);
        నిడివి = పరిమాణం(idl->p_చక్రాలు);
        src_vir = (vir_బైట్లు) &idl->p_చక్రాలు;
        ఆపు;
    }
    స్థితి GET_CPUTICKS: {
	uint64_t ticks[MINIX_CPUSTATES];
	గుర్తులేని పూర్ణాంకం cpu;
	cpu = (గుర్తులేని పూర్ణాంకం)m_ptr->m_lsys_krn_sys_getinfo.val_len2_e;
	ఒకవేళ (cpu >= CONFIG_MAX_CPUS)
		తిరిగి EINVAL;
	get_cpu_ticks(cpu, ticks);
	నిడివి = పరిమాణం(ticks);
	src_vir = (vir_బైట్లు)ticks;
	ఆపు;
    }
    అప్రమేయ:
	రాయి("do_getinfo: invalid request %d\n",
		m_ptr->m_lsys_krn_sys_getinfo.request);
        తిరిగి(EINVAL);
  }

  /* Try to make the actual copy for the requested data. */
  ఒకవేళ (m_ptr->m_lsys_krn_sys_getinfo.val_len > 0 &&
	నిడివి > m_ptr->m_lsys_krn_sys_getinfo.val_len)
	తిరిగి (E2BIG);

  r = దత్తాంశం_copy_vmcheck(caller, KERNEL, src_vir, caller->p_endpoint,
	m_ptr->m_lsys_krn_sys_getinfo.val_ptr, నిడివి);

  ఒకవేళ(r != OK) తిరిగి r;

	ఒకవేళ(wipe_rnd_bin >= 0 && wipe_rnd_bin < RANDOM_SOURCES) {
		krandom.bin[wipe_rnd_bin].r_కొలత = 0;
		krandom.bin[wipe_rnd_bin].r_తదుపరి = 0;
	}

  తిరిగి(OK);
}

#అయితే_ముగింపు /* USE_GETINFO */

