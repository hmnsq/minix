/* This file contains a collection of miscellaneous procedures:
 *   panic:    abort MINIX due to a fatal error
 *   kputc:          buffered putc used by kernel printf
 */

#समावेश "kernel/kernel.h"
#समावेश "arch_proto.h"

#समावेश <minix/syslib.h>
#समावेश <unistd.h>
#समावेश <stdarg.h>
#समावेश <signal.h>
#समावेश <string.h>

#समावेश <minix/sys_config.h>

#घोषणा ARE_PANICING 0xDEADC0FF

/*===========================================================================*
 *			panic                                          *
 *===========================================================================*/
व्योम panic(स्थिर अक्षर *fmt, ...)
{
  बहु_सूची arg;
  /* The system has run aground of a fatal kernel error. Terminate execution. */
  अगर (kinfo.minix_panicing == ARE_PANICING) {
  	reset();
  }
  kinfo.minix_panicing = ARE_PANICING;
  अगर (fmt != शून्य) {
	म_लिखो("kernel panic: ");
  	बहु_शुरू(arg, fmt);
	भ_लिखो(fmt, arg);
	बहु_पूर्ण(arg);
	म_लिखो("\n");
  }

  म_लिखो("kernel on CPU %d: ", cpuid);
  util_stacktrace();

#अगर 0
  अगर(get_cpulocal_var(proc_ptr)) {
	  म_लिखो("current process : ");
	  proc_stacktrace(get_cpulocal_var(proc_ptr));
  }
#पूर्ण_अगर

  /* Abort MINIX. */
  minix_shutdown(0);
}

/*===========================================================================*
 *				kputc				     	     *
 *===========================================================================*/
व्योम kputc(
  पूर्णांक c					/* character to append */
)
{
/* Accumulate a single character for a kernel message. Send a notification
 * to the output drivers if an END_OF_KMESS is encountered.
 */
  अगर (c != END_OF_KMESS) {
      पूर्णांक maxblpos = माप(kmess.kmess_buf) - 2;
#अगर_घोषित DEBUG_SERIAL
      अगर (kinfo.do_serial_debug) {
	अगर(c == '\n')
      		ser_putc('\r');
      	ser_putc(c);
      }
#पूर्ण_अगर
      kmess.km_buf[kmess.km_अग्र] = c;	/* put normal char in buffer */
      kmess.kmess_buf[kmess.blpos] = c;
      अगर (kmess.km_आकार < माप(kmess.km_buf))
          kmess.km_आकार += 1;
      kmess.km_अग्र = (kmess.km_अग्र + 1) % KMESS_BUF_आकार;
      अगर(kmess.blpos == maxblpos) {
      	स_हटाओ(kmess.kmess_buf,
		kmess.kmess_buf+1, माप(kmess.kmess_buf)-1);
      } अन्यथा kmess.blpos++;
  } अन्यथा अगर (!(kinfo.minix_panicing || kinfo.do_serial_debug)) {
	send_diag_sig();
  }
}

/*===========================================================================*
 *				_exit				     	     *
 *===========================================================================*/
व्योम _निकास(
  पूर्णांक e					/* error code */
)
{
  panic("_exit called from within the kernel, should not happen. (err %i)", e);
}
