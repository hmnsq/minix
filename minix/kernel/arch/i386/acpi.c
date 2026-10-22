
#অন্তর্ভাৱ <string.h>

#অন্তর্ভাৱ "acpi.h"
#অন্তর্ভাৱ "arch_proto.h"

প্রকারনির্দেশ পূর্ণাঙ্ক ((* acpi_read_t)(phys_bytes addr, শূন্য * buff, size_t size));

সংরচনা acpi_rsdp acpi_rsdp;

স্থির acpi_read_t read_func;

#define MAX_RSDT	35 /* ACPI defines 35 signatures */
#define SLP_EN_CODE	(1 << 13) /* ACPI SLP_EN_CODE code */
#define AMI_PACKAGE_OP_CODE (0x12)
#define AMI_NAME_OP_CODE (0x8)
#define AMI_BYTE_PREFIX_CODE (0xA)
#define AMI_PACKAGE_LENGTH_ENCODING_BITS_MASK (0xC0)
#define AMI_PACKAGE_LENGTH_ENCODING_BITS_SHIFT (6)
#define AMI_MIN_PACKAGE_LENGTH (1)
#define AMI_অঙ্ক_ELEMENTS_LENGTH (1)
#define AMI_SLP_TYPA_SHIFT (10)
#define AMI_SLP_TYPB_SHIFT (10)
#define AMI_S5_NAME_OP_OFFSET_1 (-1)
#define AMI_S5_NAME_OP_OFFSET_2 (-2)
#define AMI_S5_PACKAGE_OP_OFFSET (4)
#define AMI_S5_PACKET_LENGTH_OFFSET (5)

স্থির সংরচনা acpi_rsdt {
	সংরচনা acpi_sdt_header	hdr;
	u32_t			data[MAX_RSDT];
} rsdt;
	
স্থির সংরচনা {
	ৱর্ণ	signature [ACPI_SDT_SIGNATURE_LEN + 1];
	size_t	length;
} sdt_trans[MAX_RSDT];

স্থির পূর্ণাঙ্ক sdt_গুনতি;
স্থির u16_t pm1a_cnt_blk = 0;
স্থির u16_t pm1b_cnt_blk = 0;
স্থির u16_t slp_typa = 0;
স্থির u16_t slp_typb = 0;

স্থির পূর্ণাঙ্ক acpi_check_csum(সংরচনা acpi_sdt_header * tb, size_t size)
{
	u8_t total = 0;
	পূর্ণাঙ্ক i;
	বাবে (i = 0; i < size; i++)
		total += ((unsigned ৱর্ণ *)tb)[i];
	ঘূৰাই total == 0 ? 0 : -1;
}

স্থির পূর্ণাঙ্ক acpi_check_signature(ধ্রুৱ ৱর্ণ * orig, ধ্রুৱ ৱর্ণ * match)
{
	ঘূৰাই strncmp(orig, match, ACPI_SDT_SIGNATURE_LEN);
}

স্থির u32_t acpi_phys2vir(u32_t p)
{
	যদি(!vm_running) {
		DEBUGEXTRA(("acpi: returning 0x%lx as vir addr\n", p));
		ঘূৰাই p;
	}
	panic("acpi: can't get virtual address of arbitrary physical address");
}

স্থির পূর্ণাঙ্ক acpi_phys_copy(phys_bytes phys, শূন্য *target, size_t len)
{
	যদি(!vm_running) {
		memcpy(target, (শূন্য *) phys, len);
		ঘূৰাই 0;
	}
	panic("can't acpi_phys_copy with vm");
}

