#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ __ACPI_H__
#ವ್ಯಾಖ್ಯಾನಿಸು __ACPI_H__

#ಅನ್ತರ್ಭಾವ "kernel/kernel.h"

/* ACPI root system description pointer */
ರಚನೆ acpi_rsdp {
	ಅಕ್ಷರ	signature[8]; /* must be "RSD PTR " */
	u8_t	checksum;
	ಅಕ್ಷರ	oemid[6];
	u8_t	revision;
	u32_t	rsdt_addr;
	u32_t	ಉದ್ದಳತೆ;
}; 

#ವ್ಯಾಖ್ಯಾನಿಸು ACPI_SDT_SIGNATURE_LEN	4

#ವ್ಯಾಖ್ಯಾನಿಸು ACPI_SDT_SIGNATURE(name)	#name

/* header common to all system description tables */
ರಚನೆ acpi_sdt_ಶೀರ್ಷಿಕೆ {
	ಅಕ್ಷರ	signature[ACPI_SDT_SIGNATURE_LEN];
	u32_t	ಉದ್ದಳತೆ;
	u8_t	revision;
	u8_t	checksum;
	ಅಕ್ಷರ	oemid[6];
	ಅಕ್ಷರ	oem_ಕೋಷ್ಟಕ_id[8];
	u32_t	oem_revision;
	u32_t	creator_id;
	u32_t	creator_revision;
};

ರಚನೆ acpi_generic_ವಿಳಾಸ {
	u8_t ವಿಳಾಸ_space_id;
	u8_t ರಿಜಿಸ್ಟರ್_ಬಿಟ್_ಅಗಲ;
	u8_t ರಿಜಿಸ್ಟರ್_ಬಿಟ್_ಸ್ಥಾನಾಂತರ;
	u8_t access_ಗಾತ್ರ;
	u64_t ವಿಳಾಸ;
};

ರಚನೆ acpi_fadt_ಶೀರ್ಷಿಕೆ
{
	ರಚನೆ acpi_sdt_ಶೀರ್ಷಿಕೆ hdr;
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
	u8_t gpe1_ತಳ;
	u8_t cst_cnt;
	u16_t p_lvl2_lat;
	u16_t p_lvl3_lat;
	u16_t flush_ಗಾತ್ರ;
	u16_t flush_stride;
	u8_t duty_ಸ್ಥಾನಾಂತರ;
	u8_t duty_ಅಗಲ;
	u8_t day_alrm;
	u8_t mon_alrm;
	u8_t century;
	u16_t iapc_boot_arch;
	u8_t reserved1;
	u32_t flags;
	ರಚನೆ acpi_generic_ವಿಳಾಸ reset_reg;
	u8_t reset_ಮೌಲ್ಯ;
	u8_t reserved2[3];
	u64_t xfacs;
	u64_t xdsdt;
	ರಚನೆ acpi_generic_ವಿಳಾಸ xpm1a_evt_blk;
	ರಚನೆ acpi_generic_ವಿಳಾಸ xpm1b_evt_blk;
	ರಚನೆ acpi_generic_ವಿಳಾಸ xpm1a_cnt_blk;
	ರಚನೆ acpi_generic_ವಿಳಾಸ xpm1b_cnt_blk;
	ರಚನೆ acpi_generic_ವಿಳಾಸ xpm2_cnt_blk;
	ರಚನೆ acpi_generic_ವಿಳಾಸ xpm_tmr_blk;
	ರಚನೆ acpi_generic_ವಿಳಾಸ xgpe0_blk;
	ರಚನೆ acpi_generic_ವಿಳಾಸ xgpe1_blk;
};

ರಚನೆ acpi_madt_hdr {
	ರಚನೆ acpi_sdt_ಶೀರ್ಷಿಕೆ	hdr;
	u32_t			local_apic_ವಿಳಾಸ;
	u32_t			flags;
};

#ವ್ಯಾಖ್ಯಾನಿಸು ACPI_MADT_TYPE_LAPIC		0
#ವ್ಯಾಖ್ಯಾನಿಸು ACPI_MADT_TYPE_IOAPIC		1
#ವ್ಯಾಖ್ಯಾನಿಸು ACPI_MADT_TYPE_INT_SRC		2
#ವ್ಯಾಖ್ಯಾನಿಸು ACPI_MADT_TYPE_NMI_SRC		3
#ವ್ಯಾಖ್ಯಾನಿಸು ACPI_MADT_TYPE_LAPIC_NMI	4
#ವ್ಯಾಖ್ಯಾನಿಸು ACPI_MADT_TYPE_LAPIC_ADRESS	5
#ವ್ಯಾಖ್ಯಾನಿಸು ACPI_MADT_TYPE_IOSAPIC		6
#ವ್ಯಾಖ್ಯಾನಿಸು ACPI_MADT_TYPE_LSAPIC		7
#ವ್ಯಾಖ್ಯಾನಿಸು ACPI_MADT_TYPE_PLATFORM_INT_SRC	8
#ವ್ಯಾಖ್ಯಾನಿಸು ACPI_MADT_TYPE_Lx2APIC		9
#ವ್ಯಾಖ್ಯಾನಿಸು ACPI_MADT_TYPE_Lx2APIC_NMI	10

ರಚನೆ acpi_madt_ಅಂಶ_hdr{
	u8_t	type;
	u8_t	ಉದ್ದಳತೆ;
};

ರಚನೆ acpi_madt_lapic {
	ರಚನೆ acpi_madt_ಅಂಶ_hdr hdr;
	u8_t	acpi_cpu_id;
	u8_t	apic_id;
	u32_t	flags;
};

ರಚನೆ acpi_madt_ioapic {
	ರಚನೆ acpi_madt_ಅಂಶ_hdr hdr;
	u8_t	id;
	u8_t	__reserved;
	u32_t	ವಿಳಾಸ;
	u32_t	global_int_ತಳ;
};

ರಚನೆ acpi_madt_int_src {
	ರಚನೆ acpi_madt_ಅಂಶ_hdr hdr;
	u8_t	bus;
	u8_t	bus_int;
	u32_t	global_int;
	u16_t	mps_flags;
};

ರಚನೆ acpi_madt_nmi {
	ರಚನೆ acpi_madt_ಅಂಶ_hdr hdr;
	u16_t	flags;
	u32_t	global_int;
};

ಶೂನ್ಯ acpi_init(ಶೂನ್ಯ);

ಶೂನ್ಯ acpi_poweroff(ಶೂನ್ಯ);

/* 
 * Returns a pointer to the io acpi structure in the MADT table in ACPI. The
 * pointer is valid only until paging is turned off. No memory is allocated in
 * this function thus no memory needs to be freed
 */
ರಚನೆ acpi_madt_ioapic * acpi_get_ioapic_ಮುಂದಿನ(ಶೂನ್ಯ);
/* same as above for local APICs */
ರಚನೆ acpi_madt_lapic * acpi_get_lapic_ಮುಂದಿನ(ಶೂನ್ಯ);

#ಆದರೆ_ಅಂತ್ಯ /* __ACPI_H__ */
