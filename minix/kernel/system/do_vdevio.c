/* The kernel call implemented in this file:
 *   m_type:	SYS_VDEVIO
 *
 * The parameters for this kernel call are:
 *    m_lsys_krn_sys_vdevio.request	(request input or output)
 *    m_lsys_krn_sys_vdevio.vec_addr	(pointer to port/ value pairs)
 *    m_lsys_krn_sys_vdevio.vec_size	(number of ports to read or write)
 */

#ಅನ್ತರ್ಭಾವ "kernel/system.h"
#ಅನ್ತರ್ಭಾವ <minix/devio.h>
#ಅನ್ತರ್ಭಾವ <minix/endpoint.h>

#ಆದರೆ USE_VDEVIO

/* Buffer for SYS_VDEVIO to copy (port,value)-pairs from/ to user. */
ಸ್ಥಿರ ಅಕ್ಷರ vdevio_buf[VDEVIO_BUF_ಗಾತ್ರ];      
ಸ್ಥಿರ pvb_pair_t * ಸ್ಥಿರಾಂಕ pvb = (pvb_pair_t *) vdevio_buf;           
ಸ್ಥಿರ pvw_pair_t * ಸ್ಥಿರಾಂಕ pvw = (pvw_pair_t *) vdevio_buf;      
ಸ್ಥಿರ pvl_pair_t * ಸ್ಥಿರಾಂಕ pvl = (pvl_pair_t *) vdevio_buf;     

/*===========================================================================*
 *			        do_vdevio                                    *
 *===========================================================================*/
