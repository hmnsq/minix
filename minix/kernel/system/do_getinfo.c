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

#समावेश <string.h>

#समावेश "kernel/system.h"


#अगर USE_GETINFO

#समावेश <minix/u64.h>
#समावेश <sys/resource.h>

/*===========================================================================*
 *			        update_idle_time			     *
 *===========================================================================*/
अटल व्योम update_idle_time(व्योम)
{
	पूर्णांक i;
	काष्ठा proc * idl = proc_addr(IDLE);

	idl->p_cycles = make64(0, 0);

	क्रम (i = 0; i < CONFIG_MAX_CPUS ; i++) {
		idl->p_cycles += get_cpu_var(i, idle_proc).p_cycles;
	}
}

/*===========================================================================*
 *			        do_getinfo				     *
 *===========================================================================*/
पूर्णांक do_getinfo(काष्ठा proc * caller, message * m_ptr)
{
/* Request system information to be copied to caller's address space. This
 * call simply copies entire data structures to the caller.
 */
  माप_प्रकार लंबाई;
  vir_bytes src_vir; 
  पूर्णांक nr_e, nr, r;
  पूर्णांक wipe_rnd_bin = -1;
  काष्ठा proc *p;
  काष्ठा rusage r_usage;

  /* Set source address and length based on request type. */
  चयन (m_ptr->m_lsys_krn_sys_getinfo.request) {
    हाल GET_MACHINE: {
        लंबाई = माप(काष्ठा machine);
        src_vir = (vir_bytes) &machine;
        अवरोध;
    }
    हाल GET_KINFO: {
        लंबाई = माप(काष्ठा kinfo);
        src_vir = (vir_bytes) &kinfo;
        अवरोध;
    }
    हाल GET_LOADINFO: {
        लंबाई = माप(काष्ठा loadinfo);
        src_vir = (vir_bytes) &kloadinfo;
        अवरोध;
    }
    हाल GET_CPUINFO: {
        लंबाई = माप(cpu_info);
        src_vir = (vir_bytes) &cpu_info;
        अवरोध;
    }
    हाल GET_HZ: {
        लंबाई = माप(system_hz);
        src_vir = (vir_bytes) &system_hz;
        अवरोध;
    }
    हाल GET_IMAGE: {
        लंबाई = माप(काष्ठा boot_image) * NR_BOOT_PROCS;
        src_vir = (vir_bytes) image;
        अवरोध;
    }
    हाल GET_IRQHOOKS: {
        लंबाई = माप(काष्ठा irq_hook) * NR_IRQ_HOOKS;
        src_vir = (vir_bytes) irq_hooks;
        अवरोध;
    }
    हाल GET_PROCTAB: {
	update_idle_time();
        लंबाई = माप(काष्ठा proc) * (NR_PROCS + NR_TASKS);
        src_vir = (vir_bytes) proc;
        अवरोध;
    }
    हाल GET_PRIVTAB: {
        लंबाई = माप(काष्ठा priv) * (NR_SYS_PROCS);
        src_vir = (vir_bytes) priv;
        अवरोध;
    }
    हाल GET_PROC: {
        nr_e = (m_ptr->m_lsys_krn_sys_getinfo.val_len2_e == SELF) ?
		caller->p_endpoint : m_ptr->m_lsys_krn_sys_getinfo.val_len2_e;
	अगर(!isokendpt(nr_e, &nr)) वापस EINVAL; /* validate request */
        लंबाई = माप(काष्ठा proc);
        src_vir = (vir_bytes) proc_addr(nr);
        अवरोध;
    }
    हाल GET_PRIV: {
        nr_e = (m_ptr->m_lsys_krn_sys_getinfo.val_len2_e == SELF) ?
            caller->p_endpoint : m_ptr->m_lsys_krn_sys_getinfo.val_len2_e;
        अगर(!isokendpt(nr_e, &nr)) वापस EINVAL; /* validate request */
        लंबाई = माप(काष्ठा priv);
        src_vir = (vir_bytes) priv_addr(nr_to_id(nr));
        अवरोध;
    }
    हाल GET_REGS: {
        nr_e = (m_ptr->m_lsys_krn_sys_getinfo.val_len2_e == SELF) ?
            caller->p_endpoint : m_ptr->m_lsys_krn_sys_getinfo.val_len2_e;
        अगर(!isokendpt(nr_e, &nr)) वापस EINVAL; /* validate request */
        p = proc_addr(nr);
        लंबाई = माप(p->p_reg);
        src_vir = (vir_bytes) &p->p_reg;
        अवरोध;
    }
    हाल GET_WHOAMI: {
	पूर्णांक len;
	m_ptr->m_krn_lsys_sys_getwhoami.endpt = caller->p_endpoint;
	len = MIN(माप(m_ptr->m_krn_lsys_sys_getwhoami.name),
		माप(caller->p_name))-1;
	म_नकलन(m_ptr->m_krn_lsys_sys_getwhoami.name, caller->p_name, len);
	m_ptr->m_krn_lsys_sys_getwhoami.name[len] = '\0';
	m_ptr->m_krn_lsys_sys_getwhoami.privflags = priv(caller)->s_flags;
        m_ptr->m_krn_lsys_sys_getwhoami.initflags = priv(caller)->s_init_flags;
	वापस OK;
    }
    हाल GET_MONPARAMS: {
        src_vir = (vir_bytes) kinfo.param_buf;
	लंबाई = माप(kinfo.param_buf);
        अवरोध;
    }
    हाल GET_RANDOMNESS: {		
        अटल काष्ठा k_randomness copy;	/* copy to keep counters */
	पूर्णांक i;

        copy = krandom;
        क्रम (i= 0; i<RANDOM_SOURCES; i++) {
  		krandom.bin[i].r_आकार = 0;	/* invalidate random data */
  		krandom.bin[i].r_अग्र = 0;
	}
    	लंबाई = माप(copy);
    	src_vir = (vir_bytes) &copy;
    	अवरोध;
    }
    हाल GET_RANDOMNESS_BIN: {		
	पूर्णांक bin = m_ptr->m_lsys_krn_sys_getinfo.val_len2_e;

	अगर(bin < 0 || bin >= RANDOM_SOURCES) {
		म_लिखो("SYSTEM: GET_RANDOMNESS_BIN: %d out of range\n", bin);
		वापस EINVAL;
	}

	अगर(krandom.bin[bin].r_आकार < RANDOM_ELEMENTS)
		वापस ENOENT;

    	लंबाई = माप(krandom.bin[bin]);
    	src_vir = (vir_bytes) &krandom.bin[bin];

	wipe_rnd_bin = bin;

    	अवरोध;
    }
    हाल GET_IRQACTIDS: {
        लंबाई = माप(irq_actids);
        src_vir = (vir_bytes) irq_actids;
        अवरोध;
    }
    हाल GET_IDLETSC: {
	काष्ठा proc * idl;
	update_idle_time();
	idl = proc_addr(IDLE);
        लंबाई = माप(idl->p_cycles);
        src_vir = (vir_bytes) &idl->p_cycles;
        अवरोध;
    }
    हाल GET_CPUTICKS: {
	uint64_t ticks[MINIX_CPUSTATES];
	अचिन्हित पूर्णांक cpu;
	cpu = (अचिन्हित पूर्णांक)m_ptr->m_lsys_krn_sys_getinfo.val_len2_e;
	अगर (cpu >= CONFIG_MAX_CPUS)
		वापस EINVAL;
	get_cpu_ticks(cpu, ticks);
	लंबाई = माप(ticks);
	src_vir = (vir_bytes)ticks;
	अवरोध;
    }
    शेष:
	म_लिखो("do_getinfo: invalid request %d\n",
		m_ptr->m_lsys_krn_sys_getinfo.request);
        वापस(EINVAL);
  }

  /* Try to make the actual copy for the requested data. */
  अगर (m_ptr->m_lsys_krn_sys_getinfo.val_len > 0 &&
	लंबाई > m_ptr->m_lsys_krn_sys_getinfo.val_len)
	वापस (E2BIG);

  r = तथ्य_copy_vmcheck(caller, KERNEL, src_vir, caller->p_endpoint,
	m_ptr->m_lsys_krn_sys_getinfo.val_ptr, लंबाई);

  अगर(r != OK) वापस r;

	अगर(wipe_rnd_bin >= 0 && wipe_rnd_bin < RANDOM_SOURCES) {
		krandom.bin[wipe_rnd_bin].r_आकार = 0;
		krandom.bin[wipe_rnd_bin].r_अग्र = 0;
	}

  वापस(OK);
}

#पूर्ण_अगर /* USE_GETINFO */

