/* The kernel call implemented in this file:
 *   m_type:	SYS_VDEVIO
 *
 * The parameters for this kernel call are:
 *    m_lsys_krn_sys_vdevio.request	(request input or output)
 *    m_lsys_krn_sys_vdevio.vec_addr	(pointer to port/ value pairs)
 *    m_lsys_krn_sys_vdevio.vec_size	(number of ports to read or write)
 */

#অন্তর্ভাব "kernel/system.h"
#অন্তর্ভাব <minix/devio.h>
#অন্তর্ভাব <minix/endpoint.h>

#যদি USE_VDEVIO

/* Buffer for SYS_VDEVIO to copy (port,value)-pairs from/ to user. */
স্থির অক্ষর vdevio_buf[VDEVIO_BUF_আকার];      
স্থির pvb_pair_t * ধ্রুবক pvb = (pvb_pair_t *) vdevio_buf;           
স্থির pvw_pair_t * ধ্রুবক pvw = (pvw_pair_t *) vdevio_buf;      
স্থির pvl_pair_t * ধ্রুবক pvl = (pvl_pair_t *) vdevio_buf;     

/*===========================================================================*
 *			        do_vdevio                                    *
 *===========================================================================*/
পূর্ণসংখ্যা do_vdevio(কাঠামো proc * caller, message * m_ptr)
{
/* Perform a series of device I/O on behalf of a non-kernel process. The 
 * I/O addresses and I/O values are fetched from and returned to some buffer
 * in user space. The actual I/O is wrapped by lock() and unlock() to prevent
 * that I/O batch from being interrupted.
 * This is the counterpart of do_devio, which performs a single device I/O. 
 */ 
  পূর্ণসংখ্যা vec_আকার;               /* size of vector */
  পূর্ণসংখ্যা io_in;                  /* true if input */
  size_t bytes;               /* # bytes to be copied */
  port_t port;
  পূর্ণসংখ্যা i, j, io_আকার, nr_io_range;
  পূর্ণসংখ্যা io_dir, io_type;
  কাঠামো priv *privp;
  কাঠামো io_range *iorp;
  পূর্ণসংখ্যা r;
    
  /* Get the request, size of the request vector, and check the values. */
  io_dir = m_ptr->m_lsys_krn_sys_vdevio.request & _DIO_DIRMASK;
  io_type = m_ptr->m_lsys_krn_sys_vdevio.request & _DIO_TYPEMASK;
  যদি (io_dir == DIO_প্রবেশ) io_in = TRUE;
  অন্যথা যদি (io_dir == _DIO_OUTPUT) io_in = FALSE;
  অন্যথা ফেরত(EINVAL);
  যদি ((vec_আকার = m_ptr->m_lsys_krn_sys_vdevio.vec_আকার) <= 0) ফেরত(EINVAL);
  নির্বাচন (io_type) {
      ক্ষেত্র _DIO_BYTE:
	bytes = vec_আকার * আকার(pvb_pair_t);
	io_আকার= আকার(u8_t);
	বিরতি;
      ক্ষেত্র _DIO_WORD:
	bytes = vec_আকার * আকার(pvw_pair_t);
	io_আকার= আকার(u16_t);
	বিরতি;
      ক্ষেত্র _DIO_LONG:
	bytes = vec_আকার * আকার(pvl_pair_t);
	io_আকার= আকার(u32_t);
	বিরতি;
      পূর্বনির্ধারিত:  ফেরত(EINVAL);   /* check type once and for all */
  }
  যদি (bytes > আকার(vdevio_buf))  ফেরত(E2BIG);

  /* Copy (port,value)-pairs from user. */
  যদি((r=তথ্য_copy(caller->p_endpoint, m_ptr->m_lsys_krn_sys_vdevio.vec_addr,
    KERNEL, (vir_bytes) vdevio_buf, bytes)) != OK)
	ফেরত r;

  privp= priv(caller);
  যদি (privp && (privp->s_flags & CHECK_IO_PORT))
  {
	/* Check whether the I/O is allowed */
	nr_io_range= privp->s_nr_io_range;
	জন্য (i=0; i<vec_আকার; i++)
	{
		নির্বাচন (io_type) {
		ক্ষেত্র _DIO_BYTE: port= pvb[i].port; বিরতি;
		ক্ষেত্র _DIO_WORD: port= pvw[i].port; বিরতি;
		পূর্বনির্ধারিত:	port= pvl[i].port; বিরতি;
		}
		জন্য (j= 0, iorp= privp->s_io_tab; j<nr_io_range; j++, iorp++)
		{
			যদি (port >= iorp->ior_base &&
				port+io_আকার-1 <= iorp->ior_limit)
			{
				বিরতি;
			}
		}
		যদি (j >= nr_io_range)
		{
			printf(
		"do_vdevio: I/O port check failed for proc %d, port 0x%x\n",
				caller->p_endpoint, port);
			ফেরত EPERM;
		}
	}
  }

  /* Perform actual device I/O for byte, word, and long values */
  নির্বাচন (io_type) {
  ক্ষেত্র _DIO_BYTE: 					 /* byte values */
      যদি (io_in) জন্য (i=0; i<vec_আকার; i++) 
		pvb[i].মান = inb( pvb[i].port); 
      অন্যথা      জন্য (i=0; i<vec_আকার; i++)
		outb( pvb[i].port, pvb[i].মান); 
      বিরতি; 
  ক্ষেত্র _DIO_WORD:					  /* word values */
      যদি (io_in)
      {
	জন্য (i=0; i<vec_আকার; i++)  
	{
		port= pvw[i].port;
		যদি (port & 1) যাও bad;
		pvw[i].মান = inw( pvw[i].port);  
	}
      }
      অন্যথা
      {
	জন্য (i=0; i<vec_আকার; i++) 
	{
		port= pvw[i].port;
		যদি (port & 1) যাও bad;
		outw( pvw[i].port, pvw[i].মান); 
	}
      }
      বিরতি; 
  পূর্বনির্ধারিত:            					  /* long values */
      যদি (io_in)
      {
	জন্য (i=0; i<vec_আকার; i++)
	{
		port= pvl[i].port;
		যদি (port & 3) যাও bad;
		pvl[i].মান = inl(pvl[i].port);  
	}
      }
      অন্যথা
      {
	জন্য (i=0; i<vec_আকার; i++)
	{
		port= pvl[i].port;
		যদি (port & 3) যাও bad;
		outl( pvb[i].port, pvl[i].মান); 
	}
      }
  }
    
  /* Almost done, copy back results for input requests. */
  যদি (io_in) 
	যদি((r=তথ্য_copy(KERNEL, (vir_bytes) vdevio_buf,
	  caller->p_endpoint, m_ptr->m_lsys_krn_sys_vdevio.vec_addr,
	  (phys_bytes) bytes)) != OK)
		ফেরত r;
  ফেরত(OK);

bad:
	panic("do_vdevio: unaligned port: %d", port);
	ফেরত EPERM;
}

#endif /* USE_VDEVIO */

