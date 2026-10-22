
#ଅନ୍ତର୍ଭାଵ <string.h>

#ଅନ୍ତର୍ଭାଵ "acpi.h"
#ଅନ୍ତର୍ଭାଵ "arch_proto.h"

ପ୍ରକାରବ୍ୟାଖ୍ୟା ପୂର୍ଣ୍ଣସଂଖ୍ୟା ((* acpi_read_t)(phys_bytes addr, ଶୂନ୍ୟ * buff, size_t size));

ସଂରଚନା acpi_rsdp acpi_rsdp;

ସ୍ଥିର acpi_read_t read_func;

#define MAX_RSDT	35 /* ACPI defines 35 signatures */
#define SLP_EN_CODE	(1 << 13) /* ACPI SLP_EN_CODE code */
#define AMI_PACKAGE_OP_CODE (0x12)
#define AMI_NAME_OP_CODE (0x8)
#define AMI_BYTE_PREFIX_CODE (0xA)
#define AMI_PACKAGE_LENGTH_ENCODING_BITS_MASK (0xC0)
#define AMI_PACKAGE_LENGTH_ENCODING_BITS_SHIFT (6)
#define AMI_MIN_PACKAGE_LENGTH (1)
#define AMI_ଅଙ୍କ_ELEMENTS_LENGTH (1)
#define AMI_SLP_TYPA_SHIFT (10)
#define AMI_SLP_TYPB_SHIFT (10)
#define AMI_S5_NAME_OP_OFFSET_1 (-1)
#define AMI_S5_NAME_OP_OFFSET_2 (-2)
#define AMI_S5_PACKAGE_OP_OFFSET (4)
#define AMI_S5_PACKET_LENGTH_OFFSET (5)

ସ୍ଥିର ସଂରଚନା acpi_rsdt {
	ସଂରଚନା acpi_sdt_header	hdr;
	u32_t			data[MAX_RSDT];
} rsdt;
	
