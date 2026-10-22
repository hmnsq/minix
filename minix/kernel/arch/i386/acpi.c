
#અન્તર્ભાવ <string.h>

#અન્તર્ભાવ "acpi.h"
#અન્તર્ભાવ "arch_proto.h"

પ્રકારવ્યાખ્યા પૂર્ણાંક ((* acpi_read_t)(phys_bytes addr, શૂન્ય * buff, size_t size));

સંરચના acpi_rsdp acpi_rsdp;

સ્થિર acpi_read_t read_func;

#define MAX_RSDT	35 /* ACPI defines 35 signatures */
#define SLP_EN_CODE	(1 << 13) /* ACPI SLP_EN_CODE code */
#define AMI_PACKAGE_OP_CODE (0x12)
#define AMI_NAME_OP_CODE (0x8)
#define AMI_BYTE_PREFIX_CODE (0xA)
#define AMI_PACKAGE_LENGTH_ENCODING_BITS_MASK (0xC0)
#define AMI_PACKAGE_LENGTH_ENCODING_BITS_SHIFT (6)
#define AMI_MIN_PACKAGE_LENGTH (1)
#define AMI_અંક_ELEMENTS_LENGTH (1)
#define AMI_SLP_TYPA_SHIFT (10)
#define AMI_SLP_TYPB_SHIFT (10)
#define AMI_S5_NAME_OP_OFFSET_1 (-1)
#define AMI_S5_NAME_OP_OFFSET_2 (-2)
#define AMI_S5_PACKAGE_OP_OFFSET (4)
#define AMI_S5_PACKET_LENGTH_OFFSET (5)

સ્થિર સંરચના acpi_rsdt {
	સંરચના acpi_sdt_header	hdr;
	u32_t			data[MAX_RSDT];
} rsdt;
	
સ્થિર સંરચના {
	અક્ષર	signature [ACPI_SDT_SIGNATURE_LEN + 1];
	size_t	length;
} sdt_trans[MAX_RSDT];

સ્થિર પૂર્ણાંક sdt_ગણતરી;
સ્થિર u16_t pm1a_cnt_blk = 0;
સ્થિર u16_t pm1b_cnt_blk = 0;
સ્થિર u16_t slp_typa = 0;
સ્થિર u16_t slp_typb = 0;

સ્થિર પૂર્ણાંક acpi_check_csum(સંરચના acpi_sdt_header * tb, size_t size)
{
	u8_t total = 0;
	પૂર્ણાંક i;
	માટે (i = 0; i < size; i++)
		total += ((unsigned અક્ષર *)tb)[i];
	પાછા total == 0 ? 0 : -1;
}

સ્થિર પૂર્ણાંક acpi_check_signature(અચલ અક્ષર * orig, અચલ અક્ષર * match)
{
	પાછા strncmp(orig, match, ACPI_SDT_SIGNATURE_LEN);
}

સ્થિર u32_t acpi_phys2vir(u32_t p)
{
	જો(!vm_running) {
		DEBUGEXTRA(("acpi: returning 0x%lx as vir addr\n", p));
		પાછા p;
	}
	panic("acpi: can't get virtual address of arbitrary physical address");
}

સ્થિર પૂર્ણાંક acpi_phys_copy(phys_bytes phys, શૂન્ય *target, size_t len)
{
	જો(!vm_running) {
		memcpy(target, (શૂન્ય *) phys, len);
		પાછા 0;
	}
	panic("can't acpi_phys_copy with vm");
}

