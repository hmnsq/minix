#اگر_غیر_تعریف_شدہ __ACPI_H__
#تعریف_کرو __ACPI_H__

#شامل "kernel/kernel.h"

/* ACPI root system description pointer */
ساخت acpi_rsdp {
	حرف	signature[8]; /* must be "RSD PTR " */
	u8_t	checksum;
	حرف	oemid[6];
	u8_t	revision;
	u32_t	rsdt_addr;
	u32_t	لمبائی;
}; 

#تعریف_کرو ACPI_SDT_SIGNATURE_LEN	4

#تعریف_کرو ACPI_SDT_SIGNATURE(name)	#name

/* header common to all system description tables */
ساخت acpi_sdt_سرخی {
	حرف	signature[ACPI_SDT_SIGNATURE_LEN];
	u32_t	لمبائی;
	u8_t	revision;
	u8_t	checksum;
	حرف	oemid[6];
	حرف	oem_جدول_id[8];
	u32_t	oem_revision;
	u32_t	creator_id;
	u32_t	creator_revision;
};

ساخت acpi_generic_پتہ {
	u8_t پتہ_space_id;
	u8_t رجسٹر_ثنائی_ہندسہ_چوڑائی;
	u8_t رجسٹر_ثنائی_ہندسہ_انحراف;
	u8_t access_سائز;
	u64_t پتہ;
};

ساخت acpi_fadt_سرخی
{
	ساخت acpi_sdt_سرخی hdr;
	u32_t facs;
	u32_t dsdt;
	u8_t model;
	u8_t preferred_pm_profile;
	u16_t sci_int;
	u32_t smi_cmd;
	u8_t acpi_enable;
	u8_t acpi_disable;
	u8_t s4bios_req;
	u8_t pstate_cnt;
	u32_t pm1a_evt_blk;
	u32_t pm1b_evt_blk;
	u32_t pm1a_cnt_blk;
	u32_t pm1b_cnt_blk;
	u32_t pm2_cnt_blk;
	u32_t pm_tmr_blk;
	u32_t gpe0_blk;
	u32_t gpe1_blk;
	u8_t pm1_evt_len;
	u8_t pm1_cnt_len;
	u8_t pm2_cnt_len;
	u8_t pm_tmr_len;
	u8_t gpe0_blk_len;
	u8_t gpe1_blk_len;
	u8_t gpe1_بنیاد;
	u8_t cst_cnt;
	u16_t p_lvl2_lat;
	u16_t p_lvl3_lat;
	u16_t flush_سائز;
	u16_t flush_stride;
	u8_t duty_انحراف;
	u8_t duty_چوڑائی;
	u8_t day_alrm;
	u8_t mon_alrm;
	u8_t century;
	u16_t iapc_boot_arch;
	u8_t reserved1;
	u32_t flags;
	ساخت acpi_generic_پتہ reset_reg;
	u8_t reset_قدر;
	u8_t reserved2[3];
	u64_t xfacs;
	u64_t xdsdt;
	ساخت acpi_generic_پتہ xpm1a_evt_blk;
	ساخت acpi_generic_پتہ xpm1b_evt_blk;
	ساخت acpi_generic_پتہ xpm1a_cnt_blk;
	ساخت acpi_generic_پتہ xpm1b_cnt_blk;
	ساخت acpi_generic_پتہ xpm2_cnt_blk;
	ساخت acpi_generic_پتہ xpm_tmr_blk;
	ساخت acpi_generic_پتہ xgpe0_blk;
	ساخت acpi_generic_پتہ xgpe1_blk;
};

ساخت acpi_madt_hdr {
	ساخت acpi_sdt_سرخی	hdr;
	u32_t			local_apic_پتہ;
	u32_t			flags;
};

#تعریف_کرو ACPI_MADT_TYPE_LAPIC		0
#تعریف_کرو ACPI_MADT_TYPE_IOAPIC		1
#تعریف_کرو ACPI_MADT_TYPE_INT_SRC		2
#تعریف_کرو ACPI_MADT_TYPE_NMI_SRC		3
#تعریف_کرو ACPI_MADT_TYPE_LAPIC_NMI	4
#تعریف_کرو ACPI_MADT_TYPE_LAPIC_ADRESS	5
#تعریف_کرو ACPI_MADT_TYPE_IOSAPIC		6
#تعریف_کرو ACPI_MADT_TYPE_LSAPIC		7
#تعریف_کرو ACPI_MADT_TYPE_PLATFORM_INT_SRC	8
#تعریف_کرو ACPI_MADT_TYPE_Lx2APIC		9
#تعریف_کرو ACPI_MADT_TYPE_Lx2APIC_NMI	10

ساخت acpi_madt_شے_hdr{
	u8_t	type;
	u8_t	لمبائی;
};

ساخت acpi_madt_lapic {
	ساخت acpi_madt_شے_hdr hdr;
	u8_t	acpi_cpu_id;
	u8_t	apic_id;
	u32_t	flags;
};

ساخت acpi_madt_ioapic {
	ساخت acpi_madt_شے_hdr hdr;
	u8_t	id;
	u8_t	__reserved;
	u32_t	پتہ;
	u32_t	global_int_بنیاد;
};

ساخت acpi_madt_int_src {
	ساخت acpi_madt_شے_hdr hdr;
	u8_t	bus;
	u8_t	bus_int;
	u32_t	global_int;
	u16_t	mps_flags;
};

ساخت acpi_madt_nmi {
	ساخت acpi_madt_شے_hdr hdr;
	u16_t	flags;
	u32_t	global_int;
};

خالی acpi_init(خالی);

خالی acpi_poweroff(خالی);

/* 
 * Returns a pointer to the io acpi structure in the MADT table in ACPI. The
 * pointer is valid only until paging is turned off. No memory is allocated in
 * this function thus no memory needs to be freed
 */
ساخت acpi_madt_ioapic * acpi_get_ioapic_اگلا(خالی);
/* same as above for local APICs */
ساخت acpi_madt_lapic * acpi_get_lapic_اگلا(خالی);

#اگر_ختم /* __ACPI_H__ */
