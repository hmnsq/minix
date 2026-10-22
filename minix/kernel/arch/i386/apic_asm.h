#अगर_अघोषित __APIC_ASM_H__
#घोषणा __APIC_ASM_H__


#अगर_अघोषित __ASSEMBLY__
#समावेश "kernel/kernel.h"

व्योम apic_hwint0(व्योम);
व्योम apic_hwint1(व्योम);
व्योम apic_hwint2(व्योम);
व्योम apic_hwint3(व्योम);
व्योम apic_hwint4(व्योम);
व्योम apic_hwint5(व्योम);
व्योम apic_hwint6(व्योम);
व्योम apic_hwint7(व्योम);
व्योम apic_hwint8(व्योम);
व्योम apic_hwint9(व्योम);
व्योम apic_hwint10(व्योम);
व्योम apic_hwint11(व्योम);
व्योम apic_hwint12(व्योम);
व्योम apic_hwint13(व्योम);
व्योम apic_hwint14(व्योम);
व्योम apic_hwint15(व्योम);
व्योम apic_hwint16(व्योम);
व्योम apic_hwint17(व्योम);
व्योम apic_hwint18(व्योम);
व्योम apic_hwint19(व्योम);
व्योम apic_hwint20(व्योम);
व्योम apic_hwint21(व्योम);
व्योम apic_hwint22(व्योम);
व्योम apic_hwint23(व्योम);
व्योम apic_hwint24(व्योम);
व्योम apic_hwint25(व्योम);
व्योम apic_hwint26(व्योम);
व्योम apic_hwint27(व्योम);
व्योम apic_hwint28(व्योम);
व्योम apic_hwint29(व्योम);
व्योम apic_hwint30(व्योम);
व्योम apic_hwint31(व्योम);
व्योम apic_hwint32(व्योम);
व्योम apic_hwint33(व्योम);
व्योम apic_hwint34(व्योम);
व्योम apic_hwint35(व्योम);
व्योम apic_hwint36(व्योम);
व्योम apic_hwint37(व्योम);
व्योम apic_hwint38(व्योम);
व्योम apic_hwint39(व्योम);
व्योम apic_hwint40(व्योम);
व्योम apic_hwint41(व्योम);
व्योम apic_hwint42(व्योम);
व्योम apic_hwint43(व्योम);
व्योम apic_hwint44(व्योम);
व्योम apic_hwint45(व्योम);
व्योम apic_hwint46(व्योम);
व्योम apic_hwint47(व्योम);
व्योम apic_hwint48(व्योम);
व्योम apic_hwint49(व्योम);
व्योम apic_hwint50(व्योम);
व्योम apic_hwint51(व्योम);
व्योम apic_hwint52(व्योम);
व्योम apic_hwint53(व्योम);
व्योम apic_hwint54(व्योम);
व्योम apic_hwint55(व्योम);
व्योम apic_hwint56(व्योम);
व्योम apic_hwint57(व्योम);
व्योम apic_hwint58(व्योम);
व्योम apic_hwint59(व्योम);
व्योम apic_hwint60(व्योम);
व्योम apic_hwint61(व्योम);
व्योम apic_hwint62(व्योम);
व्योम apic_hwint63(व्योम);

/* The local APIC timer tick handlers */
व्योम lapic_timer_int_handler(व्योम);
व्योम apic_spurios_intr(व्योम);
व्योम apic_error_intr(व्योम);

#पूर्ण_अगर

#घोषणा APIC_DEBUG

#अगर_घोषित APIC_DEBUG

#घोषणा LAPIC_INTR_DUMMY_HANDLER_आकार	32

#अगर_अघोषित __ASSEMBLY__
EXTERN अक्षर lapic_intr_dummy_handles_start;
EXTERN अक्षर lapic_intr_dummy_handles_end;
#पूर्ण_अगर

#पूर्ण_अगर /* APIC_DEBUG */

#पूर्ण_अगर /* __APIC_ASM_H__ */