સ્થિર પૂર્ણાંક acpi_read_sdt_at(phys_bytes addr,
				સંરચના acpi_sdt_header * tb,
				size_t size,
				અચલ અક્ષર * name)
{
	સંરચના acpi_sdt_header hdr;

	/* if NULL is supplied, we only return the size of the table */
	જો (tb == ખાલી) {
		જો (read_func(addr, &hdr, કદ(સંરચના acpi_sdt_header))) {
			printf("ERROR acpi cannot read %s header\n", name);
			પાછા -1;
		}

		પાછા hdr.length;
	}

	જો (read_func(addr, tb, કદ(સંરચના acpi_sdt_header))) {
		printf("ERROR acpi cannot read %s header\n", name);
		પાછા -1;
	}

	જો (acpi_check_signature(tb->signature, name)) {
		printf("ERROR acpi %s signature does not match\n", name);
		પાછા -1;
	}

	જો (size < tb->length) {
		printf("ERROR acpi buffer too small for %s\n", name);
		પાછા -1;
	}

	જો (read_func(addr, tb, size)) {
		printf("ERROR acpi cannot read %s\n", name);
		પાછા -1;
	}

	જો (acpi_check_csum(tb, tb->length)) {
		printf("ERROR acpi %s checksum does not match\n", name);
		પાછા -1;
	}

	પાછા tb->length;
}

phys_bytes acpi_get_table_base(અચલ અક્ષર * name)
{
	પૂર્ણાંક i;

	માટે(i = 0; i < sdt_ગણતરી; i++) {
		જો (strncmp(name, sdt_trans[i].signature,
					ACPI_SDT_SIGNATURE_LEN) == 0)
			પાછા (phys_bytes) rsdt.data[i];
	}

	પાછા (phys_bytes) ખાલી;
}

size_t acpi_get_table_length(અચલ અક્ષર * name)
{
	પૂર્ણાંક i;

	માટે(i = 0; i < sdt_ગણતરી; i++) {
		જો (strncmp(name, sdt_trans[i].signature,
					ACPI_SDT_SIGNATURE_LEN) == 0)
			પાછા sdt_trans[i].length;
	}

	પાછા 0;
}

સ્થિર શૂન્ય * acpi_madt_get_typed_item(સંરચના acpi_madt_hdr * hdr,
					unsigned અક્ષર type,
					unsigned idx)
{
	u8_t * t, * end;
	પૂર્ણાંક i;

	t = (u8_t *) hdr + કદ(સંરચના acpi_madt_hdr);
	end = (u8_t *) hdr + hdr->hdr.length;

	i = 0;
	જ્યારે(t < end) {
		જો (type == ((સંરચના acpi_madt_item_hdr *) t)->type) {
			જો (i == idx)
				પાછા t;
			નહીંતર
				i++;
		}
		t += ((સંરચના acpi_madt_item_hdr *) t)->length;
	}

	પાછા ખાલી;
}

#જો 0
સ્થિર શૂન્ય * acpi_madt_get_item(સંરચના acpi_madt_hdr * hdr,
				unsigned idx)
{
	u8_t * t, * end;
	પૂર્ણાંક i;

	t = (u8_t *) hdr + કદ(સંરચના acpi_madt_hdr);
	end = (u8_t *) hdr + hdr->hdr.length;

	માટે(i = 0 ; i <= idx && t < end; i++) {
		જો (i == idx)
			પાછા t;
		t += ((સંરચના acpi_madt_item_hdr *) t)->length;
	}

	પાછા ખાલી;
}
#endif

સ્થિર પૂર્ણાંક acpi_rsdp_test(શૂન્ય * buff)
{
	સંરચના acpi_rsdp * rsdp = (સંરચના acpi_rsdp *) buff;

	જો (!platform_tbl_checksum_ok(buff, 20))
		પાછા 0;
	જો (strncmp(rsdp->signature, "RSD PTR ", 8))
		પાછા 0;

	પાછા 1;
}

