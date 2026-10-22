
#include <string.h>

#include "acpi.h"
#include "arch_proto.h"

typedef 整数 ((* acpi_read_t)(phys_bytes addr, void * buff, size_t size));

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

static 整数 sdt_count;
static u16_t pm1a_cnt_blk = 0;
static u16_t pm1b_cnt_blk = 0;
static u16_t slp_typa = 0;
static u16_t slp_typb = 0;

static 整数 acpi_check_csum(struct acpi_sdt_header * tb, size_t size)
{
	u8_t total = 0;
	整数 i;
	为 (i = 0; i < size; i++)
		total += ((unsigned char *)tb)[i];
	返回 total == 0 ? 0 : -1;
}

static 整数 acpi_check_signature(const char * orig, const char * match)
{
	返回 strncmp(orig, match, ACPI_SDT_SIGNATURE_LEN);
}

static u32_t acpi_phys2vir(u32_t p)
{
	如果(!vm_running) {
		DEBUGEXTRA(("acpi: returning 0x%lx as vir addr\n", p));
		返回 p;
	}
	panic("acpi: can't get virtual address of arbitrary physical address");
}

static 整数 acpi_phys_copy(phys_bytes phys, void *target, size_t len)
{
	如果(!vm_running) {
		memcpy(target, (void *) phys, len);
		返回 0;
	}
	panic("can't acpi_phys_copy with vm");
}

static 整数 acpi_read_sdt_at(phys_bytes addr,
				struct acpi_sdt_header * tb,
				size_t size,
				const char * name)
{
	struct acpi_sdt_header hdr;

	/* if NULL is supplied, we only return the size of the table */
	如果 (tb == NULL) {
		如果 (read_func(addr, &hdr, sizeof(struct acpi_sdt_header))) {
			printf("ERROR acpi cannot read %s header\n", name);
			返回 -1;
		}

		返回 hdr.length;
	}

	如果 (read_func(addr, tb, sizeof(struct acpi_sdt_header))) {
		printf("ERROR acpi cannot read %s header\n", name);
		返回 -1;
	}

	如果 (acpi_check_signature(tb->signature, name)) {
		printf("ERROR acpi %s signature does not match\n", name);
		返回 -1;
	}

	如果 (size < tb->length) {
		printf("ERROR acpi buffer too small for %s\n", name);
		返回 -1;
	}

	如果 (read_func(addr, tb, size)) {
		printf("ERROR acpi cannot read %s\n", name);
		返回 -1;
	}

	如果 (acpi_check_csum(tb, tb->length)) {
		printf("ERROR acpi %s checksum does not match\n", name);
		返回 -1;
	}

	返回 tb->length;
}

phys_bytes acpi_get_table_base(const char * name)
{
	整数 i;

	为(i = 0; i < sdt_count; i++) {
		如果 (strncmp(name, sdt_trans[i].signature,
					ACPI_SDT_SIGNATURE_LEN) == 0)
			返回 (phys_bytes) rsdt.data[i];
	}

	返回 (phys_bytes) NULL;
}

size_t acpi_get_table_length(const char * name)
{
	整数 i;

	为(i = 0; i < sdt_count; i++) {
		如果 (strncmp(name, sdt_trans[i].signature,
					ACPI_SDT_SIGNATURE_LEN) == 0)
			返回 sdt_trans[i].length;
	}

	返回 0;
}

static void * acpi_madt_get_typed_item(struct acpi_madt_hdr * hdr,
					unsigned char type,
					unsigned idx)
{
	u8_t * t, * end;
	整数 i;

	t = (u8_t *) hdr + sizeof(struct acpi_madt_hdr);
	end = (u8_t *) hdr + hdr->hdr.length;

	i = 0;
	当(t < end) {
		如果 (type == ((struct acpi_madt_item_hdr *) t)->type) {
			如果 (i == idx)
				返回 t;
			否则
				i++;
		}
		t += ((struct acpi_madt_item_hdr *) t)->length;
	}

	返回 NULL;
}

#如果 0
static void * acpi_madt_get_item(struct acpi_madt_hdr * hdr,
				unsigned idx)
{
	u8_t * t, * end;
	整数 i;

	t = (u8_t *) hdr + sizeof(struct acpi_madt_hdr);
	end = (u8_t *) hdr + hdr->hdr.length;

	为(i = 0 ; i <= idx && t < end; i++) {
		如果 (i == idx)
			返回 t;
		t += ((struct acpi_madt_item_hdr *) t)->length;
	}

	返回 NULL;
}
#endif

static 整数 acpi_rsdp_test(void * buff)
{
	struct acpi_rsdp * rsdp = (struct acpi_rsdp *) buff;

	如果 (!platform_tbl_checksum_ok(buff, 20))
		返回 0;
	如果 (strncmp(rsdp->signature, "RSD PTR ", 8))
		返回 0;

	返回 1;
}

