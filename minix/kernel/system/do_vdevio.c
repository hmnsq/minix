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

#якщо USE_VDEVIO

/* Buffer for SYS_VDEVIO to copy (port,value)-pairs from/ to user. */
static char vdevio_buf[VDEVIO_BUF_SIZE];      
static pvb_pair_t * const pvb = (pvb_pair_t *) vdevio_buf;           
static pvw_pair_t * const pvw = (pvw_pair_t *) vdevio_buf;      
static pvl_pair_t * const pvl = (pvl_pair_t *) vdevio_buf;     

/*===========================================================================*
 *			        do_vdevio                                    *
 *===========================================================================*/
ціле do_vdevio(struct proc * caller, message * m_ptr)
{
/* Perform a series of device I/O on behalf of a non-kernel process. The 
 * I/O addresses and I/O values are fetched from and returned to some buffer
 * in user space. The actual I/O is wrapped by lock() and unlock() to prevent
 * that I/O batch from being interrupted.
 * This is the counterpart of do_devio, which performs a single device I/O. 
 */ 
  ціле vec_size;               /* size of vector */
  ціле io_in;                  /* true if input */
  size_t bytes;               /* # bytes to be copied */
  port_t port;
  ціле i, j, io_size, nr_io_range;
  ціле io_dir, io_type;
  struct priv *privp;
  struct io_range *iorp;
  ціле r;
    
  /* Get the request, size of the request vector, and check the values. */
  io_dir = m_ptr->m_lsys_krn_sys_vdevio.request & _DIO_DIRMASK;
  io_type = m_ptr->m_lsys_krn_sys_vdevio.request & _DIO_TYPEMASK;
  якщо (io_dir == _DIO_INPUT) io_in = TRUE;
  інакше якщо (io_dir == _DIO_OUTPUT) io_in = FALSE;
  інакше повернути(EINVAL);
  якщо ((vec_size = m_ptr->m_lsys_krn_sys_vdevio.vec_size) <= 0) повернути(EINVAL);
  switch (io_type) {
      case _DIO_BYTE:
	bytes = vec_size * sizeof(pvb_pair_t);
	io_size= sizeof(u8_t);
	break;
      case _DIO_WORD:
	bytes = vec_size * sizeof(pvw_pair_t);
	io_size= sizeof(u16_t);
	break;
      case _DIO_LONG:
	bytes = vec_size * sizeof(pvl_pair_t);
	io_size= sizeof(u32_t);
	break;
      default:  повернути(EINVAL);   /* check type once and for all */
  }
  якщо (bytes > sizeof(vdevio_buf))  повернути(E2BIG);

  /* Copy (port,value)-pairs from user. */
  якщо((r=data_copy(caller->p_endpoint, m_ptr->m_lsys_krn_sys_vdevio.vec_addr,
    KERNEL, (vir_bytes) vdevio_buf, bytes)) != OK)
	повернути r;

  privp= priv(caller);
  якщо (privp && (privp->s_flags & CHECK_IO_PORT))
  {
	/* Check whether the I/O is allowed */
	nr_io_range= privp->s_nr_io_range;
	для (i=0; i<vec_size; i++)
	{
		switch (io_type) {
		case _DIO_BYTE: port= pvb[i].port; break;
		case _DIO_WORD: port= pvw[i].port; break;
		default:	port= pvl[i].port; break;
		}
		для (j= 0, iorp= privp->s_io_tab; j<nr_io_range; j++, iorp++)
		{
			якщо (port >= iorp->ior_base &&
				port+io_size-1 <= iorp->ior_limit)
			{
				break;
			}
		}
		якщо (j >= nr_io_range)
		{
			printf(
		"do_vdevio: I/O port check failed for proc %d, port 0x%x\n",
				caller->p_endpoint, port);
			повернути EPERM;
		}
	}
  }

  /* Perform actual device I/O for byte, word, and long values */
  switch (io_type) {
  case _DIO_BYTE: 					 /* byte values */
      якщо (io_in) для (i=0; i<vec_size; i++) 
		pvb[i].value = inb( pvb[i].port); 
      інакше      для (i=0; i<vec_size; i++)
		outb( pvb[i].port, pvb[i].value); 
      break; 
  case _DIO_WORD:					  /* word values */
      якщо (io_in)
      {
	для (i=0; i<vec_size; i++)  
	{
		port= pvw[i].port;
		якщо (port & 1) goto bad;
		pvw[i].value = inw( pvw[i].port);  
	}
      }
      інакше
      {
	для (i=0; i<vec_size; i++) 
	{
		port= pvw[i].port;
		якщо (port & 1) goto bad;
		outw( pvw[i].port, pvw[i].value); 
	}
      }
      break; 
  default:            					  /* long values */
      якщо (io_in)
      {
	для (i=0; i<vec_size; i++)
	{
		port= pvl[i].port;
		якщо (port & 3) goto bad;
		pvl[i].value = inl(pvl[i].port);  
	}
      }
      інакше
      {
	для (i=0; i<vec_size; i++)
	{
		port= pvl[i].port;
		якщо (port & 3) goto bad;
		outl( pvb[i].port, pvl[i].value); 
	}
      }
  }
    
  /* Almost done, copy back results for input requests. */
  якщо (io_in) 
	якщо((r=data_copy(KERNEL, (vir_bytes) vdevio_buf,
	  caller->p_endpoint, m_ptr->m_lsys_krn_sys_vdevio.vec_addr,
	  (phys_bytes) bytes)) != OK)
		повернути r;
  повернути(OK);

bad:
	panic("do_vdevio: unaligned port: %d", port);
	повернути EPERM;
}

#endif /* USE_VDEVIO */

