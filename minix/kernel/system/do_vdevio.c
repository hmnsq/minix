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
ساکن حرف vdevio_buf[VDEVIO_BUF_سائز];      
ساکن pvb_pair_t * ثابت pvb = (pvb_pair_t *) vdevio_buf;           
ساکن pvw_pair_t * ثابت pvw = (pvw_pair_t *) vdevio_buf;      
ساکن pvl_pair_t * ثابت pvl = (pvl_pair_t *) vdevio_buf;     

/*===========================================================================*
 *			        do_vdevio                                    *
 *===========================================================================*/
صحیح do_vdevio(ساخت proc * caller, پیغام * m_ptr)
{
/* Perform a series of device I/O on behalf of a non-kernel process. The 
 * I/O addresses and I/O values are fetched from and returned to some buffer
 * in user space. The actual I/O is wrapped by lock() and unlock() to prevent
 * that I/O batch from being interrupted.
 * This is the counterpart of do_devio, which performs a single device I/O. 
 */ 
  صحیح vec_سائز;               /* size of vector */
  صحیح io_اندر;                  /* true if input */
  size_t بائٹیں;               /* # bytes to be copied */
  پورٹ_t پورٹ;
  صحیح i, j, io_سائز, nr_io_range;
  صحیح io_dir, io_type;
  ساخت priv *privp;
  ساخت io_range *iorp;
  صحیح r;
    
  /* Get the request, size of the request vector, and check the values. */
  io_dir = m_ptr->m_lsys_krn_sys_vdevio.request & _DIO_DIRMASK;
  io_type = m_ptr->m_lsys_krn_sys_vdevio.request & _DIO_TYPEMASK;
  اگر (io_dir == DIO_داخلہ) io_اندر = TRUE;
  ورنہ اگر (io_dir == _DIO_OUTPUT) io_اندر = FALSE;
  ورنہ واپس(EINVAL);
  اگر ((vec_سائز = m_ptr->m_lsys_krn_sys_vdevio.vec_سائز) <= 0) واپس(EINVAL);
  انتخاب (io_type) {
      حالت _DIO_BYTE:
	بائٹیں = vec_سائز * حجم(pvb_pair_t);
	io_سائز= حجم(u8_t);
	توڑو;
      حالت DIO_لفظ:
	بائٹیں = vec_سائز * حجم(pvw_pair_t);
	io_سائز= حجم(u16_t);
	توڑو;
      حالت _DIO_LONG:
	بائٹیں = vec_سائز * حجم(pvl_pair_t);
	io_سائز= حجم(u32_t);
	توڑو;
      طےشدہ:  واپس(EINVAL);   /* check type once and for all */
  }
  اگر (بائٹیں > حجم(vdevio_buf))  واپس(E2BIG);

  /* Copy (port,value)-pairs from user. */
  اگر((r=ڈیٹا_copy(caller->p_endpoint, m_ptr->m_lsys_krn_sys_vdevio.vec_addr,
    KERNEL, (vir_بائٹیں) vdevio_buf, بائٹیں)) != OK)
	واپس r;

  privp= priv(caller);
  اگر (privp && (privp->s_flags & CHECK_IO_پورٹ))
  {
	/* Check whether the I/O is allowed */
	nr_io_range= privp->s_nr_io_range;
	برائے (i=0; i<vec_سائز; i++)
	{
		انتخاب (io_type) {
		حالت _DIO_BYTE: پورٹ= pvb[i].پورٹ; توڑو;
		حالت DIO_لفظ: پورٹ= pvw[i].پورٹ; توڑو;
		طےشدہ:	پورٹ= pvl[i].پورٹ; توڑو;
		}
		برائے (j= 0, iorp= privp->s_io_tab; j<nr_io_range; j++, iorp++)
		{
			اگر (پورٹ >= iorp->ior_بنیاد &&
				پورٹ+io_سائز-1 <= iorp->ior_حد_بندی)
			{
				توڑو;
			}
		}
		اگر (j >= nr_io_range)
		{
			لکھو(
		"do_vdevio: I/O port check failed for proc %d, port 0x%x\n",
				caller->p_endpoint, پورٹ);
			واپس EPERM;
		}
	}
  }

  /* Perform actual device I/O for byte, word, and long values */
  انتخاب (io_type) {
  حالت _DIO_BYTE: 					 /* byte values */
      اگر (io_اندر) برائے (i=0; i<vec_سائز; i++) 
		pvb[i].قدر = inb( pvb[i].پورٹ); 
      ورنہ      برائے (i=0; i<vec_سائز; i++)
		outb( pvb[i].پورٹ, pvb[i].قدر); 
      توڑو; 
  حالت DIO_لفظ:					  /* word values */
      اگر (io_اندر)
      {
	برائے (i=0; i<vec_سائز; i++)  
	{
		پورٹ= pvw[i].پورٹ;
		اگر (پورٹ & 1) جاؤ bad;
		pvw[i].قدر = inw( pvw[i].پورٹ);  
	}
      }
      ورنہ
      {
	برائے (i=0; i<vec_سائز; i++) 
	{
		پورٹ= pvw[i].پورٹ;
		اگر (پورٹ & 1) جاؤ bad;
		outw( pvw[i].پورٹ, pvw[i].قدر); 
	}
      }
      توڑو; 
  طےشدہ:            					  /* long values */
      اگر (io_اندر)
      {
	برائے (i=0; i<vec_سائز; i++)
	{
		پورٹ= pvl[i].پورٹ;
		اگر (پورٹ & 3) جاؤ bad;
		pvl[i].قدر = inl(pvl[i].پورٹ);  
	}
      }
      ورنہ
      {
	برائے (i=0; i<vec_سائز; i++)
	{
		پورٹ= pvl[i].پورٹ;
		اگر (پورٹ & 3) جاؤ bad;
		outl( pvb[i].پورٹ, pvl[i].قدر); 
	}
      }
  }
    
  /* Almost done, copy back results for input requests. */
  اگر (io_اندر) 
	اگر((r=ڈیٹا_copy(KERNEL, (vir_بائٹیں) vdevio_buf,
	  caller->p_endpoint, m_ptr->m_lsys_krn_sys_vdevio.vec_addr,
	  (phys_بائٹیں) بائٹیں)) != OK)
		واپس r;
  واپس(OK);

bad:
	panic("do_vdevio: unaligned port: %d", پورٹ);
	واپس EPERM;
}

#اگر_ختم /* USE_VDEVIO */

