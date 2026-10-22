/* This file contains a collection of miscellaneous procedures:
 *   panic:    abort MINIX due to a fatal error
 *   kputc:          buffered putc used by kernel printf
 */

#অন্তর্ভাব "kernel/kernel.h"
#অন্তর্ভাব "arch_proto.h"

#অন্তর্ভাব <minix/syslib.h>
#অন্তর্ভাব <unistd.h>
#অন্তর্ভাব <stdarg.h>
#অন্তর্ভাব <signal.h>
#অন্তর্ভাব <string.h>

#অন্তর্ভাব <minix/sys_config.h>

#define ARE_PANICING 0xDEADC0FF

/*===========================================================================*
 *			panic                                          *
 *===========================================================================*/
শূন্য panic(ধ্রুবক অক্ষর *fmt, ...)
{
  va_list arg;
  /* The system has run aground of a fatal kernel error. Terminate execution. */
  যদি (kinfo.minix_panicing == ARE_PANICING) {
  	reset();
  }
  kinfo.minix_panicing = ARE_PANICING;
  যদি (fmt != ফাঁকা) {
	printf("kernel panic: ");
  	va_start(arg, fmt);
	vprintf(fmt, arg);
	va_end(arg);
	printf("\n");
  }

  printf("kernel on CPU %d: ", cpuid);
  util_stacktrace();

#যদি 0
  যদি(get_cpulocal_var(proc_ptr)) {
	  printf("current process : ");
	  proc_stacktrace(get_cpulocal_var(proc_ptr));
  }
#endif

  /* Abort MINIX. */
  minix_shutdown(0);
}

/*===========================================================================*
 *				kputc				     	     *
 *===========================================================================*/
শূন্য kputc(
  পূর্ণসংখ্যা c					/* character to append */
)
{
/* Accumulate a single character for a kernel message. Send a notification
 * to the output drivers if an END_OF_KMESS is encountered.
 */
  যদি (c != END_OF_KMESS) {
      পূর্ণসংখ্যা maxblpos = আকার(kmess.kmess_buf) - 2;
#ifdef DEBUG_SERIAL
      যদি (kinfo.do_serial_debug) {
	যদি(c == '\n')
      		ser_putc('\r');
      	ser_putc(c);
      }
#endif
      kmess.km_buf[kmess.km_পরবর্তী] = c;	/* put normal char in buffer */
      kmess.kmess_buf[kmess.blpos] = c;
      যদি (kmess.km_আকার < আকার(kmess.km_buf))
          kmess.km_আকার += 1;
      kmess.km_পরবর্তী = (kmess.km_পরবর্তী + 1) % KMESS_BUF_আকার;
      যদি(kmess.blpos == maxblpos) {
      	memmove(kmess.kmess_buf,
		kmess.kmess_buf+1, আকার(kmess.kmess_buf)-1);
      } অন্যথা kmess.blpos++;
  } অন্যথা যদি (!(kinfo.minix_panicing || kinfo.do_serial_debug)) {
	send_diag_sig();
  }
}

/*===========================================================================*
 *				_exit				     	     *
 *===========================================================================*/
শূন্য _exit(
  পূর্ণসংখ্যা e					/* error code */
)
{
  panic("_exit called from within the kernel, should not happen. (err %i)", e);
}
