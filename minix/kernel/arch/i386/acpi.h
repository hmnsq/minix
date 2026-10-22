#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ __ACPI_H__
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ __ACPI_H__

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/kernel.h"

/* ACPI root system description pointer */
ਬਣਤਰ acpi_rsdp {
	ਅੱਖਰ	signature[8]; /* must be "RSD PTR " */
	u8_t	checksum;
	ਅੱਖਰ	oemid[6];
	u8_t	revision;
	u32_t	rsdt_addr;
	u32_t	ਲੰਮਾਈ;
}; 

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ACPI_SDT_SIGNATURE_LEN	4

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ACPI_SDT_SIGNATURE(name)	#name

/* header common to all system description tables */
ਬਣਤਰ acpi_sdt_ਸਿਰਲੇਖ {
	ਅੱਖਰ	signature[ACPI_SDT_SIGNATURE_LEN];
	u32_t	ਲੰਮਾਈ;
	u8_t	revision;
	u8_t	checksum;
	ਅੱਖਰ	oemid[6];
	ਅੱਖਰ	oem_ਸਾਰਣੀ_id[8];
	u32_t	oem_revision;
	u32_t	creator_id;
	u32_t	creator_revision;
};

ਬਣਤਰ acpi_generic_ਪਤਾ {
	u8_t ਪਤਾ_space_id;
	u8_t ਰਜਿਸਟਰ_ਦੋਆਧਾਰੀ_ਅੰਕ_ਚੌੜਾਈ;
	u8_t ਰਜਿਸਟਰ_ਦੋਆਧਾਰੀ_ਅੰਕ_ਵਿੱਥ;
	u8_t access_ਮਾਪ;
	u64_t ਪਤਾ;
};

ਬਣਤਰ acpi_fadt_ਸਿਰਲੇਖ
{
	ਬਣਤਰ acpi_sdt_ਸਿਰਲੇਖ hdr;
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
	u8_t gpe1_ਅਧਾਰ;
	u8_t cst_cnt;
	u16_t p_lvl2_lat;
	u16_t p_lvl3_lat;
	u16_t flush_ਮਾਪ;
	u16_t flush_stride;
	u8_t duty_ਵਿੱਥ;
	u8_t duty_ਚੌੜਾਈ;
	u8_t day_alrm;
	u8_t mon_alrm;
	u8_t century;
	u16_t iapc_boot_arch;
	u8_t reserved1;
	u32_t flags;
	ਬਣਤਰ acpi_generic_ਪਤਾ reset_reg;
	u8_t reset_ਮੁੱਲ;
	u8_t reserved2[3];
	u64_t xfacs;
	u64_t xdsdt;
	ਬਣਤਰ acpi_generic_ਪਤਾ xpm1a_evt_blk;
	ਬਣਤਰ acpi_generic_ਪਤਾ xpm1b_evt_blk;
	ਬਣਤਰ acpi_generic_ਪਤਾ xpm1a_cnt_blk;
	ਬਣਤਰ acpi_generic_ਪਤਾ xpm1b_cnt_blk;
	ਬਣਤਰ acpi_generic_ਪਤਾ xpm2_cnt_blk;
	ਬਣਤਰ acpi_generic_ਪਤਾ xpm_tmr_blk;
	ਬਣਤਰ acpi_generic_ਪਤਾ xgpe0_blk;
	ਬਣਤਰ acpi_generic_ਪਤਾ xgpe1_blk;
};

ਬਣਤਰ acpi_madt_hdr {
	ਬਣਤਰ acpi_sdt_ਸਿਰਲੇਖ	hdr;
	u32_t			local_apic_ਪਤਾ;
	u32_t			flags;
};

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ACPI_MADT_TYPE_LAPIC		0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ACPI_MADT_TYPE_IOAPIC		1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ACPI_MADT_TYPE_INT_SRC		2
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ACPI_MADT_TYPE_NMI_SRC		3
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ACPI_MADT_TYPE_LAPIC_NMI	4
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ACPI_MADT_TYPE_LAPIC_ADRESS	5
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ACPI_MADT_TYPE_IOSAPIC		6
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ACPI_MADT_TYPE_LSAPIC		7
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ACPI_MADT_TYPE_PLATFORM_INT_SRC	8
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ACPI_MADT_TYPE_Lx2APIC		9
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ACPI_MADT_TYPE_Lx2APIC_NMI	10

ਬਣਤਰ acpi_madt_ਵਸਤੂ_hdr{
	u8_t	type;
	u8_t	ਲੰਮਾਈ;
};

ਬਣਤਰ acpi_madt_lapic {
	ਬਣਤਰ acpi_madt_ਵਸਤੂ_hdr hdr;
	u8_t	acpi_cpu_id;
	u8_t	apic_id;
	u32_t	flags;
};

ਬਣਤਰ acpi_madt_ioapic {
	ਬਣਤਰ acpi_madt_ਵਸਤੂ_hdr hdr;
	u8_t	id;
	u8_t	__reserved;
	u32_t	ਪਤਾ;
	u32_t	global_int_ਅਧਾਰ;
};

ਬਣਤਰ acpi_madt_int_src {
	ਬਣਤਰ acpi_madt_ਵਸਤੂ_hdr hdr;
	u8_t	bus;
	u8_t	bus_int;
	u32_t	global_int;
	u16_t	mps_flags;
};

ਬਣਤਰ acpi_madt_nmi {
	ਬਣਤਰ acpi_madt_ਵਸਤੂ_hdr hdr;
	u16_t	flags;
	u32_t	global_int;
};

ਖਾਲੀ acpi_init(ਖਾਲੀ);

ਖਾਲੀ acpi_poweroff(ਖਾਲੀ);

/* 
 * Returns a pointer to the io acpi structure in the MADT table in ACPI. The
 * pointer is valid only until paging is turned off. No memory is allocated in
 * this function thus no memory needs to be freed
 */
ਬਣਤਰ acpi_madt_ioapic * acpi_get_ioapic_ਅਗਲਾ(ਖਾਲੀ);
/* same as above for local APICs */
ਬਣਤਰ acpi_madt_lapic * acpi_get_lapic_ਅਗਲਾ(ਖਾਲੀ);

#ਜੇ_ਅੰਤ /* __ACPI_H__ */
