/* The kernel call implemented in this file:
 *   m_type:	SYS_VDEVIO
 *
 * The parameters for this kernel call are:
 *    m_lsys_krn_sys_vdevio.request	(request input or output)
 *    m_lsys_krn_sys_vdevio.vec_addr	(pointer to port/ value pairs)
 *    m_lsys_krn_sys_vdevio.vec_size	(number of ports to read or write)
 */

#ضمن "kernel/system.h"
#ضمن <minix/devio.h>
#ضمن <minix/endpoint.h>

#إذا USE_VDEVIO

/* Buffer for SYS_VDEVIO to copy (port,value)-pairs from/ to user. */
ساكن حرف vdevio_buf[VDEVIO_BUF_SIZE];      
ساكن pvb_pair_t * ثابت pvb = (pvb_pair_t *) vdevio_buf;           
ساكن pvw_pair_t * ثابت pvw = (pvw_pair_t *) vdevio_buf;      
ساكن pvl_pair_t * ثابت pvl = (pvl_pair_t *) vdevio_buf;     

/*===========================================================================*
 *			        do_vdevio                                    *
 *===========================================================================*/
صحيح do_vdevio(بنية proc * caller, message * m_ptr)
{
/* Perform a series of device I/O on behalf of a non-kernel process. The 
 * I/O addresses and I/O values are fetched from and returned to some buffer
 * in user space. The actual I/O is wrapped by lock() and unlock() to prevent
 * that I/O batch from being interrupted.
 * This is the counterpart of do_devio, which performs a single device I/O. 
 */ 
  صحيح vec_size;               /* size of vector */
  صحيح io_in;                  /* true if input */
  size_t bytes;               /* # bytes to be copied */
  port_t port;
  صحيح i, j, io_size, nr_io_range;
  صحيح io_dir, io_type;
  بنية priv *privp;
  بنية io_range *iorp;
  صحيح r;
    
  /* Get the request, size of the request vector, and check the values. */
  io_dir = m_ptr->m_lsys_krn_sys_vdevio.request & _DIO_DIRMASK;
  io_type = m_ptr->m_lsys_krn_sys_vdevio.request & _DIO_TYPEMASK;
  إذا (io_dir == DIO_إدخال) io_in = TRUE;
  وإلا إذا (io_dir == _DIO_OUTPUT) io_in = FALSE;
  وإلا إرجاع(EINVAL);
  إذا ((vec_size = m_ptr->m_lsys_krn_sys_vdevio.vec_size) <= 0) إرجاع(EINVAL);
  بدل (io_type) {
      حالة _DIO_BYTE:
	bytes = vec_size * حجم(pvb_pair_t);
	io_size= حجم(u8_t);
	اكسر;
      حالة _DIO_WORD:
	bytes = vec_size * حجم(pvw_pair_t);
	io_size= حجم(u16_t);
	اكسر;
      حالة _DIO_LONG:
	bytes = vec_size * حجم(pvl_pair_t);
	io_size= حجم(u32_t);
	اكسر;
      افتراضي:  إرجاع(EINVAL);   /* check type once and for all */
  }
  إذا (bytes > حجم(vdevio_buf))  إرجاع(E2BIG);

  /* Copy (port,value)-pairs from user. */
  إذا((r=data_copy(caller->p_endpoint, m_ptr->m_lsys_krn_sys_vdevio.vec_addr,
    KERNEL, (vir_bytes) vdevio_buf, bytes)) != OK)
	إرجاع r;

  privp= priv(caller);
  إذا (privp && (privp->s_flags & CHECK_IO_PORT))
  {
	/* Check whether the I/O is allowed */
	nr_io_range= privp->s_nr_io_range;
	لكل (i=0; i<vec_size; i++)
	{
		بدل (io_type) {
		حالة _DIO_BYTE: port= pvb[i].port; اكسر;
		حالة _DIO_WORD: port= pvw[i].port; اكسر;
		افتراضي:	port= pvl[i].port; اكسر;
		}
		لكل (j= 0, iorp= privp->s_io_tab; j<nr_io_range; j++, iorp++)
		{
			إذا (port >= iorp->ior_base &&
				port+io_size-1 <= iorp->ior_limit)
			{
				اكسر;
			}
		}
		إذا (j >= nr_io_range)
		{
			printf(
		"do_vdevio: I/O port check failed for proc %d, port 0x%x\n",
				caller->p_endpoint, port);
			إرجاع EPERM;
		}
	}
  }

  /* Perform actual device I/O for byte, word, and long values */
  بدل (io_type) {
  حالة _DIO_BYTE: 					 /* byte values */
      إذا (io_in) لكل (i=0; i<vec_size; i++) 
		pvb[i].قيمة = inb( pvb[i].port); 
      وإلا      لكل (i=0; i<vec_size; i++)
		outb( pvb[i].port, pvb[i].قيمة); 
      اكسر; 
  حالة _DIO_WORD:					  /* word values */
      إذا (io_in)
      {
	لكل (i=0; i<vec_size; i++)  
	{
		port= pvw[i].port;
		إذا (port & 1) اذهب bad;
		pvw[i].قيمة = inw( pvw[i].port);  
	}
      }
      وإلا
      {
	لكل (i=0; i<vec_size; i++) 
	{
		port= pvw[i].port;
		إذا (port & 1) اذهب bad;
		outw( pvw[i].port, pvw[i].قيمة); 
	}
      }
      اكسر; 
  افتراضي:            					  /* long values */
      إذا (io_in)
      {
	لكل (i=0; i<vec_size; i++)
	{
		port= pvl[i].port;
		إذا (port & 3) اذهب bad;
		pvl[i].قيمة = inl(pvl[i].port);  
	}
      }
      وإلا
      {
	لكل (i=0; i<vec_size; i++)
	{
		port= pvl[i].port;
		إذا (port & 3) اذهب bad;
		outl( pvb[i].port, pvl[i].قيمة); 
	}
      }
  }
    
  /* Almost done, copy back results for input requests. */
  إذا (io_in) 
	إذا((r=data_copy(KERNEL, (vir_bytes) vdevio_buf,
	  caller->p_endpoint, m_ptr->m_lsys_krn_sys_vdevio.vec_addr,
	  (phys_bytes) bytes)) != OK)
		إرجاع r;
  إرجاع(OK);

bad:
	panic("do_vdevio: unaligned port: %d", port);
	إرجاع EPERM;
}

#endif /* USE_VDEVIO */

