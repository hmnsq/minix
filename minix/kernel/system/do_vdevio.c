/* The kernel call implemented in this file:
 *   m_type:	SYS_VDEVIO
 *
 * The parameters for this kernel call are:
 *    m_lsys_krn_sys_vdevio.request	(request input or output)
 *    m_lsys_krn_sys_vdevio.vec_addr	(pointer to port/ value pairs)
 *    m_lsys_krn_sys_vdevio.vec_size	(number of ports to read or write)
 */

#અન્તર્ભાવ "kernel/system.h"
#અન્તર્ભાવ <minix/devio.h>
#અન્તર્ભાવ <minix/endpoint.h>

#જો USE_VDEVIO

/* Buffer for SYS_VDEVIO to copy (port,value)-pairs from/ to user. */
સ્થિર અક્ષર vdevio_buf[VDEVIO_BUF_SIZE];      
સ્થિર pvb_pair_t * અચલ pvb = (pvb_pair_t *) vdevio_buf;           
સ્થિર pvw_pair_t * અચલ pvw = (pvw_pair_t *) vdevio_buf;      
સ્થિર pvl_pair_t * અચલ pvl = (pvl_pair_t *) vdevio_buf;     

/*===========================================================================*
 *			        do_vdevio                                    *
 *===========================================================================*/
પૂર્ણાંક do_vdevio(સંરચના proc * caller, message * m_ptr)
{
/* Perform a series of device I/O on behalf of a non-kernel process. The 
 * I/O addresses and I/O values are fetched from and returned to some buffer
 * in user space. The actual I/O is wrapped by lock() and unlock() to prevent
 * that I/O batch from being interrupted.
 * This is the counterpart of do_devio, which performs a single device I/O. 
 */ 
  પૂર્ણાંક vec_size;               /* size of vector */
  પૂર્ણાંક io_in;                  /* true if input */
  size_t bytes;               /* # bytes to be copied */
  port_t port;
  પૂર્ણાંક i, j, io_size, nr_io_range;
  પૂર્ણાંક io_dir, io_type;
  સંરચના priv *privp;
  સંરચના io_range *iorp;
  પૂર્ણાંક r;
    
  /* Get the request, size of the request vector, and check the values. */
  io_dir = m_ptr->m_lsys_krn_sys_vdevio.request & _DIO_DIRMASK;
  io_type = m_ptr->m_lsys_krn_sys_vdevio.request & _DIO_TYPEMASK;
  જો (io_dir == DIO_આગત) io_in = TRUE;
  નહીંતર જો (io_dir == _DIO_OUTPUT) io_in = FALSE;
  નહીંતર પાછા(EINVAL);
  જો ((vec_size = m_ptr->m_lsys_krn_sys_vdevio.vec_size) <= 0) પાછા(EINVAL);
  પસંદગી (io_type) {
      સ્થિતિ _DIO_BYTE:
	bytes = vec_size * કદ(pvb_pair_t);
	io_size= કદ(u8_t);
	તોડો;
      સ્થિતિ _DIO_WORD:
	bytes = vec_size * કદ(pvw_pair_t);
	io_size= કદ(u16_t);
	તોડો;
      સ્થિતિ _DIO_LONG:
	bytes = vec_size * કદ(pvl_pair_t);
	io_size= કદ(u32_t);
	તોડો;
      મૂળભૂત:  પાછા(EINVAL);   /* check type once and for all */
  }
  જો (bytes > કદ(vdevio_buf))  પાછા(E2BIG);

  /* Copy (port,value)-pairs from user. */
  જો((r=data_copy(caller->p_endpoint, m_ptr->m_lsys_krn_sys_vdevio.vec_addr,
    KERNEL, (vir_bytes) vdevio_buf, bytes)) != OK)
	પાછા r;

  privp= priv(caller);
  જો (privp && (privp->s_flags & CHECK_IO_PORT))
  {
	/* Check whether the I/O is allowed */
	nr_io_range= privp->s_nr_io_range;
	માટે (i=0; i<vec_size; i++)
	{
		પસંદગી (io_type) {
		સ્થિતિ _DIO_BYTE: port= pvb[i].port; તોડો;
		સ્થિતિ _DIO_WORD: port= pvw[i].port; તોડો;
		મૂળભૂત:	port= pvl[i].port; તોડો;
		}
		માટે (j= 0, iorp= privp->s_io_tab; j<nr_io_range; j++, iorp++)
		{
			જો (port >= iorp->ior_base &&
				port+io_size-1 <= iorp->ior_limit)
			{
				તોડો;
			}
		}
		જો (j >= nr_io_range)
		{
			printf(
		"do_vdevio: I/O port check failed for proc %d, port 0x%x\n",
				caller->p_endpoint, port);
			પાછા EPERM;
		}
	}
  }

  /* Perform actual device I/O for byte, word, and long values */
  પસંદગી (io_type) {
  સ્થિતિ _DIO_BYTE: 					 /* byte values */
      જો (io_in) માટે (i=0; i<vec_size; i++) 
		pvb[i].કિંમત = inb( pvb[i].port); 
      નહીંતર      માટે (i=0; i<vec_size; i++)
		outb( pvb[i].port, pvb[i].કિંમત); 
      તોડો; 
  સ્થિતિ _DIO_WORD:					  /* word values */
      જો (io_in)
      {
	માટે (i=0; i<vec_size; i++)  
	{
		port= pvw[i].port;
		જો (port & 1) જાઓ bad;
		pvw[i].કિંમત = inw( pvw[i].port);  
	}
      }
      નહીંતર
      {
	માટે (i=0; i<vec_size; i++) 
	{
		port= pvw[i].port;
		જો (port & 1) જાઓ bad;
		outw( pvw[i].port, pvw[i].કિંમત); 
	}
      }
      તોડો; 
  મૂળભૂત:            					  /* long values */
      જો (io_in)
      {
	માટે (i=0; i<vec_size; i++)
	{
		port= pvl[i].port;
		જો (port & 3) જાઓ bad;
		pvl[i].કિંમત = inl(pvl[i].port);  
	}
      }
      નહીંતર
      {
	માટે (i=0; i<vec_size; i++)
	{
		port= pvl[i].port;
		જો (port & 3) જાઓ bad;
		outl( pvb[i].port, pvl[i].કિંમત); 
	}
      }
  }
    
  /* Almost done, copy back results for input requests. */
  જો (io_in) 
	જો((r=data_copy(KERNEL, (vir_bytes) vdevio_buf,
	  caller->p_endpoint, m_ptr->m_lsys_krn_sys_vdevio.vec_addr,
	  (phys_bytes) bytes)) != OK)
		પાછા r;
  પાછા(OK);

bad:
	panic("do_vdevio: unaligned port: %d", port);
	પાછા EPERM;
}

#endif /* USE_VDEVIO */

