/*
 * APIC handling routines. APIC is a requirement for SMP
 */
#ಅನ್ತರ್ಭಾವ <assert.h>

#ಅನ್ತರ್ಭಾವ <unistd.h>
#ಅನ್ತರ್ಭಾವ <minix/portio.h>

#ಅನ್ತರ್ಭಾವ <minix/syslib.h>
#ಅನ್ತರ್ಭಾವ <machine/cmos.h>

#ಅನ್ತರ್ಭಾವ <minix/u64.h>

#ಅನ್ತರ್ಭಾವ "apic.h"
#ಅನ್ತರ್ಭಾವ "apic_asm.h"
#ಅನ್ತರ್ಭಾವ "kernel/clock.h"
#ಅನ್ತರ್ಭಾವ "glo.h"
#ಅನ್ತರ್ಭಾವ "hw_intr.h"

#ಅನ್ತರ್ಭಾವ "acpi.h"

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_WATCHDOG
#ಅನ್ತರ್ಭಾವ "kernel/watchdog.h"
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿಸು APIC_ENABLE		0x100
#ವ್ಯಾಖ್ಯಾನಿಸು APIC_FOCUS_DISABLED	(1 << 9)
#ವ್ಯಾಖ್ಯಾನಿಸು APIC_SIV		0xFF

#ವ್ಯಾಖ್ಯಾನಿಸು APIC_TDCR_2	0x00
#ವ್ಯಾಖ್ಯಾನಿಸು APIC_TDCR_4	0x01
#ವ್ಯಾಖ್ಯಾನಿಸು APIC_TDCR_8	0x02
#ವ್ಯಾಖ್ಯಾನಿಸು APIC_TDCR_16	0x03
#ವ್ಯಾಖ್ಯಾನಿಸು APIC_TDCR_32	0x08
#ವ್ಯಾಖ್ಯಾನಿಸು APIC_TDCR_64	0x09
#ವ್ಯಾಖ್ಯಾನಿಸು APIC_TDCR_128	0x0a
#ವ್ಯಾಖ್ಯಾನಿಸು APIC_TDCR_1	0x0b

#ವ್ಯಾಖ್ಯಾನಿಸು IS_ಗಣ(ಮುಖವಾಡ)		(ಮುಖವಾಡ)
#ವ್ಯಾಖ್ಯಾನಿಸು IS_CLEAR(ಮುಖವಾಡ)		0

#ವ್ಯಾಖ್ಯಾನಿಸು APIC_LVTT_ಸದಿಶ_ಮುಖವಾಡ	0x000000FF
#ವ್ಯಾಖ್ಯಾನಿಸು APIC_LVTT_DS_PENDING	(1 << 12)
#ವ್ಯಾಖ್ಯಾನಿಸು APIC_LVTT_ಮುಖವಾಡ		(1 << 16)
#ವ್ಯಾಖ್ಯಾನಿಸು APIC_LVTT_TM		(1 << 17)

#ವ್ಯಾಖ್ಯಾನಿಸು APIC_LVT_IIPP_ಮುಖವಾಡ	0x00002000
#ವ್ಯಾಖ್ಯಾನಿಸು APIC_LVT_IIPP_AH	0x00002000
#ವ್ಯಾಖ್ಯಾನಿಸು APIC_LVT_IIPP_AL	0x00000000

#ವ್ಯಾಖ್ಯಾನಿಸು APIC_LVT_TM_ONESHOT	IS_CLEAR(APIC_LVTT_TM)
#ವ್ಯಾಖ್ಯಾನಿಸು APIC_LVT_TM_PERIODIC	IS_ಗಣ(APIC_LVTT_TM)

#ವ್ಯಾಖ್ಯಾನಿಸು IOAPIC_REGSEL		0x0
#ವ್ಯಾಖ್ಯಾನಿಸು IOAPIC_RW		0x10

#ವ್ಯಾಖ್ಯಾನಿಸು APIC_ICR_DM_ಮುಖವಾಡ		0x00000700
#ವ್ಯಾಖ್ಯಾನಿಸು APIC_ICR_ಸದಿಶ			APIC_LVTT_ಸದಿಶ_ಮುಖವಾಡ
#ವ್ಯಾಖ್ಯಾನಿಸು APIC_ICR_DM_FIXED		(0 << 8)
#ವ್ಯಾಖ್ಯಾನಿಸು APIC_ICR_DM_ಅತಿಕಡಿಮೆ_ಆದ್ಯತೆ	(1 << 8)
#ವ್ಯಾಖ್ಯಾನಿಸು APIC_ICR_DM_SMI			(2 << 8)
#ವ್ಯಾಖ್ಯಾನಿಸು APIC_ICR_DM_RESERVED		(3 << 8)
#ವ್ಯಾಖ್ಯಾನಿಸು APIC_ICR_DM_NMI			(4 << 8)
#ವ್ಯಾಖ್ಯಾನಿಸು APIC_ICR_DM_INIT		(5 << 8)
#ವ್ಯಾಖ್ಯಾನಿಸು APIC_ICR_DM_STARTUP		(6 << 8)
#ವ್ಯಾಖ್ಯಾನಿಸು APIC_ICR_DM_EXTINT		(7 << 8)

#ವ್ಯಾಖ್ಯಾನಿಸು APIC_ICR_DM_PHYSICAL		(0 << 11)
#ವ್ಯಾಖ್ಯಾನಿಸು APIC_ICR_DM_LOGICAL		(1 << 11)

#ವ್ಯಾಖ್ಯಾನಿಸು APIC_ICR_DELIVERY_PENDING	(1 << 12)

#ವ್ಯಾಖ್ಯಾನಿಸು APIC_ICR_INT_POLARITY		(1 << 13)
#ವ್ಯಾಖ್ಯಾನಿಸು APIC_ICR_INTPOL_ಕೆಳಗಿನ		IS_ಗಣ(APIC_ICR_INT_POLARITY)
#ವ್ಯಾಖ್ಯಾನಿಸು APIC_ICR_INTPOL_ಮೇಲಿನ		IS_CLEAR(APIC_ICR_INT_POLARITY)

#ವ್ಯಾಖ್ಯಾನಿಸು APIC_ICR_LEVEL_ASSERT		(1 << 14)
#ವ್ಯಾಖ್ಯಾನಿಸು APIC_ICR_LEVEL_DEASSERT		(0 << 14)

#ವ್ಯಾಖ್ಯಾನಿಸು APIC_ICR_TRIGGER		(1 << 15)
#ವ್ಯಾಖ್ಯಾನಿಸು APIC_ICR_TM_LEVEL		IS_CLEAR(APIC_ICR_TRIGGER)
#ವ್ಯಾಖ್ಯಾನಿಸು APIC_ICR_TM_ಅಂಚು		IS_CLEAR(APIC_ICR_TRIGGER)

#ವ್ಯಾಖ್ಯಾನಿಸು APIC_ICR_INT_ಮುಖವಾಡ		(1 << 16)

#ವ್ಯಾಖ್ಯಾನಿಸು APIC_ICR_DEST_ಕ್ಷೇತ್ರ		(0 << 18)
#ವ್ಯಾಖ್ಯಾನಿಸು APIC_ICR_DEST_SELF		(1 << 18)
#ವ್ಯಾಖ್ಯಾನಿಸು APIC_ICR_DEST_ALL		(2 << 18)
#ವ್ಯಾಖ್ಯಾನಿಸು APIC_ICR_DEST_ALL_BUT_SELF	(3 << 18)

#ವ್ಯಾಖ್ಯಾನಿಸು IA32_APIC_ತಳ	0x1b
#ವ್ಯಾಖ್ಯಾನಿಸು IA32_APIC_ತಳ_ENABLE_ಬಿಟ್	11

/* FIXME we should spread the irqs across as many priority levels as possible
 * due to buggy hw */
#ವ್ಯಾಖ್ಯಾನಿಸು LAPIC_ಸದಿಶ(irq)	(IRQ0_ಸದಿಶ +(irq))

#ವ್ಯಾಖ್ಯಾನಿಸು IOAPIC_IRQ_STATE_MASKED 0x1

/* currently only 2 interrupt priority levels are used */
#ವ್ಯಾಖ್ಯಾನಿಸು SPL0				0x0
#ವ್ಯಾಖ್ಯಾನಿಸು	SPLHI				0xF


