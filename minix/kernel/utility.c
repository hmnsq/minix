/* This file contains a collection of miscellaneous procedures:
 *   panic:    abort MINIX due to a fatal error
 *   kputc:          buffered putc used by kernel printf
 */

#அந்தர்பாவ "kernel/kernel.h"
#அந்தர்பாவ "arch_proto.h"

#அந்தர்பாவ <minix/syslib.h>
#அந்தர்பாவ <unistd.h>
#அந்தர்பாவ <stdarg.h>
#அந்தர்பாவ <signal.h>
#அந்தர்பாவ <string.h>

#அந்தர்பாவ <minix/sys_config.h>

#define ARE_PANICING 0xDEADC0FF

/*===========================================================================*
 *			panic                                          *
 *===========================================================================*/
வெறுமை panic(மாறிலி எழுத்து *fmt, ...)
{
  va_list arg;
  /* The system has run aground of a fatal kernel error. Terminate execution. */
  எனில் (kinfo.minix_panicing == ARE_PANICING) {
  	reset();
  }
  kinfo.minix_panicing = ARE_PANICING;
  எனில் (fmt != வெற்று) {
	printf("kernel panic: ");
  	va_start(arg, fmt);
	vprintf(fmt, arg);
	va_end(arg);
	printf("\n");
  }

  printf("kernel on CPU %d: ", cpuid);
  util_stacktrace();

#எனில் 0
  எனில்(get_cpulocal_var(proc_ptr)) {
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
வெறுமை kputc(
  முழு c					/* character to append */
)
{
/* Accumulate a single character for a kernel message. Send a notification
 * to the output drivers if an END_OF_KMESS is encountered.
 */
  எனில் (c != END_OF_KMESS) {
      முழு maxblpos = அளவு(kmess.kmess_buf) - 2;
#ifdef DEBUG_SERIAL
      எனில் (kinfo.do_serial_debug) {
	எனில்(c == '\n')
      		ser_putc('\r');
      	ser_putc(c);
      }
#endif
      kmess.km_buf[kmess.km_next] = c;	/* put normal char in buffer */
      kmess.kmess_buf[kmess.blpos] = c;
      எனில் (kmess.km_size < அளவு(kmess.km_buf))
          kmess.km_size += 1;
      kmess.km_next = (kmess.km_next + 1) % _KMESS_BUF_SIZE;
      எனில்(kmess.blpos == maxblpos) {
      	memmove(kmess.kmess_buf,
		kmess.kmess_buf+1, அளவு(kmess.kmess_buf)-1);
      } இல்லையெனில் kmess.blpos++;
  } இல்லையெனில் எனில் (!(kinfo.minix_panicing || kinfo.do_serial_debug)) {
	send_diag_sig();
  }
}

/*===========================================================================*
 *				_exit				     	     *
 *===========================================================================*/
வெறுமை _exit(
  முழு e					/* error code */
)
{
  panic("_exit called from within the kernel, should not happen. (err %i)", e);
}
