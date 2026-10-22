
#شامل <string.h>

#شامل "acpi.h"
#شامل "arch_proto.h"

تعریفنوع صحیح ((* acpi_read_t)(phys_bytes addr, تهی * buff, size_t size));

ساختار acpi_rsdp acpi_rsdp;

ایستا acpi_read_t read_func;

#define MAX_RSDT	35 /* ACPI defines 35 signatures */
#define SLP_EN_CODE	(1 << 13) /* ACPI SLP_EN_CODE code */
#define AMI_PACKAGE_OP_CODE (0x12)
#define AMI_NAME_OP_CODE (0x8)
#define AMI_BYTE_PREFIX_CODE (0xA)
#define AMI_PACKAGE_LENGTH_ENCODING_BITS_MASK (0xC0)
#define AMI_PACKAGE_LENGTH_ENCODING_BITS_SHIFT (6)
#define AMI_MIN_PACKAGE_LENGTH (1)
#define AMI_رقم_ELEMENTS_LENGTH (1)
#define AMI_SLP_TYPA_SHIFT (10)
#define AMI_SLP_TYPB_SHIFT (10)
#define AMI_S5_NAME_OP_OFFSET_1 (-1)
#define AMI_S5_NAME_OP_OFFSET_2 (-2)
#define AMI_S5_PACKAGE_OP_OFFSET (4)
#define AMI_S5_PACKET_LENGTH_OFFSET (5)

ایستا ساختار acpi_rsdt {
	ساختار acpi_sdt_header	hdr;
	u32_t			data[MAX_RSDT];
} rsdt;
	
ایستا ساختار {
	نویسه	signature [ACPI_SDT_SIGNATURE_LEN + 1];
	size_t	length;
} sdt_trans[MAX_RSDT];

ایستا صحیح sdt_شمار;
ایستا u16_t pm1a_cnt_blk = 0;
ایستا u16_t pm1b_cnt_blk = 0;
ایستا u16_t slp_typa = 0;
ایستا u16_t slp_typb = 0;

ایستا صحیح acpi_check_csum(ساختار acpi_sdt_header * tb, size_t size)
{
	u8_t total = 0;
	صحیح i;
	برای (i = 0; i < size; i++)
		total += ((unsigned نویسه *)tb)[i];
	بازگشت total == 0 ? 0 : -1;
}

ایستا صحیح acpi_check_signature(ثابت نویسه * orig, ثابت نویسه * match)
{
	بازگشت strncmp(orig, match, ACPI_SDT_SIGNATURE_LEN);
}

ایستا u32_t acpi_phys2vir(u32_t p)
{
	اگر(!vm_running) {
		DEBUGEXTRA(("acpi: returning 0x%lx as vir addr\n", p));
		بازگشت p;
	}
	panic("acpi: can't get virtual address of arbitrary physical address");
}

ایستا صحیح acpi_phys_copy(phys_bytes phys, تهی *target, size_t len)
{
	اگر(!vm_running) {
		memcpy(target, (تهی *) phys, len);
		بازگشت 0;
	}
	panic("can't acpi_phys_copy with vm");
}

ایستا صحیح acpi_read_sdt_at(phys_bytes addr,
				ساختار acpi_sdt_header * tb,
				size_t size,
				ثابت نویسه * name)
{
	ساختار acpi_sdt_header hdr;

	/* if NULL is supplied, we only return the size of the table */
	اگر (tb == پوچ) {
		اگر (read_func(addr, &hdr, اندازه(ساختار acpi_sdt_header))) {
			printf("ERROR acpi cannot read %s header\n", name);
			بازگشت -1;
		}

		بازگشت hdr.length;
	}

	اگر (read_func(addr, tb, اندازه(ساختار acpi_sdt_header))) {
		printf("ERROR acpi cannot read %s header\n", name);
		بازگشت -1;
	}

	اگر (acpi_check_signature(tb->signature, name)) {
		printf("ERROR acpi %s signature does not match\n", name);
		بازگشت -1;
	}

	اگر (size < tb->length) {
		printf("ERROR acpi buffer too small for %s\n", name);
		بازگشت -1;
	}

	اگر (read_func(addr, tb, size)) {
		printf("ERROR acpi cannot read %s\n", name);
		بازگشت -1;
	}

	اگر (acpi_check_csum(tb, tb->length)) {
		printf("ERROR acpi %s checksum does not match\n", name);
		بازگشت -1;
	}

	بازگشت tb->length;
}

phys_bytes acpi_get_table_base(ثابت نویسه * name)
{
	صحیح i;

	برای(i = 0; i < sdt_شمار; i++) {
		اگر (strncmp(name, sdt_trans[i].signature,
					ACPI_SDT_SIGNATURE_LEN) == 0)
			بازگشت (phys_bytes) rsdt.data[i];
	}

	بازگشت (phys_bytes) پوچ;
}

