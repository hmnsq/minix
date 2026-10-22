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

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/system.h"
#ਅਨ੍ਤਰ੍ਭਾਵ <minix/devio.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <minix/endpoint.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "arch_proto.h"

#ਜੇ USE_SDEVIO

/*===========================================================================*
 *			        do_sdevio                                    *
 *===========================================================================*/
ਪੂਰਨ do_sdevio(ਬਣਤਰ proc * caller, ਸੁਨੇਹਾ *m_ptr)
{
  vir_ਬਾਈਟਾਂ newoffset;
  endpoint_t newep;
  ਪੂਰਨ proc_nr;
  endpoint_t proc_nr_e = m_ptr->m_lsys_krn_sys_sdevio.vec_endpt;
  vir_ਬਾਈਟਾਂ ਗਿਣਤੀ_2 = m_ptr->m_lsys_krn_sys_sdevio.vec_ਮਾਪ;
  ਲੰਮਾ ਪੋਰਟ = m_ptr->m_lsys_krn_sys_sdevio.ਪੋਰਟ;
  phys_ਬਾਈਟਾਂ vir_buf;
  ਪੂਰਨ i, r, req_type, req_dir, ਮਾਪ, nr_io_range;
  ਬਣਤਰ priv *privp;
  ਬਣਤਰ io_range *iorp;
  ਬਣਤਰ proc *destproc;
  ਪੂਰਨ retval;

  /* Allow safe copies and accesses to SELF */
  ਜੇ ((m_ptr->m_lsys_krn_sys_sdevio.request & _DIO_SAFEMASK) != DIO_ਸੁਰੱਖਿਅਤ &&
	proc_nr_e != SELF)
  {
	ਸਥਿਰ ਪੂਰਨ ਪਹਿਲਾ= 1;
	ਜੇ (ਪਹਿਲਾ)
	{
		ਪਹਿਲਾ= 0;
		ਲਿਖੋ("do_sdevio: for %d, req %d\n",
			m_ptr->m_ਸਰੋਤ, m_ptr->m_lsys_krn_sys_sdevio.request);
	}
  }

  /* Check if process endpoint is OK. 
   * A driver may directly provide a pointer to a buffer at the user-process
   * that initiated the device I/O. Kernel processes, of course, are denied.
   */
  ਜੇ (proc_nr_e == SELF)
	okendpt(caller->p_endpoint, &proc_nr);
  ਨਹੀਂਤਾਂ
	ਜੇ(!isokendpt(proc_nr_e, &proc_nr))
		ਵਾਪਸ(EINVAL);
  ਜੇ (iskerneln(proc_nr)) ਵਾਪਸ(EPERM);

  /* Extract direction (in or out) and type (size). */
  req_dir = m_ptr->m_lsys_krn_sys_sdevio.request & _DIO_DIRMASK;
  req_type = m_ptr->m_lsys_krn_sys_sdevio.request & _DIO_TYPEMASK;

  /* Check for 'safe' variants. */
  ਜੇ((m_ptr->m_lsys_krn_sys_sdevio.request & _DIO_SAFEMASK) == DIO_ਸੁਰੱਖਿਅਤ) {
     /* Map grant address to physical address. */
     ਜੇ((r=verify_grant(proc_nr_e, caller->p_endpoint,
		m_ptr->m_lsys_krn_sys_sdevio.vec_addr, ਗਿਣਤੀ_2,
		req_dir == DIO_ਨਿਵੇਸ਼ ? CPF_WRITE : CPF_READ,
		m_ptr->m_lsys_krn_sys_sdevio.ਵਿੱਥ, &newoffset, &newep,
		ਨਿਰਾ)) != OK) {
	ਜੇ(r == ENOTREADY) ਵਾਪਸ r;
	ਲਿਖੋ("do_sdevio: verify_grant failed\n");
	ਵਾਪਸ EPERM;
    }
	ਜੇ(!isokendpt(newep, &proc_nr))
		ਵਾਪਸ(EINVAL);
     destproc = proc_addr(proc_nr);
     vir_buf = newoffset;
  } ਨਹੀਂਤਾਂ {
     ਜੇ(proc_nr != _ENDPOINT_P(caller->p_endpoint))
     {
	ਲਿਖੋ("do_sdevio: unsafe sdevio by %d in %d denied\n",
		caller->p_endpoint, proc_nr_e);
	ਵਾਪਸ EPERM;
     }
     /* Get and check physical address. */
     vir_buf = m_ptr->m_lsys_krn_sys_sdevio.vec_addr;
     destproc = proc_addr(proc_nr);
  }
     /* current process must be target for phys_* to be OK */

  switch_ਪਤਾ_space(destproc);

  ਚੋਣ (req_type)
  {
	ਹਾਲਤ _DIO_BYTE: ਮਾਪ= 1; ਰੋਕੋ;
	ਹਾਲਤ DIO_ਸ਼ਬਦ: ਮਾਪ= 2; ਰੋਕੋ;
	ਹਾਲਤ _DIO_LONG: ਮਾਪ= 4; ਰੋਕੋ;
	ਮੂਲ: ਮਾਪ= 4; ਰੋਕੋ;	/* Be conservative */
  }

  privp= priv(caller);
  ਜੇ (privp && privp->s_flags & CHECK_IO_ਪੋਰਟ)
  {
	ਪੋਰਟ= m_ptr->m_lsys_krn_sys_sdevio.ਪੋਰਟ;
	nr_io_range= privp->s_nr_io_range;
	ਲਈ (i= 0, iorp= privp->s_io_tab; i<nr_io_range; i++, iorp++)
	{
		ਜੇ (ਪੋਰਟ >= iorp->ior_ਅਧਾਰ && ਪੋਰਟ+ਮਾਪ-1 <= iorp->ior_ਹੱਦਬੰਦੀ)
			ਰੋਕੋ;
	}
	ਜੇ (i >= nr_io_range)
	{
		ਲਿਖੋ(
		"do_sdevio: I/O port check failed for proc %d, port 0x%x\n",
			m_ptr->m_ਸਰੋਤ, ਪੋਰਟ);
		retval = EPERM;
		ਜਾਓ return_error;
	}
  }

  ਜੇ (ਪੋਰਟ & (ਮਾਪ-1))
  {
	ਲਿਖੋ("do_devio: unaligned port 0x%x (size %d)\n", ਪੋਰਟ, ਮਾਪ);
	retval = EPERM;
	ਜਾਓ return_error;
  }

  /* Perform device I/O for bytes and words. Longs are not supported. */
  ਜੇ (req_dir == DIO_ਨਿਵੇਸ਼) { 
      ਚੋਣ (req_type) {
      ਹਾਲਤ _DIO_BYTE: phys_insb(ਪੋਰਟ, vir_buf, ਗਿਣਤੀ_2); ਰੋਕੋ; 
      ਹਾਲਤ DIO_ਸ਼ਬਦ: phys_insw(ਪੋਰਟ, vir_buf, ਗਿਣਤੀ_2); ਰੋਕੋ; 
      ਮੂਲ:
  		retval = EINVAL;
		ਜਾਓ return_error;
      } 
  } ਨਹੀਂਤਾਂ ਜੇ (req_dir == _DIO_OUTPUT) { 
      ਚੋਣ (req_type) {
      ਹਾਲਤ _DIO_BYTE: phys_outsb(ਪੋਰਟ, vir_buf, ਗਿਣਤੀ_2); ਰੋਕੋ; 
      ਹਾਲਤ DIO_ਸ਼ਬਦ: phys_outsw(ਪੋਰਟ, vir_buf, ਗਿਣਤੀ_2); ਰੋਕੋ; 
      ਮੂਲ:
  		retval = EINVAL;
		ਜਾਓ return_error;
      } 
  }
  ਨਹੀਂਤਾਂ {
	  retval = EINVAL;
	  ਜਾਓ return_error;
  }
  retval = OK;

return_error:
  /* switch back to the address of the process which made the call */
  switch_ਪਤਾ_space(caller);
  ਵਾਪਸ retval;
}

#ਜੇ_ਅੰਤ /* USE_SDEVIO */
