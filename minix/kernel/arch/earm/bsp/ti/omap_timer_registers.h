#جيڪڏهن_وصف_نه_ٿيل _OMAP_TIMER_REGISTERS_H
#وصف_ڪريو _OMAP_TIMER_REGISTERS_H


/* General-purpose timer register map */
#وصف_ڪريو OMAP3_GPTIMER1_بنياد  0x48318000 /* GPTIMER1 physical address */
#وصف_ڪريو OMAP3_GPTIMER2_بنياد  0x49032000 /* GPTIMER2 physical address */
#وصف_ڪريو OMAP3_GPTIMER3_بنياد  0x49034000 /* GPTIMER3 physical address */
#وصف_ڪريو OMAP3_GPTIMER4_بنياد  0x49036000 /* GPTIMER4 physical address */
#وصف_ڪريو OMAP3_GPTIMER5_بنياد  0x49038000 /* GPTIMER5 physical address */
#وصف_ڪريو OMAP3_GPTIMER6_بنياد  0x4903A000 /* GPTIMER6 physical address */
#وصف_ڪريو OMAP3_GPTIMER7_بنياد  0x4903C000 /* GPTIMER7 physical address */
#وصف_ڪريو OMAP3_GPTIMER8_بنياد  0x4903E000 /* GPTIMER8 physical address */
#وصف_ڪريو OMAP3_GPTIMER9_بنياد  0x49040000 /* GPTIMER9 physical address */
#وصف_ڪريو OMAP3_GPTIMER10_بنياد 0x48086000 /* GPTIMER10 physical address */
#وصف_ڪريو OMAP3_GPTIMER11_بنياد 0x48088000 /* GPTIMER11 physical address */


/* General-purpose timer registers */
#وصف_ڪريو OMAP3_TIMER_TIDR      0x000 /* IP revision code */
#وصف_ڪريو OMAP3_TIMER_TIOCP_CFG 0x010 /* Controls params for GP timer L4 interface */
#وصف_ڪريو OMAP3_TIMER_TISTAT    0x014 /* Status (excl. interrupt status) */
#وصف_ڪريو OMAP3_TIMER_TISR      0x018 /* Pending interrupt status */
#وصف_ڪريو OMAP3_TIMER_TIER      0x01C /* Interrupt enable */
#وصف_ڪريو OMAP3_TIMER_TWER      0x020 /* Wakeup enable */
#وصف_ڪريو OMAP3_TIMER_TCLR      0x024 /* Controls optional features */
#وصف_ڪريو OMAP3_TIMER_TCRR      0x028 /* Internal counter value */
#وصف_ڪريو OMAP3_TIMER_TLDR      0x02C /* Timer load value */
#وصف_ڪريو OMAP3_TIMER_TTGR      0x030 /* Triggers counter reload */
#وصف_ڪريو OMAP3_TIMER_TWPS      0x034 /* Indicates if Write-Posted pending */
#وصف_ڪريو OMAP3_TIMER_TMAR      0x038 /* Value to be compared with counter */
#وصف_ڪريو OMAP3_TIMER_TCAR1     0x03C /* First captured value of counter register */
#وصف_ڪريو OMAP3_TIMER_TSICR     0x040 /* Control posted mode and functional SW reset */
#وصف_ڪريو OMAP3_TIMER_TCAR2     0x044 /* Second captured value of counter register */
#وصف_ڪريو OMAP3_TIMER_TPIR      0x048 /* Positive increment (1 ms tick) */
#وصف_ڪريو OMAP3_TIMER_TNIR      0x04C /* Negative increment (1 ms tick) */
#وصف_ڪريو OMAP3_TIMER_TCVR      0x050 /* Defines TCRR is sub/over-period (1 ms tick) */
#وصف_ڪريو OMAP3_TIMER_TOCR      0x054 /* Masks tick interrupt */
#وصف_ڪريو OMAP3_TIMER_TOWR      0x058 /* Number of masked overflow interrupts */

#وصف_ڪريو AM335_X_DMTIMER0_بنياد      0x44E05000  /* DMTimer0 Registers */
#وصف_ڪريو AM335_X_DMTIMER1_1_MS_بنياد  0x44E31000 /* DMTimer1 1ms Registers (Accurate 1ms timer) */
#وصف_ڪريو AM335_X_DMTIMER2_بنياد      0x48040000 /*  DMTimer2 Registers */
#وصف_ڪريو AM335_X_DMTIMER3_بنياد      0x48042000 /*  DMTimer3 Registers */
#وصف_ڪريو AM335_X_DMTIMER4_بنياد      0x48044000 /* DMTimer4 Registers  */
#وصف_ڪريو AM335_X_DMTIMER5_بنياد      0x48046000 /* DMTimer5 Registers  */
#وصف_ڪريو AM335_X_DMTIMER6_بنياد      0x48048000 /*  DMTimer6 Registers */
#وصف_ڪريو AM335_X_DMTIMER7_بنياد      0x4804A000 /*  DMTimer7 Registers */

