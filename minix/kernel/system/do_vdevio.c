/* The kernel call implemented in this file:
 *   m_type:	SYS_VDEVIO
 *
 * The parameters for this kernel call are:
 *    m_lsys_krn_sys_vdevio.request	(request input or output)
 *    m_lsys_krn_sys_vdevio.vec_addr	(pointer to port/ value pairs)
 *    m_lsys_krn_sys_vdevio.vec_size	(number of ports to read or write)
 */

#अन्तर्भाव "kernel/system.h"
#अन्तर्भाव <minix/devio.h>
#अन्तर्भाव <minix/endpoint.h>

#जर USE_VDEVIO

/* Buffer for SYS_VDEVIO to copy (port,value)-pairs from/ to user. */
स्थिर अक्षर vdevio_buf[VDEVIO_BUF_SIZE];      
स्थिर pvb_pair_t * स्थिरांक pvb = (pvb_pair_t *) vdevio_buf;           
स्थिर pvw_pair_t * स्थिरांक pvw = (pvw_pair_t *) vdevio_buf;      
स्थिर pvl_pair_t * स्थिरांक pvl = (pvl_pair_t *) vdevio_buf;     

/*===========================================================================*
 *			        do_vdevio                                    *
 *===========================================================================*/
पूर्णांक do_vdevio(संरचना proc * caller, message * m_ptr)
{
/* Perform a series of device I/O on behalf of a non-kernel process. The 
 * I/O addresses and I/O values are fetched from and returned to some buffer
 * in user space. The actual I/O is wrapped by lock() and unlock() to prevent
 * that I/O batch from being interrupted.
 * This is the counterpart of do_devio, which performs a single device I/O. 
 */ 
  पूर्णांक vec_size;               /* size of vector */
  पूर्णांक io_in;                  /* true if input */
  माप_प्रकार bytes;               /* # bytes to be copied */
  port_t port;
  पूर्णांक i, j, io_size, nr_io_range;
  पूर्णांक io_dir, io_type;
  संरचना priv *privp;
  संरचना io_range *iorp;
  पूर्णांक r;
    
  /* Get the request, size of the request vector, and check the values. */
  io_dir = m_ptr->m_lsys_krn_sys_vdevio.request & _DIO_DIRMASK;
  io_type = m_ptr->m_lsys_krn_sys_vdevio.request & _DIO_TYPEMASK;
  जर (io_dir == DIO_आदान) io_in = TRUE;
  अन्यथा जर (io_dir == _DIO_OUTPUT) io_in = FALSE;
  अन्यथा परत(EINVAL);
  जर ((vec_size = m_ptr->m_lsys_krn_sys_vdevio.vec_size) <= 0) परत(EINVAL);
  निवड (io_type) {
      स्थिती _DIO_BYTE:
	bytes = vec_size * आकार(pvb_pair_t);
	io_size= आकार(u8_t);
	थांबा;
      स्थिती _DIO_WORD:
	bytes = vec_size * आकार(pvw_pair_t);
	io_size= आकार(u16_t);
	थांबा;
      स्थिती _DIO_LONG:
	bytes = vec_size * आकार(pvl_pair_t);
	io_size= आकार(u32_t);
	थांबा;
      पूर्वनिर्धारित:  परत(EINVAL);   /* check type once and for all */
  }
  जर (bytes > आकार(vdevio_buf))  परत(E2BIG);

  /* Copy (port,value)-pairs from user. */
  जर((r=data_copy(caller->p_endpoint, m_ptr->m_lsys_krn_sys_vdevio.vec_addr,
    KERNEL, (vir_bytes) vdevio_buf, bytes)) != OK)
	परत r;

  privp= priv(caller);
  जर (privp && (privp->s_flags & CHECK_IO_PORT))
  {
	/* Check whether the I/O is allowed */
	nr_io_range= privp->s_nr_io_range;
	साठी (i=0; i<vec_size; i++)
	{
		निवड (io_type) {
		स्थिती _DIO_BYTE: port= pvb[i].port; थांबा;
		स्थिती _DIO_WORD: port= pvw[i].port; थांबा;
		पूर्वनिर्धारित:	port= pvl[i].port; थांबा;
		}
		साठी (j= 0, iorp= privp->s_io_tab; j<nr_io_range; j++, iorp++)
		{
			जर (port >= iorp->ior_base &&
				port+io_size-1 <= iorp->ior_limit)
			{
				थांबा;
			}
		}
		जर (j >= nr_io_range)
		{
			म_लिखो(
		"do_vdevio: I/O port check failed for proc %d, port 0x%x\n",
				caller->p_endpoint, port);
			परत EPERM;
		}
	}
  }

  /* Perform actual device I/O for byte, word, and long values */
  निवड (io_type) {
  स्थिती _DIO_BYTE: 					 /* byte values */
      जर (io_in) साठी (i=0; i<vec_size; i++) 
		pvb[i].मूल्य = inb( pvb[i].port); 
      अन्यथा      साठी (i=0; i<vec_size; i++)
		outb( pvb[i].port, pvb[i].मूल्य); 
      थांबा; 
  स्थिती _DIO_WORD:					  /* word values */
      जर (io_in)
      {
	साठी (i=0; i<vec_size; i++)  
	{
		port= pvw[i].port;
		जर (port & 1) जा bad;
		pvw[i].मूल्य = inw( pvw[i].port);  
	}
      }
      अन्यथा
      {
	साठी (i=0; i<vec_size; i++) 
	{
		port= pvw[i].port;
		जर (port & 1) जा bad;
		outw( pvw[i].port, pvw[i].मूल्य); 
	}
      }
      थांबा; 
  पूर्वनिर्धारित:            					  /* long values */
      जर (io_in)
      {
	साठी (i=0; i<vec_size; i++)
	{
		port= pvl[i].port;
		जर (port & 3) जा bad;
		pvl[i].मूल्य = inl(pvl[i].port);  
	}
      }
      अन्यथा
      {
	साठी (i=0; i<vec_size; i++)
	{
		port= pvl[i].port;
		जर (port & 3) जा bad;
		outl( pvb[i].port, pvl[i].मूल्य); 
	}
      }
  }
    
  /* Almost done, copy back results for input requests. */
  जर (io_in) 
	जर((r=data_copy(KERNEL, (vir_bytes) vdevio_buf,
	  caller->p_endpoint, m_ptr->m_lsys_krn_sys_vdevio.vec_addr,
	  (phys_bytes) bytes)) != OK)
		परत r;
  परत(OK);

bad:
	panic("do_vdevio: unaligned port: %d", port);
	परत EPERM;
}

#endif /* USE_VDEVIO */

