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

#ಯದಿ USE_VDEVIO

/* Buffer for SYS_VDEVIO to copy (port,value)-pairs from/ to user. */
ಸ್ಥಿರ ವರ್ಣ vdevio_buf[VDEVIO_BUF_SIZE];      
ಸ್ಥಿರ pvb_pair_t * ಧ್ರುವ pvb = (pvb_pair_t *) vdevio_buf;           
ಸ್ಥಿರ pvw_pair_t * ಧ್ರುವ pvw = (pvw_pair_t *) vdevio_buf;      
ಸ್ಥಿರ pvl_pair_t * ಧ್ರುವ pvl = (pvl_pair_t *) vdevio_buf;     

/*===========================================================================*
 *			        do_vdevio                                    *
 *===========================================================================*/
ಪೂರ್ಣಾಙ್ಕ do_vdevio(ಸಂರಚನಾ proc * caller, message * m_ptr)
{
/* Perform a series of device I/O on behalf of a non-kernel process. The 
 * I/O addresses and I/O values are fetched from and returned to some buffer
 * in user space. The actual I/O is wrapped by lock() and unlock() to prevent
 * that I/O batch from being interrupted.
 * This is the counterpart of do_devio, which performs a single device I/O. 
 */ 
  ಪೂರ್ಣಾಙ್ಕ vec_size;               /* size of vector */
  ಪೂರ್ಣಾಙ್ಕ io_in;                  /* true if input */
  size_t bytes;               /* # bytes to be copied */
  port_t port;
  ಪೂರ್ಣಾಙ್ಕ i, j, io_size, nr_io_range;
  ಪೂರ್ಣಾಙ್ಕ io_dir, io_type;
  ಸಂರಚನಾ priv *privp;
  ಸಂರಚನಾ io_range *iorp;
  ಪೂರ್ಣಾಙ್ಕ r;
    
  /* Get the request, size of the request vector, and check the values. */
  io_dir = m_ptr->m_lsys_krn_sys_vdevio.request & _DIO_DIRMASK;
  io_type = m_ptr->m_lsys_krn_sys_vdevio.request & _DIO_TYPEMASK;
  ಯದಿ (io_dir == DIO_ಒಳಹರಿವು) io_in = TRUE;
  ಅನ್ಯಥಾ ಯದಿ (io_dir == _DIO_OUTPUT) io_in = FALSE;
  ಅನ್ಯಥಾ ಪ್ರತ್ಯಾಗಮ(EINVAL);
  ಯದಿ ((vec_size = m_ptr->m_lsys_krn_sys_vdevio.vec_size) <= 0) ಪ್ರತ್ಯಾಗಮ(EINVAL);
  ವಿಕಲ್ಪ (io_type) {
      ಪಕ್ಷ _DIO_BYTE:
	bytes = vec_size * ಪರಿಮಾಣ(pvb_pair_t);
	io_size= ಪರಿಮಾಣ(u8_t);
	ಭಙ್ಗ;
      ಪಕ್ಷ _DIO_WORD:
	bytes = vec_size * ಪರಿಮಾಣ(pvw_pair_t);
	io_size= ಪರಿಮಾಣ(u16_t);
	ಭಙ್ಗ;
      ಪಕ್ಷ _DIO_LONG:
	bytes = vec_size * ಪರಿಮಾಣ(pvl_pair_t);
	io_size= ಪರಿಮಾಣ(u32_t);
	ಭಙ್ಗ;
      ಸ್ವಭಾವ:  ಪ್ರತ್ಯಾಗಮ(EINVAL);   /* check type once and for all */
  }
  ಯದಿ (bytes > ಪರಿಮಾಣ(vdevio_buf))  ಪ್ರತ್ಯಾಗಮ(E2BIG);

  /* Copy (port,value)-pairs from user. */
  ಯದಿ((r=data_copy(caller->p_endpoint, m_ptr->m_lsys_krn_sys_vdevio.vec_addr,
    KERNEL, (vir_bytes) vdevio_buf, bytes)) != OK)
	ಪ್ರತ್ಯಾಗಮ r;

  privp= priv(caller);
  ಯದಿ (privp && (privp->s_flags & CHECK_IO_PORT))
  {
	/* Check whether the I/O is allowed */
	nr_io_range= privp->s_nr_io_range;
	ಪ್ರತಿ (i=0; i<vec_size; i++)
	{
		ವಿಕಲ್ಪ (io_type) {
		ಪಕ್ಷ _DIO_BYTE: port= pvb[i].port; ಭಙ್ಗ;
		ಪಕ್ಷ _DIO_WORD: port= pvw[i].port; ಭಙ್ಗ;
		ಸ್ವಭಾವ:	port= pvl[i].port; ಭಙ್ಗ;
		}
		ಪ್ರತಿ (j= 0, iorp= privp->s_io_tab; j<nr_io_range; j++, iorp++)
		{
			ಯದಿ (port >= iorp->ior_base &&
				port+io_size-1 <= iorp->ior_limit)
			{
				ಭಙ್ಗ;
			}
		}
		ಯದಿ (j >= nr_io_range)
		{
			printf(
		"do_vdevio: I/O port check failed for proc %d, port 0x%x\n",
				caller->p_endpoint, port);
			ಪ್ರತ್ಯಾಗಮ EPERM;
		}
	}
  }

  /* Perform actual device I/O for byte, word, and long values */
  ವಿಕಲ್ಪ (io_type) {
  ಪಕ್ಷ _DIO_BYTE: 					 /* byte values */
      ಯದಿ (io_in) ಪ್ರತಿ (i=0; i<vec_size; i++) 
		pvb[i].ಮೌಲ್ಯ = inb( pvb[i].port); 
      ಅನ್ಯಥಾ      ಪ್ರತಿ (i=0; i<vec_size; i++)
		outb( pvb[i].port, pvb[i].ಮೌಲ್ಯ); 
      ಭಙ್ಗ; 
  ಪಕ್ಷ _DIO_WORD:					  /* word values */
      ಯದಿ (io_in)
      {
	ಪ್ರತಿ (i=0; i<vec_size; i++)  
	{
		port= pvw[i].port;
		ಯದಿ (port & 1) ಗಚ್ಛ bad;
		pvw[i].ಮೌಲ್ಯ = inw( pvw[i].port);  
	}
      }
      ಅನ್ಯಥಾ
      {
	ಪ್ರತಿ (i=0; i<vec_size; i++) 
	{
		port= pvw[i].port;
		ಯದಿ (port & 1) ಗಚ್ಛ bad;
		outw( pvw[i].port, pvw[i].ಮೌಲ್ಯ); 
	}
      }
      ಭಙ್ಗ; 
  ಸ್ವಭಾವ:            					  /* long values */
      ಯದಿ (io_in)
      {
	ಪ್ರತಿ (i=0; i<vec_size; i++)
	{
		port= pvl[i].port;
		ಯದಿ (port & 3) ಗಚ್ಛ bad;
		pvl[i].ಮೌಲ್ಯ = inl(pvl[i].port);  
	}
      }
      ಅನ್ಯಥಾ
      {
	ಪ್ರತಿ (i=0; i<vec_size; i++)
	{
		port= pvl[i].port;
		ಯದಿ (port & 3) ಗಚ್ಛ bad;
		outl( pvb[i].port, pvl[i].ಮೌಲ್ಯ); 
	}
      }
  }
    
  /* Almost done, copy back results for input requests. */
  ಯದಿ (io_in) 
	ಯದಿ((r=data_copy(KERNEL, (vir_bytes) vdevio_buf,
	  caller->p_endpoint, m_ptr->m_lsys_krn_sys_vdevio.vec_addr,
	  (phys_bytes) bytes)) != OK)
		ಪ್ರತ್ಯಾಗಮ r;
  ಪ್ರತ್ಯಾಗಮ(OK);

bad:
	panic("do_vdevio: unaligned port: %d", port);
	ಪ್ರತ್ಯಾಗಮ EPERM;
}

#endif /* USE_VDEVIO */

