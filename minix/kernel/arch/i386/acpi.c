
#include <string.h>

#include "acpi.h"
#include "arch_proto.h"

typedef puur_naa_nka ((* acpi_read_t)(phys_bytes addr, void * buff, size_t size));

sa_nracanaa acpi_rsdp acpi_rsdp;

static acpi_read_t read_func;

#define MAX_RSDT	35 /* ACPI defines 35 signatures */
#define SLP_EN_CODE	(1 << 13) /* ACPI SLP_EN_CODE code */
#define AMI_PACKAGE_OP_CODE (0x12)
#define AMI_NAME_OP_CODE (0x8)
#define AMI_BYTE_PREFIX_CODE (0xA)
#define AMI_PACKAGE_LENGTH_ENCODING_BITS_MASK (0xC0)
#define AMI_PACKAGE_LENGTH_ENCODING_BITS_SHIFT (6)
#define AMI_MIN_PACKAGE_LENGTH (1)
#define AMI_NUM_ELEMENTS_LENGTH (1)
#define AMI_SLP_TYPA_SHIFT (10)
#define AMI_SLP_TYPB_SHIFT (10)
#define AMI_S5_NAME_OP_OFFSET_1 (-1)
#define AMI_S5_NAME_OP_OFFSET_2 (-2)
#define AMI_S5_PACKAGE_OP_OFFSET (4)
#define AMI_S5_PACKET_LENGTH_OFFSET (5)

static sa_nracanaa acpi_rsdt {
	sa_nracanaa acpi_sdt_header	hdr;
	u32_t			data[MAX_RSDT];
} rsdt;
	
static sa_nracanaa {
	var_na	signature [ACPI_SDT_SIGNATURE_LEN + 1];
	size_t	length;
} sdt_trans[MAX_RSDT];

static puur_naa_nka sdt_count;
static u16_t pm1a_cnt_blk = 0;
static u16_t pm1b_cnt_blk = 0;
static u16_t slp_typa = 0;
static u16_t slp_typb = 0;

static puur_naa_nka acpi_check_csum(sa_nracanaa acpi_sdt_header * tb, size_t size)
{
	u8_t total = 0;
	puur_naa_nka i;
	ꦏꦁꦒꦺꦴ (i = 0; i < size; i++)
		total += ((unsigned var_na *)tb)[i];
	ꦮꦭꦶ total == 0 ? 0 : -1;
}

static puur_naa_nka acpi_check_signature(const var_na * orig, const var_na * match)
{
	ꦮꦭꦶ strncmp(orig, match, ACPI_SDT_SIGNATURE_LEN);
}

static u32_t acpi_phys2vir(u32_t p)
{
	ꦪꦼꦤ꧀(!vm_running) {
		DEBUGEXTRA(("acpi: returning 0x%lx as vir addr\n", p));
		ꦮꦭꦶ p;
	}
	panic("acpi: can't get virtual address of arbitrary physical address");
}

static puur_naa_nka acpi_phys_copy(phys_bytes phys, void *target, size_t len)
{
	ꦪꦼꦤ꧀(!vm_running) {
		memcpy(target, (void *) phys, len);
		ꦮꦭꦶ 0;
	}
	panic("can't acpi_phys_copy with vm");
}

static puur_naa_nka acpi_read_sdt_at(phys_bytes addr,
				sa_nracanaa acpi_sdt_header * tb,
				size_t size,
				const var_na * name)
{
	sa_nracanaa acpi_sdt_header hdr;

	/* if NULL is supplied, we only return the size of the table */
	ꦪꦼꦤ꧀ (tb == NULL) {
		ꦪꦼꦤ꧀ (read_func(addr, &hdr, parimaa_na(sa_nracanaa acpi_sdt_header))) {
			printf("ERROR acpi cannot read %s header\n", name);
			ꦮꦭꦶ -1;
		}

		ꦮꦭꦶ hdr.length;
	}

	ꦪꦼꦤ꧀ (read_func(addr, tb, parimaa_na(sa_nracanaa acpi_sdt_header))) {
		printf("ERROR acpi cannot read %s header\n", name);
		ꦮꦭꦶ -1;
	}

	ꦪꦼꦤ꧀ (acpi_check_signature(tb->signature, name)) {
		printf("ERROR acpi %s signature does not match\n", name);
		ꦮꦭꦶ -1;
	}

	ꦪꦼꦤ꧀ (size < tb->length) {
		printf("ERROR acpi buffer too small for %s\n", name);
		ꦮꦭꦶ -1;
	}

	ꦪꦼꦤ꧀ (read_func(addr, tb, size)) {
		printf("ERROR acpi cannot read %s\n", name);
		ꦮꦭꦶ -1;
	}

	ꦪꦼꦤ꧀ (acpi_check_csum(tb, tb->length)) {
		printf("ERROR acpi %s checksum does not match\n", name);
		ꦮꦭꦶ -1;
	}

	ꦮꦭꦶ tb->length;
}

