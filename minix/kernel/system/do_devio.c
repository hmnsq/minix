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
صحيح do_devio(بنية proc * caller, پیغام * m_ptr)
{
    بنية priv *privp;
    پورټ_t پورټ;
    بنية io_range *iorp;
    صحيح i, اندازه, nr_io_range;
    صحيح io_type, io_dir;

    io_type = m_ptr->m_lsys_krn_sys_devio.request & _DIO_TYPEMASK;
    io_dir  = m_ptr->m_lsys_krn_sys_devio.request & _DIO_DIRMASK;

    بدل (io_type)
    {
	حالة _DIO_BYTE: اندازه= 1; اكسر;
	حالة DIO_کلمه: اندازه= 2; اكسر;
	حالة _DIO_LONG: اندازه= 4; اكسر;
	افتراضي: اندازه= 4; اكسر;	/* Be conservative */
    }

    privp= priv(caller);
    إذا (!privp)
    {
	چاپ_کړه("no priv structure!\n");
	اذهب doit;
    }
    إذا (privp->s_flags & CHECK_IO_پورټ)
    {
	پورټ= m_ptr->m_lsys_krn_sys_devio.پورټ;
	nr_io_range= privp->s_nr_io_range;
	لكل (i= 0, iorp= privp->s_io_tab; i<nr_io_range; i++, iorp++)
	{
		إذا (پورټ >= iorp->ior_بنسټ && پورټ+اندازه-1 <= iorp->ior_حد)
			اكسر;
	}
	إذا (i >= nr_io_range)
	{
			چاپ_کړه("do_devio: port 0x%x (size %d) not allowed\n",
				m_ptr->m_lsys_krn_sys_devio.پورټ, اندازه);
		أرجع EPERM;
	}
    }

doit:
    إذا (m_ptr->m_lsys_krn_sys_devio.پورټ & (اندازه-1))
    {
		چاپ_کړه("do_devio: unaligned port 0x%x (size %d)\n",
			m_ptr->m_lsys_krn_sys_devio.پورټ, اندازه);
	أرجع EPERM;
    }

/* Process a single I/O request for byte, word, and long values. */
    إذا (io_dir == DIO_ننوت) { 
      بدل (io_type) {
	/* maybe "it" should not be called ports */
        حالة _DIO_BYTE:
		m_ptr->m_krn_lsys_sys_devio.ارزښت =
			inb(m_ptr->m_lsys_krn_sys_devio.پورټ);
		اكسر;
        حالة DIO_کلمه:
		m_ptr->m_krn_lsys_sys_devio.ارزښت =
			inw(m_ptr->m_lsys_krn_sys_devio.پورټ);
		اكسر;
        حالة _DIO_LONG:
		m_ptr->m_krn_lsys_sys_devio.ارزښت =
			inl(m_ptr->m_lsys_krn_sys_devio.پورټ);
		اكسر;
    	افتراضي: أرجع(EINVAL);
      } 
    } وإلا { 
      بدل (io_type) {
	حالة _DIO_BYTE:
		outb(m_ptr->m_lsys_krn_sys_devio.پورټ,
			m_ptr->m_lsys_krn_sys_devio.ارزښت);
		اكسر;
	حالة DIO_کلمه:
		outw(m_ptr->m_lsys_krn_sys_devio.پورټ,
			m_ptr->m_lsys_krn_sys_devio.ارزښت);
		اكسر;
	حالة _DIO_LONG:
		outl(m_ptr->m_lsys_krn_sys_devio.پورټ,
			m_ptr->m_lsys_krn_sys_devio.ارزښت);
		اكسر;
    	افتراضي: أرجع(EINVAL);
      } 
    }
    أرجع(OK);
}

#د_که_پای /* USE_DEVIO */
