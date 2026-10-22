/* The kernel call implemented in this file:
 *   m_type:	SYS_VDEVIO
 *
 * The parameters for this kernel call are:
 *    m_lsys_krn_sys_vdevio.request	(request input or output)
 *    m_lsys_krn_sys_vdevio.vec_addr	(pointer to port/ value pairs)
 *    m_lsys_krn_sys_vdevio.vec_size	(number of ports to read or write)
 */

#شامل "kernel/system.h"
#شامل <minix/devio.h>
#شامل <minix/endpoint.h>

#اگر USE_VDEVIO

/* Buffer for SYS_VDEVIO to copy (port,value)-pairs from/ to user. */
ایستا نویسه vdevio_buf[VDEVIO_BUF_SIZE];      
ایستا pvb_pair_t * ثابت pvb = (pvb_pair_t *) vdevio_buf;           
ایستا pvw_pair_t * ثابت pvw = (pvw_pair_t *) vdevio_buf;      
ایستا pvl_pair_t * ثابت pvl = (pvl_pair_t *) vdevio_buf;     

/*===========================================================================*
 *			        do_vdevio                                    *
 *===========================================================================*/
صحیح do_vdevio(ساختار proc * caller, message * m_ptr)
{
/* Perform a series of device I/O on behalf of a non-kernel process. The 
 * I/O addresses and I/O values are fetched from and returned to some buffer
 * in user space. The actual I/O is wrapped by lock() and unlock() to prevent
 * that I/O batch from being interrupted.
 * This is the counterpart of do_devio, which performs a single device I/O. 
 */ 
  صحیح vec_size;               /* size of vector */
  صحیح io_in;                  /* true if input */
  size_t bytes;               /* # bytes to be copied */
  port_t port;
  صحیح i, j, io_size, nr_io_range;
  صحیح io_dir, io_type;
  ساختار priv *privp;
  ساختار io_range *iorp;
  صحیح r;
    
  /* Get the request, size of the request vector, and check the values. */
  io_dir = m_ptr->m_lsys_krn_sys_vdevio.request & _DIO_DIRMASK;
  io_type = m_ptr->m_lsys_krn_sys_vdevio.request & _DIO_TYPEMASK;
  اگر (io_dir == DIO_درون_داد) io_in = TRUE;
  وگرنه اگر (io_dir == _DIO_OUTPUT) io_in = FALSE;
  وگرنه بازگشت(EINVAL);
  اگر ((vec_size = m_ptr->m_lsys_krn_sys_vdevio.vec_size) <= 0) بازگشت(EINVAL);
  انتخاب (io_type) {
      حالت _DIO_BYTE:
	bytes = vec_size * اندازه(pvb_pair_t);
	io_size= اندازه(u8_t);
	بشکن;
      حالت _DIO_WORD:
	bytes = vec_size * اندازه(pvw_pair_t);
	io_size= اندازه(u16_t);
	بشکن;
      حالت _DIO_LONG:
	bytes = vec_size * اندازه(pvl_pair_t);
	io_size= اندازه(u32_t);
	بشکن;
      پیشفرض:  بازگشت(EINVAL);   /* check type once and for all */
  }
  اگر (bytes > اندازه(vdevio_buf))  بازگشت(E2BIG);

  /* Copy (port,value)-pairs from user. */
  اگر((r=data_copy(caller->p_endpoint, m_ptr->m_lsys_krn_sys_vdevio.vec_addr,
    KERNEL, (vir_bytes) vdevio_buf, bytes)) != OK)
	بازگشت r;

  privp= priv(caller);
  اگر (privp && (privp->s_flags & CHECK_IO_PORT))
  {
	/* Check whether the I/O is allowed */
	nr_io_range= privp->s_nr_io_range;
	برای (i=0; i<vec_size; i++)
	{
		انتخاب (io_type) {
		حالت _DIO_BYTE: port= pvb[i].port; بشکن;
		حالت _DIO_WORD: port= pvw[i].port; بشکن;
		پیشفرض:	port= pvl[i].port; بشکن;
		}
		برای (j= 0, iorp= privp->s_io_tab; j<nr_io_range; j++, iorp++)
		{
			اگر (port >= iorp->ior_base &&
				port+io_size-1 <= iorp->ior_limit)
			{
				بشکن;
			}
		}
		اگر (j >= nr_io_range)
		{
			printf(
		"do_vdevio: I/O port check failed for proc %d, port 0x%x\n",
				caller->p_endpoint, port);
			بازگشت EPERM;
		}
	}
  }

  /* Perform actual device I/O for byte, word, and long values */
  انتخاب (io_type) {
  حالت _DIO_BYTE: 					 /* byte values */
      اگر (io_in) برای (i=0; i<vec_size; i++) 
		pvb[i].مقدار = inb( pvb[i].port); 
      وگرنه      برای (i=0; i<vec_size; i++)
		outb( pvb[i].port, pvb[i].مقدار); 
      بشکن; 
  حالت _DIO_WORD:					  /* word values */
      اگر (io_in)
      {
	برای (i=0; i<vec_size; i++)  
	{
		port= pvw[i].port;
		اگر (port & 1) برو bad;
		pvw[i].مقدار = inw( pvw[i].port);  
	}
      }
      وگرنه
      {
	برای (i=0; i<vec_size; i++) 
	{
		port= pvw[i].port;
		اگر (port & 1) برو bad;
		outw( pvw[i].port, pvw[i].مقدار); 
	}
      }
      بشکن; 
  پیشفرض:            					  /* long values */
      اگر (io_in)
      {
	برای (i=0; i<vec_size; i++)
	{
		port= pvl[i].port;
		اگر (port & 3) برو bad;
		pvl[i].مقدار = inl(pvl[i].port);  
	}
      }
      وگرنه
      {
	برای (i=0; i<vec_size; i++)
	{
		port= pvl[i].port;
		اگر (port & 3) برو bad;
		outl( pvb[i].port, pvl[i].مقدار); 
	}
      }
  }
    
  /* Almost done, copy back results for input requests. */
  اگر (io_in) 
	اگر((r=data_copy(KERNEL, (vir_bytes) vdevio_buf,
	  caller->p_endpoint, m_ptr->m_lsys_krn_sys_vdevio.vec_addr,
	  (phys_bytes) bytes)) != OK)
		بازگشت r;
  بازگشت(OK);

bad:
	panic("do_vdevio: unaligned port: %d", port);
	بازگشت EPERM;
}

#endif /* USE_VDEVIO */

