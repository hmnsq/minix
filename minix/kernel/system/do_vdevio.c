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
ساكن حرف vdevio_buf[VDEVIO_BUF_ناپ];      
ساكن pvb_pair_t * ثابت pvb = (pvb_pair_t *) vdevio_buf;           
ساكن pvw_pair_t * ثابت pvw = (pvw_pair_t *) vdevio_buf;      
ساكن pvl_pair_t * ثابت pvl = (pvl_pair_t *) vdevio_buf;     

/*===========================================================================*
 *			        do_vdevio                                    *
 *===========================================================================*/
صحيح do_vdevio(بنية proc * caller, سنیہا * m_ptr)
{
/* Perform a series of device I/O on behalf of a non-kernel process. The 
 * I/O addresses and I/O values are fetched from and returned to some buffer
 * in user space. The actual I/O is wrapped by lock() and unlock() to prevent
 * that I/O batch from being interrupted.
 * This is the counterpart of do_devio, which performs a single device I/O. 
 */ 
  صحيح vec_ناپ;               /* size of vector */
  صحيح io_وچ;                  /* true if input */
  size_t بائٹاں;               /* # bytes to be copied */
  پورٹ_t پورٹ;
  صحيح i, j, io_ناپ, nr_io_range;
  صحيح io_dir, io_type;
  بنية priv *privp;
  بنية io_range *iorp;
  صحيح r;
    
  /* Get the request, size of the request vector, and check the values. */
  io_dir = m_ptr->m_lsys_krn_sys_vdevio.request & _DIO_DIRMASK;
  io_type = m_ptr->m_lsys_krn_sys_vdevio.request & _DIO_TYPEMASK;
  إذا (io_dir == DIO_داخلہ) io_وچ = TRUE;
  وإلا إذا (io_dir == _DIO_OUTPUT) io_وچ = FALSE;
  وإلا أرجع(EINVAL);
  إذا ((vec_ناپ = m_ptr->m_lsys_krn_sys_vdevio.vec_ناپ) <= 0) أرجع(EINVAL);
  بدل (io_type) {
      حالة _DIO_BYTE:
	بائٹاں = vec_ناپ * حجم(pvb_pair_t);
	io_ناپ= حجم(u8_t);
	اكسر;
      حالة DIO_لفظ:
	بائٹاں = vec_ناپ * حجم(pvw_pair_t);
	io_ناپ= حجم(u16_t);
	اكسر;
      حالة _DIO_LONG:
	بائٹاں = vec_ناپ * حجم(pvl_pair_t);
	io_ناپ= حجم(u32_t);
	اكسر;
      افتراضي:  أرجع(EINVAL);   /* check type once and for all */
  }
  إذا (بائٹاں > حجم(vdevio_buf))  أرجع(E2BIG);

  /* Copy (port,value)-pairs from user. */
  إذا((r=انکڑے_copy(caller->p_endpoint, m_ptr->m_lsys_krn_sys_vdevio.vec_addr,
    KERNEL, (vir_بائٹاں) vdevio_buf, بائٹاں)) != OK)
	أرجع r;

  privp= priv(caller);
  إذا (privp && (privp->s_flags & CHECK_IO_پورٹ))
  {
	/* Check whether the I/O is allowed */
	nr_io_range= privp->s_nr_io_range;
	لكل (i=0; i<vec_ناپ; i++)
	{
		بدل (io_type) {
		حالة _DIO_BYTE: پورٹ= pvb[i].پورٹ; اكسر;
		حالة DIO_لفظ: پورٹ= pvw[i].پورٹ; اكسر;
		افتراضي:	پورٹ= pvl[i].پورٹ; اكسر;
		}
		لكل (j= 0, iorp= privp->s_io_tab; j<nr_io_range; j++, iorp++)
		{
			إذا (پورٹ >= iorp->ior_ادھار &&
				پورٹ+io_ناپ-1 <= iorp->ior_حد_بندی)
			{
				اكسر;
			}
		}
		إذا (j >= nr_io_range)
		{
			لکھو(
		"do_vdevio: I/O port check failed for proc %d, port 0x%x\n",
				caller->p_endpoint, پورٹ);
			أرجع EPERM;
		}
	}
  }

  /* Perform actual device I/O for byte, word, and long values */
  بدل (io_type) {
  حالة _DIO_BYTE: 					 /* byte values */
      إذا (io_وچ) لكل (i=0; i<vec_ناپ; i++) 
		pvb[i].قدر = inb( pvb[i].پورٹ); 
      وإلا      لكل (i=0; i<vec_ناپ; i++)
		outb( pvb[i].پورٹ, pvb[i].قدر); 
      اكسر; 
  حالة DIO_لفظ:					  /* word values */
      إذا (io_وچ)
      {
	لكل (i=0; i<vec_ناپ; i++)  
	{
		پورٹ= pvw[i].پورٹ;
		إذا (پورٹ & 1) اذهب bad;
		pvw[i].قدر = inw( pvw[i].پورٹ);  
	}
      }
      وإلا
      {
	لكل (i=0; i<vec_ناپ; i++) 
	{
		پورٹ= pvw[i].پورٹ;
		إذا (پورٹ & 1) اذهب bad;
		outw( pvw[i].پورٹ, pvw[i].قدر); 
	}
      }
      اكسر; 
  افتراضي:            					  /* long values */
      إذا (io_وچ)
      {
	لكل (i=0; i<vec_ناپ; i++)
	{
		پورٹ= pvl[i].پورٹ;
		إذا (پورٹ & 3) اذهب bad;
		pvl[i].قدر = inl(pvl[i].پورٹ);  
	}
      }
      وإلا
      {
	لكل (i=0; i<vec_ناپ; i++)
	{
		پورٹ= pvl[i].پورٹ;
		إذا (پورٹ & 3) اذهب bad;
		outl( pvb[i].پورٹ, pvl[i].قدر); 
	}
      }
  }
    
  /* Almost done, copy back results for input requests. */
  إذا (io_وچ) 
	إذا((r=انکڑے_copy(KERNEL, (vir_بائٹاں) vdevio_buf,
	  caller->p_endpoint, m_ptr->m_lsys_krn_sys_vdevio.vec_addr,
	  (phys_بائٹاں) بائٹاں)) != OK)
		أرجع r;
  أرجع(OK);

bad:
	panic("do_vdevio: unaligned port: %d", پورٹ);
	أرجع EPERM;
}

#جے_انت /* USE_VDEVIO */