સ્થિર પૂર્ણાંક get_acpi_rsdp(શૂન્ય)
{
	u16_t ebda;
	/*
	 * Read 40:0Eh - to find the starting address of the EBDA.
	 */
	acpi_phys_copy (0x40E, &ebda, કદ(ebda));
	જો (ebda) {
		ebda <<= 4;
		જો(platform_tbl_ptr(ebda, ebda + 0x400, 16, &acpi_rsdp,
					કદ(acpi_rsdp), &machine.acpi_rsdp,
					acpi_rsdp_test))
			પાછા 1;
	} 

	/* try BIOS read only mem space */
	જો(platform_tbl_ptr(0xE0000, 0x100000, 16, &acpi_rsdp,
				કદ(acpi_rsdp), &machine.acpi_rsdp,
				acpi_rsdp_test))
		પાછા 1;
	
	machine.acpi_rsdp = 0; /* RSDP cannot be found at this address therefore
				  it is a valid negative value */
	પાછા 0;
}

સ્થિર શૂન્ય acpi_init_poweroff(શૂન્ય)
{
	u8_t *ptr = ખાલી;
	u8_t *start = ખાલી;
	u8_t *end = ખાલી;
	સંરચના acpi_fadt_header *fadt_header = ખાલી;
	સંરચના acpi_rsdt * dsdt_header = ખાલી;
	અક્ષર *સંદેશ = ખાલી;

	/* Everything used here existed since ACPI spec 1.0 */
	/* So we can safely use them */
	fadt_header = (સંરચના acpi_fadt_header *)
		acpi_phys2vir(acpi_get_table_base("FACP"));
	જો (fadt_header == ખાલી) {
		સંદેશ = "Could not load FACP";
		જાઓ exit;
	}

	dsdt_header = (સંરચના acpi_rsdt *)
		acpi_phys2vir((phys_bytes) fadt_header->dsdt);
	જો (dsdt_header == ખાલી) {
		સંદેશ = "Could not load DSDT";
		જાઓ exit;
	}

	pm1a_cnt_blk = fadt_header->pm1a_cnt_blk;
	pm1b_cnt_blk = fadt_header->pm1b_cnt_blk;

	ptr = start = (u8_t *) dsdt_header->data;
	end = start + dsdt_header->hdr.length - 4;

	/* See http://forum.osdev.org/viewtopic.php?t=16990 */
	/* for layout of \_S5 */
	જ્યારે (ptr < end && memcmp(ptr, "_S5_", 4) != 0)
		ptr++;

	સંદેશ = "Could not read S5 data. Use default SLP_TYPa and SLP_TYPb";
	જો (ptr >= end || ptr == start)
		જાઓ exit;

	/* validate AML structure */
	જો (*(ptr + AMI_S5_PACKAGE_OP_OFFSET) != AMI_PACKAGE_OP_CODE)
		જાઓ exit;

	જો ((ptr < start + (-AMI_S5_NAME_OP_OFFSET_2) ||
		(*(ptr + AMI_S5_NAME_OP_OFFSET_2) != AMI_NAME_OP_CODE ||
		 *(ptr + AMI_S5_NAME_OP_OFFSET_2 + 1) != '\\')) &&
		*(ptr + AMI_S5_NAME_OP_OFFSET_1) != AMI_NAME_OP_CODE)
		જાઓ exit;

	ptr += AMI_S5_PACKET_LENGTH_OFFSET;
	જો (ptr >= end)
		જાઓ exit;

	/* package length */
	ptr += ((*ptr & AMI_PACKAGE_LENGTH_ENCODING_BITS_MASK) >>
		AMI_PACKAGE_LENGTH_ENCODING_BITS_SHIFT) +
		AMI_MIN_PACKAGE_LENGTH + AMI_અંક_ELEMENTS_LENGTH;
	જો (ptr >= end)
		જાઓ exit;

	જો (*ptr == AMI_BYTE_PREFIX_CODE)
		ptr++; /* skip byte prefix */

	slp_typa = (*ptr) << AMI_SLP_TYPA_SHIFT;

	ptr++; /* move to SLP_TYPb */
	જો (*ptr == AMI_BYTE_PREFIX_CODE)
		ptr++; /* skip byte prefix */

	slp_typb = (*ptr) << AMI_SLP_TYPB_SHIFT;

	સંદેશ = "poweroff initialized";

exit:
	જો (સંદેશ) {
		DEBUGBASIC(("acpi: %s\n", સંદેશ));
	}
}