স্থির পূর্ণাঙ্ক acpi_read_sdt_at(phys_bytes addr,
				সংরচনা acpi_sdt_header * tb,
				size_t size,
				ধ্রুৱ ৱর্ণ * name)
{
	সংরচনা acpi_sdt_header hdr;

	/* if NULL is supplied, we only return the size of the table */
	যদি (tb == রিক্ত) {
		যদি (read_func(addr, &hdr, পরিমাণ(সংরচনা acpi_sdt_header))) {
			printf("ERROR acpi cannot read %s header\n", name);
			ঘূৰাই -1;
		}

		ঘূৰাই hdr.length;
	}

	যদি (read_func(addr, tb, পরিমাণ(সংরচনা acpi_sdt_header))) {
		printf("ERROR acpi cannot read %s header\n", name);
		ঘূৰাই -1;
	}

	যদি (acpi_check_signature(tb->signature, name)) {
		printf("ERROR acpi %s signature does not match\n", name);
		ঘূৰাই -1;
	}

	যদি (size < tb->length) {
		printf("ERROR acpi buffer too small for %s\n", name);
		ঘূৰাই -1;
	}

	যদি (read_func(addr, tb, size)) {
		printf("ERROR acpi cannot read %s\n", name);
		ঘূৰাই -1;
	}

	যদি (acpi_check_csum(tb, tb->length)) {
		printf("ERROR acpi %s checksum does not match\n", name);
		ঘূৰাই -1;
	}

	ঘূৰাই tb->length;
}

phys_bytes acpi_get_table_base(ধ্রুৱ ৱর্ণ * name)
{
	পূর্ণাঙ্ক i;

	বাবে(i = 0; i < sdt_গুনতি; i++) {
		যদি (strncmp(name, sdt_trans[i].signature,
					ACPI_SDT_SIGNATURE_LEN) == 0)
			ঘূৰাই (phys_bytes) rsdt.data[i];
	}

	ঘূৰাই (phys_bytes) রিক্ত;
}

size_t acpi_get_table_length(ধ্রুৱ ৱর্ণ * name)
{
	পূর্ণাঙ্ক i;

	বাবে(i = 0; i < sdt_গুনতি; i++) {
		যদি (strncmp(name, sdt_trans[i].signature,
					ACPI_SDT_SIGNATURE_LEN) == 0)
			ঘূৰাই sdt_trans[i].length;
	}

	ঘূৰাই 0;
}

স্থির শূন্য * acpi_madt_get_typed_item(সংরচনা acpi_madt_hdr * hdr,
					unsigned ৱর্ণ type,
					unsigned idx)
{
	u8_t * t, * end;
	পূর্ণাঙ্ক i;

	t = (u8_t *) hdr + পরিমাণ(সংরচনা acpi_madt_hdr);
	end = (u8_t *) hdr + hdr->hdr.length;

	i = 0;
	যেতিয়ালৈ(t < end) {
		যদি (type == ((সংরচনা acpi_madt_item_hdr *) t)->type) {
			যদি (i == idx)
				ঘূৰাই t;
			অন্যথা
				i++;
		}
		t += ((সংরচনা acpi_madt_item_hdr *) t)->length;
	}

	ঘূৰাই রিক্ত;
}

#যদি 0
স্থির শূন্য * acpi_madt_get_item(সংরচনা acpi_madt_hdr * hdr,
				unsigned idx)
{
	u8_t * t, * end;
	পূর্ণাঙ্ক i;

	t = (u8_t *) hdr + পরিমাণ(সংরচনা acpi_madt_hdr);
	end = (u8_t *) hdr + hdr->hdr.length;

	বাবে(i = 0 ; i <= idx && t < end; i++) {
		যদি (i == idx)
			ঘূৰাই t;
		t += ((সংরচনা acpi_madt_item_hdr *) t)->length;
	}

	ঘূৰাই রিক্ত;
}
#endif

স্থির পূর্ণাঙ্ক acpi_rsdp_test(শূন্য * buff)
{
	সংরচনা acpi_rsdp * rsdp = (সংরচনা acpi_rsdp *) buff;

	যদি (!platform_tbl_checksum_ok(buff, 20))
		ঘূৰাই 0;
	যদি (strncmp(rsdp->signature, "RSD PTR ", 8))
		ঘূৰাই 0;

	ঘূৰাই 1;
}

