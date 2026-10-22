
#অন্তর্ভাব <string.h>

#অন্তর্ভাব "acpi.h"
#অন্তর্ভাব "arch_proto.h"

প্রকারলেখ পূর্ণসংখ্যা ((* acpi_read_t)(phys_bytes addr, শূন্য * buff, size_t আকার_2));

কাঠামো acpi_rsdp acpi_rsdp;

স্থির acpi_read_t read_func;

#define MAX_RSDT	35 /* ACPI defines 35 signatures */
#define SLP_EN_CODE	(1 << 13) /* ACPI SLP_EN_CODE code */
#define AMI_PACKAGE_OP_CODE (0x12)
#define AMI_NAME_OP_CODE (0x8)
#define AMI_BYTE_PREFIX_CODE (0xA)
#define AMI_PACKAGE_দৈর্ঘ্য_ENCODING_BITS_MASK (0xC0)
#define AMI_PACKAGE_দৈর্ঘ্য_ENCODING_BITS_SHIFT (6)
#define AMI_MIN_PACKAGE_দৈর্ঘ্য (1)
#define AMI_অঙ্ক_ELEMENTS_দৈর্ঘ্য (1)
#define AMI_SLP_TYPA_SHIFT (10)
#define AMI_SLP_TYPB_SHIFT (10)
#define AMI_S5_NAME_OP_OFFSET_1 (-1)
#define AMI_S5_NAME_OP_OFFSET_2 (-2)
#define AMI_S5_PACKAGE_OP_OFFSET (4)
#define AMI_S5_PACKET_দৈর্ঘ্য_OFFSET (5)

স্থির কাঠামো acpi_rsdt {
	কাঠামো acpi_sdt_header	hdr;
	u32_t			তথ্য[MAX_RSDT];
} rsdt;
	
স্থির কাঠামো {
	অক্ষর	signature [ACPI_SDT_SIGNATURE_LEN + 1];
	size_t	দৈর্ঘ্য;
} sdt_trans[MAX_RSDT];

স্থির পূর্ণসংখ্যা sdt_গুনতি;
স্থির u16_t pm1a_cnt_blk = 0;
স্থির u16_t pm1b_cnt_blk = 0;
স্থির u16_t slp_typa = 0;
স্থির u16_t slp_typb = 0;

স্থির পূর্ণসংখ্যা acpi_check_csum(কাঠামো acpi_sdt_header * tb, size_t আকার_2)
{
	u8_t যোগ = 0;
	পূর্ণসংখ্যা i;
	জন্য (i = 0; i < আকার_2; i++)
		যোগ += ((unsigned অক্ষর *)tb)[i];
	ফেরত যোগ == 0 ? 0 : -1;
}

স্থির পূর্ণসংখ্যা acpi_check_signature(ধ্রুবক অক্ষর * orig, ধ্রুবক অক্ষর * match)
{
	ফেরত strncmp(orig, match, ACPI_SDT_SIGNATURE_LEN);
}

স্থির u32_t acpi_phys2vir(u32_t p)
{
	যদি(!vm_running) {
		DEBUGEXTRA(("acpi: returning 0x%lx as vir addr\n", p));
		ফেরত p;
	}
	panic("acpi: can't get virtual address of arbitrary physical address");
}

স্থির পূর্ণসংখ্যা acpi_phys_copy(phys_bytes phys, শূন্য *target, size_t len)
{
	যদি(!vm_running) {
		memcpy(target, (শূন্য *) phys, len);
		ফেরত 0;
	}
	panic("can't acpi_phys_copy with vm");
}

