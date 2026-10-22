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

#만약 USE_VDEVIO

/* Buffer for SYS_VDEVIO to copy (port,value)-pairs from/ to user. */
static char vdevio_buf[VDEVIO_BUF_SIZE];      
static pvb_pair_t * const pvb = (pvb_pair_t *) vdevio_buf;           
static pvw_pair_t * const pvw = (pvw_pair_t *) vdevio_buf;      
static pvl_pair_t * const pvl = (pvl_pair_t *) vdevio_buf;     

/*===========================================================================*
 *			        do_vdevio                                    *
 *===========================================================================*/
정수 do_vdevio(struct proc * caller, message * m_ptr)
{
/* Perform a series of device I/O on behalf of a non-kernel process. The 
 * I/O addresses and I/O values are fetched from and returned to some buffer
 * in user space. The actual I/O is wrapped by lock() and unlock() to prevent
 * that I/O batch from being interrupted.
 * This is the counterpart of do_devio, which performs a single device I/O. 
 */ 
  정수 vec_size;               /* size of vector */
  정수 io_in;                  /* true if input */
  size_t bytes;               /* # bytes to be copied */
  port_t port;
  정수 i, j, io_size, nr_io_range;
  정수 io_dir, io_type;
  struct priv *privp;
  struct io_range *iorp;
  정수 r;
    
  /* Get the request, size of the request vector, and check the values. */
  io_dir = m_ptr->m_lsys_krn_sys_vdevio.request & _DIO_DIRMASK;
  io_type = m_ptr->m_lsys_krn_sys_vdevio.request & _DIO_TYPEMASK;
  만약 (io_dir == _DIO_INPUT) io_in = TRUE;
  아니면 만약 (io_dir == _DIO_OUTPUT) io_in = FALSE;
  아니면 반환(EINVAL);
  만약 ((vec_size = m_ptr->m_lsys_krn_sys_vdevio.vec_size) <= 0) 반환(EINVAL);
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
      default:  반환(EINVAL);   /* check type once and for all */
  }
  만약 (bytes > sizeof(vdevio_buf))  반환(E2BIG);

  /* Copy (port,value)-pairs from user. */
  만약((r=data_copy(caller->p_endpoint, m_ptr->m_lsys_krn_sys_vdevio.vec_addr,
    KERNEL, (vir_bytes) vdevio_buf, bytes)) != OK)
	반환 r;

  privp= priv(caller);
  만약 (privp && (privp->s_flags & CHECK_IO_PORT))
  {
	/* Check whether the I/O is allowed */
	nr_io_range= privp->s_nr_io_range;
	위해 (i=0; i<vec_size; i++)
	{
		switch (io_type) {
		case _DIO_BYTE: port= pvb[i].port; break;
		case _DIO_WORD: port= pvw[i].port; break;
		default:	port= pvl[i].port; break;
		}
		위해 (j= 0, iorp= privp->s_io_tab; j<nr_io_range; j++, iorp++)
		{
			만약 (port >= iorp->ior_base &&
				port+io_size-1 <= iorp->ior_limit)
			{
				break;
			}
		}
		만약 (j >= nr_io_range)
		{
			printf(
		"do_vdevio: I/O port check failed for proc %d, port 0x%x\n",
				caller->p_endpoint, port);
			반환 EPERM;
		}
	}
  }

  /* Perform actual device I/O for byte, word, and long values */
  switch (io_type) {
  case _DIO_BYTE: 					 /* byte values */
      만약 (io_in) 위해 (i=0; i<vec_size; i++) 
		pvb[i].value = inb( pvb[i].port); 
      아니면      위해 (i=0; i<vec_size; i++)
		outb( pvb[i].port, pvb[i].value); 
      break; 
  case _DIO_WORD:					  /* word values */
      만약 (io_in)
      {
	위해 (i=0; i<vec_size; i++)  
	{
		port= pvw[i].port;
		만약 (port & 1) goto bad;
		pvw[i].value = inw( pvw[i].port);  
	}
      }
      아니면
      {
	위해 (i=0; i<vec_size; i++) 
	{
		port= pvw[i].port;
		만약 (port & 1) goto bad;
		outw( pvw[i].port, pvw[i].value); 
	}
      }
      break; 
  default:            					  /* long values */
      만약 (io_in)
      {
	위해 (i=0; i<vec_size; i++)
	{
		port= pvl[i].port;
		만약 (port & 3) goto bad;
		pvl[i].value = inl(pvl[i].port);  
	}
      }
      아니면
      {
	위해 (i=0; i<vec_size; i++)
	{
		port= pvl[i].port;
		만약 (port & 3) goto bad;
		outl( pvb[i].port, pvl[i].value); 
	}
      }
  }
    
  /* Almost done, copy back results for input requests. */
  만약 (io_in) 
	만약((r=data_copy(KERNEL, (vir_bytes) vdevio_buf,
	  caller->p_endpoint, m_ptr->m_lsys_krn_sys_vdevio.vec_addr,
	  (phys_bytes) bytes)) != OK)
		반환 r;
  반환(OK);

bad:
	panic("do_vdevio: unaligned port: %d", port);
	반환 EPERM;
}

#endif /* USE_VDEVIO */

