/* The kernel call implemented in this file:
 *   m_type:	SYS_DEVIO
 *
 * The parameters for this kernel call are:
 *   m_lsys_krn_sys_devio.request	(request input or output)
 *   m_lsys_krn_sys_devio.port		(port to read/ write)
 *   m_lsys_krn_sys_devio.value		(value to write/ return value read)
 */

#अन्तर्भाव "kernel/system.h"
#अन्तर्भाव <minix/devio.h>
#अन्तर्भाव <minix/endpoint.h>

#यदि USE_DEVIO

/*===========================================================================*
 *			        do_devio                                     *
 *===========================================================================*/
पूर्णाङ्क do_devio(संरचना proc * caller, सन्देशः * m_ptr)
{
    संरचना priv *privp;
    पत्तनम्_t पत्तनम्;
    संरचना io_range *iorp;
    पूर्णाङ्क i, आकार, nr_io_range;
    पूर्णाङ्क io_type, io_dir;

    io_type = m_ptr->m_lsys_krn_sys_devio.request & _DIO_TYPEMASK;
    io_dir  = m_ptr->m_lsys_krn_sys_devio.request & _DIO_DIRMASK;

    विकल्प (io_type)
    {
	पक्ष _DIO_BYTE: आकार= 1; भङ्ग;
	पक्ष _DIO_WORD: आकार= 2; भङ्ग;
	पक्ष _DIO_LONG: आकार= 4; भङ्ग;
	स्वभाव: आकार= 4; भङ्ग;	/* Be conservative */
    }

    privp= priv(caller);
    यदि (!privp)
    {
	प्रारूप_लिख("no priv structure!\n");
	गच्छ doit;
    }
    यदि (privp->s_flags & CHECK_IO_पत्तनम्)
    {
	पत्तनम्= m_ptr->m_lsys_krn_sys_devio.पत्तनम्;
	nr_io_range= privp->s_nr_io_range;
	प्रति (i= 0, iorp= privp->s_io_tab; i<nr_io_range; i++, iorp++)
	{
		यदि (पत्तनम् >= iorp->ior_आधारः && पत्तनम्+आकार-1 <= iorp->ior_सीमा)
			भङ्ग;
	}
	यदि (i >= nr_io_range)
	{
			प्रारूप_लिख("do_devio: port 0x%x (size %d) not allowed\n",
				m_ptr->m_lsys_krn_sys_devio.पत्तनम्, आकार);
		प्रत्यागम EPERM;
	}
    }

doit:
    यदि (m_ptr->m_lsys_krn_sys_devio.पत्तनम् & (आकार-1))
    {
		प्रारूप_लिख("do_devio: unaligned port 0x%x (size %d)\n",
			m_ptr->m_lsys_krn_sys_devio.पत्तनम्, आकार);
	प्रत्यागम EPERM;
    }

/* Process a single I/O request for byte, word, and long values. */
    यदि (io_dir == DIO_निवेशः) { 
      विकल्प (io_type) {
	/* maybe "it" should not be called ports */
        पक्ष _DIO_BYTE:
		m_ptr->m_krn_lsys_sys_devio.मानम् =
			inb(m_ptr->m_lsys_krn_sys_devio.पत्तनम्);
		भङ्ग;
        पक्ष _DIO_WORD:
		m_ptr->m_krn_lsys_sys_devio.मानम् =
			inw(m_ptr->m_lsys_krn_sys_devio.पत्तनम्);
		भङ्ग;
        पक्ष _DIO_LONG:
		m_ptr->m_krn_lsys_sys_devio.मानम् =
			inl(m_ptr->m_lsys_krn_sys_devio.पत्तनम्);
		भङ्ग;
    	स्वभाव: प्रत्यागम(EINVAL);
      } 
    } अन्यथा { 
      विकल्प (io_type) {
	पक्ष _DIO_BYTE:
		outb(m_ptr->m_lsys_krn_sys_devio.पत्तनम्,
			m_ptr->m_lsys_krn_sys_devio.मानम्);
		भङ्ग;
	पक्ष _DIO_WORD:
		outw(m_ptr->m_lsys_krn_sys_devio.पत्तनम्,
			m_ptr->m_lsys_krn_sys_devio.मानम्);
		भङ्ग;
	पक्ष _DIO_LONG:
		outl(m_ptr->m_lsys_krn_sys_devio.पत्तनम्,
			m_ptr->m_lsys_krn_sys_devio.मानम्);
		भङ्ग;
    	स्वभाव: प्रत्यागम(EINVAL);
      } 
    }
    प्रत्यागम(OK);
}

#यदि_अन्तः /* USE_DEVIO */
