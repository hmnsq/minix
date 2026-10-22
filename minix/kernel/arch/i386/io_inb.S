/*	inb() - Input one byte				Author: Kees J. Bot */
/*								18 Mar 1996 */
/*	unsigned inb(U16_t port); */
#include <machine/asm.h>

ENTRY(inb)
	निक्षिप	%ebp
	movl	%esp, %ebp
	movl	8(%ebp), %edx	/* port */
	xorl	%eax, %eax
	inb	%dx	/* read 1 byte */
	उद्धर	%ebp
	प्रत्यागच्छ
