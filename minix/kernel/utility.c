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

#define ARE_PANICING 0xDEADC0FF

/*===========================================================================*
 *			panic                                          *
 *===========================================================================*/
فراغ panic(ثابت حرف *fmt, ...)
{
  va_list arg;
  /* The system has run aground of a fatal kernel error. Terminate execution. */
  ܐܢ (kinfo.minix_panicing == ARE_PANICING) {
  	reset();
  }
  kinfo.minix_panicing = ARE_PANICING;
  ܐܢ (fmt != عدم) {
	printf("kernel panic: ");
  	va_start(arg, fmt);
	vprintf(fmt, arg);
	va_end(arg);
	printf("\n");
  }

  printf("kernel on CPU %d: ", cpuid);
  util_stacktrace();

#ܐܢ 0
  ܐܢ(get_cpulocal_var(proc_ptr)) {
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
فراغ kputc(
  صحيح c					/* character to append */
)
{
/* Accumulate a single character for a kernel message. Send a notification
 * to the output drivers if an END_OF_KMESS is encountered.
 */
  ܐܢ (c != END_OF_KMESS) {
      صحيح maxblpos = حجم(kmess.kmess_buf) - 2;
#ifdef DEBUG_SERIAL
      ܐܢ (kinfo.do_serial_debug) {
	ܐܢ(c == '\n')
      		ser_putc('\r');
      	ser_putc(c);
      }
#endif
      kmess.km_buf[kmess.km_next] = c;	/* put normal char in buffer */
      kmess.kmess_buf[kmess.blpos] = c;
      ܐܢ (kmess.km_size < حجم(kmess.km_buf))
          kmess.km_size += 1;
      kmess.km_next = (kmess.km_next + 1) % _KMESS_BUF_SIZE;
      ܐܢ(kmess.blpos == maxblpos) {
      	memmove(kmess.kmess_buf,
		kmess.kmess_buf+1, حجم(kmess.kmess_buf)-1);
      } ܐܚܪܢܐ kmess.blpos++;
  } ܐܚܪܢܐ ܐܢ (!(kinfo.minix_panicing || kinfo.do_serial_debug)) {
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
