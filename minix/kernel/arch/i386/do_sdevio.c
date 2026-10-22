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

#అన్తర్భావ "kernel/system.h"
#అన్తర్భావ <minix/devio.h>
#అన్తర్భావ <minix/endpoint.h>

#అన్తర్భావ "arch_proto.h"

#ఒకవేళ USE_SDEVIO

/*===========================================================================*
 *			        do_sdevio                                    *
 *===========================================================================*/
పూర్ణాంకం do_sdevio(నిర్మాణం proc * caller, సందేశం *m_ptr)
{
  vir_బైట్లు newoffset;
  endpoint_t newep;
  పూర్ణాంకం proc_nr;
  endpoint_t proc_nr_e = m_ptr->m_lsys_krn_sys_sdevio.vec_endpt;
  vir_బైట్లు లెక్క = m_ptr->m_lsys_krn_sys_sdevio.vec_కొలత;
  దీర్ఘ పోర్టు = m_ptr->m_lsys_krn_sys_sdevio.పోర్టు;
  phys_బైట్లు vir_buf;
  పూర్ణాంకం i, r, req_type, req_dir, కొలత, nr_io_range;
  నిర్మాణం priv *privp;
  నిర్మాణం io_range *iorp;
  నిర్మాణం proc *destproc;
  పూర్ణాంకం retval;

  /* Allow safe copies and accesses to SELF */
  ఒకవేళ ((m_ptr->m_lsys_krn_sys_sdevio.request & _DIO_SAFEMASK) != DIO_సురక్షిత &&
	proc_nr_e != SELF)
  {
	స్థిర పూర్ణాంకం మొదటి= 1;
	ఒకవేళ (మొదటి)
	{
		మొదటి= 0;
		రాయి("do_sdevio: for %d, req %d\n",
			m_ptr->m_ఆధారం, m_ptr->m_lsys_krn_sys_sdevio.request);
	}
  }

  /* Check if process endpoint is OK. 
   * A driver may directly provide a pointer to a buffer at the user-process
   * that initiated the device I/O. Kernel processes, of course, are denied.
   */
  ఒకవేళ (proc_nr_e == SELF)
	okendpt(caller->p_endpoint, &proc_nr);
  లేకపోతే
	ఒకవేళ(!isokendpt(proc_nr_e, &proc_nr))
		తిరిగి(EINVAL);
  ఒకవేళ (iskerneln(proc_nr)) తిరిగి(EPERM);

  /* Extract direction (in or out) and type (size). */
  req_dir = m_ptr->m_lsys_krn_sys_sdevio.request & _DIO_DIRMASK;
  req_type = m_ptr->m_lsys_krn_sys_sdevio.request & _DIO_TYPEMASK;

  /* Check for 'safe' variants. */
  ఒకవేళ((m_ptr->m_lsys_krn_sys_sdevio.request & _DIO_SAFEMASK) == DIO_సురక్షిత) {
     /* Map grant address to physical address. */
     ఒకవేళ((r=verify_grant(proc_nr_e, caller->p_endpoint,
		m_ptr->m_lsys_krn_sys_sdevio.vec_addr, లెక్క,
		req_dir == DIO_ఆదానం ? CPF_WRITE : CPF_READ,
		m_ptr->m_lsys_krn_sys_sdevio.స్థానభ్రంశం, &newoffset, &newep,
		ఖాళీ)) != OK) {
	ఒకవేళ(r == ENOTREADY) తిరిగి r;
	రాయి("do_sdevio: verify_grant failed\n");
	తిరిగి EPERM;
    }
	ఒకవేళ(!isokendpt(newep, &proc_nr))
		తిరిగి(EINVAL);
     destproc = proc_addr(proc_nr);
     vir_buf = newoffset;
  } లేకపోతే {
     ఒకవేళ(proc_nr != _ENDPOINT_P(caller->p_endpoint))
     {
	రాయి("do_sdevio: unsafe sdevio by %d in %d denied\n",
		caller->p_endpoint, proc_nr_e);
	తిరిగి EPERM;
     }
     /* Get and check physical address. */
     vir_buf = m_ptr->m_lsys_krn_sys_sdevio.vec_addr;
     destproc = proc_addr(proc_nr);
  }
     /* current process must be target for phys_* to be OK */

  switch_చిరునామా_space(destproc);

  ఎంపిక (req_type)
  {
	స్థితి _DIO_BYTE: కొలత= 1; ఆపు;
	స్థితి DIO_మాట: కొలత= 2; ఆపు;
	స్థితి _DIO_LONG: కొలత= 4; ఆపు;
	అప్రమేయ: కొలత= 4; ఆపు;	/* Be conservative */
  }

  privp= priv(caller);
  ఒకవేళ (privp && privp->s_flags & CHECK_IO_పోర్టు)
  {
	పోర్టు= m_ptr->m_lsys_krn_sys_sdevio.పోర్టు;
	nr_io_range= privp->s_nr_io_range;
	కొరకు (i= 0, iorp= privp->s_io_tab; i<nr_io_range; i++, iorp++)
	{
		ఒకవేళ (పోర్టు >= iorp->ior_పునాది && పోర్టు+కొలత-1 <= iorp->ior_హద్దు)
			ఆపు;
	}
	ఒకవేళ (i >= nr_io_range)
	{
		రాయి(
		"do_sdevio: I/O port check failed for proc %d, port 0x%x\n",
			m_ptr->m_ఆధారం, పోర్టు);
		retval = EPERM;
		వెళ్ళు return_error;
	}
  }

  ఒకవేళ (పోర్టు & (కొలత-1))
  {
	రాయి("do_devio: unaligned port 0x%x (size %d)\n", పోర్టు, కొలత);
	retval = EPERM;
	వెళ్ళు return_error;
  }

  /* Perform device I/O for bytes and words. Longs are not supported. */
  ఒకవేళ (req_dir == DIO_ఆదానం) { 
      ఎంపిక (req_type) {
      స్థితి _DIO_BYTE: phys_insb(పోర్టు, vir_buf, లెక్క); ఆపు; 
      స్థితి DIO_మాట: phys_insw(పోర్టు, vir_buf, లెక్క); ఆపు; 
      అప్రమేయ:
  		retval = EINVAL;
		వెళ్ళు return_error;
      } 
  } లేకపోతే ఒకవేళ (req_dir == _DIO_OUTPUT) { 
      ఎంపిక (req_type) {
      స్థితి _DIO_BYTE: phys_outsb(పోర్టు, vir_buf, లెక్క); ఆపు; 
      స్థితి DIO_మాట: phys_outsw(పోర్టు, vir_buf, లెక్క); ఆపు; 
      అప్రమేయ:
  		retval = EINVAL;
		వెళ్ళు return_error;
      } 
  }
  లేకపోతే {
	  retval = EINVAL;
	  వెళ్ళు return_error;
  }
  retval = OK;

return_error:
  /* switch back to the address of the process which made the call */
  switch_చిరునామా_space(caller);
  తిరిగి retval;
}

#అయితే_ముగింపు /* USE_SDEVIO */
