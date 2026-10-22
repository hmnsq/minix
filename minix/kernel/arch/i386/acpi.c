
#ಅನ್ತರ್ಭಾವ <string.h>

#ಅನ್ತರ್ಭಾವ "acpi.h"
#ಅನ್ತರ್ಭಾವ "arch_proto.h"

ಪ್ರಕಾರನಿರ್ದೇಶ ಪೂರ್ಣಾಙ್ಕ ((* acpi_read_t)(phys_bytes addr, ಶೂನ್ಯ * buff, size_t size));

ಸಂರಚನಾ acpi_rsdp acpi_rsdp;

ಸ್ಥಿರ acpi_read_t read_func;

#define MAX_RSDT	35 /* ACPI defines 35 signatures */
#define SLP_EN_CODE	(1 << 13) /* ACPI SLP_EN_CODE code */
#define AMI_PACKAGE_OP_CODE (0x12)
#define AMI_NAME_OP_CODE (0x8)
#define AMI_BYTE_PREFIX_CODE (0xA)
#define AMI_PACKAGE_LENGTH_ENCODING_BITS_MASK (0xC0)
#define AMI_PACKAGE_LENGTH_ENCODING_BITS_SHIFT (6)
#define AMI_MIN_PACKAGE_LENGTH (1)
#define AMI_ಅಂಕೆ_ELEMENTS_LENGTH (1)
#define AMI_SLP_TYPA_SHIFT (10)
#define AMI_SLP_TYPB_SHIFT (10)
#define AMI_S5_NAME_OP_OFFSET_1 (-1)
#define AMI_S5_NAME_OP_OFFSET_2 (-2)
#define AMI_S5_PACKAGE_OP_OFFSET (4)
#define AMI_S5_PACKET_LENGTH_OFFSET (5)

ಸ್ಥಿರ ಸಂರಚನಾ acpi_rsdt {
	ಸಂರಚನಾ acpi_sdt_header	hdr;
	u32_t			data[MAX_RSDT];
} rsdt;
	
