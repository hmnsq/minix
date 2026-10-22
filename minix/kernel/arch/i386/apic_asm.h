#ifndef __APIC_ASM_H__
#define __APIC_ASM_H__


#ifndef __ASSEMBLY__
#شامل "kernel/kernel.h"

تهی apic_hwint0(تهی);
تهی apic_hwint1(تهی);
تهی apic_hwint2(تهی);
تهی apic_hwint3(تهی);
تهی apic_hwint4(تهی);
تهی apic_hwint5(تهی);
تهی apic_hwint6(تهی);
تهی apic_hwint7(تهی);
تهی apic_hwint8(تهی);
تهی apic_hwint9(تهی);
تهی apic_hwint10(تهی);
تهی apic_hwint11(تهی);
تهی apic_hwint12(تهی);
تهی apic_hwint13(تهی);
تهی apic_hwint14(تهی);
تهی apic_hwint15(تهی);
تهی apic_hwint16(تهی);
تهی apic_hwint17(تهی);
تهی apic_hwint18(تهی);
تهی apic_hwint19(تهی);
تهی apic_hwint20(تهی);
تهی apic_hwint21(تهی);
تهی apic_hwint22(تهی);
تهی apic_hwint23(تهی);
تهی apic_hwint24(تهی);
تهی apic_hwint25(تهی);
تهی apic_hwint26(تهی);
تهی apic_hwint27(تهی);
تهی apic_hwint28(تهی);
تهی apic_hwint29(تهی);
تهی apic_hwint30(تهی);
تهی apic_hwint31(تهی);
تهی apic_hwint32(تهی);
تهی apic_hwint33(تهی);
تهی apic_hwint34(تهی);
تهی apic_hwint35(تهی);
تهی apic_hwint36(تهی);
تهی apic_hwint37(تهی);
تهی apic_hwint38(تهی);
تهی apic_hwint39(تهی);
تهی apic_hwint40(تهی);
تهی apic_hwint41(تهی);
تهی apic_hwint42(تهی);
تهی apic_hwint43(تهی);
تهی apic_hwint44(تهی);
تهی apic_hwint45(تهی);
تهی apic_hwint46(تهی);
تهی apic_hwint47(تهی);
تهی apic_hwint48(تهی);
تهی apic_hwint49(تهی);
تهی apic_hwint50(تهی);
تهی apic_hwint51(تهی);
تهی apic_hwint52(تهی);
تهی apic_hwint53(تهی);
تهی apic_hwint54(تهی);
تهی apic_hwint55(تهی);
تهی apic_hwint56(تهی);
تهی apic_hwint57(تهی);
تهی apic_hwint58(تهی);
تهی apic_hwint59(تهی);
تهی apic_hwint60(تهی);
تهی apic_hwint61(تهی);
تهی apic_hwint62(تهی);
تهی apic_hwint63(تهی);

/* The local APIC timer tick handlers */
تهی lapic_timer_int_handler(تهی);
تهی apic_spurios_intr(تهی);
تهی apic_error_intr(تهی);

#endif

#define APIC_DEBUG

#ifdef APIC_DEBUG

#define LAPIC_INTR_DUMMY_HANDLER_SIZE	32

#ifndef __ASSEMBLY__
EXTERN نویسه lapic_intr_dummy_handles_start;
EXTERN نویسه lapic_intr_dummy_handles_end;
#endif

#endif /* APIC_DEBUG */

#endif /* __APIC_ASM_H__ */
