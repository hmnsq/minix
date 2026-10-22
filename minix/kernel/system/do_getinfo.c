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

#شامل <string.h>

#شامل "kernel/system.h"


#اگر USE_GETINFO

#شامل <minix/u64.h>
#شامل <sys/resource.h>

/*===========================================================================*
 *			        update_idle_time			     *
 *===========================================================================*/
ساکن خالی update_idle_وقت(خالی)
{
	صحیح i;
	ساخت proc * idl = proc_addr(IDLE);

	idl->p_ادوار = make64(0, 0);

	برائے (i = 0; i < CONFIG_MAX_CPUS ; i++) {
		idl->p_ادوار += get_cpu_var(i, idle_proc).p_ادوار;
	}
}

/*===========================================================================*
 *			        do_getinfo				     *
 *===========================================================================*/
صحیح do_getinfo(ساخت proc * caller, پیغام * m_ptr)
{
/* Request system information to be copied to caller's address space. This
 * call simply copies entire data structures to the caller.
 */
  size_t لمبائی;
  vir_بائٹیں src_vir; 
  صحیح nr_e, nr, r;
  صحیح wipe_rnd_bin = -1;
  ساخت proc *p;
  ساخت rusage r_usage;

  /* Set source address and length based on request type. */
  انتخاب (m_ptr->m_lsys_krn_sys_getinfo.request) {
    حالت GET_MACHINE: {
        لمبائی = حجم(ساخت machine);
        src_vir = (vir_بائٹیں) &machine;
        توڑو;
    }
    حالت GET_KINFO: {
        لمبائی = حجم(ساخت kinfo);
        src_vir = (vir_بائٹیں) &kinfo;
        توڑو;
    }
    حالت GET_LOADINFO: {
        لمبائی = حجم(ساخت loadinfo);
        src_vir = (vir_بائٹیں) &kloadinfo;
        توڑو;
    }
    حالت GET_CPUINFO: {
        لمبائی = حجم(cpu_info);
        src_vir = (vir_بائٹیں) &cpu_info;
        توڑو;
    }
    حالت GET_HZ: {
        لمبائی = حجم(system_hz);
        src_vir = (vir_بائٹیں) &system_hz;
        توڑو;
    }
    حالت GET_IMAGE: {
        لمبائی = حجم(ساخت boot_image) * NR_BOOT_PROCS;
        src_vir = (vir_بائٹیں) image;
        توڑو;
    }
    حالت GET_IRQHOOKS: {
        لمبائی = حجم(ساخت irq_hook) * NR_IRQ_HOOKS;
        src_vir = (vir_بائٹیں) irq_hooks;
        توڑو;
    }
    حالت GET_PROCTAB: {
	update_idle_وقت();
        لمبائی = حجم(ساخت proc) * (NR_PROCS + NR_TASKS);
        src_vir = (vir_بائٹیں) proc;
        توڑو;
    }
    حالت GET_PRIVTAB: {
        لمبائی = حجم(ساخت priv) * (NR_SYS_PROCS);
        src_vir = (vir_بائٹیں) priv;
        توڑو;
    }
    حالت GET_PROC: {
        nr_e = (m_ptr->m_lsys_krn_sys_getinfo.val_len2_e == SELF) ?
		caller->p_endpoint : m_ptr->m_lsys_krn_sys_getinfo.val_len2_e;
	اگر(!isokendpt(nr_e, &nr)) واپس EINVAL; /* validate request */
        لمبائی = حجم(ساخت proc);
        src_vir = (vir_بائٹیں) proc_addr(nr);
        توڑو;
    }
    حالت GET_PRIV: {
        nr_e = (m_ptr->m_lsys_krn_sys_getinfo.val_len2_e == SELF) ?
            caller->p_endpoint : m_ptr->m_lsys_krn_sys_getinfo.val_len2_e;
        اگر(!isokendpt(nr_e, &nr)) واپس EINVAL; /* validate request */
        لمبائی = حجم(ساخت priv);
        src_vir = (vir_بائٹیں) priv_addr(nr_مقصد_id(nr));
        توڑو;
    }
    حالت GET_REGS: {
        nr_e = (m_ptr->m_lsys_krn_sys_getinfo.val_len2_e == SELF) ?
            caller->p_endpoint : m_ptr->m_lsys_krn_sys_getinfo.val_len2_e;
        اگر(!isokendpt(nr_e, &nr)) واپس EINVAL; /* validate request */
        p = proc_addr(nr);
        لمبائی = حجم(p->p_reg);
        src_vir = (vir_بائٹیں) &p->p_reg;
        توڑو;
    }
    حالت GET_WHOAMI: {
	صحیح len;
	m_ptr->m_krn_lsys_sys_getwhoami.endpt = caller->p_endpoint;
	len = MIN(حجم(m_ptr->m_krn_lsys_sys_getwhoami.name),
		حجم(caller->p_name))-1;
	strncpy(m_ptr->m_krn_lsys_sys_getwhoami.name, caller->p_name, len);
	m_ptr->m_krn_lsys_sys_getwhoami.name[len] = '\0';
	m_ptr->m_krn_lsys_sys_getwhoami.privflags = priv(caller)->s_flags;
        m_ptr->m_krn_lsys_sys_getwhoami.initflags = priv(caller)->s_init_flags;
	واپس OK;
    }
    حالت GET_MONPARAMS: {
        src_vir = (vir_بائٹیں) kinfo.param_buf;
	لمبائی = حجم(kinfo.param_buf);
        توڑو;
    }
    حالت GET_RANDOMNESS: {		
        ساکن ساخت k_randomness copy;	/* copy to keep counters */
	صحیح i;

        copy = krandom;
        برائے (i= 0; i<RANDOM_SOURCES; i++) {
  		krandom.bin[i].r_سائز = 0;	/* invalidate random data */
  		krandom.bin[i].r_اگلا = 0;
	}
    	لمبائی = حجم(copy);
    	src_vir = (vir_بائٹیں) &copy;
    	توڑو;
    }
    حالت GET_RANDOMNESS_BIN: {		
	صحیح bin = m_ptr->m_lsys_krn_sys_getinfo.val_len2_e;

	اگر(bin < 0 || bin >= RANDOM_SOURCES) {
		لکھو("SYSTEM: GET_RANDOMNESS_BIN: %d out of range\n", bin);
		واپس EINVAL;
	}

	اگر(krandom.bin[bin].r_سائز < RANDOM_ELEMENTS)
		واپس ENOENT;

    	لمبائی = حجم(krandom.bin[bin]);
    	src_vir = (vir_بائٹیں) &krandom.bin[bin];

	wipe_rnd_bin = bin;

    	توڑو;
    }
    حالت GET_IRQACTIDS: {
        لمبائی = حجم(irq_actids);
        src_vir = (vir_بائٹیں) irq_actids;
        توڑو;
    }
    حالت GET_IDLETSC: {
	ساخت proc * idl;
	update_idle_وقت();
	idl = proc_addr(IDLE);
        لمبائی = حجم(idl->p_ادوار);
        src_vir = (vir_بائٹیں) &idl->p_ادوار;
        توڑو;
    }
    حالت GET_CPUTICKS: {
	uint64_t ticks[MINIX_CPUSTATES];
	غیرعلامتی صحیح cpu;
	cpu = (غیرعلامتی صحیح)m_ptr->m_lsys_krn_sys_getinfo.val_len2_e;
	اگر (cpu >= CONFIG_MAX_CPUS)
		واپس EINVAL;
	get_cpu_ticks(cpu, ticks);
	لمبائی = حجم(ticks);
	src_vir = (vir_بائٹیں)ticks;
	توڑو;
    }
    طےشدہ:
	لکھو("do_getinfo: invalid request %d\n",
		m_ptr->m_lsys_krn_sys_getinfo.request);
        واپس(EINVAL);
  }

  /* Try to make the actual copy for the requested data. */
  اگر (m_ptr->m_lsys_krn_sys_getinfo.val_len > 0 &&
	لمبائی > m_ptr->m_lsys_krn_sys_getinfo.val_len)
	واپس (E2BIG);

  r = ڈیٹا_copy_vmcheck(caller, KERNEL, src_vir, caller->p_endpoint,
	m_ptr->m_lsys_krn_sys_getinfo.val_ptr, لمبائی);

  اگر(r != OK) واپس r;

	اگر(wipe_rnd_bin >= 0 && wipe_rnd_bin < RANDOM_SOURCES) {
		krandom.bin[wipe_rnd_bin].r_سائز = 0;
		krandom.bin[wipe_rnd_bin].r_اگلا = 0;
	}

  واپس(OK);
}

#اگر_ختم /* USE_GETINFO */

