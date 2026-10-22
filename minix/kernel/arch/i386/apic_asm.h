#ifndef __APIC_ASM_H__
#define __APIC_ASM_H__


#ifndef __ASSEMBLY__
#അന്തര്ഭാവ "kernel/kernel.h"

ശൂന്യം apic_hwint0(ശൂന്യം);
ശൂന്യം apic_hwint1(ശൂന്യം);
ശൂന്യം apic_hwint2(ശൂന്യം);
ശൂന്യം apic_hwint3(ശൂന്യം);
ശൂന്യം apic_hwint4(ശൂന്യം);
ശൂന്യം apic_hwint5(ശൂന്യം);
ശൂന്യം apic_hwint6(ശൂന്യം);
ശൂന്യം apic_hwint7(ശൂന്യം);
ശൂന്യം apic_hwint8(ശൂന്യം);
ശൂന്യം apic_hwint9(ശൂന്യം);
ശൂന്യം apic_hwint10(ശൂന്യം);
ശൂന്യം apic_hwint11(ശൂന്യം);
ശൂന്യം apic_hwint12(ശൂന്യം);
ശൂന്യം apic_hwint13(ശൂന്യം);
ശൂന്യം apic_hwint14(ശൂന്യം);
ശൂന്യം apic_hwint15(ശൂന്യം);
ശൂന്യം apic_hwint16(ശൂന്യം);
ശൂന്യം apic_hwint17(ശൂന്യം);
ശൂന്യം apic_hwint18(ശൂന്യം);
ശൂന്യം apic_hwint19(ശൂന്യം);
ശൂന്യം apic_hwint20(ശൂന്യം);
ശൂന്യം apic_hwint21(ശൂന്യം);
ശൂന്യം apic_hwint22(ശൂന്യം);
ശൂന്യം apic_hwint23(ശൂന്യം);
ശൂന്യം apic_hwint24(ശൂന്യം);
ശൂന്യം apic_hwint25(ശൂന്യം);
ശൂന്യം apic_hwint26(ശൂന്യം);
ശൂന്യം apic_hwint27(ശൂന്യം);
ശൂന്യം apic_hwint28(ശൂന്യം);
ശൂന്യം apic_hwint29(ശൂന്യം);
ശൂന്യം apic_hwint30(ശൂന്യം);
ശൂന്യം apic_hwint31(ശൂന്യം);
ശൂന്യം apic_hwint32(ശൂന്യം);
ശൂന്യം apic_hwint33(ശൂന്യം);
ശൂന്യം apic_hwint34(ശൂന്യം);
ശൂന്യം apic_hwint35(ശൂന്യം);
ശൂന്യം apic_hwint36(ശൂന്യം);
ശൂന്യം apic_hwint37(ശൂന്യം);
ശൂന്യം apic_hwint38(ശൂന്യം);
ശൂന്യം apic_hwint39(ശൂന്യം);
ശൂന്യം apic_hwint40(ശൂന്യം);
ശൂന്യം apic_hwint41(ശൂന്യം);
ശൂന്യം apic_hwint42(ശൂന്യം);
ശൂന്യം apic_hwint43(ശൂന്യം);
ശൂന്യം apic_hwint44(ശൂന്യം);
ശൂന്യം apic_hwint45(ശൂന്യം);
ശൂന്യം apic_hwint46(ശൂന്യം);
ശൂന്യം apic_hwint47(ശൂന്യം);
ശൂന്യം apic_hwint48(ശൂന്യം);
ശൂന്യം apic_hwint49(ശൂന്യം);
ശൂന്യം apic_hwint50(ശൂന്യം);
ശൂന്യം apic_hwint51(ശൂന്യം);
ശൂന്യം apic_hwint52(ശൂന്യം);
ശൂന്യം apic_hwint53(ശൂന്യം);
ശൂന്യം apic_hwint54(ശൂന്യം);
ശൂന്യം apic_hwint55(ശൂന്യം);
ശൂന്യം apic_hwint56(ശൂന്യം);
ശൂന്യം apic_hwint57(ശൂന്യം);
ശൂന്യം apic_hwint58(ശൂന്യം);
ശൂന്യം apic_hwint59(ശൂന്യം);
ശൂന്യം apic_hwint60(ശൂന്യം);
ശൂന്യം apic_hwint61(ശൂന്യം);
ശൂന്യം apic_hwint62(ശൂന്യം);
ശൂന്യം apic_hwint63(ശൂന്യം);

/* The local APIC timer tick handlers */
ശൂന്യം lapic_timer_int_handler(ശൂന്യം);
ശൂന്യം apic_spurios_intr(ശൂന്യം);
ശൂന്യം apic_error_intr(ശൂന്യം);

#endif

#define APIC_DEBUG

#ifdef APIC_DEBUG

#define LAPIC_INTR_DUMMY_HANDLER_SIZE	32

#ifndef __ASSEMBLY__
EXTERN അക്ഷരം lapic_intr_dummy_handles_start;
EXTERN അക്ഷരം lapic_intr_dummy_handles_end;
#endif

#endif /* APIC_DEBUG */

#endif /* __APIC_ASM_H__ */
