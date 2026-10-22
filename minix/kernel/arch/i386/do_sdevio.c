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

#ಅನ್ತರ್ಭಾವ "kernel/system.h"
#ಅನ್ತರ್ಭಾವ <minix/devio.h>
#ಅನ್ತರ್ಭಾವ <minix/endpoint.h>

#ಅನ್ತರ್ಭಾವ "arch_proto.h"

#ಆದರೆ USE_SDEVIO

/*===========================================================================*
 *			        do_sdevio                                    *
 *===========================================================================*/
ಪೂರ್ಣಾಂಕ do_sdevio(ರಚನೆ proc * caller, ಸಂದೇಶ *m_ptr)
{
  vir_ಬೈಟುಗಳು newoffset;
  endpoint_t newep;
  ಪೂರ್ಣಾಂಕ proc_nr;
  endpoint_t proc_nr_e = m_ptr->m_lsys_krn_sys_sdevio.vec_endpt;
  vir_ಬೈಟುಗಳು ಎಣಿಕೆ_2 = m_ptr->m_lsys_krn_sys_sdevio.vec_ಗಾತ್ರ;
  ದೀರ್ಘ ಪೋರ್ಟ್ = m_ptr->m_lsys_krn_sys_sdevio.ಪೋರ್ಟ್;
  phys_ಬೈಟುಗಳು vir_buf;
  ಪೂರ್ಣಾಂಕ i, r, req_type, req_dir, ಗಾತ್ರ_2, nr_io_range;
  ರಚನೆ priv *privp;
  ರಚನೆ io_range *iorp;
  ರಚನೆ proc *destproc;
  ಪೂರ್ಣಾಂಕ retval;

  /* Allow safe copies and accesses to SELF */
  ಆದರೆ ((m_ptr->m_lsys_krn_sys_sdevio.request & _DIO_SAFEMASK) != DIO_ಸುರಕ್ಷಿತ &&
	proc_nr_e != SELF)
  {
	ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಮೊದಲ= 1;
	ಆದರೆ (ಮೊದಲ)
	{
		ಮೊದಲ= 0;
		ಬರೆ("do_sdevio: for %d, req %d\n",
			m_ptr->m_ಮೂಲ, m_ptr->m_lsys_krn_sys_sdevio.request);
	}
  }

  /* Check if process endpoint is OK. 
   * A driver may directly provide a pointer to a buffer at the user-process
   * that initiated the device I/O. Kernel processes, of course, are denied.
   */
  ಆದರೆ (proc_nr_e == SELF)
	okendpt(caller->p_endpoint, &proc_nr);
  ಇಲ್ಲದಿದ್ದರೆ
	ಆದರೆ(!isokendpt(proc_nr_e, &proc_nr))
		ಹಿಂತಿರುಗಿ(EINVAL);
  ಆದರೆ (iskerneln(proc_nr)) ಹಿಂತಿರುಗಿ(EPERM);

  /* Extract direction (in or out) and type (size). */
  req_dir = m_ptr->m_lsys_krn_sys_sdevio.request & _DIO_DIRMASK;
  req_type = m_ptr->m_lsys_krn_sys_sdevio.request & _DIO_TYPEMASK;

  /* Check for 'safe' variants. */
  ಆದರೆ((m_ptr->m_lsys_krn_sys_sdevio.request & _DIO_SAFEMASK) == DIO_ಸುರಕ್ಷಿತ) {
     /* Map grant address to physical address. */
     ಆದರೆ((r=verify_grant(proc_nr_e, caller->p_endpoint,
		m_ptr->m_lsys_krn_sys_sdevio.vec_addr, ಎಣಿಕೆ_2,
		req_dir == DIO_ಒಳಹರಿವು ? CPF_WRITE : CPF_READ,
		m_ptr->m_lsys_krn_sys_sdevio.ಸ್ಥಾನಾಂತರ, &newoffset, &newep,
		ಖಾಲಿ)) != OK) {
	ಆದರೆ(r == ENOTREADY) ಹಿಂತಿರುಗಿ r;
	ಬರೆ("do_sdevio: verify_grant failed\n");
	ಹಿಂತಿರುಗಿ EPERM;
    }
	ಆದರೆ(!isokendpt(newep, &proc_nr))
		ಹಿಂತಿರುಗಿ(EINVAL);
     destproc = proc_addr(proc_nr);
     vir_buf = newoffset;
  } ಇಲ್ಲದಿದ್ದರೆ {
     ಆದರೆ(proc_nr != _ENDPOINT_P(caller->p_endpoint))
     {
	ಬರೆ("do_sdevio: unsafe sdevio by %d in %d denied\n",
		caller->p_endpoint, proc_nr_e);
	ಹಿಂತಿರುಗಿ EPERM;
     }
     /* Get and check physical address. */
     vir_buf = m_ptr->m_lsys_krn_sys_sdevio.vec_addr;
     destproc = proc_addr(proc_nr);
  }
     /* current process must be target for phys_* to be OK */

  switch_ವಿಳಾಸ_space(destproc);

  ಆಯ್ಕೆ (req_type)
  {
	ಸ್ಥಿತಿ _DIO_BYTE: ಗಾತ್ರ_2= 1; ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ DIO_ಶಬ್ದ: ಗಾತ್ರ_2= 2; ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ _DIO_LONG: ಗಾತ್ರ_2= 4; ನಿಲ್ಲಿಸು;
	ಪೂರ್ವನಿಯೋಜಿತ: ಗಾತ್ರ_2= 4; ನಿಲ್ಲಿಸು;	/* Be conservative */
  }

  privp= priv(caller);
  ಆದರೆ (privp && privp->s_flags & CHECK_IO_ಪೋರ್ಟ್)
  {
	ಪೋರ್ಟ್= m_ptr->m_lsys_krn_sys_sdevio.ಪೋರ್ಟ್;
	nr_io_range= privp->s_nr_io_range;
	ಗಾಗಿ (i= 0, iorp= privp->s_io_tab; i<nr_io_range; i++, iorp++)
	{
		ಆದರೆ (ಪೋರ್ಟ್ >= iorp->ior_ತಳ && ಪೋರ್ಟ್+ಗಾತ್ರ_2-1 <= iorp->ior_ಎಲ್ಲೆ)
			ನಿಲ್ಲಿಸು;
	}
	ಆದರೆ (i >= nr_io_range)
	{
		ಬರೆ(
		"do_sdevio: I/O port check failed for proc %d, port 0x%x\n",
			m_ptr->m_ಮೂಲ, ಪೋರ್ಟ್);
		retval = EPERM;
		ಹೋಗು return_error;
	}
  }

  ಆದರೆ (ಪೋರ್ಟ್ & (ಗಾತ್ರ_2-1))
  {
	ಬರೆ("do_devio: unaligned port 0x%x (size %d)\n", ಪೋರ್ಟ್, ಗಾತ್ರ_2);
	retval = EPERM;
	ಹೋಗು return_error;
  }

  /* Perform device I/O for bytes and words. Longs are not supported. */
  ಆದರೆ (req_dir == DIO_ಒಳಹರಿವು) { 
      ಆಯ್ಕೆ (req_type) {
      ಸ್ಥಿತಿ _DIO_BYTE: phys_insb(ಪೋರ್ಟ್, vir_buf, ಎಣಿಕೆ_2); ನಿಲ್ಲಿಸು; 
      ಸ್ಥಿತಿ DIO_ಶಬ್ದ: phys_insw(ಪೋರ್ಟ್, vir_buf, ಎಣಿಕೆ_2); ನಿಲ್ಲಿಸು; 
      ಪೂರ್ವನಿಯೋಜಿತ:
  		retval = EINVAL;
		ಹೋಗು return_error;
      } 
  } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (req_dir == _DIO_OUTPUT) { 
      ಆಯ್ಕೆ (req_type) {
      ಸ್ಥಿತಿ _DIO_BYTE: phys_outsb(ಪೋರ್ಟ್, vir_buf, ಎಣಿಕೆ_2); ನಿಲ್ಲಿಸು; 
      ಸ್ಥಿತಿ DIO_ಶಬ್ದ: phys_outsw(ಪೋರ್ಟ್, vir_buf, ಎಣಿಕೆ_2); ನಿಲ್ಲಿಸು; 
      ಪೂರ್ವನಿಯೋಜಿತ:
  		retval = EINVAL;
		ಹೋಗು return_error;
      } 
  }
  ಇಲ್ಲದಿದ್ದರೆ {
	  retval = EINVAL;
	  ಹೋಗು return_error;
  }
  retval = OK;

return_error:
  /* switch back to the address of the process which made the call */
  switch_ವಿಳಾಸ_space(caller);
  ಹಿಂತಿರುಗಿ retval;
}

#ಆದರೆ_ಅಂತ್ಯ /* USE_SDEVIO */
