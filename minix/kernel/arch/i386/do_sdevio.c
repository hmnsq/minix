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

#जइ USE_SDEVIO

/*===========================================================================*
 *			        do_sdevio                                    *
 *===========================================================================*/
पूर्णाङ्क do_sdevio(संरचना proc * caller, message *m_ptr)
{
  vir_bytes newoffset;
  endpoint_t newep;
  पूर्णाङ्क proc_nr;
  endpoint_t proc_nr_e = m_ptr->m_lsys_krn_sys_sdevio.vec_endpt;
  vir_bytes गणनम् = m_ptr->m_lsys_krn_sys_sdevio.vec_size;
  long port = m_ptr->m_lsys_krn_sys_sdevio.port;
  phys_bytes vir_buf;
  पूर्णाङ्क i, r, req_type, req_dir, size, nr_io_range;
  संरचना priv *privp;
  संरचना io_range *iorp;
  संरचना proc *destproc;
  पूर्णाङ्क retval;

  /* Allow safe copies and accesses to SELF */
  जइ ((m_ptr->m_lsys_krn_sys_sdevio.request & _DIO_SAFEMASK) != _DIO_SAFE &&
	proc_nr_e != SELF)
  {
	स्थिर पूर्णाङ्क first= 1;
	जइ (first)
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
  जइ (proc_nr_e == SELF)
	okendpt(caller->p_endpoint, &proc_nr);
  अण्णहा
	जइ(!isokendpt(proc_nr_e, &proc_nr))
		प्रत्यागम(EINVAL);
  जइ (iskerneln(proc_nr)) प्रत्यागम(EPERM);

  /* Extract direction (in or out) and type (size). */
  req_dir = m_ptr->m_lsys_krn_sys_sdevio.request & _DIO_DIRMASK;
  req_type = m_ptr->m_lsys_krn_sys_sdevio.request & _DIO_TYPEMASK;

  /* Check for 'safe' variants. */
  जइ((m_ptr->m_lsys_krn_sys_sdevio.request & _DIO_SAFEMASK) == _DIO_SAFE) {
     /* Map grant address to physical address. */
     जइ((r=verify_grant(proc_nr_e, caller->p_endpoint,
		m_ptr->m_lsys_krn_sys_sdevio.vec_addr, गणनम्,
		req_dir == DIO_निवेशः ? CPF_WRITE : CPF_READ,
		m_ptr->m_lsys_krn_sys_sdevio.offset, &newoffset, &newep,
		रिक्त)) != OK) {
	जइ(r == ENOTREADY) प्रत्यागम r;
	printf("do_sdevio: verify_grant failed\n");
	प्रत्यागम EPERM;
    }
	जइ(!isokendpt(newep, &proc_nr))
		प्रत्यागम(EINVAL);
     destproc = proc_addr(proc_nr);
     vir_buf = newoffset;
  } अण्णहा {
     जइ(proc_nr != _ENDPOINT_P(caller->p_endpoint))
     {
	printf("do_sdevio: unsafe sdevio by %d in %d denied\n",
		caller->p_endpoint, proc_nr_e);
	प्रत्यागम EPERM;
     }
     /* Get and check physical address. */
     vir_buf = m_ptr->m_lsys_krn_sys_sdevio.vec_addr;
     destproc = proc_addr(proc_nr);
  }
     /* current process must be target for phys_* to be OK */

  switch_address_space(destproc);

  विकल्प (req_type)
  {
	पक्ष _DIO_BYTE: size= 1; भङ्ग;
	पक्ष _DIO_WORD: size= 2; भङ्ग;
	पक्ष _DIO_LONG: size= 4; भङ्ग;
	स्वभाव: size= 4; भङ्ग;	/* Be conservative */
  }

  privp= priv(caller);
  जइ (privp && privp->s_flags & CHECK_IO_PORT)
  {
	port= m_ptr->m_lsys_krn_sys_sdevio.port;
	nr_io_range= privp->s_nr_io_range;
	प्रति (i= 0, iorp= privp->s_io_tab; i<nr_io_range; i++, iorp++)
	{
		जइ (port >= iorp->ior_base && port+size-1 <= iorp->ior_limit)
			भङ्ग;
	}
	जइ (i >= nr_io_range)
	{
		printf(
		"do_sdevio: I/O port check failed for proc %d, port 0x%x\n",
			m_ptr->m_source, port);
		retval = EPERM;
		गच्छ return_error;
	}
  }

  जइ (port & (size-1))
  {
	printf("do_devio: unaligned port 0x%x (size %d)\n", port, size);
	retval = EPERM;
	गच्छ return_error;
  }

  /* Perform device I/O for bytes and words. Longs are not supported. */
  जइ (req_dir == DIO_निवेशः) { 
      विकल्प (req_type) {
      पक्ष _DIO_BYTE: phys_insb(port, vir_buf, गणनम्); भङ्ग; 
      पक्ष _DIO_WORD: phys_insw(port, vir_buf, गणनम्); भङ्ग; 
      स्वभाव:
  		retval = EINVAL;
		गच्छ return_error;
      } 
  } अण्णहा जइ (req_dir == _DIO_OUTPUT) { 
      विकल्प (req_type) {
      पक्ष _DIO_BYTE: phys_outsb(port, vir_buf, गणनम्); भङ्ग; 
      पक्ष _DIO_WORD: phys_outsw(port, vir_buf, गणनम्); भङ्ग; 
      स्वभाव:
  		retval = EINVAL;
		गच्छ return_error;
      } 
  }
  अण्णहा {
	  retval = EINVAL;
	  गच्छ return_error;
  }
  retval = OK;

return_error:
  /* switch back to the address of the process which made the call */
  switch_address_space(caller);
  प्रत्यागम retval;
}

#endif /* USE_SDEVIO */
