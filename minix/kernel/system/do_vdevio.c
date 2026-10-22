/* The kernel call implemented in this file:
 *   m_type:	SYS_VDEVIO
 *
 * The parameters for this kernel call are:
 *    m_lsys_krn_sys_vdevio.request	(request input or output)
 *    m_lsys_krn_sys_vdevio.vec_addr	(pointer to port/ value pairs)
 *    m_lsys_krn_sys_vdevio.vec_size	(number of ports to read or write)
 */

#அந்தர்பாவ "kernel/system.h"
#அந்தர்பாவ <minix/devio.h>
#அந்தர்பாவ <minix/endpoint.h>

#எனில் USE_VDEVIO

/* Buffer for SYS_VDEVIO to copy (port,value)-pairs from/ to user. */
நிலையான எழுத்து vdevio_buf[VDEVIO_BUF_SIZE];      
நிலையான pvb_pair_t * மாறிலி pvb = (pvb_pair_t *) vdevio_buf;           
நிலையான pvw_pair_t * மாறிலி pvw = (pvw_pair_t *) vdevio_buf;      
நிலையான pvl_pair_t * மாறிலி pvl = (pvl_pair_t *) vdevio_buf;     

/*===========================================================================*
 *			        do_vdevio                                    *
 *===========================================================================*/
முழு do_vdevio(கட்டமைப்பு proc * caller, message * m_ptr)
{
/* Perform a series of device I/O on behalf of a non-kernel process. The 
 * I/O addresses and I/O values are fetched from and returned to some buffer
 * in user space. The actual I/O is wrapped by lock() and unlock() to prevent
 * that I/O batch from being interrupted.
 * This is the counterpart of do_devio, which performs a single device I/O. 
 */ 
  முழு vec_size;               /* size of vector */
  முழு io_in;                  /* true if input */
  size_t bytes;               /* # bytes to be copied */
  port_t port;
  முழு i, j, io_size, nr_io_range;
  முழு io_dir, io_type;
  கட்டமைப்பு priv *privp;
  கட்டமைப்பு io_range *iorp;
  முழு r;
    
  /* Get the request, size of the request vector, and check the values. */
  io_dir = m_ptr->m_lsys_krn_sys_vdevio.request & _DIO_DIRMASK;
  io_type = m_ptr->m_lsys_krn_sys_vdevio.request & _DIO_TYPEMASK;
  எனில் (io_dir == DIO_உள்ளீடு) io_in = TRUE;
  இல்லையெனில் எனில் (io_dir == _DIO_OUTPUT) io_in = FALSE;
  இல்லையெனில் திரும்பு(EINVAL);
  எனில் ((vec_size = m_ptr->m_lsys_krn_sys_vdevio.vec_size) <= 0) திரும்பு(EINVAL);
  தேர்வு (io_type) {
      நிலை _DIO_BYTE:
	bytes = vec_size * அளவு(pvb_pair_t);
	io_size= அளவு(u8_t);
	நிறுத்து;
      நிலை _DIO_WORD:
	bytes = vec_size * அளவு(pvw_pair_t);
	io_size= அளவு(u16_t);
	நிறுத்து;
      நிலை _DIO_LONG:
	bytes = vec_size * அளவு(pvl_pair_t);
	io_size= அளவு(u32_t);
	நிறுத்து;
      இயல்பு:  திரும்பு(EINVAL);   /* check type once and for all */
  }
  எனில் (bytes > அளவு(vdevio_buf))  திரும்பு(E2BIG);

  /* Copy (port,value)-pairs from user. */
  எனில்((r=data_copy(caller->p_endpoint, m_ptr->m_lsys_krn_sys_vdevio.vec_addr,
    KERNEL, (vir_bytes) vdevio_buf, bytes)) != OK)
	திரும்பு r;

  privp= priv(caller);
  எனில் (privp && (privp->s_flags & CHECK_IO_PORT))
  {
	/* Check whether the I/O is allowed */
	nr_io_range= privp->s_nr_io_range;
	க்கு (i=0; i<vec_size; i++)
	{
		தேர்வு (io_type) {
		நிலை _DIO_BYTE: port= pvb[i].port; நிறுத்து;
		நிலை _DIO_WORD: port= pvw[i].port; நிறுத்து;
		இயல்பு:	port= pvl[i].port; நிறுத்து;
		}
		க்கு (j= 0, iorp= privp->s_io_tab; j<nr_io_range; j++, iorp++)
		{
			எனில் (port >= iorp->ior_base &&
				port+io_size-1 <= iorp->ior_limit)
			{
				நிறுத்து;
			}
		}
		எனில் (j >= nr_io_range)
		{
			printf(
		"do_vdevio: I/O port check failed for proc %d, port 0x%x\n",
				caller->p_endpoint, port);
			திரும்பு EPERM;
		}
	}
  }

  /* Perform actual device I/O for byte, word, and long values */
  தேர்வு (io_type) {
  நிலை _DIO_BYTE: 					 /* byte values */
      எனில் (io_in) க்கு (i=0; i<vec_size; i++) 
		pvb[i].மதிப்பு = inb( pvb[i].port); 
      இல்லையெனில்      க்கு (i=0; i<vec_size; i++)
		outb( pvb[i].port, pvb[i].மதிப்பு); 
      நிறுத்து; 
  நிலை _DIO_WORD:					  /* word values */
      எனில் (io_in)
      {
	க்கு (i=0; i<vec_size; i++)  
	{
		port= pvw[i].port;
		எனில் (port & 1) செல் bad;
		pvw[i].மதிப்பு = inw( pvw[i].port);  
	}
      }
      இல்லையெனில்
      {
	க்கு (i=0; i<vec_size; i++) 
	{
		port= pvw[i].port;
		எனில் (port & 1) செல் bad;
		outw( pvw[i].port, pvw[i].மதிப்பு); 
	}
      }
      நிறுத்து; 
  இயல்பு:            					  /* long values */
      எனில் (io_in)
      {
	க்கு (i=0; i<vec_size; i++)
	{
		port= pvl[i].port;
		எனில் (port & 3) செல் bad;
		pvl[i].மதிப்பு = inl(pvl[i].port);  
	}
      }
      இல்லையெனில்
      {
	க்கு (i=0; i<vec_size; i++)
	{
		port= pvl[i].port;
		எனில் (port & 3) செல் bad;
		outl( pvb[i].port, pvl[i].மதிப்பு); 
	}
      }
  }
    
  /* Almost done, copy back results for input requests. */
  எனில் (io_in) 
	எனில்((r=data_copy(KERNEL, (vir_bytes) vdevio_buf,
	  caller->p_endpoint, m_ptr->m_lsys_krn_sys_vdevio.vec_addr,
	  (phys_bytes) bytes)) != OK)
		திரும்பு r;
  திரும்பு(OK);

bad:
	panic("do_vdevio: unaligned port: %d", port);
	திரும்பு EPERM;
}

#endif /* USE_VDEVIO */