ರಚನೆ io_apic io_apic[MAX_NR_IOAPICS];
ಚಿಹ್ನೆರಹಿತ nioapics;

ರಚನೆ irq;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಶೂನ್ಯ (* eoi_ವಿಧಾನ_t)(ರಚನೆ irq *);

ರಚನೆ irq {
	ರಚನೆ io_apic * 	ioa;
	ಚಿಹ್ನೆರಹಿತ		pin;
	ಚಿಹ್ನೆರಹಿತ		ಸದಿಶ;
	eoi_ವಿಧಾನ_t		eoi;
	ಚಿಹ್ನೆರಹಿತ		state;
};

ಸ್ಥಿರ ರಚನೆ irq io_apic_irq[NR_IRQ_VECTORS];

/* 
 * to make APIC work if SMP is not configured, we need to set the maximal number
 * of CPUS to 1, cpuid to return 0 and the current cpu is always BSP
 */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_SMP

#ಅನ್ತರ್ಭಾವ "kernel/smp.h"

#ಆದರೆ_ಅಂತ್ಯ

#ಅನ್ತರ್ಭಾವ "kernel/spinlock.h"


#ವ್ಯಾಖ್ಯಾನಿಸು lapic_write_icr1(val)	lapic_write(LAPIC_ICR1, val)
#ವ್ಯಾಖ್ಯಾನಿಸು lapic_write_icr2(val)	lapic_write(LAPIC_ICR2, val)

#ವ್ಯಾಖ್ಯಾನಿಸು lapic_read_icr1(x)	lapic_read(LAPIC_ICR1)
#ವ್ಯಾಖ್ಯಾನಿಸು lapic_read_icr2(x)	lapic_read(LAPIC_ICR2)

#ವ್ಯಾಖ್ಯಾನಿಸು is_boot_apic(apicid)	((apicid) == bsp_lapic_id)

#ವ್ಯಾಖ್ಯಾನಿಸು VERBOSE_APIC(x) x

ಪೂರ್ಣಾಂಕ ioapic_enabled;
u32_t lapic_addr_vaddr;
vir_ಬೈಟುಗಳು lapic_addr;
vir_ಬೈಟುಗಳು lapic_eoi_addr;
ಪೂರ್ಣಾಂಕ bsp_lapic_id;

ಸ್ಥಿರ ಅಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ probe_ticks;
ಸ್ಥಿರ	u64_t tsc0, tsc1;
ಸ್ಥಿರ	u32_t lapic_tctr0, lapic_tctr1;

ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ apic_imcrp;
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ nlints = 0;

ಶೂನ್ಯ arch_eoi(ಶೂನ್ಯ)
{
	apic_eoi();
}

/*
 * FIXME this should be a cpulocal variable but there are some problems with
 * arch specific cpulocals. As this variable is write-once-read-only it is ok to
 * have at as an array until we resolve the cpulocals properly
 */
ಸ್ಥಿರ u32_t lapic_bus_freq[CONFIG_MAX_CPUS];
/* the probe period will be roughly 100ms */
#ವ್ಯಾಖ್ಯಾನಿಸು PROBE_TICKS	(system_hz / 10)

#ವ್ಯಾಖ್ಯಾನಿಸು IOAPIC_IOREGSEL	0x0
#ವ್ಯಾಖ್ಯಾನಿಸು IOAPIC_IOWIN	0x10

ಸ್ಥಿರ u32_t ioapic_read(u32_t ioa_ತಳ, u32_t reg)
{
	*((ಅಸ್ಥಿರ u32_t *)(ioa_ತಳ + IOAPIC_IOREGSEL)) = (reg & 0xff);
	ಹಿಂತಿರುಗಿ *(ಅಸ್ಥಿರ u32_t *)(ioa_ತಳ + IOAPIC_IOWIN);
}

ಸ್ಥಿರ ಶೂನ್ಯ ioapic_write(u32_t ioa_ತಳ, u8_t reg, u32_t val)
{
	*((ಅಸ್ಥಿರ u32_t *)(ioa_ತಳ + IOAPIC_IOREGSEL)) = reg;
	*((ಅಸ್ಥಿರ u32_t *)(ioa_ತಳ + IOAPIC_IOWIN)) = val;
}

ಶೂನ್ಯ lapic_microsec_sleep(ಚಿಹ್ನೆರಹಿತ ಎಣಿಕೆ_2);
ಶೂನ್ಯ apic_idt_init(ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ reset);

ಸ್ಥಿರ ಶೂನ್ಯ ioapic_enable_pin(vir_ಬೈಟುಗಳು ioapic_addr, ಪೂರ್ಣಾಂಕ pin)
{
	u32_t lo = ioapic_read(ioapic_addr, IOAPIC_REDIR_ಕೋಷ್ಟಕ + pin * 2);

	lo &= ~APIC_ICR_INT_ಮುಖವಾಡ;
	ioapic_write(ioapic_addr, IOAPIC_REDIR_ಕೋಷ್ಟಕ + pin * 2, lo);
}

ಸ್ಥಿರ ಶೂನ್ಯ ioapic_disable_pin(vir_ಬೈಟುಗಳು ioapic_addr, ಪೂರ್ಣಾಂಕ pin)
{
	u32_t lo = ioapic_read(ioapic_addr, IOAPIC_REDIR_ಕೋಷ್ಟಕ + pin * 2);

	lo |= APIC_ICR_INT_ಮುಖವಾಡ;
	ioapic_write(ioapic_addr, IOAPIC_REDIR_ಕೋಷ್ಟಕ + pin * 2, lo);
}

#ಆದರೆ 0
ಸ್ಥಿರ ಶೂನ್ಯ ioapic_redirt_ನಮೂದು_read(ಶೂನ್ಯ * ioapic_addr,
					ಪೂರ್ಣಾಂಕ ನಮೂದು,
					u32_t *hi,
					u32_t *lo)
{
	*lo = ioapic_read((u32_t)ioapic_addr, (u8_t) (IOAPIC_REDIR_ಕೋಷ್ಟಕ + ನಮೂದು * 2));
	*hi = ioapic_read((u32_t)ioapic_addr, (u8_t) (IOAPIC_REDIR_ಕೋಷ್ಟಕ + ನಮೂದು * 2 + 1));

}
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ ಶೂನ್ಯ ioapic_redirt_ನಮೂದು_write(ಶೂನ್ಯ * ioapic_addr,
					ಪೂರ್ಣಾಂಕ ನಮೂದು,
					u32_t hi,
					u32_t lo)
{
#ಆದರೆ 0
	VERBOSE_APIC(ಬರೆ("IO apic redir entry %3d "
				"write 0x%08x 0x%08x\n", ನಮೂದು, hi, lo));
#ಆದರೆ_ಅಂತ್ಯ
	ioapic_write((u32_t)ioapic_addr, (u8_t) (IOAPIC_REDIR_ಕೋಷ್ಟಕ + ನಮೂದು * 2 + 1), hi);
	ioapic_write((u32_t)ioapic_addr, (u8_t) (IOAPIC_REDIR_ಕೋಷ್ಟಕ + ನಮೂದು * 2), lo);
}

#ವ್ಯಾಖ್ಯಾನಿಸು apic_read_tmr_ಸದಿಶ(vec) \
		lapic_read(LAPIC_TMR + 0x10 * ((vec) >> 5))

#ವ್ಯಾಖ್ಯಾನಿಸು apic_read_irr_ಸದಿಶ(vec) \
		lapic_read(LAPIC_IRR + 0x10 * ((vec) >> 5))

#ವ್ಯಾಖ್ಯಾನಿಸು apic_read_isr_ಸದಿಶ(vec) \
		lapic_read(LAPIC_ISR + 0x10 * ((vec) >> 5))

#ವ್ಯಾಖ್ಯಾನಿಸು lapic_test_delivery_val(val, ಸದಿಶ) ((val) & (1 << ((ಸದಿಶ) & 0x1f)))

