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

#如果 USE_DEVIO

/*===========================================================================*
 *			        do_devio                                     *
 *===========================================================================*/
整数 do_devio(struct proc * caller, message * m_ptr)
{
    struct priv *privp;
    port_t port;
    struct io_range *iorp;
    整数 i, size, nr_io_range;
    整数 io_type, io_dir;

    io_type = m_ptr->m_lsys_krn_sys_devio.request & _DIO_TYPEMASK;
    io_dir  = m_ptr->m_lsys_krn_sys_devio.request & _DIO_DIRMASK;

    switch (io_type)
    {
	case _DIO_BYTE: size= 1; break;
	case _DIO_WORD: size= 2; break;
	case _DIO_LONG: size= 4; break;
	default: size= 4; break;	/* Be conservative */
    }

    privp= priv(caller);
    如果 (!privp)
    {
	printf("no priv structure!\n");
	goto doit;
    }
    如果 (privp->s_flags & CHECK_IO_PORT)
    {
	port= m_ptr->m_lsys_krn_sys_devio.port;
	nr_io_range= privp->s_nr_io_range;
	为 (i= 0, iorp= privp->s_io_tab; i<nr_io_range; i++, iorp++)
	{
		如果 (port >= iorp->ior_base && port+size-1 <= iorp->ior_limit)
			break;
	}
	如果 (i >= nr_io_range)
	{
			printf("do_devio: port 0x%x (size %d) not allowed\n",
				m_ptr->m_lsys_krn_sys_devio.port, size);
		返回 EPERM;
	}
    }

doit:
    如果 (m_ptr->m_lsys_krn_sys_devio.port & (size-1))
    {
		printf("do_devio: unaligned port 0x%x (size %d)\n",
			m_ptr->m_lsys_krn_sys_devio.port, size);
	返回 EPERM;
    }

/* Process a single I/O request for byte, word, and long values. */
    如果 (io_dir == _DIO_INPUT) { 
      switch (io_type) {
	/* maybe "it" should not be called ports */
        case _DIO_BYTE:
		m_ptr->m_krn_lsys_sys_devio.value =
			inb(m_ptr->m_lsys_krn_sys_devio.port);
		break;
        case _DIO_WORD:
		m_ptr->m_krn_lsys_sys_devio.value =
			inw(m_ptr->m_lsys_krn_sys_devio.port);
		break;
        case _DIO_LONG:
		m_ptr->m_krn_lsys_sys_devio.value =
			inl(m_ptr->m_lsys_krn_sys_devio.port);
		break;
    	default: 返回(EINVAL);
      } 
    } 否则 { 
      switch (io_type) {
	case _DIO_BYTE:
		outb(m_ptr->m_lsys_krn_sys_devio.port,
			m_ptr->m_lsys_krn_sys_devio.value);
		break;
	case _DIO_WORD:
		outw(m_ptr->m_lsys_krn_sys_devio.port,
			m_ptr->m_lsys_krn_sys_devio.value);
		break;
	case _DIO_LONG:
		outl(m_ptr->m_lsys_krn_sys_devio.port,
			m_ptr->m_lsys_krn_sys_devio.value);
		break;
    	default: 返回(EINVAL);
      } 
    }
    返回(OK);
}

#endif /* USE_DEVIO */
