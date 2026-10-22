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

#அந்தர்பாவ "kernel/system.h"
#அந்தர்பாவ <minix/devio.h>
#அந்தர்பாவ <minix/endpoint.h>

#அந்தர்பாவ "arch_proto.h"

#எனில் USE_SDEVIO

/*===========================================================================*
 *			        do_sdevio                                    *
 *===========================================================================*/
முழு do_sdevio(கட்டமைப்பு proc * caller, message *m_ptr)
{
  vir_bytes newoffset;
  endpoint_t newep;
  முழு proc_nr;
  endpoint_t proc_nr_e = m_ptr->m_lsys_krn_sys_sdevio.vec_endpt;
  vir_bytes மொத்தம் = m_ptr->m_lsys_krn_sys_sdevio.vec_size;
  long port = m_ptr->m_lsys_krn_sys_sdevio.port;
  phys_bytes vir_buf;
  முழு i, r, req_type, req_dir, size, nr_io_range;
  கட்டமைப்பு priv *privp;
  கட்டமைப்பு io_range *iorp;
  கட்டமைப்பு proc *destproc;
  முழு retval;

  /* Allow safe copies and accesses to SELF */
  எனில் ((m_ptr->m_lsys_krn_sys_sdevio.request & _DIO_SAFEMASK) != _DIO_SAFE &&
	proc_nr_e != SELF)
  {
	நிலையான முழு first= 1;
	எனில் (first)
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
  எனில் (proc_nr_e == SELF)
	okendpt(caller->p_endpoint, &proc_nr);
  இல்லையெனில்
	எனில்(!isokendpt(proc_nr_e, &proc_nr))
		திரும்பு(EINVAL);
  எனில் (iskerneln(proc_nr)) திரும்பு(EPERM);

  /* Extract direction (in or out) and type (size). */
  req_dir = m_ptr->m_lsys_krn_sys_sdevio.request & _DIO_DIRMASK;
  req_type = m_ptr->m_lsys_krn_sys_sdevio.request & _DIO_TYPEMASK;

  /* Check for 'safe' variants. */
  எனில்((m_ptr->m_lsys_krn_sys_sdevio.request & _DIO_SAFEMASK) == _DIO_SAFE) {
     /* Map grant address to physical address. */
     எனில்((r=verify_grant(proc_nr_e, caller->p_endpoint,
		m_ptr->m_lsys_krn_sys_sdevio.vec_addr, மொத்தம்,
		req_dir == DIO_உள்ளீடு ? CPF_WRITE : CPF_READ,
		m_ptr->m_lsys_krn_sys_sdevio.offset, &newoffset, &newep,
		வெற்று)) != OK) {
	எனில்(r == ENOTREADY) திரும்பு r;
	printf("do_sdevio: verify_grant failed\n");
	திரும்பு EPERM;
    }
	எனில்(!isokendpt(newep, &proc_nr))
		திரும்பு(EINVAL);
     destproc = proc_addr(proc_nr);
     vir_buf = newoffset;
  } இல்லையெனில் {
     எனில்(proc_nr != _ENDPOINT_P(caller->p_endpoint))
     {
	printf("do_sdevio: unsafe sdevio by %d in %d denied\n",
		caller->p_endpoint, proc_nr_e);
	திரும்பு EPERM;
     }
     /* Get and check physical address. */
     vir_buf = m_ptr->m_lsys_krn_sys_sdevio.vec_addr;
     destproc = proc_addr(proc_nr);
  }
     /* current process must be target for phys_* to be OK */

  switch_address_space(destproc);

  தேர்வு (req_type)
  {
	நிலை _DIO_BYTE: size= 1; நிறுத்து;
	நிலை _DIO_WORD: size= 2; நிறுத்து;
	நிலை _DIO_LONG: size= 4; நிறுத்து;
	இயல்பு: size= 4; நிறுத்து;	/* Be conservative */
  }

  privp= priv(caller);
  எனில் (privp && privp->s_flags & CHECK_IO_PORT)
  {
	port= m_ptr->m_lsys_krn_sys_sdevio.port;
	nr_io_range= privp->s_nr_io_range;
	க்கு (i= 0, iorp= privp->s_io_tab; i<nr_io_range; i++, iorp++)
	{
		எனில் (port >= iorp->ior_base && port+size-1 <= iorp->ior_limit)
			நிறுத்து;
	}
	எனில் (i >= nr_io_range)
	{
		printf(
		"do_sdevio: I/O port check failed for proc %d, port 0x%x\n",
			m_ptr->m_source, port);
		retval = EPERM;
		செல் return_error;
	}
  }

  எனில் (port & (size-1))
  {
	printf("do_devio: unaligned port 0x%x (size %d)\n", port, size);
	retval = EPERM;
	செல் return_error;
  }

  /* Perform device I/O for bytes and words. Longs are not supported. */
  எனில் (req_dir == DIO_உள்ளீடு) { 
      தேர்வு (req_type) {
      நிலை _DIO_BYTE: phys_insb(port, vir_buf, மொத்தம்); நிறுத்து; 
      நிலை _DIO_WORD: phys_insw(port, vir_buf, மொத்தம்); நிறுத்து; 
      இயல்பு:
  		retval = EINVAL;
		செல் return_error;
      } 
  } இல்லையெனில் எனில் (req_dir == _DIO_OUTPUT) { 
      தேர்வு (req_type) {
      நிலை _DIO_BYTE: phys_outsb(port, vir_buf, மொத்தம்); நிறுத்து; 
      நிலை _DIO_WORD: phys_outsw(port, vir_buf, மொத்தம்); நிறுத்து; 
      இயல்பு:
  		retval = EINVAL;
		செல் return_error;
      } 
  }
  இல்லையெனில் {
	  retval = EINVAL;
	  செல் return_error;
  }
  retval = OK;

return_error:
  /* switch back to the address of the process which made the call */
  switch_address_space(caller);
  திரும்பு retval;
}

#endif /* USE_SDEVIO */
