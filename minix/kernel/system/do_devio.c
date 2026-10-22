/* The kernel call implemented in this file:
 *   m_type:	SYS_DEVIO
 *
 * The parameters for this kernel call are:
 *   m_lsys_krn_sys_devio.request	(request input or output)
 *   m_lsys_krn_sys_devio.port		(port to read/ write)
 *   m_lsys_krn_sys_devio.value		(value to write/ return value read)
 */

#অন্তর্ভাব "kernel/system.h"
#অন্তর্ভাব <minix/devio.h>
#অন্তর্ভাব <minix/endpoint.h>

#যদি USE_DEVIO

/*===========================================================================*
 *			        do_devio                                     *
 *===========================================================================*/
পূর্ণসংখ্যা do_devio(কাঠামো proc * caller, message * m_ptr)
{
    কাঠামো priv *privp;
    port_t port;
    কাঠামো io_range *iorp;
    পূর্ণসংখ্যা i, আকার_2, nr_io_range;
    পূর্ণসংখ্যা io_type, io_dir;

    io_type = m_ptr->m_lsys_krn_sys_devio.request & _DIO_TYPEMASK;
    io_dir  = m_ptr->m_lsys_krn_sys_devio.request & _DIO_DIRMASK;

    নির্বাচন (io_type)
    {
	ক্ষেত্র _DIO_BYTE: আকার_2= 1; বিরতি;
	ক্ষেত্র _DIO_WORD: আকার_2= 2; বিরতি;
	ক্ষেত্র _DIO_LONG: আকার_2= 4; বিরতি;
	পূর্বনির্ধারিত: আকার_2= 4; বিরতি;	/* Be conservative */
    }

    privp= priv(caller);
    যদি (!privp)
    {
	printf("no priv structure!\n");
	যাও doit;
    }
    যদি (privp->s_flags & CHECK_IO_PORT)
    {
	port= m_ptr->m_lsys_krn_sys_devio.port;
	nr_io_range= privp->s_nr_io_range;
	জন্য (i= 0, iorp= privp->s_io_tab; i<nr_io_range; i++, iorp++)
	{
		যদি (port >= iorp->ior_base && port+আকার_2-1 <= iorp->ior_limit)
			বিরতি;
	}
	যদি (i >= nr_io_range)
	{
			printf("do_devio: port 0x%x (size %d) not allowed\n",
				m_ptr->m_lsys_krn_sys_devio.port, আকার_2);
		ফেরত EPERM;
	}
    }

doit:
    যদি (m_ptr->m_lsys_krn_sys_devio.port & (আকার_2-1))
    {
		printf("do_devio: unaligned port 0x%x (size %d)\n",
			m_ptr->m_lsys_krn_sys_devio.port, আকার_2);
	ফেরত EPERM;
    }

/* Process a single I/O request for byte, word, and long values. */
    যদি (io_dir == DIO_প্রবেশ) { 
      নির্বাচন (io_type) {
	/* maybe "it" should not be called ports */
        ক্ষেত্র _DIO_BYTE:
		m_ptr->m_krn_lsys_sys_devio.মান =
			inb(m_ptr->m_lsys_krn_sys_devio.port);
		বিরতি;
        ক্ষেত্র _DIO_WORD:
		m_ptr->m_krn_lsys_sys_devio.মান =
			inw(m_ptr->m_lsys_krn_sys_devio.port);
		বিরতি;
        ক্ষেত্র _DIO_LONG:
		m_ptr->m_krn_lsys_sys_devio.মান =
			inl(m_ptr->m_lsys_krn_sys_devio.port);
		বিরতি;
    	পূর্বনির্ধারিত: ফেরত(EINVAL);
      } 
    } অন্যথা { 
      নির্বাচন (io_type) {
	ক্ষেত্র _DIO_BYTE:
		outb(m_ptr->m_lsys_krn_sys_devio.port,
			m_ptr->m_lsys_krn_sys_devio.মান);
		বিরতি;
	ক্ষেত্র _DIO_WORD:
		outw(m_ptr->m_lsys_krn_sys_devio.port,
			m_ptr->m_lsys_krn_sys_devio.মান);
		বিরতি;
	ক্ষেত্র _DIO_LONG:
		outl(m_ptr->m_lsys_krn_sys_devio.port,
			m_ptr->m_lsys_krn_sys_devio.মান);
		বিরতি;
    	পূর্বনির্ধারিত: ফেরত(EINVAL);
      } 
    }
    ফেরত(OK);
}

#endif /* USE_DEVIO */
