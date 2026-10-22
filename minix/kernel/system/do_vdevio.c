/* The kernel call implemented in this file:
 *   m_type:	SYS_VDEVIO
 *
 * The parameters for this kernel call are:
 *    m_lsys_krn_sys_vdevio.request	(request input or output)
 *    m_lsys_krn_sys_vdevio.vec_addr	(pointer to port/ value pairs)
 *    m_lsys_krn_sys_vdevio.vec_size	(number of ports to read or write)
 */

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/system.h"
#ਅਨ੍ਤਰ੍ਭਾਵ <minix/devio.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <minix/endpoint.h>

#ਜੇ USE_VDEVIO

/* Buffer for SYS_VDEVIO to copy (port,value)-pairs from/ to user. */
ਸਥਿਰ ਅੱਖਰ vdevio_buf[VDEVIO_BUF_ਮਾਪ];      
ਸਥਿਰ pvb_pair_t * ਅਚਲ pvb = (pvb_pair_t *) vdevio_buf;           
ਸਥਿਰ pvw_pair_t * ਅਚਲ pvw = (pvw_pair_t *) vdevio_buf;      
ਸਥਿਰ pvl_pair_t * ਅਚਲ pvl = (pvl_pair_t *) vdevio_buf;     

/*===========================================================================*
 *			        do_vdevio                                    *
 *===========================================================================*/
