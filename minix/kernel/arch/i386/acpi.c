
#அந்தர்பாவ <string.h>

#அந்தர்பாவ "acpi.h"
#அந்தர்பாவ "arch_proto.h"

வகைவரை முழு ((* acpi_read_t)(phys_bytes addr, வெறுமை * buff, size_t size));

கட்டமைப்பு acpi_rsdp acpi_rsdp;

நிலையான acpi_read_t read_func;

#define MAX_RSDT	35 /* ACPI defines 35 signatures */
#define SLP_EN_CODE	(1 << 13) /* ACPI SLP_EN_CODE code */
#define AMI_PACKAGE_OP_CODE (0x12)
#define AMI_NAME_OP_CODE (0x8)
#define AMI_BYTE_PREFIX_CODE (0xA)
#define AMI_PACKAGE_LENGTH_ENCODING_BITS_MASK (0xC0)
#define AMI_PACKAGE_LENGTH_ENCODING_BITS_SHIFT (6)
#define AMI_MIN_PACKAGE_LENGTH (1)
#define AMI_இலக்கம்_ELEMENTS_LENGTH (1)
#define AMI_SLP_TYPA_SHIFT (10)
#define AMI_SLP_TYPB_SHIFT (10)
#define AMI_S5_NAME_OP_OFFSET_1 (-1)
#define AMI_S5_NAME_OP_OFFSET_2 (-2)
#define AMI_S5_PACKAGE_OP_OFFSET (4)
#define AMI_S5_PACKET_LENGTH_OFFSET (5)

நிலையான கட்டமைப்பு acpi_rsdt {
	கட்டமைப்பு acpi_sdt_header	hdr;
	u32_t			data[MAX_RSDT];
} rsdt;
	
நிலையான கட்டமைப்பு {
	எழுத்து	signature [ACPI_SDT_SIGNATURE_LEN + 1];
	size_t	length;
} sdt_trans[MAX_RSDT];

நிலையான முழு sdt_மொத்தம்;
நிலையான u16_t pm1a_cnt_blk = 0;
நிலையான u16_t pm1b_cnt_blk = 0;
நிலையான u16_t slp_typa = 0;
நிலையான u16_t slp_typb = 0;

நிலையான முழு acpi_check_csum(கட்டமைப்பு acpi_sdt_header * tb, size_t size)
{
	u8_t total = 0;
	முழு i;
	க்கு (i = 0; i < size; i++)
		total += ((unsigned எழுத்து *)tb)[i];
	திரும்பு total == 0 ? 0 : -1;
}

நிலையான முழு acpi_check_signature(மாறிலி எழுத்து * orig, மாறிலி எழுத்து * match)
{
	திரும்பு strncmp(orig, match, ACPI_SDT_SIGNATURE_LEN);
}

நிலையான u32_t acpi_phys2vir(u32_t p)
{
	எனில்(!vm_running) {
		DEBUGEXTRA(("acpi: returning 0x%lx as vir addr\n", p));
		திரும்பு p;
	}
	panic("acpi: can't get virtual address of arbitrary physical address");
}

நிலையான முழு acpi_phys_copy(phys_bytes phys, வெறுமை *target, size_t len)
{
	எனில்(!vm_running) {
		memcpy(target, (வெறுமை *) phys, len);
		திரும்பு 0;
	}
	panic("can't acpi_phys_copy with vm");
}

