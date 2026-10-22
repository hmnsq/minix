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

#إذا USE_DEVIO

/*===========================================================================*
 *			        do_devio                                     *
 *===========================================================================*/
صحيح do_devio(بنية proc * caller, پيغام * m_ptr)
{
    بنية priv *privp;
    پورٽ_t پورٽ;
    بنية io_range *iorp;
    صحيح i, سائيز, nr_io_range;
    صحيح io_type, io_dir;

    io_type = m_ptr->m_lsys_krn_sys_devio.request & _DIO_TYPEMASK;
    io_dir  = m_ptr->m_lsys_krn_sys_devio.request & _DIO_DIRMASK;

    بدل (io_type)
    {
	حالة _DIO_BYTE: سائيز= 1; اكسر;
	حالة DIO_لفظ: سائيز= 2; اكسر;
	حالة _DIO_LONG: سائيز= 4; اكسر;
	افتراضي: سائيز= 4; اكسر;	/* Be conservative */
    }

    privp= priv(caller);
    إذا (!privp)
    {
	لکو("no priv structure!\n");
	اذهب doit;
    }
    إذا (privp->s_flags & CHECK_IO_پورٽ)
    {
	پورٽ= m_ptr->m_lsys_krn_sys_devio.پورٽ;
	nr_io_range= privp->s_nr_io_range;
	لكل (i= 0, iorp= privp->s_io_tab; i<nr_io_range; i++, iorp++)
	{
		إذا (پورٽ >= iorp->ior_بنياد && پورٽ+سائيز-1 <= iorp->ior_حد_بندي)
			اكسر;
	}
	إذا (i >= nr_io_range)
	{
			لکو("do_devio: port 0x%x (size %d) not allowed\n",
				m_ptr->m_lsys_krn_sys_devio.پورٽ, سائيز);
		أرجع EPERM;
	}
    }

doit:
    إذا (m_ptr->m_lsys_krn_sys_devio.پورٽ & (سائيز-1))
    {
		لکو("do_devio: unaligned port 0x%x (size %d)\n",
			m_ptr->m_lsys_krn_sys_devio.پورٽ, سائيز);
	أرجع EPERM;
    }

/* Process a single I/O request for byte, word, and long values. */
    إذا (io_dir == DIO_داخلا) { 
      بدل (io_type) {
	/* maybe "it" should not be called ports */
        حالة _DIO_BYTE:
		m_ptr->m_krn_lsys_sys_devio.قدر =
			inb(m_ptr->m_lsys_krn_sys_devio.پورٽ);
		اكسر;
        حالة DIO_لفظ:
		m_ptr->m_krn_lsys_sys_devio.قدر =
			inw(m_ptr->m_lsys_krn_sys_devio.پورٽ);
		اكسر;
        حالة _DIO_LONG:
		m_ptr->m_krn_lsys_sys_devio.قدر =
			inl(m_ptr->m_lsys_krn_sys_devio.پورٽ);
		اكسر;
    	افتراضي: أرجع(EINVAL);
      } 
    } وإلا { 
      بدل (io_type) {
	حالة _DIO_BYTE:
		outb(m_ptr->m_lsys_krn_sys_devio.پورٽ,
			m_ptr->m_lsys_krn_sys_devio.قدر);
		اكسر;
	حالة DIO_لفظ:
		outw(m_ptr->m_lsys_krn_sys_devio.پورٽ,
			m_ptr->m_lsys_krn_sys_devio.قدر);
		اكسر;
	حالة _DIO_LONG:
		outl(m_ptr->m_lsys_krn_sys_devio.پورٽ,
			m_ptr->m_lsys_krn_sys_devio.قدر);
		اكسر;
    	افتراضي: أرجع(EINVAL);
      } 
    }
    أرجع(OK);
}

#جيڪڏهن_پورو /* USE_DEVIO */
