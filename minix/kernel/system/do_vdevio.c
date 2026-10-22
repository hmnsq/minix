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

#אם USE_VDEVIO

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
  אם (io_dir == _DIO_INPUT) io_in = TRUE;
  אחרת אם (io_dir == _DIO_OUTPUT) io_in = FALSE;
  אחרת החזר(EINVAL);
  אם ((vec_size = m_ptr->m_lsys_krn_sys_vdevio.vec_size) <= 0) החזר(EINVAL);
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
      افتراضي:  החזר(EINVAL);   /* check type once and for all */
  }
  אם (bytes > حجم(vdevio_buf))  החזר(E2BIG);

  /* Copy (port,value)-pairs from user. */
  אם((r=data_copy(caller->p_endpoint, m_ptr->m_lsys_krn_sys_vdevio.vec_addr,
    KERNEL, (vir_bytes) vdevio_buf, bytes)) != OK)
	החזר r;

  privp= priv(caller);
  אם (privp && (privp->s_flags & CHECK_IO_PORT))
  {
	/* Check whether the I/O is allowed */
	nr_io_range= privp->s_nr_io_range;
	עבור (i=0; i<vec_size; i++)
	{
		بدل (io_type) {
		حالة _DIO_BYTE: port= pvb[i].port; اكسر;
		حالة _DIO_WORD: port= pvw[i].port; اكسر;
		افتراضي:	port= pvl[i].port; اكسر;
		}
		עבור (j= 0, iorp= privp->s_io_tab; j<nr_io_range; j++, iorp++)
		{
			אם (port >= iorp->ior_base &&
				port+io_size-1 <= iorp->ior_limit)
			{
				اكسر;
			}
		}
		אם (j >= nr_io_range)
		{
			printf(
		"do_vdevio: I/O port check failed for proc %d, port 0x%x\n",
				caller->p_endpoint, port);
			החזר EPERM;
		}
	}
  }

  /* Perform actual device I/O for byte, word, and long values */
  بدل (io_type) {
  حالة _DIO_BYTE: 					 /* byte values */
      אם (io_in) עבור (i=0; i<vec_size; i++) 
		pvb[i].value = inb( pvb[i].port); 
      אחרת      עבור (i=0; i<vec_size; i++)
		outb( pvb[i].port, pvb[i].value); 
      اكسر; 
  حالة _DIO_WORD:					  /* word values */
      אם (io_in)
      {
	עבור (i=0; i<vec_size; i++)  
	{
		port= pvw[i].port;
		אם (port & 1) اذهب bad;
		pvw[i].value = inw( pvw[i].port);  
	}
      }
      אחרת
      {
	עבור (i=0; i<vec_size; i++) 
	{
		port= pvw[i].port;
		אם (port & 1) اذهب bad;
		outw( pvw[i].port, pvw[i].value); 
	}
      }
      اكسر; 
  افتراضي:            					  /* long values */
      אם (io_in)
      {
	עבור (i=0; i<vec_size; i++)
	{
		port= pvl[i].port;
		אם (port & 3) اذهب bad;
		pvl[i].value = inl(pvl[i].port);  
	}
      }
      אחרת
      {
	עבור (i=0; i<vec_size; i++)
	{
		port= pvl[i].port;
		אם (port & 3) اذهب bad;
		outl( pvb[i].port, pvl[i].value); 
	}
      }
  }
    
  /* Almost done, copy back results for input requests. */
  אם (io_in) 
	אם((r=data_copy(KERNEL, (vir_bytes) vdevio_buf,
	  caller->p_endpoint, m_ptr->m_lsys_krn_sys_vdevio.vec_addr,
	  (phys_bytes) bytes)) != OK)
		החזר r;
  החזר(OK);

bad:
	panic("do_vdevio: unaligned port: %d", port);
	החזר EPERM;
}

#endif /* USE_VDEVIO */

