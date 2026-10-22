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
ساكن u64_t لوړ_frc;

بنية omap_timer_registers;

بنية omap_timer
{
	vir_بایټونه بنسټ;
	صحيح irq_nr;
	بنية omap_timer_registers *regs;
};

بنية omap_timer_registers
{
	vir_بایټونه TIDR;
	vir_بایټونه TIOCP_CFG;
	vir_بایټونه TISTAT;
	vir_بایټونه TISR;
	vir_بایټونه TIER;
	vir_بایټونه TWER;
	vir_بایټونه TCLR;
	vir_بایټونه TCRR;
	vir_بایټونه TLDR;
	vir_بایټونه TTGR;
	vir_بایټونه TWPS;
	vir_بایټونه TMAR;
	vir_بایټونه TCAR1;
	vir_بایټونه TSICR;
	vir_بایټونه TCAR2;
	vir_بایټونه TPIR;
	vir_بایټونه TNIR;
	vir_بایټونه TCVR;
	vir_بایټونه TOCR;
	vir_بایټونه TOWR;

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
	.TIER = AM335_X_TIMER_IRQENABLE_سېټ,
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
	.بنسټ = OMAP3_GPTIMER1_بنسټ,
	.irq_nr = OMAP3_GPT1_IRQ,
	.regs = &regs_v1
};

/* free running timer */
ساكن بنية omap_timer dm37xx_fr_timer = {
	.بنسټ = OMAP3_GPTIMER10_بنسټ,
	.irq_nr = OMAP3_GPT10_IRQ,
	.regs = &regs_v1
};

/* normal timer */
ساكن بنية omap_timer am335x_timer = {
	.بنسټ = AM335_X_DMTIMER1_1_MS_بنسټ,
	.irq_nr = AM335X_INT_TINT1_1MS,
	.regs = &regs_v1
};

/* free running timer */
ساكن بنية omap_timer am335x_fr_timer = {
	.بنسټ = AM335_X_DMTIMER7_بنسټ,
	.irq_nr = AM335X_INT_TINT7,
	.regs = &regs_v2
};

ساكن بنية omap_timer *timer;
ساكن بنية omap_timer *fr_timer;

ساكن صحيح بشپړ = 0;