நிலையான முழு acpi_read_sdt_at(phys_bytes addr,
				கட்டமைப்பு acpi_sdt_header * tb,
				size_t size,
				மாறிலி எழுத்து * name)
{
	கட்டமைப்பு acpi_sdt_header hdr;

	/* if NULL is supplied, we only return the size of the table */
	எனில் (tb == வெற்று) {
		எனில் (read_func(addr, &hdr, அளவு(கட்டமைப்பு acpi_sdt_header))) {
			printf("ERROR acpi cannot read %s header\n", name);
			திரும்பு -1;
		}

		திரும்பு hdr.length;
	}

	எனில் (read_func(addr, tb, அளவு(கட்டமைப்பு acpi_sdt_header))) {
		printf("ERROR acpi cannot read %s header\n", name);
		திரும்பு -1;
	}

	எனில் (acpi_check_signature(tb->signature, name)) {
		printf("ERROR acpi %s signature does not match\n", name);
		திரும்பு -1;
	}

	எனில் (size < tb->length) {
		printf("ERROR acpi buffer too small for %s\n", name);
		திரும்பு -1;
	}

	எனில் (read_func(addr, tb, size)) {
		printf("ERROR acpi cannot read %s\n", name);
		திரும்பு -1;
	}

	எனில் (acpi_check_csum(tb, tb->length)) {
		printf("ERROR acpi %s checksum does not match\n", name);
		திரும்பு -1;
	}

	திரும்பு tb->length;
}

phys_bytes acpi_get_table_base(மாறிலி எழுத்து * name)
{
	முழு i;

	க்கு(i = 0; i < sdt_மொத்தம்; i++) {
		எனில் (strncmp(name, sdt_trans[i].signature,
					ACPI_SDT_SIGNATURE_LEN) == 0)
			திரும்பு (phys_bytes) rsdt.data[i];
	}

	திரும்பு (phys_bytes) வெற்று;
}

size_t acpi_get_table_length(மாறிலி எழுத்து * name)
{
	முழு i;

	க்கு(i = 0; i < sdt_மொத்தம்; i++) {
		எனில் (strncmp(name, sdt_trans[i].signature,
					ACPI_SDT_SIGNATURE_LEN) == 0)
			திரும்பு sdt_trans[i].length;
	}

	திரும்பு 0;
}

நிலையான வெறுமை * acpi_madt_get_typed_item(கட்டமைப்பு acpi_madt_hdr * hdr,
					unsigned எழுத்து type,
					unsigned idx)
{
	u8_t * t, * end;
	முழு i;

	t = (u8_t *) hdr + அளவு(கட்டமைப்பு acpi_madt_hdr);
	end = (u8_t *) hdr + hdr->hdr.length;

	i = 0;
	போது(t < end) {
		எனில் (type == ((கட்டமைப்பு acpi_madt_item_hdr *) t)->type) {
			எனில் (i == idx)
				திரும்பு t;
			இல்லையெனில்
				i++;
		}
		t += ((கட்டமைப்பு acpi_madt_item_hdr *) t)->length;
	}

	திரும்பு வெற்று;
}

#எனில் 0
நிலையான வெறுமை * acpi_madt_get_item(கட்டமைப்பு acpi_madt_hdr * hdr,
				unsigned idx)
{
	u8_t * t, * end;
	முழு i;

	t = (u8_t *) hdr + அளவு(கட்டமைப்பு acpi_madt_hdr);
	end = (u8_t *) hdr + hdr->hdr.length;

	க்கு(i = 0 ; i <= idx && t < end; i++) {
		எனில் (i == idx)
			திரும்பு t;
		t += ((கட்டமைப்பு acpi_madt_item_hdr *) t)->length;
	}

	திரும்பு வெற்று;
}
#endif

நிலையான முழு acpi_rsdp_test(வெறுமை * buff)
{
	கட்டமைப்பு acpi_rsdp * rsdp = (கட்டமைப்பு acpi_rsdp *) buff;

	எனில் (!platform_tbl_checksum_ok(buff, 20))
		திரும்பு 0;
	எனில் (strncmp(rsdp->signature, "RSD PTR ", 8))
		திரும்பு 0;

	திரும்பு 1;
}

