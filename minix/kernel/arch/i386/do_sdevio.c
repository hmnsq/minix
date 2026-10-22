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
पूर्णाङ्क do_sdevio(संरचना proc * caller, सन्देशः *m_ptr)
{
  vir_अष्टकानि newoffset;
  endpoint_t newep;
  पूर्णाङ्क proc_nr;
  endpoint_t proc_nr_e = m_ptr->m_lsys_krn_sys_sdevio.vec_endpt;
  vir_अष्टकानि गणनम् = m_ptr->m_lsys_krn_sys_sdevio.vec_आकार;
  दीर्घ पत्तनम् = m_ptr->m_lsys_krn_sys_sdevio.पत्तनम्;
  phys_अष्टकानि vir_buf;
  पूर्णाङ्क i, r, req_type, req_dir, आकार, nr_io_range;
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
		प्रारूप_लिख("do_sdevio: for %d, req %d\n",
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
		प्रत्यागम(EINVAL);
  यदि (iskerneln(proc_nr)) प्रत्यागम(EPERM);

  /* Extract direction (in or out) and type (size). */
  req_dir = m_ptr->m_lsys_krn_sys_sdevio.request & _DIO_DIRMASK;
  req_type = m_ptr->m_lsys_krn_sys_sdevio.request & _DIO_TYPEMASK;

  /* Check for 'safe' variants. */
  यदि((m_ptr->m_lsys_krn_sys_sdevio.request & _DIO_SAFEMASK) == _DIO_SAFE) {
     /* Map grant address to physical address. */
     यदि((r=verify_grant(proc_nr_e, caller->p_endpoint,
		m_ptr->m_lsys_krn_sys_sdevio.vec_addr, गणनम्,
		req_dir == DIO_निवेशः ? CPF_WRITE : CPF_READ,
		m_ptr->m_lsys_krn_sys_sdevio.विस्थापनम्, &newoffset, &newep,
		रिक्त)) != OK) {
	यदि(r == ENOTREADY) प्रत्यागम r;
	प्रारूप_लिख("do_sdevio: verify_grant failed\n");
	प्रत्यागम EPERM;
    }
	यदि(!isokendpt(newep, &proc_nr))
		प्रत्यागम(EINVAL);
     destproc = proc_addr(proc_nr);
     vir_buf = newoffset;
  } अन्यथा {
     यदि(proc_nr != _ENDPOINT_P(caller->p_endpoint))
     {
	प्रारूप_लिख("do_sdevio: unsafe sdevio by %d in %d denied\n",
		caller->p_endpoint, proc_nr_e);
	प्रत्यागम EPERM;
     }
     /* Get and check physical address. */
     vir_buf = m_ptr->m_lsys_krn_sys_sdevio.vec_addr;
     destproc = proc_addr(proc_nr);
  }
     /* current process must be target for phys_* to be OK */

  switch_स्थानाङ्कः_space(destproc);

  विकल्प (req_type)
  {
	पक्ष _DIO_BYTE: आकार= 1; भङ्ग;
	पक्ष _DIO_WORD: आकार= 2; भङ्ग;
	पक्ष _DIO_LONG: आकार= 4; भङ्ग;
	स्वभाव: आकार= 4; भङ्ग;	/* Be conservative */
  }

  privp= priv(caller);
  यदि (privp && privp->s_flags & CHECK_IO_पत्तनम्)
  {
	पत्तनम्= m_ptr->m_lsys_krn_sys_sdevio.पत्तनम्;
	nr_io_range= privp->s_nr_io_range;
	प्रति (i= 0, iorp= privp->s_io_tab; i<nr_io_range; i++, iorp++)
	{
		यदि (पत्तनम् >= iorp->ior_आधारः && पत्तनम्+आकार-1 <= iorp->ior_सीमा)
			भङ्ग;
	}
	यदि (i >= nr_io_range)
	{
		प्रारूप_लिख(
		"do_sdevio: I/O port check failed for proc %d, port 0x%x\n",
			m_ptr->m_source, पत्तनम्);
		retval = EPERM;
		गच्छ return_error;
	}
  }

  यदि (पत्तनम् & (आकार-1))
  {
	प्रारूप_लिख("do_devio: unaligned port 0x%x (size %d)\n", पत्तनम्, आकार);
	retval = EPERM;
	गच्छ return_error;
  }

  /* Perform device I/O for bytes and words. Longs are not supported. */
  यदि (req_dir == DIO_निवेशः) { 
      विकल्प (req_type) {
      पक्ष _DIO_BYTE: phys_insb(पत्तनम्, vir_buf, गणनम्); भङ्ग; 
      पक्ष _DIO_WORD: phys_insw(पत्तनम्, vir_buf, गणनम्); भङ्ग; 
      स्वभाव:
  		retval = EINVAL;
		गच्छ return_error;
      } 
  } अन्यथा यदि (req_dir == _DIO_OUTPUT) { 
      विकल्प (req_type) {
      पक्ष _DIO_BYTE: phys_outsb(पत्तनम्, vir_buf, गणनम्); भङ्ग; 
      पक्ष _DIO_WORD: phys_outsw(पत्तनम्, vir_buf, गणनम्); भङ्ग; 
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
  switch_स्थानाङ्कः_space(caller);
  प्रत्यागम retval;
}

#यदि_अन्तः /* USE_SDEVIO */
