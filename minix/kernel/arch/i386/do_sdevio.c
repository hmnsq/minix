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

#شامل "kernel/system.h"
#شامل <minix/devio.h>
#شامل <minix/endpoint.h>

#شامل "arch_proto.h"

#اگر USE_SDEVIO

/*===========================================================================*
 *			        do_sdevio                                    *
 *===========================================================================*/
صحیح do_sdevio(ساختار proc * caller, message *m_ptr)
{
  vir_bytes newoffset;
  endpoint_t newep;
  صحیح proc_nr;
  endpoint_t proc_nr_e = m_ptr->m_lsys_krn_sys_sdevio.vec_endpt;
  vir_bytes شمار = m_ptr->m_lsys_krn_sys_sdevio.vec_size;
  long port = m_ptr->m_lsys_krn_sys_sdevio.port;
  phys_bytes vir_buf;
  صحیح i, r, req_type, req_dir, size, nr_io_range;
  ساختار priv *privp;
  ساختار io_range *iorp;
  ساختار proc *destproc;
  صحیح retval;

  /* Allow safe copies and accesses to SELF */
  اگر ((m_ptr->m_lsys_krn_sys_sdevio.request & _DIO_SAFEMASK) != _DIO_SAFE &&
	proc_nr_e != SELF)
  {
	ایستا صحیح first= 1;
	اگر (first)
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
  اگر (proc_nr_e == SELF)
	okendpt(caller->p_endpoint, &proc_nr);
  وگرنه
	اگر(!isokendpt(proc_nr_e, &proc_nr))
		بازگشت(EINVAL);
  اگر (iskerneln(proc_nr)) بازگشت(EPERM);

  /* Extract direction (in or out) and type (size). */
  req_dir = m_ptr->m_lsys_krn_sys_sdevio.request & _DIO_DIRMASK;
  req_type = m_ptr->m_lsys_krn_sys_sdevio.request & _DIO_TYPEMASK;

  /* Check for 'safe' variants. */
  اگر((m_ptr->m_lsys_krn_sys_sdevio.request & _DIO_SAFEMASK) == _DIO_SAFE) {
     /* Map grant address to physical address. */
     اگر((r=verify_grant(proc_nr_e, caller->p_endpoint,
		m_ptr->m_lsys_krn_sys_sdevio.vec_addr, شمار,
		req_dir == DIO_درون_داد ? CPF_WRITE : CPF_READ,
		m_ptr->m_lsys_krn_sys_sdevio.offset, &newoffset, &newep,
		پوچ)) != OK) {
	اگر(r == ENOTREADY) بازگشت r;
	printf("do_sdevio: verify_grant failed\n");
	بازگشت EPERM;
    }
	اگر(!isokendpt(newep, &proc_nr))
		بازگشت(EINVAL);
     destproc = proc_addr(proc_nr);
     vir_buf = newoffset;
  } وگرنه {
     اگر(proc_nr != _ENDPOINT_P(caller->p_endpoint))
     {
	printf("do_sdevio: unsafe sdevio by %d in %d denied\n",
		caller->p_endpoint, proc_nr_e);
	بازگشت EPERM;
     }
     /* Get and check physical address. */
     vir_buf = m_ptr->m_lsys_krn_sys_sdevio.vec_addr;
     destproc = proc_addr(proc_nr);
  }
     /* current process must be target for phys_* to be OK */

  switch_address_space(destproc);

  انتخاب (req_type)
  {
	حالت _DIO_BYTE: size= 1; بشکن;
	حالت _DIO_WORD: size= 2; بشکن;
	حالت _DIO_LONG: size= 4; بشکن;
	پیشفرض: size= 4; بشکن;	/* Be conservative */
  }

  privp= priv(caller);
  اگر (privp && privp->s_flags & CHECK_IO_PORT)
  {
	port= m_ptr->m_lsys_krn_sys_sdevio.port;
	nr_io_range= privp->s_nr_io_range;
	برای (i= 0, iorp= privp->s_io_tab; i<nr_io_range; i++, iorp++)
	{
		اگر (port >= iorp->ior_base && port+size-1 <= iorp->ior_limit)
			بشکن;
	}
	اگر (i >= nr_io_range)
	{
		printf(
		"do_sdevio: I/O port check failed for proc %d, port 0x%x\n",
			m_ptr->m_source, port);
		retval = EPERM;
		برو return_error;
	}
  }

  اگر (port & (size-1))
  {
	printf("do_devio: unaligned port 0x%x (size %d)\n", port, size);
	retval = EPERM;
	برو return_error;
  }

  /* Perform device I/O for bytes and words. Longs are not supported. */
  اگر (req_dir == DIO_درون_داد) { 
      انتخاب (req_type) {
      حالت _DIO_BYTE: phys_insb(port, vir_buf, شمار); بشکن; 
      حالت _DIO_WORD: phys_insw(port, vir_buf, شمار); بشکن; 
      پیشفرض:
  		retval = EINVAL;
		برو return_error;
      } 
  } وگرنه اگر (req_dir == _DIO_OUTPUT) { 
      انتخاب (req_type) {
      حالت _DIO_BYTE: phys_outsb(port, vir_buf, شمار); بشکن; 
      حالت _DIO_WORD: phys_outsw(port, vir_buf, شمار); بشکن; 
      پیشفرض:
  		retval = EINVAL;
		برو return_error;
      } 
  }
  وگرنه {
	  retval = EINVAL;
	  برو return_error;
  }
  retval = OK;

return_error:
  /* switch back to the address of the process which made the call */
  switch_address_space(caller);
  بازگشت retval;
}

#endif /* USE_SDEVIO */
