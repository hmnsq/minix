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

#यदि USE_VDEVIO

/* Buffer for SYS_VDEVIO to copy (port,value)-pairs from/ to user. */
स्थिर वर्ण vdevio_buf[VDEVIO_BUF_आकार];      
स्थिर pvb_pair_t * ध्रुव pvb = (pvb_pair_t *) vdevio_buf;           
स्थिर pvw_pair_t * ध्रुव pvw = (pvw_pair_t *) vdevio_buf;      
स्थिर pvl_pair_t * ध्रुव pvl = (pvl_pair_t *) vdevio_buf;     

/*===========================================================================*
 *			        do_vdevio                                    *
 *===========================================================================*/
पूर्णाङ्क do_vdevio(संरचना proc * caller, सन्देशः * m_ptr)
{
/* Perform a series of device I/O on behalf of a non-kernel process. The 
 * I/O addresses and I/O values are fetched from and returned to some buffer
 * in user space. The actual I/O is wrapped by lock() and unlock() to prevent
 * that I/O batch from being interrupted.
 * This is the counterpart of do_devio, which performs a single device I/O. 
 */ 
  पूर्णाङ्क vec_आकार;               /* size of vector */
  पूर्णाङ्क io_अन्तर्;                  /* true if input */
  size_t अष्टकानि;               /* # bytes to be copied */
  पत्तनम्_t पत्तनम्;
  पूर्णाङ्क i, j, io_आकार, nr_io_range;
  पूर्णाङ्क io_dir, io_type;
  संरचना priv *privp;
  संरचना io_range *iorp;
  पूर्णाङ्क r;
    
  /* Get the request, size of the request vector, and check the values. */
  io_dir = m_ptr->m_lsys_krn_sys_vdevio.request & _DIO_DIRMASK;
  io_type = m_ptr->m_lsys_krn_sys_vdevio.request & _DIO_TYPEMASK;
  यदि (io_dir == DIO_निवेशः) io_अन्तर् = TRUE;
  अन्यथा यदि (io_dir == _DIO_OUTPUT) io_अन्तर् = FALSE;
  अन्यथा प्रत्यागम(EINVAL);
  यदि ((vec_आकार = m_ptr->m_lsys_krn_sys_vdevio.vec_आकार) <= 0) प्रत्यागम(EINVAL);
  विकल्प (io_type) {
      पक्ष _DIO_BYTE:
	अष्टकानि = vec_आकार * परिमाण(pvb_pair_t);
	io_आकार= परिमाण(u8_t);
	भङ्ग;
      पक्ष _DIO_WORD:
	अष्टकानि = vec_आकार * परिमाण(pvw_pair_t);
	io_आकार= परिमाण(u16_t);
	भङ्ग;
      पक्ष _DIO_LONG:
	अष्टकानि = vec_आकार * परिमाण(pvl_pair_t);
	io_आकार= परिमाण(u32_t);
	भङ्ग;
      स्वभाव:  प्रत्यागम(EINVAL);   /* check type once and for all */
  }
  यदि (अष्टकानि > परिमाण(vdevio_buf))  प्रत्यागम(E2BIG);

  /* Copy (port,value)-pairs from user. */
  यदि((r=दत्त_copy(caller->p_endpoint, m_ptr->m_lsys_krn_sys_vdevio.vec_addr,
    KERNEL, (vir_अष्टकानि) vdevio_buf, अष्टकानि)) != OK)
	प्रत्यागम r;

  privp= priv(caller);
  यदि (privp && (privp->s_flags & CHECK_IO_पत्तनम्))
  {
	/* Check whether the I/O is allowed */
	nr_io_range= privp->s_nr_io_range;
	प्रति (i=0; i<vec_आकार; i++)
	{
		विकल्प (io_type) {
		पक्ष _DIO_BYTE: पत्तनम्= pvb[i].पत्तनम्; भङ्ग;
		पक्ष _DIO_WORD: पत्तनम्= pvw[i].पत्तनम्; भङ्ग;
		स्वभाव:	पत्तनम्= pvl[i].पत्तनम्; भङ्ग;
		}
		प्रति (j= 0, iorp= privp->s_io_tab; j<nr_io_range; j++, iorp++)
		{
			यदि (पत्तनम् >= iorp->ior_आधारः &&
				पत्तनम्+io_आकार-1 <= iorp->ior_सीमा)
			{
				भङ्ग;
			}
		}
		यदि (j >= nr_io_range)
		{
			प्रारूप_लिख(
		"do_vdevio: I/O port check failed for proc %d, port 0x%x\n",
				caller->p_endpoint, पत्तनम्);
			प्रत्यागम EPERM;
		}
	}
  }

  /* Perform actual device I/O for byte, word, and long values */
  विकल्प (io_type) {
  पक्ष _DIO_BYTE: 					 /* byte values */
      यदि (io_अन्तर्) प्रति (i=0; i<vec_आकार; i++) 
		pvb[i].मानम् = inb( pvb[i].पत्तनम्); 
      अन्यथा      प्रति (i=0; i<vec_आकार; i++)
		outb( pvb[i].पत्तनम्, pvb[i].मानम्); 
      भङ्ग; 
  पक्ष _DIO_WORD:					  /* word values */
      यदि (io_अन्तर्)
      {
	प्रति (i=0; i<vec_आकार; i++)  
	{
		पत्तनम्= pvw[i].पत्तनम्;
		यदि (पत्तनम् & 1) गच्छ bad;
		pvw[i].मानम् = inw( pvw[i].पत्तनम्);  
	}
      }
      अन्यथा
      {
	प्रति (i=0; i<vec_आकार; i++) 
	{
		पत्तनम्= pvw[i].पत्तनम्;
		यदि (पत्तनम् & 1) गच्छ bad;
		outw( pvw[i].पत्तनम्, pvw[i].मानम्); 
	}
      }
      भङ्ग; 
  स्वभाव:            					  /* long values */
      यदि (io_अन्तर्)
      {
	प्रति (i=0; i<vec_आकार; i++)
	{
		पत्तनम्= pvl[i].पत्तनम्;
		यदि (पत्तनम् & 3) गच्छ bad;
		pvl[i].मानम् = inl(pvl[i].पत्तनम्);  
	}
      }
      अन्यथा
      {
	प्रति (i=0; i<vec_आकार; i++)
	{
		पत्तनम्= pvl[i].पत्तनम्;
		यदि (पत्तनम् & 3) गच्छ bad;
		outl( pvb[i].पत्तनम्, pvl[i].मानम्); 
	}
      }
  }
    
  /* Almost done, copy back results for input requests. */
  यदि (io_अन्तर्) 
	यदि((r=दत्त_copy(KERNEL, (vir_अष्टकानि) vdevio_buf,
	  caller->p_endpoint, m_ptr->m_lsys_krn_sys_vdevio.vec_addr,
	  (phys_अष्टकानि) अष्टकानि)) != OK)
		प्रत्यागम r;
  प्रत्यागम(OK);

bad:
	panic("do_vdevio: unaligned port: %d", पत्तनम्);
	प्रत्यागम EPERM;
}

#यदि_अन्तः /* USE_VDEVIO */

