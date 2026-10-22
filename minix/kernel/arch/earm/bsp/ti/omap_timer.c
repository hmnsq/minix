#అన్తర్భావ "kernel/kernel.h"
#అన్తర్భావ "kernel/clock.h"
#అన్తర్భావ <sys/types.h>
#అన్తర్భావ <machine/cpu.h>
#అన్తర్భావ <minix/board.h>
#అన్తర్భావ <minix/mmio.h>
#అన్తర్భావ <assert.h>
#అన్తర్భావ <io.h>
#అన్తర్భావ <stdlib.h>
#అన్తర్భావ <stdio.h>
#అన్తర్భావ "arch_proto.h"
#అన్తర్భావ "bsp_timer.h"
#అన్తర్భావ "omap_timer_registers.h"
#అన్తర్భావ "omap_intr_registers.h"
#అన్తర్భావ "bsp_intr.h"

/* interrupt handler hook */
స్థిర irq_hook_t omap3_timer_hook;
స్థిర u64_t ఎగువ_frc;

నిర్మాణం omap_timer_registers;

నిర్మాణం omap_timer
{
	vir_బైట్లు పునాది;
	పూర్ణాంకం irq_nr;
	నిర్మాణం omap_timer_registers *regs;
};

నిర్మాణం omap_timer_registers
{
	vir_బైట్లు TIDR;
	vir_బైట్లు TIOCP_CFG;
	vir_బైట్లు TISTAT;
	vir_బైట్లు TISR;
	vir_బైట్లు TIER;
	vir_బైట్లు TWER;
	vir_బైట్లు TCLR;
	vir_బైట్లు TCRR;
	vir_బైట్లు TLDR;
	vir_బైట్లు TTGR;
	vir_బైట్లు TWPS;
	vir_బైట్లు TMAR;
	vir_బైట్లు TCAR1;
	vir_బైట్లు TSICR;
	vir_బైట్లు TCAR2;
	vir_బైట్లు TPIR;
	vir_బైట్లు TNIR;
	vir_బైట్లు TCVR;
	vir_బైట్లు TOCR;
	vir_బైట్లు TOWR;

};