ಸ್ಥಿರ ಶೂನ್ಯ ioapic_eoi_level(ರಚನೆ irq * irq)
{
	reg_t tmr;

	tmr = apic_read_tmr_ಸದಿಶ(irq->ಸದಿಶ);
	apic_eoi();

	/* 
	 * test if it was a level or edge triggered interrupt. If delivered as
	 * edge exec the workaround for broken chipsets
	 */
	ಆದರೆ (!lapic_test_delivery_val(tmr, irq->ಸದಿಶ)) {
		ಪೂರ್ಣಾಂಕ is_masked;
		u32_t lo;
		
		panic("EDGE instead of LEVEL!");

		lo = ioapic_read(irq->ioa->addr,
				IOAPIC_REDIR_ಕೋಷ್ಟಕ + irq->pin * 2);

		is_masked = lo & APIC_ICR_INT_ಮುಖವಾಡ;

		/* set mask and edge */
		lo |= APIC_ICR_INT_ಮುಖವಾಡ;
		lo &= ~APIC_ICR_TRIGGER;
		ioapic_write(irq->ioa->addr,
				IOAPIC_REDIR_ಕೋಷ್ಟಕ + irq->pin * 2, lo);

		/* set back to level and restore the mask bit */
		lo = ioapic_read(irq->ioa->addr,
				IOAPIC_REDIR_ಕೋಷ್ಟಕ + irq->pin * 2);

		lo |= APIC_ICR_TRIGGER;
		ಆದರೆ (is_masked)
			lo |= APIC_ICR_INT_ಮುಖವಾಡ;
		ಇಲ್ಲದಿದ್ದರೆ
			lo &= ~APIC_ICR_INT_ಮುಖವಾಡ;
		ioapic_write(irq->ioa->addr,
				IOAPIC_REDIR_ಕೋಷ್ಟಕ + irq->pin * 2, lo);
	}
}

ಸ್ಥಿರ ಶೂನ್ಯ ioapic_eoi_ಅಂಚು(__unused ರಚನೆ irq * irq)
{
	apic_eoi();
}

ಶೂನ್ಯ ioapic_eoi(ಪೂರ್ಣಾಂಕ irq)
{
	ಆದರೆ (ioapic_enabled) {
		io_apic_irq[irq].eoi(&io_apic_irq[irq]);
	}
	ಇಲ್ಲದಿದ್ದರೆ
		irq_8259_eoi(irq);
}
 
ಶೂನ್ಯ ioapic_ಗಣ_id(u32_t addr, ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ id)
{
	ioapic_write(addr, IOAPIC_ID, id << 24);
}

ಪೂರ್ಣಾಂಕ ioapic_enable_all(ಶೂನ್ಯ)
{
	i8259_disable();

	ಆದರೆ (apic_imcrp) {
		/* Select IMCR and disconnect 8259s. */
		outb(0x22, 0x70);
		outb(0x23, 0x01);
	}

	ಹಿಂತಿರುಗಿ ioapic_enabled = 1;
}

/* disables a single IO APIC */
ಸ್ಥಿರ ಶೂನ್ಯ ioapic_disable(ರಚನೆ io_apic * ioapic)
{
	ಚಿಹ್ನೆರಹಿತ p;
	
	ಗಾಗಿ (p = 0; p < io_apic->pins; p++) {
		u32_t ಕೆಳಗಿನ_32, hi_32;
		ಕೆಳಗಿನ_32 = ioapic_read((u32_t)ioapic->addr,
				(uint8_t) (IOAPIC_REDIR_ಕೋಷ್ಟಕ + p * 2));
		hi_32 = ioapic_read((u32_t)ioapic->addr,
				(uint8_t) (IOAPIC_REDIR_ಕೋಷ್ಟಕ + p * 2 + 1));

		ಆದರೆ (!(ಕೆಳಗಿನ_32 & APIC_ICR_INT_ಮುಖವಾಡ)) {
			ಕೆಳಗಿನ_32 |= APIC_ICR_INT_ಮುಖವಾಡ;
			ioapic_write((u32_t)ioapic->addr,
				(uint8_t) (IOAPIC_REDIR_ಕೋಷ್ಟಕ + p * 2 + 1), hi_32);
			ioapic_write((u32_t)ioapic->addr,
				(uint8_t) (IOAPIC_REDIR_ಕೋಷ್ಟಕ + p * 2), ಕೆಳಗಿನ_32);
		}
	}
}

/* disables all IO APICs */
ಶೂನ್ಯ ioapic_disable_all(ಶೂನ್ಯ)
{
	ಚಿಹ್ನೆರಹಿತ ioa;
	ಆದರೆ (!ioapic_enabled)
		ಹಿಂತಿರುಗಿ;

	ಗಾಗಿ (ioa = 0 ; ioa < nioapics; ioa++) 
		ioapic_disable(&io_apic[ioa]);

	ioapic_enabled = 0; /* io apic, disabled */

	/* Enable 8259 - write 0x00 in OCW1 master and slave.  */
	ಆದರೆ (apic_imcrp) {
		outb(0x22, 0x70);
		outb(0x23, 0x00);
	}

	lapic_microsec_sleep(200); /* to enable APIC to switch to PIC */

	apic_idt_init(TRUE); /* reset */
	idt_reload();
}

ಸ್ಥಿರ ಶೂನ್ಯ ioapic_disable_irq(ಚಿಹ್ನೆರಹಿತ irq)
{
	ಆದರೆ(!(io_apic_irq[irq].ioa)) {
		ಬರೆ("ioapic_disable_irq: no ioa set for irq %d!\n", irq);
		ಹಿಂತಿರುಗಿ;
	}

	ಪ್ರತಿಜ್ಞಾ(io_apic_irq[irq].ioa);

	ioapic_disable_pin(io_apic_irq[irq].ioa->addr, io_apic_irq[irq].pin);
	io_apic_irq[irq].state |= IOAPIC_IRQ_STATE_MASKED;
}

ಸ್ಥಿರ ಶೂನ್ಯ ioapic_enable_irq(ಚಿಹ್ನೆರಹಿತ irq)
{
	ಆದರೆ(!(io_apic_irq[irq].ioa)) {
		ಬರೆ("ioapic_enable_irq: no ioa set for irq %d!\n", irq);
		ಹಿಂತಿರುಗಿ;
	}

	ಪ್ರತಿಜ್ಞಾ(io_apic_irq[irq].ioa);

	ioapic_enable_pin(io_apic_irq[irq].ioa->addr, io_apic_irq[irq].pin);
	io_apic_irq[irq].state &= ~IOAPIC_IRQ_STATE_MASKED;
}

ಶೂನ್ಯ ioapic_unmask_irq(ಚಿಹ್ನೆರಹಿತ irq)
{
	ಆದರೆ (ioapic_enabled)
		ioapic_enable_irq(irq);
	ಇಲ್ಲದಿದ್ದರೆ
		/* FIXME unlikely */
		irq_8259_unmask(irq);
}

