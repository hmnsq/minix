#اگر_غیر_تعریف_شدہ __APIC_ASM_H__
#تعریف_کرو __APIC_ASM_H__


#اگر_غیر_تعریف_شدہ __ASSEMBLY__
#شامل "kernel/kernel.h"

خالی apic_hwint0(خالی);
خالی apic_hwint1(خالی);
خالی apic_hwint2(خالی);
خالی apic_hwint3(خالی);
خالی apic_hwint4(خالی);
خالی apic_hwint5(خالی);
خالی apic_hwint6(خالی);
خالی apic_hwint7(خالی);
خالی apic_hwint8(خالی);
خالی apic_hwint9(خالی);
خالی apic_hwint10(خالی);
خالی apic_hwint11(خالی);
خالی apic_hwint12(خالی);
خالی apic_hwint13(خالی);
خالی apic_hwint14(خالی);
خالی apic_hwint15(خالی);
خالی apic_hwint16(خالی);
خالی apic_hwint17(خالی);
خالی apic_hwint18(خالی);
خالی apic_hwint19(خالی);
خالی apic_hwint20(خالی);
خالی apic_hwint21(خالی);
خالی apic_hwint22(خالی);
خالی apic_hwint23(خالی);
خالی apic_hwint24(خالی);
خالی apic_hwint25(خالی);
خالی apic_hwint26(خالی);
خالی apic_hwint27(خالی);
خالی apic_hwint28(خالی);
خالی apic_hwint29(خالی);
خالی apic_hwint30(خالی);
خالی apic_hwint31(خالی);
خالی apic_hwint32(خالی);
خالی apic_hwint33(خالی);
خالی apic_hwint34(خالی);
خالی apic_hwint35(خالی);
خالی apic_hwint36(خالی);
خالی apic_hwint37(خالی);
خالی apic_hwint38(خالی);
خالی apic_hwint39(خالی);
خالی apic_hwint40(خالی);
خالی apic_hwint41(خالی);
خالی apic_hwint42(خالی);
خالی apic_hwint43(خالی);
خالی apic_hwint44(خالی);
خالی apic_hwint45(خالی);
خالی apic_hwint46(خالی);
خالی apic_hwint47(خالی);
خالی apic_hwint48(خالی);
خالی apic_hwint49(خالی);
خالی apic_hwint50(خالی);
خالی apic_hwint51(خالی);
خالی apic_hwint52(خالی);
خالی apic_hwint53(خالی);
خالی apic_hwint54(خالی);
خالی apic_hwint55(خالی);
خالی apic_hwint56(خالی);
خالی apic_hwint57(خالی);
خالی apic_hwint58(خالی);
خالی apic_hwint59(خالی);
خالی apic_hwint60(خالی);
خالی apic_hwint61(خالی);
خالی apic_hwint62(خالی);
خالی apic_hwint63(خالی);

/* The local APIC timer tick handlers */
خالی lapic_timer_int_handler(خالی);
خالی apic_spurios_intr(خالی);
خالی apic_error_intr(خالی);

#اگر_ختم

#تعریف_کرو APIC_DEBUG

#اگر_تعریف_شدہ APIC_DEBUG

#تعریف_کرو LAPIC_INTR_DUMMY_HANDLER_سائز	32

#اگر_غیر_تعریف_شدہ __ASSEMBLY__
EXTERN حرف lapic_intr_dummy_handles_آغاز;
EXTERN حرف lapic_intr_dummy_handles_انتہا;
#اگر_ختم

#اگر_ختم /* APIC_DEBUG */

#اگر_ختم /* __APIC_ASM_H__ */
