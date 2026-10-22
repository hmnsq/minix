/* The kernel call implemented in this file:
 *   m_type:	SYS_DEVIO
 *
 * The parameters for this kernel call are:
 *   m_lsys_krn_sys_devio.request	(request input or output)
 *   m_lsys_krn_sys_devio.port		(port to read/ write)
 *   m_lsys_krn_sys_devio.value		(value to write/ return value read)
 */

#समावेश "kernel/system.h"
#समावेश <minix/devio.h>
#समावेश <minix/endpoint.h>

#अगर USE_DEVIO

/*===========================================================================*
 *			        do_devio                                     *
 *===========================================================================*/
पूर्णांक do_devio(काष्ठा proc * caller, message * m_ptr)
{
    काष्ठा priv *privp;
    port_t port;
    काष्ठा io_range *iorp;
    पूर्णांक i, आकार, nr_io_range;
    पूर्णांक io_type, io_dir;

    io_type = m_ptr->m_lsys_krn_sys_devio.request & _DIO_TYPEMASK;
    io_dir  = m_ptr->m_lsys_krn_sys_devio.request & _DIO_DIRMASK;

    चयन (io_type)
    {
	हाल _DIO_BYTE: आकार= 1; अवरोध;
	हाल _DIO_WORD: आकार= 2; अवरोध;
	हाल _DIO_LONG: आकार= 4; अवरोध;
	शेष: आकार= 4; अवरोध;	/* Be conservative */
    }

    privp= priv(caller);
    अगर (!privp)
    {
	म_लिखो("no priv structure!\n");
	जाओ doit;
    }
    अगर (privp->s_flags & CHECK_IO_PORT)
    {
	port= m_ptr->m_lsys_krn_sys_devio.port;
	nr_io_range= privp->s_nr_io_range;
	क्रम (i= 0, iorp= privp->s_io_tab; i<nr_io_range; i++, iorp++)
	{
		अगर (port >= iorp->ior_base && port+आकार-1 <= iorp->ior_limit)
			अवरोध;
	}
	अगर (i >= nr_io_range)
	{
			म_लिखो("do_devio: port 0x%x (size %d) not allowed\n",
				m_ptr->m_lsys_krn_sys_devio.port, आकार);
		वापस EPERM;
	}
    }

doit:
    अगर (m_ptr->m_lsys_krn_sys_devio.port & (आकार-1))
    {
		म_लिखो("do_devio: unaligned port 0x%x (size %d)\n",
			m_ptr->m_lsys_krn_sys_devio.port, आकार);
	वापस EPERM;
    }

/* Process a single I/O request for byte, word, and long values. */
    अगर (io_dir == DIO_निवेश) { 
      चयन (io_type) {
	/* maybe "it" should not be called ports */
        हाल _DIO_BYTE:
		m_ptr->m_krn_lsys_sys_devio.मूल्य =
			inb(m_ptr->m_lsys_krn_sys_devio.port);
		अवरोध;
        हाल _DIO_WORD:
		m_ptr->m_krn_lsys_sys_devio.मूल्य =
			inw(m_ptr->m_lsys_krn_sys_devio.port);
		अवरोध;
        हाल _DIO_LONG:
		m_ptr->m_krn_lsys_sys_devio.मूल्य =
			inl(m_ptr->m_lsys_krn_sys_devio.port);
		अवरोध;
    	शेष: वापस(EINVAL);
      } 
    } अन्यथा { 
      चयन (io_type) {
	हाल _DIO_BYTE:
		outb(m_ptr->m_lsys_krn_sys_devio.port,
			m_ptr->m_lsys_krn_sys_devio.मूल्य);
		अवरोध;
	हाल _DIO_WORD:
		outw(m_ptr->m_lsys_krn_sys_devio.port,
			m_ptr->m_lsys_krn_sys_devio.मूल्य);
		अवरोध;
	हाल _DIO_LONG:
		outl(m_ptr->m_lsys_krn_sys_devio.port,
			m_ptr->m_lsys_krn_sys_devio.मूल्य);
		अवरोध;
    	शेष: वापस(EINVAL);
      } 
    }
    वापस(OK);
}

#पूर्ण_अगर /* USE_DEVIO */
