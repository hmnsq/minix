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

#ಯದಿ USE_DEVIO

/*===========================================================================*
 *			        do_devio                                     *
 *===========================================================================*/
ಪೂರ್ಣಾಙ್ಕ do_devio(ಸಂರಚನಾ proc * caller, message * m_ptr)
{
    ಸಂರಚನಾ priv *privp;
    port_t port;
    ಸಂರಚನಾ io_range *iorp;
    ಪೂರ್ಣಾಙ್ಕ i, size, nr_io_range;
    ಪೂರ್ಣಾಙ್ಕ io_type, io_dir;

    io_type = m_ptr->m_lsys_krn_sys_devio.request & _DIO_TYPEMASK;
    io_dir  = m_ptr->m_lsys_krn_sys_devio.request & _DIO_DIRMASK;

    ವಿಕಲ್ಪ (io_type)
    {
	ಪಕ್ಷ _DIO_BYTE: size= 1; ಭಙ್ಗ;
	ಪಕ್ಷ _DIO_WORD: size= 2; ಭಙ್ಗ;
	ಪಕ್ಷ _DIO_LONG: size= 4; ಭಙ್ಗ;
	ಸ್ವಭಾವ: size= 4; ಭಙ್ಗ;	/* Be conservative */
    }

    privp= priv(caller);
    ಯದಿ (!privp)
    {
	printf("no priv structure!\n");
	ಗಚ್ಛ doit;
    }
    ಯದಿ (privp->s_flags & CHECK_IO_PORT)
    {
	port= m_ptr->m_lsys_krn_sys_devio.port;
	nr_io_range= privp->s_nr_io_range;
	ಪ್ರತಿ (i= 0, iorp= privp->s_io_tab; i<nr_io_range; i++, iorp++)
	{
		ಯದಿ (port >= iorp->ior_base && port+size-1 <= iorp->ior_limit)
			ಭಙ್ಗ;
	}
	ಯದಿ (i >= nr_io_range)
	{
			printf("do_devio: port 0x%x (size %d) not allowed\n",
				m_ptr->m_lsys_krn_sys_devio.port, size);
		ಪ್ರತ್ಯಾಗಮ EPERM;
	}
    }

doit:
    ಯದಿ (m_ptr->m_lsys_krn_sys_devio.port & (size-1))
    {
		printf("do_devio: unaligned port 0x%x (size %d)\n",
			m_ptr->m_lsys_krn_sys_devio.port, size);
	ಪ್ರತ್ಯಾಗಮ EPERM;
    }

/* Process a single I/O request for byte, word, and long values. */
    ಯದಿ (io_dir == DIO_ಒಳಹರಿವು) { 
      ವಿಕಲ್ಪ (io_type) {
	/* maybe "it" should not be called ports */
        ಪಕ್ಷ _DIO_BYTE:
		m_ptr->m_krn_lsys_sys_devio.ಮೌಲ್ಯ =
			inb(m_ptr->m_lsys_krn_sys_devio.port);
		ಭಙ್ಗ;
        ಪಕ್ಷ _DIO_WORD:
		m_ptr->m_krn_lsys_sys_devio.ಮೌಲ್ಯ =
			inw(m_ptr->m_lsys_krn_sys_devio.port);
		ಭಙ್ಗ;
        ಪಕ್ಷ _DIO_LONG:
		m_ptr->m_krn_lsys_sys_devio.ಮೌಲ್ಯ =
			inl(m_ptr->m_lsys_krn_sys_devio.port);
		ಭಙ್ಗ;
    	ಸ್ವಭಾವ: ಪ್ರತ್ಯಾಗಮ(EINVAL);
      } 
    } ಅನ್ಯಥಾ { 
      ವಿಕಲ್ಪ (io_type) {
	ಪಕ್ಷ _DIO_BYTE:
		outb(m_ptr->m_lsys_krn_sys_devio.port,
			m_ptr->m_lsys_krn_sys_devio.ಮೌಲ್ಯ);
		ಭಙ್ಗ;
	ಪಕ್ಷ _DIO_WORD:
		outw(m_ptr->m_lsys_krn_sys_devio.port,
			m_ptr->m_lsys_krn_sys_devio.ಮೌಲ್ಯ);
		ಭಙ್ಗ;
	ಪಕ್ಷ _DIO_LONG:
		outl(m_ptr->m_lsys_krn_sys_devio.port,
			m_ptr->m_lsys_krn_sys_devio.ಮೌಲ್ಯ);
		ಭಙ್ಗ;
    	ಸ್ವಭಾವ: ಪ್ರತ್ಯಾಗಮ(EINVAL);
      } 
    }
    ಪ್ರತ್ಯಾಗಮ(OK);
}

#endif /* USE_DEVIO */
