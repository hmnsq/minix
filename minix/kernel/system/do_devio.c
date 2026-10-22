/* The kernel call implemented in this file:
 *   m_type:	SYS_DEVIO
 *
 * The parameters for this kernel call are:
 *   m_lsys_krn_sys_devio.request	(request input or output)
 *   m_lsys_krn_sys_devio.port		(port to read/ write)
 *   m_lsys_krn_sys_devio.value		(value to write/ return value read)
 */

#ضمن "kernel/system.h"
#ضمن <minix/devio.h>
#ضمن <minix/endpoint.h>

#אם USE_DEVIO

/*===========================================================================*
 *			        do_devio                                     *
 *===========================================================================*/
صحيح do_devio(بنية proc * caller, message * m_ptr)
{
    بنية priv *privp;
    port_t port;
    بنية io_range *iorp;
    صحيح i, size, nr_io_range;
    صحيح io_type, io_dir;

    io_type = m_ptr->m_lsys_krn_sys_devio.request & _DIO_TYPEMASK;
    io_dir  = m_ptr->m_lsys_krn_sys_devio.request & _DIO_DIRMASK;

    بدل (io_type)
    {
	حالة _DIO_BYTE: size= 1; اكسر;
	حالة _DIO_WORD: size= 2; اكسر;
	حالة _DIO_LONG: size= 4; اكسر;
	افتراضي: size= 4; اكسر;	/* Be conservative */
    }

    privp= priv(caller);
    אם (!privp)
    {
	printf("no priv structure!\n");
	اذهب doit;
    }
    אם (privp->s_flags & CHECK_IO_PORT)
    {
	port= m_ptr->m_lsys_krn_sys_devio.port;
	nr_io_range= privp->s_nr_io_range;
	עבור (i= 0, iorp= privp->s_io_tab; i<nr_io_range; i++, iorp++)
	{
		אם (port >= iorp->ior_base && port+size-1 <= iorp->ior_limit)
			اكسر;
	}
	אם (i >= nr_io_range)
	{
			printf("do_devio: port 0x%x (size %d) not allowed\n",
				m_ptr->m_lsys_krn_sys_devio.port, size);
		החזר EPERM;
	}
    }

doit:
    אם (m_ptr->m_lsys_krn_sys_devio.port & (size-1))
    {
		printf("do_devio: unaligned port 0x%x (size %d)\n",
			m_ptr->m_lsys_krn_sys_devio.port, size);
	החזר EPERM;
    }

/* Process a single I/O request for byte, word, and long values. */
    אם (io_dir == _DIO_INPUT) { 
      بدل (io_type) {
	/* maybe "it" should not be called ports */
        حالة _DIO_BYTE:
		m_ptr->m_krn_lsys_sys_devio.value =
			inb(m_ptr->m_lsys_krn_sys_devio.port);
		اكسر;
        حالة _DIO_WORD:
		m_ptr->m_krn_lsys_sys_devio.value =
			inw(m_ptr->m_lsys_krn_sys_devio.port);
		اكسر;
        حالة _DIO_LONG:
		m_ptr->m_krn_lsys_sys_devio.value =
			inl(m_ptr->m_lsys_krn_sys_devio.port);
		اكسر;
    	افتراضي: החזר(EINVAL);
      } 
    } אחרת { 
      بدل (io_type) {
	حالة _DIO_BYTE:
		outb(m_ptr->m_lsys_krn_sys_devio.port,
			m_ptr->m_lsys_krn_sys_devio.value);
		اكسر;
	حالة _DIO_WORD:
		outw(m_ptr->m_lsys_krn_sys_devio.port,
			m_ptr->m_lsys_krn_sys_devio.value);
		اكسر;
	حالة _DIO_LONG:
		outl(m_ptr->m_lsys_krn_sys_devio.port,
			m_ptr->m_lsys_krn_sys_devio.value);
		اكسر;
    	افتراضي: החזר(EINVAL);
      } 
    }
    החזר(OK);
}

#endif /* USE_DEVIO */