/* General-purpose timer registers  AM335x non 1MS timers have different offsets */
#وصف_ڪريو AM335X_TIMER_TIDR             0x000 /* IP revision code */
#وصف_ڪريو AM335X_TIMER_TIOCP_CFG        0x010 /* Controls params for GP timer L4 interface */
#وصف_ڪريو AM335X_TIMER_IRQSTATUS_RAW    0x024 /* Timer IRQSTATUS Raw Register */
#وصف_ڪريو AM335X_TIMER_IRQSTATUS        0x028 /* Timer IRQSTATUS Register */
#وصف_ڪريو AM335_X_TIMER_IRQENABLE_سيٽ    0x02C /* Timer IRQENABLE Set Register */
#وصف_ڪريو AM335X_TIMER_IRQENABLE_CLR    0x030 /* Timer IRQENABLE Clear Register */
#وصف_ڪريو AM335X_TIMER_IRQWAKEEN        0x034 /* Timer IRQ Wakeup Enable Register */
#وصف_ڪريو AM335X_TIMER_TCLR      0x038 /* Controls optional features */
#وصف_ڪريو AM335X_TIMER_TCRR      0x03C /* Internal counter value */
#وصف_ڪريو AM335X_TIMER_TLDR      0x040 /* Timer load value */
#وصف_ڪريو AM335X_TIMER_TTGR      0x044 /* Triggers counter reload */
#وصف_ڪريو AM335X_TIMER_TWPS      0x048 /* Indicates if Write-Posted pending */
#وصف_ڪريو AM335X_TIMER_TMAR      0x04C /* Value to be compared with counter */
#وصف_ڪريو AM335X_TIMER_TCAR1     0x050 /* First captured value of counter register */
#وصف_ڪريو AM335X_TIMER_TSICR     0x054 /* Control posted mode and functional SW reset */
#وصف_ڪريو AM335X_TIMER_TCAR2     0x058 /* Second captured value of counter register */

#وصف_ڪريو AM335_X_WDT_بنياد		0x44E35000	/* watchdog timer */
#وصف_ڪريو AM335X_WDT_WWPS		0x34		/* command posted status */
#وصف_ڪريو AM335X_WDT_WSPR		0x48		/* activate/deactivate sequence */

/* Interrupt status register fields */
#وصف_ڪريو OMAP3_TISR_MAT_IT_FLAG  (1 << 0) /* Pending match interrupt status */
#وصف_ڪريو OMAP3_TISR_OVF_IT_FLAG  (1 << 1) /* Pending overflow interrupt status */
#وصف_ڪريو OMAP3_TISR_TCAR_IT_FLAG (1 << 2) /* Pending capture interrupt status */

/* Interrupt enable register fields */
#وصف_ڪريو OMAP3_TIER_MAT_IT_ENA  (1 << 0) /* Enable match interrupt */
#وصف_ڪريو OMAP3_TIER_OVF_IT_ENA  (1 << 1) /* Enable overflow interrupt */
#وصف_ڪريو OMAP3_TIER_TCAR_IT_ENA (1 << 2) /* Enable capture interrupt */

/* Timer control fields */
#وصف_ڪريو OMAP3_TCLR_ST       (1 << 0)  /* Start/stop timer */
#وصف_ڪريو OMAP3_TCLR_AR       (1 << 1)  /* Autoreload or one-shot mode */
#وصف_ڪريو OMAP3_TCLR_PRE      (1 << 5)  /* Prescaler on */
#وصف_ڪريو OMAP3_TCLR_PTV      2
#وصف_ڪريو OMAP3_TCLR_OVF_TRG  (1 << 10) /* Overflow trigger */


#وصف_ڪريو OMAP3_CM_CLKSEL_GFX		0x48004b40
#وصف_ڪريو OMAP3_CM_CLKEN_PLL		0x48004d00
#وصف_ڪريو OMAP3_CM_FCLKEN1_CORE	0x48004A00
#وصف_ڪريو OMAP3_CM_CLKSEL_CORE	0x48004A40 /* GPT10 src clock sel. */
#وصف_ڪريو OMAP3_CM_FCLKEN_في		0x48005000
#وصف_ڪريو OMAP3_CM_CLKSEL_في		0x48005040
#وصف_ڪريو OMAP3_CM_CLKSEL_WKUP    0x48004c40 /* GPT1 source clock selection */


