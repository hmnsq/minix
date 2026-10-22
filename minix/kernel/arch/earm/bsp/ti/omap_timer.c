#अन्तर्भाव "kernel/kernel.h"
#अन्तर्भाव "kernel/clock.h"
#अन्तर्भाव <sys/types.h>
#अन्तर्भाव <machine/cpu.h>
#अन्तर्भाव <minix/board.h>
#अन्तर्भाव <minix/mmio.h>
#अन्तर्भाव <assert.h>
#अन्तर्भाव <io.h>
#अन्तर्भाव <stdlib.h>
#अन्तर्भाव <stdio.h>
#अन्तर्भाव "arch_proto.h"
#अन्तर्भाव "bsp_timer.h"
#अन्तर्भाव "omap_timer_registers.h"
#अन्तर्भाव "omap_intr_registers.h"
#अन्तर्भाव "bsp_intr.h"

/* interrupt handler hook */
स्थिर irq_hook_t omap3_timer_hook;
स्थिर u64_t उच्चः_frc;

संरचना omap_timer_registers;

संरचना omap_timer
{
	vir_अष्टकानि आधारः;
	पूर्णाङ्क irq_nr;
	संरचना omap_timer_registers *regs;
};

संरचना omap_timer_registers
{
	vir_अष्टकानि TIDR;
	vir_अष्टकानि TIOCP_CFG;
	vir_अष्टकानि TISTAT;
	vir_अष्टकानि TISR;
	vir_अष्टकानि TIER;
	vir_अष्टकानि TWER;
	vir_अष्टकानि TCLR;
	vir_अष्टकानि TCRR;
	vir_अष्टकानि TLDR;
	vir_अष्टकानि TTGR;
	vir_अष्टकानि TWPS;
	vir_अष्टकानि TMAR;
	vir_अष्टकानि TCAR1;
	vir_अष्टकानि TSICR;
	vir_अष्टकानि TCAR2;
	vir_अष्टकानि TPIR;
	vir_अष्टकानि TNIR;
	vir_अष्टकानि TCVR;
	vir_अष्टकानि TOCR;
	vir_अष्टकानि TOWR;

};

