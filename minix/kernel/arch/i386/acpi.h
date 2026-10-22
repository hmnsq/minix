#నిర్వచితం_కాకపోతే __ACPI_H__
#నిర్వచించు __ACPI_H__

#అన్తర్భావ "kernel/kernel.h"

/* ACPI root system description pointer */
నిర్మాణం acpi_rsdp {
	అక్షరం	signature[8]; /* must be "RSD PTR " */
	u8_t	checksum;
	అక్షరం	oemid[6];
	u8_t	revision;
	u32_t	rsdt_addr;
	u32_t	నిడివి;
}; 

#నిర్వచించు ACPI_SDT_SIGNATURE_LEN	4

#నిర్వచించు ACPI_SDT_SIGNATURE(name)	#name

/* header common to all system description tables */
నిర్మాణం acpi_sdt_శీర్షిక {
	అక్షరం	signature[ACPI_SDT_SIGNATURE_LEN];
	u32_t	నిడివి;
	u8_t	revision;
	u8_t	checksum;
	అక్షరం	oemid[6];
	అక్షరం	oem_పట్టిక_id[8];
	u32_t	oem_revision;
	u32_t	creator_id;
	u32_t	creator_revision;
};

నిర్మాణం acpi_generic_చిరునామా {
	u8_t చిరునామా_space_id;
	u8_t రిజిస్టరు_బిట్టు_వెడల్పు;
	u8_t రిజిస్టరు_బిట్టు_స్థానభ్రంశం;
	u8_t access_కొలత;
	u64_t చిరునామా;
};

నిర్మాణం acpi_fadt_శీర్షిక
{
	నిర్మాణం acpi_sdt_శీర్షిక hdr;
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
	u8_t gpe1_పునాది;
	u8_t cst_cnt;
	u16_t p_lvl2_lat;
	u16_t p_lvl3_lat;
	u16_t flush_కొలత;
	u16_t flush_stride;
	u8_t duty_స్థానభ్రంశం;
	u8_t duty_వెడల్పు;
	u8_t day_alrm;
	u8_t mon_alrm;
	u8_t century;
	u16_t iapc_boot_arch;
	u8_t reserved1;
	u32_t flags;
	నిర్మాణం acpi_generic_చిరునామా reset_reg;
	u8_t reset_విలువ;
	u8_t reserved2[3];
	u64_t xfacs;
	u64_t xdsdt;
	నిర్మాణం acpi_generic_చిరునామా xpm1a_evt_blk;
	నిర్మాణం acpi_generic_చిరునామా xpm1b_evt_blk;
	నిర్మాణం acpi_generic_చిరునామా xpm1a_cnt_blk;
	నిర్మాణం acpi_generic_చిరునామా xpm1b_cnt_blk;
	నిర్మాణం acpi_generic_చిరునామా xpm2_cnt_blk;
	నిర్మాణం acpi_generic_చిరునామా xpm_tmr_blk;
	నిర్మాణం acpi_generic_చిరునామా xgpe0_blk;
	నిర్మాణం acpi_generic_చిరునామా xgpe1_blk;
};

నిర్మాణం acpi_madt_hdr {
	నిర్మాణం acpi_sdt_శీర్షిక	hdr;
	u32_t			local_apic_చిరునామా;
	u32_t			flags;
};

#నిర్వచించు ACPI_MADT_TYPE_LAPIC		0
#నిర్వచించు ACPI_MADT_TYPE_IOAPIC		1
#నిర్వచించు ACPI_MADT_TYPE_INT_SRC		2
#నిర్వచించు ACPI_MADT_TYPE_NMI_SRC		3
#నిర్వచించు ACPI_MADT_TYPE_LAPIC_NMI	4
#నిర్వచించు ACPI_MADT_TYPE_LAPIC_ADRESS	5
#నిర్వచించు ACPI_MADT_TYPE_IOSAPIC		6
#నిర్వచించు ACPI_MADT_TYPE_LSAPIC		7
#నిర్వచించు ACPI_MADT_TYPE_PLATFORM_INT_SRC	8
#నిర్వచించు ACPI_MADT_TYPE_Lx2APIC		9
#నిర్వచించు ACPI_MADT_TYPE_Lx2APIC_NMI	10

నిర్మాణం acpi_madt_అంశం_hdr{
	u8_t	type;
	u8_t	నిడివి;
};

నిర్మాణం acpi_madt_lapic {
	నిర్మాణం acpi_madt_అంశం_hdr hdr;
	u8_t	acpi_cpu_id;
	u8_t	apic_id;
	u32_t	flags;
};

నిర్మాణం acpi_madt_ioapic {
	నిర్మాణం acpi_madt_అంశం_hdr hdr;
	u8_t	id;
	u8_t	__reserved;
	u32_t	చిరునామా;
	u32_t	global_int_పునాది;
};

నిర్మాణం acpi_madt_int_src {
	నిర్మాణం acpi_madt_అంశం_hdr hdr;
	u8_t	bus;
	u8_t	bus_int;
	u32_t	global_int;
	u16_t	mps_flags;
};

నిర్మాణం acpi_madt_nmi {
	నిర్మాణం acpi_madt_అంశం_hdr hdr;
	u16_t	flags;
	u32_t	global_int;
};

శూన్య acpi_init(శూన్య);

శూన్య acpi_poweroff(శూన్య);

/* 
 * Returns a pointer to the io acpi structure in the MADT table in ACPI. The
 * pointer is valid only until paging is turned off. No memory is allocated in
 * this function thus no memory needs to be freed
 */
నిర్మాణం acpi_madt_ioapic * acpi_get_ioapic_తదుపరి(శూన్య);
/* same as above for local APICs */
నిర్మాణం acpi_madt_lapic * acpi_get_lapic_తదుపరి(శూన్య);

#అయితే_ముగింపు /* __ACPI_H__ */
