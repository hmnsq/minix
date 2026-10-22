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

#অন্তর্ভাব "kernel/system.h"
#অন্তর্ভাব <minix/devio.h>
#অন্তর্ভাব <minix/endpoint.h>

#অন্তর্ভাব "arch_proto.h"

#যদি USE_SDEVIO

/*===========================================================================*
 *			        do_sdevio                                    *
 *===========================================================================*/
পূর্ণসংখ্যা do_sdevio(কাঠামো proc * caller, message *m_ptr)
{
  vir_bytes newoffset;
  endpoint_t newep;
  পূর্ণসংখ্যা proc_nr;
  endpoint_t proc_nr_e = m_ptr->m_lsys_krn_sys_sdevio.vec_endpt;
  vir_bytes গুনতি = m_ptr->m_lsys_krn_sys_sdevio.vec_আকার;
  long port = m_ptr->m_lsys_krn_sys_sdevio.port;
  phys_bytes vir_buf;
  পূর্ণসংখ্যা i, r, req_type, req_dir, আকার_2, nr_io_range;
  কাঠামো priv *privp;
  কাঠামো io_range *iorp;
  কাঠামো proc *destproc;
  পূর্ণসংখ্যা retval;

  /* Allow safe copies and accesses to SELF */
  যদি ((m_ptr->m_lsys_krn_sys_sdevio.request & _DIO_SAFEMASK) != _DIO_SAFE &&
	proc_nr_e != SELF)
  {
	স্থির পূর্ণসংখ্যা first= 1;
	যদি (first)
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
  যদি (proc_nr_e == SELF)
	okendpt(caller->p_endpoint, &proc_nr);
  অন্যথা
	যদি(!isokendpt(proc_nr_e, &proc_nr))
		ফেরত(EINVAL);
  যদি (iskerneln(proc_nr)) ফেরত(EPERM);

  /* Extract direction (in or out) and type (size). */
  req_dir = m_ptr->m_lsys_krn_sys_sdevio.request & _DIO_DIRMASK;
  req_type = m_ptr->m_lsys_krn_sys_sdevio.request & _DIO_TYPEMASK;

  /* Check for 'safe' variants. */
  যদি((m_ptr->m_lsys_krn_sys_sdevio.request & _DIO_SAFEMASK) == _DIO_SAFE) {
     /* Map grant address to physical address. */
     যদি((r=verify_grant(proc_nr_e, caller->p_endpoint,
		m_ptr->m_lsys_krn_sys_sdevio.vec_addr, গুনতি,
		req_dir == DIO_প্রবেশ ? CPF_WRITE : CPF_READ,
		m_ptr->m_lsys_krn_sys_sdevio.offset, &newoffset, &newep,
		ফাঁকা)) != OK) {
	যদি(r == ENOTREADY) ফেরত r;
	printf("do_sdevio: verify_grant failed\n");
	ফেরত EPERM;
    }
	যদি(!isokendpt(newep, &proc_nr))
		ফেরত(EINVAL);
     destproc = proc_addr(proc_nr);
     vir_buf = newoffset;
  } অন্যথা {
     যদি(proc_nr != _ENDPOINT_P(caller->p_endpoint))
     {
	printf("do_sdevio: unsafe sdevio by %d in %d denied\n",
		caller->p_endpoint, proc_nr_e);
	ফেরত EPERM;
     }
     /* Get and check physical address. */
     vir_buf = m_ptr->m_lsys_krn_sys_sdevio.vec_addr;
     destproc = proc_addr(proc_nr);
  }
     /* current process must be target for phys_* to be OK */

  switch_address_space(destproc);

  নির্বাচন (req_type)
  {
	ক্ষেত্র _DIO_BYTE: আকার_2= 1; বিরতি;
	ক্ষেত্র _DIO_WORD: আকার_2= 2; বিরতি;
	ক্ষেত্র _DIO_LONG: আকার_2= 4; বিরতি;
	পূর্বনির্ধারিত: আকার_2= 4; বিরতি;	/* Be conservative */
  }

  privp= priv(caller);
  যদি (privp && privp->s_flags & CHECK_IO_PORT)
  {
	port= m_ptr->m_lsys_krn_sys_sdevio.port;
	nr_io_range= privp->s_nr_io_range;
	জন্য (i= 0, iorp= privp->s_io_tab; i<nr_io_range; i++, iorp++)
	{
		যদি (port >= iorp->ior_base && port+আকার_2-1 <= iorp->ior_limit)
			বিরতি;
	}
	যদি (i >= nr_io_range)
	{
		printf(
		"do_sdevio: I/O port check failed for proc %d, port 0x%x\n",
			m_ptr->m_source, port);
		retval = EPERM;
		যাও return_error;
	}
  }

  যদি (port & (আকার_2-1))
  {
	printf("do_devio: unaligned port 0x%x (size %d)\n", port, আকার_2);
	retval = EPERM;
	যাও return_error;
  }

  /* Perform device I/O for bytes and words. Longs are not supported. */
  যদি (req_dir == DIO_প্রবেশ) { 
      নির্বাচন (req_type) {
      ক্ষেত্র _DIO_BYTE: phys_insb(port, vir_buf, গুনতি); বিরতি; 
      ক্ষেত্র _DIO_WORD: phys_insw(port, vir_buf, গুনতি); বিরতি; 
      পূর্বনির্ধারিত:
  		retval = EINVAL;
		যাও return_error;
      } 
  } অন্যথা যদি (req_dir == _DIO_OUTPUT) { 
      নির্বাচন (req_type) {
      ক্ষেত্র _DIO_BYTE: phys_outsb(port, vir_buf, গুনতি); বিরতি; 
      ক্ষেত্র _DIO_WORD: phys_outsw(port, vir_buf, গুনতি); বিরতি; 
      পূর্বনির্ধারিত:
  		retval = EINVAL;
		যাও return_error;
      } 
  }
  অন্যথা {
	  retval = EINVAL;
	  যাও return_error;
  }
  retval = OK;

return_error:
  /* switch back to the address of the process which made the call */
  switch_address_space(caller);
  ফেরত retval;
}

#endif /* USE_SDEVIO */
