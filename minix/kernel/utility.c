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

#define ARE_PANICING 0xDEADC0FF

/*===========================================================================*
 *			panic                                          *
 *===========================================================================*/
शून्य panic(स्थिरांक अक्षर *fmt, ...)
{
  बहु_सूची arg;
  /* The system has run aground of a fatal kernel error. Terminate execution. */
  जर (kinfo.minix_panicing == ARE_PANICING) {
  	reset();
  }
  kinfo.minix_panicing = ARE_PANICING;
  जर (fmt != रिक्त) {
	म_लिखो("kernel panic: ");
  	बहु_शुरू(arg, fmt);
	भ_लिखो(fmt, arg);
	बहु_पूर्ण(arg);
	म_लिखो("\n");
  }

  म_लिखो("kernel on CPU %d: ", cpuid);
  util_stacktrace();

#जर 0
  जर(get_cpulocal_var(proc_ptr)) {
	  म_लिखो("current process : ");
	  proc_stacktrace(get_cpulocal_var(proc_ptr));
  }
#endif

  /* Abort MINIX. */
  minix_shutdown(0);
}

/*===========================================================================*
 *				kputc				     	     *
 *===========================================================================*/
शून्य kputc(
  पूर्णांक c					/* character to append */
)
{
/* Accumulate a single character for a kernel message. Send a notification
 * to the output drivers if an END_OF_KMESS is encountered.
 */
  जर (c != END_OF_KMESS) {
      पूर्णांक maxblpos = आकार(kmess.kmess_buf) - 2;
#ifdef DEBUG_SERIAL
      जर (kinfo.do_serial_debug) {
	जर(c == '\n')
      		ser_putc('\r');
      	ser_putc(c);
      }
#endif
      kmess.km_buf[kmess.km_next] = c;	/* put normal char in buffer */
      kmess.kmess_buf[kmess.blpos] = c;
      जर (kmess.km_size < आकार(kmess.km_buf))
          kmess.km_size += 1;
      kmess.km_next = (kmess.km_next + 1) % _KMESS_BUF_SIZE;
      जर(kmess.blpos == maxblpos) {
      	स_हटाओ(kmess.kmess_buf,
		kmess.kmess_buf+1, आकार(kmess.kmess_buf)-1);
      } अन्यथा kmess.blpos++;
  } अन्यथा जर (!(kinfo.minix_panicing || kinfo.do_serial_debug)) {
	send_diag_sig();
  }
}

/*===========================================================================*
 *				_exit				     	     *
 *===========================================================================*/
शून्य _exit(
  पूर्णांक e					/* error code */
)
{
  panic("_exit called from within the kernel, should not happen. (err %i)", e);
}
