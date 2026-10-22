#include <machine/asm.h>

#define LD_ST_REG(reg)							;\
ENTRY(ld_##reg)							;\
	चालय	4(%esp), %eax						;\
	चालय	%eax,	%reg						;\
	प्रत्यागच्छ								;\
									;\
ENTRY(st_##reg)							;\
	चालय	%reg,	%eax						;\
	प्रत्यागच्छ

/*
 * void ld_dr0(phys_bytes value)
 * phys_bytes st_dr0(void)
 */
LD_ST_REG(dr0)
LD_ST_REG(dr1)
LD_ST_REG(dr2)
LD_ST_REG(dr3)

/*
 * void ld_dr0(unsigned long value)
 * unsigned long st_dr0(void)
 */
LD_ST_REG(dr6)
LD_ST_REG(dr7)