স্থির পূর্ণাঙ্ক get_acpi_rsdp(শূন্য)
{
	u16_t ebda;
	/*
	 * Read 40:0Eh - to find the starting address of the EBDA.
	 */
	acpi_phys_copy (0x40E, &ebda, পরিমাণ(ebda));
	যদি (ebda) {
		ebda <<= 4;
		যদি(platform_tbl_ptr(ebda, ebda + 0x400, 16, &acpi_rsdp,
					পরিমাণ(acpi_rsdp), &machine.acpi_rsdp,
					acpi_rsdp_test))
			ঘূৰাই 1;
	} 

	/* try BIOS read only mem space */
	যদি(platform_tbl_ptr(0xE0000, 0x100000, 16, &acpi_rsdp,
				পরিমাণ(acpi_rsdp), &machine.acpi_rsdp,
				acpi_rsdp_test))
		ঘূৰাই 1;
	
	machine.acpi_rsdp = 0; /* RSDP cannot be found at this address therefore
				  it is a valid negative value */
	ঘূৰাই 0;
}

স্থির শূন্য acpi_init_poweroff(শূন্য)
{
	u8_t *ptr = রিক্ত;
	u8_t *start = রিক্ত;
	u8_t *end = রিক্ত;
	সংরচনা acpi_fadt_header *fadt_header = রিক্ত;
	সংরচনা acpi_rsdt * dsdt_header = রিক্ত;
	ৱর্ণ *বাৰ্তা = রিক্ত;

	/* Everything used here existed since ACPI spec 1.0 */
	/* So we can safely use them */
	fadt_header = (সংরচনা acpi_fadt_header *)
		acpi_phys2vir(acpi_get_table_base("FACP"));
	যদি (fadt_header == রিক্ত) {
		বাৰ্তা = "Could not load FACP";
		গচ্ছ exit;
	}

	dsdt_header = (সংরচনা acpi_rsdt *)
		acpi_phys2vir((phys_bytes) fadt_header->dsdt);
	যদি (dsdt_header == রিক্ত) {
		বাৰ্তা = "Could not load DSDT";
		গচ্ছ exit;
	}

	pm1a_cnt_blk = fadt_header->pm1a_cnt_blk;
	pm1b_cnt_blk = fadt_header->pm1b_cnt_blk;

	ptr = start = (u8_t *) dsdt_header->data;
	end = start + dsdt_header->hdr.length - 4;

	/* See http://forum.osdev.org/viewtopic.php?t=16990 */
	/* for layout of \_S5 */
	যেতিয়ালৈ (ptr < end && memcmp(ptr, "_S5_", 4) != 0)
		ptr++;

	বাৰ্তা = "Could not read S5 data. Use default SLP_TYPa and SLP_TYPb";
	যদি (ptr >= end || ptr == start)
		গচ্ছ exit;

	/* validate AML structure */
	যদি (*(ptr + AMI_S5_PACKAGE_OP_OFFSET) != AMI_PACKAGE_OP_CODE)
		গচ্ছ exit;

	যদি ((ptr < start + (-AMI_S5_NAME_OP_OFFSET_2) ||
		(*(ptr + AMI_S5_NAME_OP_OFFSET_2) != AMI_NAME_OP_CODE ||
		 *(ptr + AMI_S5_NAME_OP_OFFSET_2 + 1) != '\\')) &&
		*(ptr + AMI_S5_NAME_OP_OFFSET_1) != AMI_NAME_OP_CODE)
		গচ্ছ exit;

	ptr += AMI_S5_PACKET_LENGTH_OFFSET;
	যদি (ptr >= end)
		গচ্ছ exit;

	/* package length */
	ptr += ((*ptr & AMI_PACKAGE_LENGTH_ENCODING_BITS_MASK) >>
		AMI_PACKAGE_LENGTH_ENCODING_BITS_SHIFT) +
		AMI_MIN_PACKAGE_LENGTH + AMI_অঙ্ক_ELEMENTS_LENGTH;
	যদি (ptr >= end)
		গচ্ছ exit;

	যদি (*ptr == AMI_BYTE_PREFIX_CODE)
		ptr++; /* skip byte prefix */

	slp_typa = (*ptr) << AMI_SLP_TYPA_SHIFT;

	ptr++; /* move to SLP_TYPb */
	যদি (*ptr == AMI_BYTE_PREFIX_CODE)
		ptr++; /* skip byte prefix */

	slp_typb = (*ptr) << AMI_SLP_TYPB_SHIFT;

	বাৰ্তা = "poweroff initialized";

exit:
	যদি (বাৰ্তা) {
		DEBUGBASIC(("acpi: %s\n", বাৰ্তা));
	}
}

