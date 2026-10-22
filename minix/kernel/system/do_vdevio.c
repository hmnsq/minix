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
স্থির বর্ণ vdevio_buf[VDEVIO_BUF_SIZE];      
স্থির pvb_pair_t * ধ্রুব pvb = (pvb_pair_t *) vdevio_buf;           
স্থির pvw_pair_t * ধ্রুব pvw = (pvw_pair_t *) vdevio_buf;      
স্থির pvl_pair_t * ধ্রুব pvl = (pvl_pair_t *) vdevio_buf;     

/*===========================================================================*
 *			        do_vdevio                                    *
 *===========================================================================*/
পূর্ণাঙ্ক do_vdevio(সংরচনা proc * caller, message * m_ptr)
{
/* Perform a series of device I/O on behalf of a non-kernel process. The 
 * I/O addresses and I/O values are fetched from and returned to some buffer
 * in user space. The actual I/O is wrapped by lock() and unlock() to prevent
 * that I/O batch from being interrupted.
 * This is the counterpart of do_devio, which performs a single device I/O. 
 */ 
  পূর্ণাঙ্ক vec_size;               /* size of vector */
  পূর্ণাঙ্ক io_in;                  /* true if input */
  size_t bytes;               /* # bytes to be copied */
  port_t port;
  পূর্ণাঙ্ক i, j, io_size, nr_io_range;
  পূর্ণাঙ্ক io_dir, io_type;
  সংরচনা priv *privp;
  সংরচনা io_range *iorp;
  পূর্ণাঙ্ক r;
    
  /* Get the request, size of the request vector, and check the values. */
  io_dir = m_ptr->m_lsys_krn_sys_vdevio.request & _DIO_DIRMASK;
  io_type = m_ptr->m_lsys_krn_sys_vdevio.request & _DIO_TYPEMASK;
  যদি (io_dir == DIO_প্রবেশ) io_in = TRUE;
  অন্যথা যদি (io_dir == _DIO_OUTPUT) io_in = FALSE;
  অন্যথা প্রত্যাগম(EINVAL);
  যদি ((vec_size = m_ptr->m_lsys_krn_sys_vdevio.vec_size) <= 0) প্রত্যাগম(EINVAL);
  বিকল্প (io_type) {
      পক্ষ _DIO_BYTE:
	bytes = vec_size * পরিমাণ(pvb_pair_t);
	io_size= পরিমাণ(u8_t);
	ভঙ্গ;
      পক্ষ _DIO_WORD:
	bytes = vec_size * পরিমাণ(pvw_pair_t);
	io_size= পরিমাণ(u16_t);
	ভঙ্গ;
      পক্ষ _DIO_LONG:
	bytes = vec_size * পরিমাণ(pvl_pair_t);
	io_size= পরিমাণ(u32_t);
	ভঙ্গ;
      স্বভাব:  প্রত্যাগম(EINVAL);   /* check type once and for all */
  }
  যদি (bytes > পরিমাণ(vdevio_buf))  প্রত্যাগম(E2BIG);

  /* Copy (port,value)-pairs from user. */
  যদি((r=data_copy(caller->p_endpoint, m_ptr->m_lsys_krn_sys_vdevio.vec_addr,
    KERNEL, (vir_bytes) vdevio_buf, bytes)) != OK)
	প্রত্যাগম r;

  privp= priv(caller);
  যদি (privp && (privp->s_flags & CHECK_IO_PORT))
  {
	/* Check whether the I/O is allowed */
	nr_io_range= privp->s_nr_io_range;
	প্রতি (i=0; i<vec_size; i++)
	{
		বিকল্প (io_type) {
		পক্ষ _DIO_BYTE: port= pvb[i].port; ভঙ্গ;
		পক্ষ _DIO_WORD: port= pvw[i].port; ভঙ্গ;
		স্বভাব:	port= pvl[i].port; ভঙ্গ;
		}
		প্রতি (j= 0, iorp= privp->s_io_tab; j<nr_io_range; j++, iorp++)
		{
			যদি (port >= iorp->ior_base &&
				port+io_size-1 <= iorp->ior_limit)
			{
				ভঙ্গ;
			}
		}
		যদি (j >= nr_io_range)
		{
			printf(
		"do_vdevio: I/O port check failed for proc %d, port 0x%x\n",
				caller->p_endpoint, port);
			প্রত্যাগম EPERM;
		}
	}
  }

  /* Perform actual device I/O for byte, word, and long values */
  বিকল্প (io_type) {
  পক্ষ _DIO_BYTE: 					 /* byte values */
      যদি (io_in) প্রতি (i=0; i<vec_size; i++) 
		pvb[i].মান = inb( pvb[i].port); 
      অন্যথা      প্রতি (i=0; i<vec_size; i++)
		outb( pvb[i].port, pvb[i].মান); 
      ভঙ্গ; 
  পক্ষ _DIO_WORD:					  /* word values */
      যদি (io_in)
      {
	প্রতি (i=0; i<vec_size; i++)  
	{
		port= pvw[i].port;
		যদি (port & 1) গচ্ছ bad;
		pvw[i].মান = inw( pvw[i].port);  
	}
      }
      অন্যথা
      {
	প্রতি (i=0; i<vec_size; i++) 
	{
		port= pvw[i].port;
		যদি (port & 1) গচ্ছ bad;
		outw( pvw[i].port, pvw[i].মান); 
	}
      }
      ভঙ্গ; 
  স্বভাব:            					  /* long values */
      যদি (io_in)
      {
	প্রতি (i=0; i<vec_size; i++)
	{
		port= pvl[i].port;
		যদি (port & 3) গচ্ছ bad;
		pvl[i].মান = inl(pvl[i].port);  
	}
      }
      অন্যথা
      {
	প্রতি (i=0; i<vec_size; i++)
	{
		port= pvl[i].port;
		যদি (port & 3) গচ্ছ bad;
		outl( pvb[i].port, pvl[i].মান); 
	}
      }
  }
    
  /* Almost done, copy back results for input requests. */
  যদি (io_in) 
	যদি((r=data_copy(KERNEL, (vir_bytes) vdevio_buf,
	  caller->p_endpoint, m_ptr->m_lsys_krn_sys_vdevio.vec_addr,
	  (phys_bytes) bytes)) != OK)
		প্রত্যাগম r;
  প্রত্যাগম(OK);

bad:
	panic("do_vdevio: unaligned port: %d", port);
	প্রত্যাগম EPERM;
}

#endif /* USE_VDEVIO */

