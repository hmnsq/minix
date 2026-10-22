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
ساكن حرف vdevio_buf[VDEVIO_BUF_سائيز];      
ساكن pvb_pair_t * ثابت pvb = (pvb_pair_t *) vdevio_buf;           
ساكن pvw_pair_t * ثابت pvw = (pvw_pair_t *) vdevio_buf;      
ساكن pvl_pair_t * ثابت pvl = (pvl_pair_t *) vdevio_buf;     

/*===========================================================================*
 *			        do_vdevio                                    *
 *===========================================================================*/
صحيح do_vdevio(بنية proc * caller, پيغام * m_ptr)
{
/* Perform a series of device I/O on behalf of a non-kernel process. The 
 * I/O addresses and I/O values are fetched from and returned to some buffer
 * in user space. The actual I/O is wrapped by lock() and unlock() to prevent
 * that I/O batch from being interrupted.
 * This is the counterpart of do_devio, which performs a single device I/O. 
 */ 
  صحيح vec_سائيز;               /* size of vector */
  صحيح io_اندريون;                  /* true if input */
  size_t بائيٽون;               /* # bytes to be copied */
  پورٽ_t پورٽ;
  صحيح i, j, io_سائيز, nr_io_range;
  صحيح io_dir, io_type;
  بنية priv *privp;
  بنية io_range *iorp;
  صحيح r;
    
  /* Get the request, size of the request vector, and check the values. */
  io_dir = m_ptr->m_lsys_krn_sys_vdevio.request & _DIO_DIRMASK;
  io_type = m_ptr->m_lsys_krn_sys_vdevio.request & _DIO_TYPEMASK;
  إذا (io_dir == DIO_داخلا) io_اندريون = TRUE;
  وإلا إذا (io_dir == _DIO_OUTPUT) io_اندريون = FALSE;
  وإلا أرجع(EINVAL);
  إذا ((vec_سائيز = m_ptr->m_lsys_krn_sys_vdevio.vec_سائيز) <= 0) أرجع(EINVAL);
  بدل (io_type) {
      حالة _DIO_BYTE:
	بائيٽون = vec_سائيز * حجم(pvb_pair_t);
	io_سائيز= حجم(u8_t);
	اكسر;
      حالة DIO_لفظ:
	بائيٽون = vec_سائيز * حجم(pvw_pair_t);
	io_سائيز= حجم(u16_t);
	اكسر;
      حالة _DIO_LONG:
	بائيٽون = vec_سائيز * حجم(pvl_pair_t);
	io_سائيز= حجم(u32_t);
	اكسر;
      افتراضي:  أرجع(EINVAL);   /* check type once and for all */
  }
  إذا (بائيٽون > حجم(vdevio_buf))  أرجع(E2BIG);

  /* Copy (port,value)-pairs from user. */
  إذا((r=معلومات_copy(caller->p_endpoint, m_ptr->m_lsys_krn_sys_vdevio.vec_addr,
    KERNEL, (vir_بائيٽون) vdevio_buf, بائيٽون)) != OK)
	أرجع r;

  privp= priv(caller);
  إذا (privp && (privp->s_flags & CHECK_IO_پورٽ))
  {
	/* Check whether the I/O is allowed */
	nr_io_range= privp->s_nr_io_range;
	لكل (i=0; i<vec_سائيز; i++)
	{
		بدل (io_type) {
		حالة _DIO_BYTE: پورٽ= pvb[i].پورٽ; اكسر;
		حالة DIO_لفظ: پورٽ= pvw[i].پورٽ; اكسر;
		افتراضي:	پورٽ= pvl[i].پورٽ; اكسر;
		}
		لكل (j= 0, iorp= privp->s_io_tab; j<nr_io_range; j++, iorp++)
		{
			إذا (پورٽ >= iorp->ior_بنياد &&
				پورٽ+io_سائيز-1 <= iorp->ior_حد_بندي)
			{
				اكسر;
			}
		}
		إذا (j >= nr_io_range)
		{
			لکو(
		"do_vdevio: I/O port check failed for proc %d, port 0x%x\n",
				caller->p_endpoint, پورٽ);
			أرجع EPERM;
		}
	}
  }

  /* Perform actual device I/O for byte, word, and long values */
  بدل (io_type) {
  حالة _DIO_BYTE: 					 /* byte values */
      إذا (io_اندريون) لكل (i=0; i<vec_سائيز; i++) 
		pvb[i].قدر = inb( pvb[i].پورٽ); 
      وإلا      لكل (i=0; i<vec_سائيز; i++)
		outb( pvb[i].پورٽ, pvb[i].قدر); 
      اكسر; 
  حالة DIO_لفظ:					  /* word values */
      إذا (io_اندريون)
      {
	لكل (i=0; i<vec_سائيز; i++)  
	{
		پورٽ= pvw[i].پورٽ;
		إذا (پورٽ & 1) اذهب bad;
		pvw[i].قدر = inw( pvw[i].پورٽ);  
	}
      }
      وإلا
      {
	لكل (i=0; i<vec_سائيز; i++) 
	{
		پورٽ= pvw[i].پورٽ;
		إذا (پورٽ & 1) اذهب bad;
		outw( pvw[i].پورٽ, pvw[i].قدر); 
	}
      }
      اكسر; 
  افتراضي:            					  /* long values */
      إذا (io_اندريون)
      {
	لكل (i=0; i<vec_سائيز; i++)
	{
		پورٽ= pvl[i].پورٽ;
		إذا (پورٽ & 3) اذهب bad;
		pvl[i].قدر = inl(pvl[i].پورٽ);  
	}
      }
      وإلا
      {
	لكل (i=0; i<vec_سائيز; i++)
	{
		پورٽ= pvl[i].پورٽ;
		إذا (پورٽ & 3) اذهب bad;
		outl( pvb[i].پورٽ, pvl[i].قدر); 
	}
      }
  }
    
  /* Almost done, copy back results for input requests. */
  إذا (io_اندريون) 
	إذا((r=معلومات_copy(KERNEL, (vir_بائيٽون) vdevio_buf,
	  caller->p_endpoint, m_ptr->m_lsys_krn_sys_vdevio.vec_addr,
	  (phys_بائيٽون) بائيٽون)) != OK)
		أرجع r;
  أرجع(OK);

bad:
	panic("do_vdevio: unaligned port: %d", پورٽ);
	أرجع EPERM;
}

#جيڪڏهن_پورو /* USE_VDEVIO */

