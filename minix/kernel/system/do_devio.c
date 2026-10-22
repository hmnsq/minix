/* The kernel call implemented in this file:
 *   m_type:	SYS_DEVIO
 *
 * The parameters for this kernel call are:
 *   m_lsys_krn_sys_devio.request	(request input or output)
 *   m_lsys_krn_sys_devio.port		(port to read/ write)
 *   m_lsys_krn_sys_devio.value		(value to write/ return value read)
 */

#ಅನ್ತರ್ಭಾವ "kernel/system.h"
#ಅನ್ತರ್ಭಾವ <minix/devio.h>
#ಅನ್ತರ್ಭಾವ <minix/endpoint.h>

#ಆದರೆ USE_DEVIO

/*===========================================================================*
 *			        do_devio                                     *
 *===========================================================================*/
ಪೂರ್ಣಾಂಕ do_devio(ರಚನೆ proc * caller, ಸಂದೇಶ * m_ptr)
{
    ರಚನೆ priv *privp;
    ಪೋರ್ಟ್_t ಪೋರ್ಟ್;
    ರಚನೆ io_range *iorp;
    ಪೂರ್ಣಾಂಕ i, ಗಾತ್ರ_2, nr_io_range;
    ಪೂರ್ಣಾಂಕ io_type, io_dir;

    io_type = m_ptr->m_lsys_krn_sys_devio.request & _DIO_TYPEMASK;
    io_dir  = m_ptr->m_lsys_krn_sys_devio.request & _DIO_DIRMASK;

    ಆಯ್ಕೆ (io_type)
    {
	ಸ್ಥಿತಿ _DIO_BYTE: ಗಾತ್ರ_2= 1; ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ DIO_ಶಬ್ದ: ಗಾತ್ರ_2= 2; ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ _DIO_LONG: ಗಾತ್ರ_2= 4; ನಿಲ್ಲಿಸು;
	ಪೂರ್ವನಿಯೋಜಿತ: ಗಾತ್ರ_2= 4; ನಿಲ್ಲಿಸು;	/* Be conservative */
    }

    privp= priv(caller);
    ಆದರೆ (!privp)
    {
	ಬರೆ("no priv structure!\n");
	ಹೋಗು doit;
    }
    ಆದರೆ (privp->s_flags & CHECK_IO_ಪೋರ್ಟ್)
    {
	ಪೋರ್ಟ್= m_ptr->m_lsys_krn_sys_devio.ಪೋರ್ಟ್;
	nr_io_range= privp->s_nr_io_range;
	ಗಾಗಿ (i= 0, iorp= privp->s_io_tab; i<nr_io_range; i++, iorp++)
	{
		ಆದರೆ (ಪೋರ್ಟ್ >= iorp->ior_ತಳ && ಪೋರ್ಟ್+ಗಾತ್ರ_2-1 <= iorp->ior_ಎಲ್ಲೆ)
			ನಿಲ್ಲಿಸು;
	}
	ಆದರೆ (i >= nr_io_range)
	{
			ಬರೆ("do_devio: port 0x%x (size %d) not allowed\n",
				m_ptr->m_lsys_krn_sys_devio.ಪೋರ್ಟ್, ಗಾತ್ರ_2);
		ಹಿಂತಿರುಗಿ EPERM;
	}
    }

doit:
    ಆದರೆ (m_ptr->m_lsys_krn_sys_devio.ಪೋರ್ಟ್ & (ಗಾತ್ರ_2-1))
    {
		ಬರೆ("do_devio: unaligned port 0x%x (size %d)\n",
			m_ptr->m_lsys_krn_sys_devio.ಪೋರ್ಟ್, ಗಾತ್ರ_2);
	ಹಿಂತಿರುಗಿ EPERM;
    }

/* Process a single I/O request for byte, word, and long values. */
    ಆದರೆ (io_dir == DIO_ಒಳಹರಿವು) { 
      ಆಯ್ಕೆ (io_type) {
	/* maybe "it" should not be called ports */
        ಸ್ಥಿತಿ _DIO_BYTE:
		m_ptr->m_krn_lsys_sys_devio.ಮೌಲ್ಯ =
			inb(m_ptr->m_lsys_krn_sys_devio.ಪೋರ್ಟ್);
		ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ DIO_ಶಬ್ದ:
		m_ptr->m_krn_lsys_sys_devio.ಮೌಲ್ಯ =
			inw(m_ptr->m_lsys_krn_sys_devio.ಪೋರ್ಟ್);
		ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ _DIO_LONG:
		m_ptr->m_krn_lsys_sys_devio.ಮೌಲ್ಯ =
			inl(m_ptr->m_lsys_krn_sys_devio.ಪೋರ್ಟ್);
		ನಿಲ್ಲಿಸು;
    	ಪೂರ್ವನಿಯೋಜಿತ: ಹಿಂತಿರುಗಿ(EINVAL);
      } 
    } ಇಲ್ಲದಿದ್ದರೆ { 
      ಆಯ್ಕೆ (io_type) {
	ಸ್ಥಿತಿ _DIO_BYTE:
		outb(m_ptr->m_lsys_krn_sys_devio.ಪೋರ್ಟ್,
			m_ptr->m_lsys_krn_sys_devio.ಮೌಲ್ಯ);
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ DIO_ಶಬ್ದ:
		outw(m_ptr->m_lsys_krn_sys_devio.ಪೋರ್ಟ್,
			m_ptr->m_lsys_krn_sys_devio.ಮೌಲ್ಯ);
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ _DIO_LONG:
		outl(m_ptr->m_lsys_krn_sys_devio.ಪೋರ್ಟ್,
			m_ptr->m_lsys_krn_sys_devio.ಮೌಲ್ಯ);
		ನಿಲ್ಲಿಸು;
    	ಪೂರ್ವನಿಯೋಜಿತ: ಹಿಂತಿರುಗಿ(EINVAL);
      } 
    }
    ಹಿಂತಿರುಗಿ(OK);
}

#ಆದರೆ_ಅಂತ್ಯ /* USE_DEVIO */
