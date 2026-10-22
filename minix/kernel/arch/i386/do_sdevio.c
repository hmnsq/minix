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

#অন্তর্ভাৱ "kernel/system.h"
#অন্তর্ভাৱ <minix/devio.h>
#অন্তর্ভাৱ <minix/endpoint.h>

#অন্তর্ভাৱ "arch_proto.h"

#যদি USE_SDEVIO

/*===========================================================================*
 *			        do_sdevio                                    *
 *===========================================================================*/
পূর্ণাঙ্ক do_sdevio(সংরচনা proc * caller, message *m_ptr)
{
  vir_bytes newoffset;
  endpoint_t newep;
  পূর্ণাঙ্ক proc_nr;
  endpoint_t proc_nr_e = m_ptr->m_lsys_krn_sys_sdevio.vec_endpt;
  vir_bytes গুনতি = m_ptr->m_lsys_krn_sys_sdevio.vec_size;
  long port = m_ptr->m_lsys_krn_sys_sdevio.port;
  phys_bytes vir_buf;
  পূর্ণাঙ্ক i, r, req_type, req_dir, size, nr_io_range;
  সংরচনা priv *privp;
  সংরচনা io_range *iorp;
  সংরচনা proc *destproc;
  পূর্ণাঙ্ক retval;

  /* Allow safe copies and accesses to SELF */
  যদি ((m_ptr->m_lsys_krn_sys_sdevio.request & _DIO_SAFEMASK) != _DIO_SAFE &&
	proc_nr_e != SELF)
  {
	স্থির পূর্ণাঙ্ক first= 1;
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
		ঘূৰাই(EINVAL);
  যদি (iskerneln(proc_nr)) ঘূৰাই(EPERM);

  /* Extract direction (in or out) and type (size). */
  req_dir = m_ptr->m_lsys_krn_sys_sdevio.request & _DIO_DIRMASK;
  req_type = m_ptr->m_lsys_krn_sys_sdevio.request & _DIO_TYPEMASK;

  /* Check for 'safe' variants. */
  যদি((m_ptr->m_lsys_krn_sys_sdevio.request & _DIO_SAFEMASK) == _DIO_SAFE) {
     /* Map grant address to physical address. */
     যদি((r=verify_grant(proc_nr_e, caller->p_endpoint,
		m_ptr->m_lsys_krn_sys_sdevio.vec_addr, গুনতি,
		req_dir == DIO_প্ৰবেশ ? CPF_WRITE : CPF_READ,
		m_ptr->m_lsys_krn_sys_sdevio.offset, &newoffset, &newep,
		রিক্ত)) != OK) {
	যদি(r == ENOTREADY) ঘূৰাই r;
	printf("do_sdevio: verify_grant failed\n");
	ঘূৰাই EPERM;
    }
	যদি(!isokendpt(newep, &proc_nr))
		ঘূৰাই(EINVAL);
     destproc = proc_addr(proc_nr);
     vir_buf = newoffset;
  } অন্যথা {
     যদি(proc_nr != _ENDPOINT_P(caller->p_endpoint))
     {
	printf("do_sdevio: unsafe sdevio by %d in %d denied\n",
		caller->p_endpoint, proc_nr_e);
	ঘূৰাই EPERM;
     }
     /* Get and check physical address. */
     vir_buf = m_ptr->m_lsys_krn_sys_sdevio.vec_addr;
     destproc = proc_addr(proc_nr);
  }
     /* current process must be target for phys_* to be OK */

  switch_address_space(destproc);

  ৱিকল্প (req_type)
  {
	পক্ষ _DIO_BYTE: size= 1; ভঙ্গ;
	পক্ষ _DIO_WORD: size= 2; ভঙ্গ;
	পক্ষ _DIO_LONG: size= 4; ভঙ্গ;
	স্ৱভাৱ: size= 4; ভঙ্গ;	/* Be conservative */
  }

  privp= priv(caller);
  যদি (privp && privp->s_flags & CHECK_IO_PORT)
  {
	port= m_ptr->m_lsys_krn_sys_sdevio.port;
	nr_io_range= privp->s_nr_io_range;
	বাবে (i= 0, iorp= privp->s_io_tab; i<nr_io_range; i++, iorp++)
	{
		যদি (port >= iorp->ior_base && port+size-1 <= iorp->ior_limit)
			ভঙ্গ;
	}
	যদি (i >= nr_io_range)
	{
		printf(
		"do_sdevio: I/O port check failed for proc %d, port 0x%x\n",
			m_ptr->m_source, port);
		retval = EPERM;
		গচ্ছ return_error;
	}
  }

  যদি (port & (size-1))
  {
	printf("do_devio: unaligned port 0x%x (size %d)\n", port, size);
	retval = EPERM;
	গচ্ছ return_error;
  }

  /* Perform device I/O for bytes and words. Longs are not supported. */
  যদি (req_dir == DIO_প্ৰবেশ) { 
      ৱিকল্প (req_type) {
      পক্ষ _DIO_BYTE: phys_insb(port, vir_buf, গুনতি); ভঙ্গ; 
      পক্ষ _DIO_WORD: phys_insw(port, vir_buf, গুনতি); ভঙ্গ; 
      স্ৱভাৱ:
  		retval = EINVAL;
		গচ্ছ return_error;
      } 
  } অন্যথা যদি (req_dir == _DIO_OUTPUT) { 
      ৱিকল্প (req_type) {
      পক্ষ _DIO_BYTE: phys_outsb(port, vir_buf, গুনতি); ভঙ্গ; 
      পক্ষ _DIO_WORD: phys_outsw(port, vir_buf, গুনতি); ভঙ্গ; 
      স্ৱভাৱ:
  		retval = EINVAL;
		গচ্ছ return_error;
      } 
  }
  অন্যথা {
	  retval = EINVAL;
	  গচ্ছ return_error;
  }
  retval = OK;

return_error:
  /* switch back to the address of the process which made the call */
  switch_address_space(caller);
  ঘূৰাই retval;
}

#endif /* USE_SDEVIO */
