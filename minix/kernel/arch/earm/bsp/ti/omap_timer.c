#شامل "kernel/kernel.h"
#شامل "kernel/clock.h"
#شامل <sys/types.h>
#شامل <machine/cpu.h>
#شامل <minix/board.h>
#شامل <minix/mmio.h>
#شامل <assert.h>
#شامل <io.h>
#شامل <stdlib.h>
#شامل <stdio.h>
#شامل "arch_proto.h"
#شامل "bsp_timer.h"
#شامل "omap_timer_registers.h"
#شامل "omap_intr_registers.h"
#شامل "bsp_intr.h"

/* interrupt handler hook */
ساکن irq_hook_t omap3_timer_hook;
ساکن u64_t بلند_frc;

ساخت omap_timer_registers;

ساخت omap_timer
{
	vir_بائٹیں بنیاد;
	صحیح irq_nr;
	ساخت omap_timer_registers *regs;
};

ساخت omap_timer_registers
{
	vir_بائٹیں TIDR;
	vir_بائٹیں TIOCP_CFG;
	vir_بائٹیں TISTAT;
	vir_بائٹیں TISR;
	vir_بائٹیں TIER;
	vir_بائٹیں TWER;
	vir_بائٹیں TCLR;
	vir_بائٹیں TCRR;
	vir_بائٹیں TLDR;
	vir_بائٹیں TTGR;
	vir_بائٹیں TWPS;
	vir_بائٹیں TMAR;
	vir_بائٹیں TCAR1;
	vir_بائٹیں TSICR;
	vir_بائٹیں TCAR2;
	vir_بائٹیں TPIR;
	vir_بائٹیں TNIR;
	vir_بائٹیں TCVR;
	vir_بائٹیں TOCR;
	vir_بائٹیں TOWR;

};

ساکن ساخت omap_timer_registers regs_v1 = {
	.TIDR = OMAP3_TIMER_TIDR,
	.TIOCP_CFG = OMAP3_TIMER_TIOCP_CFG,
	.TISTAT = OMAP3_TIMER_TISTAT,
	.TISR = OMAP3_TIMER_TISR,
	.TIER = OMAP3_TIMER_TIER,
	.TWER = OMAP3_TIMER_TWER,
	.TCLR = OMAP3_TIMER_TCLR,
	.TCRR = OMAP3_TIMER_TCRR,
	.TLDR = OMAP3_TIMER_TLDR,
	.TTGR = OMAP3_TIMER_TTGR,
	.TWPS = OMAP3_TIMER_TWPS,
	.TMAR = OMAP3_TIMER_TMAR,
	.TCAR1 = OMAP3_TIMER_TCAR1,
	.TSICR = OMAP3_TIMER_TSICR,
	.TCAR2 = OMAP3_TIMER_TCAR2,
	.TPIR = OMAP3_TIMER_TPIR,
	.TNIR = OMAP3_TIMER_TNIR,
	.TCVR = OMAP3_TIMER_TCVR,
	.TOCR = OMAP3_TIMER_TOCR,
	.TOWR = OMAP3_TIMER_TOWR,
};

/* AM335X has a different ip block for the non 
 1ms timers */
ساکن ساخت omap_timer_registers regs_v2 = {
	.TIDR = AM335X_TIMER_TIDR,
	.TIOCP_CFG = AM335X_TIMER_TIOCP_CFG,
	.TISTAT = AM335X_TIMER_IRQSTATUS_RAW,
	.TISR = AM335X_TIMER_IRQSTATUS,
	.TIER = AM335_X_TIMER_IRQENABLE_سیٹ,
	.TWER = AM335X_TIMER_IRQWAKEEN,
	.TCLR = AM335X_TIMER_TCLR,
	.TCRR = AM335X_TIMER_TCRR,
	.TLDR = AM335X_TIMER_TLDR,
	.TTGR = AM335X_TIMER_TTGR,
	.TWPS = AM335X_TIMER_TWPS,
	.TMAR = AM335X_TIMER_TMAR,
	.TCAR1 = AM335X_TIMER_TCAR1,
	.TSICR = AM335X_TIMER_TSICR,
	.TCAR2 = AM335X_TIMER_TCAR2,
	.TPIR = -1,		/* UNDEF */
	.TNIR = -1,		/* UNDEF */
	.TCVR = -1,		/* UNDEF */
	.TOCR = -1,		/* UNDEF */
	.TOWR = -1		/* UNDEF */
};

