#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/kernel.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/clock.h"
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/types.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <machine/cpu.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <minix/board.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <minix/mmio.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <assert.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <io.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdlib.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdio.h>
#ਅਨ੍ਤਰ੍ਭਾਵ "arch_proto.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "bsp_timer.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "omap_timer_registers.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "omap_intr_registers.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "bsp_intr.h"

/* interrupt handler hook */
ਸਥਿਰ irq_hook_t omap3_timer_hook;
ਸਥਿਰ u64_t ਉੱਚਾ_frc;

ਬਣਤਰ omap_timer_registers;

ਬਣਤਰ omap_timer
{
	vir_ਬਾਈਟਾਂ ਅਧਾਰ;
	ਪੂਰਨ irq_nr;
	ਬਣਤਰ omap_timer_registers *regs;
};

ਬਣਤਰ omap_timer_registers
{
	vir_ਬਾਈਟਾਂ TIDR;
	vir_ਬਾਈਟਾਂ TIOCP_CFG;
	vir_ਬਾਈਟਾਂ TISTAT;
	vir_ਬਾਈਟਾਂ TISR;
	vir_ਬਾਈਟਾਂ TIER;
	vir_ਬਾਈਟਾਂ TWER;
	vir_ਬਾਈਟਾਂ TCLR;
	vir_ਬਾਈਟਾਂ TCRR;
	vir_ਬਾਈਟਾਂ TLDR;
	vir_ਬਾਈਟਾਂ TTGR;
	vir_ਬਾਈਟਾਂ TWPS;
	vir_ਬਾਈਟਾਂ TMAR;
	vir_ਬਾਈਟਾਂ TCAR1;
	vir_ਬਾਈਟਾਂ TSICR;
	vir_ਬਾਈਟਾਂ TCAR2;
	vir_ਬਾਈਟਾਂ TPIR;
	vir_ਬਾਈਟਾਂ TNIR;
	vir_ਬਾਈਟਾਂ TCVR;
	vir_ਬਾਈਟਾਂ TOCR;
	vir_ਬਾਈਟਾਂ TOWR;

};

