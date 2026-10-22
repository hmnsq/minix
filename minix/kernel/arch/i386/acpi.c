
#ضمن <string.h>

#ضمن "acpi.h"
#ضمن "arch_proto.h"

تعريفنوع صحيح ((* acpi_read_t)(phys_bytes addr, فراغ * buff, size_t size));

بنية acpi_rsdp acpi_rsdp;

ساكن acpi_read_t read_func;

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

ساكن بنية acpi_rsdt {
	بنية acpi_sdt_header	hdr;
	u32_t			data[MAX_RSDT];
} rsdt;
	
ساكن بنية {
	حرف	signature [ACPI_SDT_SIGNATURE_LEN + 1];
	size_t	length;
} sdt_trans[MAX_RSDT];

ساكن صحيح sdt_count;
ساكن u16_t pm1a_cnt_blk = 0;
ساكن u16_t pm1b_cnt_blk = 0;
ساكن u16_t slp_typa = 0;
ساكن u16_t slp_typb = 0;

ساكن صحيح acpi_check_csum(بنية acpi_sdt_header * tb, size_t size)
{
	u8_t total = 0;
	صحيح i;
	لكل (i = 0; i < size; i++)
		total += ((unsigned حرف *)tb)[i];
	ܦܢܐ total == 0 ? 0 : -1;
}

ساكن صحيح acpi_check_signature(ثابت حرف * orig, ثابت حرف * match)
{
	ܦܢܐ strncmp(orig, match, ACPI_SDT_SIGNATURE_LEN);
}

ساكن u32_t acpi_phys2vir(u32_t p)
{
	ܐܢ(!vm_running) {
		DEBUGEXTRA(("acpi: returning 0x%lx as vir addr\n", p));
		ܦܢܐ p;
	}
	panic("acpi: can't get virtual address of arbitrary physical address");
}

ساكن صحيح acpi_phys_copy(phys_bytes phys, فراغ *target, size_t len)
{
	ܐܢ(!vm_running) {
		memcpy(target, (فراغ *) phys, len);
		ܦܢܐ 0;
	}
	panic("can't acpi_phys_copy with vm");
}

ساكن صحيح acpi_read_sdt_at(phys_bytes addr,
				بنية acpi_sdt_header * tb,
				size_t size,
				ثابت حرف * name)
{
	بنية acpi_sdt_header hdr;

	/* if NULL is supplied, we only return the size of the table */
	ܐܢ (tb == عدم) {
		ܐܢ (read_func(addr, &hdr, حجم(بنية acpi_sdt_header))) {
			printf("ERROR acpi cannot read %s header\n", name);
			ܦܢܐ -1;
		}

		ܦܢܐ hdr.length;
	}

	ܐܢ (read_func(addr, tb, حجم(بنية acpi_sdt_header))) {
		printf("ERROR acpi cannot read %s header\n", name);
		ܦܢܐ -1;
	}

	ܐܢ (acpi_check_signature(tb->signature, name)) {
		printf("ERROR acpi %s signature does not match\n", name);
		ܦܢܐ -1;
	}

	ܐܢ (size < tb->length) {
		printf("ERROR acpi buffer too small for %s\n", name);
		ܦܢܐ -1;
	}

	ܐܢ (read_func(addr, tb, size)) {
		printf("ERROR acpi cannot read %s\n", name);
		ܦܢܐ -1;
	}

	ܐܢ (acpi_check_csum(tb, tb->length)) {
		printf("ERROR acpi %s checksum does not match\n", name);
		ܦܢܐ -1;
	}

	ܦܢܐ tb->length;
}

phys_bytes acpi_get_table_base(ثابت حرف * name)
{
	صحيح i;

	لكل(i = 0; i < sdt_count; i++) {
		ܐܢ (strncmp(name, sdt_trans[i].signature,
					ACPI_SDT_SIGNATURE_LEN) == 0)
			ܦܢܐ (phys_bytes) rsdt.data[i];
	}

	ܦܢܐ (phys_bytes) عدم;
}

size_t acpi_get_table_length(ثابت حرف * name)
{
	صحيح i;

	لكل(i = 0; i < sdt_count; i++) {
		ܐܢ (strncmp(name, sdt_trans[i].signature,
					ACPI_SDT_SIGNATURE_LEN) == 0)
			ܦܢܐ sdt_trans[i].length;
	}

	ܦܢܐ 0;
}