শূন্য acpi_init(শূন্য)
{
	পূর্ণাঙ্ক s, i;
	read_func = acpi_phys_copy;

	যদি (!get_acpi_rsdp()) {
		printf("WARNING : Cannot configure ACPI\n");
		ঘূৰাই;
	}
	
	s = acpi_read_sdt_at(acpi_rsdp.rsdt_addr, (সংরচনা acpi_sdt_header *) &rsdt,
			পরিমাণ(সংরচনা acpi_rsdt), ACPI_SDT_SIGNATURE(RSDT));

	sdt_গুনতি = (s - পরিমাণ(সংরচনা acpi_sdt_header)) / পরিমাণ(u32_t);

	বাবে (i = 0; i < sdt_গুনতি; i++) {
		সংরচনা acpi_sdt_header hdr;
		পূর্ণাঙ্ক j;
		যদি (read_func(rsdt.data[i], &hdr, পরিমাণ(সংরচনা acpi_sdt_header))) {
			printf("ERROR acpi cannot read header at 0x%x\n",
								rsdt.data[i]);
			ঘূৰাই;
		}

		বাবে (j = 0 ; j < ACPI_SDT_SIGNATURE_LEN; j++)
			sdt_trans[i].signature[j] = hdr.signature[j];
		sdt_trans[i].signature[ACPI_SDT_SIGNATURE_LEN] = '\0';
		sdt_trans[i].length = hdr.length;
	}

	acpi_init_poweroff();
}

সংরচনা acpi_madt_ioapic * acpi_get_ioapic_next(শূন্য)
{
	স্থির unsigned idx = 0;
	স্থির সংরচনা acpi_madt_hdr * madt_hdr;

	সংরচনা acpi_madt_ioapic * ret;

	যদি (idx == 0) {
		madt_hdr = (সংরচনা acpi_madt_hdr *)
			acpi_phys2vir(acpi_get_table_base("APIC"));
		যদি (madt_hdr == রিক্ত)
			ঘূৰাই রিক্ত;
	}

	ret = (সংরচনা acpi_madt_ioapic *)
		acpi_madt_get_typed_item(madt_hdr, ACPI_MADT_TYPE_IOAPIC, idx);
	যদি (ret)
		idx++;

	ঘূৰাই ret;
}

সংরচনা acpi_madt_lapic * acpi_get_lapic_next(শূন্য)
{
	স্থির unsigned idx = 0;
	স্থির সংরচনা acpi_madt_hdr * madt_hdr;

	সংরচনা acpi_madt_lapic * ret;

	যদি (idx == 0) {
		madt_hdr = (সংরচনা acpi_madt_hdr *)
			acpi_phys2vir(acpi_get_table_base("APIC"));
		যদি (madt_hdr == রিক্ত)
			ঘূৰাই রিক্ত;
	}

	বাবে (;;) {
		ret = (সংরচনা acpi_madt_lapic *)
			acpi_madt_get_typed_item(madt_hdr,
					ACPI_MADT_TYPE_LAPIC, idx);
		যদি (!ret)
			ভঙ্গ;

		idx++;

		/* report only usable CPUs */
		যদি (ret->flags & 1)
			ভঙ্গ;
	}

	ঘূৰাই ret;
}

শূন্য __k_unpaged_acpi_poweroff(শূন্য)
{
	/* NO OP poweroff symbol*/
}

শূন্য acpi_poweroff(শূন্য)
{
	যদি (pm1a_cnt_blk == 0) {
		ঘূৰাই;
	}
	outw(pm1a_cnt_blk, slp_typa | SLP_EN_CODE);
	যদি (pm1b_cnt_blk != 0) {
		outw(pm1b_cnt_blk, slp_typb | SLP_EN_CODE);
	}
}
