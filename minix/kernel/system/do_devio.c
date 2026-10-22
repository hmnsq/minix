/* The kernel call implemented in this file:
 *   m_type:	SYS_DEVIO
 *
 * The parameters for this kernel call are:
 *   m_lsys_krn_sys_devio.request	(request input or output)
 *   m_lsys_krn_sys_devio.port		(port to read/ write)
 *   m_lsys_krn_sys_devio.value		(value to write/ return value read)
 */

#شامل "kernel/system.h"
#شامل <minix/devio.h>
#شامل <minix/endpoint.h>

#اگر USE_DEVIO

/*===========================================================================*
 *			        do_devio                                     *
 *===========================================================================*/
صحیح do_devio(ساختار proc * caller, message * m_ptr)
{
    ساختار priv *privp;
    port_t port;
    ساختار io_range *iorp;
    صحیح i, size, nr_io_range;
    صحیح io_type, io_dir;

    io_type = m_ptr->m_lsys_krn_sys_devio.request & _DIO_TYPEMASK;
    io_dir  = m_ptr->m_lsys_krn_sys_devio.request & _DIO_DIRMASK;

    انتخاب (io_type)
    {
	حالت _DIO_BYTE: size= 1; بشکن;
	حالت _DIO_WORD: size= 2; بشکن;
	حالت _DIO_LONG: size= 4; بشکن;
	پیشفرض: size= 4; بشکن;	/* Be conservative */
    }

    privp= priv(caller);
    اگر (!privp)
    {
	printf("no priv structure!\n");
	برو doit;
    }
    اگر (privp->s_flags & CHECK_IO_PORT)
    {
	port= m_ptr->m_lsys_krn_sys_devio.port;
	nr_io_range= privp->s_nr_io_range;
	برای (i= 0, iorp= privp->s_io_tab; i<nr_io_range; i++, iorp++)
	{
		اگر (port >= iorp->ior_base && port+size-1 <= iorp->ior_limit)
			بشکن;
	}
	اگر (i >= nr_io_range)
	{
			printf("do_devio: port 0x%x (size %d) not allowed\n",
				m_ptr->m_lsys_krn_sys_devio.port, size);
		بازگشت EPERM;
	}
    }

doit:
    اگر (m_ptr->m_lsys_krn_sys_devio.port & (size-1))
    {
		printf("do_devio: unaligned port 0x%x (size %d)\n",
			m_ptr->m_lsys_krn_sys_devio.port, size);
	بازگشت EPERM;
    }

/* Process a single I/O request for byte, word, and long values. */
    اگر (io_dir == DIO_درون_داد) { 
      انتخاب (io_type) {
	/* maybe "it" should not be called ports */
        حالت _DIO_BYTE:
		m_ptr->m_krn_lsys_sys_devio.مقدار =
			inb(m_ptr->m_lsys_krn_sys_devio.port);
		بشکن;
        حالت _DIO_WORD:
		m_ptr->m_krn_lsys_sys_devio.مقدار =
			inw(m_ptr->m_lsys_krn_sys_devio.port);
		بشکن;
        حالت _DIO_LONG:
		m_ptr->m_krn_lsys_sys_devio.مقدار =
			inl(m_ptr->m_lsys_krn_sys_devio.port);
		بشکن;
    	پیشفرض: بازگشت(EINVAL);
      } 
    } وگرنه { 
      انتخاب (io_type) {
	حالت _DIO_BYTE:
		outb(m_ptr->m_lsys_krn_sys_devio.port,
			m_ptr->m_lsys_krn_sys_devio.مقدار);
		بشکن;
	حالت _DIO_WORD:
		outw(m_ptr->m_lsys_krn_sys_devio.port,
			m_ptr->m_lsys_krn_sys_devio.مقدار);
		بشکن;
	حالت _DIO_LONG:
		outl(m_ptr->m_lsys_krn_sys_devio.port,
			m_ptr->m_lsys_krn_sys_devio.مقدار);
		بشکن;
    	پیشفرض: بازگشت(EINVAL);
      } 
    }
    بازگشت(OK);
}

#endif /* USE_DEVIO */
