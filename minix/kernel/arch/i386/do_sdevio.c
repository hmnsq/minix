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

#إذا USE_SDEVIO

/*===========================================================================*
 *			        do_sdevio                                    *
 *===========================================================================*/
صحيح do_sdevio(بنية proc * caller, سنیہا *m_ptr)
{
  vir_بائٹاں newoffset;
  endpoint_t newep;
  صحيح proc_nr;
  endpoint_t proc_nr_e = m_ptr->m_lsys_krn_sys_sdevio.vec_endpt;
  vir_بائٹاں گنتی = m_ptr->m_lsys_krn_sys_sdevio.vec_ناپ;
  لما پورٹ = m_ptr->m_lsys_krn_sys_sdevio.پورٹ;
  phys_بائٹاں vir_buf;
  صحيح i, r, req_type, req_dir, ناپ, nr_io_range;
  بنية priv *privp;
  بنية io_range *iorp;
  بنية proc *destproc;
  صحيح retval;

  /* Allow safe copies and accesses to SELF */
  إذا ((m_ptr->m_lsys_krn_sys_sdevio.request & _DIO_SAFEMASK) != DIO_محفوظ &&
	proc_nr_e != SELF)
  {
	ساكن صحيح پہلا= 1;
	إذا (پہلا)
	{
		پہلا= 0;
		لکھو("do_sdevio: for %d, req %d\n",
			m_ptr->m_سروت, m_ptr->m_lsys_krn_sys_sdevio.request);
	}
  }

  /* Check if process endpoint is OK. 
   * A driver may directly provide a pointer to a buffer at the user-process
   * that initiated the device I/O. Kernel processes, of course, are denied.
   */
  إذا (proc_nr_e == SELF)
	okendpt(caller->p_endpoint, &proc_nr);
  وإلا
	إذا(!isokendpt(proc_nr_e, &proc_nr))
		أرجع(EINVAL);
  إذا (iskerneln(proc_nr)) أرجع(EPERM);

  /* Extract direction (in or out) and type (size). */
  req_dir = m_ptr->m_lsys_krn_sys_sdevio.request & _DIO_DIRMASK;
  req_type = m_ptr->m_lsys_krn_sys_sdevio.request & _DIO_TYPEMASK;

  /* Check for 'safe' variants. */
  إذا((m_ptr->m_lsys_krn_sys_sdevio.request & _DIO_SAFEMASK) == DIO_محفوظ) {
     /* Map grant address to physical address. */
     إذا((r=verify_grant(proc_nr_e, caller->p_endpoint,
		m_ptr->m_lsys_krn_sys_sdevio.vec_addr, گنتی,
		req_dir == DIO_داخلہ ? CPF_WRITE : CPF_READ,
		m_ptr->m_lsys_krn_sys_sdevio.وتھ, &newoffset, &newep,
		عدم)) != OK) {
	إذا(r == ENOTREADY) أرجع r;
	لکھو("do_sdevio: verify_grant failed\n");
	أرجع EPERM;
    }
	إذا(!isokendpt(newep, &proc_nr))
		أرجع(EINVAL);
     destproc = proc_addr(proc_nr);
     vir_buf = newoffset;
  } وإلا {
     إذا(proc_nr != _ENDPOINT_P(caller->p_endpoint))
     {
	لکھو("do_sdevio: unsafe sdevio by %d in %d denied\n",
		caller->p_endpoint, proc_nr_e);
	أرجع EPERM;
     }
     /* Get and check physical address. */
     vir_buf = m_ptr->m_lsys_krn_sys_sdevio.vec_addr;
     destproc = proc_addr(proc_nr);
  }
     /* current process must be target for phys_* to be OK */

  switch_پتہ_space(destproc);

  بدل (req_type)
  {
	حالة _DIO_BYTE: ناپ= 1; اكسر;
	حالة DIO_لفظ: ناپ= 2; اكسر;
	حالة _DIO_LONG: ناپ= 4; اكسر;
	افتراضي: ناپ= 4; اكسر;	/* Be conservative */
  }

  privp= priv(caller);
  إذا (privp && privp->s_flags & CHECK_IO_پورٹ)
  {
	پورٹ= m_ptr->m_lsys_krn_sys_sdevio.پورٹ;
	nr_io_range= privp->s_nr_io_range;
	لكل (i= 0, iorp= privp->s_io_tab; i<nr_io_range; i++, iorp++)
	{
		إذا (پورٹ >= iorp->ior_ادھار && پورٹ+ناپ-1 <= iorp->ior_حد_بندی)
			اكسر;
	}
	إذا (i >= nr_io_range)
	{
		لکھو(
		"do_sdevio: I/O port check failed for proc %d, port 0x%x\n",
			m_ptr->m_سروت, پورٹ);
		retval = EPERM;
		اذهب return_error;
	}
  }

  إذا (پورٹ & (ناپ-1))
  {
	لکھو("do_devio: unaligned port 0x%x (size %d)\n", پورٹ, ناپ);
	retval = EPERM;
	اذهب return_error;
  }

  /* Perform device I/O for bytes and words. Longs are not supported. */
  إذا (req_dir == DIO_داخلہ) { 
      بدل (req_type) {
      حالة _DIO_BYTE: phys_insb(پورٹ, vir_buf, گنتی); اكسر; 
      حالة DIO_لفظ: phys_insw(پورٹ, vir_buf, گنتی); اكسر; 
      افتراضي:
  		retval = EINVAL;
		اذهب return_error;
      } 
  } وإلا إذا (req_dir == _DIO_OUTPUT) { 
      بدل (req_type) {
      حالة _DIO_BYTE: phys_outsb(پورٹ, vir_buf, گنتی); اكسر; 
      حالة DIO_لفظ: phys_outsw(پورٹ, vir_buf, گنتی); اكسر; 
      افتراضي:
  		retval = EINVAL;
		اذهب return_error;
      } 
  }
  وإلا {
	  retval = EINVAL;
	  اذهب return_error;
  }
  retval = OK;

return_error:
  /* switch back to the address of the process which made the call */
  switch_پتہ_space(caller);
  أرجع retval;
}

#جے_انت /* USE_SDEVIO */
