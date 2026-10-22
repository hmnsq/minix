/* The kernel call implemented in this file:
 *   m_type:	SYS_VDEVIO
 *
 * The parameters for this kernel call are:
 *    m_lsys_krn_sys_vdevio.request	(request input or output)
 *    m_lsys_krn_sys_vdevio.vec_addr	(pointer to port/ value pairs)
 *    m_lsys_krn_sys_vdevio.vec_size	(number of ports to read or write)
 */

#అన్తర్భావ "kernel/system.h"
#అన్తర్భావ <minix/devio.h>
#అన్తర్భావ <minix/endpoint.h>

#ఒకవేళ USE_VDEVIO

/* Buffer for SYS_VDEVIO to copy (port,value)-pairs from/ to user. */
స్థిర అక్షరం vdevio_buf[VDEVIO_BUF_కొలత];      
స్థిర pvb_pair_t * స్థిరరాశి pvb = (pvb_pair_t *) vdevio_buf;           
స్థిర pvw_pair_t * స్థిరరాశి pvw = (pvw_pair_t *) vdevio_buf;      
స్థిర pvl_pair_t * స్థిరరాశి pvl = (pvl_pair_t *) vdevio_buf;     

/*===========================================================================*
 *			        do_vdevio                                    *
 *===========================================================================*/
