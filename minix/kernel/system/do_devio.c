/* The kernel call implemented in this file:
 *   m_type:	SYS_DEVIO
 *
 * The parameters for this kernel call are:
 *   m_lsys_krn_sys_devio.request	(request input or output)
 *   m_lsys_krn_sys_devio.port		(port to read/ write)
 *   m_lsys_krn_sys_devio.value		(value to write/ return value read)
 */

#അന്തര്ഭാവ "kernel/system.h"
#അന്തര്ഭാവ <minix/devio.h>
#അന്തര്ഭാവ <minix/endpoint.h>

#എങ്കിൽ USE_DEVIO

/*===========================================================================*
 *			        do_devio                                     *
 *===========================================================================*/
പൂർണ്ണസംഖ്യ do_devio(ഘടന proc * caller, message * m_ptr)
{
    ഘടന priv *privp;
    port_t port;
    ഘടന io_range *iorp;
    പൂർണ്ണസംഖ്യ i, size, nr_io_range;
    പൂർണ്ണസംഖ്യ io_type, io_dir;

    io_type = m_ptr->m_lsys_krn_sys_devio.request & _DIO_TYPEMASK;
    io_dir  = m_ptr->m_lsys_krn_sys_devio.request & _DIO_DIRMASK;

    തിരഞ്ഞെടുപ്പ് (io_type)
    {
	അവസ്ഥ _DIO_BYTE: size= 1; നിർത്തുക;
	അവസ്ഥ _DIO_WORD: size= 2; നിർത്തുക;
	അവസ്ഥ _DIO_LONG: size= 4; നിർത്തുക;
	സ്ഥിരസ്ഥിതി: size= 4; നിർത്തുക;	/* Be conservative */
    }

    privp= priv(caller);
    എങ്കിൽ (!privp)
    {
	printf("no priv structure!\n");
	പോകുക doit;
    }
    എങ്കിൽ (privp->s_flags & CHECK_IO_PORT)
    {
	port= m_ptr->m_lsys_krn_sys_devio.port;
	nr_io_range= privp->s_nr_io_range;
	വേണ്ടി (i= 0, iorp= privp->s_io_tab; i<nr_io_range; i++, iorp++)
	{
		എങ്കിൽ (port >= iorp->ior_base && port+size-1 <= iorp->ior_limit)
			നിർത്തുക;
	}
	എങ്കിൽ (i >= nr_io_range)
	{
			printf("do_devio: port 0x%x (size %d) not allowed\n",
				m_ptr->m_lsys_krn_sys_devio.port, size);
		തിരികെ EPERM;
	}
    }

doit:
    എങ്കിൽ (m_ptr->m_lsys_krn_sys_devio.port & (size-1))
    {
		printf("do_devio: unaligned port 0x%x (size %d)\n",
			m_ptr->m_lsys_krn_sys_devio.port, size);
	തിരികെ EPERM;
    }

/* Process a single I/O request for byte, word, and long values. */
    എങ്കിൽ (io_dir == DIO_നിവേശം) { 
      തിരഞ്ഞെടുപ്പ് (io_type) {
	/* maybe "it" should not be called ports */
        അവസ്ഥ _DIO_BYTE:
		m_ptr->m_krn_lsys_sys_devio.മൂല്യം =
			inb(m_ptr->m_lsys_krn_sys_devio.port);
		നിർത്തുക;
        അവസ്ഥ _DIO_WORD:
		m_ptr->m_krn_lsys_sys_devio.മൂല്യം =
			inw(m_ptr->m_lsys_krn_sys_devio.port);
		നിർത്തുക;
        അവസ്ഥ _DIO_LONG:
		m_ptr->m_krn_lsys_sys_devio.മൂല്യം =
			inl(m_ptr->m_lsys_krn_sys_devio.port);
		നിർത്തുക;
    	സ്ഥിരസ്ഥിതി: തിരികെ(EINVAL);
      } 
    } അല്ലെങ്കിൽ { 
      തിരഞ്ഞെടുപ്പ് (io_type) {
	അവസ്ഥ _DIO_BYTE:
		outb(m_ptr->m_lsys_krn_sys_devio.port,
			m_ptr->m_lsys_krn_sys_devio.മൂല്യം);
		നിർത്തുക;
	അവസ്ഥ _DIO_WORD:
		outw(m_ptr->m_lsys_krn_sys_devio.port,
			m_ptr->m_lsys_krn_sys_devio.മൂല്യം);
		നിർത്തുക;
	അവസ്ഥ _DIO_LONG:
		outl(m_ptr->m_lsys_krn_sys_devio.port,
			m_ptr->m_lsys_krn_sys_devio.മൂല്യം);
		നിർത്തുക;
    	സ്ഥിരസ്ഥിതി: തിരികെ(EINVAL);
      } 
    }
    തിരികെ(OK);
}

#endif /* USE_DEVIO */
