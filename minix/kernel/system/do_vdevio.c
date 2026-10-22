/* The kernel call implemented in this file:
 *   m_type:	SYS_VDEVIO
 *
 * The parameters for this kernel call are:
 *    m_lsys_krn_sys_vdevio.request	(request input or output)
 *    m_lsys_krn_sys_vdevio.vec_addr	(pointer to port/ value pairs)
 *    m_lsys_krn_sys_vdevio.vec_size	(number of ports to read or write)
 */

#അന്തര്ഭാവ "kernel/system.h"
#അന്തര്ഭാവ <minix/devio.h>
#അന്തര്ഭാവ <minix/endpoint.h>

#എങ്കിൽ USE_VDEVIO

/* Buffer for SYS_VDEVIO to copy (port,value)-pairs from/ to user. */
സ്ഥിതം അക്ഷരം vdevio_buf[VDEVIO_BUF_SIZE];      
സ്ഥിതം pvb_pair_t * സ്ഥിരം pvb = (pvb_pair_t *) vdevio_buf;           
സ്ഥിതം pvw_pair_t * സ്ഥിരം pvw = (pvw_pair_t *) vdevio_buf;      
സ്ഥിതം pvl_pair_t * സ്ഥിരം pvl = (pvl_pair_t *) vdevio_buf;     

/*===========================================================================*
 *			        do_vdevio                                    *
 *===========================================================================*/