स्थिर संरचना omap_timer_registers regs_v1 = {
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
स्थिर संरचना omap_timer_registers regs_v2 = {
	.TIDR = AM335X_TIMER_TIDR,
	.TIOCP_CFG = AM335X_TIMER_TIOCP_CFG,
	.TISTAT = AM335X_TIMER_IRQSTATUS_RAW,
	.TISR = AM335X_TIMER_IRQSTATUS,
	.TIER = AM335_X_TIMER_IRQENABLE_समुच्चयः,
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

स्थिर संरचना omap_timer dm37xx_timer = {
	.आधारः = OMAP3_GPTIMER1_आधारः,
	.irq_nr = OMAP3_GPT1_IRQ,
	.regs = &regs_v1
};

/* free running timer */
स्थिर संरचना omap_timer dm37xx_fr_timer = {
	.आधारः = OMAP3_GPTIMER10_आधारः,
	.irq_nr = OMAP3_GPT10_IRQ,
	.regs = &regs_v1
};

/* normal timer */
स्थिर संरचना omap_timer am335x_timer = {
	.आधारः = AM335_X_DMTIMER1_1_MS_आधारः,
	.irq_nr = AM335X_INT_TINT1_1MS,
	.regs = &regs_v1
};

/* free running timer */
स्थिर संरचना omap_timer am335x_fr_timer = {
	.आधारः = AM335_X_DMTIMER7_आधारः,
	.irq_nr = AM335X_INT_TINT7,
	.regs = &regs_v2
};

स्थिर संरचना omap_timer *timer;
स्थिर संरचना omap_timer *fr_timer;

स्थिर पूर्णाङ्क समाप्तम् = 0;

पूर्णाङ्क
bsp_पञ्जिकाकोशः_timer_handler(ध्रुव irq_handler_t handler)
{
	/* Initialize the CLOCK's interrupt hook. */
	omap3_timer_hook.proc_nr_e = NONE;
	omap3_timer_hook.irq = timer->irq_nr;

	put_irq_handler(&omap3_timer_hook, timer->irq_nr, handler);
	/* only unmask interrupts after registering */
	bsp_irq_unmask(timer->irq_nr);

	प्रत्यागम 0;
}

/* meta data for remapping */
स्थिर kern_phys_map timer_phys_map;
स्थिर kern_phys_map fr_timer_phys_map;
स्थिर kern_phys_map fr_timer_user_phys_map;	/* struct for when the free */
						/* running timer is mapped to */
						/* userland */

/* callback for when the free running clock gets mapped */
पूर्णाङ्क
kern_phys_fr_user_mapped(vir_अष्टकानि id, phys_अष्टकानि स्थानाङ्कः)
{
	/* the only thing we need to do at this stage is to set the address */
	/* in the kerninfo struct */
	यदि (BOARD_IS_BBXM(machine.board_id)) {
		arm_frclock.tcrr = स्थानाङ्कः + OMAP3_TIMER_TCRR;
		arm_frclock.hz = 1625000;
	} अन्यथा यदि (BOARD_IS_BB(machine.board_id)) {
		arm_frclock.tcrr = स्थानाङ्कः + AM335X_TIMER_TCRR;
		arm_frclock.hz = 1500000;
	}
	प्रत्यागम 0;
}

शून्य
omap3_frclock_init(शून्य)
{
	u32_t tisr;

	/* enable the clock */
	यदि (BOARD_IS_BBXM(machine.board_id)) {
		fr_timer = &dm37xx_fr_timer;

		kern_phys_map_ptr(fr_timer->आधारः, ARM_PAGE_आकार,
		    VMMF_UNCACHED | VMMF_WRITE, &fr_timer_phys_map,
		    (vir_अष्टकानि) & fr_timer->आधारः);

		/* the timer is also mapped in user space hence the this */
		/* second mapping and callback to set kerninfo frclock_tcrr */
		kern_req_phys_map(fr_timer->आधारः, ARM_PAGE_आकार,
		    VMMF_UNCACHED | VMMF_USER,
		    &fr_timer_user_phys_map, kern_phys_fr_user_mapped, 0);

		/* Stop timer */
		mmio_clear(fr_timer->आधारः + fr_timer->regs->TCLR,
		    OMAP3_TCLR_ST);

		/* Use functional clock source for GPTIMER10 */
		mmio_समुच्चयः(OMAP3_CM_CLKSEL_CORE, OMAP3_CLKSEL_GPT10);

		/* Scale timer down to 13/8 = 1.625 Mhz to roughly get
		 * microsecond ticks */
		/* The scale is computed as 2^(PTV+1). So if PTV == 2, we get
		 * 2^3 = 8. */
		mmio_समुच्चयः(fr_timer->आधारः + fr_timer->regs->TCLR,
		    (2 << OMAP3_TCLR_PTV));
	} अन्यथा यदि (BOARD_IS_BB(machine.board_id)) {
		fr_timer = &am335x_fr_timer;
		kern_phys_map_ptr(fr_timer->आधारः, ARM_PAGE_आकार,
		    VMMF_UNCACHED | VMMF_WRITE,
		    &fr_timer_phys_map, (vir_अष्टकानि) & fr_timer->आधारः);

		/* the timer is also mapped in user space hence the this */
		/* second mapping and callback to set kerninfo frclock_tcrr */
		kern_req_phys_map(fr_timer->आधारः, ARM_PAGE_आकार,
		    VMMF_UNCACHED | VMMF_USER,
		    &fr_timer_user_phys_map, kern_phys_fr_user_mapped, 0);
		/* Disable the module and wait for the module to be disabled */
		set32(CM_प्रत्येकम्_TIMER7_CLKCTRL, CM_MODULEMODE_आवरणम्,
		    CM_MODULEMODE_DISABLED);
		यावत् ((mmio_read(CM_प्रत्येकम्_TIMER7_CLKCTRL) & CM_CLKCTRL_IDLEST)
		    != CM_CLKCTRL_IDLEST_DISABLE);

		set32(CLKSEL_TIMER7_CLK, CLKSEL_TIMER7_CLK_SEL_आवरणम्,
		    CLKSEL_TIMER7_CLK_SEL_SEL2);
		यावत् ((read32(CLKSEL_TIMER7_CLK) & CLKSEL_TIMER7_CLK_SEL_आवरणम्)
		    != CLKSEL_TIMER7_CLK_SEL_SEL2);

		/* enable the module and wait for the module to be ready */
		set32(CM_प्रत्येकम्_TIMER7_CLKCTRL, CM_MODULEMODE_आवरणम्,
		    CM_MODULEMODE_ENABLE);
		यावत् ((mmio_read(CM_प्रत्येकम्_TIMER7_CLKCTRL) & CM_CLKCTRL_IDLEST)
		    != CM_CLKCTRL_IDLEST_FUNC);

		/* Stop timer */
		mmio_clear(fr_timer->आधारः + fr_timer->regs->TCLR,
		    OMAP3_TCLR_ST);

		/* 24Mhz / 16 = 1.5 Mhz */
		mmio_समुच्चयः(fr_timer->आधारः + fr_timer->regs->TCLR,
		    (3 << OMAP3_TCLR_PTV));
	}

	/* Start and auto-reload at 0 */
	mmio_write(fr_timer->आधारः + fr_timer->regs->TLDR, 0x0);
	mmio_write(fr_timer->आधारः + fr_timer->regs->TCRR, 0x0);

	/* Set up overflow interrupt */
	tisr = OMAP3_TISR_MAT_IT_FLAG | OMAP3_TISR_OVF_IT_FLAG |
	    OMAP3_TISR_TCAR_IT_FLAG;
	/* Clear interrupt status */
	mmio_write(fr_timer->आधारः + fr_timer->regs->TISR, tisr);
	mmio_write(fr_timer->आधारः + fr_timer->regs->TIER,
	    OMAP3_TIER_OVF_IT_ENA);

	/* Start timer */
	mmio_समुच्चयः(fr_timer->आधारः + fr_timer->regs->TCLR,
	    OMAP3_TCLR_OVF_TRG | OMAP3_TCLR_AR | OMAP3_TCLR_ST |
	    OMAP3_TCLR_PRE);
	समाप्तम् = 1;
}

शून्य
omap3_frclock_stop(शून्य)
{
	mmio_clear(fr_timer->आधारः + fr_timer->regs->TCLR, OMAP3_TCLR_ST);
}

शून्य
bsp_timer_init(अचिह्नित freq)
{
	/* we only support 1ms resolution */
	u32_t tisr;
	यदि (BOARD_IS_BBXM(machine.board_id)) {
		timer = &dm37xx_timer;
		kern_phys_map_ptr(timer->आधारः, ARM_PAGE_आकार,
		    VMMF_UNCACHED | VMMF_WRITE,
		    &timer_phys_map, (vir_अष्टकानि) & timer->आधारः);
		/* Stop timer */
		mmio_clear(timer->आधारः + timer->regs->TCLR, OMAP3_TCLR_ST);

		/* Use 32 KHz clock source for GPTIMER1 */
		mmio_clear(OMAP3_CM_CLKSEL_WKUP, OMAP3_CLKSEL_GPT1);
	} अन्यथा यदि (BOARD_IS_BB(machine.board_id)) {
		timer = &am335x_timer;
		kern_phys_map_ptr(timer->आधारः, ARM_PAGE_आकार,
		    VMMF_UNCACHED | VMMF_WRITE,
		    &timer_phys_map, (vir_अष्टकानि) & timer->आधारः);
		/* disable the module and wait for the module to be disabled */
		set32(CM_WKUP_TIMER1_CLKCTRL, CM_MODULEMODE_आवरणम्,
		    CM_MODULEMODE_DISABLED);
		यावत् ((mmio_read(CM_WKUP_TIMER1_CLKCTRL) & CM_CLKCTRL_IDLEST)
		    != CM_CLKCTRL_IDLEST_DISABLE);

		set32(CLKSEL_TIMER1MS_CLK, CLKSEL_TIMER1_MS_CLK_SEL_आवरणम्,
		    CLKSEL_TIMER1MS_CLK_SEL_SEL2);
		यावत् ((read32(CLKSEL_TIMER1MS_CLK) &
			CLKSEL_TIMER1_MS_CLK_SEL_आवरणम्) !=
		    CLKSEL_TIMER1MS_CLK_SEL_SEL2);

		/* enable the module and wait for the module to be ready */
		set32(CM_WKUP_TIMER1_CLKCTRL, CM_MODULEMODE_आवरणम्,
		    CM_MODULEMODE_ENABLE);
		यावत् ((mmio_read(CM_WKUP_TIMER1_CLKCTRL) & CM_CLKCTRL_IDLEST)
		    != CM_CLKCTRL_IDLEST_FUNC);

		/* Stop timer */
		mmio_clear(timer->आधारः + timer->regs->TCLR, OMAP3_TCLR_ST);
	}

	/* Use 1-ms tick mode for GPTIMER1 TRM 16.2.4.2.1 */
	mmio_write(timer->आधारः + timer->regs->TPIR, 232000);
	mmio_write(timer->आधारः + timer->regs->TNIR, -768000);
	mmio_write(timer->आधारः + timer->regs->TLDR,
	    0xffffffff - (32768 / freq) + 1);
	mmio_write(timer->आधारः + timer->regs->TCRR,
	    0xffffffff - (32768 / freq) + 1);

	/* Set up overflow interrupt */
	tisr = OMAP3_TISR_MAT_IT_FLAG | OMAP3_TISR_OVF_IT_FLAG |
	    OMAP3_TISR_TCAR_IT_FLAG;
	/* Clear interrupt status */
	mmio_write(timer->आधारः + timer->regs->TISR, tisr);
	mmio_write(timer->आधारः + timer->regs->TIER, OMAP3_TIER_OVF_IT_ENA);

	/* Start timer */
	mmio_समुच्चयः(timer->आधारः + timer->regs->TCLR,
	    OMAP3_TCLR_OVF_TRG | OMAP3_TCLR_AR | OMAP3_TCLR_ST);
	/* also initilize the free runnning timer */
	omap3_frclock_init();
}

शून्य
bsp_timer_stop(शून्य)
{
	mmio_clear(timer->आधारः + timer->regs->TCLR, OMAP3_TCLR_ST);
}

स्थिर u32_t
read_frc(शून्य)
{
	यदि (समाप्तम् == 0) {
		प्रत्यागम 0;
	}
	प्रत्यागम mmio_read(fr_timer->आधारः + fr_timer->regs->TCRR);
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
स्थिर शून्य
frc_अतिप्रवाहः_check(u32_t cur_frc)
{
	स्थिर पूर्णाङ्क पूर्व_frc_valid;
	स्थिर u32_t पूर्व_frc;
	यदि (पूर्व_frc_valid && पूर्व_frc > cur_frc) {
		उच्चः_frc++;
	}
	पूर्व_frc = cur_frc;
	पूर्व_frc_valid = 1;
}

शून्य
bsp_timer_int_handler(शून्य)
{
	/* Clear all interrupts */
	u32_t tisr, now;

	/* when the kernel itself is running interrupts are disabled. We
	 * should therefore also read the overflow counter to detect this as
	 * to not miss events. */
	tisr = OMAP3_TISR_MAT_IT_FLAG | OMAP3_TISR_OVF_IT_FLAG |
	    OMAP3_TISR_TCAR_IT_FLAG;
	mmio_write(timer->आधारः + timer->regs->TISR, tisr);

	now = read_frc();
	frc_अतिप्रवाहः_check(now);
}

/* Use the free running clock as TSC */
शून्य
read_tsc_64(u64_t * t)
{
	u32_t now;
	now = read_frc();
	frc_अतिप्रवाहः_check(now);
	*t = (u64_t) now + (उच्चः_frc << 32);
}