ਸਥਿਰ ਬਣਤਰ omap_timer_registers regs_v1 = {
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
ਸਥਿਰ ਬਣਤਰ omap_timer_registers regs_v2 = {
	.TIDR = AM335X_TIMER_TIDR,
	.TIOCP_CFG = AM335X_TIMER_TIOCP_CFG,
	.TISTAT = AM335X_TIMER_IRQSTATUS_RAW,
	.TISR = AM335X_TIMER_IRQSTATUS,
	.TIER = AM335_X_TIMER_IRQENABLE_ਸੈੱਟ,
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

ਸਥਿਰ ਬਣਤਰ omap_timer dm37xx_timer = {
	.ਅਧਾਰ = OMAP3_GPTIMER1_ਅਧਾਰ,
	.irq_nr = OMAP3_GPT1_IRQ,
	.regs = &regs_v1
};

/* free running timer */
ਸਥਿਰ ਬਣਤਰ omap_timer dm37xx_fr_timer = {
	.ਅਧਾਰ = OMAP3_GPTIMER10_ਅਧਾਰ,
	.irq_nr = OMAP3_GPT10_IRQ,
	.regs = &regs_v1
};

/* normal timer */
ਸਥਿਰ ਬਣਤਰ omap_timer am335x_timer = {
	.ਅਧਾਰ = AM335_X_DMTIMER1_1_MS_ਅਧਾਰ,
	.irq_nr = AM335X_INT_TINT1_1MS,
	.regs = &regs_v1
};

/* free running timer */
ਸਥਿਰ ਬਣਤਰ omap_timer am335x_fr_timer = {
	.ਅਧਾਰ = AM335_X_DMTIMER7_ਅਧਾਰ,
	.irq_nr = AM335X_INT_TINT7,
	.regs = &regs_v2
};

ਸਥਿਰ ਬਣਤਰ omap_timer *timer;
ਸਥਿਰ ਬਣਤਰ omap_timer *fr_timer;

ਸਥਿਰ ਪੂਰਨ ਪੂਰਾ = 0;

ਪੂਰਨ
bsp_ਰਜਿਸਟਰ_timer_handler(ਅਚਲ irq_handler_t handler)
{
	/* Initialize the CLOCK's interrupt hook. */
	omap3_timer_hook.proc_nr_e = NONE;
	omap3_timer_hook.irq = timer->irq_nr;

	put_irq_handler(&omap3_timer_hook, timer->irq_nr, handler);
	/* only unmask interrupts after registering */
	bsp_irq_unmask(timer->irq_nr);

	ਵਾਪਸ 0;
}

/* meta data for remapping */
ਸਥਿਰ kern_phys_map timer_phys_map;
ਸਥਿਰ kern_phys_map fr_timer_phys_map;
ਸਥਿਰ kern_phys_map fr_timer_user_phys_map;	/* struct for when the free */
						/* running timer is mapped to */
						/* userland */

/* callback for when the free running clock gets mapped */
ਪੂਰਨ
kern_phys_fr_user_mapped(vir_ਬਾਈਟਾਂ id, phys_ਬਾਈਟਾਂ ਪਤਾ)
{
	/* the only thing we need to do at this stage is to set the address */
	/* in the kerninfo struct */
	ਜੇ (BOARD_IS_BBXM(machine.board_id)) {
		arm_frclock.tcrr = ਪਤਾ + OMAP3_TIMER_TCRR;
		arm_frclock.hz = 1625000;
	} ਨਹੀਂਤਾਂ ਜੇ (BOARD_IS_BB(machine.board_id)) {
		arm_frclock.tcrr = ਪਤਾ + AM335X_TIMER_TCRR;
		arm_frclock.hz = 1500000;
	}
	ਵਾਪਸ 0;
}

ਖਾਲੀ
omap3_frclock_init(ਖਾਲੀ)
{
	u32_t tisr;

	/* enable the clock */
	ਜੇ (BOARD_IS_BBXM(machine.board_id)) {
		fr_timer = &dm37xx_fr_timer;

		kern_phys_map_ptr(fr_timer->ਅਧਾਰ, ARM_ਪੰਨਾ_ਮਾਪ,
		    VMMF_UNCACHED | VMMF_WRITE, &fr_timer_phys_map,
		    (vir_ਬਾਈਟਾਂ) & fr_timer->ਅਧਾਰ);

		/* the timer is also mapped in user space hence the this */
		/* second mapping and callback to set kerninfo frclock_tcrr */
		kern_req_phys_map(fr_timer->ਅਧਾਰ, ARM_ਪੰਨਾ_ਮਾਪ,
		    VMMF_UNCACHED | VMMF_USER,
		    &fr_timer_user_phys_map, kern_phys_fr_user_mapped, 0);

		/* Stop timer */
		mmio_clear(fr_timer->ਅਧਾਰ + fr_timer->regs->TCLR,
		    OMAP3_TCLR_ST);

		/* Use functional clock source for GPTIMER10 */
		mmio_ਸੈੱਟ(OMAP3_CM_CLKSEL_CORE, OMAP3_CLKSEL_GPT10);

		/* Scale timer down to 13/8 = 1.625 Mhz to roughly get
		 * microsecond ticks */
		/* The scale is computed as 2^(PTV+1). So if PTV == 2, we get
		 * 2^3 = 8. */
		mmio_ਸੈੱਟ(fr_timer->ਅਧਾਰ + fr_timer->regs->TCLR,
		    (2 << OMAP3_TCLR_PTV));
	} ਨਹੀਂਤਾਂ ਜੇ (BOARD_IS_BB(machine.board_id)) {
		fr_timer = &am335x_fr_timer;
		kern_phys_map_ptr(fr_timer->ਅਧਾਰ, ARM_ਪੰਨਾ_ਮਾਪ,
		    VMMF_UNCACHED | VMMF_WRITE,
		    &fr_timer_phys_map, (vir_ਬਾਈਟਾਂ) & fr_timer->ਅਧਾਰ);

		/* the timer is also mapped in user space hence the this */
		/* second mapping and callback to set kerninfo frclock_tcrr */
		kern_req_phys_map(fr_timer->ਅਧਾਰ, ARM_ਪੰਨਾ_ਮਾਪ,
		    VMMF_UNCACHED | VMMF_USER,
		    &fr_timer_user_phys_map, kern_phys_fr_user_mapped, 0);
		/* Disable the module and wait for the module to be disabled */
		set32(CM_ਪ੍ਰਤੀ_TIMER7_CLKCTRL, CM_MODULEMODE_ਨਕਾਬ,
		    CM_MODULEMODE_DISABLED);
		ਜਦੋਂ ((mmio_read(CM_ਪ੍ਰਤੀ_TIMER7_CLKCTRL) & CM_CLKCTRL_IDLEST)
		    != CM_CLKCTRL_IDLEST_DISABLE);

		set32(CLKSEL_TIMER7_CLK, CLKSEL_TIMER7_CLK_SEL_ਨਕਾਬ,
		    CLKSEL_TIMER7_CLK_SEL_SEL2);
		ਜਦੋਂ ((read32(CLKSEL_TIMER7_CLK) & CLKSEL_TIMER7_CLK_SEL_ਨਕਾਬ)
		    != CLKSEL_TIMER7_CLK_SEL_SEL2);

		/* enable the module and wait for the module to be ready */
		set32(CM_ਪ੍ਰਤੀ_TIMER7_CLKCTRL, CM_MODULEMODE_ਨਕਾਬ,
		    CM_MODULEMODE_ENABLE);
		ਜਦੋਂ ((mmio_read(CM_ਪ੍ਰਤੀ_TIMER7_CLKCTRL) & CM_CLKCTRL_IDLEST)
		    != CM_CLKCTRL_IDLEST_FUNC);

		/* Stop timer */
		mmio_clear(fr_timer->ਅਧਾਰ + fr_timer->regs->TCLR,
		    OMAP3_TCLR_ST);

		/* 24Mhz / 16 = 1.5 Mhz */
		mmio_ਸੈੱਟ(fr_timer->ਅਧਾਰ + fr_timer->regs->TCLR,
		    (3 << OMAP3_TCLR_PTV));
	}

	/* Start and auto-reload at 0 */
	mmio_write(fr_timer->ਅਧਾਰ + fr_timer->regs->TLDR, 0x0);
	mmio_write(fr_timer->ਅਧਾਰ + fr_timer->regs->TCRR, 0x0);

	/* Set up overflow interrupt */
	tisr = OMAP3_TISR_MAT_IT_FLAG | OMAP3_TISR_OVF_IT_FLAG |
	    OMAP3_TISR_TCAR_IT_FLAG;
	/* Clear interrupt status */
	mmio_write(fr_timer->ਅਧਾਰ + fr_timer->regs->TISR, tisr);
	mmio_write(fr_timer->ਅਧਾਰ + fr_timer->regs->TIER,
	    OMAP3_TIER_OVF_IT_ENA);

	/* Start timer */
	mmio_ਸੈੱਟ(fr_timer->ਅਧਾਰ + fr_timer->regs->TCLR,
	    OMAP3_TCLR_OVF_TRG | OMAP3_TCLR_AR | OMAP3_TCLR_ST |
	    OMAP3_TCLR_PRE);
	ਪੂਰਾ = 1;
}

ਖਾਲੀ
omap3_frclock_stop(ਖਾਲੀ)
{
	mmio_clear(fr_timer->ਅਧਾਰ + fr_timer->regs->TCLR, OMAP3_TCLR_ST);
}

ਖਾਲੀ
bsp_timer_init(ਬਿਨਾਨਿਸ਼ਾਨੀ freq)
{
	/* we only support 1ms resolution */
	u32_t tisr;
	ਜੇ (BOARD_IS_BBXM(machine.board_id)) {
		timer = &dm37xx_timer;
		kern_phys_map_ptr(timer->ਅਧਾਰ, ARM_ਪੰਨਾ_ਮਾਪ,
		    VMMF_UNCACHED | VMMF_WRITE,
		    &timer_phys_map, (vir_ਬਾਈਟਾਂ) & timer->ਅਧਾਰ);
		/* Stop timer */
		mmio_clear(timer->ਅਧਾਰ + timer->regs->TCLR, OMAP3_TCLR_ST);

		/* Use 32 KHz clock source for GPTIMER1 */
		mmio_clear(OMAP3_CM_CLKSEL_WKUP, OMAP3_CLKSEL_GPT1);
	} ਨਹੀਂਤਾਂ ਜੇ (BOARD_IS_BB(machine.board_id)) {
		timer = &am335x_timer;
		kern_phys_map_ptr(timer->ਅਧਾਰ, ARM_ਪੰਨਾ_ਮਾਪ,
		    VMMF_UNCACHED | VMMF_WRITE,
		    &timer_phys_map, (vir_ਬਾਈਟਾਂ) & timer->ਅਧਾਰ);
		/* disable the module and wait for the module to be disabled */
		set32(CM_WKUP_TIMER1_CLKCTRL, CM_MODULEMODE_ਨਕਾਬ,
		    CM_MODULEMODE_DISABLED);
		ਜਦੋਂ ((mmio_read(CM_WKUP_TIMER1_CLKCTRL) & CM_CLKCTRL_IDLEST)
		    != CM_CLKCTRL_IDLEST_DISABLE);

		set32(CLKSEL_TIMER1MS_CLK, CLKSEL_TIMER1_MS_CLK_SEL_ਨਕਾਬ,
		    CLKSEL_TIMER1MS_CLK_SEL_SEL2);
		ਜਦੋਂ ((read32(CLKSEL_TIMER1MS_CLK) &
			CLKSEL_TIMER1_MS_CLK_SEL_ਨਕਾਬ) !=
		    CLKSEL_TIMER1MS_CLK_SEL_SEL2);

		/* enable the module and wait for the module to be ready */
		set32(CM_WKUP_TIMER1_CLKCTRL, CM_MODULEMODE_ਨਕਾਬ,
		    CM_MODULEMODE_ENABLE);
		ਜਦੋਂ ((mmio_read(CM_WKUP_TIMER1_CLKCTRL) & CM_CLKCTRL_IDLEST)
		    != CM_CLKCTRL_IDLEST_FUNC);

		/* Stop timer */
		mmio_clear(timer->ਅਧਾਰ + timer->regs->TCLR, OMAP3_TCLR_ST);
	}

	/* Use 1-ms tick mode for GPTIMER1 TRM 16.2.4.2.1 */
	mmio_write(timer->ਅਧਾਰ + timer->regs->TPIR, 232000);
	mmio_write(timer->ਅਧਾਰ + timer->regs->TNIR, -768000);
	mmio_write(timer->ਅਧਾਰ + timer->regs->TLDR,
	    0xffffffff - (32768 / freq) + 1);
	mmio_write(timer->ਅਧਾਰ + timer->regs->TCRR,
	    0xffffffff - (32768 / freq) + 1);

	/* Set up overflow interrupt */
	tisr = OMAP3_TISR_MAT_IT_FLAG | OMAP3_TISR_OVF_IT_FLAG |
	    OMAP3_TISR_TCAR_IT_FLAG;
	/* Clear interrupt status */
	mmio_write(timer->ਅਧਾਰ + timer->regs->TISR, tisr);
	mmio_write(timer->ਅਧਾਰ + timer->regs->TIER, OMAP3_TIER_OVF_IT_ENA);

	/* Start timer */
	mmio_ਸੈੱਟ(timer->ਅਧਾਰ + timer->regs->TCLR,
	    OMAP3_TCLR_OVF_TRG | OMAP3_TCLR_AR | OMAP3_TCLR_ST);
	/* also initilize the free runnning timer */
	omap3_frclock_init();
}

ਖਾਲੀ
bsp_timer_stop(ਖਾਲੀ)
{
	mmio_clear(timer->ਅਧਾਰ + timer->regs->TCLR, OMAP3_TCLR_ST);
}

ਸਥਿਰ u32_t
read_frc(ਖਾਲੀ)
{
	ਜੇ (ਪੂਰਾ == 0) {
		ਵਾਪਸ 0;
	}
	ਵਾਪਸ mmio_read(fr_timer->ਅਧਾਰ + fr_timer->regs->TCRR);
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
ਸਥਿਰ ਖਾਲੀ
frc_ਉਛਾਲ_check(u32_t cur_frc)
{
	ਸਥਿਰ ਪੂਰਨ prev_frc_valid;
	ਸਥਿਰ u32_t prev_frc;
	ਜੇ (prev_frc_valid && prev_frc > cur_frc) {
		ਉੱਚਾ_frc++;
	}
	prev_frc = cur_frc;
	prev_frc_valid = 1;
}

ਖਾਲੀ
bsp_timer_int_handler(ਖਾਲੀ)
{
	/* Clear all interrupts */
	u32_t tisr, now;

	/* when the kernel itself is running interrupts are disabled. We
	 * should therefore also read the overflow counter to detect this as
	 * to not miss events. */
	tisr = OMAP3_TISR_MAT_IT_FLAG | OMAP3_TISR_OVF_IT_FLAG |
	    OMAP3_TISR_TCAR_IT_FLAG;
	mmio_write(timer->ਅਧਾਰ + timer->regs->TISR, tisr);

	now = read_frc();
	frc_ਉਛਾਲ_check(now);
}

/* Use the free running clock as TSC */
ਖਾਲੀ
read_tsc_64(u64_t * t)
{
	u32_t now;
	now = read_frc();
	frc_ਉਛਾਲ_check(now);
	*t = (u64_t) now + (ਉੱਚਾ_frc << 32);
}
