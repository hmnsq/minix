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

#यदि USE_DEVIO

/*===========================================================================*
 *			        do_devio                                     *
 *===========================================================================*/
पूर्णाङ्क do_devio(संरचना proc * caller, message * m_ptr)
{
    संरचना priv *privp;
    port_t port;
    संरचना io_range *iorp;
    पूर्णाङ्क i, size, nr_io_range;
    पूर्णाङ्क io_type, io_dir;

    io_type = m_ptr->m_lsys_krn_sys_devio.request & _DIO_TYPEMASK;
    io_dir  = m_ptr->m_lsys_krn_sys_devio.request & _DIO_DIRMASK;

    विकल्प (io_type)
    {
	पक्ष _DIO_BYTE: size= 1; भङ्ग;
	पक्ष _DIO_WORD: size= 2; भङ्ग;
	पक्ष _DIO_LONG: size= 4; भङ्ग;
	स्वभाव: size= 4; भङ्ग;	/* Be conservative */
    }

    privp= priv(caller);
    यदि (!privp)
    {
	printf("no priv structure!\n");
	गच्छ doit;
    }
    यदि (privp->s_flags & CHECK_IO_PORT)
    {
	port= m_ptr->m_lsys_krn_sys_devio.port;
	nr_io_range= privp->s_nr_io_range;
	प्रति (i= 0, iorp= privp->s_io_tab; i<nr_io_range; i++, iorp++)
	{
		यदि (port >= iorp->ior_base && port+size-1 <= iorp->ior_limit)
			भङ्ग;
	}
	यदि (i >= nr_io_range)
	{
			printf("do_devio: port 0x%x (size %d) not allowed\n",
				m_ptr->m_lsys_krn_sys_devio.port, size);
		प्रत्यागम EPERM;
	}
    }

doit:
    यदि (m_ptr->m_lsys_krn_sys_devio.port & (size-1))
    {
		printf("do_devio: unaligned port 0x%x (size %d)\n",
			m_ptr->m_lsys_krn_sys_devio.port, size);
	प्रत्यागम EPERM;
    }

/* Process a single I/O request for byte, word, and long values. */
    यदि (io_dir == DIO_निवेश) { 
      विकल्प (io_type) {
	/* maybe "it" should not be called ports */
        पक्ष _DIO_BYTE:
		m_ptr->m_krn_lsys_sys_devio.मूल्य =
			inb(m_ptr->m_lsys_krn_sys_devio.port);
		भङ्ग;
        पक्ष _DIO_WORD:
		m_ptr->m_krn_lsys_sys_devio.मूल्य =
			inw(m_ptr->m_lsys_krn_sys_devio.port);
		भङ्ग;
        पक्ष _DIO_LONG:
		m_ptr->m_krn_lsys_sys_devio.मूल्य =
			inl(m_ptr->m_lsys_krn_sys_devio.port);
		भङ्ग;
    	स्वभाव: प्रत्यागम(EINVAL);
      } 
    } अन्यथा { 
      विकल्प (io_type) {
	पक्ष _DIO_BYTE:
		outb(m_ptr->m_lsys_krn_sys_devio.port,
			m_ptr->m_lsys_krn_sys_devio.मूल्य);
		भङ्ग;
	पक्ष _DIO_WORD:
		outw(m_ptr->m_lsys_krn_sys_devio.port,
			m_ptr->m_lsys_krn_sys_devio.मूल्य);
		भङ्ग;
	पक्ष _DIO_LONG:
		outl(m_ptr->m_lsys_krn_sys_devio.port,
			m_ptr->m_lsys_krn_sys_devio.मूल्य);
		भङ्ग;
    	स्वभाव: प्रत्यागम(EINVAL);
      } 
    }
    प्रत्यागम(OK);
}

#endif /* USE_DEVIO */
