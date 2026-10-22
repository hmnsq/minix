#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ _OMAP_INTR_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _OMAP_INTR_H

/* Interrupt controller memory map */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OMAP3_DM37_XX_INTR_ਅਧਾਰ 0x48200000 /* INTCPS physical address */


/* Interrupt controller memory map */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OMAP3_AM335_X_INTR_ਅਧਾਰ 0x48200000 /* INTCPS physical address */

/* Interrupt controller registers */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OMAP3_INTCPS_REVISION     0x000 /* IP revision code */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OMAP3_INTCPS_SYSCONFIG    0x010 /* Controls params */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OMAP3_INTCPS_SYSSTATUS    0x014 /*ਸਥਿਤੀ*/
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OMAP3_INTCPS_SIR_IRQ      0x040 /* Active IRQ number */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OMAP3_INTCPS_SIR_FIQ      0x044 /* Active FIQ number */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OMAP3_INTCPS_CONTROL      0x048 /* New int agreement bits */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OMAP3_INTCPS_PROTECTION   0x04C /* Protection for other regs */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OMAP3_INTCPS_IDLE         0x050 /* Clock auto-idle/gating */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OMAP3_INTCPS_IRQ_ਤਰਜੀਹ 0x060 /* Active IRQ priority level */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OMAP3_INTCPS_FIQ_ਤਰਜੀਹ 0x064 /* Active FIQ priority level */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OMAP3_INTCPS_ਹੱਦ    0x068 /* Priority threshold */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OMAP3_INTCPS_ITR0         0x080 /* Raw pre-masking interrupt status */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OMAP3_INTCPS_MIR0         0x084 /* Interrupt mask */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OMAP3_INTCPS_MIR_CLEAR0   0x088 /* Clear interrupt mask bits */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OMAP3_INTCPS_MIR_SET0     0x08C /* Set interrupt mask bits */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OMAP3_INTCPS_ISR_SET0     0x090 /* Set software int bits */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OMAP3_INTCPS_ISR_CLEAR0   0x094 /* Clear software int bits */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OMAP3_INTCPS_PENDING_IRQ0 0x098 /* IRQ status post-masking */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OMAP3_INTCPS_PENDING_FIQ0 0x09C /* FIQ status post-masking */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OMAP3_INTCPS_ILR0         0x100 /* Priority for interrupts */


#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OMAP3_INTR_ITR(ਅਧਾਰ,n) \
    (ਅਧਾਰ + OMAP3_INTCPS_ITR0 + 0x20 * (n))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OMAP3_INTR_MIR(ਅਧਾਰ,n) \
    (ਅਧਾਰ + OMAP3_INTCPS_MIR0 + 0x20 * (n))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OMAP3_INTR_MIR_CLEAR(ਅਧਾਰ,n)	\
    (ਅਧਾਰ + OMAP3_INTCPS_MIR_CLEAR0 + 0x20 * (n))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OMAP3_INTR_MIR_ਸੈੱਟ(ਅਧਾਰ,n) \
    (ਅਧਾਰ + OMAP3_INTCPS_MIR_SET0 + 0x20 * (n))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OMAP3_INTR_ISR_ਸੈੱਟ(ਅਧਾਰ,n) \
    (ਅਧਾਰ + OMAP3_INTCPS_ISR_SET0 + 0x20 * (n))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OMAP3_INTR_ISR_CLEAR(ਅਧਾਰ,n) \
    (ਅਧਾਰ + OMAP3_INTCPS_ISR_CLEAR0 + 0x20 * (n))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OMAP3_INTR_PENDING_IRQ(ਅਧਾਰ,n) \
    (ਅਧਾਰ + OMAP3_INTCPS_PENDING_IRQ0 + 0x20 * (n))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OMAP3_INTR_PENDING_FIQ(ਅਧਾਰ,n) \
    (ਅਧਾਰ + OMAP3_INTCPS_PENDING_FIQ0 + 0x20 * (n))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OMAP3_INTR_ILR(ਅਧਾਰ,m) \
    (ਅਧਾਰ + OMAP3_INTCPS_ILR0 + 0x4 * (m))

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OMAP3_INTR_ACTIVEIRQ_ਨਕਾਬ 0x7f /* Active IRQ mask for SIR_IRQ */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OMAP3_INTR_NEWIRQAGR      0x1  /* New IRQ Generation */