size_t acpi_get_table_length(ثابت نویسه * name)
{
	صحیح i;

	برای(i = 0; i < sdt_شمار; i++) {
		اگر (strncmp(name, sdt_trans[i].signature,
					ACPI_SDT_SIGNATURE_LEN) == 0)
			بازگشت sdt_trans[i].length;
	}

	بازگشت 0;
}

ایستا تهی * acpi_madt_get_typed_item(ساختار acpi_madt_hdr * hdr,
					unsigned نویسه type,
					unsigned idx)
{
	u8_t * t, * end;
	صحیح i;

	t = (u8_t *) hdr + اندازه(ساختار acpi_madt_hdr);
	end = (u8_t *) hdr + hdr->hdr.length;

	i = 0;
	وقتی(t < end) {
		اگر (type == ((ساختار acpi_madt_item_hdr *) t)->type) {
			اگر (i == idx)
				بازگشت t;
			وگرنه
				i++;
		}
		t += ((ساختار acpi_madt_item_hdr *) t)->length;
	}

	بازگشت پوچ;
}

#اگر 0
ایستا تهی * acpi_madt_get_item(ساختار acpi_madt_hdr * hdr,
				unsigned idx)
{
	u8_t * t, * end;
	صحیح i;

	t = (u8_t *) hdr + اندازه(ساختار acpi_madt_hdr);
	end = (u8_t *) hdr + hdr->hdr.length;

	برای(i = 0 ; i <= idx && t < end; i++) {
		اگر (i == idx)
			بازگشت t;
		t += ((ساختار acpi_madt_item_hdr *) t)->length;
	}

	بازگشت پوچ;
}
#endif

ایستا صحیح acpi_rsdp_test(تهی * buff)
{
	ساختار acpi_rsdp * rsdp = (ساختار acpi_rsdp *) buff;

	اگر (!platform_tbl_checksum_ok(buff, 20))
		بازگشت 0;
	اگر (strncmp(rsdp->signature, "RSD PTR ", 8))
		بازگشت 0;

	بازگشت 1;
}

ایستا صحیح get_acpi_rsdp(تهی)
{
	u16_t ebda;
	/*
	 * Read 40:0Eh - to find the starting address of the EBDA.
	 */
	acpi_phys_copy (0x40E, &ebda, اندازه(ebda));
	اگر (ebda) {
		ebda <<= 4;
		اگر(platform_tbl_ptr(ebda, ebda + 0x400, 16, &acpi_rsdp,
					اندازه(acpi_rsdp), &machine.acpi_rsdp,
					acpi_rsdp_test))
			بازگشت 1;
	} 

	/* try BIOS read only mem space */
	اگر(platform_tbl_ptr(0xE0000, 0x100000, 16, &acpi_rsdp,
				اندازه(acpi_rsdp), &machine.acpi_rsdp,
				acpi_rsdp_test))
		بازگشت 1;
	
	machine.acpi_rsdp = 0; /* RSDP cannot be found at this address therefore
				  it is a valid negative value */
	بازگشت 0;
}

ایستا تهی acpi_init_poweroff(تهی)
{
	u8_t *ptr = پوچ;
	u8_t *start = پوچ;
	u8_t *end = پوچ;
	ساختار acpi_fadt_header *fadt_header = پوچ;
	ساختار acpi_rsdt * dsdt_header = پوچ;
	نویسه *پیام = پوچ;

	/* Everything used here existed since ACPI spec 1.0 */
	/* So we can safely use them */
	fadt_header = (ساختار acpi_fadt_header *)
		acpi_phys2vir(acpi_get_table_base("FACP"));
	اگر (fadt_header == پوچ) {
		پیام = "Could not load FACP";
		برو exit;
	}

	dsdt_header = (ساختار acpi_rsdt *)
		acpi_phys2vir((phys_bytes) fadt_header->dsdt);
	اگر (dsdt_header == پوچ) {
		پیام = "Could not load DSDT";
		برو exit;
	}

	pm1a_cnt_blk = fadt_header->pm1a_cnt_blk;
	pm1b_cnt_blk = fadt_header->pm1b_cnt_blk;

	ptr = start = (u8_t *) dsdt_header->data;
	end = start + dsdt_header->hdr.length - 4;

	/* See http://forum.osdev.org/viewtopic.php?t=16990 */
	/* for layout of \_S5 */
	وقتی (ptr < end && memcmp(ptr, "_S5_", 4) != 0)
		ptr++;

	پیام = "Could not read S5 data. Use default SLP_TYPa and SLP_TYPb";
	اگر (ptr >= end || ptr == start)
		برو exit;

	/* validate AML structure */
	اگر (*(ptr + AMI_S5_PACKAGE_OP_OFFSET) != AMI_PACKAGE_OP_CODE)
		برو exit;

	اگر ((ptr < start + (-AMI_S5_NAME_OP_OFFSET_2) ||
		(*(ptr + AMI_S5_NAME_OP_OFFSET_2) != AMI_NAME_OP_CODE ||
		 *(ptr + AMI_S5_NAME_OP_OFFSET_2 + 1) != '\\')) &&
		*(ptr + AMI_S5_NAME_OP_OFFSET_1) != AMI_NAME_OP_CODE)
		برو exit;

	ptr += AMI_S5_PACKET_LENGTH_OFFSET;
	اگر (ptr >= end)
		برو exit;

	/* package length */
	ptr += ((*ptr & AMI_PACKAGE_LENGTH_ENCODING_BITS_MASK) >>
		AMI_PACKAGE_LENGTH_ENCODING_BITS_SHIFT) +
		AMI_MIN_PACKAGE_LENGTH + AMI_رقم_ELEMENTS_LENGTH;
	اگر (ptr >= end)
		برو exit;

	اگر (*ptr == AMI_BYTE_PREFIX_CODE)
		ptr++; /* skip byte prefix */

	slp_typa = (*ptr) << AMI_SLP_TYPA_SHIFT;

	ptr++; /* move to SLP_TYPb */
	اگر (*ptr == AMI_BYTE_PREFIX_CODE)
		ptr++; /* skip byte prefix */

	slp_typb = (*ptr) << AMI_SLP_TYPB_SHIFT;

	پیام = "poweroff initialized";

exit:
	اگر (پیام) {
		DEBUGBASIC(("acpi: %s\n", پیام));
	}
}