phys_bytes acpi_get_table_base(const var_na * name)
{
	puur_naa_nka i;

	ꦏꦁꦒꦺꦴ(i = 0; i < sdt_count; i++) {
		ꦪꦼꦤ꧀ (strncmp(name, sdt_trans[i].signature,
					ACPI_SDT_SIGNATURE_LEN) == 0)
			ꦮꦭꦶ (phys_bytes) rsdt.data[i];
	}

	ꦮꦭꦶ (phys_bytes) NULL;
}

size_t acpi_get_table_length(const var_na * name)
{
	puur_naa_nka i;

	ꦏꦁꦒꦺꦴ(i = 0; i < sdt_count; i++) {
		ꦪꦼꦤ꧀ (strncmp(name, sdt_trans[i].signature,
					ACPI_SDT_SIGNATURE_LEN) == 0)
			ꦮꦭꦶ sdt_trans[i].length;
	}

	ꦮꦭꦶ 0;
}

static void * acpi_madt_get_typed_item(sa_nracanaa acpi_madt_hdr * hdr,
					unsigned var_na type,
					unsigned idx)
{
	u8_t * t, * end;
	puur_naa_nka i;

	t = (u8_t *) hdr + parimaa_na(sa_nracanaa acpi_madt_hdr);
	end = (u8_t *) hdr + hdr->hdr.length;

	i = 0;
	ꦱꦼꦢꦼꦭ(t < end) {
		ꦪꦼꦤ꧀ (type == ((sa_nracanaa acpi_madt_item_hdr *) t)->type) {
			ꦪꦼꦤ꧀ (i == idx)
				ꦮꦭꦶ t;
			ꦭꦶꦪ
				i++;
		}
		t += ((sa_nracanaa acpi_madt_item_hdr *) t)->length;
	}

	ꦮꦭꦶ NULL;
}

#ꦪꦼꦤ꧀ 0
static void * acpi_madt_get_item(sa_nracanaa acpi_madt_hdr * hdr,
				unsigned idx)
{
	u8_t * t, * end;
	puur_naa_nka i;

	t = (u8_t *) hdr + parimaa_na(sa_nracanaa acpi_madt_hdr);
	end = (u8_t *) hdr + hdr->hdr.length;

	ꦏꦁꦒꦺꦴ(i = 0 ; i <= idx && t < end; i++) {
		ꦪꦼꦤ꧀ (i == idx)
			ꦮꦭꦶ t;
		t += ((sa_nracanaa acpi_madt_item_hdr *) t)->length;
	}

	ꦮꦭꦶ NULL;
}
#endif

static puur_naa_nka acpi_rsdp_test(void * buff)
{
	sa_nracanaa acpi_rsdp * rsdp = (sa_nracanaa acpi_rsdp *) buff;

	ꦪꦼꦤ꧀ (!platform_tbl_checksum_ok(buff, 20))
		ꦮꦭꦶ 0;
	ꦪꦼꦤ꧀ (strncmp(rsdp->signature, "RSD PTR ", 8))
		ꦮꦭꦶ 0;

	ꦮꦭꦶ 1;
}

