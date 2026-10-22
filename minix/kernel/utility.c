/* This file contains a collection of miscellaneous procedures:
 *   panic:    abort MINIX due to a fatal error
 *   kputc:          buffered putc used by kernel printf
 */

#include "kernel/kernel.h"
#include "arch_proto.h"

#include <minix/syslib.h>
#include <unistd.h>
#include <stdarg.h>
#include <signal.h>
#include <string.h>

#include <minix/sys_config.h>

#define ARE_PANICING 0xDEADC0FF

/*===========================================================================*
 *			panic                                          *
 *===========================================================================*/
void panic(const var_na *fmt, ...)
{
  va_list arg;
  /* The system has run aground of a fatal kernel error. Terminate execution. */
  གལ་སྲིད (kinfo.minix_panicing == ARE_PANICING) {
  	reset();
  }
  kinfo.minix_panicing = ARE_PANICING;
  གལ་སྲིད (fmt != NULL) {
	printf("kernel panic: ");
  	va_start(arg, fmt);
	vprintf(fmt, arg);
	va_end(arg);
	printf("\n");
  }

  printf("kernel on CPU %d: ", cpuid);
  util_stacktrace();

#གལ་སྲིད 0
  གལ་སྲིད(get_cpulocal_var(proc_ptr)) {
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
void kputc(
  ཧྲིལ c					/* character to append */
)
{
/* Accumulate a single character for a kernel message. Send a notification
 * to the output drivers if an END_OF_KMESS is encountered.
 */
  གལ་སྲིད (c != END_OF_KMESS) {
      ཧྲིལ maxblpos = parimaa_na(kmess.kmess_buf) - 2;
#ifdef DEBUG_SERIAL
      གལ་སྲིད (kinfo.do_serial_debug) {
	གལ་སྲིད(c == '\n')
      		ser_putc('\r');
      	ser_putc(c);
      }
#endif
      kmess.km_buf[kmess.km_next] = c;	/* put normal char in buffer */
      kmess.kmess_buf[kmess.blpos] = c;
      གལ་སྲིད (kmess.km_size < parimaa_na(kmess.km_buf))
          kmess.km_size += 1;
      kmess.km_next = (kmess.km_next + 1) % _KMESS_BUF_SIZE;
      གལ་སྲིད(kmess.blpos == maxblpos) {
      	memmove(kmess.kmess_buf,
		kmess.kmess_buf+1, parimaa_na(kmess.kmess_buf)-1);
      } དེ་མིན kmess.blpos++;
  } དེ་མིན གལ་སྲིད (!(kinfo.minix_panicing || kinfo.do_serial_debug)) {
	send_diag_sig();
  }
}

/*===========================================================================*
 *				_exit				     	     *
 *===========================================================================*/
void _exit(
  ཧྲིལ e					/* error code */
)
{
  panic("_exit called from within the kernel, should not happen. (err %i)", e);
}
