/* The kernel call implemented in this file:
 *   m_type:	SYS_VDEVIO
 *
 * The parameters for this kernel call are:
 *    m_lsys_krn_sys_vdevio.request	(request input or output)
 *    m_lsys_krn_sys_vdevio.vec_addr	(pointer to port/ value pairs)
 *    m_lsys_krn_sys_vdevio.vec_size	(number of ports to read or write)
 */

#ଅନ୍ତର୍ଭାଵ "kernel/system.h"
#ଅନ୍ତର୍ଭାଵ <minix/devio.h>
#ଅନ୍ତର୍ଭାଵ <minix/endpoint.h>

#ଯଦି USE_VDEVIO

/* Buffer for SYS_VDEVIO to copy (port,value)-pairs from/ to user. */
ସ୍ଥିର ଅକ୍ଷର vdevio_buf[VDEVIO_BUF_SIZE];      
ସ୍ଥିର pvb_pair_t * ଅଚଳ pvb = (pvb_pair_t *) vdevio_buf;           
ସ୍ଥିର pvw_pair_t * ଅଚଳ pvw = (pvw_pair_t *) vdevio_buf;      
ସ୍ଥିର pvl_pair_t * ଅଚଳ pvl = (pvl_pair_t *) vdevio_buf;     

/*===========================================================================*
 *			        do_vdevio                                    *
 *===========================================================================*/
ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_vdevio(ସଂରଚନା proc * caller, message * m_ptr)
{
/* Perform a series of device I/O on behalf of a non-kernel process. The 
 * I/O addresses and I/O values are fetched from and returned to some buffer
 * in user space. The actual I/O is wrapped by lock() and unlock() to prevent
 * that I/O batch from being interrupted.
 * This is the counterpart of do_devio, which performs a single device I/O. 
 */ 
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା vec_size;               /* size of vector */
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା io_in;                  /* true if input */
  size_t bytes;               /* # bytes to be copied */
  port_t port;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା i, j, io_size, nr_io_range;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା io_dir, io_type;
  ସଂରଚନା priv *privp;
  ସଂରଚନା io_range *iorp;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;
    
  /* Get the request, size of the request vector, and check the values. */
  io_dir = m_ptr->m_lsys_krn_sys_vdevio.request & _DIO_DIRMASK;
  io_type = m_ptr->m_lsys_krn_sys_vdevio.request & _DIO_TYPEMASK;
  ଯଦି (io_dir == DIO_ନିବେଶ) io_in = TRUE;
  ଅନ୍ୟଥା ଯଦି (io_dir == _DIO_OUTPUT) io_in = FALSE;
  ଅନ୍ୟଥା ଫେରନ୍ତୁ(EINVAL);
  ଯଦି ((vec_size = m_ptr->m_lsys_krn_sys_vdevio.vec_size) <= 0) ଫେରନ୍ତୁ(EINVAL);
  ବାଛ (io_type) {
      ସ୍ଥିତି _DIO_BYTE:
	bytes = vec_size * ଆକାର(pvb_pair_t);
	io_size= ଆକାର(u8_t);
	ଭାଙ୍ଗ;
      ସ୍ଥିତି _DIO_WORD:
	bytes = vec_size * ଆକାର(pvw_pair_t);
	io_size= ଆକାର(u16_t);
	ଭାଙ୍ଗ;
      ସ୍ଥିତି _DIO_LONG:
	bytes = vec_size * ଆକାର(pvl_pair_t);
	io_size= ଆକାର(u32_t);
	ଭାଙ୍ଗ;
      ପୂର୍ବନିର୍ଦ୍ଧାରିତ:  ଫେରନ୍ତୁ(EINVAL);   /* check type once and for all */
  }
  ଯଦି (bytes > ଆକାର(vdevio_buf))  ଫେରନ୍ତୁ(E2BIG);

  /* Copy (port,value)-pairs from user. */
  ଯଦି((r=data_copy(caller->p_endpoint, m_ptr->m_lsys_krn_sys_vdevio.vec_addr,
    KERNEL, (vir_bytes) vdevio_buf, bytes)) != OK)
	ଫେରନ୍ତୁ r;

  privp= priv(caller);
  ଯଦି (privp && (privp->s_flags & CHECK_IO_PORT))
  {
	/* Check whether the I/O is allowed */
	nr_io_range= privp->s_nr_io_range;
	ପାଇଁ (i=0; i<vec_size; i++)
	{
		ବାଛ (io_type) {
		ସ୍ଥିତି _DIO_BYTE: port= pvb[i].port; ଭାଙ୍ଗ;
		ସ୍ଥିତି _DIO_WORD: port= pvw[i].port; ଭାଙ୍ଗ;
		ପୂର୍ବନିର୍ଦ୍ଧାରିତ:	port= pvl[i].port; ଭାଙ୍ଗ;
		}
		ପାଇଁ (j= 0, iorp= privp->s_io_tab; j<nr_io_range; j++, iorp++)
		{
			ଯଦି (port >= iorp->ior_base &&
				port+io_size-1 <= iorp->ior_limit)
			{
				ଭାଙ୍ଗ;
			}
		}
		ଯଦି (j >= nr_io_range)
		{
			printf(
		"do_vdevio: I/O port check failed for proc %d, port 0x%x\n",
				caller->p_endpoint, port);
			ଫେରନ୍ତୁ EPERM;
		}
	}
  }

  /* Perform actual device I/O for byte, word, and long values */
  ବାଛ (io_type) {
  ସ୍ଥିତି _DIO_BYTE: 					 /* byte values */
      ଯଦି (io_in) ପାଇଁ (i=0; i<vec_size; i++) 
		pvb[i].ମୂଲ୍ୟ = inb( pvb[i].port); 
      ଅନ୍ୟଥା      ପାଇଁ (i=0; i<vec_size; i++)
		outb( pvb[i].port, pvb[i].ମୂଲ୍ୟ); 
      ଭାଙ୍ଗ; 
  ସ୍ଥିତି _DIO_WORD:					  /* word values */
      ଯଦି (io_in)
      {
	ପାଇଁ (i=0; i<vec_size; i++)  
	{
		port= pvw[i].port;
		ଯଦି (port & 1) ଯାଅ bad;
		pvw[i].ମୂଲ୍ୟ = inw( pvw[i].port);  
	}
      }
      ଅନ୍ୟଥା
      {
	ପାଇଁ (i=0; i<vec_size; i++) 
	{
		port= pvw[i].port;
		ଯଦି (port & 1) ଯାଅ bad;
		outw( pvw[i].port, pvw[i].ମୂଲ୍ୟ); 
	}
      }
      ଭାଙ୍ଗ; 
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:            					  /* long values */
      ଯଦି (io_in)
      {
	ପାଇଁ (i=0; i<vec_size; i++)
	{
		port= pvl[i].port;
		ଯଦି (port & 3) ଯାଅ bad;
		pvl[i].ମୂଲ୍ୟ = inl(pvl[i].port);  
	}
      }
      ଅନ୍ୟଥା
      {
	ପାଇଁ (i=0; i<vec_size; i++)
	{
		port= pvl[i].port;
		ଯଦି (port & 3) ଯାଅ bad;
		outl( pvb[i].port, pvl[i].ମୂଲ୍ୟ); 
	}
      }
  }
    
  /* Almost done, copy back results for input requests. */
  ଯଦି (io_in) 
	ଯଦି((r=data_copy(KERNEL, (vir_bytes) vdevio_buf,
	  caller->p_endpoint, m_ptr->m_lsys_krn_sys_vdevio.vec_addr,
	  (phys_bytes) bytes)) != OK)
		ଫେରନ୍ତୁ r;
  ଫେରନ୍ତୁ(OK);

bad:
	panic("do_vdevio: unaligned port: %d", port);
	ଫେରନ୍ତୁ EPERM;
}

#endif /* USE_VDEVIO */