நிலையான முழு get_acpi_rsdp(வெறுமை)
{
	u16_t ebda;
	/*
	 * Read 40:0Eh - to find the starting address of the EBDA.
	 */
	acpi_phys_copy (0x40E, &ebda, அளவு(ebda));
	எனில் (ebda) {
		ebda <<= 4;
		எனில்(platform_tbl_ptr(ebda, ebda + 0x400, 16, &acpi_rsdp,
					அளவு(acpi_rsdp), &machine.acpi_rsdp,
					acpi_rsdp_test))
			திரும்பு 1;
	} 

	/* try BIOS read only mem space */
	எனில்(platform_tbl_ptr(0xE0000, 0x100000, 16, &acpi_rsdp,
				அளவு(acpi_rsdp), &machine.acpi_rsdp,
				acpi_rsdp_test))
		திரும்பு 1;
	
	machine.acpi_rsdp = 0; /* RSDP cannot be found at this address therefore
				  it is a valid negative value */
	திரும்பு 0;
}

நிலையான வெறுமை acpi_init_poweroff(வெறுமை)
{
	u8_t *ptr = வெற்று;
	u8_t *start = வெற்று;
	u8_t *end = வெற்று;
	கட்டமைப்பு acpi_fadt_header *fadt_header = வெற்று;
	கட்டமைப்பு acpi_rsdt * dsdt_header = வெற்று;
	எழுத்து *செய்தி = வெற்று;

	/* Everything used here existed since ACPI spec 1.0 */
	/* So we can safely use them */
	fadt_header = (கட்டமைப்பு acpi_fadt_header *)
		acpi_phys2vir(acpi_get_table_base("FACP"));
	எனில் (fadt_header == வெற்று) {
		செய்தி = "Could not load FACP";
		செல் exit;
	}

	dsdt_header = (கட்டமைப்பு acpi_rsdt *)
		acpi_phys2vir((phys_bytes) fadt_header->dsdt);
	எனில் (dsdt_header == வெற்று) {
		செய்தி = "Could not load DSDT";
		செல் exit;
	}

	pm1a_cnt_blk = fadt_header->pm1a_cnt_blk;
	pm1b_cnt_blk = fadt_header->pm1b_cnt_blk;

	ptr = start = (u8_t *) dsdt_header->data;
	end = start + dsdt_header->hdr.length - 4;

	/* See http://forum.osdev.org/viewtopic.php?t=16990 */
	/* for layout of \_S5 */
	போது (ptr < end && memcmp(ptr, "_S5_", 4) != 0)
		ptr++;

	செய்தி = "Could not read S5 data. Use default SLP_TYPa and SLP_TYPb";
	எனில் (ptr >= end || ptr == start)
		செல் exit;

	/* validate AML structure */
	எனில் (*(ptr + AMI_S5_PACKAGE_OP_OFFSET) != AMI_PACKAGE_OP_CODE)
		செல் exit;

	எனில் ((ptr < start + (-AMI_S5_NAME_OP_OFFSET_2) ||
		(*(ptr + AMI_S5_NAME_OP_OFFSET_2) != AMI_NAME_OP_CODE ||
		 *(ptr + AMI_S5_NAME_OP_OFFSET_2 + 1) != '\\')) &&
		*(ptr + AMI_S5_NAME_OP_OFFSET_1) != AMI_NAME_OP_CODE)
		செல் exit;

	ptr += AMI_S5_PACKET_LENGTH_OFFSET;
	எனில் (ptr >= end)
		செல் exit;

	/* package length */
	ptr += ((*ptr & AMI_PACKAGE_LENGTH_ENCODING_BITS_MASK) >>
		AMI_PACKAGE_LENGTH_ENCODING_BITS_SHIFT) +
		AMI_MIN_PACKAGE_LENGTH + AMI_இலக்கம்_ELEMENTS_LENGTH;
	எனில் (ptr >= end)
		செல் exit;

	எனில் (*ptr == AMI_BYTE_PREFIX_CODE)
		ptr++; /* skip byte prefix */

	slp_typa = (*ptr) << AMI_SLP_TYPA_SHIFT;

	ptr++; /* move to SLP_TYPb */
	எனில் (*ptr == AMI_BYTE_PREFIX_CODE)
		ptr++; /* skip byte prefix */

	slp_typb = (*ptr) << AMI_SLP_TYPB_SHIFT;

	செய்தி = "poweroff initialized";

exit:
	எனில் (செய்தி) {
		DEBUGBASIC(("acpi: %s\n", செய்தி));
	}
}