স্থির পূর্ণসংখ্যা acpi_read_sdt_at(phys_bytes addr,
				কাঠামো acpi_sdt_header * tb,
				size_t আকার_2,
				ধ্রুবক অক্ষর * name)
{
	কাঠামো acpi_sdt_header hdr;

	/* if NULL is supplied, we only return the size of the table */
	যদি (tb == ফাঁকা) {
		যদি (read_func(addr, &hdr, আকার(কাঠামো acpi_sdt_header))) {
			printf("ERROR acpi cannot read %s header\n", name);
			ফেরত -1;
		}

		ফেরত hdr.দৈর্ঘ্য;
	}

	যদি (read_func(addr, tb, আকার(কাঠামো acpi_sdt_header))) {
		printf("ERROR acpi cannot read %s header\n", name);
		ফেরত -1;
	}

	যদি (acpi_check_signature(tb->signature, name)) {
		printf("ERROR acpi %s signature does not match\n", name);
		ফেরত -1;
	}

	যদি (আকার_2 < tb->দৈর্ঘ্য) {
		printf("ERROR acpi buffer too small for %s\n", name);
		ফেরত -1;
	}

	যদি (read_func(addr, tb, আকার_2)) {
		printf("ERROR acpi cannot read %s\n", name);
		ফেরত -1;
	}

	যদি (acpi_check_csum(tb, tb->দৈর্ঘ্য)) {
		printf("ERROR acpi %s checksum does not match\n", name);
		ফেরত -1;
	}

	ফেরত tb->দৈর্ঘ্য;
}

phys_bytes acpi_get_table_base(ধ্রুবক অক্ষর * name)
{
	পূর্ণসংখ্যা i;

	জন্য(i = 0; i < sdt_গুনতি; i++) {
		যদি (strncmp(name, sdt_trans[i].signature,
					ACPI_SDT_SIGNATURE_LEN) == 0)
			ফেরত (phys_bytes) rsdt.তথ্য[i];
	}

	ফেরত (phys_bytes) ফাঁকা;
}

size_t acpi_get_table_দৈর্ঘ্য(ধ্রুবক অক্ষর * name)
{
	পূর্ণসংখ্যা i;

	জন্য(i = 0; i < sdt_গুনতি; i++) {
		যদি (strncmp(name, sdt_trans[i].signature,
					ACPI_SDT_SIGNATURE_LEN) == 0)
			ফেরত sdt_trans[i].দৈর্ঘ্য;
	}

	ফেরত 0;
}

স্থির শূন্য * acpi_madt_get_typed_item(কাঠামো acpi_madt_hdr * hdr,
					unsigned অক্ষর type,
					unsigned idx)
{
	u8_t * t, * end;
	পূর্ণসংখ্যা i;

	t = (u8_t *) hdr + আকার(কাঠামো acpi_madt_hdr);
	end = (u8_t *) hdr + hdr->hdr.দৈর্ঘ্য;

	i = 0;
	যখন(t < end) {
		যদি (type == ((কাঠামো acpi_madt_item_hdr *) t)->type) {
			যদি (i == idx)
				ফেরত t;
			অন্যথা
				i++;
		}
		t += ((কাঠামো acpi_madt_item_hdr *) t)->দৈর্ঘ্য;
	}

	ফেরত ফাঁকা;
}

#যদি 0
স্থির শূন্য * acpi_madt_get_item(কাঠামো acpi_madt_hdr * hdr,
				unsigned idx)
{
	u8_t * t, * end;
	পূর্ণসংখ্যা i;

	t = (u8_t *) hdr + আকার(কাঠামো acpi_madt_hdr);
	end = (u8_t *) hdr + hdr->hdr.দৈর্ঘ্য;

	জন্য(i = 0 ; i <= idx && t < end; i++) {
		যদি (i == idx)
			ফেরত t;
		t += ((কাঠামো acpi_madt_item_hdr *) t)->দৈর্ঘ্য;
	}

	ফেরত ফাঁকা;
}
#endif

স্থির পূর্ণসংখ্যা acpi_rsdp_test(শূন্য * buff)
{
	কাঠামো acpi_rsdp * rsdp = (কাঠামো acpi_rsdp *) buff;

	যদি (!platform_tbl_checksum_ok(buff, 20))
		ফেরত 0;
	যদি (strncmp(rsdp->signature, "RSD PTR ", 8))
		ফেরত 0;

	ফেরত 1;
}

