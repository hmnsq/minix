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

#གལ་ཏེ USE_SDEVIO

/*===========================================================================*
 *			        do_sdevio                                    *
 *===========================================================================*/
ཧྲིལ do_sdevio(sa_nracanaa proc * caller, message *m_ptr)
{
  vir_bytes newoffset;
  endpoint_t newep;
  ཧྲིལ proc_nr;
  endpoint_t proc_nr_e = m_ptr->m_lsys_krn_sys_sdevio.vec_endpt;
  vir_bytes count = m_ptr->m_lsys_krn_sys_sdevio.vec_size;
  long port = m_ptr->m_lsys_krn_sys_sdevio.port;
  phys_bytes vir_buf;
  ཧྲིལ i, r, req_type, req_dir, size, nr_io_range;
  sa_nracanaa priv *privp;
  sa_nracanaa io_range *iorp;
  sa_nracanaa proc *destproc;
  ཧྲིལ retval;

  /* Allow safe copies and accesses to SELF */
  གལ་ཏེ ((m_ptr->m_lsys_krn_sys_sdevio.request & _DIO_SAFEMASK) != _DIO_SAFE &&
	proc_nr_e != SELF)
  {
	static ཧྲིལ first= 1;
	གལ་ཏེ (first)
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
  གལ་ཏེ (proc_nr_e == SELF)
	okendpt(caller->p_endpoint, &proc_nr);
  དེ་མིན
	གལ་ཏེ(!isokendpt(proc_nr_e, &proc_nr))
		ཕྱིར(EINVAL);
  གལ་ཏེ (iskerneln(proc_nr)) ཕྱིར(EPERM);

  /* Extract direction (in or out) and type (size). */
  req_dir = m_ptr->m_lsys_krn_sys_sdevio.request & _DIO_DIRMASK;
  req_type = m_ptr->m_lsys_krn_sys_sdevio.request & _DIO_TYPEMASK;

  /* Check for 'safe' variants. */
  གལ་ཏེ((m_ptr->m_lsys_krn_sys_sdevio.request & _DIO_SAFEMASK) == _DIO_SAFE) {
     /* Map grant address to physical address. */
     གལ་ཏེ((r=verify_grant(proc_nr_e, caller->p_endpoint,
		m_ptr->m_lsys_krn_sys_sdevio.vec_addr, count,
		req_dir == _DIO_INPUT ? CPF_WRITE : CPF_READ,
		m_ptr->m_lsys_krn_sys_sdevio.offset, &newoffset, &newep,
		NULL)) != OK) {
	གལ་ཏེ(r == ENOTREADY) ཕྱིར r;
	printf("do_sdevio: verify_grant failed\n");
	ཕྱིར EPERM;
    }
	གལ་ཏེ(!isokendpt(newep, &proc_nr))
		ཕྱིར(EINVAL);
     destproc = proc_addr(proc_nr);
     vir_buf = newoffset;
  } དེ་མིན {
     གལ་ཏེ(proc_nr != _ENDPOINT_P(caller->p_endpoint))
     {
	printf("do_sdevio: unsafe sdevio by %d in %d denied\n",
		caller->p_endpoint, proc_nr_e);
	ཕྱིར EPERM;
     }
     /* Get and check physical address. */
     vir_buf = m_ptr->m_lsys_krn_sys_sdevio.vec_addr;
     destproc = proc_addr(proc_nr);
  }
     /* current process must be target for phys_* to be OK */

  switch_address_space(destproc);

  switch (req_type)
  {
	pak_sa _DIO_BYTE: size= 1; bha_nga;
	pak_sa _DIO_WORD: size= 2; bha_nga;
	pak_sa _DIO_LONG: size= 4; bha_nga;
	default: size= 4; bha_nga;	/* Be conservative */
  }

  privp= priv(caller);
  གལ་ཏེ (privp && privp->s_flags & CHECK_IO_PORT)
  {
	port= m_ptr->m_lsys_krn_sys_sdevio.port;
	nr_io_range= privp->s_nr_io_range;
	ཆེད་དུ (i= 0, iorp= privp->s_io_tab; i<nr_io_range; i++, iorp++)
	{
		གལ་ཏེ (port >= iorp->ior_base && port+size-1 <= iorp->ior_limit)
			bha_nga;
	}
	གལ་ཏེ (i >= nr_io_range)
	{
		printf(
		"do_sdevio: I/O port check failed for proc %d, port 0x%x\n",
			m_ptr->m_source, port);
		retval = EPERM;
		goto return_error;
	}
  }

  གལ་ཏེ (port & (size-1))
  {
	printf("do_devio: unaligned port 0x%x (size %d)\n", port, size);
	retval = EPERM;
	goto return_error;
  }

  /* Perform device I/O for bytes and words. Longs are not supported. */
  གལ་ཏེ (req_dir == _DIO_INPUT) { 
      switch (req_type) {
      pak_sa _DIO_BYTE: phys_insb(port, vir_buf, count); bha_nga; 
      pak_sa _DIO_WORD: phys_insw(port, vir_buf, count); bha_nga; 
      default:
  		retval = EINVAL;
		goto return_error;
      } 
  } དེ་མིན གལ་ཏེ (req_dir == _DIO_OUTPUT) { 
      switch (req_type) {
      pak_sa _DIO_BYTE: phys_outsb(port, vir_buf, count); bha_nga; 
      pak_sa _DIO_WORD: phys_outsw(port, vir_buf, count); bha_nga; 
      default:
  		retval = EINVAL;
		goto return_error;
      } 
  }
  དེ་མིན {
	  retval = EINVAL;
	  goto return_error;
  }
  retval = OK;

return_error:
  /* switch back to the address of the process which made the call */
  switch_address_space(caller);
  ཕྱིར retval;
}

#endif /* USE_SDEVIO */
