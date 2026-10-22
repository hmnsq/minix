/* The kernel call implemented in this file:
 *   m_type:	SYS_DEVIO
 *
 * The parameters for this kernel call are:
 *   m_lsys_krn_sys_devio.request	(request input or output)
 *   m_lsys_krn_sys_devio.port		(port to read/ write)
 *   m_lsys_krn_sys_devio.value		(value to write/ return value read)
 */

#ଅନ୍ତର୍ଭାଵ "kernel/system.h"
#ଅନ୍ତର୍ଭାଵ <minix/devio.h>
#ଅନ୍ତର୍ଭାଵ <minix/endpoint.h>

#ଯଦି USE_DEVIO

/*===========================================================================*
 *			        do_devio                                     *
 *===========================================================================*/
ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_devio(ସଂରଚନା proc * caller, message * m_ptr)
{
    ସଂରଚନା priv *privp;
    port_t port;
    ସଂରଚନା io_range *iorp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i, size, nr_io_range;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା io_type, io_dir;

    io_type = m_ptr->m_lsys_krn_sys_devio.request & _DIO_TYPEMASK;
    io_dir  = m_ptr->m_lsys_krn_sys_devio.request & _DIO_DIRMASK;

    ବାଛ (io_type)
    {
	ସ୍ଥିତି _DIO_BYTE: size= 1; ଭାଙ୍ଗ;
	ସ୍ଥିତି _DIO_WORD: size= 2; ଭାଙ୍ଗ;
	ସ୍ଥିତି _DIO_LONG: size= 4; ଭାଙ୍ଗ;
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ: size= 4; ଭାଙ୍ଗ;	/* Be conservative */
    }

    privp= priv(caller);
    ଯଦି (!privp)
    {
	printf("no priv structure!\n");
	ଯାଅ doit;
    }
    ଯଦି (privp->s_flags & CHECK_IO_PORT)
    {
	port= m_ptr->m_lsys_krn_sys_devio.port;
	nr_io_range= privp->s_nr_io_range;
	ପାଇଁ (i= 0, iorp= privp->s_io_tab; i<nr_io_range; i++, iorp++)
	{
		ଯଦି (port >= iorp->ior_base && port+size-1 <= iorp->ior_limit)
			ଭାଙ୍ଗ;
	}
	ଯଦି (i >= nr_io_range)
	{
			printf("do_devio: port 0x%x (size %d) not allowed\n",
				m_ptr->m_lsys_krn_sys_devio.port, size);
		ଫେରନ୍ତୁ EPERM;
	}
    }

doit:
    ଯଦି (m_ptr->m_lsys_krn_sys_devio.port & (size-1))
    {
		printf("do_devio: unaligned port 0x%x (size %d)\n",
			m_ptr->m_lsys_krn_sys_devio.port, size);
	ଫେରନ୍ତୁ EPERM;
    }

/* Process a single I/O request for byte, word, and long values. */
    ଯଦି (io_dir == DIO_ନିବେଶ) { 
      ବାଛ (io_type) {
	/* maybe "it" should not be called ports */
        ସ୍ଥିତି _DIO_BYTE:
		m_ptr->m_krn_lsys_sys_devio.ମୂଲ୍ୟ =
			inb(m_ptr->m_lsys_krn_sys_devio.port);
		ଭାଙ୍ଗ;
        ସ୍ଥିତି _DIO_WORD:
		m_ptr->m_krn_lsys_sys_devio.ମୂଲ୍ୟ =
			inw(m_ptr->m_lsys_krn_sys_devio.port);
		ଭାଙ୍ଗ;
        ସ୍ଥିତି _DIO_LONG:
		m_ptr->m_krn_lsys_sys_devio.ମୂଲ୍ୟ =
			inl(m_ptr->m_lsys_krn_sys_devio.port);
		ଭାଙ୍ଗ;
    	ପୂର୍ବନିର୍ଦ୍ଧାରିତ: ଫେରନ୍ତୁ(EINVAL);
      } 
    } ଅନ୍ୟଥା { 
      ବାଛ (io_type) {
	ସ୍ଥିତି _DIO_BYTE:
		outb(m_ptr->m_lsys_krn_sys_devio.port,
			m_ptr->m_lsys_krn_sys_devio.ମୂଲ୍ୟ);
		ଭାଙ୍ଗ;
	ସ୍ଥିତି _DIO_WORD:
		outw(m_ptr->m_lsys_krn_sys_devio.port,
			m_ptr->m_lsys_krn_sys_devio.ମୂଲ୍ୟ);
		ଭାଙ୍ଗ;
	ସ୍ଥିତି _DIO_LONG:
		outl(m_ptr->m_lsys_krn_sys_devio.port,
			m_ptr->m_lsys_krn_sys_devio.ମୂଲ୍ୟ);
		ଭାଙ୍ଗ;
    	ପୂର୍ବନିର୍ଦ୍ଧାରିତ: ଫେରନ୍ତୁ(EINVAL);
      } 
    }
    ଫେରନ୍ତୁ(OK);
}

#endif /* USE_DEVIO */