స్థిర నిర్మాణం omap_timer_registers regs_v1 = {
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
స్థిర నిర్మాణం omap_timer_registers regs_v2 = {
	.TIDR = AM335X_TIMER_TIDR,
	.TIOCP_CFG = AM335X_TIMER_TIOCP_CFG,
	.TISTAT = AM335X_TIMER_IRQSTATUS_RAW,
	.TISR = AM335X_TIMER_IRQSTATUS,
	.TIER = AM335_X_TIMER_IRQENABLE_సమితి,
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

స్థిర నిర్మాణం omap_timer dm37xx_timer = {
	.పునాది = OMAP3_GPTIMER1_పునాది,
	.irq_nr = OMAP3_GPT1_IRQ,
	.regs = &regs_v1
};

/* free running timer */
స్థిర నిర్మాణం omap_timer dm37xx_fr_timer = {
	.పునాది = OMAP3_GPTIMER10_పునాది,
	.irq_nr = OMAP3_GPT10_IRQ,
	.regs = &regs_v1
};

/* normal timer */
స్థిర నిర్మాణం omap_timer am335x_timer = {
	.పునాది = AM335_X_DMTIMER1_1_MS_పునాది,
	.irq_nr = AM335X_INT_TINT1_1MS,
	.regs = &regs_v1
};

/* free running timer */
స్థిర నిర్మాణం omap_timer am335x_fr_timer = {
	.పునాది = AM335_X_DMTIMER7_పునాది,
	.irq_nr = AM335X_INT_TINT7,
	.regs = &regs_v2
};

స్థిర నిర్మాణం omap_timer *timer;
స్థిర నిర్మాణం omap_timer *fr_timer;

స్థిర పూర్ణాంకం పూర్తి = 0;

పూర్ణాంకం
bsp_రిజిస్టరు_timer_handler(స్థిరరాశి irq_handler_t handler)
{
	/* Initialize the CLOCK's interrupt hook. */
	omap3_timer_hook.proc_nr_e = NONE;
	omap3_timer_hook.irq = timer->irq_nr;

	put_irq_handler(&omap3_timer_hook, timer->irq_nr, handler);
	/* only unmask interrupts after registering */
	bsp_irq_unmask(timer->irq_nr);

	తిరిగి 0;
}

/* meta data for remapping */
స్థిర kern_phys_map timer_phys_map;
స్థిర kern_phys_map fr_timer_phys_map;
స్థిర kern_phys_map fr_timer_user_phys_map;	/* struct for when the free */
						/* running timer is mapped to */
						/* userland */

/* callback for when the free running clock gets mapped */
పూర్ణాంకం
kern_phys_fr_user_mapped(vir_బైట్లు id, phys_బైట్లు చిరునామా)
{
	/* the only thing we need to do at this stage is to set the address */
	/* in the kerninfo struct */
	ఒకవేళ (BOARD_IS_BBXM(machine.board_id)) {
		arm_frclock.tcrr = చిరునామా + OMAP3_TIMER_TCRR;
		arm_frclock.hz = 1625000;
	} లేకపోతే ఒకవేళ (BOARD_IS_BB(machine.board_id)) {
		arm_frclock.tcrr = చిరునామా + AM335X_TIMER_TCRR;
		arm_frclock.hz = 1500000;
	}
	తిరిగి 0;
}

శూన్య
omap3_frclock_init(శూన్య)
{
	u32_t tisr;

	/* enable the clock */
	ఒకవేళ (BOARD_IS_BBXM(machine.board_id)) {
		fr_timer = &dm37xx_fr_timer;

		kern_phys_map_ptr(fr_timer->పునాది, ARM_పుట_కొలత,
		    VMMF_UNCACHED | VMMF_WRITE, &fr_timer_phys_map,
		    (vir_బైట్లు) & fr_timer->పునాది);

		/* the timer is also mapped in user space hence the this */
		/* second mapping and callback to set kerninfo frclock_tcrr */
		kern_req_phys_map(fr_timer->పునాది, ARM_పుట_కొలత,
		    VMMF_UNCACHED | VMMF_USER,
		    &fr_timer_user_phys_map, kern_phys_fr_user_mapped, 0);

		/* Stop timer */
		mmio_clear(fr_timer->పునాది + fr_timer->regs->TCLR,
		    OMAP3_TCLR_ST);

		/* Use functional clock source for GPTIMER10 */
		mmio_సమితి(OMAP3_CM_CLKSEL_CORE, OMAP3_CLKSEL_GPT10);

		/* Scale timer down to 13/8 = 1.625 Mhz to roughly get
		 * microsecond ticks */
		/* The scale is computed as 2^(PTV+1). So if PTV == 2, we get
		 * 2^3 = 8. */
		mmio_సమితి(fr_timer->పునాది + fr_timer->regs->TCLR,
		    (2 << OMAP3_TCLR_PTV));
	} లేకపోతే ఒకవేళ (BOARD_IS_BB(machine.board_id)) {
		fr_timer = &am335x_fr_timer;
		kern_phys_map_ptr(fr_timer->పునాది, ARM_పుట_కొలత,
		    VMMF_UNCACHED | VMMF_WRITE,
		    &fr_timer_phys_map, (vir_బైట్లు) & fr_timer->పునాది);

		/* the timer is also mapped in user space hence the this */
		/* second mapping and callback to set kerninfo frclock_tcrr */
		kern_req_phys_map(fr_timer->పునాది, ARM_పుట_కొలత,
		    VMMF_UNCACHED | VMMF_USER,
		    &fr_timer_user_phys_map, kern_phys_fr_user_mapped, 0);
		/* Disable the module and wait for the module to be disabled */
		set32(CM_చొప్పున_TIMER7_CLKCTRL, CM_MODULEMODE_ముసుగు,
		    CM_MODULEMODE_DISABLED);
		అయితే ((mmio_read(CM_చొప్పున_TIMER7_CLKCTRL) & CM_CLKCTRL_IDLEST)
		    != CM_CLKCTRL_IDLEST_DISABLE);

		set32(CLKSEL_TIMER7_CLK, CLKSEL_TIMER7_CLK_SEL_ముసుగు,
		    CLKSEL_TIMER7_CLK_SEL_SEL2);
		అయితే ((read32(CLKSEL_TIMER7_CLK) & CLKSEL_TIMER7_CLK_SEL_ముసుగు)
		    != CLKSEL_TIMER7_CLK_SEL_SEL2);

		/* enable the module and wait for the module to be ready */
		set32(CM_చొప్పున_TIMER7_CLKCTRL, CM_MODULEMODE_ముసుగు,
		    CM_MODULEMODE_ENABLE);
		అయితే ((mmio_read(CM_చొప్పున_TIMER7_CLKCTRL) & CM_CLKCTRL_IDLEST)
		    != CM_CLKCTRL_IDLEST_FUNC);

		/* Stop timer */
		mmio_clear(fr_timer->పునాది + fr_timer->regs->TCLR,
		    OMAP3_TCLR_ST);

		/* 24Mhz / 16 = 1.5 Mhz */
		mmio_సమితి(fr_timer->పునాది + fr_timer->regs->TCLR,
		    (3 << OMAP3_TCLR_PTV));
	}

	/* Start and auto-reload at 0 */
	mmio_write(fr_timer->పునాది + fr_timer->regs->TLDR, 0x0);
	mmio_write(fr_timer->పునాది + fr_timer->regs->TCRR, 0x0);

	/* Set up overflow interrupt */
	tisr = OMAP3_TISR_MAT_IT_FLAG | OMAP3_TISR_OVF_IT_FLAG |
	    OMAP3_TISR_TCAR_IT_FLAG;
	/* Clear interrupt status */
	mmio_write(fr_timer->పునాది + fr_timer->regs->TISR, tisr);
	mmio_write(fr_timer->పునాది + fr_timer->regs->TIER,
	    OMAP3_TIER_OVF_IT_ENA);

	/* Start timer */
	mmio_సమితి(fr_timer->పునాది + fr_timer->regs->TCLR,
	    OMAP3_TCLR_OVF_TRG | OMAP3_TCLR_AR | OMAP3_TCLR_ST |
	    OMAP3_TCLR_PRE);
	పూర్తి = 1;
}

శూన్య
omap3_frclock_stop(శూన్య)
{
	mmio_clear(fr_timer->పునాది + fr_timer->regs->TCLR, OMAP3_TCLR_ST);
}

శూన్య
bsp_timer_init(గుర్తులేని freq)
{
	/* we only support 1ms resolution */
	u32_t tisr;
	ఒకవేళ (BOARD_IS_BBXM(machine.board_id)) {
		timer = &dm37xx_timer;
		kern_phys_map_ptr(timer->పునాది, ARM_పుట_కొలత,
		    VMMF_UNCACHED | VMMF_WRITE,
		    &timer_phys_map, (vir_బైట్లు) & timer->పునాది);
		/* Stop timer */
		mmio_clear(timer->పునాది + timer->regs->TCLR, OMAP3_TCLR_ST);

		/* Use 32 KHz clock source for GPTIMER1 */
		mmio_clear(OMAP3_CM_CLKSEL_WKUP, OMAP3_CLKSEL_GPT1);
	} లేకపోతే ఒకవేళ (BOARD_IS_BB(machine.board_id)) {
		timer = &am335x_timer;
		kern_phys_map_ptr(timer->పునాది, ARM_పుట_కొలత,
		    VMMF_UNCACHED | VMMF_WRITE,
		    &timer_phys_map, (vir_బైట్లు) & timer->పునాది);
		/* disable the module and wait for the module to be disabled */
		set32(CM_WKUP_TIMER1_CLKCTRL, CM_MODULEMODE_ముసుగు,
		    CM_MODULEMODE_DISABLED);
		అయితే ((mmio_read(CM_WKUP_TIMER1_CLKCTRL) & CM_CLKCTRL_IDLEST)
		    != CM_CLKCTRL_IDLEST_DISABLE);

		set32(CLKSEL_TIMER1MS_CLK, CLKSEL_TIMER1_MS_CLK_SEL_ముసుగు,
		    CLKSEL_TIMER1MS_CLK_SEL_SEL2);
		అయితే ((read32(CLKSEL_TIMER1MS_CLK) &
			CLKSEL_TIMER1_MS_CLK_SEL_ముసుగు) !=
		    CLKSEL_TIMER1MS_CLK_SEL_SEL2);

		/* enable the module and wait for the module to be ready */
		set32(CM_WKUP_TIMER1_CLKCTRL, CM_MODULEMODE_ముసుగు,
		    CM_MODULEMODE_ENABLE);
		అయితే ((mmio_read(CM_WKUP_TIMER1_CLKCTRL) & CM_CLKCTRL_IDLEST)
		    != CM_CLKCTRL_IDLEST_FUNC);

		/* Stop timer */
		mmio_clear(timer->పునాది + timer->regs->TCLR, OMAP3_TCLR_ST);
	}

	/* Use 1-ms tick mode for GPTIMER1 TRM 16.2.4.2.1 */
	mmio_write(timer->పునాది + timer->regs->TPIR, 232000);
	mmio_write(timer->పునాది + timer->regs->TNIR, -768000);
	mmio_write(timer->పునాది + timer->regs->TLDR,
	    0xffffffff - (32768 / freq) + 1);
	mmio_write(timer->పునాది + timer->regs->TCRR,
	    0xffffffff - (32768 / freq) + 1);

	/* Set up overflow interrupt */
	tisr = OMAP3_TISR_MAT_IT_FLAG | OMAP3_TISR_OVF_IT_FLAG |
	    OMAP3_TISR_TCAR_IT_FLAG;
	/* Clear interrupt status */
	mmio_write(timer->పునాది + timer->regs->TISR, tisr);
	mmio_write(timer->పునాది + timer->regs->TIER, OMAP3_TIER_OVF_IT_ENA);

	/* Start timer */
	mmio_సమితి(timer->పునాది + timer->regs->TCLR,
	    OMAP3_TCLR_OVF_TRG | OMAP3_TCLR_AR | OMAP3_TCLR_ST);
	/* also initilize the free runnning timer */
	omap3_frclock_init();
}

శూన్య
bsp_timer_stop(శూన్య)
{
	mmio_clear(timer->పునాది + timer->regs->TCLR, OMAP3_TCLR_ST);
}

స్థిర u32_t
read_frc(శూన్య)
{
	ఒకవేళ (పూర్తి == 0) {
		తిరిగి 0;
	}
	తిరిగి mmio_read(fr_timer->పునాది + fr_timer->regs->TCRR);
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
స్థిర శూన్య
frc_అధికప్రవాహం_check(u32_t cur_frc)
{
	స్థిర పూర్ణాంకం prev_frc_valid;
	స్థిర u32_t prev_frc;
	ఒకవేళ (prev_frc_valid && prev_frc > cur_frc) {
		ఎగువ_frc++;
	}
	prev_frc = cur_frc;
	prev_frc_valid = 1;
}

శూన్య
bsp_timer_int_handler(శూన్య)
{
	/* Clear all interrupts */
	u32_t tisr, now;

	/* when the kernel itself is running interrupts are disabled. We
	 * should therefore also read the overflow counter to detect this as
	 * to not miss events. */
	tisr = OMAP3_TISR_MAT_IT_FLAG | OMAP3_TISR_OVF_IT_FLAG |
	    OMAP3_TISR_TCAR_IT_FLAG;
	mmio_write(timer->పునాది + timer->regs->TISR, tisr);

	now = read_frc();
	frc_అధికప్రవాహం_check(now);
}

/* Use the free running clock as TSC */
శూన్య
read_tsc_64(u64_t * t)
{
	u32_t now;
	now = read_frc();
	frc_అధికప్రవాహం_check(now);
	*t = (u64_t) now + (ఎగువ_frc << 32);
}