ସ୍ଥିର ସଂରଚନା {
	ଅକ୍ଷର	signature [ACPI_SDT_SIGNATURE_LEN + 1];
	size_t	length;
} sdt_trans[MAX_RSDT];

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା sdt_ଗଣନା;
ସ୍ଥିର u16_t pm1a_cnt_blk = 0;
ସ୍ଥିର u16_t pm1b_cnt_blk = 0;
ସ୍ଥିର u16_t slp_typa = 0;
ସ୍ଥିର u16_t slp_typb = 0;

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା acpi_check_csum(ସଂରଚନା acpi_sdt_header * tb, size_t size)
{
	u8_t total = 0;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
	ପାଇଁ (i = 0; i < size; i++)
		total += ((unsigned ଅକ୍ଷର *)tb)[i];
	ଫେରନ୍ତୁ total == 0 ? 0 : -1;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା acpi_check_signature(ଅଚଳ ଅକ୍ଷର * orig, ଅଚଳ ଅକ୍ଷର * match)
{
	ଫେରନ୍ତୁ strncmp(orig, match, ACPI_SDT_SIGNATURE_LEN);
}

ସ୍ଥିର u32_t acpi_phys2vir(u32_t p)
{
	ଯଦି(!vm_running) {
		DEBUGEXTRA(("acpi: returning 0x%lx as vir addr\n", p));
		ଫେରନ୍ତୁ p;
	}
	panic("acpi: can't get virtual address of arbitrary physical address");
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା acpi_phys_copy(phys_bytes phys, ଶୂନ୍ୟ *target, size_t len)
{
	ଯଦି(!vm_running) {
		memcpy(target, (ଶୂନ୍ୟ *) phys, len);
		ଫେରନ୍ତୁ 0;
	}
	panic("can't acpi_phys_copy with vm");
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା acpi_read_sdt_at(phys_bytes addr,
				ସଂରଚନା acpi_sdt_header * tb,
				size_t size,
				ଅଚଳ ଅକ୍ଷର * name)
{
	ସଂରଚନା acpi_sdt_header hdr;

	/* if NULL is supplied, we only return the size of the table */
	ଯଦି (tb == ଖାଲି) {
		ଯଦି (read_func(addr, &hdr, ଆକାର(ସଂରଚନା acpi_sdt_header))) {
			printf("ERROR acpi cannot read %s header\n", name);
			ଫେରନ୍ତୁ -1;
		}

		ଫେରନ୍ତୁ hdr.length;
	}

	ଯଦି (read_func(addr, tb, ଆକାର(ସଂରଚନା acpi_sdt_header))) {
		printf("ERROR acpi cannot read %s header\n", name);
		ଫେରନ୍ତୁ -1;
	}

	ଯଦି (acpi_check_signature(tb->signature, name)) {
		printf("ERROR acpi %s signature does not match\n", name);
		ଫେରନ୍ତୁ -1;
	}

	ଯଦି (size < tb->length) {
		printf("ERROR acpi buffer too small for %s\n", name);
		ଫେରନ୍ତୁ -1;
	}

	ଯଦି (read_func(addr, tb, size)) {
		printf("ERROR acpi cannot read %s\n", name);
		ଫେରନ୍ତୁ -1;
	}

	ଯଦି (acpi_check_csum(tb, tb->length)) {
		printf("ERROR acpi %s checksum does not match\n", name);
		ଫେରନ୍ତୁ -1;
	}

	ଫେରନ୍ତୁ tb->length;
}

phys_bytes acpi_get_table_base(ଅଚଳ ଅକ୍ଷର * name)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

	ପାଇଁ(i = 0; i < sdt_ଗଣନା; i++) {
		ଯଦି (strncmp(name, sdt_trans[i].signature,
					ACPI_SDT_SIGNATURE_LEN) == 0)
			ଫେରନ୍ତୁ (phys_bytes) rsdt.data[i];
	}

	ଫେରନ୍ତୁ (phys_bytes) ଖାଲି;
}

size_t acpi_get_table_length(ଅଚଳ ଅକ୍ଷର * name)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

	ପାଇଁ(i = 0; i < sdt_ଗଣନା; i++) {
		ଯଦି (strncmp(name, sdt_trans[i].signature,
					ACPI_SDT_SIGNATURE_LEN) == 0)
			ଫେରନ୍ତୁ sdt_trans[i].length;
	}

	ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ଶୂନ୍ୟ * acpi_madt_get_typed_item(ସଂରଚନା acpi_madt_hdr * hdr,
					unsigned ଅକ୍ଷର type,
					unsigned idx)
{
	u8_t * t, * end;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

	t = (u8_t *) hdr + ଆକାର(ସଂରଚନା acpi_madt_hdr);
	end = (u8_t *) hdr + hdr->hdr.length;

	i = 0;
	ଯେତେବେଳେ(t < end) {
		ଯଦି (type == ((ସଂରଚନା acpi_madt_item_hdr *) t)->type) {
			ଯଦି (i == idx)
				ଫେରନ୍ତୁ t;
			ଅନ୍ୟଥା
				i++;
		}
		t += ((ସଂରଚନା acpi_madt_item_hdr *) t)->length;
	}

	ଫେରନ୍ତୁ ଖାଲି;
}

#ଯଦି 0
ସ୍ଥିର ଶୂନ୍ୟ * acpi_madt_get_item(ସଂରଚନା acpi_madt_hdr * hdr,
				unsigned idx)
{
	u8_t * t, * end;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

	t = (u8_t *) hdr + ଆକାର(ସଂରଚନା acpi_madt_hdr);
	end = (u8_t *) hdr + hdr->hdr.length;

	ପାଇଁ(i = 0 ; i <= idx && t < end; i++) {
		ଯଦି (i == idx)
			ଫେରନ୍ତୁ t;
		t += ((ସଂରଚନା acpi_madt_item_hdr *) t)->length;
	}

	ଫେରନ୍ତୁ ଖାଲି;
}
#endif

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା acpi_rsdp_test(ଶୂନ୍ୟ * buff)
{
	ସଂରଚନା acpi_rsdp * rsdp = (ସଂରଚନା acpi_rsdp *) buff;

	ଯଦି (!platform_tbl_checksum_ok(buff, 20))
		ଫେରନ୍ତୁ 0;
	ଯଦି (strncmp(rsdp->signature, "RSD PTR ", 8))
		ଫେରନ୍ତୁ 0;

	ଫେରନ୍ତୁ 1;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_acpi_rsdp(ଶୂନ୍ୟ)
{
	u16_t ebda;
	/*
	 * Read 40:0Eh - to find the starting address of the EBDA.
	 */
	acpi_phys_copy (0x40E, &ebda, ଆକାର(ebda));
	ଯଦି (ebda) {
		ebda <<= 4;
		ଯଦି(platform_tbl_ptr(ebda, ebda + 0x400, 16, &acpi_rsdp,
					ଆକାର(acpi_rsdp), &machine.acpi_rsdp,
					acpi_rsdp_test))
			ଫେରନ୍ତୁ 1;
	} 

	/* try BIOS read only mem space */
	ଯଦି(platform_tbl_ptr(0xE0000, 0x100000, 16, &acpi_rsdp,
				ଆକାର(acpi_rsdp), &machine.acpi_rsdp,
				acpi_rsdp_test))
		ଫେରନ୍ତୁ 1;
	
	machine.acpi_rsdp = 0; /* RSDP cannot be found at this address therefore
				  it is a valid negative value */
	ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ଶୂନ୍ୟ acpi_init_poweroff(ଶୂନ୍ୟ)
{
	u8_t *ptr = ଖାଲି;
	u8_t *start = ଖାଲି;
	u8_t *end = ଖାଲି;
	ସଂରଚନା acpi_fadt_header *fadt_header = ଖାଲି;
	ସଂରଚନା acpi_rsdt * dsdt_header = ଖାଲି;
	ଅକ୍ଷର *ସନ୍ଦେଶ = ଖାଲି;

	/* Everything used here existed since ACPI spec 1.0 */
	/* So we can safely use them */
	fadt_header = (ସଂରଚନା acpi_fadt_header *)
		acpi_phys2vir(acpi_get_table_base("FACP"));
	ଯଦି (fadt_header == ଖାଲି) {
		ସନ୍ଦେଶ = "Could not load FACP";
		ଯାଅ exit;
	}

	dsdt_header = (ସଂରଚନା acpi_rsdt *)
		acpi_phys2vir((phys_bytes) fadt_header->dsdt);
	ଯଦି (dsdt_header == ଖାଲି) {
		ସନ୍ଦେଶ = "Could not load DSDT";
		ଯାଅ exit;
	}

	pm1a_cnt_blk = fadt_header->pm1a_cnt_blk;
	pm1b_cnt_blk = fadt_header->pm1b_cnt_blk;

	ptr = start = (u8_t *) dsdt_header->data;
	end = start + dsdt_header->hdr.length - 4;

	/* See http://forum.osdev.org/viewtopic.php?t=16990 */
	/* for layout of \_S5 */
	ଯେତେବେଳେ (ptr < end && memcmp(ptr, "_S5_", 4) != 0)
		ptr++;

	ସନ୍ଦେଶ = "Could not read S5 data. Use default SLP_TYPa and SLP_TYPb";
	ଯଦି (ptr >= end || ptr == start)
		ଯାଅ exit;

	/* validate AML structure */
	ଯଦି (*(ptr + AMI_S5_PACKAGE_OP_OFFSET) != AMI_PACKAGE_OP_CODE)
		ଯାଅ exit;

	ଯଦି ((ptr < start + (-AMI_S5_NAME_OP_OFFSET_2) ||
		(*(ptr + AMI_S5_NAME_OP_OFFSET_2) != AMI_NAME_OP_CODE ||
		 *(ptr + AMI_S5_NAME_OP_OFFSET_2 + 1) != '\\')) &&
		*(ptr + AMI_S5_NAME_OP_OFFSET_1) != AMI_NAME_OP_CODE)
		ଯାଅ exit;

	ptr += AMI_S5_PACKET_LENGTH_OFFSET;
	ଯଦି (ptr >= end)
		ଯାଅ exit;

	/* package length */
	ptr += ((*ptr & AMI_PACKAGE_LENGTH_ENCODING_BITS_MASK) >>
		AMI_PACKAGE_LENGTH_ENCODING_BITS_SHIFT) +
		AMI_MIN_PACKAGE_LENGTH + AMI_ଅଙ୍କ_ELEMENTS_LENGTH;
	ଯଦି (ptr >= end)
		ଯାଅ exit;

	ଯଦି (*ptr == AMI_BYTE_PREFIX_CODE)
		ptr++; /* skip byte prefix */

	slp_typa = (*ptr) << AMI_SLP_TYPA_SHIFT;

	ptr++; /* move to SLP_TYPb */
	ଯଦି (*ptr == AMI_BYTE_PREFIX_CODE)
		ptr++; /* skip byte prefix */

	slp_typb = (*ptr) << AMI_SLP_TYPB_SHIFT;

	ସନ୍ଦେଶ = "poweroff initialized";

exit:
	ଯଦି (ସନ୍ଦେଶ) {
		DEBUGBASIC(("acpi: %s\n", ସନ୍ଦେଶ));
	}
}

ଶୂନ୍ୟ acpi_init(ଶୂନ୍ୟ)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା s, i;
	read_func = acpi_phys_copy;

	ଯଦି (!get_acpi_rsdp()) {
		printf("WARNING : Cannot configure ACPI\n");
		ଫେରନ୍ତୁ;
	}
	
	s = acpi_read_sdt_at(acpi_rsdp.rsdt_addr, (ସଂରଚନା acpi_sdt_header *) &rsdt,
			ଆକାର(ସଂରଚନା acpi_rsdt), ACPI_SDT_SIGNATURE(RSDT));

	sdt_ଗଣନା = (s - ଆକାର(ସଂରଚନା acpi_sdt_header)) / ଆକାର(u32_t);

	ପାଇଁ (i = 0; i < sdt_ଗଣନା; i++) {
		ସଂରଚନା acpi_sdt_header hdr;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା j;
		ଯଦି (read_func(rsdt.data[i], &hdr, ଆକାର(ସଂରଚନା acpi_sdt_header))) {
			printf("ERROR acpi cannot read header at 0x%x\n",
								rsdt.data[i]);
			ଫେରନ୍ତୁ;
		}

		ପାଇଁ (j = 0 ; j < ACPI_SDT_SIGNATURE_LEN; j++)
			sdt_trans[i].signature[j] = hdr.signature[j];
		sdt_trans[i].signature[ACPI_SDT_SIGNATURE_LEN] = '\0';
		sdt_trans[i].length = hdr.length;
	}

	acpi_init_poweroff();
}

ସଂରଚନା acpi_madt_ioapic * acpi_get_ioapic_next(ଶୂନ୍ୟ)
{
	ସ୍ଥିର unsigned idx = 0;
	ସ୍ଥିର ସଂରଚନା acpi_madt_hdr * madt_hdr;

	ସଂରଚନା acpi_madt_ioapic * ret;

	ଯଦି (idx == 0) {
		madt_hdr = (ସଂରଚନା acpi_madt_hdr *)
			acpi_phys2vir(acpi_get_table_base("APIC"));
		ଯଦି (madt_hdr == ଖାଲି)
			ଫେରନ୍ତୁ ଖାଲି;
	}

	ret = (ସଂରଚନା acpi_madt_ioapic *)
		acpi_madt_get_typed_item(madt_hdr, ACPI_MADT_TYPE_IOAPIC, idx);
	ଯଦି (ret)
		idx++;

	ଫେରନ୍ତୁ ret;
}

ସଂରଚନା acpi_madt_lapic * acpi_get_lapic_next(ଶୂନ୍ୟ)
{
	ସ୍ଥିର unsigned idx = 0;
	ସ୍ଥିର ସଂରଚନା acpi_madt_hdr * madt_hdr;

	ସଂରଚନା acpi_madt_lapic * ret;

	ଯଦି (idx == 0) {
		madt_hdr = (ସଂରଚନା acpi_madt_hdr *)
			acpi_phys2vir(acpi_get_table_base("APIC"));
		ଯଦି (madt_hdr == ଖାଲି)
			ଫେରନ୍ତୁ ଖାଲି;
	}

	ପାଇଁ (;;) {
		ret = (ସଂରଚନା acpi_madt_lapic *)
			acpi_madt_get_typed_item(madt_hdr,
					ACPI_MADT_TYPE_LAPIC, idx);
		ଯଦି (!ret)
			ଭାଙ୍ଗ;

		idx++;

		/* report only usable CPUs */
		ଯଦି (ret->flags & 1)
			ଭାଙ୍ଗ;
	}

	ଫେରନ୍ତୁ ret;
}

ଶୂନ୍ୟ __k_unpaged_acpi_poweroff(ଶୂନ୍ୟ)
{
	/* NO OP poweroff symbol*/
}

ଶୂନ୍ୟ acpi_poweroff(ଶୂନ୍ୟ)
{
	ଯଦି (pm1a_cnt_blk == 0) {
		ଫେରନ୍ତୁ;
	}
	outw(pm1a_cnt_blk, slp_typa | SLP_EN_CODE);
	ଯଦି (pm1b_cnt_blk != 0) {
		outw(pm1b_cnt_blk, slp_typb | SLP_EN_CODE);
	}
}
