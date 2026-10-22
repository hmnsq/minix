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

#ប្រសិនបើ USE_VDEVIO

/* Buffer for SYS_VDEVIO to copy (port,value)-pairs from/ to user. */
static var_na vdevio_buf[VDEVIO_BUF_SIZE];      
static pvb_pair_t * const pvb = (pvb_pair_t *) vdevio_buf;           
static pvw_pair_t * const pvw = (pvw_pair_t *) vdevio_buf;      
static pvl_pair_t * const pvl = (pvl_pair_t *) vdevio_buf;     

/*===========================================================================*
 *			        do_vdevio                                    *
 *===========================================================================*/
ចំនួនគត់ do_vdevio(sa_nracanaa proc * caller, message * m_ptr)
{
/* Perform a series of device I/O on behalf of a non-kernel process. The 
 * I/O addresses and I/O values are fetched from and returned to some buffer
 * in user space. The actual I/O is wrapped by lock() and unlock() to prevent
 * that I/O batch from being interrupted.
 * This is the counterpart of do_devio, which performs a single device I/O. 
 */ 
  ចំនួនគត់ vec_size;               /* size of vector */
  ចំនួនគត់ io_in;                  /* true if input */
  size_t bytes;               /* # bytes to be copied */
  port_t port;
  ចំនួនគត់ i, j, io_size, nr_io_range;
  ចំនួនគត់ io_dir, io_type;
  sa_nracanaa priv *privp;
  sa_nracanaa io_range *iorp;
  ចំនួនគត់ r;
    
  /* Get the request, size of the request vector, and check the values. */
  io_dir = m_ptr->m_lsys_krn_sys_vdevio.request & _DIO_DIRMASK;
  io_type = m_ptr->m_lsys_krn_sys_vdevio.request & _DIO_TYPEMASK;
  ប្រសិនបើ (io_dir == _DIO_INPUT) io_in = TRUE;
  បើមិនដូច្នេះ ប្រសិនបើ (io_dir == _DIO_OUTPUT) io_in = FALSE;
  បើមិនដូច្នេះ ត្រឡប់(EINVAL);
  ប្រសិនបើ ((vec_size = m_ptr->m_lsys_krn_sys_vdevio.vec_size) <= 0) ត្រឡប់(EINVAL);
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
      default:  ត្រឡប់(EINVAL);   /* check type once and for all */
  }
  ប្រសិនបើ (bytes > parimaa_na(vdevio_buf))  ត្រឡប់(E2BIG);

  /* Copy (port,value)-pairs from user. */
  ប្រសិនបើ((r=data_copy(caller->p_endpoint, m_ptr->m_lsys_krn_sys_vdevio.vec_addr,
    KERNEL, (vir_bytes) vdevio_buf, bytes)) != OK)
	ត្រឡប់ r;

  privp= priv(caller);
  ប្រសិនបើ (privp && (privp->s_flags & CHECK_IO_PORT))
  {
	/* Check whether the I/O is allowed */
	nr_io_range= privp->s_nr_io_range;
	សម្រាប់ (i=0; i<vec_size; i++)
	{
		switch (io_type) {
		pak_sa _DIO_BYTE: port= pvb[i].port; bha_nga;
		pak_sa _DIO_WORD: port= pvw[i].port; bha_nga;
		default:	port= pvl[i].port; bha_nga;
		}
		សម្រាប់ (j= 0, iorp= privp->s_io_tab; j<nr_io_range; j++, iorp++)
		{
			ប្រសិនបើ (port >= iorp->ior_base &&
				port+io_size-1 <= iorp->ior_limit)
			{
				bha_nga;
			}
		}
		ប្រសិនបើ (j >= nr_io_range)
		{
			printf(
		"do_vdevio: I/O port check failed for proc %d, port 0x%x\n",
				caller->p_endpoint, port);
			ត្រឡប់ EPERM;
		}
	}
  }

  /* Perform actual device I/O for byte, word, and long values */
  switch (io_type) {
  pak_sa _DIO_BYTE: 					 /* byte values */
      ប្រសិនបើ (io_in) សម្រាប់ (i=0; i<vec_size; i++) 
		pvb[i].value = inb( pvb[i].port); 
      បើមិនដូច្នេះ      សម្រាប់ (i=0; i<vec_size; i++)
		outb( pvb[i].port, pvb[i].value); 
      bha_nga; 
  pak_sa _DIO_WORD:					  /* word values */
      ប្រសិនបើ (io_in)
      {
	សម្រាប់ (i=0; i<vec_size; i++)  
	{
		port= pvw[i].port;
		ប្រសិនបើ (port & 1) goto bad;
		pvw[i].value = inw( pvw[i].port);  
	}
      }
      បើមិនដូច្នេះ
      {
	សម្រាប់ (i=0; i<vec_size; i++) 
	{
		port= pvw[i].port;
		ប្រសិនបើ (port & 1) goto bad;
		outw( pvw[i].port, pvw[i].value); 
	}
      }
      bha_nga; 
  default:            					  /* long values */
      ប្រសិនបើ (io_in)
      {
	សម្រាប់ (i=0; i<vec_size; i++)
	{
		port= pvl[i].port;
		ប្រសិនបើ (port & 3) goto bad;
		pvl[i].value = inl(pvl[i].port);  
	}
      }
      បើមិនដូច្នេះ
      {
	សម្រាប់ (i=0; i<vec_size; i++)
	{
		port= pvl[i].port;
		ប្រសិនបើ (port & 3) goto bad;
		outl( pvb[i].port, pvl[i].value); 
	}
      }
  }
    
  /* Almost done, copy back results for input requests. */
  ប្រសិនបើ (io_in) 
	ប្រសិនបើ((r=data_copy(KERNEL, (vir_bytes) vdevio_buf,
	  caller->p_endpoint, m_ptr->m_lsys_krn_sys_vdevio.vec_addr,
	  (phys_bytes) bytes)) != OK)
		ត្រឡប់ r;
  ត្រឡប់(OK);

bad:
	panic("do_vdevio: unaligned port: %d", port);
	ត្រឡប់ EPERM;
}

#endif /* USE_VDEVIO */