ਪੂਰਨ do_vdevio(ਬਣਤਰ proc * caller, ਸੁਨੇਹਾ * m_ptr)
{
/* Perform a series of device I/O on behalf of a non-kernel process. The 
 * I/O addresses and I/O values are fetched from and returned to some buffer
 * in user space. The actual I/O is wrapped by lock() and unlock() to prevent
 * that I/O batch from being interrupted.
 * This is the counterpart of do_devio, which performs a single device I/O. 
 */ 
  ਪੂਰਨ vec_ਮਾਪ;               /* size of vector */
  ਪੂਰਨ io_ਵਿੱਚ;                  /* true if input */
  size_t ਬਾਈਟਾਂ;               /* # bytes to be copied */
  ਪੋਰਟ_t ਪੋਰਟ;
  ਪੂਰਨ i, j, io_ਮਾਪ, nr_io_range;
  ਪੂਰਨ io_dir, io_type;
  ਬਣਤਰ priv *privp;
  ਬਣਤਰ io_range *iorp;
  ਪੂਰਨ r;
    
  /* Get the request, size of the request vector, and check the values. */
  io_dir = m_ptr->m_lsys_krn_sys_vdevio.request & _DIO_DIRMASK;
  io_type = m_ptr->m_lsys_krn_sys_vdevio.request & _DIO_TYPEMASK;
  ਜੇ (io_dir == DIO_ਨਿਵੇਸ਼) io_ਵਿੱਚ = TRUE;
  ਨਹੀਂਤਾਂ ਜੇ (io_dir == _DIO_OUTPUT) io_ਵਿੱਚ = FALSE;
  ਨਹੀਂਤਾਂ ਵਾਪਸ(EINVAL);
  ਜੇ ((vec_ਮਾਪ = m_ptr->m_lsys_krn_sys_vdevio.vec_ਮਾਪ) <= 0) ਵਾਪਸ(EINVAL);
  ਚੋਣ (io_type) {
      ਹਾਲਤ _DIO_BYTE:
	ਬਾਈਟਾਂ = vec_ਮਾਪ * ਆਕਾਰ(pvb_pair_t);
	io_ਮਾਪ= ਆਕਾਰ(u8_t);
	ਰੋਕੋ;
      ਹਾਲਤ DIO_ਸ਼ਬਦ:
	ਬਾਈਟਾਂ = vec_ਮਾਪ * ਆਕਾਰ(pvw_pair_t);
	io_ਮਾਪ= ਆਕਾਰ(u16_t);
	ਰੋਕੋ;
      ਹਾਲਤ _DIO_LONG:
	ਬਾਈਟਾਂ = vec_ਮਾਪ * ਆਕਾਰ(pvl_pair_t);
	io_ਮਾਪ= ਆਕਾਰ(u32_t);
	ਰੋਕੋ;
      ਮੂਲ:  ਵਾਪਸ(EINVAL);   /* check type once and for all */
  }
  ਜੇ (ਬਾਈਟਾਂ > ਆਕਾਰ(vdevio_buf))  ਵਾਪਸ(E2BIG);

  /* Copy (port,value)-pairs from user. */
  ਜੇ((r=ਅੰਕੜੇ_copy(caller->p_endpoint, m_ptr->m_lsys_krn_sys_vdevio.vec_addr,
    KERNEL, (vir_ਬਾਈਟਾਂ) vdevio_buf, ਬਾਈਟਾਂ)) != OK)
	ਵਾਪਸ r;

  privp= priv(caller);
  ਜੇ (privp && (privp->s_flags & CHECK_IO_ਪੋਰਟ))
  {
	/* Check whether the I/O is allowed */
	nr_io_range= privp->s_nr_io_range;
	ਲਈ (i=0; i<vec_ਮਾਪ; i++)
	{
		ਚੋਣ (io_type) {
		ਹਾਲਤ _DIO_BYTE: ਪੋਰਟ= pvb[i].ਪੋਰਟ; ਰੋਕੋ;
		ਹਾਲਤ DIO_ਸ਼ਬਦ: ਪੋਰਟ= pvw[i].ਪੋਰਟ; ਰੋਕੋ;
		ਮੂਲ:	ਪੋਰਟ= pvl[i].ਪੋਰਟ; ਰੋਕੋ;
		}
		ਲਈ (j= 0, iorp= privp->s_io_tab; j<nr_io_range; j++, iorp++)
		{
			ਜੇ (ਪੋਰਟ >= iorp->ior_ਅਧਾਰ &&
				ਪੋਰਟ+io_ਮਾਪ-1 <= iorp->ior_ਹੱਦਬੰਦੀ)
			{
				ਰੋਕੋ;
			}
		}
		ਜੇ (j >= nr_io_range)
		{
			ਲਿਖੋ(
		"do_vdevio: I/O port check failed for proc %d, port 0x%x\n",
				caller->p_endpoint, ਪੋਰਟ);
			ਵਾਪਸ EPERM;
		}
	}
  }

  /* Perform actual device I/O for byte, word, and long values */
  ਚੋਣ (io_type) {
  ਹਾਲਤ _DIO_BYTE: 					 /* byte values */
      ਜੇ (io_ਵਿੱਚ) ਲਈ (i=0; i<vec_ਮਾਪ; i++) 
		pvb[i].ਮੁੱਲ = inb( pvb[i].ਪੋਰਟ); 
      ਨਹੀਂਤਾਂ      ਲਈ (i=0; i<vec_ਮਾਪ; i++)
		outb( pvb[i].ਪੋਰਟ, pvb[i].ਮੁੱਲ); 
      ਰੋਕੋ; 
  ਹਾਲਤ DIO_ਸ਼ਬਦ:					  /* word values */
      ਜੇ (io_ਵਿੱਚ)
      {
	ਲਈ (i=0; i<vec_ਮਾਪ; i++)  
	{
		ਪੋਰਟ= pvw[i].ਪੋਰਟ;
		ਜੇ (ਪੋਰਟ & 1) ਜਾਓ bad;
		pvw[i].ਮੁੱਲ = inw( pvw[i].ਪੋਰਟ);  
	}
      }
      ਨਹੀਂਤਾਂ
      {
	ਲਈ (i=0; i<vec_ਮਾਪ; i++) 
	{
		ਪੋਰਟ= pvw[i].ਪੋਰਟ;
		ਜੇ (ਪੋਰਟ & 1) ਜਾਓ bad;
		outw( pvw[i].ਪੋਰਟ, pvw[i].ਮੁੱਲ); 
	}
      }
      ਰੋਕੋ; 
  ਮੂਲ:            					  /* long values */
      ਜੇ (io_ਵਿੱਚ)
      {
	ਲਈ (i=0; i<vec_ਮਾਪ; i++)
	{
		ਪੋਰਟ= pvl[i].ਪੋਰਟ;
		ਜੇ (ਪੋਰਟ & 3) ਜਾਓ bad;
		pvl[i].ਮੁੱਲ = inl(pvl[i].ਪੋਰਟ);  
	}
      }
      ਨਹੀਂਤਾਂ
      {
	ਲਈ (i=0; i<vec_ਮਾਪ; i++)
	{
		ਪੋਰਟ= pvl[i].ਪੋਰਟ;
		ਜੇ (ਪੋਰਟ & 3) ਜਾਓ bad;
		outl( pvb[i].ਪੋਰਟ, pvl[i].ਮੁੱਲ); 
	}
      }
  }
    
  /* Almost done, copy back results for input requests. */
  ਜੇ (io_ਵਿੱਚ) 
	ਜੇ((r=ਅੰਕੜੇ_copy(KERNEL, (vir_ਬਾਈਟਾਂ) vdevio_buf,
	  caller->p_endpoint, m_ptr->m_lsys_krn_sys_vdevio.vec_addr,
	  (phys_ਬਾਈਟਾਂ) ਬਾਈਟਾਂ)) != OK)
		ਵਾਪਸ r;
  ਵਾਪਸ(OK);

bad:
	panic("do_vdevio: unaligned port: %d", ਪੋਰਟ);
	ਵਾਪਸ EPERM;
}

#ਜੇ_ਅੰਤ /* USE_VDEVIO */