வெறுமை acpi_init(வெறுமை)
{
	முழு s, i;
	read_func = acpi_phys_copy;

	எனில் (!get_acpi_rsdp()) {
		printf("WARNING : Cannot configure ACPI\n");
		திரும்பு;
	}
	
	s = acpi_read_sdt_at(acpi_rsdp.rsdt_addr, (கட்டமைப்பு acpi_sdt_header *) &rsdt,
			அளவு(கட்டமைப்பு acpi_rsdt), ACPI_SDT_SIGNATURE(RSDT));

	sdt_மொத்தம் = (s - அளவு(கட்டமைப்பு acpi_sdt_header)) / அளவு(u32_t);

	க்கு (i = 0; i < sdt_மொத்தம்; i++) {
		கட்டமைப்பு acpi_sdt_header hdr;
		முழு j;
		எனில் (read_func(rsdt.data[i], &hdr, அளவு(கட்டமைப்பு acpi_sdt_header))) {
			printf("ERROR acpi cannot read header at 0x%x\n",
								rsdt.data[i]);
			திரும்பு;
		}

		க்கு (j = 0 ; j < ACPI_SDT_SIGNATURE_LEN; j++)
			sdt_trans[i].signature[j] = hdr.signature[j];
		sdt_trans[i].signature[ACPI_SDT_SIGNATURE_LEN] = '\0';
		sdt_trans[i].length = hdr.length;
	}

	acpi_init_poweroff();
}

கட்டமைப்பு acpi_madt_ioapic * acpi_get_ioapic_next(வெறுமை)
{
	நிலையான unsigned idx = 0;
	நிலையான கட்டமைப்பு acpi_madt_hdr * madt_hdr;

	கட்டமைப்பு acpi_madt_ioapic * ret;

	எனில் (idx == 0) {
		madt_hdr = (கட்டமைப்பு acpi_madt_hdr *)
			acpi_phys2vir(acpi_get_table_base("APIC"));
		எனில் (madt_hdr == வெற்று)
			திரும்பு வெற்று;
	}

	ret = (கட்டமைப்பு acpi_madt_ioapic *)
		acpi_madt_get_typed_item(madt_hdr, ACPI_MADT_TYPE_IOAPIC, idx);
	எனில் (ret)
		idx++;

	திரும்பு ret;
}

கட்டமைப்பு acpi_madt_lapic * acpi_get_lapic_next(வெறுமை)
{
	நிலையான unsigned idx = 0;
	நிலையான கட்டமைப்பு acpi_madt_hdr * madt_hdr;

	கட்டமைப்பு acpi_madt_lapic * ret;

	எனில் (idx == 0) {
		madt_hdr = (கட்டமைப்பு acpi_madt_hdr *)
			acpi_phys2vir(acpi_get_table_base("APIC"));
		எனில் (madt_hdr == வெற்று)
			திரும்பு வெற்று;
	}

	க்கு (;;) {
		ret = (கட்டமைப்பு acpi_madt_lapic *)
			acpi_madt_get_typed_item(madt_hdr,
					ACPI_MADT_TYPE_LAPIC, idx);
		எனில் (!ret)
			நிறுத்து;

		idx++;

		/* report only usable CPUs */
		எனில் (ret->flags & 1)
			நிறுத்து;
	}

	திரும்பு ret;
}

வெறுமை __k_unpaged_acpi_poweroff(வெறுமை)
{
	/* NO OP poweroff symbol*/
}

வெறுமை acpi_poweroff(வெறுமை)
{
	எனில் (pm1a_cnt_blk == 0) {
		திரும்பு;
	}
	outw(pm1a_cnt_blk, slp_typa | SLP_EN_CODE);
	எனில் (pm1b_cnt_blk != 0) {
		outw(pm1b_cnt_blk, slp_typb | SLP_EN_CODE);
	}
}
