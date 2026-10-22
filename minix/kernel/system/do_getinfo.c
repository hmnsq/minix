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

#include <string.h>

#include "kernel/system.h"


#གལ་ཏེ USE_GETINFO

#include <minix/u64.h>
#include <sys/resource.h>

/*===========================================================================*
 *			        update_idle_time			     *
 *===========================================================================*/
static void update_idle_time(void)
{
	ཧྲིལ i;
	sa_nracanaa proc * idl = proc_addr(IDLE);

	idl->p_cycles = make64(0, 0);

	ཆེད་དུ (i = 0; i < CONFIG_MAX_CPUS ; i++) {
		idl->p_cycles += get_cpu_var(i, idle_proc).p_cycles;
	}
}

/*===========================================================================*
 *			        do_getinfo				     *
 *===========================================================================*/
ཧྲིལ do_getinfo(sa_nracanaa proc * caller, message * m_ptr)
{
/* Request system information to be copied to caller's address space. This
 * call simply copies entire data structures to the caller.
 */
  size_t length;
  vir_bytes src_vir; 
  ཧྲིལ nr_e, nr, r;
  ཧྲིལ wipe_rnd_bin = -1;
  sa_nracanaa proc *p;
  sa_nracanaa rusage r_usage;

  /* Set source address and length based on request type. */
  switch (m_ptr->m_lsys_krn_sys_getinfo.request) {
    pak_sa GET_MACHINE: {
        length = parimaa_na(sa_nracanaa machine);
        src_vir = (vir_bytes) &machine;
        bha_nga;
    }
    pak_sa GET_KINFO: {
        length = parimaa_na(sa_nracanaa kinfo);
        src_vir = (vir_bytes) &kinfo;
        bha_nga;
    }
    pak_sa GET_LOADINFO: {
        length = parimaa_na(sa_nracanaa loadinfo);
        src_vir = (vir_bytes) &kloadinfo;
        bha_nga;
    }
    pak_sa GET_CPUINFO: {
        length = parimaa_na(cpu_info);
        src_vir = (vir_bytes) &cpu_info;
        bha_nga;
    }
    pak_sa GET_HZ: {
        length = parimaa_na(system_hz);
        src_vir = (vir_bytes) &system_hz;
        bha_nga;
    }
    pak_sa GET_IMAGE: {
        length = parimaa_na(sa_nracanaa boot_image) * NR_BOOT_PROCS;
        src_vir = (vir_bytes) image;
        bha_nga;
    }
    pak_sa GET_IRQHOOKS: {
        length = parimaa_na(sa_nracanaa irq_hook) * NR_IRQ_HOOKS;
        src_vir = (vir_bytes) irq_hooks;
        bha_nga;
    }
    pak_sa GET_PROCTAB: {
	update_idle_time();
        length = parimaa_na(sa_nracanaa proc) * (NR_PROCS + NR_TASKS);
        src_vir = (vir_bytes) proc;
        bha_nga;
    }
    pak_sa GET_PRIVTAB: {
        length = parimaa_na(sa_nracanaa priv) * (NR_SYS_PROCS);
        src_vir = (vir_bytes) priv;
        bha_nga;
    }
    pak_sa GET_PROC: {
        nr_e = (m_ptr->m_lsys_krn_sys_getinfo.val_len2_e == SELF) ?
		caller->p_endpoint : m_ptr->m_lsys_krn_sys_getinfo.val_len2_e;
	གལ་ཏེ(!isokendpt(nr_e, &nr)) ཕྱིར EINVAL; /* validate request */
        length = parimaa_na(sa_nracanaa proc);
        src_vir = (vir_bytes) proc_addr(nr);
        bha_nga;
    }
    pak_sa GET_PRIV: {
        nr_e = (m_ptr->m_lsys_krn_sys_getinfo.val_len2_e == SELF) ?
            caller->p_endpoint : m_ptr->m_lsys_krn_sys_getinfo.val_len2_e;
        གལ་ཏེ(!isokendpt(nr_e, &nr)) ཕྱིར EINVAL; /* validate request */
        length = parimaa_na(sa_nracanaa priv);
        src_vir = (vir_bytes) priv_addr(nr_to_id(nr));
        bha_nga;
    }
    pak_sa GET_REGS: {
        nr_e = (m_ptr->m_lsys_krn_sys_getinfo.val_len2_e == SELF) ?
            caller->p_endpoint : m_ptr->m_lsys_krn_sys_getinfo.val_len2_e;
        གལ་ཏེ(!isokendpt(nr_e, &nr)) ཕྱིར EINVAL; /* validate request */
        p = proc_addr(nr);
        length = parimaa_na(p->p_reg);
        src_vir = (vir_bytes) &p->p_reg;
        bha_nga;
    }
    pak_sa GET_WHOAMI: {
	ཧྲིལ len;
	m_ptr->m_krn_lsys_sys_getwhoami.endpt = caller->p_endpoint;
	len = MIN(parimaa_na(m_ptr->m_krn_lsys_sys_getwhoami.name),
		parimaa_na(caller->p_name))-1;
	strncpy(m_ptr->m_krn_lsys_sys_getwhoami.name, caller->p_name, len);
	m_ptr->m_krn_lsys_sys_getwhoami.name[len] = '\0';
	m_ptr->m_krn_lsys_sys_getwhoami.privflags = priv(caller)->s_flags;
        m_ptr->m_krn_lsys_sys_getwhoami.initflags = priv(caller)->s_init_flags;
	ཕྱིར OK;
    }
    pak_sa GET_MONPARAMS: {
        src_vir = (vir_bytes) kinfo.param_buf;
	length = parimaa_na(kinfo.param_buf);
        bha_nga;
    }
    pak_sa GET_RANDOMNESS: {		
        static sa_nracanaa k_randomness copy;	/* copy to keep counters */
	ཧྲིལ i;

        copy = krandom;
        ཆེད་དུ (i= 0; i<RANDOM_SOURCES; i++) {
  		krandom.bin[i].r_size = 0;	/* invalidate random data */
  		krandom.bin[i].r_next = 0;
	}
    	length = parimaa_na(copy);
    	src_vir = (vir_bytes) &copy;
    	bha_nga;
    }
    pak_sa GET_RANDOMNESS_BIN: {		
	ཧྲིལ bin = m_ptr->m_lsys_krn_sys_getinfo.val_len2_e;

	གལ་ཏེ(bin < 0 || bin >= RANDOM_SOURCES) {
		printf("SYSTEM: GET_RANDOMNESS_BIN: %d out of range\n", bin);
		ཕྱིར EINVAL;
	}

	གལ་ཏེ(krandom.bin[bin].r_size < RANDOM_ELEMENTS)
		ཕྱིར ENOENT;

    	length = parimaa_na(krandom.bin[bin]);
    	src_vir = (vir_bytes) &krandom.bin[bin];

	wipe_rnd_bin = bin;

    	bha_nga;
    }
    pak_sa GET_IRQACTIDS: {
        length = parimaa_na(irq_actids);
        src_vir = (vir_bytes) irq_actids;
        bha_nga;
    }
    pak_sa GET_IDLETSC: {
	sa_nracanaa proc * idl;
	update_idle_time();
	idl = proc_addr(IDLE);
        length = parimaa_na(idl->p_cycles);
        src_vir = (vir_bytes) &idl->p_cycles;
        bha_nga;
    }
    pak_sa GET_CPUTICKS: {
	uint64_t ticks[MINIX_CPUSTATES];
	unsigned ཧྲིལ cpu;
	cpu = (unsigned ཧྲིལ)m_ptr->m_lsys_krn_sys_getinfo.val_len2_e;
	གལ་ཏེ (cpu >= CONFIG_MAX_CPUS)
		ཕྱིར EINVAL;
	get_cpu_ticks(cpu, ticks);
	length = parimaa_na(ticks);
	src_vir = (vir_bytes)ticks;
	bha_nga;
    }
    default:
	printf("do_getinfo: invalid request %d\n",
		m_ptr->m_lsys_krn_sys_getinfo.request);
        ཕྱིར(EINVAL);
  }

  /* Try to make the actual copy for the requested data. */
  གལ་ཏེ (m_ptr->m_lsys_krn_sys_getinfo.val_len > 0 &&
	length > m_ptr->m_lsys_krn_sys_getinfo.val_len)
	ཕྱིར (E2BIG);

  r = data_copy_vmcheck(caller, KERNEL, src_vir, caller->p_endpoint,
	m_ptr->m_lsys_krn_sys_getinfo.val_ptr, length);

  གལ་ཏེ(r != OK) ཕྱིར r;

	གལ་ཏེ(wipe_rnd_bin >= 0 && wipe_rnd_bin < RANDOM_SOURCES) {
		krandom.bin[wipe_rnd_bin].r_size = 0;
		krandom.bin[wipe_rnd_bin].r_next = 0;
	}

  ཕྱིར(OK);
}

#endif /* USE_GETINFO */