ساکن ساخت omap_timer dm37xx_timer = {
	.بنیاد = OMAP3_GPTIMER1_بنیاد,
	.irq_nr = OMAP3_GPT1_IRQ,
	.regs = &regs_v1
};

/* free running timer */
ساکن ساخت omap_timer dm37xx_fr_timer = {
	.بنیاد = OMAP3_GPTIMER10_بنیاد,
	.irq_nr = OMAP3_GPT10_IRQ,
	.regs = &regs_v1
};

/* normal timer */
ساکن ساخت omap_timer am335x_timer = {
	.بنیاد = AM335_X_DMTIMER1_1_MS_بنیاد,
	.irq_nr = AM335X_INT_TINT1_1MS,
	.regs = &regs_v1
};

/* free running timer */
ساکن ساخت omap_timer am335x_fr_timer = {
	.بنیاد = AM335_X_DMTIMER7_بنیاد,
	.irq_nr = AM335X_INT_TINT7,
	.regs = &regs_v2
};

ساکن ساخت omap_timer *timer;
ساکن ساخت omap_timer *fr_timer;

ساکن صحیح مکمل = 0;

صحیح
bsp_رجسٹر_timer_handler(ثابت irq_handler_t handler)
{
	/* Initialize the CLOCK's interrupt hook. */
	omap3_timer_hook.proc_nr_e = NONE;
	omap3_timer_hook.irq = timer->irq_nr;

	put_irq_handler(&omap3_timer_hook, timer->irq_nr, handler);
	/* only unmask interrupts after registering */
	bsp_irq_unmask(timer->irq_nr);

	واپس 0;
}

/* meta data for remapping */
ساکن kern_phys_map timer_phys_map;
ساکن kern_phys_map fr_timer_phys_map;
ساکن kern_phys_map fr_timer_user_phys_map;	/* struct for when the free */
						/* running timer is mapped to */
						/* userland */

/* callback for when the free running clock gets mapped */
صحیح
kern_phys_fr_user_mapped(vir_بائٹیں id, phys_بائٹیں پتہ)
{
	/* the only thing we need to do at this stage is to set the address */
	/* in the kerninfo struct */
	اگر (BOARD_IS_BBXM(machine.board_id)) {
		arm_frclock.tcrr = پتہ + OMAP3_TIMER_TCRR;
		arm_frclock.hz = 1625000;
	} ورنہ اگر (BOARD_IS_BB(machine.board_id)) {
		arm_frclock.tcrr = پتہ + AM335X_TIMER_TCRR;
		arm_frclock.hz = 1500000;
	}
	واپس 0;
}