పూర్ణాంకం do_vdevio(నిర్మాణం proc * caller, సందేశం * m_ptr)
{
/* Perform a series of device I/O on behalf of a non-kernel process. The 
 * I/O addresses and I/O values are fetched from and returned to some buffer
 * in user space. The actual I/O is wrapped by lock() and unlock() to prevent
 * that I/O batch from being interrupted.
 * This is the counterpart of do_devio, which performs a single device I/O. 
 */ 
  పూర్ణాంకం vec_కొలత;               /* size of vector */
  పూర్ణాంకం io_లోపల;                  /* true if input */
  size_t బైట్లు;               /* # bytes to be copied */
  పోర్టు_t పోర్టు;
  పూర్ణాంకం i, j, io_కొలత, nr_io_range;
  పూర్ణాంకం io_dir, io_type;
  నిర్మాణం priv *privp;
  నిర్మాణం io_range *iorp;
  పూర్ణాంకం r;
    
  /* Get the request, size of the request vector, and check the values. */
  io_dir = m_ptr->m_lsys_krn_sys_vdevio.request & _DIO_DIRMASK;
  io_type = m_ptr->m_lsys_krn_sys_vdevio.request & _DIO_TYPEMASK;
  ఒకవేళ (io_dir == DIO_ఆదానం) io_లోపల = TRUE;
  లేకపోతే ఒకవేళ (io_dir == _DIO_OUTPUT) io_లోపల = FALSE;
  లేకపోతే తిరిగి(EINVAL);
  ఒకవేళ ((vec_కొలత = m_ptr->m_lsys_krn_sys_vdevio.vec_కొలత) <= 0) తిరిగి(EINVAL);
  ఎంపిక (io_type) {
      స్థితి _DIO_BYTE:
	బైట్లు = vec_కొలత * పరిమాణం(pvb_pair_t);
	io_కొలత= పరిమాణం(u8_t);
	ఆపు;
      స్థితి DIO_మాట:
	బైట్లు = vec_కొలత * పరిమాణం(pvw_pair_t);
	io_కొలత= పరిమాణం(u16_t);
	ఆపు;
      స్థితి _DIO_LONG:
	బైట్లు = vec_కొలత * పరిమాణం(pvl_pair_t);
	io_కొలత= పరిమాణం(u32_t);
	ఆపు;
      అప్రమేయ:  తిరిగి(EINVAL);   /* check type once and for all */
  }
  ఒకవేళ (బైట్లు > పరిమాణం(vdevio_buf))  తిరిగి(E2BIG);

  /* Copy (port,value)-pairs from user. */
  ఒకవేళ((r=దత్తాంశం_copy(caller->p_endpoint, m_ptr->m_lsys_krn_sys_vdevio.vec_addr,
    KERNEL, (vir_బైట్లు) vdevio_buf, బైట్లు)) != OK)
	తిరిగి r;

  privp= priv(caller);
  ఒకవేళ (privp && (privp->s_flags & CHECK_IO_పోర్టు))
  {
	/* Check whether the I/O is allowed */
	nr_io_range= privp->s_nr_io_range;
	కొరకు (i=0; i<vec_కొలత; i++)
	{
		ఎంపిక (io_type) {
		స్థితి _DIO_BYTE: పోర్టు= pvb[i].పోర్టు; ఆపు;
		స్థితి DIO_మాట: పోర్టు= pvw[i].పోర్టు; ఆపు;
		అప్రమేయ:	పోర్టు= pvl[i].పోర్టు; ఆపు;
		}
		కొరకు (j= 0, iorp= privp->s_io_tab; j<nr_io_range; j++, iorp++)
		{
			ఒకవేళ (పోర్టు >= iorp->ior_పునాది &&
				పోర్టు+io_కొలత-1 <= iorp->ior_హద్దు)
			{
				ఆపు;
			}
		}
		ఒకవేళ (j >= nr_io_range)
		{
			రాయి(
		"do_vdevio: I/O port check failed for proc %d, port 0x%x\n",
				caller->p_endpoint, పోర్టు);
			తిరిగి EPERM;
		}
	}
  }

  /* Perform actual device I/O for byte, word, and long values */
  ఎంపిక (io_type) {
  స్థితి _DIO_BYTE: 					 /* byte values */
      ఒకవేళ (io_లోపల) కొరకు (i=0; i<vec_కొలత; i++) 
		pvb[i].విలువ = inb( pvb[i].పోర్టు); 
      లేకపోతే      కొరకు (i=0; i<vec_కొలత; i++)
		outb( pvb[i].పోర్టు, pvb[i].విలువ); 
      ఆపు; 
  స్థితి DIO_మాట:					  /* word values */
      ఒకవేళ (io_లోపల)
      {
	కొరకు (i=0; i<vec_కొలత; i++)  
	{
		పోర్టు= pvw[i].పోర్టు;
		ఒకవేళ (పోర్టు & 1) వెళ్ళు bad;
		pvw[i].విలువ = inw( pvw[i].పోర్టు);  
	}
      }
      లేకపోతే
      {
	కొరకు (i=0; i<vec_కొలత; i++) 
	{
		పోర్టు= pvw[i].పోర్టు;
		ఒకవేళ (పోర్టు & 1) వెళ్ళు bad;
		outw( pvw[i].పోర్టు, pvw[i].విలువ); 
	}
      }
      ఆపు; 
  అప్రమేయ:            					  /* long values */
      ఒకవేళ (io_లోపల)
      {
	కొరకు (i=0; i<vec_కొలత; i++)
	{
		పోర్టు= pvl[i].పోర్టు;
		ఒకవేళ (పోర్టు & 3) వెళ్ళు bad;
		pvl[i].విలువ = inl(pvl[i].పోర్టు);  
	}
      }
      లేకపోతే
      {
	కొరకు (i=0; i<vec_కొలత; i++)
	{
		పోర్టు= pvl[i].పోర్టు;
		ఒకవేళ (పోర్టు & 3) వెళ్ళు bad;
		outl( pvb[i].పోర్టు, pvl[i].విలువ); 
	}
      }
  }
    
  /* Almost done, copy back results for input requests. */
  ఒకవేళ (io_లోపల) 
	ఒకవేళ((r=దత్తాంశం_copy(KERNEL, (vir_బైట్లు) vdevio_buf,
	  caller->p_endpoint, m_ptr->m_lsys_krn_sys_vdevio.vec_addr,
	  (phys_బైట్లు) బైట్లు)) != OK)
		తిరిగి r;
  తిరిగి(OK);

bad:
	panic("do_vdevio: unaligned port: %d", పోర్టు);
	తిరిగి EPERM;
}

#అయితే_ముగింపు /* USE_VDEVIO */