#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OMAP3_DM337X_NR_IRQ_VECTORS    96

/* Interrupt mappings */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OMAP3_MCBSP2_ST_IRQ  4  /* Sidestone McBSP2 overflow */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OMAP3_MCBSP3_ST_IRQ  5  /* Sidestone McBSP3 overflow */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OMAP3_SYS_NIRQ       7  /* External source (active low) */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OMAP3_SMX_DBG_IRQ    9  /* L3 interconnect error for debug */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OMAP3_SMX_APP_IRQ   10  /* L3 interconnect error for application */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OMAP3_PRCM_IRQ      11  /* PRCM module */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OMAP3_SDMA0_IRQ     12  /* System DMA request 0 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OMAP3_SDMA1_IRQ     13  /* System DMA request 1 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OMAP3_SDMA2_IRQ     14  /* System DMA request 2 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OMAP3_SDMA3_IRQ     15  /* System DMA request 3 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OMAP3_MCBSP1_IRQ    16  /* McBSP module 1 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OMAP3_MCBSP2_IRQ    17  /* McBSP module 2 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OMAP3_GPMC_IRQ      20  /* General-purpose memory controller */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OMAP3_SGX_IRQ       21  /* 2D/3D graphics module */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OMAP3_MCBSP3_IRQ    22  /* McBSP module 3 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OMAP3_MCBSP4_IRQ    23  /* McBSP module 4 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OMAP3_CAM0_IRQ      24  /* Camera interface request 0 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OMAP3_DSS_IRQ       25  /* Display subsystem module */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OMAP3_MAIL_U0_IRQ   26  /* Mailbox user 0 request */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OMAP3_MCBSP5_IRQ    27  /* McBSP module 5 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OMAP3_IVA2_MMU_IRQ  28  /* IVA2 MMU */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OMAP3_GPIO1_IRQ     29  /* GPIO module 1 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OMAP3_GPIO2_IRQ     30  /* GPIO module 2 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OMAP3_GPIO3_IRQ     31  /* GPIO module 3 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OMAP3_GPIO4_IRQ     32  /* GPIO module 4 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OMAP3_GPIO5_IRQ     33  /* GPIO module 5 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OMAP3_GPIO6_IRQ     34  /* GPIO module 6 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OMAP3_WDT3_IRQ      36  /* Watchdog timer module 3 overflow */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OMAP3_GPT1_IRQ      37  /* General-purpose timer module 1 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OMAP3_GPT2_IRQ      38  /* General-purpose timer module 2 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OMAP3_GPT3_IRQ      39  /* General-purpose timer module 3 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OMAP3_GPT4_IRQ      40  /* General-purpose timer module 4 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OMAP3_GPT5_IRQ      41  /* General-purpose timer module 5 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OMAP3_GPT6_IRQ      42  /* General-purpose timer module 6 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OMAP3_GPT7_IRQ      43  /* General-purpose timer module 7 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OMAP3_GPT8_IRQ      44  /* General-purpose timer module 8 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OMAP3_GPT9_IRQ      45  /* General-purpose timer module 9 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OMAP3_GPT10_IRQ     46  /* General-purpose timer module 10 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OMAP3_GPT11_IRQ     47  /* General-purpose timer module 11 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OMAP3_SPI4_IRQ      48  /* McSPI module 4 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OMAP3_MCBSP4_TX_IRQ 54  /* McBSP module 4 transmit */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OMAP3_MCBSP4_RX_IRQ 55  /* McBSP module 4 receive */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OMAP3_I2C1_IRQ      56  /* I2C module 1 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OMAP3_I2C2_IRQ      57  /* I2C module 2 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OMAP3_HDQ_IRQ       58  /* HDQ/1-Wire */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OMAP3_MCBSP1_TX_IRQ 59  /* McBSP module 1 transmit */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OMAP3_MCBSP1_RX_IRQ 60  /* McBSP module 1 receive */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OMAP3_I2C3_IRQ      61  /* I2C module 3 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OMAP3_MCBSP2_TX_IRQ 62  /* McBSP module 2 transmit */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OMAP3_MCBSP2_RX_IRQ 63  /* McBSP module 2 receive */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OMAP3_SPI1_IRQ      65  /* McSPI module 1 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OMAP3_SPI2_IRQ      66  /* McSPI module 2 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OMAP3_UART1_IRQ     72  /* UART module 1 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OMAP3_UART2_IRQ     73  /* UART module 2 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OMAP3_PBIAS_IRQ     75  /* Merged interrupt for PBIASlite 1/2 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OMAP3_OHCI_IRQ      76  /* OHCI HSUSB MP Host Interrupt */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OMAP3_EHCI_IRQ      77  /* EHCI HSUSB MP Host Interrupt */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OMAP3_TLL_IRQ       78  /* HSUSB MP TLL Interrupt */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OMAP3_MCBSP5_TX_IRQ 81  /* McBSP module 5 transmit */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OMAP3_MCBSP5_RX_IRQ 82  /* McBSP module 5 receive */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OMAP3_MMC1_IRQ      83  /* MMC/SD module 1 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OMAP3_MMC2_IRQ      86  /* MMC/SD module 2 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OMAP3_ICR_IRQ       87  /* MPU ICR */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OMAP3_D2DFRINT_IRQ  88  /* 3G coproc (in stacked modem config) */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OMAP3_MCBSP3_TX_IRQ 89  /* McBSP module 3 transmit */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OMAP3_MCBSP3_RX_IRQ 90  /* McBSP module 3 receive */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OMAP3_SPI3_IRQ      91  /* McSPI module 3 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OMAP3_HSUSB_MC_IRQ  92  /* High-speed USB OTG */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OMAP3_HSUSB_DMA_IRQ 93  /* High-speed USB OTG DMA */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OMAP3_MMC3_IRQ      94  /* MMC/SD module 3 */


