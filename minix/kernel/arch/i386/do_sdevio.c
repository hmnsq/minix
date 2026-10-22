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

#ଅନ୍ତର୍ଭାଵ "kernel/system.h"
#ଅନ୍ତର୍ଭାଵ <minix/devio.h>
#ଅନ୍ତର୍ଭାଵ <minix/endpoint.h>

#ଅନ୍ତର୍ଭାଵ "arch_proto.h"

#ଯଦି USE_SDEVIO

/*===========================================================================*
 *			        do_sdevio                                    *
 *===========================================================================*/
ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_sdevio(ସଂରଚନା proc * caller, message *m_ptr)
{
  vir_bytes newoffset;
  endpoint_t newep;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା proc_nr;
  endpoint_t proc_nr_e = m_ptr->m_lsys_krn_sys_sdevio.vec_endpt;
  vir_bytes ଗଣନା_2 = m_ptr->m_lsys_krn_sys_sdevio.vec_size;
  long port = m_ptr->m_lsys_krn_sys_sdevio.port;
  phys_bytes vir_buf;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା i, r, req_type, req_dir, size, nr_io_range;
  ସଂରଚନା priv *privp;
  ସଂରଚନା io_range *iorp;
  ସଂରଚନା proc *destproc;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା retval;

  /* Allow safe copies and accesses to SELF */
  ଯଦି ((m_ptr->m_lsys_krn_sys_sdevio.request & _DIO_SAFEMASK) != _DIO_SAFE &&
	proc_nr_e != SELF)
  {
	ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା first= 1;
	ଯଦି (first)
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
  ଯଦି (proc_nr_e == SELF)
	okendpt(caller->p_endpoint, &proc_nr);
  ଅନ୍ୟଥା
	ଯଦି(!isokendpt(proc_nr_e, &proc_nr))
		ଫେରନ୍ତୁ(EINVAL);
  ଯଦି (iskerneln(proc_nr)) ଫେରନ୍ତୁ(EPERM);

  /* Extract direction (in or out) and type (size). */
  req_dir = m_ptr->m_lsys_krn_sys_sdevio.request & _DIO_DIRMASK;
  req_type = m_ptr->m_lsys_krn_sys_sdevio.request & _DIO_TYPEMASK;

  /* Check for 'safe' variants. */
  ଯଦି((m_ptr->m_lsys_krn_sys_sdevio.request & _DIO_SAFEMASK) == _DIO_SAFE) {
     /* Map grant address to physical address. */
     ଯଦି((r=verify_grant(proc_nr_e, caller->p_endpoint,
		m_ptr->m_lsys_krn_sys_sdevio.vec_addr, ଗଣନା_2,
		req_dir == DIO_ନିବେଶ ? CPF_WRITE : CPF_READ,
		m_ptr->m_lsys_krn_sys_sdevio.offset, &newoffset, &newep,
		ଖାଲି)) != OK) {
	ଯଦି(r == ENOTREADY) ଫେରନ୍ତୁ r;
	printf("do_sdevio: verify_grant failed\n");
	ଫେରନ୍ତୁ EPERM;
    }
	ଯଦି(!isokendpt(newep, &proc_nr))
		ଫେରନ୍ତୁ(EINVAL);
     destproc = proc_addr(proc_nr);
     vir_buf = newoffset;
  } ଅନ୍ୟଥା {
     ଯଦି(proc_nr != _ENDPOINT_P(caller->p_endpoint))
     {
	printf("do_sdevio: unsafe sdevio by %d in %d denied\n",
		caller->p_endpoint, proc_nr_e);
	ଫେରନ୍ତୁ EPERM;
     }
     /* Get and check physical address. */
     vir_buf = m_ptr->m_lsys_krn_sys_sdevio.vec_addr;
     destproc = proc_addr(proc_nr);
  }
     /* current process must be target for phys_* to be OK */

  switch_address_space(destproc);

  ବାଛ (req_type)
  {
	ସ୍ଥିତି _DIO_BYTE: size= 1; ଭାଙ୍ଗ;
	ସ୍ଥିତି _DIO_WORD: size= 2; ଭାଙ୍ଗ;
	ସ୍ଥିତି _DIO_LONG: size= 4; ଭାଙ୍ଗ;
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ: size= 4; ଭାଙ୍ଗ;	/* Be conservative */
  }

  privp= priv(caller);
  ଯଦି (privp && privp->s_flags & CHECK_IO_PORT)
  {
	port= m_ptr->m_lsys_krn_sys_sdevio.port;
	nr_io_range= privp->s_nr_io_range;
	ପାଇଁ (i= 0, iorp= privp->s_io_tab; i<nr_io_range; i++, iorp++)
	{
		ଯଦି (port >= iorp->ior_base && port+size-1 <= iorp->ior_limit)
			ଭାଙ୍ଗ;
	}
	ଯଦି (i >= nr_io_range)
	{
		printf(
		"do_sdevio: I/O port check failed for proc %d, port 0x%x\n",
			m_ptr->m_source, port);
		retval = EPERM;
		ଯାଅ return_error;
	}
  }

  ଯଦି (port & (size-1))
  {
	printf("do_devio: unaligned port 0x%x (size %d)\n", port, size);
	retval = EPERM;
	ଯାଅ return_error;
  }

  /* Perform device I/O for bytes and words. Longs are not supported. */
  ଯଦି (req_dir == DIO_ନିବେଶ) { 
      ବାଛ (req_type) {
      ସ୍ଥିତି _DIO_BYTE: phys_insb(port, vir_buf, ଗଣନା_2); ଭାଙ୍ଗ; 
      ସ୍ଥିତି _DIO_WORD: phys_insw(port, vir_buf, ଗଣନା_2); ଭାଙ୍ଗ; 
      ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
  		retval = EINVAL;
		ଯାଅ return_error;
      } 
  } ଅନ୍ୟଥା ଯଦି (req_dir == _DIO_OUTPUT) { 
      ବାଛ (req_type) {
      ସ୍ଥିତି _DIO_BYTE: phys_outsb(port, vir_buf, ଗଣନା_2); ଭାଙ୍ଗ; 
      ସ୍ଥିତି _DIO_WORD: phys_outsw(port, vir_buf, ଗଣନା_2); ଭାଙ୍ଗ; 
      ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
  		retval = EINVAL;
		ଯାଅ return_error;
      } 
  }
  ଅନ୍ୟଥା {
	  retval = EINVAL;
	  ଯାଅ return_error;
  }
  retval = OK;

return_error:
  /* switch back to the address of the process which made the call */
  switch_address_space(caller);
  ଫେରନ୍ତୁ retval;
}

#endif /* USE_SDEVIO */