স্থির পূর্ণসংখ্যা get_acpi_rsdp(শূন্য)
{
	u16_t ebda;
	/*
	 * Read 40:0Eh - to find the starting address of the EBDA.
	 */
	acpi_phys_copy (0x40E, &ebda, আকার(ebda));
	যদি (ebda) {
		ebda <<= 4;
		যদি(platform_tbl_ptr(ebda, ebda + 0x400, 16, &acpi_rsdp,
					আকার(acpi_rsdp), &machine.acpi_rsdp,
					acpi_rsdp_test))
			ফেরত 1;
	} 

	/* try BIOS read only mem space */
	যদি(platform_tbl_ptr(0xE0000, 0x100000, 16, &acpi_rsdp,
				আকার(acpi_rsdp), &machine.acpi_rsdp,
				acpi_rsdp_test))
		ফেরত 1;
	
	machine.acpi_rsdp = 0; /* RSDP cannot be found at this address therefore
				  it is a valid negative value */
	ফেরত 0;
}

স্থির শূন্য acpi_init_poweroff(শূন্য)
{
	u8_t *ptr = ফাঁকা;
	u8_t *start = ফাঁকা;
	u8_t *end = ফাঁকা;
	কাঠামো acpi_fadt_header *fadt_header = ফাঁকা;
	কাঠামো acpi_rsdt * dsdt_header = ফাঁকা;
	অক্ষর *বার্তা = ফাঁকা;

	/* Everything used here existed since ACPI spec 1.0 */
	/* So we can safely use them */
	fadt_header = (কাঠামো acpi_fadt_header *)
		acpi_phys2vir(acpi_get_table_base("FACP"));
	যদি (fadt_header == ফাঁকা) {
		বার্তা = "Could not load FACP";
		যাও exit;
	}

	dsdt_header = (কাঠামো acpi_rsdt *)
		acpi_phys2vir((phys_bytes) fadt_header->dsdt);
	যদি (dsdt_header == ফাঁকা) {
		বার্তা = "Could not load DSDT";
		যাও exit;
	}

	pm1a_cnt_blk = fadt_header->pm1a_cnt_blk;
	pm1b_cnt_blk = fadt_header->pm1b_cnt_blk;

	ptr = start = (u8_t *) dsdt_header->তথ্য;
	end = start + dsdt_header->hdr.দৈর্ঘ্য - 4;

	/* See http://forum.osdev.org/viewtopic.php?t=16990 */
	/* for layout of \_S5 */
	যখন (ptr < end && memcmp(ptr, "_S5_", 4) != 0)
		ptr++;

	বার্তা = "Could not read S5 data. Use default SLP_TYPa and SLP_TYPb";
	যদি (ptr >= end || ptr == start)
		যাও exit;

	/* validate AML structure */
	যদি (*(ptr + AMI_S5_PACKAGE_OP_OFFSET) != AMI_PACKAGE_OP_CODE)
		যাও exit;

	যদি ((ptr < start + (-AMI_S5_NAME_OP_OFFSET_2) ||
		(*(ptr + AMI_S5_NAME_OP_OFFSET_2) != AMI_NAME_OP_CODE ||
		 *(ptr + AMI_S5_NAME_OP_OFFSET_2 + 1) != '\\')) &&
		*(ptr + AMI_S5_NAME_OP_OFFSET_1) != AMI_NAME_OP_CODE)
		যাও exit;

	ptr += AMI_S5_PACKET_দৈর্ঘ্য_OFFSET;
	যদি (ptr >= end)
		যাও exit;

	/* package length */
	ptr += ((*ptr & AMI_PACKAGE_দৈর্ঘ্য_ENCODING_BITS_MASK) >>
		AMI_PACKAGE_দৈর্ঘ্য_ENCODING_BITS_SHIFT) +
		AMI_MIN_PACKAGE_দৈর্ঘ্য + AMI_অঙ্ক_ELEMENTS_দৈর্ঘ্য;
	যদি (ptr >= end)
		যাও exit;

	যদি (*ptr == AMI_BYTE_PREFIX_CODE)
		ptr++; /* skip byte prefix */

	slp_typa = (*ptr) << AMI_SLP_TYPA_SHIFT;

	ptr++; /* move to SLP_TYPb */
	যদি (*ptr == AMI_BYTE_PREFIX_CODE)
		ptr++; /* skip byte prefix */

	slp_typb = (*ptr) << AMI_SLP_TYPB_SHIFT;

	বার্তা = "poweroff initialized";

exit:
	যদি (বার্তা) {
		DEBUGBASIC(("acpi: %s\n", বার্তা));
	}
}

