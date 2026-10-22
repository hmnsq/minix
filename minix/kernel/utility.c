/* This file contains a collection of miscellaneous procedures:
 *   panic:    abort MINIX due to a fatal error
 *   kputc:          buffered putc used by kernel printf
 */

#ଅନ୍ତର୍ଭାଵ "kernel/kernel.h"
#ଅନ୍ତର୍ଭାଵ "arch_proto.h"

#ଅନ୍ତର୍ଭାଵ <minix/syslib.h>
#ଅନ୍ତର୍ଭାଵ <unistd.h>
#ଅନ୍ତର୍ଭାଵ <stdarg.h>
#ଅନ୍ତର୍ଭାଵ <signal.h>
#ଅନ୍ତର୍ଭାଵ <string.h>

#ଅନ୍ତର୍ଭାଵ <minix/sys_config.h>

#define ARE_PANICING 0xDEADC0FF

/*===========================================================================*
 *			panic                                          *
 *===========================================================================*/
ଶୂନ୍ୟ panic(ଅଚଳ ଅକ୍ଷର *fmt, ...)
{
  va_list arg;
  /* The system has run aground of a fatal kernel error. Terminate execution. */
  ଯଦି (kinfo.minix_panicing == ARE_PANICING) {
  	reset();
  }
  kinfo.minix_panicing = ARE_PANICING;
  ଯଦି (fmt != ଖାଲି) {
	printf("kernel panic: ");
  	va_start(arg, fmt);
	vprintf(fmt, arg);
	va_end(arg);
	printf("\n");
  }

  printf("kernel on CPU %d: ", cpuid);
  util_stacktrace();

#ଯଦି 0
  ଯଦି(get_cpulocal_var(proc_ptr)) {
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
ଶୂନ୍ୟ kputc(
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା c					/* character to append */
)
{
/* Accumulate a single character for a kernel message. Send a notification
 * to the output drivers if an END_OF_KMESS is encountered.
 */
  ଯଦି (c != END_OF_KMESS) {
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା maxblpos = ଆକାର(kmess.kmess_buf) - 2;
#ifdef DEBUG_SERIAL
      ଯଦି (kinfo.do_serial_debug) {
	ଯଦି(c == '\n')
      		ser_putc('\r');
      	ser_putc(c);
      }
#endif
      kmess.km_buf[kmess.km_next] = c;	/* put normal char in buffer */
      kmess.kmess_buf[kmess.blpos] = c;
      ଯଦି (kmess.km_size < ଆକାର(kmess.km_buf))
          kmess.km_size += 1;
      kmess.km_next = (kmess.km_next + 1) % _KMESS_BUF_SIZE;
      ଯଦି(kmess.blpos == maxblpos) {
      	memmove(kmess.kmess_buf,
		kmess.kmess_buf+1, ଆକାର(kmess.kmess_buf)-1);
      } ଅନ୍ୟଥା kmess.blpos++;
  } ଅନ୍ୟଥା ଯଦି (!(kinfo.minix_panicing || kinfo.do_serial_debug)) {
	send_diag_sig();
  }
}

/*===========================================================================*
 *				_exit				     	     *
 *===========================================================================*/
ଶୂନ୍ୟ _exit(
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା e					/* error code */
)
{
  panic("_exit called from within the kernel, should not happen. (err %i)", e);
}
