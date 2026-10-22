/* The kernel call implemented in this file:
 *   m_type:	SYS_DEVIO
 *
 * The parameters for this kernel call are:
 *   m_lsys_krn_sys_devio.request	(request input or output)
 *   m_lsys_krn_sys_devio.port		(port to read/ write)
 *   m_lsys_krn_sys_devio.value		(value to write/ return value read)
 */

#अन्तर्भाव "kernel/system.h"
#अन्तर्भाव <minix/devio.h>
#अन्तर्भाव <minix/endpoint.h>

#जर USE_DEVIO

/*===========================================================================*
 *			        do_devio                                     *
 *===========================================================================*/
पूर्णांक do_devio(संरचना proc * caller, message * m_ptr)
{
    संरचना priv *privp;
    port_t port;
    संरचना io_range *iorp;
    पूर्णांक i, size, nr_io_range;
    पूर्णांक io_type, io_dir;

    io_type = m_ptr->m_lsys_krn_sys_devio.request & _DIO_TYPEMASK;
    io_dir  = m_ptr->m_lsys_krn_sys_devio.request & _DIO_DIRMASK;

    निवड (io_type)
    {
	स्थिती _DIO_BYTE: size= 1; थांबा;
	स्थिती _DIO_WORD: size= 2; थांबा;
	स्थिती _DIO_LONG: size= 4; थांबा;
	पूर्वनिर्धारित: size= 4; थांबा;	/* Be conservative */
    }

    privp= priv(caller);
    जर (!privp)
    {
	म_लिखो("no priv structure!\n");
	जा doit;
    }
    जर (privp->s_flags & CHECK_IO_PORT)
    {
	port= m_ptr->m_lsys_krn_sys_devio.port;
	nr_io_range= privp->s_nr_io_range;
	साठी (i= 0, iorp= privp->s_io_tab; i<nr_io_range; i++, iorp++)
	{
		जर (port >= iorp->ior_base && port+size-1 <= iorp->ior_limit)
			थांबा;
	}
	जर (i >= nr_io_range)
	{
			म_लिखो("do_devio: port 0x%x (size %d) not allowed\n",
				m_ptr->m_lsys_krn_sys_devio.port, size);
		परत EPERM;
	}
    }

doit:
    जर (m_ptr->m_lsys_krn_sys_devio.port & (size-1))
    {
		म_लिखो("do_devio: unaligned port 0x%x (size %d)\n",
			m_ptr->m_lsys_krn_sys_devio.port, size);
	परत EPERM;
    }

/* Process a single I/O request for byte, word, and long values. */
    जर (io_dir == DIO_आदान) { 
      निवड (io_type) {
	/* maybe "it" should not be called ports */
        स्थिती _DIO_BYTE:
		m_ptr->m_krn_lsys_sys_devio.मूल्य =
			inb(m_ptr->m_lsys_krn_sys_devio.port);
		थांबा;
        स्थिती _DIO_WORD:
		m_ptr->m_krn_lsys_sys_devio.मूल्य =
			inw(m_ptr->m_lsys_krn_sys_devio.port);
		थांबा;
        स्थिती _DIO_LONG:
		m_ptr->m_krn_lsys_sys_devio.मूल्य =
			inl(m_ptr->m_lsys_krn_sys_devio.port);
		थांबा;
    	पूर्वनिर्धारित: परत(EINVAL);
      } 
    } अन्यथा { 
      निवड (io_type) {
	स्थिती _DIO_BYTE:
		outb(m_ptr->m_lsys_krn_sys_devio.port,
			m_ptr->m_lsys_krn_sys_devio.मूल्य);
		थांबा;
	स्थिती _DIO_WORD:
		outw(m_ptr->m_lsys_krn_sys_devio.port,
			m_ptr->m_lsys_krn_sys_devio.मूल्य);
		थांबा;
	स्थिती _DIO_LONG:
		outl(m_ptr->m_lsys_krn_sys_devio.port,
			m_ptr->m_lsys_krn_sys_devio.मूल्य);
		थांबा;
    	पूर्वनिर्धारित: परत(EINVAL);
      } 
    }
    परत(OK);
}

#endif /* USE_DEVIO */
