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

#ضمن "kernel/system.h"
#ضمن <minix/devio.h>
#ضمن <minix/endpoint.h>

#ضمن "arch_proto.h"

#ܐܢ USE_SDEVIO

/*===========================================================================*
 *			        do_sdevio                                    *
 *===========================================================================*/
صحيح do_sdevio(بنية proc * caller, message *m_ptr)
{
  vir_bytes newoffset;
  endpoint_t newep;
  صحيح proc_nr;
  endpoint_t proc_nr_e = m_ptr->m_lsys_krn_sys_sdevio.vec_endpt;
  vir_bytes count = m_ptr->m_lsys_krn_sys_sdevio.vec_size;
  long port = m_ptr->m_lsys_krn_sys_sdevio.port;
  phys_bytes vir_buf;
  صحيح i, r, req_type, req_dir, size, nr_io_range;
  بنية priv *privp;
  بنية io_range *iorp;
  بنية proc *destproc;
  صحيح retval;

  /* Allow safe copies and accesses to SELF */
  ܐܢ ((m_ptr->m_lsys_krn_sys_sdevio.request & _DIO_SAFEMASK) != _DIO_SAFE &&
	proc_nr_e != SELF)
  {
	ساكن صحيح first= 1;
	ܐܢ (first)
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
  ܐܢ (proc_nr_e == SELF)
	okendpt(caller->p_endpoint, &proc_nr);
  ܐܚܪܢܐ
	ܐܢ(!isokendpt(proc_nr_e, &proc_nr))
		ܦܢܐ(EINVAL);
  ܐܢ (iskerneln(proc_nr)) ܦܢܐ(EPERM);

  /* Extract direction (in or out) and type (size). */
  req_dir = m_ptr->m_lsys_krn_sys_sdevio.request & _DIO_DIRMASK;
  req_type = m_ptr->m_lsys_krn_sys_sdevio.request & _DIO_TYPEMASK;

  /* Check for 'safe' variants. */
  ܐܢ((m_ptr->m_lsys_krn_sys_sdevio.request & _DIO_SAFEMASK) == _DIO_SAFE) {
     /* Map grant address to physical address. */
     ܐܢ((r=verify_grant(proc_nr_e, caller->p_endpoint,
		m_ptr->m_lsys_krn_sys_sdevio.vec_addr, count,
		req_dir == _DIO_INPUT ? CPF_WRITE : CPF_READ,
		m_ptr->m_lsys_krn_sys_sdevio.offset, &newoffset, &newep,
		عدم)) != OK) {
	ܐܢ(r == ENOTREADY) ܦܢܐ r;
	printf("do_sdevio: verify_grant failed\n");
	ܦܢܐ EPERM;
    }
	ܐܢ(!isokendpt(newep, &proc_nr))
		ܦܢܐ(EINVAL);
     destproc = proc_addr(proc_nr);
     vir_buf = newoffset;
  } ܐܚܪܢܐ {
     ܐܢ(proc_nr != _ENDPOINT_P(caller->p_endpoint))
     {
	printf("do_sdevio: unsafe sdevio by %d in %d denied\n",
		caller->p_endpoint, proc_nr_e);
	ܦܢܐ EPERM;
     }
     /* Get and check physical address. */
     vir_buf = m_ptr->m_lsys_krn_sys_sdevio.vec_addr;
     destproc = proc_addr(proc_nr);
  }
     /* current process must be target for phys_* to be OK */

  switch_address_space(destproc);

  بدل (req_type)
  {
	حالة _DIO_BYTE: size= 1; اكسر;
	حالة _DIO_WORD: size= 2; اكسر;
	حالة _DIO_LONG: size= 4; اكسر;
	افتراضي: size= 4; اكسر;	/* Be conservative */
  }

  privp= priv(caller);
  ܐܢ (privp && privp->s_flags & CHECK_IO_PORT)
  {
	port= m_ptr->m_lsys_krn_sys_sdevio.port;
	nr_io_range= privp->s_nr_io_range;
	لكل (i= 0, iorp= privp->s_io_tab; i<nr_io_range; i++, iorp++)
	{
		ܐܢ (port >= iorp->ior_base && port+size-1 <= iorp->ior_limit)
			اكسر;
	}
	ܐܢ (i >= nr_io_range)
	{
		printf(
		"do_sdevio: I/O port check failed for proc %d, port 0x%x\n",
			m_ptr->m_source, port);
		retval = EPERM;
		اذهب return_error;
	}
  }

  ܐܢ (port & (size-1))
  {
	printf("do_devio: unaligned port 0x%x (size %d)\n", port, size);
	retval = EPERM;
	اذهب return_error;
  }

  /* Perform device I/O for bytes and words. Longs are not supported. */
  ܐܢ (req_dir == _DIO_INPUT) { 
      بدل (req_type) {
      حالة _DIO_BYTE: phys_insb(port, vir_buf, count); اكسر; 
      حالة _DIO_WORD: phys_insw(port, vir_buf, count); اكسر; 
      افتراضي:
  		retval = EINVAL;
		اذهب return_error;
      } 
  } ܐܚܪܢܐ ܐܢ (req_dir == _DIO_OUTPUT) { 
      بدل (req_type) {
      حالة _DIO_BYTE: phys_outsb(port, vir_buf, count); اكسر; 
      حالة _DIO_WORD: phys_outsw(port, vir_buf, count); اكسر; 
      افتراضي:
  		retval = EINVAL;
		اذهب return_error;
      } 
  }
  ܐܚܪܢܐ {
	  retval = EINVAL;
	  اذهب return_error;
  }
  retval = OK;

return_error:
  /* switch back to the address of the process which made the call */
  switch_address_space(caller);
  ܦܢܐ retval;
}

#endif /* USE_SDEVIO */
