
#include <string.h>

#include "acpi.h"
#include "arch_proto.h"

typedef цео ((* acpi_read_t)(phys_bytes addr, void * buff, size_t size));

struct acpi_rsdp acpi_rsdp;

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

static struct acpi_rsdt {
	struct acpi_sdt_header	hdr;
	u32_t			data[MAX_RSDT];
} rsdt;
	
static struct {
	char	signature [ACPI_SDT_SIGNATURE_LEN + 1];
	size_t	length;
} sdt_trans[MAX_RSDT];

static цео sdt_count;
static u16_t pm1a_cnt_blk = 0;
static u16_t pm1b_cnt_blk = 0;
static u16_t slp_typa = 0;
static u16_t slp_typb = 0;

static цео acpi_check_csum(struct acpi_sdt_header * tb, size_t size)
{
	u8_t total = 0;
	цео i;
	за (i = 0; i < size; i++)
		total += ((unsigned char *)tb)[i];
	врати total == 0 ? 0 : -1;
}

static цео acpi_check_signature(const char * orig, const char * match)
{
	врати strncmp(orig, match, ACPI_SDT_SIGNATURE_LEN);
}

static u32_t acpi_phys2vir(u32_t p)
{
	ако(!vm_running) {
		DEBUGEXTRA(("acpi: returning 0x%lx as vir addr\n", p));
		врати p;
	}
	panic("acpi: can't get virtual address of arbitrary physical address");
}

static цео acpi_phys_copy(phys_bytes phys, void *target, size_t len)
{
	ако(!vm_running) {
		memcpy(target, (void *) phys, len);
		врати 0;
	}
	panic("can't acpi_phys_copy with vm");
}

static цео acpi_read_sdt_at(phys_bytes addr,
				struct acpi_sdt_header * tb,
				size_t size,
				const char * name)
{
	struct acpi_sdt_header hdr;

	/* if NULL is supplied, we only return the size of the table */
	ако (tb == NULL) {
		ако (read_func(addr, &hdr, sizeof(struct acpi_sdt_header))) {
			printf("ERROR acpi cannot read %s header\n", name);
			врати -1;
		}

		врати hdr.length;
	}

	ако (read_func(addr, tb, sizeof(struct acpi_sdt_header))) {
		printf("ERROR acpi cannot read %s header\n", name);
		врати -1;
	}

	ако (acpi_check_signature(tb->signature, name)) {
		printf("ERROR acpi %s signature does not match\n", name);
		врати -1;
	}

	ако (size < tb->length) {
		printf("ERROR acpi buffer too small for %s\n", name);
		врати -1;
	}

	ако (read_func(addr, tb, size)) {
		printf("ERROR acpi cannot read %s\n", name);
		врати -1;
	}

	ако (acpi_check_csum(tb, tb->length)) {
		printf("ERROR acpi %s checksum does not match\n", name);
		врати -1;
	}

	врати tb->length;
}

phys_bytes acpi_get_table_base(const char * name)
{
	цео i;

	за(i = 0; i < sdt_count; i++) {
		ако (strncmp(name, sdt_trans[i].signature,
					ACPI_SDT_SIGNATURE_LEN) == 0)
			врати (phys_bytes) rsdt.data[i];
	}

	врати (phys_bytes) NULL;
}

size_t acpi_get_table_length(const char * name)
{
	цео i;

	за(i = 0; i < sdt_count; i++) {
		ако (strncmp(name, sdt_trans[i].signature,
					ACPI_SDT_SIGNATURE_LEN) == 0)
			врати sdt_trans[i].length;
	}

	врати 0;
}

static void * acpi_madt_get_typed_item(struct acpi_madt_hdr * hdr,
					unsigned char type,
					unsigned idx)
{
	u8_t * t, * end;
	цео i;

	t = (u8_t *) hdr + sizeof(struct acpi_madt_hdr);
	end = (u8_t *) hdr + hdr->hdr.length;

	i = 0;
	док(t < end) {
		ако (type == ((struct acpi_madt_item_hdr *) t)->type) {
			ако (i == idx)
				врати t;
			инако
				i++;
		}
		t += ((struct acpi_madt_item_hdr *) t)->length;
	}

	врати NULL;
}

