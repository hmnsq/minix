#ضمن "kernel/kernel.h"
#ضمن "kernel/clock.h"
#ضمن <sys/types.h>
#ضمن <machine/cpu.h>
#ضمن <minix/board.h>
#ضمن <minix/mmio.h>
#ضمن <assert.h>
#ضمن <io.h>
#ضمن <stdlib.h>
#ضمن <stdio.h>
#ضمن "arch_proto.h"
#ضمن "bsp_timer.h"
#ضمن "omap_timer_registers.h"
#ضمن "omap_intr_registers.h"
#ضمن "bsp_intr.h"

/* interrupt handler hook */
ساكن irq_hook_t omap3_timer_hook;
ساكن u64_t بلند_frc;

بنية omap_timer_registers;

بنية omap_timer
{
	vir_بائٹاں ادھار;
	صحيح irq_nr;
	بنية omap_timer_registers *regs;
};

بنية omap_timer_registers
{
	vir_بائٹاں TIDR;
	vir_بائٹاں TIOCP_CFG;
	vir_بائٹاں TISTAT;
	vir_بائٹاں TISR;
	vir_بائٹاں TIER;
	vir_بائٹاں TWER;
	vir_بائٹاں TCLR;
	vir_بائٹاں TCRR;
	vir_بائٹاں TLDR;
	vir_بائٹاں TTGR;
	vir_بائٹاں TWPS;
	vir_بائٹاں TMAR;
	vir_بائٹاں TCAR1;
	vir_بائٹاں TSICR;
	vir_بائٹاں TCAR2;
	vir_بائٹاں TPIR;
	vir_بائٹاں TNIR;
	vir_بائٹاں TCVR;
	vir_بائٹاں TOCR;
	vir_بائٹاں TOWR;

};

