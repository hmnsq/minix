/* The kernel call implemented in this file:
 *   m_type:	SYS_VDEVIO
 *
 * The parameters for this kernel call are:
 *    m_lsys_krn_sys_vdevio.request	(request input or output)
 *    m_lsys_krn_sys_vdevio.vec_addr	(pointer to port/ value pairs)
 *    m_lsys_krn_sys_vdevio.vec_size	(number of ports to read or write)
 */

#अन्तर्भाव "kernel/system.h"
#अन्तर्भाव <minix/devio.h>
#अन्तर्भाव <minix/endpoint.h>

#यदि USE_VDEVIO

/* Buffer for SYS_VDEVIO to copy (port,value)-pairs from/ to user. */
स्थिर वर्ण vdevio_buf[VDEVIO_BUF_SIZE];      
स्थिर pvb_pair_t * ध्रुव pvb = (pvb_pair_t *) vdevio_buf;           
स्थिर pvw_pair_t * ध्रुव pvw = (pvw_pair_t *) vdevio_buf;      
स्थिर pvl_pair_t * ध्रुव pvl = (pvl_pair_t *) vdevio_buf;     

/*===========================================================================*
 *			        do_vdevio                                    *
 *===========================================================================*/
पूर्णाङ्क do_vdevio(संरचना proc * caller, message * m_ptr)
{
/* Perform a series of device I/O on behalf of a non-kernel process. The 
 * I/O addresses and I/O values are fetched from and returned to some buffer
 * in user space. The actual I/O is wrapped by lock() and unlock() to prevent
 * that I/O batch from being interrupted.
 * This is the counterpart of do_devio, which performs a single device I/O. 
 */ 
  पूर्णाङ्क vec_size;               /* size of vector */
  पूर्णाङ्क io_in;                  /* true if input */
  size_t bytes;               /* # bytes to be copied */
  port_t port;
  पूर्णाङ्क i, j, io_size, nr_io_range;
  पूर्णाङ्क io_dir, io_type;
  संरचना priv *privp;
  संरचना io_range *iorp;
  पूर्णाङ्क r;
    
  /* Get the request, size of the request vector, and check the values. */
  io_dir = m_ptr->m_lsys_krn_sys_vdevio.request & _DIO_DIRMASK;
  io_type = m_ptr->m_lsys_krn_sys_vdevio.request & _DIO_TYPEMASK;
  यदि (io_dir == DIO_आदान) io_in = TRUE;
  अन्यथा यदि (io_dir == _DIO_OUTPUT) io_in = FALSE;
  अन्यथा प्रत्यागम(EINVAL);
  यदि ((vec_size = m_ptr->m_lsys_krn_sys_vdevio.vec_size) <= 0) प्रत्यागम(EINVAL);
  विकल्प (io_type) {
      पक्ष _DIO_BYTE:
	bytes = vec_size * परिमाण(pvb_pair_t);
	io_size= परिमाण(u8_t);
	भङ्ग;
      पक्ष _DIO_WORD:
	bytes = vec_size * परिमाण(pvw_pair_t);
	io_size= परिमाण(u16_t);
	भङ्ग;
      पक्ष _DIO_LONG:
	bytes = vec_size * परिमाण(pvl_pair_t);
	io_size= परिमाण(u32_t);
	भङ्ग;
      स्वभाव:  प्रत्यागम(EINVAL);   /* check type once and for all */
  }
  यदि (bytes > परिमाण(vdevio_buf))  प्रत्यागम(E2BIG);

  /* Copy (port,value)-pairs from user. */
  यदि((r=data_copy(caller->p_endpoint, m_ptr->m_lsys_krn_sys_vdevio.vec_addr,
    KERNEL, (vir_bytes) vdevio_buf, bytes)) != OK)
	प्रत्यागम r;

  privp= priv(caller);
  यदि (privp && (privp->s_flags & CHECK_IO_PORT))
  {
	/* Check whether the I/O is allowed */
	nr_io_range= privp->s_nr_io_range;
	प्रति (i=0; i<vec_size; i++)
	{
		विकल्प (io_type) {
		पक्ष _DIO_BYTE: port= pvb[i].port; भङ्ग;
		पक्ष _DIO_WORD: port= pvw[i].port; भङ्ग;
		स्वभाव:	port= pvl[i].port; भङ्ग;
		}
		प्रति (j= 0, iorp= privp->s_io_tab; j<nr_io_range; j++, iorp++)
		{
			यदि (port >= iorp->ior_base &&
				port+io_size-1 <= iorp->ior_limit)
			{
				भङ्ग;
			}
		}
		यदि (j >= nr_io_range)
		{
			printf(
		"do_vdevio: I/O port check failed for proc %d, port 0x%x\n",
				caller->p_endpoint, port);
			प्रत्यागम EPERM;
		}
	}
  }

  /* Perform actual device I/O for byte, word, and long values */
  विकल्प (io_type) {
  पक्ष _DIO_BYTE: 					 /* byte values */
      यदि (io_in) प्रति (i=0; i<vec_size; i++) 
		pvb[i].मूल्य = inb( pvb[i].port); 
      अन्यथा      प्रति (i=0; i<vec_size; i++)
		outb( pvb[i].port, pvb[i].मूल्य); 
      भङ्ग; 
  पक्ष _DIO_WORD:					  /* word values */
      यदि (io_in)
      {
	प्रति (i=0; i<vec_size; i++)  
	{
		port= pvw[i].port;
		यदि (port & 1) गच्छ bad;
		pvw[i].मूल्य = inw( pvw[i].port);  
	}
      }
      अन्यथा
      {
	प्रति (i=0; i<vec_size; i++) 
	{
		port= pvw[i].port;
		यदि (port & 1) गच्छ bad;
		outw( pvw[i].port, pvw[i].मूल्य); 
	}
      }
      भङ्ग; 
  स्वभाव:            					  /* long values */
      यदि (io_in)
      {
	प्रति (i=0; i<vec_size; i++)
	{
		port= pvl[i].port;
		यदि (port & 3) गच्छ bad;
		pvl[i].मूल्य = inl(pvl[i].port);  
	}
      }
      अन्यथा
      {
	प्रति (i=0; i<vec_size; i++)
	{
		port= pvl[i].port;
		यदि (port & 3) गच्छ bad;
		outl( pvb[i].port, pvl[i].मूल्य); 
	}
      }
  }
    
  /* Almost done, copy back results for input requests. */
  यदि (io_in) 
	यदि((r=data_copy(KERNEL, (vir_bytes) vdevio_buf,
	  caller->p_endpoint, m_ptr->m_lsys_krn_sys_vdevio.vec_addr,
	  (phys_bytes) bytes)) != OK)
		प्रत्यागम r;
  प्रत्यागम(OK);

bad:
	panic("do_vdevio: unaligned port: %d", port);
	प्रत्यागम EPERM;
}

#endif /* USE_VDEVIO */

