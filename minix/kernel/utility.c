/* This file contains a collection of miscellaneous procedures:
 *   panic:    abort MINIX due to a fatal error
 *   kputc:          buffered putc used by kernel printf
 */

#अन्तर्भाव "kernel/kernel.h"
#अन्तर्भाव "arch_proto.h"

#अन्तर्भाव <minix/syslib.h>
#अन्तर्भाव <unistd.h>
#अन्तर्भाव <stdarg.h>
#अन्तर्भाव <signal.h>
#अन्तर्भाव <string.h>

#अन्तर्भाव <minix/sys_config.h>

#परिभाषय ARE_PANICING 0xDEADC0FF

/*===========================================================================*
 *			panic                                          *
 *===========================================================================*/
शून्य panic(ध्रुव वर्ण *fmt, ...)
{
  va_list arg;
  /* The system has run aground of a fatal kernel error. Terminate execution. */
  यदि (kinfo.minix_panicing == ARE_PANICING) {
  	reset();
  }
  kinfo.minix_panicing = ARE_PANICING;
  यदि (fmt != रिक्त) {
	प्रारूप_लिख("kernel panic: ");
  	va_start(arg, fmt);
	vprintf(fmt, arg);
	va_अन्तः(arg);
	प्रारूप_लिख("\n");
  }

  प्रारूप_लिख("kernel on CPU %d: ", cpuid);
  util_stacktrace();

#यदि 0
  यदि(get_cpulocal_var(proc_ptr)) {
	  प्रारूप_लिख("current process : ");
	  proc_stacktrace(get_cpulocal_var(proc_ptr));
  }
#यदि_अन्तः

  /* Abort MINIX. */
  minix_shutdown(0);
}

/*===========================================================================*
 *				kputc				     	     *
 *===========================================================================*/
शून्य kputc(
  पूर्णाङ्क c					/* character to append */
)
{
/* Accumulate a single character for a kernel message. Send a notification
 * to the output drivers if an END_OF_KMESS is encountered.
 */
  यदि (c != अन्तः_सम्बन्धि_KMESS) {
      पूर्णाङ्क maxblpos = परिमाण(kmess.kmess_buf) - 2;
#यदि_परिभाषितम् DEBUG_SERIAL
      यदि (kinfo.do_serial_debug) {
	यदि(c == '\n')
      		ser_putc('\r');
      	ser_putc(c);
      }
#यदि_अन्तः
      kmess.km_buf[kmess.km_अग्र] = c;	/* put normal char in buffer */
      kmess.kmess_buf[kmess.blpos] = c;
      यदि (kmess.km_आकार < परिमाण(kmess.km_buf))
          kmess.km_आकार += 1;
      kmess.km_अग्र = (kmess.km_अग्र + 1) % KMESS_BUF_आकार;
      यदि(kmess.blpos == maxblpos) {
      	memmove(kmess.kmess_buf,
		kmess.kmess_buf+1, परिमाण(kmess.kmess_buf)-1);
      } अन्यथा kmess.blpos++;
  } अन्यथा यदि (!(kinfo.minix_panicing || kinfo.do_serial_debug)) {
	send_diag_sig();
  }
}

/*===========================================================================*
 *				_exit				     	     *
 *===========================================================================*/
शून्य _exit(
  पूर्णाङ्क e					/* error code */
)
{
  panic("_exit called from within the kernel, should not happen. (err %i)", e);
}
