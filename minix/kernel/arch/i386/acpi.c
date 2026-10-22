
#अन्तर्भाव <string.h>

#अन्तर्भाव "acpi.h"
#अन्तर्भाव "arch_proto.h"

प्रकारनिर्देश पूर्णाङ्क ((* acpi_read_t)(phys_bytes addr, शून्य * buff, size_t size));

संरचना acpi_rsdp acpi_rsdp;

स्थिर acpi_read_t read_func;

#define MAX_RSDT	35 /* ACPI defines 35 signatures */
#define SLP_EN_CODE	(1 << 13) /* ACPI SLP_EN_CODE code */
#define AMI_PACKAGE_OP_CODE (0x12)
#define AMI_NAME_OP_CODE (0x8)
#define AMI_BYTE_PREFIX_CODE (0xA)
#define AMI_PACKAGE_LENGTH_ENCODING_BITS_MASK (0xC0)
#define AMI_PACKAGE_LENGTH_ENCODING_BITS_SHIFT (6)
#define AMI_MIN_PACKAGE_LENGTH (1)
#define AMI_अङ्कः_ELEMENTS_LENGTH (1)
#define AMI_SLP_TYPA_SHIFT (10)
#define AMI_SLP_TYPB_SHIFT (10)
#define AMI_S5_NAME_OP_OFFSET_1 (-1)
#define AMI_S5_NAME_OP_OFFSET_2 (-2)
#define AMI_S5_PACKAGE_OP_OFFSET (4)
#define AMI_S5_PACKET_LENGTH_OFFSET (5)

स्थिर संरचना acpi_rsdt {
	संरचना acpi_sdt_header	hdr;
	u32_t			data[MAX_RSDT];
} rsdt;
	
स्थिर संरचना {
	वर्ण	signature [ACPI_SDT_SIGNATURE_LEN + 1];
	size_t	length;
} sdt_trans[MAX_RSDT];

स्थिर पूर्णाङ्क sdt_गणनम्;
स्थिर u16_t pm1a_cnt_blk = 0;
स्थिर u16_t pm1b_cnt_blk = 0;
स्थिर u16_t slp_typa = 0;
स्थिर u16_t slp_typb = 0;

स्थिर पूर्णाङ्क acpi_check_csum(संरचना acpi_sdt_header * tb, size_t size)
{
	u8_t total = 0;
	पूर्णाङ्क i;
	अत्थाय (i = 0; i < size; i++)
		total += ((unsigned वर्ण *)tb)[i];
	पटिनिवत्त total == 0 ? 0 : -1;
}

स्थिर पूर्णाङ्क acpi_check_signature(ध्रुव वर्ण * orig, ध्रुव वर्ण * match)
{
	पटिनिवत्त strncmp(orig, match, ACPI_SDT_SIGNATURE_LEN);
}

स्थिर u32_t acpi_phys2vir(u32_t p)
{
	सचे(!vm_running) {
		DEBUGEXTRA(("acpi: returning 0x%lx as vir addr\n", p));
		पटिनिवत्त p;
	}
	panic("acpi: can't get virtual address of arbitrary physical address");
}

स्थिर पूर्णाङ्क acpi_phys_copy(phys_bytes phys, शून्य *target, size_t len)
{
	सचे(!vm_running) {
		memcpy(target, (शून्य *) phys, len);
		पटिनिवत्त 0;
	}
	panic("can't acpi_phys_copy with vm");
}