ಸ್ಥಿರ ಸಂರಚನಾ {
	ವರ್ಣ	signature [ACPI_SDT_SIGNATURE_LEN + 1];
	size_t	length;
} sdt_trans[MAX_RSDT];

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ sdt_ಎಣಿಕೆ;
ಸ್ಥಿರ u16_t pm1a_cnt_blk = 0;
ಸ್ಥಿರ u16_t pm1b_cnt_blk = 0;
ಸ್ಥಿರ u16_t slp_typa = 0;
ಸ್ಥಿರ u16_t slp_typb = 0;

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ acpi_check_csum(ಸಂರಚನಾ acpi_sdt_header * tb, size_t size)
{
	u8_t total = 0;
	ಪೂರ್ಣಾಙ್ಕ i;
	ಪ್ರತಿ (i = 0; i < size; i++)
		total += ((unsigned ವರ್ಣ *)tb)[i];
	ಪ್ರತ್ಯಾಗಮ total == 0 ? 0 : -1;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ acpi_check_signature(ಧ್ರುವ ವರ್ಣ * orig, ಧ್ರುವ ವರ್ಣ * match)
{
	ಪ್ರತ್ಯಾಗಮ strncmp(orig, match, ACPI_SDT_SIGNATURE_LEN);
}

ಸ್ಥಿರ u32_t acpi_phys2vir(u32_t p)
{
	ಯದಿ(!vm_running) {
		DEBUGEXTRA(("acpi: returning 0x%lx as vir addr\n", p));
		ಪ್ರತ್ಯಾಗಮ p;
	}
	panic("acpi: can't get virtual address of arbitrary physical address");
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ acpi_phys_copy(phys_bytes phys, ಶೂನ್ಯ *target, size_t len)
{
	ಯದಿ(!vm_running) {
		memcpy(target, (ಶೂನ್ಯ *) phys, len);
		ಪ್ರತ್ಯಾಗಮ 0;
	}
	panic("can't acpi_phys_copy with vm");
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ acpi_read_sdt_at(phys_bytes addr,
				ಸಂರಚನಾ acpi_sdt_header * tb,
				size_t size,
				ಧ್ರುವ ವರ್ಣ * name)
{
	ಸಂರಚನಾ acpi_sdt_header hdr;

	/* if NULL is supplied, we only return the size of the table */
	ಯದಿ (tb == ರಿಕ್ತ) {
		ಯದಿ (read_func(addr, &hdr, ಪರಿಮಾಣ(ಸಂರಚನಾ acpi_sdt_header))) {
			printf("ERROR acpi cannot read %s header\n", name);
			ಪ್ರತ್ಯಾಗಮ -1;
		}

		ಪ್ರತ್ಯಾಗಮ hdr.length;
	}

	ಯದಿ (read_func(addr, tb, ಪರಿಮಾಣ(ಸಂರಚನಾ acpi_sdt_header))) {
		printf("ERROR acpi cannot read %s header\n", name);
		ಪ್ರತ್ಯಾಗಮ -1;
	}

	ಯದಿ (acpi_check_signature(tb->signature, name)) {
		printf("ERROR acpi %s signature does not match\n", name);
		ಪ್ರತ್ಯಾಗಮ -1;
	}

	ಯದಿ (size < tb->length) {
		printf("ERROR acpi buffer too small for %s\n", name);
		ಪ್ರತ್ಯಾಗಮ -1;
	}

	ಯದಿ (read_func(addr, tb, size)) {
		printf("ERROR acpi cannot read %s\n", name);
		ಪ್ರತ್ಯಾಗಮ -1;
	}

	ಯದಿ (acpi_check_csum(tb, tb->length)) {
		printf("ERROR acpi %s checksum does not match\n", name);
		ಪ್ರತ್ಯಾಗಮ -1;
	}

	ಪ್ರತ್ಯಾಗಮ tb->length;
}

phys_bytes acpi_get_table_base(ಧ್ರುವ ವರ್ಣ * name)
{
	ಪೂರ್ಣಾಙ್ಕ i;

	ಪ್ರತಿ(i = 0; i < sdt_ಎಣಿಕೆ; i++) {
		ಯದಿ (strncmp(name, sdt_trans[i].signature,
					ACPI_SDT_SIGNATURE_LEN) == 0)
			ಪ್ರತ್ಯಾಗಮ (phys_bytes) rsdt.data[i];
	}

	ಪ್ರತ್ಯಾಗಮ (phys_bytes) ರಿಕ್ತ;
}

size_t acpi_get_table_length(ಧ್ರುವ ವರ್ಣ * name)
{
	ಪೂರ್ಣಾಙ್ಕ i;

	ಪ್ರತಿ(i = 0; i < sdt_ಎಣಿಕೆ; i++) {
		ಯದಿ (strncmp(name, sdt_trans[i].signature,
					ACPI_SDT_SIGNATURE_LEN) == 0)
			ಪ್ರತ್ಯಾಗಮ sdt_trans[i].length;
	}

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಶೂನ್ಯ * acpi_madt_get_typed_item(ಸಂರಚನಾ acpi_madt_hdr * hdr,
					unsigned ವರ್ಣ type,
					unsigned idx)
{
	u8_t * t, * end;
	ಪೂರ್ಣಾಙ್ಕ i;

	t = (u8_t *) hdr + ಪರಿಮಾಣ(ಸಂರಚನಾ acpi_madt_hdr);
	end = (u8_t *) hdr + hdr->hdr.length;

	i = 0;
	ಯಾವತ್(t < end) {
		ಯದಿ (type == ((ಸಂರಚನಾ acpi_madt_item_hdr *) t)->type) {
			ಯದಿ (i == idx)
				ಪ್ರತ್ಯಾಗಮ t;
			ಅನ್ಯಥಾ
				i++;
		}
		t += ((ಸಂರಚನಾ acpi_madt_item_hdr *) t)->length;
	}

	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

#ಯದಿ 0
ಸ್ಥಿರ ಶೂನ್ಯ * acpi_madt_get_item(ಸಂರಚನಾ acpi_madt_hdr * hdr,
				unsigned idx)
{
	u8_t * t, * end;
	ಪೂರ್ಣಾಙ್ಕ i;

	t = (u8_t *) hdr + ಪರಿಮಾಣ(ಸಂರಚನಾ acpi_madt_hdr);
	end = (u8_t *) hdr + hdr->hdr.length;

	ಪ್ರತಿ(i = 0 ; i <= idx && t < end; i++) {
		ಯದಿ (i == idx)
			ಪ್ರತ್ಯಾಗಮ t;
		t += ((ಸಂರಚನಾ acpi_madt_item_hdr *) t)->length;
	}

	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}
#endif

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ acpi_rsdp_test(ಶೂನ್ಯ * buff)
{
	ಸಂರಚನಾ acpi_rsdp * rsdp = (ಸಂರಚನಾ acpi_rsdp *) buff;

	ಯದಿ (!platform_tbl_checksum_ok(buff, 20))
		ಪ್ರತ್ಯಾಗಮ 0;
	ಯದಿ (strncmp(rsdp->signature, "RSD PTR ", 8))
		ಪ್ರತ್ಯಾಗಮ 0;

	ಪ್ರತ್ಯಾಗಮ 1;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ get_acpi_rsdp(ಶೂನ್ಯ)
{
	u16_t ebda;
	/*
	 * Read 40:0Eh - to find the starting address of the EBDA.
	 */
	acpi_phys_copy (0x40E, &ebda, ಪರಿಮಾಣ(ebda));
	ಯದಿ (ebda) {
		ebda <<= 4;
		ಯದಿ(platform_tbl_ptr(ebda, ebda + 0x400, 16, &acpi_rsdp,
					ಪರಿಮಾಣ(acpi_rsdp), &machine.acpi_rsdp,
					acpi_rsdp_test))
			ಪ್ರತ್ಯಾಗಮ 1;
	} 

	/* try BIOS read only mem space */
	ಯದಿ(platform_tbl_ptr(0xE0000, 0x100000, 16, &acpi_rsdp,
				ಪರಿಮಾಣ(acpi_rsdp), &machine.acpi_rsdp,
				acpi_rsdp_test))
		ಪ್ರತ್ಯಾಗಮ 1;
	
	machine.acpi_rsdp = 0; /* RSDP cannot be found at this address therefore
				  it is a valid negative value */
	ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಶೂನ್ಯ acpi_init_poweroff(ಶೂನ್ಯ)
{
	u8_t *ptr = ರಿಕ್ತ;
	u8_t *start = ರಿಕ್ತ;
	u8_t *end = ರಿಕ್ತ;
	ಸಂರಚನಾ acpi_fadt_header *fadt_header = ರಿಕ್ತ;
	ಸಂರಚನಾ acpi_rsdt * dsdt_header = ರಿಕ್ತ;
	ವರ್ಣ *ಸಂದೇಶ = ರಿಕ್ತ;

	/* Everything used here existed since ACPI spec 1.0 */
	/* So we can safely use them */
	fadt_header = (ಸಂರಚನಾ acpi_fadt_header *)
		acpi_phys2vir(acpi_get_table_base("FACP"));
	ಯದಿ (fadt_header == ರಿಕ್ತ) {
		ಸಂದೇಶ = "Could not load FACP";
		ಗಚ್ಛ exit;
	}

	dsdt_header = (ಸಂರಚನಾ acpi_rsdt *)
		acpi_phys2vir((phys_bytes) fadt_header->dsdt);
	ಯದಿ (dsdt_header == ರಿಕ್ತ) {
		ಸಂದೇಶ = "Could not load DSDT";
		ಗಚ್ಛ exit;
	}

	pm1a_cnt_blk = fadt_header->pm1a_cnt_blk;
	pm1b_cnt_blk = fadt_header->pm1b_cnt_blk;

	ptr = start = (u8_t *) dsdt_header->data;
	end = start + dsdt_header->hdr.length - 4;

	/* See http://forum.osdev.org/viewtopic.php?t=16990 */
	/* for layout of \_S5 */
	ಯಾವತ್ (ptr < end && memcmp(ptr, "_S5_", 4) != 0)
		ptr++;

	ಸಂದೇಶ = "Could not read S5 data. Use default SLP_TYPa and SLP_TYPb";
	ಯದಿ (ptr >= end || ptr == start)
		ಗಚ್ಛ exit;

	/* validate AML structure */
	ಯದಿ (*(ptr + AMI_S5_PACKAGE_OP_OFFSET) != AMI_PACKAGE_OP_CODE)
		ಗಚ್ಛ exit;

	ಯದಿ ((ptr < start + (-AMI_S5_NAME_OP_OFFSET_2) ||
		(*(ptr + AMI_S5_NAME_OP_OFFSET_2) != AMI_NAME_OP_CODE ||
		 *(ptr + AMI_S5_NAME_OP_OFFSET_2 + 1) != '\\')) &&
		*(ptr + AMI_S5_NAME_OP_OFFSET_1) != AMI_NAME_OP_CODE)
		ಗಚ್ಛ exit;

	ptr += AMI_S5_PACKET_LENGTH_OFFSET;
	ಯದಿ (ptr >= end)
		ಗಚ್ಛ exit;

	/* package length */
	ptr += ((*ptr & AMI_PACKAGE_LENGTH_ENCODING_BITS_MASK) >>
		AMI_PACKAGE_LENGTH_ENCODING_BITS_SHIFT) +
		AMI_MIN_PACKAGE_LENGTH + AMI_ಅಂಕೆ_ELEMENTS_LENGTH;
	ಯದಿ (ptr >= end)
		ಗಚ್ಛ exit;

	ಯದಿ (*ptr == AMI_BYTE_PREFIX_CODE)
		ptr++; /* skip byte prefix */

	slp_typa = (*ptr) << AMI_SLP_TYPA_SHIFT;

	ptr++; /* move to SLP_TYPb */
	ಯದಿ (*ptr == AMI_BYTE_PREFIX_CODE)
		ptr++; /* skip byte prefix */

	slp_typb = (*ptr) << AMI_SLP_TYPB_SHIFT;

	ಸಂದೇಶ = "poweroff initialized";

exit:
	ಯದಿ (ಸಂದೇಶ) {
		DEBUGBASIC(("acpi: %s\n", ಸಂದೇಶ));
	}
}

ಶೂನ್ಯ acpi_init(ಶೂನ್ಯ)
{
	ಪೂರ್ಣಾಙ್ಕ s, i;
	read_func = acpi_phys_copy;

	ಯದಿ (!get_acpi_rsdp()) {
		printf("WARNING : Cannot configure ACPI\n");
		ಪ್ರತ್ಯಾಗಮ;
	}
	
	s = acpi_read_sdt_at(acpi_rsdp.rsdt_addr, (ಸಂರಚನಾ acpi_sdt_header *) &rsdt,
			ಪರಿಮಾಣ(ಸಂರಚನಾ acpi_rsdt), ACPI_SDT_SIGNATURE(RSDT));

	sdt_ಎಣಿಕೆ = (s - ಪರಿಮಾಣ(ಸಂರಚನಾ acpi_sdt_header)) / ಪರಿಮಾಣ(u32_t);

	ಪ್ರತಿ (i = 0; i < sdt_ಎಣಿಕೆ; i++) {
		ಸಂರಚನಾ acpi_sdt_header hdr;
		ಪೂರ್ಣಾಙ್ಕ j;
		ಯದಿ (read_func(rsdt.data[i], &hdr, ಪರಿಮಾಣ(ಸಂರಚನಾ acpi_sdt_header))) {
			printf("ERROR acpi cannot read header at 0x%x\n",
								rsdt.data[i]);
			ಪ್ರತ್ಯಾಗಮ;
		}

		ಪ್ರತಿ (j = 0 ; j < ACPI_SDT_SIGNATURE_LEN; j++)
			sdt_trans[i].signature[j] = hdr.signature[j];
		sdt_trans[i].signature[ACPI_SDT_SIGNATURE_LEN] = '\0';
		sdt_trans[i].length = hdr.length;
	}

	acpi_init_poweroff();
}

ಸಂರಚನಾ acpi_madt_ioapic * acpi_get_ioapic_next(ಶೂನ್ಯ)
{
	ಸ್ಥಿರ unsigned idx = 0;
	ಸ್ಥಿರ ಸಂರಚನಾ acpi_madt_hdr * madt_hdr;

	ಸಂರಚನಾ acpi_madt_ioapic * ret;

	ಯದಿ (idx == 0) {
		madt_hdr = (ಸಂರಚನಾ acpi_madt_hdr *)
			acpi_phys2vir(acpi_get_table_base("APIC"));
		ಯದಿ (madt_hdr == ರಿಕ್ತ)
			ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}

	ret = (ಸಂರಚನಾ acpi_madt_ioapic *)
		acpi_madt_get_typed_item(madt_hdr, ACPI_MADT_TYPE_IOAPIC, idx);
	ಯದಿ (ret)
		idx++;

	ಪ್ರತ್ಯಾಗಮ ret;
}

ಸಂರಚನಾ acpi_madt_lapic * acpi_get_lapic_next(ಶೂನ್ಯ)
{
	ಸ್ಥಿರ unsigned idx = 0;
	ಸ್ಥಿರ ಸಂರಚನಾ acpi_madt_hdr * madt_hdr;

	ಸಂರಚನಾ acpi_madt_lapic * ret;

	ಯದಿ (idx == 0) {
		madt_hdr = (ಸಂರಚನಾ acpi_madt_hdr *)
			acpi_phys2vir(acpi_get_table_base("APIC"));
		ಯದಿ (madt_hdr == ರಿಕ್ತ)
			ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}

	ಪ್ರತಿ (;;) {
		ret = (ಸಂರಚನಾ acpi_madt_lapic *)
			acpi_madt_get_typed_item(madt_hdr,
					ACPI_MADT_TYPE_LAPIC, idx);
		ಯದಿ (!ret)
			ಭಙ್ಗ;

		idx++;

		/* report only usable CPUs */
		ಯದಿ (ret->flags & 1)
			ಭಙ್ಗ;
	}

	ಪ್ರತ್ಯಾಗಮ ret;
}

ಶೂನ್ಯ __k_unpaged_acpi_poweroff(ಶೂನ್ಯ)
{
	/* NO OP poweroff symbol*/
}

ಶೂನ್ಯ acpi_poweroff(ಶೂನ್ಯ)
{
	ಯದಿ (pm1a_cnt_blk == 0) {
		ಪ್ರತ್ಯಾಗಮ;
	}
	outw(pm1a_cnt_blk, slp_typa | SLP_EN_CODE);
	ಯದಿ (pm1b_cnt_blk != 0) {
		outw(pm1b_cnt_blk, slp_typb | SLP_EN_CODE);
	}
}