خالی
omap3_frclock_init(خالی)
{
	u32_t tisr;

	/* enable the clock */
	اگر (BOARD_IS_BBXM(machine.board_id)) {
		fr_timer = &dm37xx_fr_timer;

		kern_phys_map_ptr(fr_timer->بنیاد, ARM_صفحہ_سائز,
		    VMMF_UNCACHED | VMMF_WRITE, &fr_timer_phys_map,
		    (vir_بائٹیں) & fr_timer->بنیاد);

		/* the timer is also mapped in user space hence the this */
		/* second mapping and callback to set kerninfo frclock_tcrr */
		kern_req_phys_map(fr_timer->بنیاد, ARM_صفحہ_سائز,
		    VMMF_UNCACHED | VMMF_USER,
		    &fr_timer_user_phys_map, kern_phys_fr_user_mapped, 0);

		/* Stop timer */
		mmio_clear(fr_timer->بنیاد + fr_timer->regs->TCLR,
		    OMAP3_TCLR_ST);

		/* Use functional clock source for GPTIMER10 */
		mmio_سیٹ(OMAP3_CM_CLKSEL_CORE, OMAP3_CLKSEL_GPT10);

		/* Scale timer down to 13/8 = 1.625 Mhz to roughly get
		 * microsecond ticks */
		/* The scale is computed as 2^(PTV+1). So if PTV == 2, we get
		 * 2^3 = 8. */
		mmio_سیٹ(fr_timer->بنیاد + fr_timer->regs->TCLR,
		    (2 << OMAP3_TCLR_PTV));
	} ورنہ اگر (BOARD_IS_BB(machine.board_id)) {
		fr_timer = &am335x_fr_timer;
		kern_phys_map_ptr(fr_timer->بنیاد, ARM_صفحہ_سائز,
		    VMMF_UNCACHED | VMMF_WRITE,
		    &fr_timer_phys_map, (vir_بائٹیں) & fr_timer->بنیاد);

		/* the timer is also mapped in user space hence the this */
		/* second mapping and callback to set kerninfo frclock_tcrr */
		kern_req_phys_map(fr_timer->بنیاد, ARM_صفحہ_سائز,
		    VMMF_UNCACHED | VMMF_USER,
		    &fr_timer_user_phys_map, kern_phys_fr_user_mapped, 0);
		/* Disable the module and wait for the module to be disabled */
		set32(CM_فی_TIMER7_CLKCTRL, CM_MODULEMODE_نقاب,
		    CM_MODULEMODE_DISABLED);
		جب ((mmio_read(CM_فی_TIMER7_CLKCTRL) & CM_CLKCTRL_IDLEST)
		    != CM_CLKCTRL_IDLEST_DISABLE);

		set32(CLKSEL_TIMER7_CLK, CLKSEL_TIMER7_CLK_SEL_نقاب,
		    CLKSEL_TIMER7_CLK_SEL_SEL2);
		جب ((read32(CLKSEL_TIMER7_CLK) & CLKSEL_TIMER7_CLK_SEL_نقاب)
		    != CLKSEL_TIMER7_CLK_SEL_SEL2);

		/* enable the module and wait for the module to be ready */
		set32(CM_فی_TIMER7_CLKCTRL, CM_MODULEMODE_نقاب,
		    CM_MODULEMODE_ENABLE);
		جب ((mmio_read(CM_فی_TIMER7_CLKCTRL) & CM_CLKCTRL_IDLEST)
		    != CM_CLKCTRL_IDLEST_FUNC);

		/* Stop timer */
		mmio_clear(fr_timer->بنیاد + fr_timer->regs->TCLR,
		    OMAP3_TCLR_ST);

		/* 24Mhz / 16 = 1.5 Mhz */
		mmio_سیٹ(fr_timer->بنیاد + fr_timer->regs->TCLR,
		    (3 << OMAP3_TCLR_PTV));
	}

	/* Start and auto-reload at 0 */
	mmio_write(fr_timer->بنیاد + fr_timer->regs->TLDR, 0x0);
	mmio_write(fr_timer->بنیاد + fr_timer->regs->TCRR, 0x0);

	/* Set up overflow interrupt */
	tisr = OMAP3_TISR_MAT_IT_FLAG | OMAP3_TISR_OVF_IT_FLAG |
	    OMAP3_TISR_TCAR_IT_FLAG;
	/* Clear interrupt status */
	mmio_write(fr_timer->بنیاد + fr_timer->regs->TISR, tisr);
	mmio_write(fr_timer->بنیاد + fr_timer->regs->TIER,
	    OMAP3_TIER_OVF_IT_ENA);

	/* Start timer */
	mmio_سیٹ(fr_timer->بنیاد + fr_timer->regs->TCLR,
	    OMAP3_TCLR_OVF_TRG | OMAP3_TCLR_AR | OMAP3_TCLR_ST |
	    OMAP3_TCLR_PRE);
	مکمل = 1;
}

خالی
omap3_frclock_stop(خالی)
{
	mmio_clear(fr_timer->بنیاد + fr_timer->regs->TCLR, OMAP3_TCLR_ST);
}