स्थिर पूर्णाङ्क acpi_read_sdt_at(phys_bytes addr,
				संरचना acpi_sdt_header * tb,
				size_t size,
				ध्रुव वर्ण * name)
{
	संरचना acpi_sdt_header hdr;

	/* if NULL is supplied, we only return the size of the table */
	सचे (tb == रिक्त) {
		सचे (read_func(addr, &hdr, परिमाण(संरचना acpi_sdt_header))) {
			printf("ERROR acpi cannot read %s header\n", name);
			पटिनिवत्त -1;
		}

		पटिनिवत्त hdr.length;
	}

	सचे (read_func(addr, tb, परिमाण(संरचना acpi_sdt_header))) {
		printf("ERROR acpi cannot read %s header\n", name);
		पटिनिवत्त -1;
	}

	सचे (acpi_check_signature(tb->signature, name)) {
		printf("ERROR acpi %s signature does not match\n", name);
		पटिनिवत्त -1;
	}

	सचे (size < tb->length) {
		printf("ERROR acpi buffer too small for %s\n", name);
		पटिनिवत्त -1;
	}

	सचे (read_func(addr, tb, size)) {
		printf("ERROR acpi cannot read %s\n", name);
		पटिनिवत्त -1;
	}

	सचे (acpi_check_csum(tb, tb->length)) {
		printf("ERROR acpi %s checksum does not match\n", name);
		पटिनिवत्त -1;
	}

	पटिनिवत्त tb->length;
}

phys_bytes acpi_get_table_base(ध्रुव वर्ण * name)
{
	पूर्णाङ्क i;

	अत्थाय(i = 0; i < sdt_गणनम्; i++) {
		सचे (strncmp(name, sdt_trans[i].signature,
					ACPI_SDT_SIGNATURE_LEN) == 0)
			पटिनिवत्त (phys_bytes) rsdt.data[i];
	}

	पटिनिवत्त (phys_bytes) रिक्त;
}

size_t acpi_get_table_length(ध्रुव वर्ण * name)
{
	पूर्णाङ्क i;

	अत्थाय(i = 0; i < sdt_गणनम्; i++) {
		सचे (strncmp(name, sdt_trans[i].signature,
					ACPI_SDT_SIGNATURE_LEN) == 0)
			पटिनिवत्त sdt_trans[i].length;
	}

	पटिनिवत्त 0;
}

स्थिर शून्य * acpi_madt_get_typed_item(संरचना acpi_madt_hdr * hdr,
					unsigned वर्ण type,
					unsigned idx)
{
	u8_t * t, * end;
	पूर्णाङ्क i;

	t = (u8_t *) hdr + परिमाण(संरचना acpi_madt_hdr);
	end = (u8_t *) hdr + hdr->hdr.length;

	i = 0;
	याव(t < end) {
		सचे (type == ((संरचना acpi_madt_item_hdr *) t)->type) {
			सचे (i == idx)
				पटिनिवत्त t;
			अञ्ञथा
				i++;
		}
		t += ((संरचना acpi_madt_item_hdr *) t)->length;
	}

	पटिनिवत्त रिक्त;
}

#सचे 0
स्थिर शून्य * acpi_madt_get_item(संरचना acpi_madt_hdr * hdr,
				unsigned idx)
{
	u8_t * t, * end;
	पूर्णाङ्क i;

	t = (u8_t *) hdr + परिमाण(संरचना acpi_madt_hdr);
	end = (u8_t *) hdr + hdr->hdr.length;

	अत्थाय(i = 0 ; i <= idx && t < end; i++) {
		सचे (i == idx)
			पटिनिवत्त t;
		t += ((संरचना acpi_madt_item_hdr *) t)->length;
	}

	पटिनिवत्त रिक्त;
}
#endif

स्थिर पूर्णाङ्क acpi_rsdp_test(शून्य * buff)
{
	संरचना acpi_rsdp * rsdp = (संरचना acpi_rsdp *) buff;

	सचे (!platform_tbl_checksum_ok(buff, 20))
		पटिनिवत्त 0;
	सचे (strncmp(rsdp->signature, "RSD PTR ", 8))
		पटिनिवत्त 0;

	पटिनिवत्त 1;
}

