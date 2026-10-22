#اگر_تعریف_نشده __ACPI_H__
#تعریف_کن __ACPI_H__

#ضمن "kernel/kernel.h"

/* ACPI root system description pointer */
بنية acpi_rsdp {
	حرف	signature[8]; /* must be "RSD PTR " */
	u8_t	checksum;
	حرف	oemid[6];
	u8_t	revision;
	u32_t	rsdt_addr;
	u32_t	درازی;
}; 

#تعریف_کن ACPI_SDT_SIGNATURE_LEN	4

#تعریف_کن ACPI_SDT_SIGNATURE(name)	#name

/* header common to all system description tables */
بنية acpi_sdt_سرآیند {
	حرف	signature[ACPI_SDT_SIGNATURE_LEN];
	u32_t	درازی;
	u8_t	revision;
	u8_t	checksum;
	حرف	oemid[6];
	حرف	oem_جدول_id[8];
	u32_t	oem_revision;
	u32_t	creator_id;
	u32_t	creator_revision;
};

بنية acpi_generic_آدرس {
	u8_t آدرس_space_id;
	u8_t ثبات_بیت_عرض;
	u8_t ثبات_بیت_جابه_جایی;
	u8_t access_اندازه;
	u64_t آدرس;
};

بنية acpi_fadt_سرآیند
{
	بنية acpi_sdt_سرآیند hdr;
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
	u8_t gpe1_پایه;
	u8_t cst_cnt;
	u16_t p_lvl2_lat;
	u16_t p_lvl3_lat;
	u16_t flush_اندازه;
	u16_t flush_stride;
	u8_t duty_جابه_جایی;
	u8_t duty_عرض;
	u8_t day_alrm;
	u8_t mon_alrm;
	u8_t century;
	u16_t iapc_boot_arch;
	u8_t reserved1;
	u32_t flags;
	بنية acpi_generic_آدرس reset_reg;
	u8_t reset_مقدار;
	u8_t reserved2[3];
	u64_t xfacs;
	u64_t xdsdt;
	بنية acpi_generic_آدرس xpm1a_evt_blk;
	بنية acpi_generic_آدرس xpm1b_evt_blk;
	بنية acpi_generic_آدرس xpm1a_cnt_blk;
	بنية acpi_generic_آدرس xpm1b_cnt_blk;
	بنية acpi_generic_آدرس xpm2_cnt_blk;
	بنية acpi_generic_آدرس xpm_tmr_blk;
	بنية acpi_generic_آدرس xgpe0_blk;
	بنية acpi_generic_آدرس xgpe1_blk;
};

بنية acpi_madt_hdr {
	بنية acpi_sdt_سرآیند	hdr;
	u32_t			local_apic_آدرس;
	u32_t			flags;
};

#تعریف_کن ACPI_MADT_TYPE_LAPIC		0
#تعریف_کن ACPI_MADT_TYPE_IOAPIC		1
#تعریف_کن ACPI_MADT_TYPE_INT_SRC		2
#تعریف_کن ACPI_MADT_TYPE_NMI_SRC		3
#تعریف_کن ACPI_MADT_TYPE_LAPIC_NMI	4
#تعریف_کن ACPI_MADT_TYPE_LAPIC_ADRESS	5
#تعریف_کن ACPI_MADT_TYPE_IOSAPIC		6
#تعریف_کن ACPI_MADT_TYPE_LSAPIC		7
#تعریف_کن ACPI_MADT_TYPE_PLATFORM_INT_SRC	8
#تعریف_کن ACPI_MADT_TYPE_Lx2APIC		9
#تعریف_کن ACPI_MADT_TYPE_Lx2APIC_NMI	10

بنية acpi_madt_قلم_hdr{
	u8_t	type;
	u8_t	درازی;
};

بنية acpi_madt_lapic {
	بنية acpi_madt_قلم_hdr hdr;
	u8_t	acpi_cpu_id;
	u8_t	apic_id;
	u32_t	flags;
};

بنية acpi_madt_ioapic {
	بنية acpi_madt_قلم_hdr hdr;
	u8_t	id;
	u8_t	__reserved;
	u32_t	آدرس;
	u32_t	global_int_پایه;
};

بنية acpi_madt_int_src {
	بنية acpi_madt_قلم_hdr hdr;
	u8_t	bus;
	u8_t	bus_int;
	u32_t	global_int;
	u16_t	mps_flags;
};

بنية acpi_madt_nmi {
	بنية acpi_madt_قلم_hdr hdr;
	u16_t	flags;
	u32_t	global_int;
};

فراغ acpi_init(فراغ);

فراغ acpi_poweroff(فراغ);

/* 
 * Returns a pointer to the io acpi structure in the MADT table in ACPI. The
 * pointer is valid only until paging is turned off. No memory is allocated in
 * this function thus no memory needs to be freed
 */
بنية acpi_madt_ioapic * acpi_get_ioapic_بعدی(فراغ);
/* same as above for local APICs */
بنية acpi_madt_lapic * acpi_get_lapic_بعدی(فراغ);

#پایان_اگر /* __ACPI_H__ */