خالی
bsp_timer_init(غیرعلامتی freq)
{
	/* we only support 1ms resolution */
	u32_t tisr;
	اگر (BOARD_IS_BBXM(machine.board_id)) {
		timer = &dm37xx_timer;
		kern_phys_map_ptr(timer->بنیاد, ARM_صفحہ_سائز,
		    VMMF_UNCACHED | VMMF_WRITE,
		    &timer_phys_map, (vir_بائٹیں) & timer->بنیاد);
		/* Stop timer */
		mmio_clear(timer->بنیاد + timer->regs->TCLR, OMAP3_TCLR_ST);

		/* Use 32 KHz clock source for GPTIMER1 */
		mmio_clear(OMAP3_CM_CLKSEL_WKUP, OMAP3_CLKSEL_GPT1);
	} ورنہ اگر (BOARD_IS_BB(machine.board_id)) {
		timer = &am335x_timer;
		kern_phys_map_ptr(timer->بنیاد, ARM_صفحہ_سائز,
		    VMMF_UNCACHED | VMMF_WRITE,
		    &timer_phys_map, (vir_بائٹیں) & timer->بنیاد);
		/* disable the module and wait for the module to be disabled */
		set32(CM_WKUP_TIMER1_CLKCTRL, CM_MODULEMODE_نقاب,
		    CM_MODULEMODE_DISABLED);
		جب ((mmio_read(CM_WKUP_TIMER1_CLKCTRL) & CM_CLKCTRL_IDLEST)
		    != CM_CLKCTRL_IDLEST_DISABLE);

		set32(CLKSEL_TIMER1MS_CLK, CLKSEL_TIMER1_MS_CLK_SEL_نقاب,
		    CLKSEL_TIMER1MS_CLK_SEL_SEL2);
		جب ((read32(CLKSEL_TIMER1MS_CLK) &
			CLKSEL_TIMER1_MS_CLK_SEL_نقاب) !=
		    CLKSEL_TIMER1MS_CLK_SEL_SEL2);

		/* enable the module and wait for the module to be ready */
		set32(CM_WKUP_TIMER1_CLKCTRL, CM_MODULEMODE_نقاب,
		    CM_MODULEMODE_ENABLE);
		جب ((mmio_read(CM_WKUP_TIMER1_CLKCTRL) & CM_CLKCTRL_IDLEST)
		    != CM_CLKCTRL_IDLEST_FUNC);

		/* Stop timer */
		mmio_clear(timer->بنیاد + timer->regs->TCLR, OMAP3_TCLR_ST);
	}

	/* Use 1-ms tick mode for GPTIMER1 TRM 16.2.4.2.1 */
	mmio_write(timer->بنیاد + timer->regs->TPIR, 232000);
	mmio_write(timer->بنیاد + timer->regs->TNIR, -768000);
	mmio_write(timer->بنیاد + timer->regs->TLDR,
	    0xffffffff - (32768 / freq) + 1);
	mmio_write(timer->بنیاد + timer->regs->TCRR,
	    0xffffffff - (32768 / freq) + 1);

	/* Set up overflow interrupt */
	tisr = OMAP3_TISR_MAT_IT_FLAG | OMAP3_TISR_OVF_IT_FLAG |
	    OMAP3_TISR_TCAR_IT_FLAG;
	/* Clear interrupt status */
	mmio_write(timer->بنیاد + timer->regs->TISR, tisr);
	mmio_write(timer->بنیاد + timer->regs->TIER, OMAP3_TIER_OVF_IT_ENA);

	/* Start timer */
	mmio_سیٹ(timer->بنیاد + timer->regs->TCLR,
	    OMAP3_TCLR_OVF_TRG | OMAP3_TCLR_AR | OMAP3_TCLR_ST);
	/* also initilize the free runnning timer */
	omap3_frclock_init();
}

خالی
bsp_timer_stop(خالی)
{
	mmio_clear(timer->بنیاد + timer->regs->TCLR, OMAP3_TCLR_ST);
}

ساکن u32_t
read_frc(خالی)
{
	اگر (مکمل == 0) {
		واپس 0;
	}
	واپس mmio_read(fr_timer->بنیاد + fr_timer->regs->TCRR);
}

/*
 * Check if the free running clock has overflown and
 * increase the high free running clock counter if
 * so. This method takes the current timer value as
 * parameter to ensure the overflow check is done
 * on the current timer value.
 *
 * To compose the current timer value (64 bits) you
 * need to follow the following sequence:
 *  read the current timer value.
 *  call the overflow check
 *  compose the 64 bits time based on the current timer value
 *   and high_frc.
 */
ساکن خالی
frc_حد_سے_تجاوز_check(u32_t cur_frc)
{
	ساکن صحیح پچھلا_frc_valid;
	ساکن u32_t پچھلا_frc;
	اگر (پچھلا_frc_valid && پچھلا_frc > cur_frc) {
		بلند_frc++;
	}
	پچھلا_frc = cur_frc;
	پچھلا_frc_valid = 1;
}

خالی
bsp_timer_int_handler(خالی)
{
	/* Clear all interrupts */
	u32_t tisr, now;

	/* when the kernel itself is running interrupts are disabled. We
	 * should therefore also read the overflow counter to detect this as
	 * to not miss events. */
	tisr = OMAP3_TISR_MAT_IT_FLAG | OMAP3_TISR_OVF_IT_FLAG |
	    OMAP3_TISR_TCAR_IT_FLAG;
	mmio_write(timer->بنیاد + timer->regs->TISR, tisr);

	now = read_frc();
	frc_حد_سے_تجاوز_check(now);
}

/* Use the free running clock as TSC */
خالی
read_tsc_64(u64_t * t)
{
	u32_t now;
	now = read_frc();
	frc_حد_سے_تجاوز_check(now);
	*t = (u64_t) now + (بلند_frc << 32);
}
