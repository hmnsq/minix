/* The kernel call implemented in this file:
 *   m_type:	SYS_VDEVIO
 *
 * The parameters for this kernel call are:
 *    m_lsys_krn_sys_vdevio.request	(request input or output)
 *    m_lsys_krn_sys_vdevio.vec_addr	(pointer to port/ value pairs)
 *    m_lsys_krn_sys_vdevio.vec_size	(number of ports to read or write)
 */

#include "kernel/system.h"
#include <minix/devio.h>
#include <minix/endpoint.h>

#ถ้า USE_VDEVIO

/* Buffer for SYS_VDEVIO to copy (port,value)-pairs from/ to user. */
static var_na vdevio_buf[VDEVIO_BUF_SIZE];      
static pvb_pair_t * const pvb = (pvb_pair_t *) vdevio_buf;           
static pvw_pair_t * const pvw = (pvw_pair_t *) vdevio_buf;      
static pvl_pair_t * const pvl = (pvl_pair_t *) vdevio_buf;     

/*===========================================================================*
 *			        do_vdevio                                    *
 *===========================================================================*/
จำนวนเต็ม do_vdevio(sa_nracanaa proc * caller, message * m_ptr)
{
/* Perform a series of device I/O on behalf of a non-kernel process. The 
 * I/O addresses and I/O values are fetched from and returned to some buffer
 * in user space. The actual I/O is wrapped by lock() and unlock() to prevent
 * that I/O batch from being interrupted.
 * This is the counterpart of do_devio, which performs a single device I/O. 
 */ 
  จำนวนเต็ม vec_size;               /* size of vector */
  จำนวนเต็ม io_in;                  /* true if input */
  size_t bytes;               /* # bytes to be copied */
  port_t port;
  จำนวนเต็ม i, j, io_size, nr_io_range;
  จำนวนเต็ม io_dir, io_type;
  sa_nracanaa priv *privp;
  sa_nracanaa io_range *iorp;
  จำนวนเต็ม r;
    
  /* Get the request, size of the request vector, and check the values. */
  io_dir = m_ptr->m_lsys_krn_sys_vdevio.request & _DIO_DIRMASK;
  io_type = m_ptr->m_lsys_krn_sys_vdevio.request & _DIO_TYPEMASK;
  ถ้า (io_dir == _DIO_INPUT) io_in = TRUE;
  มิฉะนั้น ถ้า (io_dir == _DIO_OUTPUT) io_in = FALSE;
  มิฉะนั้น คืน(EINVAL);
  ถ้า ((vec_size = m_ptr->m_lsys_krn_sys_vdevio.vec_size) <= 0) คืน(EINVAL);
  switch (io_type) {
      pak_sa _DIO_BYTE:
	bytes = vec_size * parimaa_na(pvb_pair_t);
	io_size= parimaa_na(u8_t);
	bha_nga;
      pak_sa _DIO_WORD:
	bytes = vec_size * parimaa_na(pvw_pair_t);
	io_size= parimaa_na(u16_t);
	bha_nga;
      pak_sa _DIO_LONG:
	bytes = vec_size * parimaa_na(pvl_pair_t);
	io_size= parimaa_na(u32_t);
	bha_nga;
      default:  คืน(EINVAL);   /* check type once and for all */
  }
  ถ้า (bytes > parimaa_na(vdevio_buf))  คืน(E2BIG);

  /* Copy (port,value)-pairs from user. */
  ถ้า((r=data_copy(caller->p_endpoint, m_ptr->m_lsys_krn_sys_vdevio.vec_addr,
    KERNEL, (vir_bytes) vdevio_buf, bytes)) != OK)
	คืน r;

  privp= priv(caller);
  ถ้า (privp && (privp->s_flags & CHECK_IO_PORT))
  {
	/* Check whether the I/O is allowed */
	nr_io_range= privp->s_nr_io_range;
	สำหรับ (i=0; i<vec_size; i++)
	{
		switch (io_type) {
		pak_sa _DIO_BYTE: port= pvb[i].port; bha_nga;
		pak_sa _DIO_WORD: port= pvw[i].port; bha_nga;
		default:	port= pvl[i].port; bha_nga;
		}
		สำหรับ (j= 0, iorp= privp->s_io_tab; j<nr_io_range; j++, iorp++)
		{
			ถ้า (port >= iorp->ior_base &&
				port+io_size-1 <= iorp->ior_limit)
			{
				bha_nga;
			}
		}
		ถ้า (j >= nr_io_range)
		{
			printf(
		"do_vdevio: I/O port check failed for proc %d, port 0x%x\n",
				caller->p_endpoint, port);
			คืน EPERM;
		}
	}
  }

  /* Perform actual device I/O for byte, word, and long values */
  switch (io_type) {
  pak_sa _DIO_BYTE: 					 /* byte values */
      ถ้า (io_in) สำหรับ (i=0; i<vec_size; i++) 
		pvb[i].value = inb( pvb[i].port); 
      มิฉะนั้น      สำหรับ (i=0; i<vec_size; i++)
		outb( pvb[i].port, pvb[i].value); 
      bha_nga; 
  pak_sa _DIO_WORD:					  /* word values */
      ถ้า (io_in)
      {
	สำหรับ (i=0; i<vec_size; i++)  
	{
		port= pvw[i].port;
		ถ้า (port & 1) goto bad;
		pvw[i].value = inw( pvw[i].port);  
	}
      }
      มิฉะนั้น
      {
	สำหรับ (i=0; i<vec_size; i++) 
	{
		port= pvw[i].port;
		ถ้า (port & 1) goto bad;
		outw( pvw[i].port, pvw[i].value); 
	}
      }
      bha_nga; 
  default:            					  /* long values */
      ถ้า (io_in)
      {
	สำหรับ (i=0; i<vec_size; i++)
	{
		port= pvl[i].port;
		ถ้า (port & 3) goto bad;
		pvl[i].value = inl(pvl[i].port);  
	}
      }
      มิฉะนั้น
      {
	สำหรับ (i=0; i<vec_size; i++)
	{
		port= pvl[i].port;
		ถ้า (port & 3) goto bad;
		outl( pvb[i].port, pvl[i].value); 
	}
      }
  }
    
  /* Almost done, copy back results for input requests. */
  ถ้า (io_in) 
	ถ้า((r=data_copy(KERNEL, (vir_bytes) vdevio_buf,
	  caller->p_endpoint, m_ptr->m_lsys_krn_sys_vdevio.vec_addr,
	  (phys_bytes) bytes)) != OK)
		คืน r;
  คืน(OK);

bad:
	panic("do_vdevio: unaligned port: %d", port);
	คืน EPERM;
}

#endif /* USE_VDEVIO */

