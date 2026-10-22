/* This file contains a collection of miscellaneous procedures:
 *   panic:    abort MINIX due to a fatal error
 *   kputc:          buffered putc used by kernel printf
 */

#అన్తర్భావ "kernel/kernel.h"
#అన్తర్భావ "arch_proto.h"

#అన్తర్భావ <minix/syslib.h>
#అన్తర్భావ <unistd.h>
#అన్తర్భావ <stdarg.h>
#అన్తర్భావ <signal.h>
#అన్తర్భావ <string.h>

#అన్తర్భావ <minix/sys_config.h>

#నిర్వచించు ARE_PANICING 0xDEADC0FF

/*===========================================================================*
 *			panic                                          *
 *===========================================================================*/
శూన్య panic(స్థిరరాశి అక్షరం *fmt, ...)
{
  va_list arg;
  /* The system has run aground of a fatal kernel error. Terminate execution. */
  ఒకవేళ (kinfo.minix_panicing == ARE_PANICING) {
  	reset();
  }
  kinfo.minix_panicing = ARE_PANICING;
  ఒకవేళ (fmt != ఖాళీ) {
	రాయి("kernel panic: ");
  	va_ప్రారంభం(arg, fmt);
	vprintf(fmt, arg);
	va_చివర(arg);
	రాయి("\n");
  }

  రాయి("kernel on CPU %d: ", cpuid);
  util_stacktrace();

#ఒకవేళ 0
  ఒకవేళ(get_cpulocal_var(proc_ptr)) {
	  రాయి("current process : ");
	  proc_stacktrace(get_cpulocal_var(proc_ptr));
  }
#అయితే_ముగింపు

  /* Abort MINIX. */
  minix_shutdown(0);
}

/*===========================================================================*
 *				kputc				     	     *
 *===========================================================================*/
శూన్య kputc(
  పూర్ణాంకం c					/* character to append */
)
{
/* Accumulate a single character for a kernel message. Send a notification
 * to the output drivers if an END_OF_KMESS is encountered.
 */
  ఒకవేళ (c != చివర_యొక్క_KMESS) {
      పూర్ణాంకం maxblpos = పరిమాణం(kmess.kmess_buf) - 2;
#నిర్వచితమైతే DEBUG_SERIAL
      ఒకవేళ (kinfo.do_serial_debug) {
	ఒకవేళ(c == '\n')
      		ser_putc('\r');
      	ser_putc(c);
      }
#అయితే_ముగింపు
      kmess.km_buf[kmess.km_తదుపరి] = c;	/* put normal char in buffer */
      kmess.kmess_buf[kmess.blpos] = c;
      ఒకవేళ (kmess.km_కొలత < పరిమాణం(kmess.km_buf))
          kmess.km_కొలత += 1;
      kmess.km_తదుపరి = (kmess.km_తదుపరి + 1) % KMESS_BUF_కొలత;
      ఒకవేళ(kmess.blpos == maxblpos) {
      	memmove(kmess.kmess_buf,
		kmess.kmess_buf+1, పరిమాణం(kmess.kmess_buf)-1);
      } లేకపోతే kmess.blpos++;
  } లేకపోతే ఒకవేళ (!(kinfo.minix_panicing || kinfo.do_serial_debug)) {
	send_diag_sig();
  }
}

/*===========================================================================*
 *				_exit				     	     *
 *===========================================================================*/
శూన్య _exit(
  పూర్ణాంకం e					/* error code */
)
{
  panic("_exit called from within the kernel, should not happen. (err %i)", e);
}