static 整数 get_acpi_rsdp(void)
{
	u16_t ebda;
	/*
	 * Read 40:0Eh - to find the starting address of the EBDA.
	 */
	acpi_phys_copy (0x40E, &ebda, sizeof(ebda));
	如果 (ebda) {
		ebda <<= 4;
		如果(platform_tbl_ptr(ebda, ebda + 0x400, 16, &acpi_rsdp,
					sizeof(acpi_rsdp), &machine.acpi_rsdp,
					acpi_rsdp_test))
			返回 1;
	} 

	/* try BIOS read only mem space */
	如果(platform_tbl_ptr(0xE0000, 0x100000, 16, &acpi_rsdp,
				sizeof(acpi_rsdp), &machine.acpi_rsdp,
				acpi_rsdp_test))
		返回 1;
	
	machine.acpi_rsdp = 0; /* RSDP cannot be found at this address therefore
				  it is a valid negative value */
	返回 0;
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
	如果 (fadt_header == NULL) {
		msg = "Could not load FACP";
		goto exit;
	}

	dsdt_header = (struct acpi_rsdt *)
		acpi_phys2vir((phys_bytes) fadt_header->dsdt);
	如果 (dsdt_header == NULL) {
		msg = "Could not load DSDT";
		goto exit;
	}

	pm1a_cnt_blk = fadt_header->pm1a_cnt_blk;
	pm1b_cnt_blk = fadt_header->pm1b_cnt_blk;

	ptr = start = (u8_t *) dsdt_header->data;
	end = start + dsdt_header->hdr.length - 4;

	/* See http://forum.osdev.org/viewtopic.php?t=16990 */
	/* for layout of \_S5 */
	当 (ptr < end && memcmp(ptr, "_S5_", 4) != 0)
		ptr++;

	msg = "Could not read S5 data. Use default SLP_TYPa and SLP_TYPb";
	如果 (ptr >= end || ptr == start)
		goto exit;

	/* validate AML structure */
	如果 (*(ptr + AMI_S5_PACKAGE_OP_OFFSET) != AMI_PACKAGE_OP_CODE)
		goto exit;

	如果 ((ptr < start + (-AMI_S5_NAME_OP_OFFSET_2) ||
		(*(ptr + AMI_S5_NAME_OP_OFFSET_2) != AMI_NAME_OP_CODE ||
		 *(ptr + AMI_S5_NAME_OP_OFFSET_2 + 1) != '\\')) &&
		*(ptr + AMI_S5_NAME_OP_OFFSET_1) != AMI_NAME_OP_CODE)
		goto exit;

	ptr += AMI_S5_PACKET_LENGTH_OFFSET;
	如果 (ptr >= end)
		goto exit;

	/* package length */
	ptr += ((*ptr & AMI_PACKAGE_LENGTH_ENCODING_BITS_MASK) >>
		AMI_PACKAGE_LENGTH_ENCODING_BITS_SHIFT) +
		AMI_MIN_PACKAGE_LENGTH + AMI_NUM_ELEMENTS_LENGTH;
	如果 (ptr >= end)
		goto exit;

	如果 (*ptr == AMI_BYTE_PREFIX_CODE)
		ptr++; /* skip byte prefix */

	slp_typa = (*ptr) << AMI_SLP_TYPA_SHIFT;

	ptr++; /* move to SLP_TYPb */
	如果 (*ptr == AMI_BYTE_PREFIX_CODE)
		ptr++; /* skip byte prefix */

	slp_typb = (*ptr) << AMI_SLP_TYPB_SHIFT;

	msg = "poweroff initialized";

exit:
	如果 (msg) {
		DEBUGBASIC(("acpi: %s\n", msg));
	}
}

void acpi_init(void)
{
	整数 s, i;
	read_func = acpi_phys_copy;

	如果 (!get_acpi_rsdp()) {
		printf("WARNING : Cannot configure ACPI\n");
		返回;
	}
	
	s = acpi_read_sdt_at(acpi_rsdp.rsdt_addr, (struct acpi_sdt_header *) &rsdt,
			sizeof(struct acpi_rsdt), ACPI_SDT_SIGNATURE(RSDT));

	sdt_count = (s - sizeof(struct acpi_sdt_header)) / sizeof(u32_t);

	为 (i = 0; i < sdt_count; i++) {
		struct acpi_sdt_header hdr;
		整数 j;
		如果 (read_func(rsdt.data[i], &hdr, sizeof(struct acpi_sdt_header))) {
			printf("ERROR acpi cannot read header at 0x%x\n",
								rsdt.data[i]);
			返回;
		}

		为 (j = 0 ; j < ACPI_SDT_SIGNATURE_LEN; j++)
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

	如果 (idx == 0) {
		madt_hdr = (struct acpi_madt_hdr *)
			acpi_phys2vir(acpi_get_table_base("APIC"));
		如果 (madt_hdr == NULL)
			返回 NULL;
	}

	ret = (struct acpi_madt_ioapic *)
		acpi_madt_get_typed_item(madt_hdr, ACPI_MADT_TYPE_IOAPIC, idx);
	如果 (ret)
		idx++;

	返回 ret;
}

struct acpi_madt_lapic * acpi_get_lapic_next(void)
{
	static unsigned idx = 0;
	static struct acpi_madt_hdr * madt_hdr;

	struct acpi_madt_lapic * ret;

	如果 (idx == 0) {
		madt_hdr = (struct acpi_madt_hdr *)
			acpi_phys2vir(acpi_get_table_base("APIC"));
		如果 (madt_hdr == NULL)
			返回 NULL;
	}

	为 (;;) {
		ret = (struct acpi_madt_lapic *)
			acpi_madt_get_typed_item(madt_hdr,
					ACPI_MADT_TYPE_LAPIC, idx);
		如果 (!ret)
			break;

		idx++;

		/* report only usable CPUs */
		如果 (ret->flags & 1)
			break;
	}

	返回 ret;
}

void __k_unpaged_acpi_poweroff(void)
{
	/* NO OP poweroff symbol*/
}

void acpi_poweroff(void)
{
	如果 (pm1a_cnt_blk == 0) {
		返回;
	}
	outw(pm1a_cnt_blk, slp_typa | SLP_EN_CODE);
	如果 (pm1b_cnt_blk != 0) {
		outw(pm1b_cnt_blk, slp_typb | SLP_EN_CODE);
	}
}
