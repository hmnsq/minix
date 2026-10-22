/*	intr_disable(), intr_enable - Disable/Enable hardware interrupts. */
/*							Author: Kees J. Bot */
/*								18 Mar 1996 */
/*	void intr_disable(void); */
/*	void intr_enable(void); */
#include <machine/asm.h>

ENTRY(intr_disable)
	cli
	प्रत्यागच्छ

ENTRY(intr_enable)
	sti
	प्रत्यागच्छ