ساكن بنية omap_timer_registers regs_v1 = {
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
ساكن بنية omap_timer_registers regs_v2 = {
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

ساكن بنية omap_timer dm37xx_timer = {
	.ادھار = OMAP3_GPTIMER1_ادھار,
	.irq_nr = OMAP3_GPT1_IRQ,
	.regs = &regs_v1
};

/* free running timer */
ساكن بنية omap_timer dm37xx_fr_timer = {
	.ادھار = OMAP3_GPTIMER10_ادھار,
	.irq_nr = OMAP3_GPT10_IRQ,
	.regs = &regs_v1
};

/* normal timer */
ساكن بنية omap_timer am335x_timer = {
	.ادھار = AM335_X_DMTIMER1_1_MS_ادھار,
	.irq_nr = AM335X_INT_TINT1_1MS,
	.regs = &regs_v1
};

/* free running timer */
ساكن بنية omap_timer am335x_fr_timer = {
	.ادھار = AM335_X_DMTIMER7_ادھار,
	.irq_nr = AM335X_INT_TINT7,
	.regs = &regs_v2
};

ساكن بنية omap_timer *timer;
ساكن بنية omap_timer *fr_timer;

ساكن صحيح پورا = 0;

صحيح
bsp_رجسٹر_timer_handler(ثابت irq_handler_t handler)
{
	/* Initialize the CLOCK's interrupt hook. */
	omap3_timer_hook.proc_nr_e = NONE;
	omap3_timer_hook.irq = timer->irq_nr;

	put_irq_handler(&omap3_timer_hook, timer->irq_nr, handler);
	/* only unmask interrupts after registering */
	bsp_irq_unmask(timer->irq_nr);

	أرجع 0;
}

/* meta data for remapping */
ساكن kern_phys_map timer_phys_map;
ساكن kern_phys_map fr_timer_phys_map;
ساكن kern_phys_map fr_timer_user_phys_map;	/* struct for when the free */
						/* running timer is mapped to */
						/* userland */

/* callback for when the free running clock gets mapped */
صحيح
kern_phys_fr_user_mapped(vir_بائٹاں id, phys_بائٹاں پتہ)
{
	/* the only thing we need to do at this stage is to set the address */
	/* in the kerninfo struct */
	إذا (BOARD_IS_BBXM(machine.board_id)) {
		arm_frclock.tcrr = پتہ + OMAP3_TIMER_TCRR;
		arm_frclock.hz = 1625000;
	} وإلا إذا (BOARD_IS_BB(machine.board_id)) {
		arm_frclock.tcrr = پتہ + AM335X_TIMER_TCRR;
		arm_frclock.hz = 1500000;
	}
	أرجع 0;
}

فراغ
omap3_frclock_init(فراغ)
{
	u32_t tisr;

	/* enable the clock */
	إذا (BOARD_IS_BBXM(machine.board_id)) {
		fr_timer = &dm37xx_fr_timer;

		kern_phys_map_ptr(fr_timer->ادھار, ARM_ورقہ_ناپ,
		    VMMF_UNCACHED | VMMF_WRITE, &fr_timer_phys_map,
		    (vir_بائٹاں) & fr_timer->ادھار);

		/* the timer is also mapped in user space hence the this */
		/* second mapping and callback to set kerninfo frclock_tcrr */
		kern_req_phys_map(fr_timer->ادھار, ARM_ورقہ_ناپ,
		    VMMF_UNCACHED | VMMF_USER,
		    &fr_timer_user_phys_map, kern_phys_fr_user_mapped, 0);

		/* Stop timer */
		mmio_clear(fr_timer->ادھار + fr_timer->regs->TCLR,
		    OMAP3_TCLR_ST);

		/* Use functional clock source for GPTIMER10 */
		mmio_سیٹ(OMAP3_CM_CLKSEL_CORE, OMAP3_CLKSEL_GPT10);

		/* Scale timer down to 13/8 = 1.625 Mhz to roughly get
		 * microsecond ticks */
		/* The scale is computed as 2^(PTV+1). So if PTV == 2, we get
		 * 2^3 = 8. */
		mmio_سیٹ(fr_timer->ادھار + fr_timer->regs->TCLR,
		    (2 << OMAP3_TCLR_PTV));
	} وإلا إذا (BOARD_IS_BB(machine.board_id)) {
		fr_timer = &am335x_fr_timer;
		kern_phys_map_ptr(fr_timer->ادھار, ARM_ورقہ_ناپ,
		    VMMF_UNCACHED | VMMF_WRITE,
		    &fr_timer_phys_map, (vir_بائٹاں) & fr_timer->ادھار);

		/* the timer is also mapped in user space hence the this */
		/* second mapping and callback to set kerninfo frclock_tcrr */
		kern_req_phys_map(fr_timer->ادھار, ARM_ورقہ_ناپ,
		    VMMF_UNCACHED | VMMF_USER,
		    &fr_timer_user_phys_map, kern_phys_fr_user_mapped, 0);
		/* Disable the module and wait for the module to be disabled */
		set32(CM_فی_TIMER7_CLKCTRL, CM_MODULEMODE_نقاب,
		    CM_MODULEMODE_DISABLED);
		طالما ((mmio_read(CM_فی_TIMER7_CLKCTRL) & CM_CLKCTRL_IDLEST)
		    != CM_CLKCTRL_IDLEST_DISABLE);

		set32(CLKSEL_TIMER7_CLK, CLKSEL_TIMER7_CLK_SEL_نقاب,
		    CLKSEL_TIMER7_CLK_SEL_SEL2);
		طالما ((read32(CLKSEL_TIMER7_CLK) & CLKSEL_TIMER7_CLK_SEL_نقاب)
		    != CLKSEL_TIMER7_CLK_SEL_SEL2);

		/* enable the module and wait for the module to be ready */
		set32(CM_فی_TIMER7_CLKCTRL, CM_MODULEMODE_نقاب,
		    CM_MODULEMODE_ENABLE);
		طالما ((mmio_read(CM_فی_TIMER7_CLKCTRL) & CM_CLKCTRL_IDLEST)
		    != CM_CLKCTRL_IDLEST_FUNC);

		/* Stop timer */
		mmio_clear(fr_timer->ادھار + fr_timer->regs->TCLR,
		    OMAP3_TCLR_ST);

		/* 24Mhz / 16 = 1.5 Mhz */
		mmio_سیٹ(fr_timer->ادھار + fr_timer->regs->TCLR,
		    (3 << OMAP3_TCLR_PTV));
	}

	/* Start and auto-reload at 0 */
	mmio_write(fr_timer->ادھار + fr_timer->regs->TLDR, 0x0);
	mmio_write(fr_timer->ادھار + fr_timer->regs->TCRR, 0x0);

	/* Set up overflow interrupt */
	tisr = OMAP3_TISR_MAT_IT_FLAG | OMAP3_TISR_OVF_IT_FLAG |
	    OMAP3_TISR_TCAR_IT_FLAG;
	/* Clear interrupt status */
	mmio_write(fr_timer->ادھار + fr_timer->regs->TISR, tisr);
	mmio_write(fr_timer->ادھار + fr_timer->regs->TIER,
	    OMAP3_TIER_OVF_IT_ENA);

	/* Start timer */
	mmio_سیٹ(fr_timer->ادھار + fr_timer->regs->TCLR,
	    OMAP3_TCLR_OVF_TRG | OMAP3_TCLR_AR | OMAP3_TCLR_ST |
	    OMAP3_TCLR_PRE);
	پورا = 1;
}

فراغ
omap3_frclock_stop(فراغ)
{
	mmio_clear(fr_timer->ادھار + fr_timer->regs->TCLR, OMAP3_TCLR_ST);
}

فراغ
bsp_timer_init(بنانشانی freq)
{
	/* we only support 1ms resolution */
	u32_t tisr;
	إذا (BOARD_IS_BBXM(machine.board_id)) {
		timer = &dm37xx_timer;
		kern_phys_map_ptr(timer->ادھار, ARM_ورقہ_ناپ,
		    VMMF_UNCACHED | VMMF_WRITE,
		    &timer_phys_map, (vir_بائٹاں) & timer->ادھار);
		/* Stop timer */
		mmio_clear(timer->ادھار + timer->regs->TCLR, OMAP3_TCLR_ST);

		/* Use 32 KHz clock source for GPTIMER1 */
		mmio_clear(OMAP3_CM_CLKSEL_WKUP, OMAP3_CLKSEL_GPT1);
	} وإلا إذا (BOARD_IS_BB(machine.board_id)) {
		timer = &am335x_timer;
		kern_phys_map_ptr(timer->ادھار, ARM_ورقہ_ناپ,
		    VMMF_UNCACHED | VMMF_WRITE,
		    &timer_phys_map, (vir_بائٹاں) & timer->ادھار);
		/* disable the module and wait for the module to be disabled */
		set32(CM_WKUP_TIMER1_CLKCTRL, CM_MODULEMODE_نقاب,
		    CM_MODULEMODE_DISABLED);
		طالما ((mmio_read(CM_WKUP_TIMER1_CLKCTRL) & CM_CLKCTRL_IDLEST)
		    != CM_CLKCTRL_IDLEST_DISABLE);

		set32(CLKSEL_TIMER1MS_CLK, CLKSEL_TIMER1_MS_CLK_SEL_نقاب,
		    CLKSEL_TIMER1MS_CLK_SEL_SEL2);
		طالما ((read32(CLKSEL_TIMER1MS_CLK) &
			CLKSEL_TIMER1_MS_CLK_SEL_نقاب) !=
		    CLKSEL_TIMER1MS_CLK_SEL_SEL2);

		/* enable the module and wait for the module to be ready */
		set32(CM_WKUP_TIMER1_CLKCTRL, CM_MODULEMODE_نقاب,
		    CM_MODULEMODE_ENABLE);
		طالما ((mmio_read(CM_WKUP_TIMER1_CLKCTRL) & CM_CLKCTRL_IDLEST)
		    != CM_CLKCTRL_IDLEST_FUNC);

		/* Stop timer */
		mmio_clear(timer->ادھار + timer->regs->TCLR, OMAP3_TCLR_ST);
	}

	/* Use 1-ms tick mode for GPTIMER1 TRM 16.2.4.2.1 */
	mmio_write(timer->ادھار + timer->regs->TPIR, 232000);
	mmio_write(timer->ادھار + timer->regs->TNIR, -768000);
	mmio_write(timer->ادھار + timer->regs->TLDR,
	    0xffffffff - (32768 / freq) + 1);
	mmio_write(timer->ادھار + timer->regs->TCRR,
	    0xffffffff - (32768 / freq) + 1);

	/* Set up overflow interrupt */
	tisr = OMAP3_TISR_MAT_IT_FLAG | OMAP3_TISR_OVF_IT_FLAG |
	    OMAP3_TISR_TCAR_IT_FLAG;
	/* Clear interrupt status */
	mmio_write(timer->ادھار + timer->regs->TISR, tisr);
	mmio_write(timer->ادھار + timer->regs->TIER, OMAP3_TIER_OVF_IT_ENA);

	/* Start timer */
	mmio_سیٹ(timer->ادھار + timer->regs->TCLR,
	    OMAP3_TCLR_OVF_TRG | OMAP3_TCLR_AR | OMAP3_TCLR_ST);
	/* also initilize the free runnning timer */
	omap3_frclock_init();
}

فراغ
bsp_timer_stop(فراغ)
{
	mmio_clear(timer->ادھار + timer->regs->TCLR, OMAP3_TCLR_ST);
}

ساكن u32_t
read_frc(فراغ)
{
	إذا (پورا == 0) {
		أرجع 0;
	}
	أرجع mmio_read(fr_timer->ادھار + fr_timer->regs->TCRR);
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
ساكن فراغ
frc_حد_توں_ودھ_check(u32_t cur_frc)
{
	ساكن صحيح prev_frc_valid;
	ساكن u32_t prev_frc;
	إذا (prev_frc_valid && prev_frc > cur_frc) {
		بلند_frc++;
	}
	prev_frc = cur_frc;
	prev_frc_valid = 1;
}

فراغ
bsp_timer_int_handler(فراغ)
{
	/* Clear all interrupts */
	u32_t tisr, now;

	/* when the kernel itself is running interrupts are disabled. We
	 * should therefore also read the overflow counter to detect this as
	 * to not miss events. */
	tisr = OMAP3_TISR_MAT_IT_FLAG | OMAP3_TISR_OVF_IT_FLAG |
	    OMAP3_TISR_TCAR_IT_FLAG;
	mmio_write(timer->ادھار + timer->regs->TISR, tisr);

	now = read_frc();
	frc_حد_توں_ودھ_check(now);
}

/* Use the free running clock as TSC */
فراغ
read_tsc_64(u64_t * t)
{
	u32_t now;
	now = read_frc();
	frc_حد_توں_ودھ_check(now);
	*t = (u64_t) now + (بلند_frc << 32);
}
