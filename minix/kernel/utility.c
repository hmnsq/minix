/* This file contains a collection of miscellaneous procedures:
 *   panic:    abort MINIX due to a fatal error
 *   kputc:          buffered putc used by kernel printf
 */

#અન્તર્ભાવ "kernel/kernel.h"
#અન્તર્ભાવ "arch_proto.h"

#અન્તર્ભાવ <minix/syslib.h>
#અન્તર્ભાવ <unistd.h>
#અન્તર્ભાવ <stdarg.h>
#અન્તર્ભાવ <signal.h>
#અન્તર્ભાવ <string.h>

#અન્તર્ભાવ <minix/sys_config.h>

#define ARE_PANICING 0xDEADC0FF

/*===========================================================================*
 *			panic                                          *
 *===========================================================================*/
શૂન્ય panic(અચલ અક્ષર *fmt, ...)
{
  va_list arg;
  /* The system has run aground of a fatal kernel error. Terminate execution. */
  જો (kinfo.minix_panicing == ARE_PANICING) {
  	reset();
  }
  kinfo.minix_panicing = ARE_PANICING;
  જો (fmt != ખાલી) {
	printf("kernel panic: ");
  	va_start(arg, fmt);
	vprintf(fmt, arg);
	va_end(arg);
	printf("\n");
  }

  printf("kernel on CPU %d: ", cpuid);
  util_stacktrace();

#જો 0
  જો(get_cpulocal_var(proc_ptr)) {
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
શૂન્ય kputc(
  પૂર્ણાંક c					/* character to append */
)
{
/* Accumulate a single character for a kernel message. Send a notification
 * to the output drivers if an END_OF_KMESS is encountered.
 */
  જો (c != END_OF_KMESS) {
      પૂર્ણાંક maxblpos = કદ(kmess.kmess_buf) - 2;
#ifdef DEBUG_SERIAL
      જો (kinfo.do_serial_debug) {
	જો(c == '\n')
      		ser_putc('\r');
      	ser_putc(c);
      }
#endif
      kmess.km_buf[kmess.km_next] = c;	/* put normal char in buffer */
      kmess.kmess_buf[kmess.blpos] = c;
      જો (kmess.km_size < કદ(kmess.km_buf))
          kmess.km_size += 1;
      kmess.km_next = (kmess.km_next + 1) % _KMESS_BUF_SIZE;
      જો(kmess.blpos == maxblpos) {
      	memmove(kmess.kmess_buf,
		kmess.kmess_buf+1, કદ(kmess.kmess_buf)-1);
      } નહીંતર kmess.blpos++;
  } નહીંતર જો (!(kinfo.minix_panicing || kinfo.do_serial_debug)) {
	send_diag_sig();
  }
}

/*===========================================================================*
 *				_exit				     	     *
 *===========================================================================*/
શૂન્ય _exit(
  પૂર્ણાંક e					/* error code */
)
{
  panic("_exit called from within the kernel, should not happen. (err %i)", e);
}