स्थिर पूर्णाङ्क get_acpi_rsdp(शून्य)
{
	u16_t ebda;
	/*
	 * Read 40:0Eh - to find the starting address of the EBDA.
	 */
	acpi_phys_copy (0x40E, &ebda, परिमाण(ebda));
	सचे (ebda) {
		ebda <<= 4;
		सचे(platform_tbl_ptr(ebda, ebda + 0x400, 16, &acpi_rsdp,
					परिमाण(acpi_rsdp), &machine.acpi_rsdp,
					acpi_rsdp_test))
			पटिनिवत्त 1;
	} 

	/* try BIOS read only mem space */
	सचे(platform_tbl_ptr(0xE0000, 0x100000, 16, &acpi_rsdp,
				परिमाण(acpi_rsdp), &machine.acpi_rsdp,
				acpi_rsdp_test))
		पटिनिवत्त 1;
	
	machine.acpi_rsdp = 0; /* RSDP cannot be found at this address therefore
				  it is a valid negative value */
	पटिनिवत्त 0;
}

स्थिर शून्य acpi_init_poweroff(शून्य)
{
	u8_t *ptr = रिक्त;
	u8_t *start = रिक्त;
	u8_t *end = रिक्त;
	संरचना acpi_fadt_header *fadt_header = रिक्त;
	संरचना acpi_rsdt * dsdt_header = रिक्त;
	वर्ण *सन्देशः = रिक्त;

	/* Everything used here existed since ACPI spec 1.0 */
	/* So we can safely use them */
	fadt_header = (संरचना acpi_fadt_header *)
		acpi_phys2vir(acpi_get_table_base("FACP"));
	सचे (fadt_header == रिक्त) {
		सन्देशः = "Could not load FACP";
		गच्छ exit;
	}

	dsdt_header = (संरचना acpi_rsdt *)
		acpi_phys2vir((phys_bytes) fadt_header->dsdt);
	सचे (dsdt_header == रिक्त) {
		सन्देशः = "Could not load DSDT";
		गच्छ exit;
	}

	pm1a_cnt_blk = fadt_header->pm1a_cnt_blk;
	pm1b_cnt_blk = fadt_header->pm1b_cnt_blk;

	ptr = start = (u8_t *) dsdt_header->data;
	end = start + dsdt_header->hdr.length - 4;

	/* See http://forum.osdev.org/viewtopic.php?t=16990 */
	/* for layout of \_S5 */
	याव (ptr < end && memcmp(ptr, "_S5_", 4) != 0)
		ptr++;

	सन्देशः = "Could not read S5 data. Use default SLP_TYPa and SLP_TYPb";
	सचे (ptr >= end || ptr == start)
		गच्छ exit;

	/* validate AML structure */
	सचे (*(ptr + AMI_S5_PACKAGE_OP_OFFSET) != AMI_PACKAGE_OP_CODE)
		गच्छ exit;

	सचे ((ptr < start + (-AMI_S5_NAME_OP_OFFSET_2) ||
		(*(ptr + AMI_S5_NAME_OP_OFFSET_2) != AMI_NAME_OP_CODE ||
		 *(ptr + AMI_S5_NAME_OP_OFFSET_2 + 1) != '\\')) &&
		*(ptr + AMI_S5_NAME_OP_OFFSET_1) != AMI_NAME_OP_CODE)
		गच्छ exit;

	ptr += AMI_S5_PACKET_LENGTH_OFFSET;
	सचे (ptr >= end)
		गच्छ exit;

	/* package length */
	ptr += ((*ptr & AMI_PACKAGE_LENGTH_ENCODING_BITS_MASK) >>
		AMI_PACKAGE_LENGTH_ENCODING_BITS_SHIFT) +
		AMI_MIN_PACKAGE_LENGTH + AMI_अङ्कः_ELEMENTS_LENGTH;
	सचे (ptr >= end)
		गच्छ exit;

	सचे (*ptr == AMI_BYTE_PREFIX_CODE)
		ptr++; /* skip byte prefix */

	slp_typa = (*ptr) << AMI_SLP_TYPA_SHIFT;

	ptr++; /* move to SLP_TYPb */
	सचे (*ptr == AMI_BYTE_PREFIX_CODE)
		ptr++; /* skip byte prefix */

	slp_typb = (*ptr) << AMI_SLP_TYPB_SHIFT;

	सन्देशः = "poweroff initialized";

exit:
	सचे (सन्देशः) {
		DEBUGBASIC(("acpi: %s\n", सन्देशः));
	}
}

