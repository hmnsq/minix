/* This file contains a collection of miscellaneous procedures:
 *   panic:    abort MINIX due to a fatal error
 *   kputc:          buffered putc used by kernel printf
 */

#ಅನ್ತರ್ಭಾವ "kernel/kernel.h"
#ಅನ್ತರ್ಭಾವ "arch_proto.h"

#ಅನ್ತರ್ಭಾವ <minix/syslib.h>
#ಅನ್ತರ್ಭಾವ <unistd.h>
#ಅನ್ತರ್ಭಾವ <stdarg.h>
#ಅನ್ತರ್ಭಾವ <signal.h>
#ಅನ್ತರ್ಭಾವ <string.h>

#ಅನ್ತರ್ಭಾವ <minix/sys_config.h>

#define ARE_PANICING 0xDEADC0FF

/*===========================================================================*
 *			panic                                          *
 *===========================================================================*/
ಶೂನ್ಯ panic(ಧ್ರುವ ವರ್ಣ *fmt, ...)
{
  va_list arg;
  /* The system has run aground of a fatal kernel error. Terminate execution. */
  ಯದಿ (kinfo.minix_panicing == ARE_PANICING) {
  	reset();
  }
  kinfo.minix_panicing = ARE_PANICING;
  ಯದಿ (fmt != ರಿಕ್ತ) {
	printf("kernel panic: ");
  	va_start(arg, fmt);
	vprintf(fmt, arg);
	va_end(arg);
	printf("\n");
  }

  printf("kernel on CPU %d: ", cpuid);
  util_stacktrace();

#ಯದಿ 0
  ಯದಿ(get_cpulocal_var(proc_ptr)) {
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
ಶೂನ್ಯ kputc(
  ಪೂರ್ಣಾಙ್ಕ c					/* character to append */
)
{
/* Accumulate a single character for a kernel message. Send a notification
 * to the output drivers if an END_OF_KMESS is encountered.
 */
  ಯದಿ (c != END_OF_KMESS) {
      ಪೂರ್ಣಾಙ್ಕ maxblpos = ಪರಿಮಾಣ(kmess.kmess_buf) - 2;
#ifdef DEBUG_SERIAL
      ಯದಿ (kinfo.do_serial_debug) {
	ಯದಿ(c == '\n')
      		ser_putc('\r');
      	ser_putc(c);
      }
#endif
      kmess.km_buf[kmess.km_next] = c;	/* put normal char in buffer */
      kmess.kmess_buf[kmess.blpos] = c;
      ಯದಿ (kmess.km_size < ಪರಿಮಾಣ(kmess.km_buf))
          kmess.km_size += 1;
      kmess.km_next = (kmess.km_next + 1) % _KMESS_BUF_SIZE;
      ಯದಿ(kmess.blpos == maxblpos) {
      	memmove(kmess.kmess_buf,
		kmess.kmess_buf+1, ಪರಿಮಾಣ(kmess.kmess_buf)-1);
      } ಅನ್ಯಥಾ kmess.blpos++;
  } ಅನ್ಯಥಾ ಯದಿ (!(kinfo.minix_panicing || kinfo.do_serial_debug)) {
	send_diag_sig();
  }
}

/*===========================================================================*
 *				_exit				     	     *
 *===========================================================================*/
ಶೂನ್ಯ _exit(
  ಪೂರ್ಣಾಙ್ಕ e					/* error code */
)
{
  panic("_exit called from within the kernel, should not happen. (err %i)", e);
}
