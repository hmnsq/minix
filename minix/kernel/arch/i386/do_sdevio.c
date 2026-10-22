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

#अन्तर्भाव "kernel/system.h"
#अन्तर्भाव <minix/devio.h>
#अन्तर्भाव <minix/endpoint.h>

#अन्तर्भाव "arch_proto.h"

#यदि USE_SDEVIO

/*===========================================================================*
 *			        do_sdevio                                    *
 *===========================================================================*/
पूर्णाङ्क do_sdevio(संरचना proc * caller, message *m_ptr)
{
  vir_bytes newoffset;
  endpoint_t newep;
  पूर्णाङ्क proc_nr;
  endpoint_t proc_nr_e = m_ptr->m_lsys_krn_sys_sdevio.vec_endpt;
  vir_bytes गन्ती = m_ptr->m_lsys_krn_sys_sdevio.vec_size;
  दीर्घ port = m_ptr->m_lsys_krn_sys_sdevio.port;
  phys_bytes vir_buf;
  पूर्णाङ्क i, r, req_type, req_dir, size, nr_io_range;
  संरचना priv *privp;
  संरचना io_range *iorp;
  संरचना proc *destproc;
  पूर्णाङ्क retval;

  /* Allow safe copies and accesses to SELF */
  यदि ((m_ptr->m_lsys_krn_sys_sdevio.request & _DIO_SAFEMASK) != _DIO_SAFE &&
	proc_nr_e != SELF)
  {
	स्थिर पूर्णाङ्क first= 1;
	यदि (first)
	{
		first= 0;
		म_लिखो("do_sdevio: for %d, req %d\n",
			m_ptr->m_source, m_ptr->m_lsys_krn_sys_sdevio.request);
	}
  }

  /* Check if process endpoint is OK. 
   * A driver may directly provide a pointer to a buffer at the user-process
   * that initiated the device I/O. Kernel processes, of course, are denied.
   */
  यदि (proc_nr_e == SELF)
	okendpt(caller->p_endpoint, &proc_nr);
  अन्यथा
	यदि(!isokendpt(proc_nr_e, &proc_nr))
		फर्काऊ(EINVAL);
  यदि (iskerneln(proc_nr)) फर्काऊ(EPERM);

  /* Extract direction (in or out) and type (size). */
  req_dir = m_ptr->m_lsys_krn_sys_sdevio.request & _DIO_DIRMASK;
  req_type = m_ptr->m_lsys_krn_sys_sdevio.request & _DIO_TYPEMASK;

  /* Check for 'safe' variants. */
  यदि((m_ptr->m_lsys_krn_sys_sdevio.request & _DIO_SAFEMASK) == _DIO_SAFE) {
     /* Map grant address to physical address. */
     यदि((r=verify_grant(proc_nr_e, caller->p_endpoint,
		m_ptr->m_lsys_krn_sys_sdevio.vec_addr, गन्ती,
		req_dir == DIO_आगत ? CPF_WRITE : CPF_READ,
		m_ptr->m_lsys_krn_sys_sdevio.offset, &newoffset, &newep,
		रिक्त)) != OK) {
	यदि(r == ENOTREADY) फर्काऊ r;
	म_लिखो("do_sdevio: verify_grant failed\n");
	फर्काऊ EPERM;
    }
	यदि(!isokendpt(newep, &proc_nr))
		फर्काऊ(EINVAL);
     destproc = proc_addr(proc_nr);
     vir_buf = newoffset;
  } अन्यथा {
     यदि(proc_nr != _ENDPOINT_P(caller->p_endpoint))
     {
	म_लिखो("do_sdevio: unsafe sdevio by %d in %d denied\n",
		caller->p_endpoint, proc_nr_e);
	फर्काऊ EPERM;
     }
     /* Get and check physical address. */
     vir_buf = m_ptr->m_lsys_krn_sys_sdevio.vec_addr;
     destproc = proc_addr(proc_nr);
  }
     /* current process must be target for phys_* to be OK */

  switch_address_space(destproc);

  विकल्प (req_type)
  {
	पक्ष _DIO_BYTE: size= 1; रोक;
	पक्ष _DIO_WORD: size= 2; रोक;
	पक्ष _DIO_LONG: size= 4; रोक;
	स्वभाव: size= 4; रोक;	/* Be conservative */
  }

  privp= priv(caller);
  यदि (privp && privp->s_flags & CHECK_IO_PORT)
  {
	port= m_ptr->m_lsys_krn_sys_sdevio.port;
	nr_io_range= privp->s_nr_io_range;
	लागि (i= 0, iorp= privp->s_io_tab; i<nr_io_range; i++, iorp++)
	{
		यदि (port >= iorp->ior_base && port+size-1 <= iorp->ior_limit)
			रोक;
	}
	यदि (i >= nr_io_range)
	{
		म_लिखो(
		"do_sdevio: I/O port check failed for proc %d, port 0x%x\n",
			m_ptr->m_source, port);
		retval = EPERM;
		गच्छ return_error;
	}
  }

  यदि (port & (size-1))
  {
	म_लिखो("do_devio: unaligned port 0x%x (size %d)\n", port, size);
	retval = EPERM;
	गच्छ return_error;
  }

  /* Perform device I/O for bytes and words. Longs are not supported. */
  यदि (req_dir == DIO_आगत) { 
      विकल्प (req_type) {
      पक्ष _DIO_BYTE: phys_insb(port, vir_buf, गन्ती); रोक; 
      पक्ष _DIO_WORD: phys_insw(port, vir_buf, गन्ती); रोक; 
      स्वभाव:
  		retval = EINVAL;
		गच्छ return_error;
      } 
  } अन्यथा यदि (req_dir == _DIO_OUTPUT) { 
      विकल्प (req_type) {
      पक्ष _DIO_BYTE: phys_outsb(port, vir_buf, गन्ती); रोक; 
      पक्ष _DIO_WORD: phys_outsw(port, vir_buf, गन्ती); रोक; 
      स्वभाव:
  		retval = EINVAL;
		गच्छ return_error;
      } 
  }
  अन्यथा {
	  retval = EINVAL;
	  गच्छ return_error;
  }
  retval = OK;

return_error:
  /* switch back to the address of the process which made the call */
  switch_address_space(caller);
  फर्काऊ retval;
}

#endif /* USE_SDEVIO */
