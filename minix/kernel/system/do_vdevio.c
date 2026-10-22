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
ساكن حرف vdevio_buf[VDEVIO_BUF_اندازه];      
ساكن pvb_pair_t * ثابت pvb = (pvb_pair_t *) vdevio_buf;           
ساكن pvw_pair_t * ثابت pvw = (pvw_pair_t *) vdevio_buf;      
ساكن pvl_pair_t * ثابت pvl = (pvl_pair_t *) vdevio_buf;     

/*===========================================================================*
 *			        do_vdevio                                    *
 *===========================================================================*/
صحيح do_vdevio(بنية proc * caller, پیغام * m_ptr)
{
/* Perform a series of device I/O on behalf of a non-kernel process. The 
 * I/O addresses and I/O values are fetched from and returned to some buffer
 * in user space. The actual I/O is wrapped by lock() and unlock() to prevent
 * that I/O batch from being interrupted.
 * This is the counterpart of do_devio, which performs a single device I/O. 
 */ 
  صحيح vec_اندازه;               /* size of vector */
  صحيح io_دننه;                  /* true if input */
  size_t بایټونه;               /* # bytes to be copied */
  پورټ_t پورټ;
  صحيح i, j, io_اندازه, nr_io_range;
  صحيح io_dir, io_type;
  بنية priv *privp;
  بنية io_range *iorp;
  صحيح r;
    
  /* Get the request, size of the request vector, and check the values. */
  io_dir = m_ptr->m_lsys_krn_sys_vdevio.request & _DIO_DIRMASK;
  io_type = m_ptr->m_lsys_krn_sys_vdevio.request & _DIO_TYPEMASK;
  إذا (io_dir == DIO_ننوت) io_دننه = TRUE;
  وإلا إذا (io_dir == _DIO_OUTPUT) io_دننه = FALSE;
  وإلا أرجع(EINVAL);
  إذا ((vec_اندازه = m_ptr->m_lsys_krn_sys_vdevio.vec_اندازه) <= 0) أرجع(EINVAL);
  بدل (io_type) {
      حالة _DIO_BYTE:
	بایټونه = vec_اندازه * حجم(pvb_pair_t);
	io_اندازه= حجم(u8_t);
	اكسر;
      حالة DIO_کلمه:
	بایټونه = vec_اندازه * حجم(pvw_pair_t);
	io_اندازه= حجم(u16_t);
	اكسر;
      حالة _DIO_LONG:
	بایټونه = vec_اندازه * حجم(pvl_pair_t);
	io_اندازه= حجم(u32_t);
	اكسر;
      افتراضي:  أرجع(EINVAL);   /* check type once and for all */
  }
  إذا (بایټونه > حجم(vdevio_buf))  أرجع(E2BIG);

  /* Copy (port,value)-pairs from user. */
  إذا((r=معلومات_copy(caller->p_endpoint, m_ptr->m_lsys_krn_sys_vdevio.vec_addr,
    KERNEL, (vir_بایټونه) vdevio_buf, بایټونه)) != OK)
	أرجع r;

  privp= priv(caller);
  إذا (privp && (privp->s_flags & CHECK_IO_پورټ))
  {
	/* Check whether the I/O is allowed */
	nr_io_range= privp->s_nr_io_range;
	لكل (i=0; i<vec_اندازه; i++)
	{
		بدل (io_type) {
		حالة _DIO_BYTE: پورټ= pvb[i].پورټ; اكسر;
		حالة DIO_کلمه: پورټ= pvw[i].پورټ; اكسر;
		افتراضي:	پورټ= pvl[i].پورټ; اكسر;
		}
		لكل (j= 0, iorp= privp->s_io_tab; j<nr_io_range; j++, iorp++)
		{
			إذا (پورټ >= iorp->ior_بنسټ &&
				پورټ+io_اندازه-1 <= iorp->ior_حد)
			{
				اكسر;
			}
		}
		إذا (j >= nr_io_range)
		{
			چاپ_کړه(
		"do_vdevio: I/O port check failed for proc %d, port 0x%x\n",
				caller->p_endpoint, پورټ);
			أرجع EPERM;
		}
	}
  }

  /* Perform actual device I/O for byte, word, and long values */
  بدل (io_type) {
  حالة _DIO_BYTE: 					 /* byte values */
      إذا (io_دننه) لكل (i=0; i<vec_اندازه; i++) 
		pvb[i].ارزښت = inb( pvb[i].پورټ); 
      وإلا      لكل (i=0; i<vec_اندازه; i++)
		outb( pvb[i].پورټ, pvb[i].ارزښت); 
      اكسر; 
  حالة DIO_کلمه:					  /* word values */
      إذا (io_دننه)
      {
	لكل (i=0; i<vec_اندازه; i++)  
	{
		پورټ= pvw[i].پورټ;
		إذا (پورټ & 1) اذهب bad;
		pvw[i].ارزښت = inw( pvw[i].پورټ);  
	}
      }
      وإلا
      {
	لكل (i=0; i<vec_اندازه; i++) 
	{
		پورټ= pvw[i].پورټ;
		إذا (پورټ & 1) اذهب bad;
		outw( pvw[i].پورټ, pvw[i].ارزښت); 
	}
      }
      اكسر; 
  افتراضي:            					  /* long values */
      إذا (io_دننه)
      {
	لكل (i=0; i<vec_اندازه; i++)
	{
		پورټ= pvl[i].پورټ;
		إذا (پورټ & 3) اذهب bad;
		pvl[i].ارزښت = inl(pvl[i].پورټ);  
	}
      }
      وإلا
      {
	لكل (i=0; i<vec_اندازه; i++)
	{
		پورټ= pvl[i].پورټ;
		إذا (پورټ & 3) اذهب bad;
		outl( pvb[i].پورټ, pvl[i].ارزښت); 
	}
      }
  }
    
  /* Almost done, copy back results for input requests. */
  إذا (io_دننه) 
	إذا((r=معلومات_copy(KERNEL, (vir_بایټونه) vdevio_buf,
	  caller->p_endpoint, m_ptr->m_lsys_krn_sys_vdevio.vec_addr,
	  (phys_بایټونه) بایټونه)) != OK)
		أرجع r;
  أرجع(OK);

bad:
	panic("do_vdevio: unaligned port: %d", پورټ);
	أرجع EPERM;
}

#د_که_پای /* USE_VDEVIO */

