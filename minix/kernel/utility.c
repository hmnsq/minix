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

#تعریف_کړه ARE_PANICING 0xDEADC0FF

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
	چاپ_کړه("kernel panic: ");
  	va_پیل(arg, fmt);
	vprintf(fmt, arg);
	va_پای_ټکی(arg);
	چاپ_کړه("\n");
  }

  چاپ_کړه("kernel on CPU %d: ", cpuid);
  util_stacktrace();

#إذا 0
  إذا(get_cpulocal_var(proc_ptr)) {
	  چاپ_کړه("current process : ");
	  proc_stacktrace(get_cpulocal_var(proc_ptr));
  }
#د_که_پای

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
  إذا (c != پای_ټکی_د_KMESS) {
      صحيح maxblpos = حجم(kmess.kmess_buf) - 2;
#که_تعریف_شوی DEBUG_SERIAL
      إذا (kinfo.do_serial_debug) {
	إذا(c == '\n')
      		ser_putc('\r');
      	ser_putc(c);
      }
#د_که_پای
      kmess.km_buf[kmess.km_بل] = c;	/* put normal char in buffer */
      kmess.kmess_buf[kmess.blpos] = c;
      إذا (kmess.km_اندازه < حجم(kmess.km_buf))
          kmess.km_اندازه += 1;
      kmess.km_بل = (kmess.km_بل + 1) % KMESS_BUF_اندازه;
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
