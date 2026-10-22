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
صحيح do_devio(بنية proc * caller, پیام * m_ptr)
{
    بنية priv *privp;
    پورت_t پورت;
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
	چاپ_کن("no priv structure!\n");
	اذهب doit;
    }
    إذا (privp->s_flags & CHECK_IO_پورت)
    {
	پورت= m_ptr->m_lsys_krn_sys_devio.پورت;
	nr_io_range= privp->s_nr_io_range;
	لكل (i= 0, iorp= privp->s_io_tab; i<nr_io_range; i++, iorp++)
	{
		إذا (پورت >= iorp->ior_پایه && پورت+اندازه-1 <= iorp->ior_حد)
			اكسر;
	}
	إذا (i >= nr_io_range)
	{
			چاپ_کن("do_devio: port 0x%x (size %d) not allowed\n",
				m_ptr->m_lsys_krn_sys_devio.پورت, اندازه);
		أرجع EPERM;
	}
    }

doit:
    إذا (m_ptr->m_lsys_krn_sys_devio.پورت & (اندازه-1))
    {
		چاپ_کن("do_devio: unaligned port 0x%x (size %d)\n",
			m_ptr->m_lsys_krn_sys_devio.پورت, اندازه);
	أرجع EPERM;
    }

/* Process a single I/O request for byte, word, and long values. */
    إذا (io_dir == DIO_درون_داد) { 
      بدل (io_type) {
	/* maybe "it" should not be called ports */
        حالة _DIO_BYTE:
		m_ptr->m_krn_lsys_sys_devio.مقدار =
			inb(m_ptr->m_lsys_krn_sys_devio.پورت);
		اكسر;
        حالة DIO_کلمه:
		m_ptr->m_krn_lsys_sys_devio.مقدار =
			inw(m_ptr->m_lsys_krn_sys_devio.پورت);
		اكسر;
        حالة _DIO_LONG:
		m_ptr->m_krn_lsys_sys_devio.مقدار =
			inl(m_ptr->m_lsys_krn_sys_devio.پورت);
		اكسر;
    	افتراضي: أرجع(EINVAL);
      } 
    } وإلا { 
      بدل (io_type) {
	حالة _DIO_BYTE:
		outb(m_ptr->m_lsys_krn_sys_devio.پورت,
			m_ptr->m_lsys_krn_sys_devio.مقدار);
		اكسر;
	حالة DIO_کلمه:
		outw(m_ptr->m_lsys_krn_sys_devio.پورت,
			m_ptr->m_lsys_krn_sys_devio.مقدار);
		اكسر;
	حالة _DIO_LONG:
		outl(m_ptr->m_lsys_krn_sys_devio.پورت,
			m_ptr->m_lsys_krn_sys_devio.مقدار);
		اكسر;
    	افتراضي: أرجع(EINVAL);
      } 
    }
    أرجع(OK);
}

#پایان_اگر /* USE_DEVIO */