ಶೂನ್ಯ ioapic_ಮುಖವಾಡ_irq(ಚಿಹ್ನೆರಹಿತ irq)
{
	ಆದರೆ (ioapic_enabled)
		ioapic_disable_irq(irq);
	ಇಲ್ಲದಿದ್ದರೆ
		/* FIXME unlikely */
		irq_8259_ಮುಖವಾಡ(irq);
}

ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ apicid(ಶೂನ್ಯ)
{
	ಹಿಂತಿರುಗಿ lapic_read(LAPIC_ID) >> 24;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ calib_clk_handler(irq_hook_t * ಬಳಸದ(hook))
{
	u32_t tcrt;
	u64_t tsc;

	probe_ticks++;
	read_tsc_64(&tsc);
	tcrt = lapic_read(LAPIC_TIMER_CCR);


	ಆದರೆ (probe_ticks == 1) {
		lapic_tctr0 = tcrt;
		tsc0 = tsc;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (probe_ticks == PROBE_TICKS) {
		lapic_tctr1 = tcrt;
		tsc1 = tsc;
		stop_8253A_timer();
	}

	BKL_UNLOCK();
	ಹಿಂತಿರುಗಿ 1;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ spurious_irq_handler(irq_hook_t * ಬಳಸದ(hook))
{
	/*
	 * Do nothing, only unlock the kernel so we do not deadlock!
	 */
	BKL_UNLOCK();
	ಹಿಂತಿರುಗಿ 1;
}

ಸ್ಥಿರ ಶೂನ್ಯ apic_calibrate_clocks(ಚಿಹ್ನೆರಹಿತ cpu)
{
	u32_t lvtt, val, lapic_delta;
	u64_t tsc_delta;
	u64_t cpu_freq;

	irq_hook_t calib_clk, spurious_irq;

	BOOT_VERBOSE(ಬರೆ("Calibrating clock\n"));
	/*
	 * Set Initial count register to the highest value so it does not
	 * underflow during the testing period
	 * */
	val = 0xffffffff;
	lapic_write (LAPIC_TIMER_ICR, val);

	/* Set Current count register */
	val = 0;
	lapic_write (LAPIC_TIMER_CCR, val);

	lvtt = lapic_read(LAPIC_TIMER_DCR) & ~0x0b;
	 /* Set Divide configuration register to 1 */
	lvtt = APIC_TDCR_1;
	lapic_write(LAPIC_TIMER_DCR, lvtt);

	/*
	 * mask the APIC timer interrupt in the LVT Timer Register so that we
	 * don't get an interrupt upon underflow which we don't know how to
	 * handle right know. If underflow happens, the system will not continue
	 * as something is wrong with the clock IRQ 0 and we cannot calibrate
	 * the clock which mean that we cannot run processes
	 */
	lvtt = lapic_read (LAPIC_LVTTR);
	lvtt |= APIC_LVTT_ಮುಖವಾಡ;
	lapic_write (LAPIC_LVTTR, lvtt);

	/* set the probe, we use the legacy timer, IRQ 0 */
	put_irq_handler(&calib_clk, CLOCK_IRQ, calib_clk_handler);

	/*
	 * A spurious interrupt may occur during the clock calibration. Since we
	 * do this calibration in kernel, we need a special handler which will
	 * leave the BKL unlocked like the clock handler. This is a corner case,
	 * boot time only situation
	 */
	put_irq_handler(&spurious_irq, SPURIOUS_IRQ, spurious_irq_handler);

	/* set the PIC timer to get some time */
	init_8253A_timer(system_hz);

	/*
	 * We must unlock BKL here as the in-kernel interrupt will lock it
	 * again. The handler will unlock it after it is done. This is
	 * absolutely safe as only the BSP is running. It is just a workaround a
	 * corner case for APIC timer calibration
	 */
	BKL_UNLOCK();
	intr_enable();

	/* loop for some time to get a sample */
	ಆಗ(probe_ticks < PROBE_TICKS) {
		intr_enable();
	}

	intr_disable();
	BKL_ಬೀಗ();

	/* remove the probe */
	rm_irq_handler(&calib_clk);
	rm_irq_handler(&spurious_irq);

	lapic_delta = lapic_tctr0 - lapic_tctr1;
	tsc_delta = tsc1 - tsc0;

	lapic_bus_freq[cpuid] = system_hz * lapic_delta / (PROBE_TICKS - 1);
	BOOT_VERBOSE(ಬರೆ("APIC bus freq %u MHz\n",
				lapic_bus_freq[cpuid] / 1000000));
	cpu_freq = (tsc_delta / (PROBE_TICKS - 1)) * make64(system_hz, 0);
	cpu_ಗಣ_freq(cpuid, cpu_freq);
	cpu_info[cpuid].freq = (ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)(cpu_freq / 1000000);
	BOOT_VERBOSE(cpu_print_freq(cpuid));
}

ಶೂನ್ಯ lapic_ಗಣ_timer_one_shot(ಸ್ಥಿರಾಂಕ u32_t usec)
{
	/* sleep in micro seconds */
	u32_t lvtt;
	u32_t ticks_ಪ್ರತಿ_us;
	ಸ್ಥಿರಾಂಕ u8_t cpu = cpuid;

	ticks_ಪ್ರತಿ_us = (lapic_bus_freq[cpu] / 1000000) * config_apic_timer_x;

	lapic_write(LAPIC_TIMER_ICR, usec * ticks_ಪ್ರತಿ_us);

	lvtt = APIC_TDCR_1;
	lapic_write(LAPIC_TIMER_DCR, lvtt);

	/* configure timer as one-shot */
	lvtt = APIC_TIMER_INT_ಸದಿಶ;
	lapic_write(LAPIC_LVTTR, lvtt);
}

ಶೂನ್ಯ lapic_ಗಣ_timer_periodic(ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ freq)
{
	/* sleep in micro seconds */
	u32_t lvtt;
	u32_t lapic_ticks_ಪ್ರತಿ_clock_tick;
	ಸ್ಥಿರಾಂಕ u8_t cpu = cpuid;

	lapic_ticks_ಪ್ರತಿ_clock_tick = (lapic_bus_freq[cpu] / freq) * config_apic_timer_x;

	lvtt = APIC_TDCR_1;
	lapic_write(LAPIC_TIMER_DCR, lvtt);

	/* configure timer as periodic */
	lvtt = APIC_LVTT_TM | APIC_TIMER_INT_ಸದಿಶ;
	lapic_write(LAPIC_LVTTR, lvtt);

	lapic_write(LAPIC_TIMER_ICR, lapic_ticks_ಪ್ರತಿ_clock_tick);
}

ಶೂನ್ಯ lapic_stop_timer(ಶೂನ್ಯ)
{
	u32_t lvtt;
	lvtt = lapic_read(LAPIC_LVTTR);
	lapic_write(LAPIC_LVTTR, lvtt | APIC_LVTT_ಮುಖವಾಡ);
	/* zero the current counter so it can be restarted again */
	lapic_write(LAPIC_TIMER_ICR, 0);
	lapic_write(LAPIC_TIMER_CCR, 0);
}

ಶೂನ್ಯ lapic_restart_timer(ಶೂನ್ಯ)
{
	/* restart the timer only if the counter reached zero, i.e. expired */
	ಆದರೆ (lapic_read(LAPIC_TIMER_CCR) == 0)
		lapic_ಗಣ_timer_one_shot(1000000/system_hz);
}

ಶೂನ್ಯ lapic_microsec_sleep(ಚಿಹ್ನೆರಹಿತ ಎಣಿಕೆ_2)
{
	lapic_ಗಣ_timer_one_shot(ಎಣಿಕೆ_2);
	ಆಗ (lapic_read(LAPIC_TIMER_CCR))
		arch_pause();
}

ಸ್ಥಿರ  u32_t lapic_errstatus(ಶೂನ್ಯ)
{
	lapic_write(LAPIC_ESR, 0);
	ಹಿಂತಿರುಗಿ lapic_read(LAPIC_ESR);
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_SMP
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ lapic_disable_ಒಳ_msr(ಶೂನ್ಯ)
{
	u32_t msr_hi, msr_lo;

	ia32_msr_read(IA32_APIC_ತಳ, &msr_hi, &msr_lo);

	msr_lo &= ~(1 << IA32_APIC_ತಳ_ENABLE_ಬಿಟ್);
	ia32_msr_write(IA32_APIC_ತಳ, msr_hi, msr_lo);

	ಹಿಂತಿರುಗಿ 1;
}
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_SMP */

ಶೂನ್ಯ lapic_disable(ಶೂನ್ಯ)
{
	/* Disable current APIC and close interrupts from PIC */
	u32_t val;

	ಆದರೆ (!lapic_addr)
		ಹಿಂತಿರುಗಿ;
	
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_SMP
	ಆದರೆ (cpu_is_bsp(cpuid) && !apic_imcrp)
#ಆದರೆ_ಅಂತ್ಯ
	{
		/* leave it enabled if imcr is not set */
		val = lapic_read(LAPIC_LINT0);
		val &= ~(APIC_ICR_DM_ಮುಖವಾಡ|APIC_ICR_INT_ಮುಖವಾಡ);
		val |= APIC_ICR_DM_EXTINT; /* ExtINT at LINT0 */
		lapic_write (LAPIC_LINT0, val);
		ಹಿಂತಿರುಗಿ;
	}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_SMP
	val = lapic_read(LAPIC_LINT0) & 0xFFFE58FF;
	val |= APIC_ICR_INT_ಮುಖವಾಡ;
	lapic_write (LAPIC_LINT0, val);

	val = lapic_read(LAPIC_LINT1) & 0xFFFE58FF;
	val |= APIC_ICR_INT_ಮುಖವಾಡ;
	lapic_write (LAPIC_LINT1, val);

	val = lapic_read(LAPIC_SIVR) & 0xFFFFFF00;
	val &= ~APIC_ENABLE;
	lapic_write(LAPIC_SIVR, val);

	lapic_disable_ಒಳ_msr();
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_SMP */
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ lapic_enable_ಒಳ_msr(ಶೂನ್ಯ)
{
	u32_t msr_hi, msr_lo;

	ia32_msr_read(IA32_APIC_ತಳ, &msr_hi, &msr_lo);

#ಆದರೆ 0
	u32_t addr;
	/*FIXME this is a problem on AP */
	/*
	 * FIXME if the location is different (unlikely) then the one we expect,
	 * update it
	 */
	addr = (msr_lo >> 12) | ((msr_hi & 0xf) << 20);
	ಆದರೆ (addr != (lapic_addr >> 12)) {
		ಆದರೆ (msr_hi & 0xf) {
			ಬರೆ("ERROR : APIC address needs more then 32 bits\n");
			ಹಿಂತಿರುಗಿ 0;
		}
		lapic_addr = msr_lo & ~((1 << 12) - 1);
	}
#ಆದರೆ_ಅಂತ್ಯ

	msr_lo |= (1 << IA32_APIC_ತಳ_ENABLE_ಬಿಟ್);
	ia32_msr_write(IA32_APIC_ತಳ, msr_hi, msr_lo);

	ಹಿಂತಿರುಗಿ 1;
}

ಪೂರ್ಣಾಂಕ lapic_enable(ಚಿಹ್ನೆರಹಿತ cpu)
{
	u32_t val, nlvt;

	ಆದರೆ (!lapic_addr)
		ಹಿಂತಿರುಗಿ 0;

	cpu_has_tsc = _cpufeature(_CPUF_I386_TSC);
	ಆದರೆ (!cpu_has_tsc) {
		ಬರೆ("CPU lacks timestamp counter, "
			"cannot calibrate LAPIC timer\n");
		ಹಿಂತಿರುಗಿ 0;
	}

	ಆದರೆ (!lapic_enable_ಒಳ_msr())
		ಹಿಂತಿರುಗಿ 0;

	/* set the highest priority for ever */
	lapic_write(LAPIC_TPR, 0x0);

	lapic_eoi_addr = LAPIC_EOI;
	/* clear error state register. */
	val = lapic_errstatus ();

	/* Enable Local APIC and set the spurious vector to 0xff. */
	val = lapic_read(LAPIC_SIVR);
	val |= APIC_ENABLE | APIC_SPURIOUS_INT_ಸದಿಶ;
	val &= ~APIC_FOCUS_DISABLED;
	lapic_write(LAPIC_SIVR, val);
	(ಶೂನ್ಯ) lapic_read(LAPIC_SIVR);

	apic_eoi();

	/* Program Logical Destination Register. */
	val = lapic_read(LAPIC_LDR) & ~0xFF000000;
	val |= (cpu & 0xFF) << 24;
	lapic_write(LAPIC_LDR, val);

	/* Program Destination Format Register for Flat mode. */
	val = lapic_read(LAPIC_DFR) | 0xF0000000;
	lapic_write (LAPIC_DFR, val);

	val = lapic_read (LAPIC_LVTER) & 0xFFFFFF00;
	lapic_write (LAPIC_LVTER, val);

	nlvt = (lapic_read(LAPIC_ಆವೃತ್ತಿ)>>16) & 0xFF;

	ಆದರೆ(nlvt >= 4) {
		val = lapic_read(LAPIC_LVTTMR);
		lapic_write(LAPIC_LVTTMR, val | APIC_ICR_INT_ಮುಖವಾಡ);
	}

	ಆದರೆ(nlvt >= 5) {
		val = lapic_read(LAPIC_LVTPCR);
		lapic_write(LAPIC_LVTPCR, val | APIC_ICR_INT_ಮುಖವಾಡ);
	}

	/* setup TPR to allow all interrupts. */
	val = lapic_read (LAPIC_TPR);
	/* accept all interrupts */
	lapic_write (LAPIC_TPR, val & ~0xFF);

	(ಶೂನ್ಯ) lapic_read (LAPIC_SIVR);
	apic_eoi();

	apic_calibrate_clocks(cpu);
	BOOT_VERBOSE(ಬರೆ("APIC timer calibrated\n"));

	ಹಿಂತಿರುಗಿ 1;
}

ಶೂನ್ಯ apic_spurios_intr_handler(ಶೂನ್ಯ)
{
	ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ x;

	x++;
	ಆದರೆ (x == 1 || (x % 100) == 0)
		ಬರೆ("WARNING spurious interrupt(s) %d on cpu %d\n", x, cpuid);
}

ಶೂನ್ಯ apic_error_intr_handler(ಶೂನ್ಯ)
{
	ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ x;

	x++;
	ಆದರೆ (x == 1 || (x % 100) == 0)
		ಬರೆ("WARNING apic error (0x%x) interrupt(s) %d on cpu %d\n",
				lapic_errstatus(), x, cpuid);
}

ಸ್ಥಿರ ರಚನೆ gate_ಕೋಷ್ಟಕ_s gate_ಕೋಷ್ಟಕ_ioapic[] = {
	{ apic_hwint0, LAPIC_ಸದಿಶ( 0), INTR_PRIVILEGE },
	{ apic_hwint1, LAPIC_ಸದಿಶ( 1), INTR_PRIVILEGE },
	{ apic_hwint2, LAPIC_ಸದಿಶ( 2), INTR_PRIVILEGE },
	{ apic_hwint3, LAPIC_ಸದಿಶ( 3), INTR_PRIVILEGE },
	{ apic_hwint4, LAPIC_ಸದಿಶ( 4), INTR_PRIVILEGE },
	{ apic_hwint5, LAPIC_ಸದಿಶ( 5), INTR_PRIVILEGE },
	{ apic_hwint6, LAPIC_ಸದಿಶ( 6), INTR_PRIVILEGE },
	{ apic_hwint7, LAPIC_ಸದಿಶ( 7), INTR_PRIVILEGE },
	{ apic_hwint8, LAPIC_ಸದಿಶ( 8), INTR_PRIVILEGE },
	{ apic_hwint9, LAPIC_ಸದಿಶ( 9), INTR_PRIVILEGE },
	{ apic_hwint10, LAPIC_ಸದಿಶ(10), INTR_PRIVILEGE },
	{ apic_hwint11, LAPIC_ಸದಿಶ(11), INTR_PRIVILEGE },
	{ apic_hwint12, LAPIC_ಸದಿಶ(12), INTR_PRIVILEGE },
	{ apic_hwint13, LAPIC_ಸದಿಶ(13), INTR_PRIVILEGE },
	{ apic_hwint14, LAPIC_ಸದಿಶ(14), INTR_PRIVILEGE },
	{ apic_hwint15, LAPIC_ಸದಿಶ(15), INTR_PRIVILEGE },
	{ apic_hwint16, LAPIC_ಸದಿಶ(16), INTR_PRIVILEGE },
	{ apic_hwint17, LAPIC_ಸದಿಶ(17), INTR_PRIVILEGE },
	{ apic_hwint18, LAPIC_ಸದಿಶ(18), INTR_PRIVILEGE },
	{ apic_hwint19, LAPIC_ಸದಿಶ(19), INTR_PRIVILEGE },
	{ apic_hwint20, LAPIC_ಸದಿಶ(20), INTR_PRIVILEGE },
	{ apic_hwint21, LAPIC_ಸದಿಶ(21), INTR_PRIVILEGE },
	{ apic_hwint22, LAPIC_ಸದಿಶ(22), INTR_PRIVILEGE },
	{ apic_hwint23, LAPIC_ಸದಿಶ(23), INTR_PRIVILEGE },
	{ apic_hwint24, LAPIC_ಸದಿಶ(24), INTR_PRIVILEGE },
	{ apic_hwint25, LAPIC_ಸದಿಶ(25), INTR_PRIVILEGE },
	{ apic_hwint26, LAPIC_ಸದಿಶ(26), INTR_PRIVILEGE },
	{ apic_hwint27, LAPIC_ಸದಿಶ(27), INTR_PRIVILEGE },
	{ apic_hwint28, LAPIC_ಸದಿಶ(28), INTR_PRIVILEGE },
	{ apic_hwint29, LAPIC_ಸದಿಶ(29), INTR_PRIVILEGE },
	{ apic_hwint30, LAPIC_ಸದಿಶ(30), INTR_PRIVILEGE },
	{ apic_hwint31, LAPIC_ಸದಿಶ(31), INTR_PRIVILEGE },
	{ apic_hwint32, LAPIC_ಸದಿಶ(32), INTR_PRIVILEGE },
	{ apic_hwint33, LAPIC_ಸದಿಶ(33), INTR_PRIVILEGE },
	{ apic_hwint34, LAPIC_ಸದಿಶ(34), INTR_PRIVILEGE },
	{ apic_hwint35, LAPIC_ಸದಿಶ(35), INTR_PRIVILEGE },
	{ apic_hwint36, LAPIC_ಸದಿಶ(36), INTR_PRIVILEGE },
	{ apic_hwint37, LAPIC_ಸದಿಶ(37), INTR_PRIVILEGE },
	{ apic_hwint38, LAPIC_ಸದಿಶ(38), INTR_PRIVILEGE },
	{ apic_hwint39, LAPIC_ಸದಿಶ(39), INTR_PRIVILEGE },
	{ apic_hwint40, LAPIC_ಸದಿಶ(40), INTR_PRIVILEGE },
	{ apic_hwint41, LAPIC_ಸದಿಶ(41), INTR_PRIVILEGE },
	{ apic_hwint42, LAPIC_ಸದಿಶ(42), INTR_PRIVILEGE },
	{ apic_hwint43, LAPIC_ಸದಿಶ(43), INTR_PRIVILEGE },
	{ apic_hwint44, LAPIC_ಸದಿಶ(44), INTR_PRIVILEGE },
	{ apic_hwint45, LAPIC_ಸದಿಶ(45), INTR_PRIVILEGE },
	{ apic_hwint46, LAPIC_ಸದಿಶ(46), INTR_PRIVILEGE },
	{ apic_hwint47, LAPIC_ಸದಿಶ(47), INTR_PRIVILEGE },
	{ apic_hwint48, LAPIC_ಸದಿಶ(48), INTR_PRIVILEGE },
	{ apic_hwint49, LAPIC_ಸದಿಶ(49), INTR_PRIVILEGE },
	{ apic_hwint50, LAPIC_ಸದಿಶ(50), INTR_PRIVILEGE },
	{ apic_hwint51, LAPIC_ಸದಿಶ(51), INTR_PRIVILEGE },
	{ apic_hwint52, LAPIC_ಸದಿಶ(52), INTR_PRIVILEGE },
	{ apic_hwint53, LAPIC_ಸದಿಶ(53), INTR_PRIVILEGE },
	{ apic_hwint54, LAPIC_ಸದಿಶ(54), INTR_PRIVILEGE },
	{ apic_hwint55, LAPIC_ಸದಿಶ(55), INTR_PRIVILEGE },
	{ apic_hwint56, LAPIC_ಸದಿಶ(56), INTR_PRIVILEGE },
	{ apic_hwint57, LAPIC_ಸದಿಶ(57), INTR_PRIVILEGE },
	{ apic_hwint58, LAPIC_ಸದಿಶ(58), INTR_PRIVILEGE },
	{ apic_hwint59, LAPIC_ಸದಿಶ(59), INTR_PRIVILEGE },
	{ apic_hwint60, LAPIC_ಸದಿಶ(60), INTR_PRIVILEGE },
	{ apic_hwint61, LAPIC_ಸದಿಶ(61), INTR_PRIVILEGE },
	{ apic_hwint62, LAPIC_ಸದಿಶ(62), INTR_PRIVILEGE },
	{ apic_hwint63, LAPIC_ಸದಿಶ(63), INTR_PRIVILEGE },
	{ apic_spurios_intr, APIC_SPURIOUS_INT_ಸದಿಶ, INTR_PRIVILEGE },
	{ apic_error_intr, APIC_ERROR_INT_ಸದಿಶ, INTR_PRIVILEGE },
	{ ಖಾಲಿ, 0, 0}
};

ಸ್ಥಿರ ರಚನೆ gate_ಕೋಷ್ಟಕ_s gate_ಕೋಷ್ಟಕ_common[] = {
	{ ipc_ನಮೂದು_softint_orig, IPC_ಸದಿಶ_ORIG, USER_PRIVILEGE },
	{ kernel_call_ನಮೂದು_orig, KERN_CALL_ಸದಿಶ_ORIG, USER_PRIVILEGE },
	{ ipc_ನಮೂದು_softint_um, IPC_ಸದಿಶ_UM, USER_PRIVILEGE },
	{ kernel_call_ನಮೂದು_um, KERN_CALL_ಸದಿಶ_UM, USER_PRIVILEGE },
	{ ಖಾಲಿ, 0, 0}
};

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_SMP
ಸ್ಥಿರ ರಚನೆ gate_ಕೋಷ್ಟಕ_s gate_ಕೋಷ್ಟಕ_smp[] = {
	{ apic_ipi_sched_intr, APIC_SMP_SCHED_PROC_ಸದಿಶ, INTR_PRIVILEGE },
	{ apic_ipi_halt_intr,  APIC_SMP_CPU_HALT_ಸದಿಶ, INTR_PRIVILEGE },
	{ ಖಾಲಿ, 0, 0}
};
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ APIC_DEBUG
ಸ್ಥಿರ ಶೂನ್ಯ lapic_ಗಣ_dummy_handlers(ಶೂನ್ಯ)
{
	ಅಕ್ಷರ * handler;
	ಪೂರ್ಣಾಂಕ vect = 32; /* skip the reserved vectors */

	handler = &lapic_intr_dummy_handles_ಆರಂಭ;
	handler += vect * LAPIC_INTR_DUMMY_HANDLER_ಗಾತ್ರ;
	ಗಾಗಿ(; handler < &lapic_intr_dummy_handles_ಕೊನೆ;
			handler += LAPIC_INTR_DUMMY_HANDLER_ಗಾತ್ರ) {
		int_gate_idt(vect++, (vir_ಬೈಟುಗಳು) handler,
				PRESENT | INT_GATE_TYPE |
				(INTR_PRIVILEGE << DPL_ಸರಿಸು));
	}
}
#ಆದರೆ_ಅಂತ್ಯ

/* Build descriptors for interrupt gates in IDT. */
ಶೂನ್ಯ apic_idt_init(ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ reset)
{
	u32_t val;

	/* Set up idt tables for smp mode.
	 */
	ಪೂರ್ಣಾಂಕ is_bsp;

	ಆದರೆ (reset) {
		idt_copy_vectors_pic();
		idt_copy_vectors(gate_ಕೋಷ್ಟಕ_common);
		ಹಿಂತಿರುಗಿ;
	}

	is_bsp = is_boot_apic(apicid());

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ APIC_DEBUG
	ಆದರೆ (is_bsp)
		ಬರೆ("APIC debugging is enabled\n");
	lapic_ಗಣ_dummy_handlers();
#ಆದರೆ_ಅಂತ್ಯ

	/* Build descriptors for interrupt gates in IDT. */
	ಆದರೆ (ioapic_enabled)
		idt_copy_vectors(gate_ಕೋಷ್ಟಕ_ioapic);
	ಇಲ್ಲದಿದ್ದರೆ
		idt_copy_vectors_pic();

	idt_copy_vectors(gate_ಕೋಷ್ಟಕ_common);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_SMP
	idt_copy_vectors(gate_ಕೋಷ್ಟಕ_smp);
#ಆದರೆ_ಅಂತ್ಯ

	/* Setup error interrupt vector */
	val = lapic_read(LAPIC_LVTER);
	val |= APIC_ERROR_INT_ಸದಿಶ;
	val &= ~ APIC_ICR_INT_ಮುಖವಾಡ;
	lapic_write(LAPIC_LVTER, val);
	(ಶೂನ್ಯ) lapic_read(LAPIC_LVTER);

	/* configure the timer interupt handler */
	ಆದರೆ (is_bsp) {
		BOOT_VERBOSE(ಬರೆ("Initiating APIC timer handler\n"));
		/* register the timer interrupt handler for this CPU */
		int_gate_idt(APIC_TIMER_INT_ಸದಿಶ, (vir_ಬೈಟುಗಳು) lapic_timer_int_handler,
				PRESENT | INT_GATE_TYPE | (INTR_PRIVILEGE << DPL_ಸರಿಸು));
	}

}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ acpi_get_ioapics(ರಚನೆ io_apic * ioa, ಚಿಹ್ನೆರಹಿತ * nioa, ಚಿಹ್ನೆರಹಿತ max)
{
	ಚಿಹ್ನೆರಹಿತ n = 0;
	ರಚನೆ acpi_madt_ioapic * acpi_ioa;

	ಆಗ (n < max) {
		acpi_ioa = acpi_get_ioapic_ಮುಂದಿನ();
		ಆದರೆ (acpi_ioa == ಖಾಲಿ)
			ನಿಲ್ಲಿಸು;

		ಪ್ರತಿಜ್ಞಾ(acpi_ioa->ವಿಳಾಸ);

		ioa[n].id = acpi_ioa->id;
		ioa[n].addr = acpi_ioa->ವಿಳಾಸ;
		ioa[n].paddr = (phys_ಬೈಟುಗಳು) acpi_ioa->ವಿಳಾಸ;
		ioa[n].gsi_ತಳ = acpi_ioa->global_int_ತಳ;
		ioa[n].pins = ((ioapic_read(ioa[n].addr,
				IOAPIC_ಆವೃತ್ತಿ) & 0xff0000) >> 16)+1;
		ಬರೆ("IO APIC idx %d id %d addr 0x%lx paddr 0x%lx pins %d\n",
				n, acpi_ioa->id, ioa[n].addr, ioa[n].paddr,
				ioa[n].pins);
		n++;
	}

	*nioa = n;
	ಹಿಂತಿರುಗಿ n;
}

ಪೂರ್ಣಾಂಕ detect_ioapics(ಶೂನ್ಯ)
{
	ಪೂರ್ಣಾಂಕ ಸ್ಥಿತಿ_2;

	ಆದರೆ (machine.acpi_rsdp) {
		ಸ್ಥಿತಿ_2 = acpi_get_ioapics(io_apic, &nioapics, MAX_NR_IOAPICS);
	} ಇಲ್ಲದಿದ್ದರೆ {
		ಸ್ಥಿತಿ_2 = 0;
	}
	ಆದರೆ (!ಸ್ಥಿತಿ_2) {
		/* try something different like MPS */
	}

	ಹಿಂತಿರುಗಿ ಸ್ಥಿತಿ_2;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_SMP

ಶೂನ್ಯ apic_send_ipi(ಚಿಹ್ನೆರಹಿತ ಸದಿಶ, ಚಿಹ್ನೆರಹಿತ cpu, ಪೂರ್ಣಾಂಕ type)
{
	u32_t icr1, icr2;

	ಆದರೆ (ncpus == 1)
		/* no need of sending an IPI */
		ಹಿಂತಿರುಗಿ;

	ಆಗ (lapic_read_icr1() & APIC_ICR_DELIVERY_PENDING) 
		arch_pause();

	icr1 = lapic_read_icr1() & 0xFFF0F800;
	icr2 = lapic_read_icr2() & 0xFFFFFF;

	ಆಯ್ಕೆ (type) {
		ಸ್ಥಿತಿ APIC_IPI_DEST:
			ಆದರೆ (!cpu_is_ready(cpu))
				ಹಿಂತಿರುಗಿ;
			lapic_write_icr2(icr2 |	(cpuid2apicid[cpu] << 24));
			lapic_write_icr1(icr1 |	APIC_ICR_DEST_ಕ್ಷೇತ್ರ | ಸದಿಶ);
			ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ APIC_IPI_SELF:
			lapic_write_icr2(icr2);
			lapic_write_icr1(icr1 |	APIC_ICR_DEST_SELF | ಸದಿಶ);
			ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ APIC_IPI_ಗಮ್ಯ_ALL_BUT_SELF:
			lapic_write_icr2(icr2);
			lapic_write_icr1(icr1 |	APIC_ICR_DEST_ALL_BUT_SELF | ಸದಿಶ);
			ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ APIC_IPI_ಗಮ್ಯ_ALL:
			lapic_write_icr2(icr2);
			lapic_write_icr1(icr1 |	APIC_ICR_DEST_ALL | ಸದಿಶ);
			ನಿಲ್ಲಿಸು;
		ಪೂರ್ವನಿಯೋಜಿತ:
			ಬರೆ("WARNING : unknown send ipi type request\n");
	}

}

ಪೂರ್ಣಾಂಕ apic_send_startup_ipi(ಚಿಹ್ನೆರಹಿತ cpu, phys_ಬೈಟುಗಳು trampoline)
{
	ಪೂರ್ಣಾಂಕ timeout;
	u32_t errstatus = 0;
	ಪೂರ್ಣಾಂಕ i;

	/* INIT-SIPI-SIPI sequence */

	ಗಾಗಿ (i = 0; i < 2; i++) {
		u32_t val;
		
		/* clear err status */
		lapic_errstatus();

		/* set target pe */
		val = lapic_read(LAPIC_ICR2) & 0xFFFFFF;
		val |= cpuid2apicid[cpu] << 24;
		lapic_write(LAPIC_ICR2, val);

		/* send SIPI */
		val = lapic_read(LAPIC_ICR1) & 0xFFF32000;
		val |= APIC_ICR_LEVEL_ASSERT |APIC_ICR_DM_STARTUP;
		val |= (((u32_t)trampoline >> 12)&0xff);
		lapic_write(LAPIC_ICR1, val);

		timeout = 1000;

		/* wait for 200 micro-seconds*/
		lapic_microsec_sleep (200);
		errstatus = 0;

		ಆಗ ((lapic_read(LAPIC_ICR1) & APIC_ICR_DELIVERY_PENDING) &&
				!errstatus) {
			errstatus = lapic_errstatus();
			timeout--;
			ಆದರೆ (!timeout) ನಿಲ್ಲಿಸು;
		}

		/* skip this one and continue with another cpu */
		ಆದರೆ (errstatus)
			ಹಿಂತಿರುಗಿ -1;
	}

	ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ apic_send_init_ipi(ಚಿಹ್ನೆರಹಿತ cpu, phys_ಬೈಟುಗಳು trampoline) 
{
	u32_t ptr, errstatus = 0;
	ಪೂರ್ಣಾಂಕ timeout;

	/* set the warm reset vector */
	ptr = (u32_t)(trampoline & 0xF);
	phys_copy(0x467, vir2phys(&ptr), ಗಾತ್ರ(u16_t ));
	ptr = (u32_t)(trampoline >> 4);
	phys_copy(0x469, vir2phys(&ptr), ಗಾತ್ರ(u16_t ));

	/* set shutdown code */
	outb (RTC_ಸೂಚ್ಯಂಕ, 0xF);
	outb (RTC_IO, 0xA);

	/* clear error state register. */
	(ಶೂನ್ಯ) lapic_errstatus();

	/* assert INIT IPI , No Shorthand, destination mode : physical */
	lapic_write(LAPIC_ICR2, (lapic_read (LAPIC_ICR2) & 0xFFFFFF) |
					(cpuid2apicid[cpu] << 24));
	lapic_write(LAPIC_ICR1, (lapic_read (LAPIC_ICR1) & 0xFFF32000) |
		APIC_ICR_DM_INIT | APIC_ICR_TM_LEVEL | APIC_ICR_LEVEL_ASSERT);

	timeout = 1000;

	/* sleep for 200 micro-seconds */
	lapic_microsec_sleep(200);

	errstatus = 0;

	ಆಗ ((lapic_read(LAPIC_ICR1) & APIC_ICR_DELIVERY_PENDING) && !errstatus) {
		errstatus = lapic_errstatus();
		timeout--;
		ಆದರೆ (!timeout) ನಿಲ್ಲಿಸು;
	}

	ಆದರೆ (errstatus) 
		ಹಿಂತಿರುಗಿ -1; /* to continue with a new processor */

	/* clear error state register. */
	lapic_errstatus();

	/* deassert INIT IPI , No Shorthand, destination mode : physical */
	lapic_write(LAPIC_ICR2, (lapic_read (LAPIC_ICR2) & 0xFFFFFF) |
					(cpuid2apicid[cpu] << 24));
	lapic_write(LAPIC_ICR1, (lapic_read (LAPIC_ICR1) & 0xFFF32000) |
		APIC_ICR_DEST_ALL | APIC_ICR_TM_LEVEL);

	timeout = 1000;
	errstatus = 0;

	/* sleep for 200 micro-seconds */
	lapic_microsec_sleep(200);

	ಆಗ ((lapic_read(LAPIC_ICR1)&APIC_ICR_DELIVERY_PENDING) && !errstatus) {
		errstatus = lapic_errstatus();
		timeout--;
		ಆದರೆ(!timeout) ನಿಲ್ಲಿಸು;
	}

	ಆದರೆ (errstatus) 
		ಹಿಂತಿರುಗಿ -1; /* with the new processor */

	/* clear error state register.  */
	(ಶೂನ್ಯ) lapic_errstatus();

	/* wait 10ms */
	lapic_microsec_sleep (10000);

	ಹಿಂತಿರುಗಿ 0;
}
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CONFIG_SMP
ಪೂರ್ಣಾಂಕ apic_single_cpu_init(ಶೂನ್ಯ)
{
	ಆದರೆ (!cpu_feature_apic_on_chip())
		ಹಿಂತಿರುಗಿ 0;

	lapic_addr = LOCAL_APIC_DEF_ADDR;
	ioapic_enabled = 0;

	ಆದರೆ (!lapic_enable(0)) {
		lapic_addr = 0x0;
		ಹಿಂತಿರುಗಿ 0;
	}

	bsp_lapic_id = apicid();
	ಬರೆ("Boot cpu apic id %d\n", bsp_lapic_id);

	acpi_init();

	ಆದರೆ (!detect_ioapics()) {
		lapic_disable();
		lapic_addr = 0x0;
		ಹಿಂತಿರುಗಿ 0;
	}

	ioapic_enable_all();

	ಆದರೆ (ioapic_enabled)
		machine.apic_enabled = 1;

	apic_idt_init(0); /* Not a reset ! */
	idt_reload();
	ಹಿಂತಿರುಗಿ 1;
}
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ eoi_ವಿಧಾನ_t ಗಣ_eoi_ವಿಧಾನ(ಚಿಹ್ನೆರಹಿತ irq)
{
	/*
	 * in APIC mode the lowest 16 IRQs are reserved for legacy (E)ISA edge
	 * triggered interrupts. All the rest is for PCI level triggered
	 * interrupts
	 */
	ಆದರೆ (irq < 16)
		ಹಿಂತಿರುಗಿ ioapic_eoi_ಅಂಚು;
	ಇಲ್ಲದಿದ್ದರೆ
		ಹಿಂತಿರುಗಿ ioapic_eoi_level;
}

ಶೂನ್ಯ ಗಣ_irq_redir_ಕೆಳಗಿನ(ಚಿಹ್ನೆರಹಿತ irq, u32_t * ಕೆಳಗಿನ)
{
	u32_t val = 0;

	/* clear the polarity, trigger, mask and vector fields */
	val &= ~(APIC_ICR_ಸದಿಶ | APIC_ICR_INT_ಮುಖವಾಡ |
			APIC_ICR_TRIGGER | APIC_ICR_INT_POLARITY);

	ಆದರೆ (irq < 16) {
		/* ISA active-high */
		val &= ~APIC_ICR_INT_POLARITY;
		/* ISA edge triggered */
		val &= ~APIC_ICR_TRIGGER;
	}
	ಇಲ್ಲದಿದ್ದರೆ {
		/* PCI active-low */
		val |= APIC_ICR_INT_POLARITY;
		/* PCI level triggered */
		val |= APIC_ICR_TRIGGER;
	}

	val |= io_apic_irq[irq].ಸದಿಶ;

	*ಕೆಳಗಿನ = val;
}

ಶೂನ್ಯ ioapic_ಗಣ_irq(ಚಿಹ್ನೆರಹಿತ irq)
{
	ಚಿಹ್ನೆರಹಿತ ioa;

	ಪ್ರತಿಜ್ಞಾ(irq < NR_IRQ_VECTORS);
	
	/* shared irq, already set */
	ಆದರೆ (io_apic_irq[irq].ioa && io_apic_irq[irq].eoi)
		ಹಿಂತಿರುಗಿ;
	
	ಪ್ರತಿಜ್ಞಾ(!io_apic_irq[irq].ioa || !io_apic_irq[irq].eoi);

	ಗಾಗಿ (ioa = 0; ioa < nioapics; ioa++) {
		ಆದರೆ (io_apic[ioa].gsi_ತಳ <= irq &&
				io_apic[ioa].gsi_ತಳ +
				io_apic[ioa].pins > irq) {
			u32_t hi_32, ಕೆಳಗಿನ_32;

			io_apic_irq[irq].ioa = &io_apic[ioa];
			io_apic_irq[irq].pin = irq - io_apic[ioa].gsi_ತಳ;
			io_apic_irq[irq].eoi = ಗಣ_eoi_ವಿಧಾನ(irq);
			io_apic_irq[irq].ಸದಿಶ = LAPIC_ಸದಿಶ(irq);

			ಗಣ_irq_redir_ಕೆಳಗಿನ(irq, &ಕೆಳಗಿನ_32);
			/*
			 * route the interrupts to the bsp by default
			 */
			hi_32 = bsp_lapic_id << 24;
			ioapic_redirt_ನಮೂದು_write((ಶೂನ್ಯ *) io_apic[ioa].addr,
					io_apic_irq[irq].pin, hi_32, ಕೆಳಗಿನ_32);
		}
	}
}

ಶೂನ್ಯ ioapic_unset_irq(ಚಿಹ್ನೆರಹಿತ irq)
{
	ಪ್ರತಿಜ್ಞಾ(irq < NR_IRQ_VECTORS);

	ioapic_disable_irq(irq);
	io_apic_irq[irq].ioa = ಖಾಲಿ;
	io_apic_irq[irq].eoi = ಖಾಲಿ;
}

ಶೂನ್ಯ ioapic_reset_pic(ಶೂನ್ಯ)
{       
	apic_idt_init(TRUE); /* reset */
	idt_reload();

	/* Enable 8259 - write 0x00 in OCW1
	 * master and slave.  */
		outb(0x22, 0x70);
		outb(0x23, 0x00);
}

ಸ್ಥಿರ ಶೂನ್ಯ irq_lapic_ಸ್ಥಿತಿ(ಪೂರ್ಣಾಂಕ irq)
{
	u32_t lo;
	reg_t tmr, irr, isr;
	ಪೂರ್ಣಾಂಕ ಸದಿಶ;
	ರಚನೆ irq * intr;

	intr = &io_apic_irq[irq];
	
	ಆದರೆ (!intr->ioa)
		ಹಿಂತಿರುಗಿ;

	ಸದಿಶ = LAPIC_ಸದಿಶ(irq);
	tmr =  apic_read_tmr_ಸದಿಶ(ಸದಿಶ);
	irr =  apic_read_irr_ಸದಿಶ(ಸದಿಶ);
	isr =  apic_read_isr_ಸದಿಶ(ಸದಿಶ);


	ಆದರೆ (lapic_test_delivery_val(isr, ಸದಿಶ)) {
		ಬರೆ("IRQ %d vec %d trigger %s irr %d isr %d\n",
				irq, ಸದಿಶ,
				lapic_test_delivery_val(tmr, ಸದಿಶ) ?
				"level" : "edge",
				lapic_test_delivery_val(irr, ಸದಿಶ) ? 1 : 0,
				lapic_test_delivery_val(isr, ಸದಿಶ) ? 1 : 0);
	} ಇಲ್ಲದಿದ್ದರೆ {
		ಬರೆ("IRQ %d vec %d irr %d\n",
				irq, ಸದಿಶ,
				lapic_test_delivery_val(irr, ಸದಿಶ) ? 1 : 0);
	}
	
	lo = ioapic_read(intr->ioa->addr,
			IOAPIC_REDIR_ಕೋಷ್ಟಕ + intr->pin * 2);
	ಬರೆ("\tpin %2d vec 0x%02x ioa %d redir_lo 0x%08x %s\n",
			intr->pin,
			intr->ಸದಿಶ,
			intr->ioa->id,
			lo,
			intr->state & IOAPIC_IRQ_STATE_MASKED ?
			"masked" : "unmasked");
}

ಶೂನ್ಯ dump_apic_irq_state(ಶೂನ್ಯ)
{
	ಪೂರ್ಣಾಂಕ irq;

	ಬರೆ("--- IRQs state dump ---\n");
	ಗಾಗಿ (irq = 0; irq < NR_IRQ_VECTORS; irq++) {
		irq_lapic_ಸ್ಥಿತಿ(irq);
	}
	ಬರೆ("--- all ---\n");
}
