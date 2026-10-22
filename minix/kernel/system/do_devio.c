/* The kernel call implemented in this file:
 *   m_type:	SYS_DEVIO
 *
 * The parameters for this kernel call are:
 *   m_lsys_krn_sys_devio.request	(request input or output)
 *   m_lsys_krn_sys_devio.port		(port to read/ write)
 *   m_lsys_krn_sys_devio.value		(value to write/ return value read)
 */

#અન્તર્ભાવ "kernel/system.h"
#અન્તર્ભાવ <minix/devio.h>
#અન્તર્ભાવ <minix/endpoint.h>

#જો USE_DEVIO

/*===========================================================================*
 *			        do_devio                                     *
 *===========================================================================*/
પૂર્ણાંક do_devio(સંરચના proc * caller, message * m_ptr)
{
    સંરચના priv *privp;
    port_t port;
    સંરચના io_range *iorp;
    પૂર્ણાંક i, size, nr_io_range;
    પૂર્ણાંક io_type, io_dir;

    io_type = m_ptr->m_lsys_krn_sys_devio.request & _DIO_TYPEMASK;
    io_dir  = m_ptr->m_lsys_krn_sys_devio.request & _DIO_DIRMASK;

    પસંદગી (io_type)
    {
	સ્થિતિ _DIO_BYTE: size= 1; તોડો;
	સ્થિતિ _DIO_WORD: size= 2; તોડો;
	સ્થિતિ _DIO_LONG: size= 4; તોડો;
	મૂળભૂત: size= 4; તોડો;	/* Be conservative */
    }

    privp= priv(caller);
    જો (!privp)
    {
	printf("no priv structure!\n");
	જાઓ doit;
    }
    જો (privp->s_flags & CHECK_IO_PORT)
    {
	port= m_ptr->m_lsys_krn_sys_devio.port;
	nr_io_range= privp->s_nr_io_range;
	માટે (i= 0, iorp= privp->s_io_tab; i<nr_io_range; i++, iorp++)
	{
		જો (port >= iorp->ior_base && port+size-1 <= iorp->ior_limit)
			તોડો;
	}
	જો (i >= nr_io_range)
	{
			printf("do_devio: port 0x%x (size %d) not allowed\n",
				m_ptr->m_lsys_krn_sys_devio.port, size);
		પાછા EPERM;
	}
    }

doit:
    જો (m_ptr->m_lsys_krn_sys_devio.port & (size-1))
    {
		printf("do_devio: unaligned port 0x%x (size %d)\n",
			m_ptr->m_lsys_krn_sys_devio.port, size);
	પાછા EPERM;
    }

/* Process a single I/O request for byte, word, and long values. */
    જો (io_dir == DIO_આગત) { 
      પસંદગી (io_type) {
	/* maybe "it" should not be called ports */
        સ્થિતિ _DIO_BYTE:
		m_ptr->m_krn_lsys_sys_devio.કિંમત =
			inb(m_ptr->m_lsys_krn_sys_devio.port);
		તોડો;
        સ્થિતિ _DIO_WORD:
		m_ptr->m_krn_lsys_sys_devio.કિંમત =
			inw(m_ptr->m_lsys_krn_sys_devio.port);
		તોડો;
        સ્થિતિ _DIO_LONG:
		m_ptr->m_krn_lsys_sys_devio.કિંમત =
			inl(m_ptr->m_lsys_krn_sys_devio.port);
		તોડો;
    	મૂળભૂત: પાછા(EINVAL);
      } 
    } નહીંતર { 
      પસંદગી (io_type) {
	સ્થિતિ _DIO_BYTE:
		outb(m_ptr->m_lsys_krn_sys_devio.port,
			m_ptr->m_lsys_krn_sys_devio.કિંમત);
		તોડો;
	સ્થિતિ _DIO_WORD:
		outw(m_ptr->m_lsys_krn_sys_devio.port,
			m_ptr->m_lsys_krn_sys_devio.કિંમત);
		તોડો;
	સ્થિતિ _DIO_LONG:
		outl(m_ptr->m_lsys_krn_sys_devio.port,
			m_ptr->m_lsys_krn_sys_devio.કિંમત);
		તોડો;
    	મૂળભૂત: પાછા(EINVAL);
      } 
    }
    પાછા(OK);
}

#endif /* USE_DEVIO */
