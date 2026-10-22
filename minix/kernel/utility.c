/* This file contains a collection of miscellaneous procedures:
 *   panic:    abort MINIX due to a fatal error
 *   kputc:          buffered putc used by kernel printf
 */

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/kernel.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "arch_proto.h"

#ਅਨ੍ਤਰ੍ਭਾਵ <minix/syslib.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <unistd.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdarg.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <signal.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>

#ਅਨ੍ਤਰ੍ਭਾਵ <minix/sys_config.h>

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ARE_PANICING 0xDEADC0FF

/*===========================================================================*
 *			panic                                          *
 *===========================================================================*/
ਖਾਲੀ panic(ਅਚਲ ਅੱਖਰ *fmt, ...)
{
  va_list arg;
  /* The system has run aground of a fatal kernel error. Terminate execution. */
  ਜੇ (kinfo.minix_panicing == ARE_PANICING) {
  	reset();
  }
  kinfo.minix_panicing = ARE_PANICING;
  ਜੇ (fmt != ਨਿਰਾ) {
	ਲਿਖੋ("kernel panic: ");
  	va_ਸ਼ੁਰੂ(arg, fmt);
	vprintf(fmt, arg);
	va_ਸਿਰਾ(arg);
	ਲਿਖੋ("\n");
  }

  ਲਿਖੋ("kernel on CPU %d: ", cpuid);
  util_stacktrace();

#ਜੇ 0
  ਜੇ(get_cpulocal_var(proc_ptr)) {
	  ਲਿਖੋ("current process : ");
	  proc_stacktrace(get_cpulocal_var(proc_ptr));
  }
#ਜੇ_ਅੰਤ

  /* Abort MINIX. */
  minix_shutdown(0);
}

/*===========================================================================*
 *				kputc				     	     *
 *===========================================================================*/
ਖਾਲੀ kputc(
  ਪੂਰਨ c					/* character to append */
)
{
/* Accumulate a single character for a kernel message. Send a notification
 * to the output drivers if an END_OF_KMESS is encountered.
 */
  ਜੇ (c != ਸਿਰਾ_ਦਾ_KMESS) {
      ਪੂਰਨ maxblpos = ਆਕਾਰ(kmess.kmess_buf) - 2;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUG_SERIAL
      ਜੇ (kinfo.do_serial_debug) {
	ਜੇ(c == '\n')
      		ser_putc('\r');
      	ser_putc(c);
      }
#ਜੇ_ਅੰਤ
      kmess.km_buf[kmess.km_ਅਗਲਾ] = c;	/* put normal char in buffer */
      kmess.kmess_buf[kmess.blpos] = c;
      ਜੇ (kmess.km_ਮਾਪ < ਆਕਾਰ(kmess.km_buf))
          kmess.km_ਮਾਪ += 1;
      kmess.km_ਅਗਲਾ = (kmess.km_ਅਗਲਾ + 1) % KMESS_BUF_ਮਾਪ;
      ਜੇ(kmess.blpos == maxblpos) {
      	memmove(kmess.kmess_buf,
		kmess.kmess_buf+1, ਆਕਾਰ(kmess.kmess_buf)-1);
      } ਨਹੀਂਤਾਂ kmess.blpos++;
  } ਨਹੀਂਤਾਂ ਜੇ (!(kinfo.minix_panicing || kinfo.do_serial_debug)) {
	send_diag_sig();
  }
}

/*===========================================================================*
 *				_exit				     	     *
 *===========================================================================*/
ਖਾਲੀ _exit(
  ਪੂਰਨ e					/* error code */
)
{
  panic("_exit called from within the kernel, should not happen. (err %i)", e);
}
