#यदि_अपरिभाषितम् __ACPI_H__
#परिभाषय __ACPI_H__

#अन्तर्भाव "kernel/kernel.h"

/* ACPI root system description pointer */
संरचना acpi_rsdp {
	वर्ण	signature[8]; /* must be "RSD PTR " */
	u8_t	checksum;
	वर्ण	oemid[6];
	u8_t	revision;
	u32_t	rsdt_addr;
	u32_t	दीर्घता;
}; 

#परिभाषय ACPI_SDT_SIGNATURE_LEN	4

#परिभाषय ACPI_SDT_SIGNATURE(name)	#name

/* header common to all system description tables */
संरचना acpi_sdt_शीर्षकम् {
	वर्ण	signature[ACPI_SDT_SIGNATURE_LEN];
	u32_t	दीर्घता;
	u8_t	revision;
	u8_t	checksum;
	वर्ण	oemid[6];
	वर्ण	oem_सारणी_id[8];
	u32_t	oem_revision;
	u32_t	creator_id;
	u32_t	creator_revision;
};

संरचना acpi_generic_स्थानाङ्कः {
	u8_t स्थानाङ्कः_space_id;
	u8_t पञ्जिकाकोशः_bit_विस्तारः;
	u8_t पञ्जिकाकोशः_bit_विस्थापनम्;
	u8_t access_आकार;
	u64_t स्थानाङ्कः;
};

संरचना acpi_fadt_शीर्षकम्
{
	संरचना acpi_sdt_शीर्षकम् hdr;
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
	u8_t gpe1_आधारः;
	u8_t cst_cnt;
	u16_t p_lvl2_lat;
	u16_t p_lvl3_lat;
	u16_t flush_आकार;
	u16_t flush_stride;
	u8_t duty_विस्थापनम्;
	u8_t duty_विस्तारः;
	u8_t day_alrm;
	u8_t mon_alrm;
	u8_t century;
	u16_t iapc_boot_arch;
	u8_t reserved1;
	u32_t flags;
	संरचना acpi_generic_स्थानाङ्कः reset_reg;
	u8_t reset_मानम्;
	u8_t reserved2[3];
	u64_t xfacs;
	u64_t xdsdt;
	संरचना acpi_generic_स्थानाङ्कः xpm1a_evt_blk;
	संरचना acpi_generic_स्थानाङ्कः xpm1b_evt_blk;
	संरचना acpi_generic_स्थानाङ्कः xpm1a_cnt_blk;
	संरचना acpi_generic_स्थानाङ्कः xpm1b_cnt_blk;
	संरचना acpi_generic_स्थानाङ्कः xpm2_cnt_blk;
	संरचना acpi_generic_स्थानाङ्कः xpm_tmr_blk;
	संरचना acpi_generic_स्थानाङ्कः xgpe0_blk;
	संरचना acpi_generic_स्थानाङ्कः xgpe1_blk;
};

संरचना acpi_madt_hdr {
	संरचना acpi_sdt_शीर्षकम्	hdr;
	u32_t			local_apic_स्थानाङ्कः;
	u32_t			flags;
};

#परिभाषय ACPI_MADT_TYPE_LAPIC		0
#परिभाषय ACPI_MADT_TYPE_IOAPIC		1
#परिभाषय ACPI_MADT_TYPE_INT_SRC		2
#परिभाषय ACPI_MADT_TYPE_NMI_SRC		3
#परिभाषय ACPI_MADT_TYPE_LAPIC_NMI	4
#परिभाषय ACPI_MADT_TYPE_LAPIC_ADRESS	5
#परिभाषय ACPI_MADT_TYPE_IOSAPIC		6
#परिभाषय ACPI_MADT_TYPE_LSAPIC		7
#परिभाषय ACPI_MADT_TYPE_PLATFORM_INT_SRC	8
#परिभाषय ACPI_MADT_TYPE_Lx2APIC		9
#परिभाषय ACPI_MADT_TYPE_Lx2APIC_NMI	10

संरचना acpi_madt_item_hdr{
	u8_t	type;
	u8_t	दीर्घता;
};

संरचना acpi_madt_lapic {
	संरचना acpi_madt_item_hdr hdr;
	u8_t	acpi_cpu_id;
	u8_t	apic_id;
	u32_t	flags;
};

संरचना acpi_madt_ioapic {
	संरचना acpi_madt_item_hdr hdr;
	u8_t	id;
	u8_t	__reserved;
	u32_t	स्थानाङ्कः;
	u32_t	global_int_आधारः;
};

संरचना acpi_madt_int_src {
	संरचना acpi_madt_item_hdr hdr;
	u8_t	bus;
	u8_t	bus_int;
	u32_t	global_int;
	u16_t	mps_flags;
};

संरचना acpi_madt_nmi {
	संरचना acpi_madt_item_hdr hdr;
	u16_t	flags;
	u32_t	global_int;
};

शून्य acpi_init(शून्य);

शून्य acpi_poweroff(शून्य);

/* 
 * Returns a pointer to the io acpi structure in the MADT table in ACPI. The
 * pointer is valid only until paging is turned off. No memory is allocated in
 * this function thus no memory needs to be freed
 */
संरचना acpi_madt_ioapic * acpi_get_ioapic_अग्र(शून्य);
/* same as above for local APICs */
संरचना acpi_madt_lapic * acpi_get_lapic_अग्र(शून्य);

#यदि_अन्तः /* __ACPI_H__ */
