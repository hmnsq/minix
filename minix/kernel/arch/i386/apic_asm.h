#ifndef __APIC_ASM_H__
#define __APIC_ASM_H__


#ifndef __ASSEMBLY__
#અન્તર્ભાવ "kernel/kernel.h"

શૂન્ય apic_hwint0(શૂન્ય);
શૂન્ય apic_hwint1(શૂન્ય);
શૂન્ય apic_hwint2(શૂન્ય);
શૂન્ય apic_hwint3(શૂન્ય);
શૂન્ય apic_hwint4(શૂન્ય);
શૂન્ય apic_hwint5(શૂન્ય);
શૂન્ય apic_hwint6(શૂન્ય);
શૂન્ય apic_hwint7(શૂન્ય);
શૂન્ય apic_hwint8(શૂન્ય);
શૂન્ય apic_hwint9(શૂન્ય);
શૂન્ય apic_hwint10(શૂન્ય);
શૂન્ય apic_hwint11(શૂન્ય);
શૂન્ય apic_hwint12(શૂન્ય);
શૂન્ય apic_hwint13(શૂન્ય);
શૂન્ય apic_hwint14(શૂન્ય);
શૂન્ય apic_hwint15(શૂન્ય);
શૂન્ય apic_hwint16(શૂન્ય);
શૂન્ય apic_hwint17(શૂન્ય);
શૂન્ય apic_hwint18(શૂન્ય);
શૂન્ય apic_hwint19(શૂન્ય);
શૂન્ય apic_hwint20(શૂન્ય);
શૂન્ય apic_hwint21(શૂન્ય);
શૂન્ય apic_hwint22(શૂન્ય);
શૂન્ય apic_hwint23(શૂન્ય);
શૂન્ય apic_hwint24(શૂન્ય);
શૂન્ય apic_hwint25(શૂન્ય);
શૂન્ય apic_hwint26(શૂન્ય);
શૂન્ય apic_hwint27(શૂન્ય);
શૂન્ય apic_hwint28(શૂન્ય);
શૂન્ય apic_hwint29(શૂન્ય);
શૂન્ય apic_hwint30(શૂન્ય);
શૂન્ય apic_hwint31(શૂન્ય);
શૂન્ય apic_hwint32(શૂન્ય);
શૂન્ય apic_hwint33(શૂન્ય);
શૂન્ય apic_hwint34(શૂન્ય);
શૂન્ય apic_hwint35(શૂન્ય);
શૂન્ય apic_hwint36(શૂન્ય);
શૂન્ય apic_hwint37(શૂન્ય);
શૂન્ય apic_hwint38(શૂન્ય);
શૂન્ય apic_hwint39(શૂન્ય);
શૂન્ય apic_hwint40(શૂન્ય);
શૂન્ય apic_hwint41(શૂન્ય);
શૂન્ય apic_hwint42(શૂન્ય);
શૂન્ય apic_hwint43(શૂન્ય);
શૂન્ય apic_hwint44(શૂન્ય);
શૂન્ય apic_hwint45(શૂન્ય);
શૂન્ય apic_hwint46(શૂન્ય);
શૂન્ય apic_hwint47(શૂન્ય);
શૂન્ય apic_hwint48(શૂન્ય);
શૂન્ય apic_hwint49(શૂન્ય);
શૂન્ય apic_hwint50(શૂન્ય);
શૂન્ય apic_hwint51(શૂન્ય);
શૂન્ય apic_hwint52(શૂન્ય);
શૂન્ય apic_hwint53(શૂન્ય);
શૂન્ય apic_hwint54(શૂન્ય);
શૂન્ય apic_hwint55(શૂન્ય);
શૂન્ય apic_hwint56(શૂન્ય);
શૂન્ય apic_hwint57(શૂન્ય);
શૂન્ય apic_hwint58(શૂન્ય);
શૂન્ય apic_hwint59(શૂન્ય);
શૂન્ય apic_hwint60(શૂન્ય);
શૂન્ય apic_hwint61(શૂન્ય);
શૂન્ય apic_hwint62(શૂન્ય);
શૂન્ય apic_hwint63(શૂન્ય);

/* The local APIC timer tick handlers */
શૂન્ય lapic_timer_int_handler(શૂન્ય);
શૂન્ય apic_spurios_intr(શૂન્ય);
શૂન્ય apic_error_intr(શૂન્ય);

#endif

#define APIC_DEBUG

#ifdef APIC_DEBUG

#define LAPIC_INTR_DUMMY_HANDLER_SIZE	32

#ifndef __ASSEMBLY__
EXTERN અક્ષર lapic_intr_dummy_handles_start;
EXTERN અક્ષર lapic_intr_dummy_handles_end;
#endif

#endif /* APIC_DEBUG */

#endif /* __APIC_ASM_H__ */