#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AM335X_INT_EMUINT                         0	/* Emulation interrupt (EMUICINTR) */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AM335X_INT_COMMTX                         1	/* CortexA8 COMMTX */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AM335X_INT_COMMRX                         2	/* CortexA8 COMMRX */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AM335X_INT_BENCH                          3	/* CortexA8 NPMUIRQ */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AM335X_INT_ELM_IRQ                        4	/* Sinterrupt (Error location process completion) */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AM335X_INT_NMI                            7	/* nmi_int */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AM335X_INT_L3DEBUG                        9	/* l3_FlagMux_top_FlagOut1 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AM335X_INT_L3APPINT                       10	/* l3_FlagMux_top_FlagOut0  */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AM335X_INT_PRCMINT                        11	/* irq_mpu */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AM335X_INT_EDMACOMPINT                    12	/* tpcc_int_pend_po0 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AM335X_INT_EDMAMPERR                      13	/* tpcc_mpint_pend_po */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AM335X_INT_EDMAERRINT                     14	/* tpcc_errint_pend_po */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AM335X_INT_ADC_TSC_GENINT                 16	/* gen_intr_pend */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AM335X_INT_USBSSINT                       17	/* usbss_intr_pend */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AM335X_INT_USB0                           18	/* usb0_intr_pend */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AM335X_INT_USB1                           19	/* usb1_intr_pend */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AM335X_INT_PRUSS1_EVTOUT0                 20	/* pr1_host_intr0_intr_pend */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AM335X_INT_PRUSS1_EVTOUT1                 21	/* pr1_host_intr1_intr_pend */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AM335X_INT_PRUSS1_EVTOUT2                 22	/* pr1_host_intr2_intr_pend */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AM335X_INT_PRUSS1_EVTOUT3                 23	/* pr1_host_intr3_intr_pend */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AM335X_INT_PRUSS1_EVTOUT4                 24	/* pr1_host_intr4_intr_pend */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AM335X_INT_PRUSS1_EVTOUT5                 25	/* pr1_host_intr5_intr_pend */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AM335X_INT_PRUSS1_EVTOUT6                 26	/* pr1_host_intr6_intr_pend */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AM335X_INT_PRUSS1_EVTOUT7                 27	/* pr1_host_intr7_intr_pend */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AM335X_INT_MMCSD1INT                      28	/* MMCSD1  SINTERRUPTN */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AM335X_INT_MMCSD2INT                      29	/* MMCSD2  SINTERRUPT */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AM335X_INT_I2C2INT                        30	/* I2C2  POINTRPEND */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AM335X_INT_eCAP0INT                       31	/* ecap_intr_intr_pend */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AM335X_INT_GPIOINT2A                      32	/* GPIO 2  POINTRPEND1 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AM335X_INT_GPIOINT2B                      33	/* GPIO 2  POINTRPEND2 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AM335X_INT_USBWAKEUP                      34	/* USBSS  slv0p_Swakeup */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AM335X_INT_LCDCINT                        36	/* LCDC  lcd_irq */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AM335X_INT_GFXINT                         37	/* SGX530  THALIAIRQ */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AM335X_INT_ePWM2INT                       39	/* (PWM Subsystem)  epwm_intr_intr_pend */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AM335X_INT_3PGSWRXTHR0                    40	/* (Ethernet)  c0_rx_thresh_pend (RX_THRESH_PULSE) */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AM335X_INT_3PGSWRXINT0                    41	/* CPSW (Ethernet)  c0_rx_pend */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AM335X_INT_3PGSWTXINT0                    42	/* CPSW (Ethernet)  c0_tx_pend */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AM335X_INT_3PGSWMISC0                     43	/* CPSW (Ethernet)  c0_misc_pend */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AM335X_INT_UART3INT                       44	/* UART3  niq */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AM335X_INT_UART4INT                       45	/* UART4  niq */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AM335X_INT_UART5INT                       46	/* UART5  niq */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AM335X_INT_eCAP1INT                       47	/* (PWM Subsystem)  ecap_intr_intr_pend */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AM335X_INT_DCAN0_INT0                     52	/* DCAN0  dcan_intr0_intr_pend */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AM335X_INT_DCAN0_INT1                     53	/* DCAN0  dcan_intr1_intr_pend */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AM335X_INT_DCAN0_PARITY                   54	/* DCAN0  dcan_uerr_intr_pend */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AM335X_INT_DCAN1_INT0                     55	/* DCAN1  dcan_intr0_intr_pend */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AM335X_INT_DCAN1_INT1                     56	/* DCAN1  dcan_intr1_intr_pend */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AM335X_INT_DCAN1_PARITY                   57	/* DCAN1  dcan_uerr_intr_pend */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AM335X_INT_ePWM0_TZINT                    58	/* eHRPWM0 TZ interrupt (PWM  epwm_tz_intr_pend Subsystem) */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AM335X_INT_ePWM1_TZINT                    59	/* eHRPWM1 TZ interrupt (PWM  epwm_tz_intr_pend Subsystem) */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AM335X_INT_ePWM2_TZINT                    60	/* eHRPWM2 TZ interrupt (PWM  epwm_tz_intr_pend Subsystem) */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AM335X_INT_eCAP2INT                       61	/* eCAP2 (PWM Subsystem)  ecap_intr_intr_pend */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AM335X_INT_GPIOINT3A                      62	/* GPIO 3  POINTRPEND1 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AM335X_INT_GPIOINT3B                      63	/* GPIO 3  POINTRPEND2 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AM335X_INT_MMCSD0INT                      64	/* MMCSD0  SINTERRUPTN */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AM335X_INT_SPI0INT                        65	/* McSPI0  SINTERRUPTN */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AM335X_INT_TINT0                          66	/* Timer0  POINTR_PEND */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AM335X_INT_TINT1_1MS                      67	/* DMTIMER_1ms  POINTR_PEND */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AM335X_INT_TINT2                          68	/* DMTIMER2  POINTR_PEND */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AM335X_INT_TINT3                          69	/* DMTIMER3  POINTR_PEND */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AM335X_INT_I2C0INT                        70	/* I2C0  POINTRPEND */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AM335X_INT_I2C1INT                        71	/* I2C1  POINTRPEND */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AM335X_INT_UART0INT                       72	/* UART0  niq */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AM335X_INT_UART1INT                       73	/* UART1  niq */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AM335X_INT_UART2INT                       74	/* UART2  niq */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AM335X_INT_RTCINT                         75	/* RTC  timer_intr_pend */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AM335X_INT_RTCALARMINT                    76	/* RTC  alarm_intr_pend */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AM335X_INT_MBINT0                         77	/* Mailbox0 (mail_u0_irq)  initiator_sinterrupt_q_n */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AM335X_INT_M3_TXEV                        78	/* Wake M3 Subsystem  TXEV */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AM335X_INT_eQEP0INT                       79	/* eQEP0 (PWM Subsystem)  eqep_intr_intr_pend */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AM335X_INT_MCATXINT0                      80	/* McASP0  mcasp_x_intr_pend */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AM335X_INT_MCARXINT0                      81	/* McASP0  mcasp_r_intr_pend */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AM335X_INT_MCATXINT1                      82	/* McASP1  mcasp_x_intr_pend */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AM335X_INT_MCARXINT1                      83	/* McASP1  mcasp_r_intr_pend */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AM335X_INT_ePWM0INT                       86	/* (PWM Subsystem)  epwm_intr_intr_pend */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AM335X_INT_ePWM1INT                       87	/* (PWM Subsystem)  epwm_intr_intr_pend */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AM335X_INT_eQEP1INT                       88	/* (PWM Subsystem)  eqep_intr_intr_pend */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AM335X_INT_eQEP2INT                       89	/* (PWM Subsystem)  eqep_intr_intr_pend */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AM335X_INT_DMA_INTR_PIN2                  90	/* External DMA/Interrupt Pin2  pi_x_dma_event_intr2 (xdma_event_intr2) */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AM335X_INT_WDT1INT                        91	/* (Public Watchdog)  WDTIMER1  PO_INT_PEND */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AM335X_INT_TINT4                          92	/* DMTIMER4  POINTR_PEN */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AM335X_INT_TINT5                          93	/* DMTIMER5  POINTR_PEN */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AM335X_INT_TINT6                          94	/* DMTIMER6  POINTR_PEND */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AM335X_INT_TINT7                          95	/* DMTIMER7  POINTR_PEND */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AM335X_INT_GPIOINT0A                      96	/* GPIO 0  POINTRPEND1 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AM335X_INT_GPIOINT0B                      97	/* GPIO 0  POINTRPEND2 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AM335X_INT_GPIOINT1A                      98	/* GPIO 1  POINTRPEND1 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AM335X_INT_GPIOINT1B                      99	/* GPIO 1  POINTRPEND2 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AM335X_INT_GPMCINT                        100	/* GPMC  gpmc_sinterrupt */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AM335X_INT_DDRERR0                        101	/* EMIF  sys_err_intr_pend */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AM335X_INT_TCERRINT0                      112	/* TPTC0  tptc_erint_pend_po */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AM335X_INT_TCERRINT1                      113	/* TPTC1  tptc_erint_pend_po */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AM335X_INT_TCERRINT2                      114	/* TPTC2  tptc_erint_pend_po */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AM335X_INT_ADC_TSC_PENINT                 115	/* ADC_TSC  pen_intr_pend */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AM335X_INT_SMRFLX_Sabertooth              120	/* Smart Reflex 0  intrpen */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AM335X_INT_SMRFLX_Core                    121	/* Smart Reflex 1  intrpend */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AM335X_INT_DMA_INTR_PIN0                  123	/* pi_x_dma_event_intr0 (xdma_event_intr0) */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AM335X_INT_DMA_INTR_PIN1                  124	/* pi_x_dma_event_intr1 (xdma_event_intr1) */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AM335X_INT_SPI1INT                        125	/* McSPI1  SINTERRUPTN */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OMAP3_AM335X_NR_IRQ_VECTORS    125 

#ਜੇ_ਅੰਤ /* _OMAP_INTR_H */
