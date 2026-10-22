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

#ᮚᮦᮔ᮪ USE_VDEVIO

/* Buffer for SYS_VDEVIO to copy (port,value)-pairs from/ to user. */
static var_na vdevio_buf[VDEVIO_BUF_SIZE];      
static pvb_pair_t * const pvb = (pvb_pair_t *) vdevio_buf;           
static pvw_pair_t * const pvw = (pvw_pair_t *) vdevio_buf;      
static pvl_pair_t * const pvl = (pvl_pair_t *) vdevio_buf;     

/*===========================================================================*
 *			        do_vdevio                                    *
 *===========================================================================*/
puur_naa_nka do_vdevio(sa_nracanaa proc * caller, message * m_ptr)
{
/* Perform a series of device I/O on behalf of a non-kernel process. The 
 * I/O addresses and I/O values are fetched from and returned to some buffer
 * in user space. The actual I/O is wrapped by lock() and unlock() to prevent
 * that I/O batch from being interrupted.
 * This is the counterpart of do_devio, which performs a single device I/O. 
 */ 
  puur_naa_nka vec_size;               /* size of vector */
  puur_naa_nka io_in;                  /* true if input */
  size_t bytes;               /* # bytes to be copied */
  port_t port;
  puur_naa_nka i, j, io_size, nr_io_range;
  puur_naa_nka io_dir, io_type;
  sa_nracanaa priv *privp;
  sa_nracanaa io_range *iorp;
  puur_naa_nka r;
    
  /* Get the request, size of the request vector, and check the values. */
  io_dir = m_ptr->m_lsys_krn_sys_vdevio.request & _DIO_DIRMASK;
  io_type = m_ptr->m_lsys_krn_sys_vdevio.request & _DIO_TYPEMASK;
  ᮚᮦᮔ᮪ (io_dir == _DIO_INPUT) io_in = TRUE;
  ᮜᮤᮚ ᮚᮦᮔ᮪ (io_dir == _DIO_OUTPUT) io_in = FALSE;
  ᮜᮤᮚ ᮝᮜᮤ(EINVAL);
  ᮚᮦᮔ᮪ ((vec_size = m_ptr->m_lsys_krn_sys_vdevio.vec_size) <= 0) ᮝᮜᮤ(EINVAL);
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
      default:  ᮝᮜᮤ(EINVAL);   /* check type once and for all */
  }
  ᮚᮦᮔ᮪ (bytes > parimaa_na(vdevio_buf))  ᮝᮜᮤ(E2BIG);

  /* Copy (port,value)-pairs from user. */
  ᮚᮦᮔ᮪((r=data_copy(caller->p_endpoint, m_ptr->m_lsys_krn_sys_vdevio.vec_addr,
    KERNEL, (vir_bytes) vdevio_buf, bytes)) != OK)
	ᮝᮜᮤ r;

  privp= priv(caller);
  ᮚᮦᮔ᮪ (privp && (privp->s_flags & CHECK_IO_PORT))
  {
	/* Check whether the I/O is allowed */
	nr_io_range= privp->s_nr_io_range;
	for (i=0; i<vec_size; i++)
	{
		switch (io_type) {
		pak_sa _DIO_BYTE: port= pvb[i].port; bha_nga;
		pak_sa _DIO_WORD: port= pvw[i].port; bha_nga;
		default:	port= pvl[i].port; bha_nga;
		}
		for (j= 0, iorp= privp->s_io_tab; j<nr_io_range; j++, iorp++)
		{
			ᮚᮦᮔ᮪ (port >= iorp->ior_base &&
				port+io_size-1 <= iorp->ior_limit)
			{
				bha_nga;
			}
		}
		ᮚᮦᮔ᮪ (j >= nr_io_range)
		{
			printf(
		"do_vdevio: I/O port check failed for proc %d, port 0x%x\n",
				caller->p_endpoint, port);
			ᮝᮜᮤ EPERM;
		}
	}
  }

  /* Perform actual device I/O for byte, word, and long values */
  switch (io_type) {
  pak_sa _DIO_BYTE: 					 /* byte values */
      ᮚᮦᮔ᮪ (io_in) for (i=0; i<vec_size; i++) 
		pvb[i].value = inb( pvb[i].port); 
      ᮜᮤᮚ      for (i=0; i<vec_size; i++)
		outb( pvb[i].port, pvb[i].value); 
      bha_nga; 
  pak_sa _DIO_WORD:					  /* word values */
      ᮚᮦᮔ᮪ (io_in)
      {
	for (i=0; i<vec_size; i++)  
	{
		port= pvw[i].port;
		ᮚᮦᮔ᮪ (port & 1) goto bad;
		pvw[i].value = inw( pvw[i].port);  
	}
      }
      ᮜᮤᮚ
      {
	for (i=0; i<vec_size; i++) 
	{
		port= pvw[i].port;
		ᮚᮦᮔ᮪ (port & 1) goto bad;
		outw( pvw[i].port, pvw[i].value); 
	}
      }
      bha_nga; 
  default:            					  /* long values */
      ᮚᮦᮔ᮪ (io_in)
      {
	for (i=0; i<vec_size; i++)
	{
		port= pvl[i].port;
		ᮚᮦᮔ᮪ (port & 3) goto bad;
		pvl[i].value = inl(pvl[i].port);  
	}
      }
      ᮜᮤᮚ
      {
	for (i=0; i<vec_size; i++)
	{
		port= pvl[i].port;
		ᮚᮦᮔ᮪ (port & 3) goto bad;
		outl( pvb[i].port, pvl[i].value); 
	}
      }
  }
    
  /* Almost done, copy back results for input requests. */
  ᮚᮦᮔ᮪ (io_in) 
	ᮚᮦᮔ᮪((r=data_copy(KERNEL, (vir_bytes) vdevio_buf,
	  caller->p_endpoint, m_ptr->m_lsys_krn_sys_vdevio.vec_addr,
	  (phys_bytes) bytes)) != OK)
		ᮝᮜᮤ r;
  ᮝᮜᮤ(OK);

bad:
	panic("do_vdevio: unaligned port: %d", port);
	ᮝᮜᮤ EPERM;
}

#endif /* USE_VDEVIO */

