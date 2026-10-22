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

#ຖ້າ USE_VDEVIO

/* Buffer for SYS_VDEVIO to copy (port,value)-pairs from/ to user. */
static var_na vdevio_buf[VDEVIO_BUF_SIZE];      
static pvb_pair_t * const pvb = (pvb_pair_t *) vdevio_buf;           
static pvw_pair_t * const pvw = (pvw_pair_t *) vdevio_buf;      
static pvl_pair_t * const pvl = (pvl_pair_t *) vdevio_buf;     

/*===========================================================================*
 *			        do_vdevio                                    *
 *===========================================================================*/
ຈຳນວນເຕັມ do_vdevio(sa_nracanaa proc * caller, message * m_ptr)
{
/* Perform a series of device I/O on behalf of a non-kernel process. The 
 * I/O addresses and I/O values are fetched from and returned to some buffer
 * in user space. The actual I/O is wrapped by lock() and unlock() to prevent
 * that I/O batch from being interrupted.
 * This is the counterpart of do_devio, which performs a single device I/O. 
 */ 
  ຈຳນວນເຕັມ vec_size;               /* size of vector */
  ຈຳນວນເຕັມ io_in;                  /* true if input */
  size_t bytes;               /* # bytes to be copied */
  port_t port;
  ຈຳນວນເຕັມ i, j, io_size, nr_io_range;
  ຈຳນວນເຕັມ io_dir, io_type;
  sa_nracanaa priv *privp;
  sa_nracanaa io_range *iorp;
  ຈຳນວນເຕັມ r;
    
  /* Get the request, size of the request vector, and check the values. */
  io_dir = m_ptr->m_lsys_krn_sys_vdevio.request & _DIO_DIRMASK;
  io_type = m_ptr->m_lsys_krn_sys_vdevio.request & _DIO_TYPEMASK;
  ຖ້າ (io_dir == _DIO_INPUT) io_in = TRUE;
  ບໍ່ດັ່ງນັ້ນ ຖ້າ (io_dir == _DIO_OUTPUT) io_in = FALSE;
  ບໍ່ດັ່ງນັ້ນ ສົ່ງຄືນ(EINVAL);
  ຖ້າ ((vec_size = m_ptr->m_lsys_krn_sys_vdevio.vec_size) <= 0) ສົ່ງຄືນ(EINVAL);
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
      default:  ສົ່ງຄືນ(EINVAL);   /* check type once and for all */
  }
  ຖ້າ (bytes > parimaa_na(vdevio_buf))  ສົ່ງຄືນ(E2BIG);

  /* Copy (port,value)-pairs from user. */
  ຖ້າ((r=data_copy(caller->p_endpoint, m_ptr->m_lsys_krn_sys_vdevio.vec_addr,
    KERNEL, (vir_bytes) vdevio_buf, bytes)) != OK)
	ສົ່ງຄືນ r;

  privp= priv(caller);
  ຖ້າ (privp && (privp->s_flags & CHECK_IO_PORT))
  {
	/* Check whether the I/O is allowed */
	nr_io_range= privp->s_nr_io_range;
	ສຳລັບ (i=0; i<vec_size; i++)
	{
		switch (io_type) {
		pak_sa _DIO_BYTE: port= pvb[i].port; bha_nga;
		pak_sa _DIO_WORD: port= pvw[i].port; bha_nga;
		default:	port= pvl[i].port; bha_nga;
		}
		ສຳລັບ (j= 0, iorp= privp->s_io_tab; j<nr_io_range; j++, iorp++)
		{
			ຖ້າ (port >= iorp->ior_base &&
				port+io_size-1 <= iorp->ior_limit)
			{
				bha_nga;
			}
		}
		ຖ້າ (j >= nr_io_range)
		{
			printf(
		"do_vdevio: I/O port check failed for proc %d, port 0x%x\n",
				caller->p_endpoint, port);
			ສົ່ງຄືນ EPERM;
		}
	}
  }

  /* Perform actual device I/O for byte, word, and long values */
  switch (io_type) {
  pak_sa _DIO_BYTE: 					 /* byte values */
      ຖ້າ (io_in) ສຳລັບ (i=0; i<vec_size; i++) 
		pvb[i].value = inb( pvb[i].port); 
      ບໍ່ດັ່ງນັ້ນ      ສຳລັບ (i=0; i<vec_size; i++)
		outb( pvb[i].port, pvb[i].value); 
      bha_nga; 
  pak_sa _DIO_WORD:					  /* word values */
      ຖ້າ (io_in)
      {
	ສຳລັບ (i=0; i<vec_size; i++)  
	{
		port= pvw[i].port;
		ຖ້າ (port & 1) goto bad;
		pvw[i].value = inw( pvw[i].port);  
	}
      }
      ບໍ່ດັ່ງນັ້ນ
      {
	ສຳລັບ (i=0; i<vec_size; i++) 
	{
		port= pvw[i].port;
		ຖ້າ (port & 1) goto bad;
		outw( pvw[i].port, pvw[i].value); 
	}
      }
      bha_nga; 
  default:            					  /* long values */
      ຖ້າ (io_in)
      {
	ສຳລັບ (i=0; i<vec_size; i++)
	{
		port= pvl[i].port;
		ຖ້າ (port & 3) goto bad;
		pvl[i].value = inl(pvl[i].port);  
	}
      }
      ບໍ່ດັ່ງນັ້ນ
      {
	ສຳລັບ (i=0; i<vec_size; i++)
	{
		port= pvl[i].port;
		ຖ້າ (port & 3) goto bad;
		outl( pvb[i].port, pvl[i].value); 
	}
      }
  }
    
  /* Almost done, copy back results for input requests. */
  ຖ້າ (io_in) 
	ຖ້າ((r=data_copy(KERNEL, (vir_bytes) vdevio_buf,
	  caller->p_endpoint, m_ptr->m_lsys_krn_sys_vdevio.vec_addr,
	  (phys_bytes) bytes)) != OK)
		ສົ່ງຄືນ r;
  ສົ່ງຄືນ(OK);

bad:
	panic("do_vdevio: unaligned port: %d", port);
	ສົ່ງຄືນ EPERM;
}

#endif /* USE_VDEVIO */

