#ಅನ್ತರ್ಭಾವ "kernel/kernel.h"
#ಅನ್ತರ್ಭಾವ "kernel/clock.h"
#ಅನ್ತರ್ಭಾವ <sys/types.h>
#ಅನ್ತರ್ಭಾವ <machine/cpu.h>
#ಅನ್ತರ್ಭಾವ <minix/board.h>
#ಅನ್ತರ್ಭಾವ <minix/mmio.h>
#ಅನ್ತರ್ಭಾವ <assert.h>
#ಅನ್ತರ್ಭಾವ <io.h>
#ಅನ್ತರ್ಭಾವ <stdlib.h>
#ಅನ್ತರ್ಭಾವ <stdio.h>
#ಅನ್ತರ್ಭಾವ "arch_proto.h"
#ಅನ್ತರ್ಭಾವ "bsp_timer.h"
#ಅನ್ತರ್ಭಾವ "omap_timer_registers.h"
#ಅನ್ತರ್ಭಾವ "omap_intr_registers.h"
#ಅನ್ತರ್ಭಾವ "bsp_intr.h"

/* interrupt handler hook */
ಸ್ಥಿರ irq_hook_t omap3_timer_hook;
ಸ್ಥಿರ u64_t ಮೇಲಿನ_frc;

ರಚನೆ omap_timer_registers;

ರಚನೆ omap_timer
{
	vir_ಬೈಟುಗಳು ತಳ;
	ಪೂರ್ಣಾಂಕ irq_nr;
	ರಚನೆ omap_timer_registers *regs;
};

ರಚನೆ omap_timer_registers
{
	vir_ಬೈಟುಗಳು TIDR;
	vir_ಬೈಟುಗಳು TIOCP_CFG;
	vir_ಬೈಟುಗಳು TISTAT;
	vir_ಬೈಟುಗಳು TISR;
	vir_ಬೈಟುಗಳು TIER;
	vir_ಬೈಟುಗಳು TWER;
	vir_ಬೈಟುಗಳು TCLR;
	vir_ಬೈಟುಗಳು TCRR;
	vir_ಬೈಟುಗಳು TLDR;
	vir_ಬೈಟುಗಳು TTGR;
	vir_ಬೈಟುಗಳು TWPS;
	vir_ಬೈಟುಗಳು TMAR;
	vir_ಬೈಟುಗಳು TCAR1;
	vir_ಬೈಟುಗಳು TSICR;
	vir_ಬೈಟುಗಳು TCAR2;
	vir_ಬೈಟುಗಳು TPIR;
	vir_ಬೈಟುಗಳು TNIR;
	vir_ಬೈಟುಗಳು TCVR;
	vir_ಬೈಟುಗಳು TOCR;
	vir_ಬೈಟುಗಳು TOWR;

};