ساكن فراغ * acpi_madt_get_typed_item(بنية acpi_madt_hdr * hdr,
					unsigned حرف type,
					unsigned idx)
{
	u8_t * t, * end;
	صحيح i;

	t = (u8_t *) hdr + حجم(بنية acpi_madt_hdr);
	end = (u8_t *) hdr + hdr->hdr.length;

	i = 0;
	ܟܡܐ(t < end) {
		ܐܢ (type == ((بنية acpi_madt_item_hdr *) t)->type) {
			ܐܢ (i == idx)
				ܦܢܐ t;
			ܐܚܪܢܐ
				i++;
		}
		t += ((بنية acpi_madt_item_hdr *) t)->length;
	}

	ܦܢܐ عدم;
}

#ܐܢ 0
ساكن فراغ * acpi_madt_get_item(بنية acpi_madt_hdr * hdr,
				unsigned idx)
{
	u8_t * t, * end;
	صحيح i;

	t = (u8_t *) hdr + حجم(بنية acpi_madt_hdr);
	end = (u8_t *) hdr + hdr->hdr.length;

	لكل(i = 0 ; i <= idx && t < end; i++) {
		ܐܢ (i == idx)
			ܦܢܐ t;
		t += ((بنية acpi_madt_item_hdr *) t)->length;
	}

	ܦܢܐ عدم;
}
#endif

ساكن صحيح acpi_rsdp_test(فراغ * buff)
{
	بنية acpi_rsdp * rsdp = (بنية acpi_rsdp *) buff;

	ܐܢ (!platform_tbl_checksum_ok(buff, 20))
		ܦܢܐ 0;
	ܐܢ (strncmp(rsdp->signature, "RSD PTR ", 8))
		ܦܢܐ 0;

	ܦܢܐ 1;
}

ساكن صحيح get_acpi_rsdp(فراغ)
{
	u16_t ebda;
	/*
	 * Read 40:0Eh - to find the starting address of the EBDA.
	 */
	acpi_phys_copy (0x40E, &ebda, حجم(ebda));
	ܐܢ (ebda) {
		ebda <<= 4;
		ܐܢ(platform_tbl_ptr(ebda, ebda + 0x400, 16, &acpi_rsdp,
					حجم(acpi_rsdp), &machine.acpi_rsdp,
					acpi_rsdp_test))
			ܦܢܐ 1;
	} 

	/* try BIOS read only mem space */
	ܐܢ(platform_tbl_ptr(0xE0000, 0x100000, 16, &acpi_rsdp,
				حجم(acpi_rsdp), &machine.acpi_rsdp,
				acpi_rsdp_test))
		ܦܢܐ 1;
	
	machine.acpi_rsdp = 0; /* RSDP cannot be found at this address therefore
				  it is a valid negative value */
	ܦܢܐ 0;
}

ساكن فراغ acpi_init_poweroff(فراغ)
{
	u8_t *ptr = عدم;
	u8_t *start = عدم;
	u8_t *end = عدم;
	بنية acpi_fadt_header *fadt_header = عدم;
	بنية acpi_rsdt * dsdt_header = عدم;
	حرف *msg = عدم;

	/* Everything used here existed since ACPI spec 1.0 */
	/* So we can safely use them */
	fadt_header = (بنية acpi_fadt_header *)
		acpi_phys2vir(acpi_get_table_base("FACP"));
	ܐܢ (fadt_header == عدم) {
		msg = "Could not load FACP";
		اذهب exit;
	}

	dsdt_header = (بنية acpi_rsdt *)
		acpi_phys2vir((phys_bytes) fadt_header->dsdt);
	ܐܢ (dsdt_header == عدم) {
		msg = "Could not load DSDT";
		اذهب exit;
	}

	pm1a_cnt_blk = fadt_header->pm1a_cnt_blk;
	pm1b_cnt_blk = fadt_header->pm1b_cnt_blk;

	ptr = start = (u8_t *) dsdt_header->data;
	end = start + dsdt_header->hdr.length - 4;

	/* See http://forum.osdev.org/viewtopic.php?t=16990 */
	/* for layout of \_S5 */
	ܟܡܐ (ptr < end && memcmp(ptr, "_S5_", 4) != 0)
		ptr++;

	msg = "Could not read S5 data. Use default SLP_TYPa and SLP_TYPb";
	ܐܢ (ptr >= end || ptr == start)
		اذهب exit;

	/* validate AML structure */
	ܐܢ (*(ptr + AMI_S5_PACKAGE_OP_OFFSET) != AMI_PACKAGE_OP_CODE)
		اذهب exit;

	ܐܢ ((ptr < start + (-AMI_S5_NAME_OP_OFFSET_2) ||
		(*(ptr + AMI_S5_NAME_OP_OFFSET_2) != AMI_NAME_OP_CODE ||
		 *(ptr + AMI_S5_NAME_OP_OFFSET_2 + 1) != '\\')) &&
		*(ptr + AMI_S5_NAME_OP_OFFSET_1) != AMI_NAME_OP_CODE)
		اذهب exit;

	ptr += AMI_S5_PACKET_LENGTH_OFFSET;
	ܐܢ (ptr >= end)
		اذهب exit;

	/* package length */
	ptr += ((*ptr & AMI_PACKAGE_LENGTH_ENCODING_BITS_MASK) >>
		AMI_PACKAGE_LENGTH_ENCODING_BITS_SHIFT) +
		AMI_MIN_PACKAGE_LENGTH + AMI_NUM_ELEMENTS_LENGTH;
	ܐܢ (ptr >= end)
		اذهب exit;

	ܐܢ (*ptr == AMI_BYTE_PREFIX_CODE)
		ptr++; /* skip byte prefix */

	slp_typa = (*ptr) << AMI_SLP_TYPA_SHIFT;

	ptr++; /* move to SLP_TYPb */
	ܐܢ (*ptr == AMI_BYTE_PREFIX_CODE)
		ptr++; /* skip byte prefix */

	slp_typb = (*ptr) << AMI_SLP_TYPB_SHIFT;

	msg = "poweroff initialized";

exit:
	ܐܢ (msg) {
		DEBUGBASIC(("acpi: %s\n", msg));
	}
}