صحيح
bsp_ثبتوونکی_timer_handler(ثابت irq_handler_t handler)
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
kern_phys_fr_user_mapped(vir_بایټونه id, phys_بایټونه پته)
{
	/* the only thing we need to do at this stage is to set the address */
	/* in the kerninfo struct */
	إذا (BOARD_IS_BBXM(machine.board_id)) {
		arm_frclock.tcrr = پته + OMAP3_TIMER_TCRR;
		arm_frclock.hz = 1625000;
	} وإلا إذا (BOARD_IS_BB(machine.board_id)) {
		arm_frclock.tcrr = پته + AM335X_TIMER_TCRR;
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

		kern_phys_map_ptr(fr_timer->بنسټ, ARM_پاڼه_اندازه,
		    VMMF_UNCACHED | VMMF_WRITE, &fr_timer_phys_map,
		    (vir_بایټونه) & fr_timer->بنسټ);

		/* the timer is also mapped in user space hence the this */
		/* second mapping and callback to set kerninfo frclock_tcrr */
		kern_req_phys_map(fr_timer->بنسټ, ARM_پاڼه_اندازه,
		    VMMF_UNCACHED | VMMF_USER,
		    &fr_timer_user_phys_map, kern_phys_fr_user_mapped, 0);

		/* Stop timer */
		mmio_clear(fr_timer->بنسټ + fr_timer->regs->TCLR,
		    OMAP3_TCLR_ST);

		/* Use functional clock source for GPTIMER10 */
		mmio_سېټ(OMAP3_CM_CLKSEL_CORE, OMAP3_CLKSEL_GPT10);

		/* Scale timer down to 13/8 = 1.625 Mhz to roughly get
		 * microsecond ticks */
		/* The scale is computed as 2^(PTV+1). So if PTV == 2, we get
		 * 2^3 = 8. */
		mmio_سېټ(fr_timer->بنسټ + fr_timer->regs->TCLR,
		    (2 << OMAP3_TCLR_PTV));
	} وإلا إذا (BOARD_IS_BB(machine.board_id)) {
		fr_timer = &am335x_fr_timer;
		kern_phys_map_ptr(fr_timer->بنسټ, ARM_پاڼه_اندازه,
		    VMMF_UNCACHED | VMMF_WRITE,
		    &fr_timer_phys_map, (vir_بایټونه) & fr_timer->بنسټ);

		/* the timer is also mapped in user space hence the this */
		/* second mapping and callback to set kerninfo frclock_tcrr */
		kern_req_phys_map(fr_timer->بنسټ, ARM_پاڼه_اندازه,
		    VMMF_UNCACHED | VMMF_USER,
		    &fr_timer_user_phys_map, kern_phys_fr_user_mapped, 0);
		/* Disable the module and wait for the module to be disabled */
		set32(CM_فی_TIMER7_CLKCTRL, CM_MODULEMODE_ماسک,
		    CM_MODULEMODE_DISABLED);
		طالما ((mmio_read(CM_فی_TIMER7_CLKCTRL) & CM_CLKCTRL_IDLEST)
		    != CM_CLKCTRL_IDLEST_DISABLE);

		set32(CLKSEL_TIMER7_CLK, CLKSEL_TIMER7_CLK_SEL_ماسک,
		    CLKSEL_TIMER7_CLK_SEL_SEL2);
		طالما ((read32(CLKSEL_TIMER7_CLK) & CLKSEL_TIMER7_CLK_SEL_ماسک)
		    != CLKSEL_TIMER7_CLK_SEL_SEL2);

		/* enable the module and wait for the module to be ready */
		set32(CM_فی_TIMER7_CLKCTRL, CM_MODULEMODE_ماسک,
		    CM_MODULEMODE_ENABLE);
		طالما ((mmio_read(CM_فی_TIMER7_CLKCTRL) & CM_CLKCTRL_IDLEST)
		    != CM_CLKCTRL_IDLEST_FUNC);

		/* Stop timer */
		mmio_clear(fr_timer->بنسټ + fr_timer->regs->TCLR,
		    OMAP3_TCLR_ST);

		/* 24Mhz / 16 = 1.5 Mhz */
		mmio_سېټ(fr_timer->بنسټ + fr_timer->regs->TCLR,
		    (3 << OMAP3_TCLR_PTV));
	}

	/* Start and auto-reload at 0 */
	mmio_write(fr_timer->بنسټ + fr_timer->regs->TLDR, 0x0);
	mmio_write(fr_timer->بنسټ + fr_timer->regs->TCRR, 0x0);

	/* Set up overflow interrupt */
	tisr = OMAP3_TISR_MAT_IT_FLAG | OMAP3_TISR_OVF_IT_FLAG |
	    OMAP3_TISR_TCAR_IT_FLAG;
	/* Clear interrupt status */
	mmio_write(fr_timer->بنسټ + fr_timer->regs->TISR, tisr);
	mmio_write(fr_timer->بنسټ + fr_timer->regs->TIER,
	    OMAP3_TIER_OVF_IT_ENA);

	/* Start timer */
	mmio_سېټ(fr_timer->بنسټ + fr_timer->regs->TCLR,
	    OMAP3_TCLR_OVF_TRG | OMAP3_TCLR_AR | OMAP3_TCLR_ST |
	    OMAP3_TCLR_PRE);
	بشپړ = 1;
}

فراغ
omap3_frclock_stop(فراغ)
{
	mmio_clear(fr_timer->بنسټ + fr_timer->regs->TCLR, OMAP3_TCLR_ST);
}

