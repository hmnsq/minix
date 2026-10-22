/* The kernel call implemented in this file:
 *   m_type:	SYS_DEVIO
 *
 * The parameters for this kernel call are:
 *   m_lsys_krn_sys_devio.request	(request input or output)
 *   m_lsys_krn_sys_devio.port		(port to read/ write)
 *   m_lsys_krn_sys_devio.value		(value to write/ return value read)
 */

#include "kernel/system.h"
#include <minix/devio.h>
#include <minix/endpoint.h>

#ᤀᤡ᤺ USE_DEVIO

/*===========================================================================*
 *			        do_devio                                     *
 *===========================================================================*/
puur_naa_nka do_devio(sa_nracanaa proc * caller, message * m_ptr)
{
    sa_nracanaa priv *privp;
    port_t port;
    sa_nracanaa io_range *iorp;
    puur_naa_nka i, size, nr_io_range;
    puur_naa_nka io_type, io_dir;

    io_type = m_ptr->m_lsys_krn_sys_devio.request & _DIO_TYPEMASK;
    io_dir  = m_ptr->m_lsys_krn_sys_devio.request & _DIO_DIRMASK;

    switch (io_type)
    {
	pak_sa _DIO_BYTE: size= 1; bha_nga;
	pak_sa _DIO_WORD: size= 2; bha_nga;
	pak_sa _DIO_LONG: size= 4; bha_nga;
	default: size= 4; bha_nga;	/* Be conservative */
    }

    privp= priv(caller);
    ᤀᤡ᤺ (!privp)
    {
	printf("no priv structure!\n");
	goto doit;
    }
    ᤀᤡ᤺ (privp->s_flags & CHECK_IO_PORT)
    {
	port= m_ptr->m_lsys_krn_sys_devio.port;
	nr_io_range= privp->s_nr_io_range;
	for (i= 0, iorp= privp->s_io_tab; i<nr_io_range; i++, iorp++)
	{
		ᤀᤡ᤺ (port >= iorp->ior_base && port+size-1 <= iorp->ior_limit)
			bha_nga;
	}
	ᤀᤡ᤺ (i >= nr_io_range)
	{
			printf("do_devio: port 0x%x (size %d) not allowed\n",
				m_ptr->m_lsys_krn_sys_devio.port, size);
		ᤀᤡᤴ EPERM;
	}
    }

doit:
    ᤀᤡ᤺ (m_ptr->m_lsys_krn_sys_devio.port & (size-1))
    {
		printf("do_devio: unaligned port 0x%x (size %d)\n",
			m_ptr->m_lsys_krn_sys_devio.port, size);
	ᤀᤡᤴ EPERM;
    }

/* Process a single I/O request for byte, word, and long values. */
    ᤀᤡ᤺ (io_dir == _DIO_INPUT) { 
      switch (io_type) {
	/* maybe "it" should not be called ports */
        pak_sa _DIO_BYTE:
		m_ptr->m_krn_lsys_sys_devio.value =
			inb(m_ptr->m_lsys_krn_sys_devio.port);
		bha_nga;
        pak_sa _DIO_WORD:
		m_ptr->m_krn_lsys_sys_devio.value =
			inw(m_ptr->m_lsys_krn_sys_devio.port);
		bha_nga;
        pak_sa _DIO_LONG:
		m_ptr->m_krn_lsys_sys_devio.value =
			inl(m_ptr->m_lsys_krn_sys_devio.port);
		bha_nga;
    	default: ᤀᤡᤴ(EINVAL);
      } 
    } ᤀᤡ᤺ᤆᤡ { 
      switch (io_type) {
	pak_sa _DIO_BYTE:
		outb(m_ptr->m_lsys_krn_sys_devio.port,
			m_ptr->m_lsys_krn_sys_devio.value);
		bha_nga;
	pak_sa _DIO_WORD:
		outw(m_ptr->m_lsys_krn_sys_devio.port,
			m_ptr->m_lsys_krn_sys_devio.value);
		bha_nga;
	pak_sa _DIO_LONG:
		outl(m_ptr->m_lsys_krn_sys_devio.port,
			m_ptr->m_lsys_krn_sys_devio.value);
		bha_nga;
    	default: ᤀᤡᤴ(EINVAL);
      } 
    }
    ᤀᤡᤴ(OK);
}

#endif /* USE_DEVIO */
