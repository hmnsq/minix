/* The kernel call implemented in this file:
 *   m_type:	SYS_SDEVIO
 *
 * The parameters for this kernel call are:
 *    m_lsys_krn_sys_sdevio.request	(request input or output)
 *    m_lsys_krn_sys_sdevio.port	(port to read/ write)
 *    m_lsys_krn_sys_sdevio.vec_addr	(virtual address of buffer, or grant ID)
 *    m_lsys_krn_sys_sdevio.vec_size	(number of elements)
 *    m_lsys_krn_sys_sdevio.vec_endpt	(process where buffer is)
 *    m_lsys_krn_sys_sdevio.offset	(offset into the grant)
 */

#അന്തര്ഭാവ "kernel/system.h"
#അന്തര്ഭാവ <minix/devio.h>
#അന്തര്ഭാവ <minix/endpoint.h>

#അന്തര്ഭാവ "arch_proto.h"

#എങ്കിൽ USE_SDEVIO

/*===========================================================================*
 *			        do_sdevio                                    *
 *===========================================================================*/
പൂർണ്ണസംഖ്യ do_sdevio(ഘടന proc * caller, message *m_ptr)
{
  vir_bytes newoffset;
  endpoint_t newep;
  പൂർണ്ണസംഖ്യ proc_nr;
  endpoint_t proc_nr_e = m_ptr->m_lsys_krn_sys_sdevio.vec_endpt;
  vir_bytes എണ്ണം = m_ptr->m_lsys_krn_sys_sdevio.vec_size;
  long port = m_ptr->m_lsys_krn_sys_sdevio.port;
  phys_bytes vir_buf;
  പൂർണ്ണസംഖ്യ i, r, req_type, req_dir, size, nr_io_range;
  ഘടന priv *privp;
  ഘടന io_range *iorp;
  ഘടന proc *destproc;
  പൂർണ്ണസംഖ്യ retval;

  /* Allow safe copies and accesses to SELF */
  എങ്കിൽ ((m_ptr->m_lsys_krn_sys_sdevio.request & _DIO_SAFEMASK) != _DIO_SAFE &&
	proc_nr_e != SELF)
  {
	സ്ഥിതം പൂർണ്ണസംഖ്യ first= 1;
	എങ്കിൽ (first)
	{
		first= 0;
		printf("do_sdevio: for %d, req %d\n",
			m_ptr->m_source, m_ptr->m_lsys_krn_sys_sdevio.request);
	}
  }

  /* Check if process endpoint is OK. 
   * A driver may directly provide a pointer to a buffer at the user-process
   * that initiated the device I/O. Kernel processes, of course, are denied.
   */
  എങ്കിൽ (proc_nr_e == SELF)
	okendpt(caller->p_endpoint, &proc_nr);
  അല്ലെങ്കിൽ
	എങ്കിൽ(!isokendpt(proc_nr_e, &proc_nr))
		തിരികെ(EINVAL);
  എങ്കിൽ (iskerneln(proc_nr)) തിരികെ(EPERM);

  /* Extract direction (in or out) and type (size). */
  req_dir = m_ptr->m_lsys_krn_sys_sdevio.request & _DIO_DIRMASK;
  req_type = m_ptr->m_lsys_krn_sys_sdevio.request & _DIO_TYPEMASK;

  /* Check for 'safe' variants. */
  എങ്കിൽ((m_ptr->m_lsys_krn_sys_sdevio.request & _DIO_SAFEMASK) == _DIO_SAFE) {
     /* Map grant address to physical address. */
     എങ്കിൽ((r=verify_grant(proc_nr_e, caller->p_endpoint,
		m_ptr->m_lsys_krn_sys_sdevio.vec_addr, എണ്ണം,
		req_dir == DIO_നിവേശം ? CPF_WRITE : CPF_READ,
		m_ptr->m_lsys_krn_sys_sdevio.offset, &newoffset, &newep,
		ശൂന്യ)) != OK) {
	എങ്കിൽ(r == ENOTREADY) തിരികെ r;
	printf("do_sdevio: verify_grant failed\n");
	തിരികെ EPERM;
    }
	എങ്കിൽ(!isokendpt(newep, &proc_nr))
		തിരികെ(EINVAL);
     destproc = proc_addr(proc_nr);
     vir_buf = newoffset;
  } അല്ലെങ്കിൽ {
     എങ്കിൽ(proc_nr != _ENDPOINT_P(caller->p_endpoint))
     {
	printf("do_sdevio: unsafe sdevio by %d in %d denied\n",
		caller->p_endpoint, proc_nr_e);
	തിരികെ EPERM;
     }
     /* Get and check physical address. */
     vir_buf = m_ptr->m_lsys_krn_sys_sdevio.vec_addr;
     destproc = proc_addr(proc_nr);
  }
     /* current process must be target for phys_* to be OK */

  switch_address_space(destproc);

  തിരഞ്ഞെടുപ്പ് (req_type)
  {
	അവസ്ഥ _DIO_BYTE: size= 1; നിർത്തുക;
	അവസ്ഥ _DIO_WORD: size= 2; നിർത്തുക;
	അവസ്ഥ _DIO_LONG: size= 4; നിർത്തുക;
	സ്ഥിരസ്ഥിതി: size= 4; നിർത്തുക;	/* Be conservative */
  }

  privp= priv(caller);
  എങ്കിൽ (privp && privp->s_flags & CHECK_IO_PORT)
  {
	port= m_ptr->m_lsys_krn_sys_sdevio.port;
	nr_io_range= privp->s_nr_io_range;
	വേണ്ടി (i= 0, iorp= privp->s_io_tab; i<nr_io_range; i++, iorp++)
	{
		എങ്കിൽ (port >= iorp->ior_base && port+size-1 <= iorp->ior_limit)
			നിർത്തുക;
	}
	എങ്കിൽ (i >= nr_io_range)
	{
		printf(
		"do_sdevio: I/O port check failed for proc %d, port 0x%x\n",
			m_ptr->m_source, port);
		retval = EPERM;
		പോകുക return_error;
	}
  }

  എങ്കിൽ (port & (size-1))
  {
	printf("do_devio: unaligned port 0x%x (size %d)\n", port, size);
	retval = EPERM;
	പോകുക return_error;
  }

  /* Perform device I/O for bytes and words. Longs are not supported. */
  എങ്കിൽ (req_dir == DIO_നിവേശം) { 
      തിരഞ്ഞെടുപ്പ് (req_type) {
      അവസ്ഥ _DIO_BYTE: phys_insb(port, vir_buf, എണ്ണം); നിർത്തുക; 
      അവസ്ഥ _DIO_WORD: phys_insw(port, vir_buf, എണ്ണം); നിർത്തുക; 
      സ്ഥിരസ്ഥിതി:
  		retval = EINVAL;
		പോകുക return_error;
      } 
  } അല്ലെങ്കിൽ എങ്കിൽ (req_dir == _DIO_OUTPUT) { 
      തിരഞ്ഞെടുപ്പ് (req_type) {
      അവസ്ഥ _DIO_BYTE: phys_outsb(port, vir_buf, എണ്ണം); നിർത്തുക; 
      അവസ്ഥ _DIO_WORD: phys_outsw(port, vir_buf, എണ്ണം); നിർത്തുക; 
      സ്ഥിരസ്ഥിതി:
  		retval = EINVAL;
		പോകുക return_error;
      } 
  }
  അല്ലെങ്കിൽ {
	  retval = EINVAL;
	  പോകുക return_error;
  }
  retval = OK;

return_error:
  /* switch back to the address of the process which made the call */
  switch_address_space(caller);
  തിരികെ retval;
}

#endif /* USE_SDEVIO */