#ако 0
static void * acpi_madt_get_item(struct acpi_madt_hdr * hdr,
				unsigned idx)
{
	u8_t * t, * end;
	цео i;

	t = (u8_t *) hdr + sizeof(struct acpi_madt_hdr);
	end = (u8_t *) hdr + hdr->hdr.length;

	за(i = 0 ; i <= idx && t < end; i++) {
		ако (i == idx)
			врати t;
		t += ((struct acpi_madt_item_hdr *) t)->length;
	}

	врати NULL;
}
#endif

static цео acpi_rsdp_test(void * buff)
{
	struct acpi_rsdp * rsdp = (struct acpi_rsdp *) buff;

	ако (!platform_tbl_checksum_ok(buff, 20))
		врати 0;
	ако (strncmp(rsdp->signature, "RSD PTR ", 8))
		врати 0;

	врати 1;
}

static цео get_acpi_rsdp(void)
{
	u16_t ebda;
	/*
	 * Read 40:0Eh - to find the starting address of the EBDA.
	 */
	acpi_phys_copy (0x40E, &ebda, sizeof(ebda));
	ако (ebda) {
		ebda <<= 4;
		ако(platform_tbl_ptr(ebda, ebda + 0x400, 16, &acpi_rsdp,
					sizeof(acpi_rsdp), &machine.acpi_rsdp,
					acpi_rsdp_test))
			врати 1;
	} 

	/* try BIOS read only mem space */
	ако(platform_tbl_ptr(0xE0000, 0x100000, 16, &acpi_rsdp,
				sizeof(acpi_rsdp), &machine.acpi_rsdp,
				acpi_rsdp_test))
		врати 1;
	
	machine.acpi_rsdp = 0; /* RSDP cannot be found at this address therefore
				  it is a valid negative value */
	врати 0;
}

static void acpi_init_poweroff(void)
{
	u8_t *ptr = NULL;
	u8_t *start = NULL;
	u8_t *end = NULL;
	struct acpi_fadt_header *fadt_header = NULL;
	struct acpi_rsdt * dsdt_header = NULL;
	char *msg = NULL;

	/* Everything used here existed since ACPI spec 1.0 */
	/* So we can safely use them */
	fadt_header = (struct acpi_fadt_header *)
		acpi_phys2vir(acpi_get_table_base("FACP"));
	ако (fadt_header == NULL) {
		msg = "Could not load FACP";
		goto exit;
	}

	dsdt_header = (struct acpi_rsdt *)
		acpi_phys2vir((phys_bytes) fadt_header->dsdt);
	ако (dsdt_header == NULL) {
		msg = "Could not load DSDT";
		goto exit;
	}

	pm1a_cnt_blk = fadt_header->pm1a_cnt_blk;
	pm1b_cnt_blk = fadt_header->pm1b_cnt_blk;

	ptr = start = (u8_t *) dsdt_header->data;
	end = start + dsdt_header->hdr.length - 4;

	/* See http://forum.osdev.org/viewtopic.php?t=16990 */
	/* for layout of \_S5 */
	док (ptr < end && memcmp(ptr, "_S5_", 4) != 0)
		ptr++;

	msg = "Could not read S5 data. Use default SLP_TYPa and SLP_TYPb";
	ако (ptr >= end || ptr == start)
		goto exit;

	/* validate AML structure */
	ако (*(ptr + AMI_S5_PACKAGE_OP_OFFSET) != AMI_PACKAGE_OP_CODE)
		goto exit;

	ако ((ptr < start + (-AMI_S5_NAME_OP_OFFSET_2) ||
		(*(ptr + AMI_S5_NAME_OP_OFFSET_2) != AMI_NAME_OP_CODE ||
		 *(ptr + AMI_S5_NAME_OP_OFFSET_2 + 1) != '\\')) &&
		*(ptr + AMI_S5_NAME_OP_OFFSET_1) != AMI_NAME_OP_CODE)
		goto exit;

	ptr += AMI_S5_PACKET_LENGTH_OFFSET;
	ако (ptr >= end)
		goto exit;

	/* package length */
	ptr += ((*ptr & AMI_PACKAGE_LENGTH_ENCODING_BITS_MASK) >>
		AMI_PACKAGE_LENGTH_ENCODING_BITS_SHIFT) +
		AMI_MIN_PACKAGE_LENGTH + AMI_NUM_ELEMENTS_LENGTH;
	ако (ptr >= end)
		goto exit;

	ако (*ptr == AMI_BYTE_PREFIX_CODE)
		ptr++; /* skip byte prefix */

	slp_typa = (*ptr) << AMI_SLP_TYPA_SHIFT;

	ptr++; /* move to SLP_TYPb */
	ако (*ptr == AMI_BYTE_PREFIX_CODE)
		ptr++; /* skip byte prefix */

	slp_typb = (*ptr) << AMI_SLP_TYPB_SHIFT;

	msg = "poweroff initialized";

exit:
	ако (msg) {
		DEBUGBASIC(("acpi: %s\n", msg));
	}
}