ಸ್ಥಿರ ರಚನೆ omap_timer_registers regs_v1 = {
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
ಸ್ಥಿರ ರಚನೆ omap_timer_registers regs_v2 = {
	.TIDR = AM335X_TIMER_TIDR,
	.TIOCP_CFG = AM335X_TIMER_TIOCP_CFG,
	.TISTAT = AM335X_TIMER_IRQSTATUS_RAW,
	.TISR = AM335X_TIMER_IRQSTATUS,
	.TIER = AM335_X_TIMER_IRQENABLE_ಗಣ,
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

ಸ್ಥಿರ ರಚನೆ omap_timer dm37xx_timer = {
	.ತಳ = OMAP3_GPTIMER1_ತಳ,
	.irq_nr = OMAP3_GPT1_IRQ,
	.regs = &regs_v1
};

/* free running timer */
ಸ್ಥಿರ ರಚನೆ omap_timer dm37xx_fr_timer = {
	.ತಳ = OMAP3_GPTIMER10_ತಳ,
	.irq_nr = OMAP3_GPT10_IRQ,
	.regs = &regs_v1
};

/* normal timer */
ಸ್ಥಿರ ರಚನೆ omap_timer am335x_timer = {
	.ತಳ = AM335_X_DMTIMER1_1_MS_ತಳ,
	.irq_nr = AM335X_INT_TINT1_1MS,
	.regs = &regs_v1
};

/* free running timer */
ಸ್ಥಿರ ರಚನೆ omap_timer am335x_fr_timer = {
	.ತಳ = AM335_X_DMTIMER7_ತಳ,
	.irq_nr = AM335X_INT_TINT7,
	.regs = &regs_v2
};

ಸ್ಥಿರ ರಚನೆ omap_timer *timer;
ಸ್ಥಿರ ರಚನೆ omap_timer *fr_timer;

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಮುಗಿದ = 0;

ಪೂರ್ಣಾಂಕ
bsp_ರಿಜಿಸ್ಟರ್_timer_handler(ಸ್ಥಿರಾಂಕ irq_handler_t handler)
{
	/* Initialize the CLOCK's interrupt hook. */
	omap3_timer_hook.proc_nr_e = NONE;
	omap3_timer_hook.irq = timer->irq_nr;

	put_irq_handler(&omap3_timer_hook, timer->irq_nr, handler);
	/* only unmask interrupts after registering */
	bsp_irq_unmask(timer->irq_nr);

	ಹಿಂತಿರುಗಿ 0;
}

/* meta data for remapping */
ಸ್ಥಿರ kern_phys_map timer_phys_map;
ಸ್ಥಿರ kern_phys_map fr_timer_phys_map;
ಸ್ಥಿರ kern_phys_map fr_timer_user_phys_map;	/* struct for when the free */
						/* running timer is mapped to */
						/* userland */

/* callback for when the free running clock gets mapped */
ಪೂರ್ಣಾಂಕ
kern_phys_fr_user_mapped(vir_ಬೈಟುಗಳು id, phys_ಬೈಟುಗಳು ವಿಳಾಸ)
{
	/* the only thing we need to do at this stage is to set the address */
	/* in the kerninfo struct */
	ಆದರೆ (BOARD_IS_BBXM(machine.board_id)) {
		arm_frclock.tcrr = ವಿಳಾಸ + OMAP3_TIMER_TCRR;
		arm_frclock.hz = 1625000;
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (BOARD_IS_BB(machine.board_id)) {
		arm_frclock.tcrr = ವಿಳಾಸ + AM335X_TIMER_TCRR;
		arm_frclock.hz = 1500000;
	}
	ಹಿಂತಿರುಗಿ 0;
}

ಶೂನ್ಯ
omap3_frclock_init(ಶೂನ್ಯ)
{
	u32_t tisr;

	/* enable the clock */
	ಆದರೆ (BOARD_IS_BBXM(machine.board_id)) {
		fr_timer = &dm37xx_fr_timer;

		kern_phys_map_ptr(fr_timer->ತಳ, ARM_ಪುಟ_ಗಾತ್ರ,
		    VMMF_UNCACHED | VMMF_WRITE, &fr_timer_phys_map,
		    (vir_ಬೈಟುಗಳು) & fr_timer->ತಳ);

		/* the timer is also mapped in user space hence the this */
		/* second mapping and callback to set kerninfo frclock_tcrr */
		kern_req_phys_map(fr_timer->ತಳ, ARM_ಪುಟ_ಗಾತ್ರ,
		    VMMF_UNCACHED | VMMF_USER,
		    &fr_timer_user_phys_map, kern_phys_fr_user_mapped, 0);

		/* Stop timer */
		mmio_clear(fr_timer->ತಳ + fr_timer->regs->TCLR,
		    OMAP3_TCLR_ST);

		/* Use functional clock source for GPTIMER10 */
		mmio_ಗಣ(OMAP3_CM_CLKSEL_CORE, OMAP3_CLKSEL_GPT10);

		/* Scale timer down to 13/8 = 1.625 Mhz to roughly get
		 * microsecond ticks */
		/* The scale is computed as 2^(PTV+1). So if PTV == 2, we get
		 * 2^3 = 8. */
		mmio_ಗಣ(fr_timer->ತಳ + fr_timer->regs->TCLR,
		    (2 << OMAP3_TCLR_PTV));
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (BOARD_IS_BB(machine.board_id)) {
		fr_timer = &am335x_fr_timer;
		kern_phys_map_ptr(fr_timer->ತಳ, ARM_ಪುಟ_ಗಾತ್ರ,
		    VMMF_UNCACHED | VMMF_WRITE,
		    &fr_timer_phys_map, (vir_ಬೈಟುಗಳು) & fr_timer->ತಳ);

		/* the timer is also mapped in user space hence the this */
		/* second mapping and callback to set kerninfo frclock_tcrr */
		kern_req_phys_map(fr_timer->ತಳ, ARM_ಪುಟ_ಗಾತ್ರ,
		    VMMF_UNCACHED | VMMF_USER,
		    &fr_timer_user_phys_map, kern_phys_fr_user_mapped, 0);
		/* Disable the module and wait for the module to be disabled */
		set32(CM_ಪ್ರತಿ_TIMER7_CLKCTRL, CM_MODULEMODE_ಮುಖವಾಡ,
		    CM_MODULEMODE_DISABLED);
		ಆಗ ((mmio_read(CM_ಪ್ರತಿ_TIMER7_CLKCTRL) & CM_CLKCTRL_IDLEST)
		    != CM_CLKCTRL_IDLEST_DISABLE);

		set32(CLKSEL_TIMER7_CLK, CLKSEL_TIMER7_CLK_SEL_ಮುಖವಾಡ,
		    CLKSEL_TIMER7_CLK_SEL_SEL2);
		ಆಗ ((read32(CLKSEL_TIMER7_CLK) & CLKSEL_TIMER7_CLK_SEL_ಮುಖವಾಡ)
		    != CLKSEL_TIMER7_CLK_SEL_SEL2);

		/* enable the module and wait for the module to be ready */
		set32(CM_ಪ್ರತಿ_TIMER7_CLKCTRL, CM_MODULEMODE_ಮುಖವಾಡ,
		    CM_MODULEMODE_ENABLE);
		ಆಗ ((mmio_read(CM_ಪ್ರತಿ_TIMER7_CLKCTRL) & CM_CLKCTRL_IDLEST)
		    != CM_CLKCTRL_IDLEST_FUNC);

		/* Stop timer */
		mmio_clear(fr_timer->ತಳ + fr_timer->regs->TCLR,
		    OMAP3_TCLR_ST);

		/* 24Mhz / 16 = 1.5 Mhz */
		mmio_ಗಣ(fr_timer->ತಳ + fr_timer->regs->TCLR,
		    (3 << OMAP3_TCLR_PTV));
	}

	/* Start and auto-reload at 0 */
	mmio_write(fr_timer->ತಳ + fr_timer->regs->TLDR, 0x0);
	mmio_write(fr_timer->ತಳ + fr_timer->regs->TCRR, 0x0);

	/* Set up overflow interrupt */
	tisr = OMAP3_TISR_MAT_IT_FLAG | OMAP3_TISR_OVF_IT_FLAG |
	    OMAP3_TISR_TCAR_IT_FLAG;
	/* Clear interrupt status */
	mmio_write(fr_timer->ತಳ + fr_timer->regs->TISR, tisr);
	mmio_write(fr_timer->ತಳ + fr_timer->regs->TIER,
	    OMAP3_TIER_OVF_IT_ENA);

	/* Start timer */
	mmio_ಗಣ(fr_timer->ತಳ + fr_timer->regs->TCLR,
	    OMAP3_TCLR_OVF_TRG | OMAP3_TCLR_AR | OMAP3_TCLR_ST |
	    OMAP3_TCLR_PRE);
	ಮುಗಿದ = 1;
}

ಶೂನ್ಯ
omap3_frclock_stop(ಶೂನ್ಯ)
{
	mmio_clear(fr_timer->ತಳ + fr_timer->regs->TCLR, OMAP3_TCLR_ST);
}

ಶೂನ್ಯ
bsp_timer_init(ಚಿಹ್ನೆರಹಿತ freq)
{
	/* we only support 1ms resolution */
	u32_t tisr;
	ಆದರೆ (BOARD_IS_BBXM(machine.board_id)) {
		timer = &dm37xx_timer;
		kern_phys_map_ptr(timer->ತಳ, ARM_ಪುಟ_ಗಾತ್ರ,
		    VMMF_UNCACHED | VMMF_WRITE,
		    &timer_phys_map, (vir_ಬೈಟುಗಳು) & timer->ತಳ);
		/* Stop timer */
		mmio_clear(timer->ತಳ + timer->regs->TCLR, OMAP3_TCLR_ST);

		/* Use 32 KHz clock source for GPTIMER1 */
		mmio_clear(OMAP3_CM_CLKSEL_WKUP, OMAP3_CLKSEL_GPT1);
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (BOARD_IS_BB(machine.board_id)) {
		timer = &am335x_timer;
		kern_phys_map_ptr(timer->ತಳ, ARM_ಪುಟ_ಗಾತ್ರ,
		    VMMF_UNCACHED | VMMF_WRITE,
		    &timer_phys_map, (vir_ಬೈಟುಗಳು) & timer->ತಳ);
		/* disable the module and wait for the module to be disabled */
		set32(CM_WKUP_TIMER1_CLKCTRL, CM_MODULEMODE_ಮುಖವಾಡ,
		    CM_MODULEMODE_DISABLED);
		ಆಗ ((mmio_read(CM_WKUP_TIMER1_CLKCTRL) & CM_CLKCTRL_IDLEST)
		    != CM_CLKCTRL_IDLEST_DISABLE);

		set32(CLKSEL_TIMER1MS_CLK, CLKSEL_TIMER1_MS_CLK_SEL_ಮುಖವಾಡ,
		    CLKSEL_TIMER1MS_CLK_SEL_SEL2);
		ಆಗ ((read32(CLKSEL_TIMER1MS_CLK) &
			CLKSEL_TIMER1_MS_CLK_SEL_ಮುಖವಾಡ) !=
		    CLKSEL_TIMER1MS_CLK_SEL_SEL2);

		/* enable the module and wait for the module to be ready */
		set32(CM_WKUP_TIMER1_CLKCTRL, CM_MODULEMODE_ಮುಖವಾಡ,
		    CM_MODULEMODE_ENABLE);
		ಆಗ ((mmio_read(CM_WKUP_TIMER1_CLKCTRL) & CM_CLKCTRL_IDLEST)
		    != CM_CLKCTRL_IDLEST_FUNC);

		/* Stop timer */
		mmio_clear(timer->ತಳ + timer->regs->TCLR, OMAP3_TCLR_ST);
	}

	/* Use 1-ms tick mode for GPTIMER1 TRM 16.2.4.2.1 */
	mmio_write(timer->ತಳ + timer->regs->TPIR, 232000);
	mmio_write(timer->ತಳ + timer->regs->TNIR, -768000);
	mmio_write(timer->ತಳ + timer->regs->TLDR,
	    0xffffffff - (32768 / freq) + 1);
	mmio_write(timer->ತಳ + timer->regs->TCRR,
	    0xffffffff - (32768 / freq) + 1);

	/* Set up overflow interrupt */
	tisr = OMAP3_TISR_MAT_IT_FLAG | OMAP3_TISR_OVF_IT_FLAG |
	    OMAP3_TISR_TCAR_IT_FLAG;
	/* Clear interrupt status */
	mmio_write(timer->ತಳ + timer->regs->TISR, tisr);
	mmio_write(timer->ತಳ + timer->regs->TIER, OMAP3_TIER_OVF_IT_ENA);

	/* Start timer */
	mmio_ಗಣ(timer->ತಳ + timer->regs->TCLR,
	    OMAP3_TCLR_OVF_TRG | OMAP3_TCLR_AR | OMAP3_TCLR_ST);
	/* also initilize the free runnning timer */
	omap3_frclock_init();
}

ಶೂನ್ಯ
bsp_timer_stop(ಶೂನ್ಯ)
{
	mmio_clear(timer->ತಳ + timer->regs->TCLR, OMAP3_TCLR_ST);
}

ಸ್ಥಿರ u32_t
read_frc(ಶೂನ್ಯ)
{
	ಆದರೆ (ಮುಗಿದ == 0) {
		ಹಿಂತಿರುಗಿ 0;
	}
	ಹಿಂತಿರುಗಿ mmio_read(fr_timer->ತಳ + fr_timer->regs->TCRR);
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
ಸ್ಥಿರ ಶೂನ್ಯ
frc_ಅತಿಪ್ರವಾಹ_check(u32_t cur_frc)
{
	ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ prev_frc_valid;
	ಸ್ಥಿರ u32_t prev_frc;
	ಆದರೆ (prev_frc_valid && prev_frc > cur_frc) {
		ಮೇಲಿನ_frc++;
	}
	prev_frc = cur_frc;
	prev_frc_valid = 1;
}

ಶೂನ್ಯ
bsp_timer_int_handler(ಶೂನ್ಯ)
{
	/* Clear all interrupts */
	u32_t tisr, now;

	/* when the kernel itself is running interrupts are disabled. We
	 * should therefore also read the overflow counter to detect this as
	 * to not miss events. */
	tisr = OMAP3_TISR_MAT_IT_FLAG | OMAP3_TISR_OVF_IT_FLAG |
	    OMAP3_TISR_TCAR_IT_FLAG;
	mmio_write(timer->ತಳ + timer->regs->TISR, tisr);

	now = read_frc();
	frc_ಅತಿಪ್ರವಾಹ_check(now);
}

/* Use the free running clock as TSC */
ಶೂನ್ಯ
read_tsc_64(u64_t * t)
{
	u32_t now;
	now = read_frc();
	frc_ಅತಿಪ್ರವಾಹ_check(now);
	*t = (u64_t) now + (ಮೇಲಿನ_frc << 32);
}