শূন্য acpi_init(শূন্য)
{
	পূর্ণসংখ্যা s, i;
	read_func = acpi_phys_copy;

	যদি (!get_acpi_rsdp()) {
		printf("WARNING : Cannot configure ACPI\n");
		ফেরত;
	}
	
	s = acpi_read_sdt_at(acpi_rsdp.rsdt_addr, (কাঠামো acpi_sdt_header *) &rsdt,
			আকার(কাঠামো acpi_rsdt), ACPI_SDT_SIGNATURE(RSDT));

	sdt_গুনতি = (s - আকার(কাঠামো acpi_sdt_header)) / আকার(u32_t);

	জন্য (i = 0; i < sdt_গুনতি; i++) {
		কাঠামো acpi_sdt_header hdr;
		পূর্ণসংখ্যা j;
		যদি (read_func(rsdt.তথ্য[i], &hdr, আকার(কাঠামো acpi_sdt_header))) {
			printf("ERROR acpi cannot read header at 0x%x\n",
								rsdt.তথ্য[i]);
			ফেরত;
		}

		জন্য (j = 0 ; j < ACPI_SDT_SIGNATURE_LEN; j++)
			sdt_trans[i].signature[j] = hdr.signature[j];
		sdt_trans[i].signature[ACPI_SDT_SIGNATURE_LEN] = '\0';
		sdt_trans[i].দৈর্ঘ্য = hdr.দৈর্ঘ্য;
	}

	acpi_init_poweroff();
}

কাঠামো acpi_madt_ioapic * acpi_get_ioapic_পরবর্তী(শূন্য)
{
	স্থির unsigned idx = 0;
	স্থির কাঠামো acpi_madt_hdr * madt_hdr;

	কাঠামো acpi_madt_ioapic * ret;

	যদি (idx == 0) {
		madt_hdr = (কাঠামো acpi_madt_hdr *)
			acpi_phys2vir(acpi_get_table_base("APIC"));
		যদি (madt_hdr == ফাঁকা)
			ফেরত ফাঁকা;
	}

	ret = (কাঠামো acpi_madt_ioapic *)
		acpi_madt_get_typed_item(madt_hdr, ACPI_MADT_TYPE_IOAPIC, idx);
	যদি (ret)
		idx++;

	ফেরত ret;
}

কাঠামো acpi_madt_lapic * acpi_get_lapic_পরবর্তী(শূন্য)
{
	স্থির unsigned idx = 0;
	স্থির কাঠামো acpi_madt_hdr * madt_hdr;

	কাঠামো acpi_madt_lapic * ret;

	যদি (idx == 0) {
		madt_hdr = (কাঠামো acpi_madt_hdr *)
			acpi_phys2vir(acpi_get_table_base("APIC"));
		যদি (madt_hdr == ফাঁকা)
			ফেরত ফাঁকা;
	}

	জন্য (;;) {
		ret = (কাঠামো acpi_madt_lapic *)
			acpi_madt_get_typed_item(madt_hdr,
					ACPI_MADT_TYPE_LAPIC, idx);
		যদি (!ret)
			বিরতি;

		idx++;

		/* report only usable CPUs */
		যদি (ret->flags & 1)
			বিরতি;
	}

	ফেরত ret;
}

শূন্য __k_unpaged_acpi_poweroff(শূন্য)
{
	/* NO OP poweroff symbol*/
}

শূন্য acpi_poweroff(শূন্য)
{
	যদি (pm1a_cnt_blk == 0) {
		ফেরত;
	}
	outw(pm1a_cnt_blk, slp_typa | SLP_EN_CODE);
	যদি (pm1b_cnt_blk != 0) {
		outw(pm1b_cnt_blk, slp_typb | SLP_EN_CODE);
	}
}