શૂન્ય acpi_init(શૂન્ય)
{
	પૂર્ણાંક s, i;
	read_func = acpi_phys_copy;

	જો (!get_acpi_rsdp()) {
		printf("WARNING : Cannot configure ACPI\n");
		પાછા;
	}
	
	s = acpi_read_sdt_at(acpi_rsdp.rsdt_addr, (સંરચના acpi_sdt_header *) &rsdt,
			કદ(સંરચના acpi_rsdt), ACPI_SDT_SIGNATURE(RSDT));

	sdt_ગણતરી = (s - કદ(સંરચના acpi_sdt_header)) / કદ(u32_t);

	માટે (i = 0; i < sdt_ગણતરી; i++) {
		સંરચના acpi_sdt_header hdr;
		પૂર્ણાંક j;
		જો (read_func(rsdt.data[i], &hdr, કદ(સંરચના acpi_sdt_header))) {
			printf("ERROR acpi cannot read header at 0x%x\n",
								rsdt.data[i]);
			પાછા;
		}

		માટે (j = 0 ; j < ACPI_SDT_SIGNATURE_LEN; j++)
			sdt_trans[i].signature[j] = hdr.signature[j];
		sdt_trans[i].signature[ACPI_SDT_SIGNATURE_LEN] = '\0';
		sdt_trans[i].length = hdr.length;
	}

	acpi_init_poweroff();
}

સંરચના acpi_madt_ioapic * acpi_get_ioapic_next(શૂન્ય)
{
	સ્થિર unsigned idx = 0;
	સ્થિર સંરચના acpi_madt_hdr * madt_hdr;

	સંરચના acpi_madt_ioapic * ret;

	જો (idx == 0) {
		madt_hdr = (સંરચના acpi_madt_hdr *)
			acpi_phys2vir(acpi_get_table_base("APIC"));
		જો (madt_hdr == ખાલી)
			પાછા ખાલી;
	}

	ret = (સંરચના acpi_madt_ioapic *)
		acpi_madt_get_typed_item(madt_hdr, ACPI_MADT_TYPE_IOAPIC, idx);
	જો (ret)
		idx++;

	પાછા ret;
}

સંરચના acpi_madt_lapic * acpi_get_lapic_next(શૂન્ય)
{
	સ્થિર unsigned idx = 0;
	સ્થિર સંરચના acpi_madt_hdr * madt_hdr;

	સંરચના acpi_madt_lapic * ret;

	જો (idx == 0) {
		madt_hdr = (સંરચના acpi_madt_hdr *)
			acpi_phys2vir(acpi_get_table_base("APIC"));
		જો (madt_hdr == ખાલી)
			પાછા ખાલી;
	}

	માટે (;;) {
		ret = (સંરચના acpi_madt_lapic *)
			acpi_madt_get_typed_item(madt_hdr,
					ACPI_MADT_TYPE_LAPIC, idx);
		જો (!ret)
			તોડો;

		idx++;

		/* report only usable CPUs */
		જો (ret->flags & 1)
			તોડો;
	}

	પાછા ret;
}

શૂન્ય __k_unpaged_acpi_poweroff(શૂન્ય)
{
	/* NO OP poweroff symbol*/
}

શૂન્ય acpi_poweroff(શૂન્ય)
{
	જો (pm1a_cnt_blk == 0) {
		પાછા;
	}
	outw(pm1a_cnt_blk, slp_typa | SLP_EN_CODE);
	જો (pm1b_cnt_blk != 0) {
		outw(pm1b_cnt_blk, slp_typb | SLP_EN_CODE);
	}
}