ಪೂರ್ಣಾಂಕ do_vdevio(ರಚನೆ proc * caller, ಸಂದೇಶ * m_ptr)
{
/* Perform a series of device I/O on behalf of a non-kernel process. The 
 * I/O addresses and I/O values are fetched from and returned to some buffer
 * in user space. The actual I/O is wrapped by lock() and unlock() to prevent
 * that I/O batch from being interrupted.
 * This is the counterpart of do_devio, which performs a single device I/O. 
 */ 
  ಪೂರ್ಣಾಂಕ vec_ಗಾತ್ರ;               /* size of vector */
  ಪೂರ್ಣಾಂಕ io_ಒಳ;                  /* true if input */
  size_t ಬೈಟುಗಳು;               /* # bytes to be copied */
  ಪೋರ್ಟ್_t ಪೋರ್ಟ್;
  ಪೂರ್ಣಾಂಕ i, j, io_ಗಾತ್ರ, nr_io_range;
  ಪೂರ್ಣಾಂಕ io_dir, io_type;
  ರಚನೆ priv *privp;
  ರಚನೆ io_range *iorp;
  ಪೂರ್ಣಾಂಕ r;
    
  /* Get the request, size of the request vector, and check the values. */
  io_dir = m_ptr->m_lsys_krn_sys_vdevio.request & _DIO_DIRMASK;
  io_type = m_ptr->m_lsys_krn_sys_vdevio.request & _DIO_TYPEMASK;
  ಆದರೆ (io_dir == DIO_ಒಳಹರಿವು) io_ಒಳ = TRUE;
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (io_dir == _DIO_OUTPUT) io_ಒಳ = FALSE;
  ಇಲ್ಲದಿದ್ದರೆ ಹಿಂತಿರುಗಿ(EINVAL);
  ಆದರೆ ((vec_ಗಾತ್ರ = m_ptr->m_lsys_krn_sys_vdevio.vec_ಗಾತ್ರ) <= 0) ಹಿಂತಿರುಗಿ(EINVAL);
  ಆಯ್ಕೆ (io_type) {
      ಸ್ಥಿತಿ _DIO_BYTE:
	ಬೈಟುಗಳು = vec_ಗಾತ್ರ * ಗಾತ್ರ(pvb_pair_t);
	io_ಗಾತ್ರ= ಗಾತ್ರ(u8_t);
	ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ DIO_ಶಬ್ದ:
	ಬೈಟುಗಳು = vec_ಗಾತ್ರ * ಗಾತ್ರ(pvw_pair_t);
	io_ಗಾತ್ರ= ಗಾತ್ರ(u16_t);
	ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ _DIO_LONG:
	ಬೈಟುಗಳು = vec_ಗಾತ್ರ * ಗಾತ್ರ(pvl_pair_t);
	io_ಗಾತ್ರ= ಗಾತ್ರ(u32_t);
	ನಿಲ್ಲಿಸು;
      ಪೂರ್ವನಿಯೋಜಿತ:  ಹಿಂತಿರುಗಿ(EINVAL);   /* check type once and for all */
  }
  ಆದರೆ (ಬೈಟುಗಳು > ಗಾತ್ರ(vdevio_buf))  ಹಿಂತಿರುಗಿ(E2BIG);

  /* Copy (port,value)-pairs from user. */
  ಆದರೆ((r=ದತ್ತಾಂಶ_copy(caller->p_endpoint, m_ptr->m_lsys_krn_sys_vdevio.vec_addr,
    KERNEL, (vir_ಬೈಟುಗಳು) vdevio_buf, ಬೈಟುಗಳು)) != OK)
	ಹಿಂತಿರುಗಿ r;

  privp= priv(caller);
  ಆದರೆ (privp && (privp->s_flags & CHECK_IO_ಪೋರ್ಟ್))
  {
	/* Check whether the I/O is allowed */
	nr_io_range= privp->s_nr_io_range;
	ಗಾಗಿ (i=0; i<vec_ಗಾತ್ರ; i++)
	{
		ಆಯ್ಕೆ (io_type) {
		ಸ್ಥಿತಿ _DIO_BYTE: ಪೋರ್ಟ್= pvb[i].ಪೋರ್ಟ್; ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ DIO_ಶಬ್ದ: ಪೋರ್ಟ್= pvw[i].ಪೋರ್ಟ್; ನಿಲ್ಲಿಸು;
		ಪೂರ್ವನಿಯೋಜಿತ:	ಪೋರ್ಟ್= pvl[i].ಪೋರ್ಟ್; ನಿಲ್ಲಿಸು;
		}
		ಗಾಗಿ (j= 0, iorp= privp->s_io_tab; j<nr_io_range; j++, iorp++)
		{
			ಆದರೆ (ಪೋರ್ಟ್ >= iorp->ior_ತಳ &&
				ಪೋರ್ಟ್+io_ಗಾತ್ರ-1 <= iorp->ior_ಎಲ್ಲೆ)
			{
				ನಿಲ್ಲಿಸು;
			}
		}
		ಆದರೆ (j >= nr_io_range)
		{
			ಬರೆ(
		"do_vdevio: I/O port check failed for proc %d, port 0x%x\n",
				caller->p_endpoint, ಪೋರ್ಟ್);
			ಹಿಂತಿರುಗಿ EPERM;
		}
	}
  }

  /* Perform actual device I/O for byte, word, and long values */
  ಆಯ್ಕೆ (io_type) {
  ಸ್ಥಿತಿ _DIO_BYTE: 					 /* byte values */
      ಆದರೆ (io_ಒಳ) ಗಾಗಿ (i=0; i<vec_ಗಾತ್ರ; i++) 
		pvb[i].ಮೌಲ್ಯ = inb( pvb[i].ಪೋರ್ಟ್); 
      ಇಲ್ಲದಿದ್ದರೆ      ಗಾಗಿ (i=0; i<vec_ಗಾತ್ರ; i++)
		outb( pvb[i].ಪೋರ್ಟ್, pvb[i].ಮೌಲ್ಯ); 
      ನಿಲ್ಲಿಸು; 
  ಸ್ಥಿತಿ DIO_ಶಬ್ದ:					  /* word values */
      ಆದರೆ (io_ಒಳ)
      {
	ಗಾಗಿ (i=0; i<vec_ಗಾತ್ರ; i++)  
	{
		ಪೋರ್ಟ್= pvw[i].ಪೋರ್ಟ್;
		ಆದರೆ (ಪೋರ್ಟ್ & 1) ಹೋಗು bad;
		pvw[i].ಮೌಲ್ಯ = inw( pvw[i].ಪೋರ್ಟ್);  
	}
      }
      ಇಲ್ಲದಿದ್ದರೆ
      {
	ಗಾಗಿ (i=0; i<vec_ಗಾತ್ರ; i++) 
	{
		ಪೋರ್ಟ್= pvw[i].ಪೋರ್ಟ್;
		ಆದರೆ (ಪೋರ್ಟ್ & 1) ಹೋಗು bad;
		outw( pvw[i].ಪೋರ್ಟ್, pvw[i].ಮೌಲ್ಯ); 
	}
      }
      ನಿಲ್ಲಿಸು; 
  ಪೂರ್ವನಿಯೋಜಿತ:            					  /* long values */
      ಆದರೆ (io_ಒಳ)
      {
	ಗಾಗಿ (i=0; i<vec_ಗಾತ್ರ; i++)
	{
		ಪೋರ್ಟ್= pvl[i].ಪೋರ್ಟ್;
		ಆದರೆ (ಪೋರ್ಟ್ & 3) ಹೋಗು bad;
		pvl[i].ಮೌಲ್ಯ = inl(pvl[i].ಪೋರ್ಟ್);  
	}
      }
      ಇಲ್ಲದಿದ್ದರೆ
      {
	ಗಾಗಿ (i=0; i<vec_ಗಾತ್ರ; i++)
	{
		ಪೋರ್ಟ್= pvl[i].ಪೋರ್ಟ್;
		ಆದರೆ (ಪೋರ್ಟ್ & 3) ಹೋಗು bad;
		outl( pvb[i].ಪೋರ್ಟ್, pvl[i].ಮೌಲ್ಯ); 
	}
      }
  }
    
  /* Almost done, copy back results for input requests. */
  ಆದರೆ (io_ಒಳ) 
	ಆದರೆ((r=ದತ್ತಾಂಶ_copy(KERNEL, (vir_ಬೈಟುಗಳು) vdevio_buf,
	  caller->p_endpoint, m_ptr->m_lsys_krn_sys_vdevio.vec_addr,
	  (phys_ಬೈಟುಗಳು) ಬೈಟುಗಳು)) != OK)
		ಹಿಂತಿರುಗಿ r;
  ಹಿಂತಿರುಗಿ(OK);

bad:
	panic("do_vdevio: unaligned port: %d", ಪೋರ್ಟ್);
	ಹಿಂತಿರುಗಿ EPERM;
}

#ಆದರೆ_ಅಂತ್ಯ /* USE_VDEVIO */

