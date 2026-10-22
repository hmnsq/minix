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

#ವ್ಯಾಖ್ಯಾನಿಸು ARE_PANICING 0xDEADC0FF

/*===========================================================================*
 *			panic                                          *
 *===========================================================================*/
ಶೂನ್ಯ panic(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fmt, ...)
{
  va_list arg;
  /* The system has run aground of a fatal kernel error. Terminate execution. */
  ಆದರೆ (kinfo.minix_panicing == ARE_PANICING) {
  	reset();
  }
  kinfo.minix_panicing = ARE_PANICING;
  ಆದರೆ (fmt != ಖಾಲಿ) {
	ಬರೆ("kernel panic: ");
  	va_ಆರಂಭ(arg, fmt);
	vprintf(fmt, arg);
	va_ಕೊನೆ(arg);
	ಬರೆ("\n");
  }

  ಬರೆ("kernel on CPU %d: ", cpuid);
  util_stacktrace();

#ಆದರೆ 0
  ಆದರೆ(get_cpulocal_var(proc_ptr)) {
	  ಬರೆ("current process : ");
	  proc_stacktrace(get_cpulocal_var(proc_ptr));
  }
#ಆದರೆ_ಅಂತ್ಯ

  /* Abort MINIX. */
  minix_shutdown(0);
}

/*===========================================================================*
 *				kputc				     	     *
 *===========================================================================*/
ಶೂನ್ಯ kputc(
  ಪೂರ್ಣಾಂಕ c					/* character to append */
)
{
/* Accumulate a single character for a kernel message. Send a notification
 * to the output drivers if an END_OF_KMESS is encountered.
 */
  ಆದರೆ (c != ಕೊನೆ_ದ_KMESS) {
      ಪೂರ್ಣಾಂಕ maxblpos = ಗಾತ್ರ(kmess.kmess_buf) - 2;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUG_SERIAL
      ಆದರೆ (kinfo.do_serial_debug) {
	ಆದರೆ(c == '\n')
      		ser_putc('\r');
      	ser_putc(c);
      }
#ಆದರೆ_ಅಂತ್ಯ
      kmess.km_buf[kmess.km_ಮುಂದಿನ] = c;	/* put normal char in buffer */
      kmess.kmess_buf[kmess.blpos] = c;
      ಆದರೆ (kmess.km_ಗಾತ್ರ < ಗಾತ್ರ(kmess.km_buf))
          kmess.km_ಗಾತ್ರ += 1;
      kmess.km_ಮುಂದಿನ = (kmess.km_ಮುಂದಿನ + 1) % KMESS_BUF_ಗಾತ್ರ;
      ಆದರೆ(kmess.blpos == maxblpos) {
      	memmove(kmess.kmess_buf,
		kmess.kmess_buf+1, ಗಾತ್ರ(kmess.kmess_buf)-1);
      } ಇಲ್ಲದಿದ್ದರೆ kmess.blpos++;
  } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!(kinfo.minix_panicing || kinfo.do_serial_debug)) {
	send_diag_sig();
  }
}

/*===========================================================================*
 *				_exit				     	     *
 *===========================================================================*/
ಶೂನ್ಯ _exit(
  ಪೂರ್ಣಾಂಕ e					/* error code */
)
{
  panic("_exit called from within the kernel, should not happen. (err %i)", e);
}
