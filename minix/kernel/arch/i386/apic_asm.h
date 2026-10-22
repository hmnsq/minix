#ifndef __APIC_ASM_H__
#define __APIC_ASM_H__


#ifndef __ASSEMBLY__
#অন্তর্ভাব "kernel/kernel.h"

শূন্য apic_hwint0(শূন্য);
শূন্য apic_hwint1(শূন্য);
শূন্য apic_hwint2(শূন্য);
শূন্য apic_hwint3(শূন্য);
শূন্য apic_hwint4(শূন্য);
শূন্য apic_hwint5(শূন্য);
শূন্য apic_hwint6(শূন্য);
শূন্য apic_hwint7(শূন্য);
শূন্য apic_hwint8(শূন্য);
শূন্য apic_hwint9(শূন্য);
শূন্য apic_hwint10(শূন্য);
শূন্য apic_hwint11(শূন্য);
শূন্য apic_hwint12(শূন্য);
শূন্য apic_hwint13(শূন্য);
শূন্য apic_hwint14(শূন্য);
শূন্য apic_hwint15(শূন্য);
শূন্য apic_hwint16(শূন্য);
শূন্য apic_hwint17(শূন্য);
শূন্য apic_hwint18(শূন্য);
শূন্য apic_hwint19(শূন্য);
শূন্য apic_hwint20(শূন্য);
শূন্য apic_hwint21(শূন্য);
শূন্য apic_hwint22(শূন্য);
শূন্য apic_hwint23(শূন্য);
শূন্য apic_hwint24(শূন্য);
শূন্য apic_hwint25(শূন্য);
শূন্য apic_hwint26(শূন্য);
শূন্য apic_hwint27(শূন্য);
শূন্য apic_hwint28(শূন্য);
শূন্য apic_hwint29(শূন্য);
শূন্য apic_hwint30(শূন্য);
শূন্য apic_hwint31(শূন্য);
শূন্য apic_hwint32(শূন্য);
শূন্য apic_hwint33(শূন্য);
শূন্য apic_hwint34(শূন্য);
শূন্য apic_hwint35(শূন্য);
শূন্য apic_hwint36(শূন্য);
শূন্য apic_hwint37(শূন্য);
শূন্য apic_hwint38(শূন্য);
শূন্য apic_hwint39(শূন্য);
শূন্য apic_hwint40(শূন্য);
শূন্য apic_hwint41(শূন্য);
শূন্য apic_hwint42(শূন্য);
শূন্য apic_hwint43(শূন্য);
শূন্য apic_hwint44(শূন্য);
শূন্য apic_hwint45(শূন্য);
শূন্য apic_hwint46(শূন্য);
শূন্য apic_hwint47(শূন্য);
শূন্য apic_hwint48(শূন্য);
শূন্য apic_hwint49(শূন্য);
শূন্য apic_hwint50(শূন্য);
শূন্য apic_hwint51(শূন্য);
শূন্য apic_hwint52(শূন্য);
শূন্য apic_hwint53(শূন্য);
শূন্য apic_hwint54(শূন্য);
শূন্য apic_hwint55(শূন্য);
শূন্য apic_hwint56(শূন্য);
শূন্য apic_hwint57(শূন্য);
শূন্য apic_hwint58(শূন্য);
শূন্য apic_hwint59(শূন্য);
শূন্য apic_hwint60(শূন্য);
শূন্য apic_hwint61(শূন্য);
শূন্য apic_hwint62(শূন্য);
শূন্য apic_hwint63(শূন্য);

/* The local APIC timer tick handlers */
শূন্য lapic_timer_int_handler(শূন্য);
শূন্য apic_spurios_intr(শূন্য);
শূন্য apic_error_intr(শূন্য);

#endif

#define APIC_DEBUG

#ifdef APIC_DEBUG

#define LAPIC_INTR_DUMMY_HANDLER_SIZE	32

#ifndef __ASSEMBLY__
EXTERN বর্ণ lapic_intr_dummy_handles_start;
EXTERN বর্ণ lapic_intr_dummy_handles_end;
#endif

#endif /* APIC_DEBUG */

#endif /* __APIC_ASM_H__ */
