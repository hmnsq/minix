/* The kernel call implemented in this file:
 *   m_type:	SYS_DEVIO
 *
 * The parameters for this kernel call are:
 *   m_lsys_krn_sys_devio.request	(request input or output)
 *   m_lsys_krn_sys_devio.port		(port to read/ write)
 *   m_lsys_krn_sys_devio.value		(value to write/ return value read)
 */

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/system.h"
#ਅਨ੍ਤਰ੍ਭਾਵ <minix/devio.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <minix/endpoint.h>

#ਜੇ USE_DEVIO

/*===========================================================================*
 *			        do_devio                                     *
 *===========================================================================*/
ਪੂਰਨ do_devio(ਬਣਤਰ proc * caller, ਸੁਨੇਹਾ * m_ptr)
{
    ਬਣਤਰ priv *privp;
    ਪੋਰਟ_t ਪੋਰਟ;
    ਬਣਤਰ io_range *iorp;
    ਪੂਰਨ i, ਮਾਪ, nr_io_range;
    ਪੂਰਨ io_type, io_dir;

    io_type = m_ptr->m_lsys_krn_sys_devio.request & _DIO_TYPEMASK;
    io_dir  = m_ptr->m_lsys_krn_sys_devio.request & _DIO_DIRMASK;

    ਚੋਣ (io_type)
    {
	ਹਾਲਤ _DIO_BYTE: ਮਾਪ= 1; ਰੋਕੋ;
	ਹਾਲਤ DIO_ਸ਼ਬਦ: ਮਾਪ= 2; ਰੋਕੋ;
	ਹਾਲਤ _DIO_LONG: ਮਾਪ= 4; ਰੋਕੋ;
	ਮੂਲ: ਮਾਪ= 4; ਰੋਕੋ;	/* Be conservative */
    }

    privp= priv(caller);
    ਜੇ (!privp)
    {
	ਲਿਖੋ("no priv structure!\n");
	ਜਾਓ doit;
    }
    ਜੇ (privp->s_flags & CHECK_IO_ਪੋਰਟ)
    {
	ਪੋਰਟ= m_ptr->m_lsys_krn_sys_devio.ਪੋਰਟ;
	nr_io_range= privp->s_nr_io_range;
	ਲਈ (i= 0, iorp= privp->s_io_tab; i<nr_io_range; i++, iorp++)
	{
		ਜੇ (ਪੋਰਟ >= iorp->ior_ਅਧਾਰ && ਪੋਰਟ+ਮਾਪ-1 <= iorp->ior_ਹੱਦਬੰਦੀ)
			ਰੋਕੋ;
	}
	ਜੇ (i >= nr_io_range)
	{
			ਲਿਖੋ("do_devio: port 0x%x (size %d) not allowed\n",
				m_ptr->m_lsys_krn_sys_devio.ਪੋਰਟ, ਮਾਪ);
		ਵਾਪਸ EPERM;
	}
    }

doit:
    ਜੇ (m_ptr->m_lsys_krn_sys_devio.ਪੋਰਟ & (ਮਾਪ-1))
    {
		ਲਿਖੋ("do_devio: unaligned port 0x%x (size %d)\n",
			m_ptr->m_lsys_krn_sys_devio.ਪੋਰਟ, ਮਾਪ);
	ਵਾਪਸ EPERM;
    }

/* Process a single I/O request for byte, word, and long values. */
    ਜੇ (io_dir == DIO_ਨਿਵੇਸ਼) { 
      ਚੋਣ (io_type) {
	/* maybe "it" should not be called ports */
        ਹਾਲਤ _DIO_BYTE:
		m_ptr->m_krn_lsys_sys_devio.ਮੁੱਲ =
			inb(m_ptr->m_lsys_krn_sys_devio.ਪੋਰਟ);
		ਰੋਕੋ;
        ਹਾਲਤ DIO_ਸ਼ਬਦ:
		m_ptr->m_krn_lsys_sys_devio.ਮੁੱਲ =
			inw(m_ptr->m_lsys_krn_sys_devio.ਪੋਰਟ);
		ਰੋਕੋ;
        ਹਾਲਤ _DIO_LONG:
		m_ptr->m_krn_lsys_sys_devio.ਮੁੱਲ =
			inl(m_ptr->m_lsys_krn_sys_devio.ਪੋਰਟ);
		ਰੋਕੋ;
    	ਮੂਲ: ਵਾਪਸ(EINVAL);
      } 
    } ਨਹੀਂਤਾਂ { 
      ਚੋਣ (io_type) {
	ਹਾਲਤ _DIO_BYTE:
		outb(m_ptr->m_lsys_krn_sys_devio.ਪੋਰਟ,
			m_ptr->m_lsys_krn_sys_devio.ਮੁੱਲ);
		ਰੋਕੋ;
	ਹਾਲਤ DIO_ਸ਼ਬਦ:
		outw(m_ptr->m_lsys_krn_sys_devio.ਪੋਰਟ,
			m_ptr->m_lsys_krn_sys_devio.ਮੁੱਲ);
		ਰੋਕੋ;
	ਹਾਲਤ _DIO_LONG:
		outl(m_ptr->m_lsys_krn_sys_devio.ਪੋਰਟ,
			m_ptr->m_lsys_krn_sys_devio.ਮੁੱਲ);
		ਰੋਕੋ;
    	ਮੂਲ: ਵਾਪਸ(EINVAL);
      } 
    }
    ਵਾਪਸ(OK);
}

#ਜੇ_ਅੰਤ /* USE_DEVIO */
