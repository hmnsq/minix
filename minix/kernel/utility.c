/* This file contains a collection of miscellaneous procedures:
 *   panic:    abort MINIX due to a fatal error
 *   kputc:          buffered putc used by kernel printf
 */

#شامل "kernel/kernel.h"
#شامل "arch_proto.h"

#شامل <minix/syslib.h>
#شامل <unistd.h>
#شامل <stdarg.h>
#شامل <signal.h>
#شامل <string.h>

#شامل <minix/sys_config.h>

#تعریف_کرو ARE_PANICING 0xDEADC0FF

/*===========================================================================*
 *			panic                                          *
 *===========================================================================*/
خالی panic(ثابت حرف *fmt, ...)
{
  va_list arg;
  /* The system has run aground of a fatal kernel error. Terminate execution. */
  اگر (kinfo.minix_panicing == ARE_PANICING) {
  	reset();
  }
  kinfo.minix_panicing = ARE_PANICING;
  اگر (fmt != تہی) {
	لکھو("kernel panic: ");
  	va_آغاز(arg, fmt);
	vprintf(fmt, arg);
	va_انتہا(arg);
	لکھو("\n");
  }

  لکھو("kernel on CPU %d: ", cpuid);
  util_stacktrace();

#اگر 0
  اگر(get_cpulocal_var(proc_ptr)) {
	  لکھو("current process : ");
	  proc_stacktrace(get_cpulocal_var(proc_ptr));
  }
#اگر_ختم

  /* Abort MINIX. */
  minix_shutdown(0);
}

/*===========================================================================*
 *				kputc				     	     *
 *===========================================================================*/
خالی kputc(
  صحیح c					/* character to append */
)
{
/* Accumulate a single character for a kernel message. Send a notification
 * to the output drivers if an END_OF_KMESS is encountered.
 */
  اگر (c != انتہا_کا_KMESS) {
      صحیح maxblpos = حجم(kmess.kmess_buf) - 2;
#اگر_تعریف_شدہ DEBUG_SERIAL
      اگر (kinfo.do_serial_debug) {
	اگر(c == '\n')
      		ser_putc('\r');
      	ser_putc(c);
      }
#اگر_ختم
      kmess.km_buf[kmess.km_اگلا] = c;	/* put normal char in buffer */
      kmess.kmess_buf[kmess.blpos] = c;
      اگر (kmess.km_سائز < حجم(kmess.km_buf))
          kmess.km_سائز += 1;
      kmess.km_اگلا = (kmess.km_اگلا + 1) % KMESS_BUF_سائز;
      اگر(kmess.blpos == maxblpos) {
      	memmove(kmess.kmess_buf,
		kmess.kmess_buf+1, حجم(kmess.kmess_buf)-1);
      } ورنہ kmess.blpos++;
  } ورنہ اگر (!(kinfo.minix_panicing || kinfo.do_serial_debug)) {
	send_diag_sig();
  }
}

/*===========================================================================*
 *				_exit				     	     *
 *===========================================================================*/
خالی _exit(
  صحیح e					/* error code */
)
{
  panic("_exit called from within the kernel, should not happen. (err %i)", e);
}
