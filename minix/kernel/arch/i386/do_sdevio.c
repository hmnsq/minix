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

#include "kernel/system.h"
#include <minix/devio.h>
#include <minix/endpoint.h>

#include "arch_proto.h"

#ако USE_SDEVIO

/*===========================================================================*
 *			        do_sdevio                                    *
 *===========================================================================*/
цео do_sdevio(struct proc * caller, message *m_ptr)
{
  vir_bytes newoffset;
  endpoint_t newep;
  цео proc_nr;
  endpoint_t proc_nr_e = m_ptr->m_lsys_krn_sys_sdevio.vec_endpt;
  vir_bytes count = m_ptr->m_lsys_krn_sys_sdevio.vec_size;
  long port = m_ptr->m_lsys_krn_sys_sdevio.port;
  phys_bytes vir_buf;
  цео i, r, req_type, req_dir, size, nr_io_range;
  struct priv *privp;
  struct io_range *iorp;
  struct proc *destproc;
  цео retval;

  /* Allow safe copies and accesses to SELF */
  ако ((m_ptr->m_lsys_krn_sys_sdevio.request & _DIO_SAFEMASK) != _DIO_SAFE &&
	proc_nr_e != SELF)
  {
	static цео first= 1;
	ако (first)
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
  ако (proc_nr_e == SELF)
	okendpt(caller->p_endpoint, &proc_nr);
  инако
	ако(!isokendpt(proc_nr_e, &proc_nr))
		врати(EINVAL);
  ако (iskerneln(proc_nr)) врати(EPERM);

  /* Extract direction (in or out) and type (size). */
  req_dir = m_ptr->m_lsys_krn_sys_sdevio.request & _DIO_DIRMASK;
  req_type = m_ptr->m_lsys_krn_sys_sdevio.request & _DIO_TYPEMASK;

  /* Check for 'safe' variants. */
  ако((m_ptr->m_lsys_krn_sys_sdevio.request & _DIO_SAFEMASK) == _DIO_SAFE) {
     /* Map grant address to physical address. */
     ако((r=verify_grant(proc_nr_e, caller->p_endpoint,
		m_ptr->m_lsys_krn_sys_sdevio.vec_addr, count,
		req_dir == _DIO_INPUT ? CPF_WRITE : CPF_READ,
		m_ptr->m_lsys_krn_sys_sdevio.offset, &newoffset, &newep,
		NULL)) != OK) {
	ако(r == ENOTREADY) врати r;
	printf("do_sdevio: verify_grant failed\n");
	врати EPERM;
    }
	ако(!isokendpt(newep, &proc_nr))
		врати(EINVAL);
     destproc = proc_addr(proc_nr);
     vir_buf = newoffset;
  } инако {
     ако(proc_nr != _ENDPOINT_P(caller->p_endpoint))
     {
	printf("do_sdevio: unsafe sdevio by %d in %d denied\n",
		caller->p_endpoint, proc_nr_e);
	врати EPERM;
     }
     /* Get and check physical address. */
     vir_buf = m_ptr->m_lsys_krn_sys_sdevio.vec_addr;
     destproc = proc_addr(proc_nr);
  }
     /* current process must be target for phys_* to be OK */

  switch_address_space(destproc);

  switch (req_type)
  {
	case _DIO_BYTE: size= 1; break;
	case _DIO_WORD: size= 2; break;
	case _DIO_LONG: size= 4; break;
	default: size= 4; break;	/* Be conservative */
  }

  privp= priv(caller);
  ако (privp && privp->s_flags & CHECK_IO_PORT)
  {
	port= m_ptr->m_lsys_krn_sys_sdevio.port;
	nr_io_range= privp->s_nr_io_range;
	за (i= 0, iorp= privp->s_io_tab; i<nr_io_range; i++, iorp++)
	{
		ако (port >= iorp->ior_base && port+size-1 <= iorp->ior_limit)
			break;
	}
	ако (i >= nr_io_range)
	{
		printf(
		"do_sdevio: I/O port check failed for proc %d, port 0x%x\n",
			m_ptr->m_source, port);
		retval = EPERM;
		goto return_error;
	}
  }

  ако (port & (size-1))
  {
	printf("do_devio: unaligned port 0x%x (size %d)\n", port, size);
	retval = EPERM;
	goto return_error;
  }

  /* Perform device I/O for bytes and words. Longs are not supported. */
  ако (req_dir == _DIO_INPUT) { 
      switch (req_type) {
      case _DIO_BYTE: phys_insb(port, vir_buf, count); break; 
      case _DIO_WORD: phys_insw(port, vir_buf, count); break; 
      default:
  		retval = EINVAL;
		goto return_error;
      } 
  } инако ако (req_dir == _DIO_OUTPUT) { 
      switch (req_type) {
      case _DIO_BYTE: phys_outsb(port, vir_buf, count); break; 
      case _DIO_WORD: phys_outsw(port, vir_buf, count); break; 
      default:
  		retval = EINVAL;
		goto return_error;
      } 
  }
  инако {
	  retval = EINVAL;
	  goto return_error;
  }
  retval = OK;

return_error:
  /* switch back to the address of the process which made the call */
  switch_address_space(caller);
  врати retval;
}

#endif /* USE_SDEVIO */
