/* The kernel call implemented in this file:
 *   m_type:	SYS_VDEVIO
 *
 * The parameters for this kernel call are:
 *    m_lsys_krn_sys_vdevio.request	(request input or output)
 *    m_lsys_krn_sys_vdevio.vec_addr	(pointer to port/ value pairs)
 *    m_lsys_krn_sys_vdevio.vec_size	(number of ports to read or write)
 */

#समावेश "kernel/system.h"
#समावेश <minix/devio.h>
#समावेश <minix/endpoint.h>

#अगर USE_VDEVIO

/* Buffer for SYS_VDEVIO to copy (port,value)-pairs from/ to user. */
अटल अक्षर vdevio_buf[VDEVIO_BUF_आकार];      
अटल pvb_pair_t * स्थिर pvb = (pvb_pair_t *) vdevio_buf;           
अटल pvw_pair_t * स्थिर pvw = (pvw_pair_t *) vdevio_buf;      
अटल pvl_pair_t * स्थिर pvl = (pvl_pair_t *) vdevio_buf;     

/*===========================================================================*
 *			        do_vdevio                                    *
 *===========================================================================*/
पूर्णांक do_vdevio(काष्ठा proc * caller, message * m_ptr)
{
/* Perform a series of device I/O on behalf of a non-kernel process. The 
 * I/O addresses and I/O values are fetched from and returned to some buffer
 * in user space. The actual I/O is wrapped by lock() and unlock() to prevent
 * that I/O batch from being interrupted.
 * This is the counterpart of do_devio, which performs a single device I/O. 
 */ 
  पूर्णांक vec_आकार;               /* size of vector */
  पूर्णांक io_in;                  /* true if input */
  माप_प्रकार bytes;               /* # bytes to be copied */
  port_t port;
  पूर्णांक i, j, io_आकार, nr_io_range;
  पूर्णांक io_dir, io_type;
  काष्ठा priv *privp;
  काष्ठा io_range *iorp;
  पूर्णांक r;
    
  /* Get the request, size of the request vector, and check the values. */
  io_dir = m_ptr->m_lsys_krn_sys_vdevio.request & _DIO_DIRMASK;
  io_type = m_ptr->m_lsys_krn_sys_vdevio.request & _DIO_TYPEMASK;
  अगर (io_dir == DIO_निवेश) io_in = TRUE;
  अन्यथा अगर (io_dir == _DIO_OUTPUT) io_in = FALSE;
  अन्यथा वापस(EINVAL);
  अगर ((vec_आकार = m_ptr->m_lsys_krn_sys_vdevio.vec_आकार) <= 0) वापस(EINVAL);
  चयन (io_type) {
      हाल _DIO_BYTE:
	bytes = vec_आकार * माप(pvb_pair_t);
	io_आकार= माप(u8_t);
	अवरोध;
      हाल _DIO_WORD:
	bytes = vec_आकार * माप(pvw_pair_t);
	io_आकार= माप(u16_t);
	अवरोध;
      हाल _DIO_LONG:
	bytes = vec_आकार * माप(pvl_pair_t);
	io_आकार= माप(u32_t);
	अवरोध;
      शेष:  वापस(EINVAL);   /* check type once and for all */
  }
  अगर (bytes > माप(vdevio_buf))  वापस(E2BIG);

  /* Copy (port,value)-pairs from user. */
  अगर((r=तथ्य_copy(caller->p_endpoint, m_ptr->m_lsys_krn_sys_vdevio.vec_addr,
    KERNEL, (vir_bytes) vdevio_buf, bytes)) != OK)
	वापस r;

  privp= priv(caller);
  अगर (privp && (privp->s_flags & CHECK_IO_PORT))
  {
	/* Check whether the I/O is allowed */
	nr_io_range= privp->s_nr_io_range;
	क्रम (i=0; i<vec_आकार; i++)
	{
		चयन (io_type) {
		हाल _DIO_BYTE: port= pvb[i].port; अवरोध;
		हाल _DIO_WORD: port= pvw[i].port; अवरोध;
		शेष:	port= pvl[i].port; अवरोध;
		}
		क्रम (j= 0, iorp= privp->s_io_tab; j<nr_io_range; j++, iorp++)
		{
			अगर (port >= iorp->ior_base &&
				port+io_आकार-1 <= iorp->ior_limit)
			{
				अवरोध;
			}
		}
		अगर (j >= nr_io_range)
		{
			म_लिखो(
		"do_vdevio: I/O port check failed for proc %d, port 0x%x\n",
				caller->p_endpoint, port);
			वापस EPERM;
		}
	}
  }

  /* Perform actual device I/O for byte, word, and long values */
  चयन (io_type) {
  हाल _DIO_BYTE: 					 /* byte values */
      अगर (io_in) क्रम (i=0; i<vec_आकार; i++) 
		pvb[i].मूल्य = inb( pvb[i].port); 
      अन्यथा      क्रम (i=0; i<vec_आकार; i++)
		outb( pvb[i].port, pvb[i].मूल्य); 
      अवरोध; 
  हाल _DIO_WORD:					  /* word values */
      अगर (io_in)
      {
	क्रम (i=0; i<vec_आकार; i++)  
	{
		port= pvw[i].port;
		अगर (port & 1) जाओ bad;
		pvw[i].मूल्य = inw( pvw[i].port);  
	}
      }
      अन्यथा
      {
	क्रम (i=0; i<vec_आकार; i++) 
	{
		port= pvw[i].port;
		अगर (port & 1) जाओ bad;
		outw( pvw[i].port, pvw[i].मूल्य); 
	}
      }
      अवरोध; 
  शेष:            					  /* long values */
      अगर (io_in)
      {
	क्रम (i=0; i<vec_आकार; i++)
	{
		port= pvl[i].port;
		अगर (port & 3) जाओ bad;
		pvl[i].मूल्य = inl(pvl[i].port);  
	}
      }
      अन्यथा
      {
	क्रम (i=0; i<vec_आकार; i++)
	{
		port= pvl[i].port;
		अगर (port & 3) जाओ bad;
		outl( pvb[i].port, pvl[i].मूल्य); 
	}
      }
  }
    
  /* Almost done, copy back results for input requests. */
  अगर (io_in) 
	अगर((r=तथ्य_copy(KERNEL, (vir_bytes) vdevio_buf,
	  caller->p_endpoint, m_ptr->m_lsys_krn_sys_vdevio.vec_addr,
	  (phys_bytes) bytes)) != OK)
		वापस r;
  वापस(OK);

bad:
	panic("do_vdevio: unaligned port: %d", port);
	वापस EPERM;
}

#पूर्ण_अगर /* USE_VDEVIO */