تهی acpi_init(تهی)
{
	صحیح s, i;
	read_func = acpi_phys_copy;

	اگر (!get_acpi_rsdp()) {
		printf("WARNING : Cannot configure ACPI\n");
		بازگشت;
	}
	
	s = acpi_read_sdt_at(acpi_rsdp.rsdt_addr, (ساختار acpi_sdt_header *) &rsdt,
			اندازه(ساختار acpi_rsdt), ACPI_SDT_SIGNATURE(RSDT));

	sdt_شمار = (s - اندازه(ساختار acpi_sdt_header)) / اندازه(u32_t);

	برای (i = 0; i < sdt_شمار; i++) {
		ساختار acpi_sdt_header hdr;
		صحیح j;
		اگر (read_func(rsdt.data[i], &hdr, اندازه(ساختار acpi_sdt_header))) {
			printf("ERROR acpi cannot read header at 0x%x\n",
								rsdt.data[i]);
			بازگشت;
		}

		برای (j = 0 ; j < ACPI_SDT_SIGNATURE_LEN; j++)
			sdt_trans[i].signature[j] = hdr.signature[j];
		sdt_trans[i].signature[ACPI_SDT_SIGNATURE_LEN] = '\0';
		sdt_trans[i].length = hdr.length;
	}

	acpi_init_poweroff();
}

ساختار acpi_madt_ioapic * acpi_get_ioapic_next(تهی)
{
	ایستا unsigned idx = 0;
	ایستا ساختار acpi_madt_hdr * madt_hdr;

	ساختار acpi_madt_ioapic * ret;

	اگر (idx == 0) {
		madt_hdr = (ساختار acpi_madt_hdr *)
			acpi_phys2vir(acpi_get_table_base("APIC"));
		اگر (madt_hdr == پوچ)
			بازگشت پوچ;
	}

	ret = (ساختار acpi_madt_ioapic *)
		acpi_madt_get_typed_item(madt_hdr, ACPI_MADT_TYPE_IOAPIC, idx);
	اگر (ret)
		idx++;

	بازگشت ret;
}

ساختار acpi_madt_lapic * acpi_get_lapic_next(تهی)
{
	ایستا unsigned idx = 0;
	ایستا ساختار acpi_madt_hdr * madt_hdr;

	ساختار acpi_madt_lapic * ret;

	اگر (idx == 0) {
		madt_hdr = (ساختار acpi_madt_hdr *)
			acpi_phys2vir(acpi_get_table_base("APIC"));
		اگر (madt_hdr == پوچ)
			بازگشت پوچ;
	}

	برای (;;) {
		ret = (ساختار acpi_madt_lapic *)
			acpi_madt_get_typed_item(madt_hdr,
					ACPI_MADT_TYPE_LAPIC, idx);
		اگر (!ret)
			بشکن;

		idx++;

		/* report only usable CPUs */
		اگر (ret->flags & 1)
			بشکن;
	}

	بازگشت ret;
}

تهی __k_unpaged_acpi_poweroff(تهی)
{
	/* NO OP poweroff symbol*/
}

تهی acpi_poweroff(تهی)
{
	اگر (pm1a_cnt_blk == 0) {
		بازگشت;
	}
	outw(pm1a_cnt_blk, slp_typa | SLP_EN_CODE);
	اگر (pm1b_cnt_blk != 0) {
		outw(pm1b_cnt_blk, slp_typb | SLP_EN_CODE);
	}
}