#وصف_ڪريو CM_MODULEMODE_نقاب (0x3 << 0)
#وصف_ڪريو CM_MODULEMODE_ENABLE      (0x2 << 0)
#وصف_ڪريو CM_MODULEMODE_DISABLED     (0x0 << 0)

#وصف_ڪريو CM_CLKCTRL_IDLEST         (0x3 << 16)
#وصف_ڪريو CM_CLKCTRL_IDLEST_FUNC    (0x0 << 16)
#وصف_ڪريو CM_CLKCTRL_IDLEST_TRANS   (0x1 << 16)
#وصف_ڪريو CM_CLKCTRL_IDLEST_IDLE    (0x2 << 16)
#وصف_ڪريو CM_CLKCTRL_IDLEST_DISABLE (0x3 << 16)

#وصف_ڪريو CM_WKUP_بنياد 0x44E00400 /* Clock Module Wakeup Registers */

#وصف_ڪريو CM_WKUP_TIMER1_CLKCTRL	(CM_WKUP_بنياد + 0xC4) /* This register manages the TIMER1 clocks. [Memory Mapped] */


#وصف_ڪريو CM_في_بنياد 0x44E00000 /* Clock Module Peripheral Registers */
#وصف_ڪريو CM_في_TIMER7_CLKCTRL	(CM_في_بنياد + 0x7C) /* This register manages the TIMER7 clocks. [Memory Mapped] */



/* CM_DPLL registers */


#وصف_ڪريو CM_DPLL_بنياد 	0x44E00500 /* Clock Module PLL Registers */

#وصف_ڪريو CLKSEL_TIMER1MS_CLK (CM_DPLL_بنياد + 0x28)


#وصف_ڪريو CLKSEL_TIMER1_MS_CLK_SEL_نقاب (0x7 << 0)
#وصف_ڪريو CLKSEL_TIMER1MS_CLK_SEL_SEL1 (0x0 << 0) /* Select CLK_M_OSC clock */
#وصف_ڪريو CLKSEL_TIMER1MS_CLK_SEL_SEL2 (0x1 << 0) /* Select CLK_32KHZ clock */
#وصف_ڪريو CLKSEL_TIMER1MS_CLK_SEL_SEL3 (0x2 << 0) /* Select TCLKIN clock */
#وصف_ڪريو CLKSEL_TIMER1MS_CLK_SEL_SEL4 (0x3 << 0) /* Select CLK_RC32K clock */
#وصف_ڪريو CLKSEL_TIMER1MS_CLK_SEL_SEL5 (0x4 << 0) /* Selects the CLK_32768 from 32KHz Crystal Osc */

#وصف_ڪريو CLKSEL_TIMER7_CLK   (CM_DPLL_بنياد + 0x04)
#وصف_ڪريو CLKSEL_TIMER7_CLK_SEL_نقاب (0x3 << 0)
#وصف_ڪريو CLKSEL_TIMER7_CLK_SEL_SEL1 (0x0 << 0) /* Select TCLKIN clock */
#وصف_ڪريو CLKSEL_TIMER7_CLK_SEL_SEL2 (0x1 << 0) /* Select CLK_M_OSC clock */
#وصف_ڪريو CLKSEL_TIMER7_CLK_SEL_SEL3 (0x2 << 0) /* Select CLK_32KHZ clock */
#وصف_ڪريو CLKSEL_TIMER7_CLK_SEL_SEL4 (0x3 << 0) /* Reserved */




#وصف_ڪريو OMAP3_CLKSEL_GPT1    (1 << 0)   /* Selects GPTIMER 1 source
					 * clock:
					 *
					 *  0: use 32KHz clock
					 *  1: sys clock)
					 */
#وصف_ڪريو OMAP3_CLKSEL_GPT10    (1 << 6)
#وصف_ڪريو OMAP3_CLKSEL_GPT11    (1 << 7)


#وصف_ڪريو TIMER_FREQ  1000    /* clock frequency for OMAP timer (1ms) */
#وصف_ڪريو TIMER_ڳڻپ(freq) (TIMER_FREQ/(freq)) /* initial value for counter*/

#جيڪڏهن_پورو /* _OMAP_TIMER_REGISTERS_H */