فراغ acpi_init(فراغ)
{
	صحيح s, i;
	read_func = acpi_phys_copy;

	ܐܢ (!get_acpi_rsdp()) {
		printf("WARNING : Cannot configure ACPI\n");
		ܦܢܐ;
	}
	
	s = acpi_read_sdt_at(acpi_rsdp.rsdt_addr, (بنية acpi_sdt_header *) &rsdt,
			حجم(بنية acpi_rsdt), ACPI_SDT_SIGNATURE(RSDT));

	sdt_count = (s - حجم(بنية acpi_sdt_header)) / حجم(u32_t);

	لكل (i = 0; i < sdt_count; i++) {
		بنية acpi_sdt_header hdr;
		صحيح j;
		ܐܢ (read_func(rsdt.data[i], &hdr, حجم(بنية acpi_sdt_header))) {
			printf("ERROR acpi cannot read header at 0x%x\n",
								rsdt.data[i]);
			ܦܢܐ;
		}

		لكل (j = 0 ; j < ACPI_SDT_SIGNATURE_LEN; j++)
			sdt_trans[i].signature[j] = hdr.signature[j];
		sdt_trans[i].signature[ACPI_SDT_SIGNATURE_LEN] = '\0';
		sdt_trans[i].length = hdr.length;
	}

	acpi_init_poweroff();
}

بنية acpi_madt_ioapic * acpi_get_ioapic_next(فراغ)
{
	ساكن unsigned idx = 0;
	ساكن بنية acpi_madt_hdr * madt_hdr;

	بنية acpi_madt_ioapic * ret;

	ܐܢ (idx == 0) {
		madt_hdr = (بنية acpi_madt_hdr *)
			acpi_phys2vir(acpi_get_table_base("APIC"));
		ܐܢ (madt_hdr == عدم)
			ܦܢܐ عدم;
	}

	ret = (بنية acpi_madt_ioapic *)
		acpi_madt_get_typed_item(madt_hdr, ACPI_MADT_TYPE_IOAPIC, idx);
	ܐܢ (ret)
		idx++;

	ܦܢܐ ret;
}

بنية acpi_madt_lapic * acpi_get_lapic_next(فراغ)
{
	ساكن unsigned idx = 0;
	ساكن بنية acpi_madt_hdr * madt_hdr;

	بنية acpi_madt_lapic * ret;

	ܐܢ (idx == 0) {
		madt_hdr = (بنية acpi_madt_hdr *)
			acpi_phys2vir(acpi_get_table_base("APIC"));
		ܐܢ (madt_hdr == عدم)
			ܦܢܐ عدم;
	}

	لكل (;;) {
		ret = (بنية acpi_madt_lapic *)
			acpi_madt_get_typed_item(madt_hdr,
					ACPI_MADT_TYPE_LAPIC, idx);
		ܐܢ (!ret)
			اكسر;

		idx++;

		/* report only usable CPUs */
		ܐܢ (ret->flags & 1)
			اكسر;
	}

	ܦܢܐ ret;
}

فراغ __k_unpaged_acpi_poweroff(فراغ)
{
	/* NO OP poweroff symbol*/
}

فراغ acpi_poweroff(فراغ)
{
	ܐܢ (pm1a_cnt_blk == 0) {
		ܦܢܐ;
	}
	outw(pm1a_cnt_blk, slp_typa | SLP_EN_CODE);
	ܐܢ (pm1b_cnt_blk != 0) {
		outw(pm1b_cnt_blk, slp_typb | SLP_EN_CODE);
	}
}
