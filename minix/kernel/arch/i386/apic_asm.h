#ifndef __APIC_ASM_H__
#define __APIC_ASM_H__


#ifndef __ASSEMBLY__
#அந்தர்பாவ "kernel/kernel.h"

வெறுமை apic_hwint0(வெறுமை);
வெறுமை apic_hwint1(வெறுமை);
வெறுமை apic_hwint2(வெறுமை);
வெறுமை apic_hwint3(வெறுமை);
வெறுமை apic_hwint4(வெறுமை);
வெறுமை apic_hwint5(வெறுமை);
வெறுமை apic_hwint6(வெறுமை);
வெறுமை apic_hwint7(வெறுமை);
வெறுமை apic_hwint8(வெறுமை);
வெறுமை apic_hwint9(வெறுமை);
வெறுமை apic_hwint10(வெறுமை);
வெறுமை apic_hwint11(வெறுமை);
வெறுமை apic_hwint12(வெறுமை);
வெறுமை apic_hwint13(வெறுமை);
வெறுமை apic_hwint14(வெறுமை);
வெறுமை apic_hwint15(வெறுமை);
வெறுமை apic_hwint16(வெறுமை);
வெறுமை apic_hwint17(வெறுமை);
வெறுமை apic_hwint18(வெறுமை);
வெறுமை apic_hwint19(வெறுமை);
வெறுமை apic_hwint20(வெறுமை);
வெறுமை apic_hwint21(வெறுமை);
வெறுமை apic_hwint22(வெறுமை);
வெறுமை apic_hwint23(வெறுமை);
வெறுமை apic_hwint24(வெறுமை);
வெறுமை apic_hwint25(வெறுமை);
வெறுமை apic_hwint26(வெறுமை);
வெறுமை apic_hwint27(வெறுமை);
வெறுமை apic_hwint28(வெறுமை);
வெறுமை apic_hwint29(வெறுமை);
வெறுமை apic_hwint30(வெறுமை);
வெறுமை apic_hwint31(வெறுமை);
வெறுமை apic_hwint32(வெறுமை);
வெறுமை apic_hwint33(வெறுமை);
வெறுமை apic_hwint34(வெறுமை);
வெறுமை apic_hwint35(வெறுமை);
வெறுமை apic_hwint36(வெறுமை);
வெறுமை apic_hwint37(வெறுமை);
வெறுமை apic_hwint38(வெறுமை);
வெறுமை apic_hwint39(வெறுமை);
வெறுமை apic_hwint40(வெறுமை);
வெறுமை apic_hwint41(வெறுமை);
வெறுமை apic_hwint42(வெறுமை);
வெறுமை apic_hwint43(வெறுமை);
வெறுமை apic_hwint44(வெறுமை);
வெறுமை apic_hwint45(வெறுமை);
வெறுமை apic_hwint46(வெறுமை);
வெறுமை apic_hwint47(வெறுமை);
வெறுமை apic_hwint48(வெறுமை);
வெறுமை apic_hwint49(வெறுமை);
வெறுமை apic_hwint50(வெறுமை);
வெறுமை apic_hwint51(வெறுமை);
வெறுமை apic_hwint52(வெறுமை);
வெறுமை apic_hwint53(வெறுமை);
வெறுமை apic_hwint54(வெறுமை);
வெறுமை apic_hwint55(வெறுமை);
வெறுமை apic_hwint56(வெறுமை);
வெறுமை apic_hwint57(வெறுமை);
வெறுமை apic_hwint58(வெறுமை);
வெறுமை apic_hwint59(வெறுமை);
வெறுமை apic_hwint60(வெறுமை);
வெறுமை apic_hwint61(வெறுமை);
வெறுமை apic_hwint62(வெறுமை);
வெறுமை apic_hwint63(வெறுமை);

/* The local APIC timer tick handlers */
வெறுமை lapic_timer_int_handler(வெறுமை);
வெறுமை apic_spurios_intr(வெறுமை);
வெறுமை apic_error_intr(வெறுமை);

#endif

#define APIC_DEBUG

#ifdef APIC_DEBUG

#define LAPIC_INTR_DUMMY_HANDLER_SIZE	32

#ifndef __ASSEMBLY__
EXTERN எழுத்து lapic_intr_dummy_handles_start;
EXTERN எழுத்து lapic_intr_dummy_handles_end;
#endif

#endif /* APIC_DEBUG */

#endif /* __APIC_ASM_H__ */