فراغ
bsp_timer_init(بینښه freq)
{
	/* we only support 1ms resolution */
	u32_t tisr;
	إذا (BOARD_IS_BBXM(machine.board_id)) {
		timer = &dm37xx_timer;
		kern_phys_map_ptr(timer->بنسټ, ARM_پاڼه_اندازه,
		    VMMF_UNCACHED | VMMF_WRITE,
		    &timer_phys_map, (vir_بایټونه) & timer->بنسټ);
		/* Stop timer */
		mmio_clear(timer->بنسټ + timer->regs->TCLR, OMAP3_TCLR_ST);

		/* Use 32 KHz clock source for GPTIMER1 */
		mmio_clear(OMAP3_CM_CLKSEL_WKUP, OMAP3_CLKSEL_GPT1);
	} وإلا إذا (BOARD_IS_BB(machine.board_id)) {
		timer = &am335x_timer;
		kern_phys_map_ptr(timer->بنسټ, ARM_پاڼه_اندازه,
		    VMMF_UNCACHED | VMMF_WRITE,
		    &timer_phys_map, (vir_بایټونه) & timer->بنسټ);
		/* disable the module and wait for the module to be disabled */
		set32(CM_WKUP_TIMER1_CLKCTRL, CM_MODULEMODE_ماسک,
		    CM_MODULEMODE_DISABLED);
		طالما ((mmio_read(CM_WKUP_TIMER1_CLKCTRL) & CM_CLKCTRL_IDLEST)
		    != CM_CLKCTRL_IDLEST_DISABLE);

		set32(CLKSEL_TIMER1MS_CLK, CLKSEL_TIMER1_MS_CLK_SEL_ماسک,
		    CLKSEL_TIMER1MS_CLK_SEL_SEL2);
		طالما ((read32(CLKSEL_TIMER1MS_CLK) &
			CLKSEL_TIMER1_MS_CLK_SEL_ماسک) !=
		    CLKSEL_TIMER1MS_CLK_SEL_SEL2);

		/* enable the module and wait for the module to be ready */
		set32(CM_WKUP_TIMER1_CLKCTRL, CM_MODULEMODE_ماسک,
		    CM_MODULEMODE_ENABLE);
		طالما ((mmio_read(CM_WKUP_TIMER1_CLKCTRL) & CM_CLKCTRL_IDLEST)
		    != CM_CLKCTRL_IDLEST_FUNC);

		/* Stop timer */
		mmio_clear(timer->بنسټ + timer->regs->TCLR, OMAP3_TCLR_ST);
	}

	/* Use 1-ms tick mode for GPTIMER1 TRM 16.2.4.2.1 */
	mmio_write(timer->بنسټ + timer->regs->TPIR, 232000);
	mmio_write(timer->بنسټ + timer->regs->TNIR, -768000);
	mmio_write(timer->بنسټ + timer->regs->TLDR,
	    0xffffffff - (32768 / freq) + 1);
	mmio_write(timer->بنسټ + timer->regs->TCRR,
	    0xffffffff - (32768 / freq) + 1);

	/* Set up overflow interrupt */
	tisr = OMAP3_TISR_MAT_IT_FLAG | OMAP3_TISR_OVF_IT_FLAG |
	    OMAP3_TISR_TCAR_IT_FLAG;
	/* Clear interrupt status */
	mmio_write(timer->بنسټ + timer->regs->TISR, tisr);
	mmio_write(timer->بنسټ + timer->regs->TIER, OMAP3_TIER_OVF_IT_ENA);

	/* Start timer */
	mmio_سېټ(timer->بنسټ + timer->regs->TCLR,
	    OMAP3_TCLR_OVF_TRG | OMAP3_TCLR_AR | OMAP3_TCLR_ST);
	/* also initilize the free runnning timer */
	omap3_frclock_init();
}

فراغ
bsp_timer_stop(فراغ)
{
	mmio_clear(timer->بنسټ + timer->regs->TCLR, OMAP3_TCLR_ST);
}

ساكن u32_t
read_frc(فراغ)
{
	إذا (بشپړ == 0) {
		أرجع 0;
	}
	أرجع mmio_read(fr_timer->بنسټ + fr_timer->regs->TCRR);
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
frc_ډکېدنه_check(u32_t cur_frc)
{
	ساكن صحيح prev_frc_valid;
	ساكن u32_t prev_frc;
	إذا (prev_frc_valid && prev_frc > cur_frc) {
		لوړ_frc++;
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
	mmio_write(timer->بنسټ + timer->regs->TISR, tisr);

	now = read_frc();
	frc_ډکېدنه_check(now);
}

/* Use the free running clock as TSC */
فراغ
read_tsc_64(u64_t * t)
{
	u32_t now;
	now = read_frc();
	frc_ډکېدنه_check(now);
	*t = (u64_t) now + (لوړ_frc << 32);
}
