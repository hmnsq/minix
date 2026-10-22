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

#ಅನ್ತರ್ಭಾವ "kernel/system.h"
#ಅನ್ತರ್ಭಾವ <minix/devio.h>
#ಅನ್ತರ್ಭಾವ <minix/endpoint.h>

#ಅನ್ತರ್ಭಾವ "arch_proto.h"

#ಯದಿ USE_SDEVIO

/*===========================================================================*
 *			        do_sdevio                                    *
 *===========================================================================*/
ಪೂರ್ಣಾಙ್ಕ do_sdevio(ಸಂರಚನಾ proc * caller, message *m_ptr)
{
  vir_bytes newoffset;
  endpoint_t newep;
  ಪೂರ್ಣಾಙ್ಕ proc_nr;
  endpoint_t proc_nr_e = m_ptr->m_lsys_krn_sys_sdevio.vec_endpt;
  vir_bytes ಎಣಿಕೆ = m_ptr->m_lsys_krn_sys_sdevio.vec_size;
  long port = m_ptr->m_lsys_krn_sys_sdevio.port;
  phys_bytes vir_buf;
  ಪೂರ್ಣಾಙ್ಕ i, r, req_type, req_dir, size, nr_io_range;
  ಸಂರಚನಾ priv *privp;
  ಸಂರಚನಾ io_range *iorp;
  ಸಂರಚನಾ proc *destproc;
  ಪೂರ್ಣಾಙ್ಕ retval;

  /* Allow safe copies and accesses to SELF */
  ಯದಿ ((m_ptr->m_lsys_krn_sys_sdevio.request & _DIO_SAFEMASK) != _DIO_SAFE &&
	proc_nr_e != SELF)
  {
	ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ first= 1;
	ಯದಿ (first)
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
  ಯದಿ (proc_nr_e == SELF)
	okendpt(caller->p_endpoint, &proc_nr);
  ಅನ್ಯಥಾ
	ಯದಿ(!isokendpt(proc_nr_e, &proc_nr))
		ಪ್ರತ್ಯಾಗಮ(EINVAL);
  ಯದಿ (iskerneln(proc_nr)) ಪ್ರತ್ಯಾಗಮ(EPERM);

  /* Extract direction (in or out) and type (size). */
  req_dir = m_ptr->m_lsys_krn_sys_sdevio.request & _DIO_DIRMASK;
  req_type = m_ptr->m_lsys_krn_sys_sdevio.request & _DIO_TYPEMASK;

  /* Check for 'safe' variants. */
  ಯದಿ((m_ptr->m_lsys_krn_sys_sdevio.request & _DIO_SAFEMASK) == _DIO_SAFE) {
     /* Map grant address to physical address. */
     ಯದಿ((r=verify_grant(proc_nr_e, caller->p_endpoint,
		m_ptr->m_lsys_krn_sys_sdevio.vec_addr, ಎಣಿಕೆ,
		req_dir == DIO_ಒಳಹರಿವು ? CPF_WRITE : CPF_READ,
		m_ptr->m_lsys_krn_sys_sdevio.offset, &newoffset, &newep,
		ರಿಕ್ತ)) != OK) {
	ಯದಿ(r == ENOTREADY) ಪ್ರತ್ಯಾಗಮ r;
	printf("do_sdevio: verify_grant failed\n");
	ಪ್ರತ್ಯಾಗಮ EPERM;
    }
	ಯದಿ(!isokendpt(newep, &proc_nr))
		ಪ್ರತ್ಯಾಗಮ(EINVAL);
     destproc = proc_addr(proc_nr);
     vir_buf = newoffset;
  } ಅನ್ಯಥಾ {
     ಯದಿ(proc_nr != _ENDPOINT_P(caller->p_endpoint))
     {
	printf("do_sdevio: unsafe sdevio by %d in %d denied\n",
		caller->p_endpoint, proc_nr_e);
	ಪ್ರತ್ಯಾಗಮ EPERM;
     }
     /* Get and check physical address. */
     vir_buf = m_ptr->m_lsys_krn_sys_sdevio.vec_addr;
     destproc = proc_addr(proc_nr);
  }
     /* current process must be target for phys_* to be OK */

  switch_address_space(destproc);

  ವಿಕಲ್ಪ (req_type)
  {
	ಪಕ್ಷ _DIO_BYTE: size= 1; ಭಙ್ಗ;
	ಪಕ್ಷ _DIO_WORD: size= 2; ಭಙ್ಗ;
	ಪಕ್ಷ _DIO_LONG: size= 4; ಭಙ್ಗ;
	ಸ್ವಭಾವ: size= 4; ಭಙ್ಗ;	/* Be conservative */
  }

  privp= priv(caller);
  ಯದಿ (privp && privp->s_flags & CHECK_IO_PORT)
  {
	port= m_ptr->m_lsys_krn_sys_sdevio.port;
	nr_io_range= privp->s_nr_io_range;
	ಪ್ರತಿ (i= 0, iorp= privp->s_io_tab; i<nr_io_range; i++, iorp++)
	{
		ಯದಿ (port >= iorp->ior_base && port+size-1 <= iorp->ior_limit)
			ಭಙ್ಗ;
	}
	ಯದಿ (i >= nr_io_range)
	{
		printf(
		"do_sdevio: I/O port check failed for proc %d, port 0x%x\n",
			m_ptr->m_source, port);
		retval = EPERM;
		ಗಚ್ಛ return_error;
	}
  }

  ಯದಿ (port & (size-1))
  {
	printf("do_devio: unaligned port 0x%x (size %d)\n", port, size);
	retval = EPERM;
	ಗಚ್ಛ return_error;
  }

  /* Perform device I/O for bytes and words. Longs are not supported. */
  ಯದಿ (req_dir == DIO_ಒಳಹರಿವು) { 
      ವಿಕಲ್ಪ (req_type) {
      ಪಕ್ಷ _DIO_BYTE: phys_insb(port, vir_buf, ಎಣಿಕೆ); ಭಙ್ಗ; 
      ಪಕ್ಷ _DIO_WORD: phys_insw(port, vir_buf, ಎಣಿಕೆ); ಭಙ್ಗ; 
      ಸ್ವಭಾವ:
  		retval = EINVAL;
		ಗಚ್ಛ return_error;
      } 
  } ಅನ್ಯಥಾ ಯದಿ (req_dir == _DIO_OUTPUT) { 
      ವಿಕಲ್ಪ (req_type) {
      ಪಕ್ಷ _DIO_BYTE: phys_outsb(port, vir_buf, ಎಣಿಕೆ); ಭಙ್ಗ; 
      ಪಕ್ಷ _DIO_WORD: phys_outsw(port, vir_buf, ಎಣಿಕೆ); ಭಙ್ಗ; 
      ಸ್ವಭಾವ:
  		retval = EINVAL;
		ಗಚ್ಛ return_error;
      } 
  }
  ಅನ್ಯಥಾ {
	  retval = EINVAL;
	  ಗಚ್ಛ return_error;
  }
  retval = OK;

return_error:
  /* switch back to the address of the process which made the call */
  switch_address_space(caller);
  ಪ್ರತ್ಯಾಗಮ retval;
}

#endif /* USE_SDEVIO */
