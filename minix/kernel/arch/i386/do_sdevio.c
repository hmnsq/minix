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
صحیح do_sdevio(ساخت proc * caller, پیغام *m_ptr)
{
  vir_بائٹیں newoffset;
  endpoint_t newep;
  صحیح proc_nr;
  endpoint_t proc_nr_e = m_ptr->m_lsys_krn_sys_sdevio.vec_endpt;
  vir_بائٹیں گنتی = m_ptr->m_lsys_krn_sys_sdevio.vec_سائز;
  طویل پورٹ = m_ptr->m_lsys_krn_sys_sdevio.پورٹ;
  phys_بائٹیں vir_buf;
  صحیح i, r, req_type, req_dir, سائز, nr_io_range;
  ساخت priv *privp;
  ساخت io_range *iorp;
  ساخت proc *destproc;
  صحیح retval;

  /* Allow safe copies and accesses to SELF */
  اگر ((m_ptr->m_lsys_krn_sys_sdevio.request & _DIO_SAFEMASK) != DIO_محفوظ &&
	proc_nr_e != SELF)
  {
	ساکن صحیح پہلا= 1;
	اگر (پہلا)
	{
		پہلا= 0;
		لکھو("do_sdevio: for %d, req %d\n",
			m_ptr->m_ماخذ, m_ptr->m_lsys_krn_sys_sdevio.request);
	}
  }

  /* Check if process endpoint is OK. 
   * A driver may directly provide a pointer to a buffer at the user-process
   * that initiated the device I/O. Kernel processes, of course, are denied.
   */
  اگر (proc_nr_e == SELF)
	okendpt(caller->p_endpoint, &proc_nr);
  ورنہ
	اگر(!isokendpt(proc_nr_e, &proc_nr))
		واپس(EINVAL);
  اگر (iskerneln(proc_nr)) واپس(EPERM);

  /* Extract direction (in or out) and type (size). */
  req_dir = m_ptr->m_lsys_krn_sys_sdevio.request & _DIO_DIRMASK;
  req_type = m_ptr->m_lsys_krn_sys_sdevio.request & _DIO_TYPEMASK;

  /* Check for 'safe' variants. */
  اگر((m_ptr->m_lsys_krn_sys_sdevio.request & _DIO_SAFEMASK) == DIO_محفوظ) {
     /* Map grant address to physical address. */
     اگر((r=verify_grant(proc_nr_e, caller->p_endpoint,
		m_ptr->m_lsys_krn_sys_sdevio.vec_addr, گنتی,
		req_dir == DIO_داخلہ ? CPF_WRITE : CPF_READ,
		m_ptr->m_lsys_krn_sys_sdevio.انحراف, &newoffset, &newep,
		تہی)) != OK) {
	اگر(r == ENOTREADY) واپس r;
	لکھو("do_sdevio: verify_grant failed\n");
	واپس EPERM;
    }
	اگر(!isokendpt(newep, &proc_nr))
		واپس(EINVAL);
     destproc = proc_addr(proc_nr);
     vir_buf = newoffset;
  } ورنہ {
     اگر(proc_nr != _ENDPOINT_P(caller->p_endpoint))
     {
	لکھو("do_sdevio: unsafe sdevio by %d in %d denied\n",
		caller->p_endpoint, proc_nr_e);
	واپس EPERM;
     }
     /* Get and check physical address. */
     vir_buf = m_ptr->m_lsys_krn_sys_sdevio.vec_addr;
     destproc = proc_addr(proc_nr);
  }
     /* current process must be target for phys_* to be OK */

  switch_پتہ_space(destproc);

  انتخاب (req_type)
  {
	حالت _DIO_BYTE: سائز= 1; توڑو;
	حالت DIO_لفظ: سائز= 2; توڑو;
	حالت _DIO_LONG: سائز= 4; توڑو;
	طےشدہ: سائز= 4; توڑو;	/* Be conservative */
  }

  privp= priv(caller);
  اگر (privp && privp->s_flags & CHECK_IO_پورٹ)
  {
	پورٹ= m_ptr->m_lsys_krn_sys_sdevio.پورٹ;
	nr_io_range= privp->s_nr_io_range;
	برائے (i= 0, iorp= privp->s_io_tab; i<nr_io_range; i++, iorp++)
	{
		اگر (پورٹ >= iorp->ior_بنیاد && پورٹ+سائز-1 <= iorp->ior_حد_بندی)
			توڑو;
	}
	اگر (i >= nr_io_range)
	{
		لکھو(
		"do_sdevio: I/O port check failed for proc %d, port 0x%x\n",
			m_ptr->m_ماخذ, پورٹ);
		retval = EPERM;
		جاؤ return_error;
	}
  }

  اگر (پورٹ & (سائز-1))
  {
	لکھو("do_devio: unaligned port 0x%x (size %d)\n", پورٹ, سائز);
	retval = EPERM;
	جاؤ return_error;
  }

  /* Perform device I/O for bytes and words. Longs are not supported. */
  اگر (req_dir == DIO_داخلہ) { 
      انتخاب (req_type) {
      حالت _DIO_BYTE: phys_insb(پورٹ, vir_buf, گنتی); توڑو; 
      حالت DIO_لفظ: phys_insw(پورٹ, vir_buf, گنتی); توڑو; 
      طےشدہ:
  		retval = EINVAL;
		جاؤ return_error;
      } 
  } ورنہ اگر (req_dir == _DIO_OUTPUT) { 
      انتخاب (req_type) {
      حالت _DIO_BYTE: phys_outsb(پورٹ, vir_buf, گنتی); توڑو; 
      حالت DIO_لفظ: phys_outsw(پورٹ, vir_buf, گنتی); توڑو; 
      طےشدہ:
  		retval = EINVAL;
		جاؤ return_error;
      } 
  }
  ورنہ {
	  retval = EINVAL;
	  جاؤ return_error;
  }
  retval = OK;

return_error:
  /* switch back to the address of the process which made the call */
  switch_پتہ_space(caller);
  واپس retval;
}

#اگر_ختم /* USE_SDEVIO */
