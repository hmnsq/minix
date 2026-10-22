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

#અન્તર્ભાવ "kernel/system.h"
#અન્તર્ભાવ <minix/devio.h>
#અન્તર્ભાવ <minix/endpoint.h>

#અન્તર્ભાવ "arch_proto.h"

#જો USE_SDEVIO

/*===========================================================================*
 *			        do_sdevio                                    *
 *===========================================================================*/
પૂર્ણાંક do_sdevio(સંરચના proc * caller, message *m_ptr)
{
  vir_bytes newoffset;
  endpoint_t newep;
  પૂર્ણાંક proc_nr;
  endpoint_t proc_nr_e = m_ptr->m_lsys_krn_sys_sdevio.vec_endpt;
  vir_bytes ગણતરી = m_ptr->m_lsys_krn_sys_sdevio.vec_size;
  long port = m_ptr->m_lsys_krn_sys_sdevio.port;
  phys_bytes vir_buf;
  પૂર્ણાંક i, r, req_type, req_dir, size, nr_io_range;
  સંરચના priv *privp;
  સંરચના io_range *iorp;
  સંરચના proc *destproc;
  પૂર્ણાંક retval;

  /* Allow safe copies and accesses to SELF */
  જો ((m_ptr->m_lsys_krn_sys_sdevio.request & _DIO_SAFEMASK) != _DIO_SAFE &&
	proc_nr_e != SELF)
  {
	સ્થિર પૂર્ણાંક first= 1;
	જો (first)
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
  જો (proc_nr_e == SELF)
	okendpt(caller->p_endpoint, &proc_nr);
  નહીંતર
	જો(!isokendpt(proc_nr_e, &proc_nr))
		પાછા(EINVAL);
  જો (iskerneln(proc_nr)) પાછા(EPERM);

  /* Extract direction (in or out) and type (size). */
  req_dir = m_ptr->m_lsys_krn_sys_sdevio.request & _DIO_DIRMASK;
  req_type = m_ptr->m_lsys_krn_sys_sdevio.request & _DIO_TYPEMASK;

  /* Check for 'safe' variants. */
  જો((m_ptr->m_lsys_krn_sys_sdevio.request & _DIO_SAFEMASK) == _DIO_SAFE) {
     /* Map grant address to physical address. */
     જો((r=verify_grant(proc_nr_e, caller->p_endpoint,
		m_ptr->m_lsys_krn_sys_sdevio.vec_addr, ગણતરી,
		req_dir == DIO_આગત ? CPF_WRITE : CPF_READ,
		m_ptr->m_lsys_krn_sys_sdevio.offset, &newoffset, &newep,
		ખાલી)) != OK) {
	જો(r == ENOTREADY) પાછા r;
	printf("do_sdevio: verify_grant failed\n");
	પાછા EPERM;
    }
	જો(!isokendpt(newep, &proc_nr))
		પાછા(EINVAL);
     destproc = proc_addr(proc_nr);
     vir_buf = newoffset;
  } નહીંતર {
     જો(proc_nr != _ENDPOINT_P(caller->p_endpoint))
     {
	printf("do_sdevio: unsafe sdevio by %d in %d denied\n",
		caller->p_endpoint, proc_nr_e);
	પાછા EPERM;
     }
     /* Get and check physical address. */
     vir_buf = m_ptr->m_lsys_krn_sys_sdevio.vec_addr;
     destproc = proc_addr(proc_nr);
  }
     /* current process must be target for phys_* to be OK */

  switch_address_space(destproc);

  પસંદગી (req_type)
  {
	સ્થિતિ _DIO_BYTE: size= 1; તોડો;
	સ્થિતિ _DIO_WORD: size= 2; તોડો;
	સ્થિતિ _DIO_LONG: size= 4; તોડો;
	મૂળભૂત: size= 4; તોડો;	/* Be conservative */
  }

  privp= priv(caller);
  જો (privp && privp->s_flags & CHECK_IO_PORT)
  {
	port= m_ptr->m_lsys_krn_sys_sdevio.port;
	nr_io_range= privp->s_nr_io_range;
	માટે (i= 0, iorp= privp->s_io_tab; i<nr_io_range; i++, iorp++)
	{
		જો (port >= iorp->ior_base && port+size-1 <= iorp->ior_limit)
			તોડો;
	}
	જો (i >= nr_io_range)
	{
		printf(
		"do_sdevio: I/O port check failed for proc %d, port 0x%x\n",
			m_ptr->m_source, port);
		retval = EPERM;
		જાઓ return_error;
	}
  }

  જો (port & (size-1))
  {
	printf("do_devio: unaligned port 0x%x (size %d)\n", port, size);
	retval = EPERM;
	જાઓ return_error;
  }

  /* Perform device I/O for bytes and words. Longs are not supported. */
  જો (req_dir == DIO_આગત) { 
      પસંદગી (req_type) {
      સ્થિતિ _DIO_BYTE: phys_insb(port, vir_buf, ગણતરી); તોડો; 
      સ્થિતિ _DIO_WORD: phys_insw(port, vir_buf, ગણતરી); તોડો; 
      મૂળભૂત:
  		retval = EINVAL;
		જાઓ return_error;
      } 
  } નહીંતર જો (req_dir == _DIO_OUTPUT) { 
      પસંદગી (req_type) {
      સ્થિતિ _DIO_BYTE: phys_outsb(port, vir_buf, ગણતરી); તોડો; 
      સ્થિતિ _DIO_WORD: phys_outsw(port, vir_buf, ગણતરી); તોડો; 
      મૂળભૂત:
  		retval = EINVAL;
		જાઓ return_error;
      } 
  }
  નહીંતર {
	  retval = EINVAL;
	  જાઓ return_error;
  }
  retval = OK;

return_error:
  /* switch back to the address of the process which made the call */
  switch_address_space(caller);
  પાછા retval;
}

#endif /* USE_SDEVIO */
