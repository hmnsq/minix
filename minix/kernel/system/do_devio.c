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
صحیح do_devio(ساخت proc * caller, پیغام * m_ptr)
{
    ساخت priv *privp;
    پورٹ_t پورٹ;
    ساخت io_range *iorp;
    صحیح i, سائز, nr_io_range;
    صحیح io_type, io_dir;

    io_type = m_ptr->m_lsys_krn_sys_devio.request & _DIO_TYPEMASK;
    io_dir  = m_ptr->m_lsys_krn_sys_devio.request & _DIO_DIRMASK;

    انتخاب (io_type)
    {
	حالت _DIO_BYTE: سائز= 1; توڑو;
	حالت DIO_لفظ: سائز= 2; توڑو;
	حالت _DIO_LONG: سائز= 4; توڑو;
	طےشدہ: سائز= 4; توڑو;	/* Be conservative */
    }

    privp= priv(caller);
    اگر (!privp)
    {
	لکھو("no priv structure!\n");
	جاؤ doit;
    }
    اگر (privp->s_flags & CHECK_IO_پورٹ)
    {
	پورٹ= m_ptr->m_lsys_krn_sys_devio.پورٹ;
	nr_io_range= privp->s_nr_io_range;
	برائے (i= 0, iorp= privp->s_io_tab; i<nr_io_range; i++, iorp++)
	{
		اگر (پورٹ >= iorp->ior_بنیاد && پورٹ+سائز-1 <= iorp->ior_حد_بندی)
			توڑو;
	}
	اگر (i >= nr_io_range)
	{
			لکھو("do_devio: port 0x%x (size %d) not allowed\n",
				m_ptr->m_lsys_krn_sys_devio.پورٹ, سائز);
		واپس EPERM;
	}
    }

doit:
    اگر (m_ptr->m_lsys_krn_sys_devio.پورٹ & (سائز-1))
    {
		لکھو("do_devio: unaligned port 0x%x (size %d)\n",
			m_ptr->m_lsys_krn_sys_devio.پورٹ, سائز);
	واپس EPERM;
    }

/* Process a single I/O request for byte, word, and long values. */
    اگر (io_dir == DIO_داخلہ) { 
      انتخاب (io_type) {
	/* maybe "it" should not be called ports */
        حالت _DIO_BYTE:
		m_ptr->m_krn_lsys_sys_devio.قدر =
			inb(m_ptr->m_lsys_krn_sys_devio.پورٹ);
		توڑو;
        حالت DIO_لفظ:
		m_ptr->m_krn_lsys_sys_devio.قدر =
			inw(m_ptr->m_lsys_krn_sys_devio.پورٹ);
		توڑو;
        حالت _DIO_LONG:
		m_ptr->m_krn_lsys_sys_devio.قدر =
			inl(m_ptr->m_lsys_krn_sys_devio.پورٹ);
		توڑو;
    	طےشدہ: واپس(EINVAL);
      } 
    } ورنہ { 
      انتخاب (io_type) {
	حالت _DIO_BYTE:
		outb(m_ptr->m_lsys_krn_sys_devio.پورٹ,
			m_ptr->m_lsys_krn_sys_devio.قدر);
		توڑو;
	حالت DIO_لفظ:
		outw(m_ptr->m_lsys_krn_sys_devio.پورٹ,
			m_ptr->m_lsys_krn_sys_devio.قدر);
		توڑو;
	حالت _DIO_LONG:
		outl(m_ptr->m_lsys_krn_sys_devio.پورٹ,
			m_ptr->m_lsys_krn_sys_devio.قدر);
		توڑو;
    	طےشدہ: واپس(EINVAL);
      } 
    }
    واپس(OK);
}

#اگر_ختم /* USE_DEVIO */
