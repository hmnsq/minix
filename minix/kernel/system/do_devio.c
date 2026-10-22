/* The kernel call implemented in this file:
 *   m_type:	SYS_DEVIO
 *
 * The parameters for this kernel call are:
 *   m_lsys_krn_sys_devio.request	(request input or output)
 *   m_lsys_krn_sys_devio.port		(port to read/ write)
 *   m_lsys_krn_sys_devio.value		(value to write/ return value read)
 */

#অন্তর্ভাৱ "kernel/system.h"
#অন্তর্ভাৱ <minix/devio.h>
#অন্তর্ভাৱ <minix/endpoint.h>

#যদি USE_DEVIO

/*===========================================================================*
 *			        do_devio                                     *
 *===========================================================================*/
পূর্ণাঙ্ক do_devio(সংরচনা proc * caller, message * m_ptr)
{
    সংরচনা priv *privp;
    port_t port;
    সংরচনা io_range *iorp;
    পূর্ণাঙ্ক i, size, nr_io_range;
    পূর্ণাঙ্ক io_type, io_dir;

    io_type = m_ptr->m_lsys_krn_sys_devio.request & _DIO_TYPEMASK;
    io_dir  = m_ptr->m_lsys_krn_sys_devio.request & _DIO_DIRMASK;

    ৱিকল্প (io_type)
    {
	পক্ষ _DIO_BYTE: size= 1; ভঙ্গ;
	পক্ষ _DIO_WORD: size= 2; ভঙ্গ;
	পক্ষ _DIO_LONG: size= 4; ভঙ্গ;
	স্ৱভাৱ: size= 4; ভঙ্গ;	/* Be conservative */
    }

    privp= priv(caller);
    যদি (!privp)
    {
	printf("no priv structure!\n");
	গচ্ছ doit;
    }
    যদি (privp->s_flags & CHECK_IO_PORT)
    {
	port= m_ptr->m_lsys_krn_sys_devio.port;
	nr_io_range= privp->s_nr_io_range;
	বাবে (i= 0, iorp= privp->s_io_tab; i<nr_io_range; i++, iorp++)
	{
		যদি (port >= iorp->ior_base && port+size-1 <= iorp->ior_limit)
			ভঙ্গ;
	}
	যদি (i >= nr_io_range)
	{
			printf("do_devio: port 0x%x (size %d) not allowed\n",
				m_ptr->m_lsys_krn_sys_devio.port, size);
		ঘূৰাই EPERM;
	}
    }

doit:
    যদি (m_ptr->m_lsys_krn_sys_devio.port & (size-1))
    {
		printf("do_devio: unaligned port 0x%x (size %d)\n",
			m_ptr->m_lsys_krn_sys_devio.port, size);
	ঘূৰাই EPERM;
    }

/* Process a single I/O request for byte, word, and long values. */
    যদি (io_dir == DIO_প্ৰবেশ) { 
      ৱিকল্প (io_type) {
	/* maybe "it" should not be called ports */
        পক্ষ _DIO_BYTE:
		m_ptr->m_krn_lsys_sys_devio.মান =
			inb(m_ptr->m_lsys_krn_sys_devio.port);
		ভঙ্গ;
        পক্ষ _DIO_WORD:
		m_ptr->m_krn_lsys_sys_devio.মান =
			inw(m_ptr->m_lsys_krn_sys_devio.port);
		ভঙ্গ;
        পক্ষ _DIO_LONG:
		m_ptr->m_krn_lsys_sys_devio.মান =
			inl(m_ptr->m_lsys_krn_sys_devio.port);
		ভঙ্গ;
    	স্ৱভাৱ: ঘূৰাই(EINVAL);
      } 
    } অন্যথা { 
      ৱিকল্প (io_type) {
	পক্ষ _DIO_BYTE:
		outb(m_ptr->m_lsys_krn_sys_devio.port,
			m_ptr->m_lsys_krn_sys_devio.মান);
		ভঙ্গ;
	পক্ষ _DIO_WORD:
		outw(m_ptr->m_lsys_krn_sys_devio.port,
			m_ptr->m_lsys_krn_sys_devio.মান);
		ভঙ্গ;
	পক্ষ _DIO_LONG:
		outl(m_ptr->m_lsys_krn_sys_devio.port,
			m_ptr->m_lsys_krn_sys_devio.মান);
		ভঙ্গ;
    	স্ৱভাৱ: ঘূৰাই(EINVAL);
      } 
    }
    ঘূৰাই(OK);
}

#endif /* USE_DEVIO */