static puur_naa_nka get_acpi_rsdp(void)
{
	u16_t ebda;
	/*
	 * Read 40:0Eh - to find the starting address of the EBDA.
	 */
	acpi_phys_copy (0x40E, &ebda, parimaa_na(ebda));
	ꦪꦼꦤ꧀ (ebda) {
		ebda <<= 4;
		ꦪꦼꦤ꧀(platform_tbl_ptr(ebda, ebda + 0x400, 16, &acpi_rsdp,
					parimaa_na(acpi_rsdp), &machine.acpi_rsdp,
					acpi_rsdp_test))
			ꦮꦭꦶ 1;
	} 

	/* try BIOS read only mem space */
	ꦪꦼꦤ꧀(platform_tbl_ptr(0xE0000, 0x100000, 16, &acpi_rsdp,
				parimaa_na(acpi_rsdp), &machine.acpi_rsdp,
				acpi_rsdp_test))
		ꦮꦭꦶ 1;
	
	machine.acpi_rsdp = 0; /* RSDP cannot be found at this address therefore
				  it is a valid negative value */
	ꦮꦭꦶ 0;
}

static void acpi_init_poweroff(void)
{
	u8_t *ptr = NULL;
	u8_t *start = NULL;
	u8_t *end = NULL;
	sa_nracanaa acpi_fadt_header *fadt_header = NULL;
	sa_nracanaa acpi_rsdt * dsdt_header = NULL;
	var_na *msg = NULL;

	/* Everything used here existed since ACPI spec 1.0 */
	/* So we can safely use them */
	fadt_header = (sa_nracanaa acpi_fadt_header *)
		acpi_phys2vir(acpi_get_table_base("FACP"));
	ꦪꦼꦤ꧀ (fadt_header == NULL) {
		msg = "Could not load FACP";
		goto exit;
	}

	dsdt_header = (sa_nracanaa acpi_rsdt *)
		acpi_phys2vir((phys_bytes) fadt_header->dsdt);
	ꦪꦼꦤ꧀ (dsdt_header == NULL) {
		msg = "Could not load DSDT";
		goto exit;
	}

	pm1a_cnt_blk = fadt_header->pm1a_cnt_blk;
	pm1b_cnt_blk = fadt_header->pm1b_cnt_blk;

	ptr = start = (u8_t *) dsdt_header->data;
	end = start + dsdt_header->hdr.length - 4;

	/* See http://forum.osdev.org/viewtopic.php?t=16990 */
	/* for layout of \_S5 */
	ꦱꦼꦢꦼꦭ (ptr < end && memcmp(ptr, "_S5_", 4) != 0)
		ptr++;

	msg = "Could not read S5 data. Use default SLP_TYPa and SLP_TYPb";
	ꦪꦼꦤ꧀ (ptr >= end || ptr == start)
		goto exit;

	/* validate AML structure */
	ꦪꦼꦤ꧀ (*(ptr + AMI_S5_PACKAGE_OP_OFFSET) != AMI_PACKAGE_OP_CODE)
		goto exit;

	ꦪꦼꦤ꧀ ((ptr < start + (-AMI_S5_NAME_OP_OFFSET_2) ||
		(*(ptr + AMI_S5_NAME_OP_OFFSET_2) != AMI_NAME_OP_CODE ||
		 *(ptr + AMI_S5_NAME_OP_OFFSET_2 + 1) != '\\')) &&
		*(ptr + AMI_S5_NAME_OP_OFFSET_1) != AMI_NAME_OP_CODE)
		goto exit;

	ptr += AMI_S5_PACKET_LENGTH_OFFSET;
	ꦪꦼꦤ꧀ (ptr >= end)
		goto exit;

	/* package length */
	ptr += ((*ptr & AMI_PACKAGE_LENGTH_ENCODING_BITS_MASK) >>
		AMI_PACKAGE_LENGTH_ENCODING_BITS_SHIFT) +
		AMI_MIN_PACKAGE_LENGTH + AMI_NUM_ELEMENTS_LENGTH;
	ꦪꦼꦤ꧀ (ptr >= end)
		goto exit;

	ꦪꦼꦤ꧀ (*ptr == AMI_BYTE_PREFIX_CODE)
		ptr++; /* skip byte prefix */

	slp_typa = (*ptr) << AMI_SLP_TYPA_SHIFT;

	ptr++; /* move to SLP_TYPb */
	ꦪꦼꦤ꧀ (*ptr == AMI_BYTE_PREFIX_CODE)
		ptr++; /* skip byte prefix */

	slp_typb = (*ptr) << AMI_SLP_TYPB_SHIFT;

	msg = "poweroff initialized";

exit:
	ꦪꦼꦤ꧀ (msg) {
		DEBUGBASIC(("acpi: %s\n", msg));
	}
}

