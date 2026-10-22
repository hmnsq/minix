/* This file contains a collection of miscellaneous procedures:
 *   panic:    abort MINIX due to a fatal error
 *   kputc:          buffered putc used by kernel printf
 */

#ضمن "kernel/kernel.h"
#ضمن "arch_proto.h"

#ضمن <minix/syslib.h>
#ضمن <unistd.h>
#ضمن <stdarg.h>
#ضمن <signal.h>
#ضمن <string.h>

#ضمن <minix/sys_config.h>

#وصف_ڪريو ARE_PANICING 0xDEADC0FF

/*===========================================================================*
 *			panic                                          *
 *===========================================================================*/
فراغ panic(ثابت حرف *fmt, ...)
{
  va_list arg;
  /* The system has run aground of a fatal kernel error. Terminate execution. */
  إذا (kinfo.minix_panicing == ARE_PANICING) {
  	reset();
  }
  kinfo.minix_panicing = ARE_PANICING;
  إذا (fmt != عدم) {
	لکو("kernel panic: ");
  	va_شروعات(arg, fmt);
	vprintf(fmt, arg);
	va_پڇاڙي(arg);
	لکو("\n");
  }

  لکو("kernel on CPU %d: ", cpuid);
  util_stacktrace();

#إذا 0
  إذا(get_cpulocal_var(proc_ptr)) {
	  لکو("current process : ");
	  proc_stacktrace(get_cpulocal_var(proc_ptr));
  }
#جيڪڏهن_پورو

  /* Abort MINIX. */
  minix_shutdown(0);
}

/*===========================================================================*
 *				kputc				     	     *
 *===========================================================================*/
فراغ kputc(
  صحيح c					/* character to append */
)
{
/* Accumulate a single character for a kernel message. Send a notification
 * to the output drivers if an END_OF_KMESS is encountered.
 */
  إذا (c != پڇاڙي_جو_KMESS) {
      صحيح maxblpos = حجم(kmess.kmess_buf) - 2;
#جيڪڏهن_وصف_ٿيل DEBUG_SERIAL
      إذا (kinfo.do_serial_debug) {
	إذا(c == '\n')
      		ser_putc('\r');
      	ser_putc(c);
      }
#جيڪڏهن_پورو
      kmess.km_buf[kmess.km_ايندڙ] = c;	/* put normal char in buffer */
      kmess.kmess_buf[kmess.blpos] = c;
      إذا (kmess.km_سائيز < حجم(kmess.km_buf))
          kmess.km_سائيز += 1;
      kmess.km_ايندڙ = (kmess.km_ايندڙ + 1) % KMESS_BUF_سائيز;
      إذا(kmess.blpos == maxblpos) {
      	memmove(kmess.kmess_buf,
		kmess.kmess_buf+1, حجم(kmess.kmess_buf)-1);
      } وإلا kmess.blpos++;
  } وإلا إذا (!(kinfo.minix_panicing || kinfo.do_serial_debug)) {
	send_diag_sig();
  }
}

/*===========================================================================*
 *				_exit				     	     *
 *===========================================================================*/
فراغ _exit(
  صحيح e					/* error code */
)
{
  panic("_exit called from within the kernel, should not happen. (err %i)", e);
}
