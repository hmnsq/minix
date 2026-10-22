#include <minix/ipcconst.h>
#include <machine/asm.h>

/* Redefine _ENTRY to put the following function in the खण्डः
 * ".usermapped_glo". */
#undef _ENTRY
#define _ENTRY(x) \
	_ALIGN_TEXT; .globl x; .type x,@function; x:

.खण्डः .usermapped_glo, "wax", @progbits

/**========================================================================* */
/*                           IPC assembly routines			  * */
/**========================================================================* */
/* all message passing routines save ebx, but destroy eax and ecx. */

#define IPCFUNC(name,SETARGS,VEC,POSTTRAP)			 \
ENTRY(usermapped_ ## name ## _softint)					;\
	निक्षिप	%ebp							;\
	movl	%esp, %ebp						;\
	निक्षिप	%ebx							;\
	SETARGS								;\
	int	$VEC	/* trap to the kernel */			;\
	चालय	%ebx, %ecx	/* save %ebx */				;\
	POSTTRAP							;\
	उद्धर	%ebx							;\
	उद्धर	%ebp							;\
	प्रत्यागच्छ								;\
ENTRY(usermapped_ ## name ## _sysenter)					;\
	निक्षिप	%ebp							;\
	movl	%esp, %ebp						;\
	निक्षिप	%ebp							;\
	निक्षिप	%edx							;\
	निक्षिप	%ebx							;\
	निक्षिप	%esi							;\
	निक्षिप	%edi							;\
	movl	%esp, %esi	/* kernel uses %esi for restored %esp */;\
	movl	$0f, %edx	/* kernel uses %edx for restored %eip */;\
	movl	$VEC, %edi	/* %edi to distinguish ipc/kerncall */	;\
	/* !!! There is a dependency of proc_stacktrace()		;\
	 * on this stack layout; it needs to find %ebp on it.		;\
	 */								;\
	SETARGS			/* आह्वान-specific register setup */	;\
	sysenter		/* disappear into kernel */		;\
0:									;\
	चालय	%ebx, %ecx	/* return w. state mangled; save %ebx */;\
	उद्धर	%edi							;\
	उद्धर	%esi							;\
	उद्धर	%ebx							;\
	उद्धर	%edx							;\
	उद्धर	%ebp							;\
	POSTTRAP							;\
	उद्धर	%ebp							;\
	प्रत्यागच्छ								;\
ENTRY(usermapped_ ## name ## _syscall)					;\
	निक्षिप	%ebp							;\
	movl	%esp, %ebp						;\
	निक्षिप	%ebp							;\
	निक्षिप	%edx							;\
	निक्षिप	%ebx							;\
	निक्षिप	%esi							;\
	निक्षिप	%edi							;\
	movl	$VEC, %edi	/* %edi to distinguish ipc/kerncall */	;\
	/* !!! There is a dependency of proc_stacktrace()		;\
	 * on this stack layout; it needs to find %ebp on it.		;\
	 */								;\
	SETARGS			/* आह्वान-specific register setup */	;\
	movl	%ecx, %edx	/* %ecx is clobbered by SYSCALL */	;\
	syscall			/* disappear into kernel */		;\
	चालय	%ebx, %ecx	/* return w. state mangled; save %ebx */;\
	उद्धर	%edi							;\
	उद्धर	%esi							;\
	उद्धर	%ebx							;\
	उद्धर	%edx							;\
	उद्धर	%ebp							;\
	POSTTRAP							;\
	उद्धर	%ebp							;\
	प्रत्यागच्छ

#define IPCARGS(opcode)							\
	movl	8(%ebp), %eax	/* eax = dest-src */			;\
	movl	12(%ebp), %ebx	/* ebx = message pointer */		;\
	movl	$opcode, %ecx						;\

#define SENDA_ARGS		\
	movl	12(%ebp), %eax	/* eax = count */			;\
	movl	8(%ebp), %ebx	/* ebx = table */			;\
	movl	$SENDA, %ecx						;\

#define GETSTATUS							\
	निक्षिप	%eax							;\
	movl    16(%ebp), %eax      /* ecx = saved %ebx */		;\
	movl	%ecx,  (%eax)						;\
	उद्धर	%eax

#define KERNARGS चालय 8(%ebp), %eax

IPCFUNC(send,IPCARGS(SEND),IPCVEC_UM,)
IPCFUNC(receive,IPCARGS(RECEIVE),IPCVEC_UM,GETSTATUS)
IPCFUNC(sendrec,IPCARGS(SENDREC),IPCVEC_UM,)
IPCFUNC(sendnb,IPCARGS(SENDNB),IPCVEC_UM,)
IPCFUNC(notify,IPCARGS(NOTIFY),IPCVEC_UM,)
IPCFUNC(senda,SENDA_ARGS,IPCVEC_UM,)
IPCFUNC(do_kernel_call,KERNARGS,KERVEC_UM,)

.data
LABEL(usermapped_offset)
.space 4
