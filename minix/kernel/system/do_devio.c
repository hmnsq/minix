/* The kernel call implemented in this file:
 *   m_type:	SYS_DEVIO
 *
 * The parameters for this kernel call are:
 *   m_lsys_krn_sys_devio.request	(request input or output)
 *   m_lsys_krn_sys_devio.port		(port to read/ write)
 *   m_lsys_krn_sys_devio.value		(value to write/ return value read)
 */

#அந்தர்பாவ "kernel/system.h"
#அந்தர்பாவ <minix/devio.h>
#அந்தர்பாவ <minix/endpoint.h>

#எனில் USE_DEVIO

/*===========================================================================*
 *			        do_devio                                     *
 *===========================================================================*/
முழு do_devio(கட்டமைப்பு proc * caller, message * m_ptr)
{
    கட்டமைப்பு priv *privp;
    port_t port;
    கட்டமைப்பு io_range *iorp;
    முழு i, size, nr_io_range;
    முழு io_type, io_dir;

    io_type = m_ptr->m_lsys_krn_sys_devio.request & _DIO_TYPEMASK;
    io_dir  = m_ptr->m_lsys_krn_sys_devio.request & _DIO_DIRMASK;

    தேர்வு (io_type)
    {
	நிலை _DIO_BYTE: size= 1; நிறுத்து;
	நிலை _DIO_WORD: size= 2; நிறுத்து;
	நிலை _DIO_LONG: size= 4; நிறுத்து;
	இயல்பு: size= 4; நிறுத்து;	/* Be conservative */
    }

    privp= priv(caller);
    எனில் (!privp)
    {
	printf("no priv structure!\n");
	செல் doit;
    }
    எனில் (privp->s_flags & CHECK_IO_PORT)
    {
	port= m_ptr->m_lsys_krn_sys_devio.port;
	nr_io_range= privp->s_nr_io_range;
	க்கு (i= 0, iorp= privp->s_io_tab; i<nr_io_range; i++, iorp++)
	{
		எனில் (port >= iorp->ior_base && port+size-1 <= iorp->ior_limit)
			நிறுத்து;
	}
	எனில் (i >= nr_io_range)
	{
			printf("do_devio: port 0x%x (size %d) not allowed\n",
				m_ptr->m_lsys_krn_sys_devio.port, size);
		திரும்பு EPERM;
	}
    }

doit:
    எனில் (m_ptr->m_lsys_krn_sys_devio.port & (size-1))
    {
		printf("do_devio: unaligned port 0x%x (size %d)\n",
			m_ptr->m_lsys_krn_sys_devio.port, size);
	திரும்பு EPERM;
    }

/* Process a single I/O request for byte, word, and long values. */
    எனில் (io_dir == DIO_உள்ளீடு) { 
      தேர்வு (io_type) {
	/* maybe "it" should not be called ports */
        நிலை _DIO_BYTE:
		m_ptr->m_krn_lsys_sys_devio.மதிப்பு =
			inb(m_ptr->m_lsys_krn_sys_devio.port);
		நிறுத்து;
        நிலை _DIO_WORD:
		m_ptr->m_krn_lsys_sys_devio.மதிப்பு =
			inw(m_ptr->m_lsys_krn_sys_devio.port);
		நிறுத்து;
        நிலை _DIO_LONG:
		m_ptr->m_krn_lsys_sys_devio.மதிப்பு =
			inl(m_ptr->m_lsys_krn_sys_devio.port);
		நிறுத்து;
    	இயல்பு: திரும்பு(EINVAL);
      } 
    } இல்லையெனில் { 
      தேர்வு (io_type) {
	நிலை _DIO_BYTE:
		outb(m_ptr->m_lsys_krn_sys_devio.port,
			m_ptr->m_lsys_krn_sys_devio.மதிப்பு);
		நிறுத்து;
	நிலை _DIO_WORD:
		outw(m_ptr->m_lsys_krn_sys_devio.port,
			m_ptr->m_lsys_krn_sys_devio.மதிப்பு);
		நிறுத்து;
	நிலை _DIO_LONG:
		outl(m_ptr->m_lsys_krn_sys_devio.port,
			m_ptr->m_lsys_krn_sys_devio.மதிப்பு);
		நிறுத்து;
    	இயல்பு: திரும்பு(EINVAL);
      } 
    }
    திரும்பு(OK);
}

#endif /* USE_DEVIO */
