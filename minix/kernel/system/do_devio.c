/* The kernel call implemented in this file:
 *   m_type:	SYS_DEVIO
 *
 * The parameters for this kernel call are:
 *   m_lsys_krn_sys_devio.request	(request input or output)
 *   m_lsys_krn_sys_devio.port		(port to read/ write)
 *   m_lsys_krn_sys_devio.value		(value to write/ return value read)
 */

#అన్తర్భావ "kernel/system.h"
#అన్తర్భావ <minix/devio.h>
#అన్తర్భావ <minix/endpoint.h>

#ఒకవేళ USE_DEVIO

/*===========================================================================*
 *			        do_devio                                     *
 *===========================================================================*/
పూర్ణాంకం do_devio(నిర్మాణం proc * caller, సందేశం * m_ptr)
{
    నిర్మాణం priv *privp;
    పోర్టు_t పోర్టు;
    నిర్మాణం io_range *iorp;
    పూర్ణాంకం i, కొలత, nr_io_range;
    పూర్ణాంకం io_type, io_dir;

    io_type = m_ptr->m_lsys_krn_sys_devio.request & _DIO_TYPEMASK;
    io_dir  = m_ptr->m_lsys_krn_sys_devio.request & _DIO_DIRMASK;

    ఎంపిక (io_type)
    {
	స్థితి _DIO_BYTE: కొలత= 1; ఆపు;
	స్థితి DIO_మాట: కొలత= 2; ఆపు;
	స్థితి _DIO_LONG: కొలత= 4; ఆపు;
	అప్రమేయ: కొలత= 4; ఆపు;	/* Be conservative */
    }

    privp= priv(caller);
    ఒకవేళ (!privp)
    {
	రాయి("no priv structure!\n");
	వెళ్ళు doit;
    }
    ఒకవేళ (privp->s_flags & CHECK_IO_పోర్టు)
    {
	పోర్టు= m_ptr->m_lsys_krn_sys_devio.పోర్టు;
	nr_io_range= privp->s_nr_io_range;
	కొరకు (i= 0, iorp= privp->s_io_tab; i<nr_io_range; i++, iorp++)
	{
		ఒకవేళ (పోర్టు >= iorp->ior_పునాది && పోర్టు+కొలత-1 <= iorp->ior_హద్దు)
			ఆపు;
	}
	ఒకవేళ (i >= nr_io_range)
	{
			రాయి("do_devio: port 0x%x (size %d) not allowed\n",
				m_ptr->m_lsys_krn_sys_devio.పోర్టు, కొలత);
		తిరిగి EPERM;
	}
    }

doit:
    ఒకవేళ (m_ptr->m_lsys_krn_sys_devio.పోర్టు & (కొలత-1))
    {
		రాయి("do_devio: unaligned port 0x%x (size %d)\n",
			m_ptr->m_lsys_krn_sys_devio.పోర్టు, కొలత);
	తిరిగి EPERM;
    }

/* Process a single I/O request for byte, word, and long values. */
    ఒకవేళ (io_dir == DIO_ఆదానం) { 
      ఎంపిక (io_type) {
	/* maybe "it" should not be called ports */
        స్థితి _DIO_BYTE:
		m_ptr->m_krn_lsys_sys_devio.విలువ =
			inb(m_ptr->m_lsys_krn_sys_devio.పోర్టు);
		ఆపు;
        స్థితి DIO_మాట:
		m_ptr->m_krn_lsys_sys_devio.విలువ =
			inw(m_ptr->m_lsys_krn_sys_devio.పోర్టు);
		ఆపు;
        స్థితి _DIO_LONG:
		m_ptr->m_krn_lsys_sys_devio.విలువ =
			inl(m_ptr->m_lsys_krn_sys_devio.పోర్టు);
		ఆపు;
    	అప్రమేయ: తిరిగి(EINVAL);
      } 
    } లేకపోతే { 
      ఎంపిక (io_type) {
	స్థితి _DIO_BYTE:
		outb(m_ptr->m_lsys_krn_sys_devio.పోర్టు,
			m_ptr->m_lsys_krn_sys_devio.విలువ);
		ఆపు;
	స్థితి DIO_మాట:
		outw(m_ptr->m_lsys_krn_sys_devio.పోర్టు,
			m_ptr->m_lsys_krn_sys_devio.విలువ);
		ఆపు;
	స్థితి _DIO_LONG:
		outl(m_ptr->m_lsys_krn_sys_devio.పోర్టు,
			m_ptr->m_lsys_krn_sys_devio.విలువ);
		ఆపు;
    	అప్రమేయ: తిరిగి(EINVAL);
      } 
    }
    తిరిగి(OK);
}

#అయితే_ముగింపు /* USE_DEVIO */