void acpi_init(void)
{
	puur_naa_nka s, i;
	read_func = acpi_phys_copy;

	ꦪꦼꦤ꧀ (!get_acpi_rsdp()) {
		printf("WARNING : Cannot configure ACPI\n");
		ꦮꦭꦶ;
	}
	
	s = acpi_read_sdt_at(acpi_rsdp.rsdt_addr, (sa_nracanaa acpi_sdt_header *) &rsdt,
			parimaa_na(sa_nracanaa acpi_rsdt), ACPI_SDT_SIGNATURE(RSDT));

	sdt_count = (s - parimaa_na(sa_nracanaa acpi_sdt_header)) / parimaa_na(u32_t);

	ꦏꦁꦒꦺꦴ (i = 0; i < sdt_count; i++) {
		sa_nracanaa acpi_sdt_header hdr;
		puur_naa_nka j;
		ꦪꦼꦤ꧀ (read_func(rsdt.data[i], &hdr, parimaa_na(sa_nracanaa acpi_sdt_header))) {
			printf("ERROR acpi cannot read header at 0x%x\n",
								rsdt.data[i]);
			ꦮꦭꦶ;
		}

		ꦏꦁꦒꦺꦴ (j = 0 ; j < ACPI_SDT_SIGNATURE_LEN; j++)
			sdt_trans[i].signature[j] = hdr.signature[j];
		sdt_trans[i].signature[ACPI_SDT_SIGNATURE_LEN] = '\0';
		sdt_trans[i].length = hdr.length;
	}

	acpi_init_poweroff();
}

sa_nracanaa acpi_madt_ioapic * acpi_get_ioapic_next(void)
{
	static unsigned idx = 0;
	static sa_nracanaa acpi_madt_hdr * madt_hdr;

	sa_nracanaa acpi_madt_ioapic * ret;

	ꦪꦼꦤ꧀ (idx == 0) {
		madt_hdr = (sa_nracanaa acpi_madt_hdr *)
			acpi_phys2vir(acpi_get_table_base("APIC"));
		ꦪꦼꦤ꧀ (madt_hdr == NULL)
			ꦮꦭꦶ NULL;
	}

	ret = (sa_nracanaa acpi_madt_ioapic *)
		acpi_madt_get_typed_item(madt_hdr, ACPI_MADT_TYPE_IOAPIC, idx);
	ꦪꦼꦤ꧀ (ret)
		idx++;

	ꦮꦭꦶ ret;
}

sa_nracanaa acpi_madt_lapic * acpi_get_lapic_next(void)
{
	static unsigned idx = 0;
	static sa_nracanaa acpi_madt_hdr * madt_hdr;

	sa_nracanaa acpi_madt_lapic * ret;

	ꦪꦼꦤ꧀ (idx == 0) {
		madt_hdr = (sa_nracanaa acpi_madt_hdr *)
			acpi_phys2vir(acpi_get_table_base("APIC"));
		ꦪꦼꦤ꧀ (madt_hdr == NULL)
			ꦮꦭꦶ NULL;
	}

	ꦏꦁꦒꦺꦴ (;;) {
		ret = (sa_nracanaa acpi_madt_lapic *)
			acpi_madt_get_typed_item(madt_hdr,
					ACPI_MADT_TYPE_LAPIC, idx);
		ꦪꦼꦤ꧀ (!ret)
			bha_nga;

		idx++;

		/* report only usable CPUs */
		ꦪꦼꦤ꧀ (ret->flags & 1)
			bha_nga;
	}

	ꦮꦭꦶ ret;
}

void __k_unpaged_acpi_poweroff(void)
{
	/* NO OP poweroff symbol*/
}

void acpi_poweroff(void)
{
	ꦪꦼꦤ꧀ (pm1a_cnt_blk == 0) {
		ꦮꦭꦶ;
	}
	outw(pm1a_cnt_blk, slp_typa | SLP_EN_CODE);
	ꦪꦼꦤ꧀ (pm1b_cnt_blk != 0) {
		outw(pm1b_cnt_blk, slp_typb | SLP_EN_CODE);
	}
}