പൂർണ്ണസംഖ്യ do_vdevio(ഘടന proc * caller, message * m_ptr)
{
/* Perform a series of device I/O on behalf of a non-kernel process. The 
 * I/O addresses and I/O values are fetched from and returned to some buffer
 * in user space. The actual I/O is wrapped by lock() and unlock() to prevent
 * that I/O batch from being interrupted.
 * This is the counterpart of do_devio, which performs a single device I/O. 
 */ 
  പൂർണ്ണസംഖ്യ vec_size;               /* size of vector */
  പൂർണ്ണസംഖ്യ io_in;                  /* true if input */
  size_t bytes;               /* # bytes to be copied */
  port_t port;
  പൂർണ്ണസംഖ്യ i, j, io_size, nr_io_range;
  പൂർണ്ണസംഖ്യ io_dir, io_type;
  ഘടന priv *privp;
  ഘടന io_range *iorp;
  പൂർണ്ണസംഖ്യ r;
    
  /* Get the request, size of the request vector, and check the values. */
  io_dir = m_ptr->m_lsys_krn_sys_vdevio.request & _DIO_DIRMASK;
  io_type = m_ptr->m_lsys_krn_sys_vdevio.request & _DIO_TYPEMASK;
  എങ്കിൽ (io_dir == DIO_നിവേശം) io_in = TRUE;
  അല്ലെങ്കിൽ എങ്കിൽ (io_dir == _DIO_OUTPUT) io_in = FALSE;
  അല്ലെങ്കിൽ തിരികെ(EINVAL);
  എങ്കിൽ ((vec_size = m_ptr->m_lsys_krn_sys_vdevio.vec_size) <= 0) തിരികെ(EINVAL);
  തിരഞ്ഞെടുപ്പ് (io_type) {
      അവസ്ഥ _DIO_BYTE:
	bytes = vec_size * വലുപ്പം(pvb_pair_t);
	io_size= വലുപ്പം(u8_t);
	നിർത്തുക;
      അവസ്ഥ _DIO_WORD:
	bytes = vec_size * വലുപ്പം(pvw_pair_t);
	io_size= വലുപ്പം(u16_t);
	നിർത്തുക;
      അവസ്ഥ _DIO_LONG:
	bytes = vec_size * വലുപ്പം(pvl_pair_t);
	io_size= വലുപ്പം(u32_t);
	നിർത്തുക;
      സ്ഥിരസ്ഥിതി:  തിരികെ(EINVAL);   /* check type once and for all */
  }
  എങ്കിൽ (bytes > വലുപ്പം(vdevio_buf))  തിരികെ(E2BIG);

  /* Copy (port,value)-pairs from user. */
  എങ്കിൽ((r=data_copy(caller->p_endpoint, m_ptr->m_lsys_krn_sys_vdevio.vec_addr,
    KERNEL, (vir_bytes) vdevio_buf, bytes)) != OK)
	തിരികെ r;

  privp= priv(caller);
  എങ്കിൽ (privp && (privp->s_flags & CHECK_IO_PORT))
  {
	/* Check whether the I/O is allowed */
	nr_io_range= privp->s_nr_io_range;
	വേണ്ടി (i=0; i<vec_size; i++)
	{
		തിരഞ്ഞെടുപ്പ് (io_type) {
		അവസ്ഥ _DIO_BYTE: port= pvb[i].port; നിർത്തുക;
		അവസ്ഥ _DIO_WORD: port= pvw[i].port; നിർത്തുക;
		സ്ഥിരസ്ഥിതി:	port= pvl[i].port; നിർത്തുക;
		}
		വേണ്ടി (j= 0, iorp= privp->s_io_tab; j<nr_io_range; j++, iorp++)
		{
			എങ്കിൽ (port >= iorp->ior_base &&
				port+io_size-1 <= iorp->ior_limit)
			{
				നിർത്തുക;
			}
		}
		എങ്കിൽ (j >= nr_io_range)
		{
			printf(
		"do_vdevio: I/O port check failed for proc %d, port 0x%x\n",
				caller->p_endpoint, port);
			തിരികെ EPERM;
		}
	}
  }

  /* Perform actual device I/O for byte, word, and long values */
  തിരഞ്ഞെടുപ്പ് (io_type) {
  അവസ്ഥ _DIO_BYTE: 					 /* byte values */
      എങ്കിൽ (io_in) വേണ്ടി (i=0; i<vec_size; i++) 
		pvb[i].മൂല്യം = inb( pvb[i].port); 
      അല്ലെങ്കിൽ      വേണ്ടി (i=0; i<vec_size; i++)
		outb( pvb[i].port, pvb[i].മൂല്യം); 
      നിർത്തുക; 
  അവസ്ഥ _DIO_WORD:					  /* word values */
      എങ്കിൽ (io_in)
      {
	വേണ്ടി (i=0; i<vec_size; i++)  
	{
		port= pvw[i].port;
		എങ്കിൽ (port & 1) പോകുക bad;
		pvw[i].മൂല്യം = inw( pvw[i].port);  
	}
      }
      അല്ലെങ്കിൽ
      {
	വേണ്ടി (i=0; i<vec_size; i++) 
	{
		port= pvw[i].port;
		എങ്കിൽ (port & 1) പോകുക bad;
		outw( pvw[i].port, pvw[i].മൂല്യം); 
	}
      }
      നിർത്തുക; 
  സ്ഥിരസ്ഥിതി:            					  /* long values */
      എങ്കിൽ (io_in)
      {
	വേണ്ടി (i=0; i<vec_size; i++)
	{
		port= pvl[i].port;
		എങ്കിൽ (port & 3) പോകുക bad;
		pvl[i].മൂല്യം = inl(pvl[i].port);  
	}
      }
      അല്ലെങ്കിൽ
      {
	വേണ്ടി (i=0; i<vec_size; i++)
	{
		port= pvl[i].port;
		എങ്കിൽ (port & 3) പോകുക bad;
		outl( pvb[i].port, pvl[i].മൂല്യം); 
	}
      }
  }
    
  /* Almost done, copy back results for input requests. */
  എങ്കിൽ (io_in) 
	എങ്കിൽ((r=data_copy(KERNEL, (vir_bytes) vdevio_buf,
	  caller->p_endpoint, m_ptr->m_lsys_krn_sys_vdevio.vec_addr,
	  (phys_bytes) bytes)) != OK)
		തിരികെ r;
  തിരികെ(OK);

bad:
	panic("do_vdevio: unaligned port: %d", port);
	തിരികെ EPERM;
}

#endif /* USE_VDEVIO */

