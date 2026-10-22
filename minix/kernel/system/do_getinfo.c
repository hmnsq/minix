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

#ಅನ್ತರ್ಭಾವ <string.h>

#ಅನ್ತರ್ಭಾವ "kernel/system.h"


#ಆದರೆ USE_GETINFO

#ಅನ್ತರ್ಭಾವ <minix/u64.h>
#ಅನ್ತರ್ಭಾವ <sys/resource.h>

/*===========================================================================*
 *			        update_idle_time			     *
 *===========================================================================*/
ಸ್ಥಿರ ಶೂನ್ಯ update_idle_ಸಮಯ(ಶೂನ್ಯ)
{
	ಪೂರ್ಣಾಂಕ i;
	ರಚನೆ proc * idl = proc_addr(IDLE);

	idl->p_ಚಕ್ರಗಳು = make64(0, 0);

	ಗಾಗಿ (i = 0; i < CONFIG_MAX_CPUS ; i++) {
		idl->p_ಚಕ್ರಗಳು += get_cpu_var(i, idle_proc).p_ಚಕ್ರಗಳು;
	}
}

/*===========================================================================*
 *			        do_getinfo				     *
 *===========================================================================*/
ಪೂರ್ಣಾಂಕ do_getinfo(ರಚನೆ proc * caller, ಸಂದೇಶ * m_ptr)
{
/* Request system information to be copied to caller's address space. This
 * call simply copies entire data structures to the caller.
 */
  size_t ಉದ್ದಳತೆ;
  vir_ಬೈಟುಗಳು src_vir; 
  ಪೂರ್ಣಾಂಕ nr_e, nr, r;
  ಪೂರ್ಣಾಂಕ wipe_rnd_bin = -1;
  ರಚನೆ proc *p;
  ರಚನೆ rusage r_usage;

  /* Set source address and length based on request type. */
  ಆಯ್ಕೆ (m_ptr->m_lsys_krn_sys_getinfo.request) {
    ಸ್ಥಿತಿ GET_MACHINE: {
        ಉದ್ದಳತೆ = ಗಾತ್ರ(ರಚನೆ machine);
        src_vir = (vir_ಬೈಟುಗಳು) &machine;
        ನಿಲ್ಲಿಸು;
    }
    ಸ್ಥಿತಿ GET_KINFO: {
        ಉದ್ದಳತೆ = ಗಾತ್ರ(ರಚನೆ kinfo);
        src_vir = (vir_ಬೈಟುಗಳು) &kinfo;
        ನಿಲ್ಲಿಸು;
    }
    ಸ್ಥಿತಿ GET_LOADINFO: {
        ಉದ್ದಳತೆ = ಗಾತ್ರ(ರಚನೆ loadinfo);
        src_vir = (vir_ಬೈಟುಗಳು) &kloadinfo;
        ನಿಲ್ಲಿಸು;
    }
    ಸ್ಥಿತಿ GET_CPUINFO: {
        ಉದ್ದಳತೆ = ಗಾತ್ರ(cpu_info);
        src_vir = (vir_ಬೈಟುಗಳು) &cpu_info;
        ನಿಲ್ಲಿಸು;
    }
    ಸ್ಥಿತಿ GET_HZ: {
        ಉದ್ದಳತೆ = ಗಾತ್ರ(system_hz);
        src_vir = (vir_ಬೈಟುಗಳು) &system_hz;
        ನಿಲ್ಲಿಸು;
    }
    ಸ್ಥಿತಿ GET_IMAGE: {
        ಉದ್ದಳತೆ = ಗಾತ್ರ(ರಚನೆ boot_image) * NR_BOOT_PROCS;
        src_vir = (vir_ಬೈಟುಗಳು) image;
        ನಿಲ್ಲಿಸು;
    }
    ಸ್ಥಿತಿ GET_IRQHOOKS: {
        ಉದ್ದಳತೆ = ಗಾತ್ರ(ರಚನೆ irq_hook) * NR_IRQ_HOOKS;
        src_vir = (vir_ಬೈಟುಗಳು) irq_hooks;
        ನಿಲ್ಲಿಸು;
    }
    ಸ್ಥಿತಿ GET_PROCTAB: {
	update_idle_ಸಮಯ();
        ಉದ್ದಳತೆ = ಗಾತ್ರ(ರಚನೆ proc) * (NR_PROCS + NR_TASKS);
        src_vir = (vir_ಬೈಟುಗಳು) proc;
        ನಿಲ್ಲಿಸು;
    }
    ಸ್ಥಿತಿ GET_PRIVTAB: {
        ಉದ್ದಳತೆ = ಗಾತ್ರ(ರಚನೆ priv) * (NR_SYS_PROCS);
        src_vir = (vir_ಬೈಟುಗಳು) priv;
        ನಿಲ್ಲಿಸು;
    }
    ಸ್ಥಿತಿ GET_PROC: {
        nr_e = (m_ptr->m_lsys_krn_sys_getinfo.val_len2_e == SELF) ?
		caller->p_endpoint : m_ptr->m_lsys_krn_sys_getinfo.val_len2_e;
	ಆದರೆ(!isokendpt(nr_e, &nr)) ಹಿಂತಿರುಗಿ EINVAL; /* validate request */
        ಉದ್ದಳತೆ = ಗಾತ್ರ(ರಚನೆ proc);
        src_vir = (vir_ಬೈಟುಗಳು) proc_addr(nr);
        ನಿಲ್ಲಿಸು;
    }
    ಸ್ಥಿತಿ GET_PRIV: {
        nr_e = (m_ptr->m_lsys_krn_sys_getinfo.val_len2_e == SELF) ?
            caller->p_endpoint : m_ptr->m_lsys_krn_sys_getinfo.val_len2_e;
        ಆದರೆ(!isokendpt(nr_e, &nr)) ಹಿಂತಿರುಗಿ EINVAL; /* validate request */
        ಉದ್ದಳತೆ = ಗಾತ್ರ(ರಚನೆ priv);
        src_vir = (vir_ಬೈಟುಗಳು) priv_addr(nr_ಗಮ್ಯ_id(nr));
        ನಿಲ್ಲಿಸು;
    }
    ಸ್ಥಿತಿ GET_REGS: {
        nr_e = (m_ptr->m_lsys_krn_sys_getinfo.val_len2_e == SELF) ?
            caller->p_endpoint : m_ptr->m_lsys_krn_sys_getinfo.val_len2_e;
        ಆದರೆ(!isokendpt(nr_e, &nr)) ಹಿಂತಿರುಗಿ EINVAL; /* validate request */
        p = proc_addr(nr);
        ಉದ್ದಳತೆ = ಗಾತ್ರ(p->p_reg);
        src_vir = (vir_ಬೈಟುಗಳು) &p->p_reg;
        ನಿಲ್ಲಿಸು;
    }
    ಸ್ಥಿತಿ GET_WHOAMI: {
	ಪೂರ್ಣಾಂಕ len;
	m_ptr->m_krn_lsys_sys_getwhoami.endpt = caller->p_endpoint;
	len = MIN(ಗಾತ್ರ(m_ptr->m_krn_lsys_sys_getwhoami.name),
		ಗಾತ್ರ(caller->p_name))-1;
	strncpy(m_ptr->m_krn_lsys_sys_getwhoami.name, caller->p_name, len);
	m_ptr->m_krn_lsys_sys_getwhoami.name[len] = '\0';
	m_ptr->m_krn_lsys_sys_getwhoami.privflags = priv(caller)->s_flags;
        m_ptr->m_krn_lsys_sys_getwhoami.initflags = priv(caller)->s_init_flags;
	ಹಿಂತಿರುಗಿ OK;
    }
    ಸ್ಥಿತಿ GET_MONPARAMS: {
        src_vir = (vir_ಬೈಟುಗಳು) kinfo.param_buf;
	ಉದ್ದಳತೆ = ಗಾತ್ರ(kinfo.param_buf);
        ನಿಲ್ಲಿಸು;
    }
    ಸ್ಥಿತಿ GET_RANDOMNESS: {		
        ಸ್ಥಿರ ರಚನೆ k_randomness copy;	/* copy to keep counters */
	ಪೂರ್ಣಾಂಕ i;

        copy = krandom;
        ಗಾಗಿ (i= 0; i<RANDOM_SOURCES; i++) {
  		krandom.bin[i].r_ಗಾತ್ರ = 0;	/* invalidate random data */
  		krandom.bin[i].r_ಮುಂದಿನ = 0;
	}
    	ಉದ್ದಳತೆ = ಗಾತ್ರ(copy);
    	src_vir = (vir_ಬೈಟುಗಳು) &copy;
    	ನಿಲ್ಲಿಸು;
    }
    ಸ್ಥಿತಿ GET_RANDOMNESS_BIN: {		
	ಪೂರ್ಣಾಂಕ bin = m_ptr->m_lsys_krn_sys_getinfo.val_len2_e;

	ಆದರೆ(bin < 0 || bin >= RANDOM_SOURCES) {
		ಬರೆ("SYSTEM: GET_RANDOMNESS_BIN: %d out of range\n", bin);
		ಹಿಂತಿರುಗಿ EINVAL;
	}

	ಆದರೆ(krandom.bin[bin].r_ಗಾತ್ರ < RANDOM_ELEMENTS)
		ಹಿಂತಿರುಗಿ ENOENT;

    	ಉದ್ದಳತೆ = ಗಾತ್ರ(krandom.bin[bin]);
    	src_vir = (vir_ಬೈಟುಗಳು) &krandom.bin[bin];

	wipe_rnd_bin = bin;

    	ನಿಲ್ಲಿಸು;
    }
    ಸ್ಥಿತಿ GET_IRQACTIDS: {
        ಉದ್ದಳತೆ = ಗಾತ್ರ(irq_actids);
        src_vir = (vir_ಬೈಟುಗಳು) irq_actids;
        ನಿಲ್ಲಿಸು;
    }
    ಸ್ಥಿತಿ GET_IDLETSC: {
	ರಚನೆ proc * idl;
	update_idle_ಸಮಯ();
	idl = proc_addr(IDLE);
        ಉದ್ದಳತೆ = ಗಾತ್ರ(idl->p_ಚಕ್ರಗಳು);
        src_vir = (vir_ಬೈಟುಗಳು) &idl->p_ಚಕ್ರಗಳು;
        ನಿಲ್ಲಿಸು;
    }
    ಸ್ಥಿತಿ GET_CPUTICKS: {
	uint64_t ticks[MINIX_CPUSTATES];
	ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ cpu;
	cpu = (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)m_ptr->m_lsys_krn_sys_getinfo.val_len2_e;
	ಆದರೆ (cpu >= CONFIG_MAX_CPUS)
		ಹಿಂತಿರುಗಿ EINVAL;
	get_cpu_ticks(cpu, ticks);
	ಉದ್ದಳತೆ = ಗಾತ್ರ(ticks);
	src_vir = (vir_ಬೈಟುಗಳು)ticks;
	ನಿಲ್ಲಿಸು;
    }
    ಪೂರ್ವನಿಯೋಜಿತ:
	ಬರೆ("do_getinfo: invalid request %d\n",
		m_ptr->m_lsys_krn_sys_getinfo.request);
        ಹಿಂತಿರುಗಿ(EINVAL);
  }

  /* Try to make the actual copy for the requested data. */
  ಆದರೆ (m_ptr->m_lsys_krn_sys_getinfo.val_len > 0 &&
	ಉದ್ದಳತೆ > m_ptr->m_lsys_krn_sys_getinfo.val_len)
	ಹಿಂತಿರುಗಿ (E2BIG);

  r = ದತ್ತಾಂಶ_copy_vmcheck(caller, KERNEL, src_vir, caller->p_endpoint,
	m_ptr->m_lsys_krn_sys_getinfo.val_ptr, ಉದ್ದಳತೆ);

  ಆದರೆ(r != OK) ಹಿಂತಿರುಗಿ r;

	ಆದರೆ(wipe_rnd_bin >= 0 && wipe_rnd_bin < RANDOM_SOURCES) {
		krandom.bin[wipe_rnd_bin].r_ಗಾತ್ರ = 0;
		krandom.bin[wipe_rnd_bin].r_ಮುಂದಿನ = 0;
	}

  ಹಿಂತಿರುಗಿ(OK);
}

#ಆದರೆ_ಅಂತ್ಯ /* USE_GETINFO */

