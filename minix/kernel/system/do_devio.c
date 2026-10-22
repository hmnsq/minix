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
صحيح do_devio(بنية proc * caller, سنیہا * m_ptr)
{
    بنية priv *privp;
    پورٹ_t پورٹ;
    بنية io_range *iorp;
    صحيح i, ناپ, nr_io_range;
    صحيح io_type, io_dir;

    io_type = m_ptr->m_lsys_krn_sys_devio.request & _DIO_TYPEMASK;
    io_dir  = m_ptr->m_lsys_krn_sys_devio.request & _DIO_DIRMASK;

    بدل (io_type)
    {
	حالة _DIO_BYTE: ناپ= 1; اكسر;
	حالة DIO_لفظ: ناپ= 2; اكسر;
	حالة _DIO_LONG: ناپ= 4; اكسر;
	افتراضي: ناپ= 4; اكسر;	/* Be conservative */
    }

    privp= priv(caller);
    إذا (!privp)
    {
	لکھو("no priv structure!\n");
	اذهب doit;
    }
    إذا (privp->s_flags & CHECK_IO_پورٹ)
    {
	پورٹ= m_ptr->m_lsys_krn_sys_devio.پورٹ;
	nr_io_range= privp->s_nr_io_range;
	لكل (i= 0, iorp= privp->s_io_tab; i<nr_io_range; i++, iorp++)
	{
		إذا (پورٹ >= iorp->ior_ادھار && پورٹ+ناپ-1 <= iorp->ior_حد_بندی)
			اكسر;
	}
	إذا (i >= nr_io_range)
	{
			لکھو("do_devio: port 0x%x (size %d) not allowed\n",
				m_ptr->m_lsys_krn_sys_devio.پورٹ, ناپ);
		أرجع EPERM;
	}
    }

doit:
    إذا (m_ptr->m_lsys_krn_sys_devio.پورٹ & (ناپ-1))
    {
		لکھو("do_devio: unaligned port 0x%x (size %d)\n",
			m_ptr->m_lsys_krn_sys_devio.پورٹ, ناپ);
	أرجع EPERM;
    }

/* Process a single I/O request for byte, word, and long values. */
    إذا (io_dir == DIO_داخلہ) { 
      بدل (io_type) {
	/* maybe "it" should not be called ports */
        حالة _DIO_BYTE:
		m_ptr->m_krn_lsys_sys_devio.قدر =
			inb(m_ptr->m_lsys_krn_sys_devio.پورٹ);
		اكسر;
        حالة DIO_لفظ:
		m_ptr->m_krn_lsys_sys_devio.قدر =
			inw(m_ptr->m_lsys_krn_sys_devio.پورٹ);
		اكسر;
        حالة _DIO_LONG:
		m_ptr->m_krn_lsys_sys_devio.قدر =
			inl(m_ptr->m_lsys_krn_sys_devio.پورٹ);
		اكسر;
    	افتراضي: أرجع(EINVAL);
      } 
    } وإلا { 
      بدل (io_type) {
	حالة _DIO_BYTE:
		outb(m_ptr->m_lsys_krn_sys_devio.پورٹ,
			m_ptr->m_lsys_krn_sys_devio.قدر);
		اكسر;
	حالة DIO_لفظ:
		outw(m_ptr->m_lsys_krn_sys_devio.پورٹ,
			m_ptr->m_lsys_krn_sys_devio.قدر);
		اكسر;
	حالة _DIO_LONG:
		outl(m_ptr->m_lsys_krn_sys_devio.پورٹ,
			m_ptr->m_lsys_krn_sys_devio.قدر);
		اكسر;
    	افتراضي: أرجع(EINVAL);
      } 
    }
    أرجع(OK);
}

#جے_انت /* USE_DEVIO */
