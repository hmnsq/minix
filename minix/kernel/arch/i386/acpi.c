
#അന്തര്ഭാവ <string.h>

#അന്തര്ഭാവ "acpi.h"
#അന്തര്ഭാവ "arch_proto.h"

തരനിർവചനം പൂർണ്ണസംഖ്യ ((* acpi_read_t)(phys_bytes addr, ശൂന്യം * buff, size_t size));

ഘടന acpi_rsdp acpi_rsdp;

സ്ഥിതം acpi_read_t read_func;

#define MAX_RSDT	35 /* ACPI defines 35 signatures */
#define SLP_EN_CODE	(1 << 13) /* ACPI SLP_EN_CODE code */
#define AMI_PACKAGE_OP_CODE (0x12)
#define AMI_NAME_OP_CODE (0x8)
#define AMI_BYTE_PREFIX_CODE (0xA)
#define AMI_PACKAGE_LENGTH_ENCODING_BITS_MASK (0xC0)
#define AMI_PACKAGE_LENGTH_ENCODING_BITS_SHIFT (6)
#define AMI_MIN_PACKAGE_LENGTH (1)
#define AMI_അക്കം_ELEMENTS_LENGTH (1)
#define AMI_SLP_TYPA_SHIFT (10)
#define AMI_SLP_TYPB_SHIFT (10)
#define AMI_S5_NAME_OP_OFFSET_1 (-1)
#define AMI_S5_NAME_OP_OFFSET_2 (-2)
#define AMI_S5_PACKAGE_OP_OFFSET (4)
#define AMI_S5_PACKET_LENGTH_OFFSET (5)

സ്ഥിതം ഘടന acpi_rsdt {
	ഘടന acpi_sdt_header	hdr;
	u32_t			data[MAX_RSDT];
} rsdt;
	
സ്ഥിതം ഘടന {
	അക്ഷരം	signature [ACPI_SDT_SIGNATURE_LEN + 1];
	size_t	length;
} sdt_trans[MAX_RSDT];

സ്ഥിതം പൂർണ്ണസംഖ്യ sdt_എണ്ണം;
സ്ഥിതം u16_t pm1a_cnt_blk = 0;
സ്ഥിതം u16_t pm1b_cnt_blk = 0;
സ്ഥിതം u16_t slp_typa = 0;
സ്ഥിതം u16_t slp_typb = 0;