void acpi_init(void)
{
	цео s, i;
	read_func = acpi_phys_copy;

	ако (!get_acpi_rsdp()) {
		printf("WARNING : Cannot configure ACPI\n");
		врати;
	}
	
	s = acpi_read_sdt_at(acpi_rsdp.rsdt_addr, (struct acpi_sdt_header *) &rsdt,
			sizeof(struct acpi_rsdt), ACPI_SDT_SIGNATURE(RSDT));

	sdt_count = (s - sizeof(struct acpi_sdt_header)) / sizeof(u32_t);

	за (i = 0; i < sdt_count; i++) {
		struct acpi_sdt_header hdr;
		цео j;
		ако (read_func(rsdt.data[i], &hdr, sizeof(struct acpi_sdt_header))) {
			printf("ERROR acpi cannot read header at 0x%x\n",
								rsdt.data[i]);
			врати;
		}

		за (j = 0 ; j < ACPI_SDT_SIGNATURE_LEN; j++)
			sdt_trans[i].signature[j] = hdr.signature[j];
		sdt_trans[i].signature[ACPI_SDT_SIGNATURE_LEN] = '\0';
		sdt_trans[i].length = hdr.length;
	}

	acpi_init_poweroff();
}

struct acpi_madt_ioapic * acpi_get_ioapic_next(void)
{
	static unsigned idx = 0;
	static struct acpi_madt_hdr * madt_hdr;

	struct acpi_madt_ioapic * ret;

	ако (idx == 0) {
		madt_hdr = (struct acpi_madt_hdr *)
			acpi_phys2vir(acpi_get_table_base("APIC"));
		ако (madt_hdr == NULL)
			врати NULL;
	}

	ret = (struct acpi_madt_ioapic *)
		acpi_madt_get_typed_item(madt_hdr, ACPI_MADT_TYPE_IOAPIC, idx);
	ако (ret)
		idx++;

	врати ret;
}

struct acpi_madt_lapic * acpi_get_lapic_next(void)
{
	static unsigned idx = 0;
	static struct acpi_madt_hdr * madt_hdr;

	struct acpi_madt_lapic * ret;

	ако (idx == 0) {
		madt_hdr = (struct acpi_madt_hdr *)
			acpi_phys2vir(acpi_get_table_base("APIC"));
		ако (madt_hdr == NULL)
			врати NULL;
	}

	за (;;) {
		ret = (struct acpi_madt_lapic *)
			acpi_madt_get_typed_item(madt_hdr,
					ACPI_MADT_TYPE_LAPIC, idx);
		ако (!ret)
			break;

		idx++;

		/* report only usable CPUs */
		ако (ret->flags & 1)
			break;
	}

	врати ret;
}

void __k_unpaged_acpi_poweroff(void)
{
	/* NO OP poweroff symbol*/
}

void acpi_poweroff(void)
{
	ако (pm1a_cnt_blk == 0) {
		врати;
	}
	outw(pm1a_cnt_blk, slp_typa | SLP_EN_CODE);
	ако (pm1b_cnt_blk != 0) {
		outw(pm1b_cnt_blk, slp_typb | SLP_EN_CODE);
	}
}