शून्य acpi_init(शून्य)
{
	पूर्णाङ्क s, i;
	read_func = acpi_phys_copy;

	सचे (!get_acpi_rsdp()) {
		printf("WARNING : Cannot configure ACPI\n");
		पटिनिवत्त;
	}
	
	s = acpi_read_sdt_at(acpi_rsdp.rsdt_addr, (संरचना acpi_sdt_header *) &rsdt,
			परिमाण(संरचना acpi_rsdt), ACPI_SDT_SIGNATURE(RSDT));

	sdt_गणनम् = (s - परिमाण(संरचना acpi_sdt_header)) / परिमाण(u32_t);

	अत्थाय (i = 0; i < sdt_गणनम्; i++) {
		संरचना acpi_sdt_header hdr;
		पूर्णाङ्क j;
		सचे (read_func(rsdt.data[i], &hdr, परिमाण(संरचना acpi_sdt_header))) {
			printf("ERROR acpi cannot read header at 0x%x\n",
								rsdt.data[i]);
			पटिनिवत्त;
		}

		अत्थाय (j = 0 ; j < ACPI_SDT_SIGNATURE_LEN; j++)
			sdt_trans[i].signature[j] = hdr.signature[j];
		sdt_trans[i].signature[ACPI_SDT_SIGNATURE_LEN] = '\0';
		sdt_trans[i].length = hdr.length;
	}

	acpi_init_poweroff();
}

संरचना acpi_madt_ioapic * acpi_get_ioapic_next(शून्य)
{
	स्थिर unsigned idx = 0;
	स्थिर संरचना acpi_madt_hdr * madt_hdr;

	संरचना acpi_madt_ioapic * ret;

	सचे (idx == 0) {
		madt_hdr = (संरचना acpi_madt_hdr *)
			acpi_phys2vir(acpi_get_table_base("APIC"));
		सचे (madt_hdr == रिक्त)
			पटिनिवत्त रिक्त;
	}

	ret = (संरचना acpi_madt_ioapic *)
		acpi_madt_get_typed_item(madt_hdr, ACPI_MADT_TYPE_IOAPIC, idx);
	सचे (ret)
		idx++;

	पटिनिवत्त ret;
}

संरचना acpi_madt_lapic * acpi_get_lapic_next(शून्य)
{
	स्थिर unsigned idx = 0;
	स्थिर संरचना acpi_madt_hdr * madt_hdr;

	संरचना acpi_madt_lapic * ret;

	सचे (idx == 0) {
		madt_hdr = (संरचना acpi_madt_hdr *)
			acpi_phys2vir(acpi_get_table_base("APIC"));
		सचे (madt_hdr == रिक्त)
			पटिनिवत्त रिक्त;
	}

	अत्थाय (;;) {
		ret = (संरचना acpi_madt_lapic *)
			acpi_madt_get_typed_item(madt_hdr,
					ACPI_MADT_TYPE_LAPIC, idx);
		सचे (!ret)
			भङ्ग;

		idx++;

		/* report only usable CPUs */
		सचे (ret->flags & 1)
			भङ्ग;
	}

	पटिनिवत्त ret;
}

शून्य __k_unpaged_acpi_poweroff(शून्य)
{
	/* NO OP poweroff symbol*/
}

शून्य acpi_poweroff(शून्य)
{
	सचे (pm1a_cnt_blk == 0) {
		पटिनिवत्त;
	}
	outw(pm1a_cnt_blk, slp_typa | SLP_EN_CODE);
	सचे (pm1b_cnt_blk != 0) {
		outw(pm1b_cnt_blk, slp_typb | SLP_EN_CODE);
	}
}