സ്ഥിതം പൂർണ്ണസംഖ്യ acpi_check_csum(ഘടന acpi_sdt_header * tb, size_t size)
{
	u8_t total = 0;
	പൂർണ്ണസംഖ്യ i;
	വേണ്ടി (i = 0; i < size; i++)
		total += ((unsigned അക്ഷരം *)tb)[i];
	തിരികെ total == 0 ? 0 : -1;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ acpi_check_signature(സ്ഥിരം അക്ഷരം * orig, സ്ഥിരം അക്ഷരം * match)
{
	തിരികെ strncmp(orig, match, ACPI_SDT_SIGNATURE_LEN);
}

സ്ഥിതം u32_t acpi_phys2vir(u32_t p)
{
	എങ്കിൽ(!vm_running) {
		DEBUGEXTRA(("acpi: returning 0x%lx as vir addr\n", p));
		തിരികെ p;
	}
	panic("acpi: can't get virtual address of arbitrary physical address");
}

സ്ഥിതം പൂർണ്ണസംഖ്യ acpi_phys_copy(phys_bytes phys, ശൂന്യം *target, size_t len)
{
	എങ്കിൽ(!vm_running) {
		memcpy(target, (ശൂന്യം *) phys, len);
		തിരികെ 0;
	}
	panic("can't acpi_phys_copy with vm");
}

സ്ഥിതം പൂർണ്ണസംഖ്യ acpi_read_sdt_at(phys_bytes addr,
				ഘടന acpi_sdt_header * tb,
				size_t size,
				സ്ഥിരം അക്ഷരം * name)
{
	ഘടന acpi_sdt_header hdr;

	/* if NULL is supplied, we only return the size of the table */
	എങ്കിൽ (tb == ശൂന്യ) {
		എങ്കിൽ (read_func(addr, &hdr, വലുപ്പം(ഘടന acpi_sdt_header))) {
			printf("ERROR acpi cannot read %s header\n", name);
			തിരികെ -1;
		}

		തിരികെ hdr.length;
	}

	എങ്കിൽ (read_func(addr, tb, വലുപ്പം(ഘടന acpi_sdt_header))) {
		printf("ERROR acpi cannot read %s header\n", name);
		തിരികെ -1;
	}

	എങ്കിൽ (acpi_check_signature(tb->signature, name)) {
		printf("ERROR acpi %s signature does not match\n", name);
		തിരികെ -1;
	}

	എങ്കിൽ (size < tb->length) {
		printf("ERROR acpi buffer too small for %s\n", name);
		തിരികെ -1;
	}

	എങ്കിൽ (read_func(addr, tb, size)) {
		printf("ERROR acpi cannot read %s\n", name);
		തിരികെ -1;
	}

	എങ്കിൽ (acpi_check_csum(tb, tb->length)) {
		printf("ERROR acpi %s checksum does not match\n", name);
		തിരികെ -1;
	}

	തിരികെ tb->length;
}

phys_bytes acpi_get_table_base(സ്ഥിരം അക്ഷരം * name)
{
	പൂർണ്ണസംഖ്യ i;

	വേണ്ടി(i = 0; i < sdt_എണ്ണം; i++) {
		എങ്കിൽ (strncmp(name, sdt_trans[i].signature,
					ACPI_SDT_SIGNATURE_LEN) == 0)
			തിരികെ (phys_bytes) rsdt.data[i];
	}

	തിരികെ (phys_bytes) ശൂന്യ;
}

size_t acpi_get_table_length(സ്ഥിരം അക്ഷരം * name)
{
	പൂർണ്ണസംഖ്യ i;

	വേണ്ടി(i = 0; i < sdt_എണ്ണം; i++) {
		എങ്കിൽ (strncmp(name, sdt_trans[i].signature,
					ACPI_SDT_SIGNATURE_LEN) == 0)
			തിരികെ sdt_trans[i].length;
	}

	തിരികെ 0;
}

സ്ഥിതം ശൂന്യം * acpi_madt_get_typed_item(ഘടന acpi_madt_hdr * hdr,
					unsigned അക്ഷരം type,
					unsigned idx)
{
	u8_t * t, * end;
	പൂർണ്ണസംഖ്യ i;

	t = (u8_t *) hdr + വലുപ്പം(ഘടന acpi_madt_hdr);
	end = (u8_t *) hdr + hdr->hdr.length;

	i = 0;
	ആകുമ്പോൾ(t < end) {
		എങ്കിൽ (type == ((ഘടന acpi_madt_item_hdr *) t)->type) {
			എങ്കിൽ (i == idx)
				തിരികെ t;
			അല്ലെങ്കിൽ
				i++;
		}
		t += ((ഘടന acpi_madt_item_hdr *) t)->length;
	}

	തിരികെ ശൂന്യ;
}

#എങ്കിൽ 0
സ്ഥിതം ശൂന്യം * acpi_madt_get_item(ഘടന acpi_madt_hdr * hdr,
				unsigned idx)
{
	u8_t * t, * end;
	പൂർണ്ണസംഖ്യ i;

	t = (u8_t *) hdr + വലുപ്പം(ഘടന acpi_madt_hdr);
	end = (u8_t *) hdr + hdr->hdr.length;

	വേണ്ടി(i = 0 ; i <= idx && t < end; i++) {
		എങ്കിൽ (i == idx)
			തിരികെ t;
		t += ((ഘടന acpi_madt_item_hdr *) t)->length;
	}

	തിരികെ ശൂന്യ;
}
#endif

സ്ഥിതം പൂർണ്ണസംഖ്യ acpi_rsdp_test(ശൂന്യം * buff)
{
	ഘടന acpi_rsdp * rsdp = (ഘടന acpi_rsdp *) buff;

	എങ്കിൽ (!platform_tbl_checksum_ok(buff, 20))
		തിരികെ 0;
	എങ്കിൽ (strncmp(rsdp->signature, "RSD PTR ", 8))
		തിരികെ 0;

	തിരികെ 1;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ get_acpi_rsdp(ശൂന്യം)
{
	u16_t ebda;
	/*
	 * Read 40:0Eh - to find the starting address of the EBDA.
	 */
	acpi_phys_copy (0x40E, &ebda, വലുപ്പം(ebda));
	എങ്കിൽ (ebda) {
		ebda <<= 4;
		എങ്കിൽ(platform_tbl_ptr(ebda, ebda + 0x400, 16, &acpi_rsdp,
					വലുപ്പം(acpi_rsdp), &machine.acpi_rsdp,
					acpi_rsdp_test))
			തിരികെ 1;
	} 

	/* try BIOS read only mem space */
	എങ്കിൽ(platform_tbl_ptr(0xE0000, 0x100000, 16, &acpi_rsdp,
				വലുപ്പം(acpi_rsdp), &machine.acpi_rsdp,
				acpi_rsdp_test))
		തിരികെ 1;
	
	machine.acpi_rsdp = 0; /* RSDP cannot be found at this address therefore
				  it is a valid negative value */
	തിരികെ 0;
}

സ്ഥിതം ശൂന്യം acpi_init_poweroff(ശൂന്യം)
{
	u8_t *ptr = ശൂന്യ;
	u8_t *start = ശൂന്യ;
	u8_t *end = ശൂന്യ;
	ഘടന acpi_fadt_header *fadt_header = ശൂന്യ;
	ഘടന acpi_rsdt * dsdt_header = ശൂന്യ;
	അക്ഷരം *സന്ദേശം = ശൂന്യ;

	/* Everything used here existed since ACPI spec 1.0 */
	/* So we can safely use them */
	fadt_header = (ഘടന acpi_fadt_header *)
		acpi_phys2vir(acpi_get_table_base("FACP"));
	എങ്കിൽ (fadt_header == ശൂന്യ) {
		സന്ദേശം = "Could not load FACP";
		പോകുക exit;
	}

	dsdt_header = (ഘടന acpi_rsdt *)
		acpi_phys2vir((phys_bytes) fadt_header->dsdt);
	എങ്കിൽ (dsdt_header == ശൂന്യ) {
		സന്ദേശം = "Could not load DSDT";
		പോകുക exit;
	}

	pm1a_cnt_blk = fadt_header->pm1a_cnt_blk;
	pm1b_cnt_blk = fadt_header->pm1b_cnt_blk;

	ptr = start = (u8_t *) dsdt_header->data;
	end = start + dsdt_header->hdr.length - 4;

	/* See http://forum.osdev.org/viewtopic.php?t=16990 */
	/* for layout of \_S5 */
	ആകുമ്പോൾ (ptr < end && memcmp(ptr, "_S5_", 4) != 0)
		ptr++;

	സന്ദേശം = "Could not read S5 data. Use default SLP_TYPa and SLP_TYPb";
	എങ്കിൽ (ptr >= end || ptr == start)
		പോകുക exit;

	/* validate AML structure */
	എങ്കിൽ (*(ptr + AMI_S5_PACKAGE_OP_OFFSET) != AMI_PACKAGE_OP_CODE)
		പോകുക exit;

	എങ്കിൽ ((ptr < start + (-AMI_S5_NAME_OP_OFFSET_2) ||
		(*(ptr + AMI_S5_NAME_OP_OFFSET_2) != AMI_NAME_OP_CODE ||
		 *(ptr + AMI_S5_NAME_OP_OFFSET_2 + 1) != '\\')) &&
		*(ptr + AMI_S5_NAME_OP_OFFSET_1) != AMI_NAME_OP_CODE)
		പോകുക exit;

	ptr += AMI_S5_PACKET_LENGTH_OFFSET;
	എങ്കിൽ (ptr >= end)
		പോകുക exit;

	/* package length */
	ptr += ((*ptr & AMI_PACKAGE_LENGTH_ENCODING_BITS_MASK) >>
		AMI_PACKAGE_LENGTH_ENCODING_BITS_SHIFT) +
		AMI_MIN_PACKAGE_LENGTH + AMI_അക്കം_ELEMENTS_LENGTH;
	എങ്കിൽ (ptr >= end)
		പോകുക exit;

	എങ്കിൽ (*ptr == AMI_BYTE_PREFIX_CODE)
		ptr++; /* skip byte prefix */

	slp_typa = (*ptr) << AMI_SLP_TYPA_SHIFT;

	ptr++; /* move to SLP_TYPb */
	എങ്കിൽ (*ptr == AMI_BYTE_PREFIX_CODE)
		ptr++; /* skip byte prefix */

	slp_typb = (*ptr) << AMI_SLP_TYPB_SHIFT;

	സന്ദേശം = "poweroff initialized";

exit:
	എങ്കിൽ (സന്ദേശം) {
		DEBUGBASIC(("acpi: %s\n", സന്ദേശം));
	}
}

ശൂന്യം acpi_init(ശൂന്യം)
{
	പൂർണ്ണസംഖ്യ s, i;
	read_func = acpi_phys_copy;

	എങ്കിൽ (!get_acpi_rsdp()) {
		printf("WARNING : Cannot configure ACPI\n");
		തിരികെ;
	}
	
	s = acpi_read_sdt_at(acpi_rsdp.rsdt_addr, (ഘടന acpi_sdt_header *) &rsdt,
			വലുപ്പം(ഘടന acpi_rsdt), ACPI_SDT_SIGNATURE(RSDT));

	sdt_എണ്ണം = (s - വലുപ്പം(ഘടന acpi_sdt_header)) / വലുപ്പം(u32_t);

	വേണ്ടി (i = 0; i < sdt_എണ്ണം; i++) {
		ഘടന acpi_sdt_header hdr;
		പൂർണ്ണസംഖ്യ j;
		എങ്കിൽ (read_func(rsdt.data[i], &hdr, വലുപ്പം(ഘടന acpi_sdt_header))) {
			printf("ERROR acpi cannot read header at 0x%x\n",
								rsdt.data[i]);
			തിരികെ;
		}

		വേണ്ടി (j = 0 ; j < ACPI_SDT_SIGNATURE_LEN; j++)
			sdt_trans[i].signature[j] = hdr.signature[j];
		sdt_trans[i].signature[ACPI_SDT_SIGNATURE_LEN] = '\0';
		sdt_trans[i].length = hdr.length;
	}

	acpi_init_poweroff();
}

ഘടന acpi_madt_ioapic * acpi_get_ioapic_next(ശൂന്യം)
{
	സ്ഥിതം unsigned idx = 0;
	സ്ഥിതം ഘടന acpi_madt_hdr * madt_hdr;

	ഘടന acpi_madt_ioapic * ret;

	എങ്കിൽ (idx == 0) {
		madt_hdr = (ഘടന acpi_madt_hdr *)
			acpi_phys2vir(acpi_get_table_base("APIC"));
		എങ്കിൽ (madt_hdr == ശൂന്യ)
			തിരികെ ശൂന്യ;
	}

	ret = (ഘടന acpi_madt_ioapic *)
		acpi_madt_get_typed_item(madt_hdr, ACPI_MADT_TYPE_IOAPIC, idx);
	എങ്കിൽ (ret)
		idx++;

	തിരികെ ret;
}

ഘടന acpi_madt_lapic * acpi_get_lapic_next(ശൂന്യം)
{
	സ്ഥിതം unsigned idx = 0;
	സ്ഥിതം ഘടന acpi_madt_hdr * madt_hdr;

	ഘടന acpi_madt_lapic * ret;

	എങ്കിൽ (idx == 0) {
		madt_hdr = (ഘടന acpi_madt_hdr *)
			acpi_phys2vir(acpi_get_table_base("APIC"));
		എങ്കിൽ (madt_hdr == ശൂന്യ)
			തിരികെ ശൂന്യ;
	}

	വേണ്ടി (;;) {
		ret = (ഘടന acpi_madt_lapic *)
			acpi_madt_get_typed_item(madt_hdr,
					ACPI_MADT_TYPE_LAPIC, idx);
		എങ്കിൽ (!ret)
			നിർത്തുക;

		idx++;

		/* report only usable CPUs */
		എങ്കിൽ (ret->flags & 1)
			നിർത്തുക;
	}

	തിരികെ ret;
}

ശൂന്യം __k_unpaged_acpi_poweroff(ശൂന്യം)
{
	/* NO OP poweroff symbol*/
}

ശൂന്യം acpi_poweroff(ശൂന്യം)
{
	എങ്കിൽ (pm1a_cnt_blk == 0) {
		തിരികെ;
	}
	outw(pm1a_cnt_blk, slp_typa | SLP_EN_CODE);
	എങ്കിൽ (pm1b_cnt_blk != 0) {
		outw(pm1b_cnt_blk, slp_typb | SLP_EN_CODE);
	}
}
