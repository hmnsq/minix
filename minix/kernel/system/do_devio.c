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

#ຖ້າ USE_DEVIO

/*===========================================================================*
 *			        do_devio                                     *
 *===========================================================================*/
ຈຳນວນເຕັມ do_devio(sa_nracanaa proc * caller, message * m_ptr)
{
    sa_nracanaa priv *privp;
    port_t port;
    sa_nracanaa io_range *iorp;
    ຈຳນວນເຕັມ i, size, nr_io_range;
    ຈຳນວນເຕັມ io_type, io_dir;

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
    ຖ້າ (!privp)
    {
	printf("no priv structure!\n");
	goto doit;
    }
    ຖ້າ (privp->s_flags & CHECK_IO_PORT)
    {
	port= m_ptr->m_lsys_krn_sys_devio.port;
	nr_io_range= privp->s_nr_io_range;
	ສຳລັບ (i= 0, iorp= privp->s_io_tab; i<nr_io_range; i++, iorp++)
	{
		ຖ້າ (port >= iorp->ior_base && port+size-1 <= iorp->ior_limit)
			bha_nga;
	}
	ຖ້າ (i >= nr_io_range)
	{
			printf("do_devio: port 0x%x (size %d) not allowed\n",
				m_ptr->m_lsys_krn_sys_devio.port, size);
		ສົ່ງຄືນ EPERM;
	}
    }

doit:
    ຖ້າ (m_ptr->m_lsys_krn_sys_devio.port & (size-1))
    {
		printf("do_devio: unaligned port 0x%x (size %d)\n",
			m_ptr->m_lsys_krn_sys_devio.port, size);
	ສົ່ງຄືນ EPERM;
    }

/* Process a single I/O request for byte, word, and long values. */
    ຖ້າ (io_dir == _DIO_INPUT) { 
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
    	default: ສົ່ງຄືນ(EINVAL);
      } 
    } ບໍ່ດັ່ງນັ້ນ { 
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
    	default: ສົ່ງຄືນ(EINVAL);
      } 
    }
    ສົ່ງຄືນ(OK);
}

#endif /* USE_DEVIO */
