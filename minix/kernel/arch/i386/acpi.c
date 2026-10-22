
#अन्तर्भाव <string.h>

#अन्तर्भाव "acpi.h"
#अन्तर्भाव "arch_proto.h"

प्रकारनिर्देश पूर्णाङ्क ((* acpi_read_t)(phys_अष्टकानि addr, शून्य * buff, size_t आकार));

संरचना acpi_rsdp acpi_rsdp;

स्थिर acpi_read_t read_func;

#परिभाषय MAX_RSDT	35 /* ACPI defines 35 signatures */
#परिभाषय SLP_EN_संहिता	(1 << 13) /* ACPI SLP_EN_CODE code */
#परिभाषय AMI_PACKAGE_OP_संहिता (0x12)
#परिभाषय AMI_NAME_OP_संहिता (0x8)
#परिभाषय AMI_BYTE_उपसर्गः_संहिता (0xA)
#परिभाषय AMI_PACKAGE_दीर्घता_ENCODING_द्वयङ्काः_आवरणम् (0xC0)
#परिभाषय AMI_PACKAGE_दीर्घता_ENCODING_द्वयङ्काः_सरणम् (6)
#परिभाषय AMI_MIN_PACKAGE_दीर्घता (1)
#परिभाषय AMI_अङ्कः_ELEMENTS_दीर्घता (1)
#परिभाषय AMI_SLP_TYPA_सरणम् (10)
#परिभाषय AMI_SLP_TYPB_सरणम् (10)
#परिभाषय AMI_S5_NAME_OP_विस्थापनम्_1 (-1)
#परिभाषय AMI_S5_NAME_OP_विस्थापनम्_2 (-2)
#परिभाषय AMI_S5_PACKAGE_OP_विस्थापनम् (4)
#परिभाषय AMI_S5_PACKET_दीर्घता_विस्थापनम् (5)

स्थिर संरचना acpi_rsdt {
	संरचना acpi_sdt_शीर्षकम्	hdr;
	u32_t			दत्त[MAX_RSDT];
} rsdt;
	
स्थिर संरचना {
	वर्ण	signature [ACPI_SDT_SIGNATURE_LEN + 1];
	size_t	दीर्घता;
} sdt_trans[MAX_RSDT];

स्थिर पूर्णाङ्क sdt_गणनम्;
स्थिर u16_t pm1a_cnt_blk = 0;
स्थिर u16_t pm1b_cnt_blk = 0;
स्थिर u16_t slp_typa = 0;
स्थिर u16_t slp_typb = 0;

स्थिर पूर्णाङ्क acpi_check_csum(संरचना acpi_sdt_शीर्षकम् * tb, size_t आकार)
{
	u8_t योग = 0;
	पूर्णाङ्क i;
	प्रति (i = 0; i < आकार; i++)
		योग += ((अचिह्नित वर्ण *)tb)[i];
	प्रत्यागम योग == 0 ? 0 : -1;
}

स्थिर पूर्णाङ्क acpi_check_signature(ध्रुव वर्ण * orig, ध्रुव वर्ण * मेलनम्)
{
	प्रत्यागम वाक्ये_तुलय(orig, मेलनम्, ACPI_SDT_SIGNATURE_LEN);
}

स्थिर u32_t acpi_phys2vir(u32_t p)
{
	यदि(!vm_running) {
		DEBUGEXTRA(("acpi: returning 0x%lx as vir addr\n", p));
		प्रत्यागम p;
	}
	panic("acpi: can't get virtual address of arbitrary physical address");
}

स्थिर पूर्णाङ्क acpi_phys_copy(phys_अष्टकानि phys, शून्य *target, size_t len)
{
	यदि(!vm_running) {
		स्मृतिं_प्रतिलिख(target, (शून्य *) phys, len);
		प्रत्यागम 0;
	}
	panic("can't acpi_phys_copy with vm");
}

स्थिर पूर्णाङ्क acpi_read_sdt_at(phys_अष्टकानि addr,
				संरचना acpi_sdt_शीर्षकम् * tb,
				size_t आकार,
				ध्रुव वर्ण * name)
{
	संरचना acpi_sdt_शीर्षकम् hdr;

	/* if NULL is supplied, we only return the size of the table */
	यदि (tb == रिक्त) {
		यदि (read_func(addr, &hdr, परिमाण(संरचना acpi_sdt_शीर्षकम्))) {
			प्रारूप_लिख("ERROR acpi cannot read %s header\n", name);
			प्रत्यागम -1;
		}

		प्रत्यागम hdr.दीर्घता;
	}

	यदि (read_func(addr, tb, परिमाण(संरचना acpi_sdt_शीर्षकम्))) {
		प्रारूप_लिख("ERROR acpi cannot read %s header\n", name);
		प्रत्यागम -1;
	}

	यदि (acpi_check_signature(tb->signature, name)) {
		प्रारूप_लिख("ERROR acpi %s signature does not match\n", name);
		प्रत्यागम -1;
	}

	यदि (आकार < tb->दीर्घता) {
		प्रारूप_लिख("ERROR acpi buffer too small for %s\n", name);
		प्रत्यागम -1;
	}

	यदि (read_func(addr, tb, आकार)) {
		प्रारूप_लिख("ERROR acpi cannot read %s\n", name);
		प्रत्यागम -1;
	}

	यदि (acpi_check_csum(tb, tb->दीर्घता)) {
		प्रारूप_लिख("ERROR acpi %s checksum does not match\n", name);
		प्रत्यागम -1;
	}

	प्रत्यागम tb->दीर्घता;
}

phys_अष्टकानि acpi_get_सारणी_आधारः(ध्रुव वर्ण * name)
{
	पूर्णाङ्क i;

	प्रति(i = 0; i < sdt_गणनम्; i++) {
		यदि (वाक्ये_तुलय(name, sdt_trans[i].signature,
					ACPI_SDT_SIGNATURE_LEN) == 0)
			प्रत्यागम (phys_अष्टकानि) rsdt.दत्त[i];
	}

	प्रत्यागम (phys_अष्टकानि) रिक्त;
}

size_t acpi_get_सारणी_दीर्घता(ध्रुव वर्ण * name)
{
	पूर्णाङ्क i;

	प्रति(i = 0; i < sdt_गणनम्; i++) {
		यदि (वाक्ये_तुलय(name, sdt_trans[i].signature,
					ACPI_SDT_SIGNATURE_LEN) == 0)
			प्रत्यागम sdt_trans[i].दीर्घता;
	}

	प्रत्यागम 0;
}

स्थिर शून्य * acpi_madt_get_typed_item(संरचना acpi_madt_hdr * hdr,
					अचिह्नित वर्ण type,
					अचिह्नित idx)
{
	u8_t * t, * अन्तः_2;
	पूर्णाङ्क i;

	t = (u8_t *) hdr + परिमाण(संरचना acpi_madt_hdr);
	अन्तः_2 = (u8_t *) hdr + hdr->hdr.दीर्घता;

	i = 0;
	यावत्(t < अन्तः_2) {
		यदि (type == ((संरचना acpi_madt_item_hdr *) t)->type) {
			यदि (i == idx)
				प्रत्यागम t;
			अन्यथा
				i++;
		}
		t += ((संरचना acpi_madt_item_hdr *) t)->दीर्घता;
	}

	प्रत्यागम रिक्त;
}

#यदि 0
स्थिर शून्य * acpi_madt_get_item(संरचना acpi_madt_hdr * hdr,
				अचिह्नित idx)
{
	u8_t * t, * अन्तः_2;
	पूर्णाङ्क i;

	t = (u8_t *) hdr + परिमाण(संरचना acpi_madt_hdr);
	अन्तः_2 = (u8_t *) hdr + hdr->hdr.दीर्घता;

	प्रति(i = 0 ; i <= idx && t < अन्तः_2; i++) {
		यदि (i == idx)
			प्रत्यागम t;
		t += ((संरचना acpi_madt_item_hdr *) t)->दीर्घता;
	}

	प्रत्यागम रिक्त;
}
#यदि_अन्तः

स्थिर पूर्णाङ्क acpi_rsdp_test(शून्य * buff)
{
	संरचना acpi_rsdp * rsdp = (संरचना acpi_rsdp *) buff;

	यदि (!platform_tbl_checksum_ok(buff, 20))
		प्रत्यागम 0;
	यदि (वाक्ये_तुलय(rsdp->signature, "RSD PTR ", 8))
		प्रत्यागम 0;

	प्रत्यागम 1;
}

स्थिर पूर्णाङ्क get_acpi_rsdp(शून्य)
{
	u16_t ebda;
	/*
	 * Read 40:0Eh - to find the starting address of the EBDA.
	 */
	acpi_phys_copy (0x40E, &ebda, परिमाण(ebda));
	यदि (ebda) {
		ebda <<= 4;
		यदि(platform_tbl_ptr(ebda, ebda + 0x400, 16, &acpi_rsdp,
					परिमाण(acpi_rsdp), &machine.acpi_rsdp,
					acpi_rsdp_test))
			प्रत्यागम 1;
	} 

	/* try BIOS read only mem space */
	यदि(platform_tbl_ptr(0xE0000, 0x100000, 16, &acpi_rsdp,
				परिमाण(acpi_rsdp), &machine.acpi_rsdp,
				acpi_rsdp_test))
		प्रत्यागम 1;
	
	machine.acpi_rsdp = 0; /* RSDP cannot be found at this address therefore
				  it is a valid negative value */
	प्रत्यागम 0;
}

स्थिर शून्य acpi_init_poweroff(शून्य)
{
	u8_t *ptr = रिक्त;
	u8_t *start = रिक्त;
	u8_t *अन्तः_2 = रिक्त;
	संरचना acpi_fadt_शीर्षकम् *fadt_शीर्षकम् = रिक्त;
	संरचना acpi_rsdt * dsdt_शीर्षकम् = रिक्त;
	वर्ण *सन्देशः_2 = रिक्त;

	/* Everything used here existed since ACPI spec 1.0 */
	/* So we can safely use them */
	fadt_शीर्षकम् = (संरचना acpi_fadt_शीर्षकम् *)
		acpi_phys2vir(acpi_get_सारणी_आधारः("FACP"));
	यदि (fadt_शीर्षकम् == रिक्त) {
		सन्देशः_2 = "Could not load FACP";
		गच्छ निर्गच्छ;
	}

	dsdt_शीर्षकम् = (संरचना acpi_rsdt *)
		acpi_phys2vir((phys_अष्टकानि) fadt_शीर्षकम्->dsdt);
	यदि (dsdt_शीर्षकम् == रिक्त) {
		सन्देशः_2 = "Could not load DSDT";
		गच्छ निर्गच्छ;
	}

	pm1a_cnt_blk = fadt_शीर्षकम्->pm1a_cnt_blk;
	pm1b_cnt_blk = fadt_शीर्षकम्->pm1b_cnt_blk;

	ptr = start = (u8_t *) dsdt_शीर्षकम्->दत्त;
	अन्तः_2 = start + dsdt_शीर्षकम्->hdr.दीर्घता - 4;

	/* See http://forum.osdev.org/viewtopic.php?t=16990 */
	/* for layout of \_S5 */
	यावत् (ptr < अन्तः_2 && memcmp(ptr, "_S5_", 4) != 0)
		ptr++;

	सन्देशः_2 = "Could not read S5 data. Use default SLP_TYPa and SLP_TYPb";
	यदि (ptr >= अन्तः_2 || ptr == start)
		गच्छ निर्गच्छ;

	/* validate AML structure */
	यदि (*(ptr + AMI_S5_PACKAGE_OP_विस्थापनम्) != AMI_PACKAGE_OP_संहिता)
		गच्छ निर्गच्छ;

	यदि ((ptr < start + (-AMI_S5_NAME_OP_विस्थापनम्_2) ||
		(*(ptr + AMI_S5_NAME_OP_विस्थापनम्_2) != AMI_NAME_OP_संहिता ||
		 *(ptr + AMI_S5_NAME_OP_विस्थापनम्_2 + 1) != '\\')) &&
		*(ptr + AMI_S5_NAME_OP_विस्थापनम्_1) != AMI_NAME_OP_संहिता)
		गच्छ निर्गच्छ;

	ptr += AMI_S5_PACKET_दीर्घता_विस्थापनम्;
	यदि (ptr >= अन्तः_2)
		गच्छ निर्गच्छ;

	/* package length */
	ptr += ((*ptr & AMI_PACKAGE_दीर्घता_ENCODING_द्वयङ्काः_आवरणम्) >>
		AMI_PACKAGE_दीर्घता_ENCODING_द्वयङ्काः_सरणम्) +
		AMI_MIN_PACKAGE_दीर्घता + AMI_अङ्कः_ELEMENTS_दीर्घता;
	यदि (ptr >= अन्तः_2)
		गच्छ निर्गच्छ;

	यदि (*ptr == AMI_BYTE_उपसर्गः_संहिता)
		ptr++; /* skip byte prefix */

	slp_typa = (*ptr) << AMI_SLP_TYPA_सरणम्;

	ptr++; /* move to SLP_TYPb */
	यदि (*ptr == AMI_BYTE_उपसर्गः_संहिता)
		ptr++; /* skip byte prefix */

	slp_typb = (*ptr) << AMI_SLP_TYPB_सरणम्;

	सन्देशः_2 = "poweroff initialized";

निर्गच्छ:
	यदि (सन्देशः_2) {
		DEBUGBASIC(("acpi: %s\n", सन्देशः_2));
	}
}

शून्य acpi_init(शून्य)
{
	पूर्णाङ्क s, i;
	read_func = acpi_phys_copy;

	यदि (!get_acpi_rsdp()) {
		प्रारूप_लिख("WARNING : Cannot configure ACPI\n");
		प्रत्यागम;
	}
	
	s = acpi_read_sdt_at(acpi_rsdp.rsdt_addr, (संरचना acpi_sdt_शीर्षकम् *) &rsdt,
			परिमाण(संरचना acpi_rsdt), ACPI_SDT_SIGNATURE(RSDT));

	sdt_गणनम् = (s - परिमाण(संरचना acpi_sdt_शीर्षकम्)) / परिमाण(u32_t);

	प्रति (i = 0; i < sdt_गणनम्; i++) {
		संरचना acpi_sdt_शीर्षकम् hdr;
		पूर्णाङ्क j;
		यदि (read_func(rsdt.दत्त[i], &hdr, परिमाण(संरचना acpi_sdt_शीर्षकम्))) {
			प्रारूप_लिख("ERROR acpi cannot read header at 0x%x\n",
								rsdt.दत्त[i]);
			प्रत्यागम;
		}

		प्रति (j = 0 ; j < ACPI_SDT_SIGNATURE_LEN; j++)
			sdt_trans[i].signature[j] = hdr.signature[j];
		sdt_trans[i].signature[ACPI_SDT_SIGNATURE_LEN] = '\0';
		sdt_trans[i].दीर्घता = hdr.दीर्घता;
	}

	acpi_init_poweroff();
}

संरचना acpi_madt_ioapic * acpi_get_ioapic_अग्र(शून्य)
{
	स्थिर अचिह्नित idx = 0;
	स्थिर संरचना acpi_madt_hdr * madt_hdr;

	संरचना acpi_madt_ioapic * ret;

	यदि (idx == 0) {
		madt_hdr = (संरचना acpi_madt_hdr *)
			acpi_phys2vir(acpi_get_सारणी_आधारः("APIC"));
		यदि (madt_hdr == रिक्त)
			प्रत्यागम रिक्त;
	}

	ret = (संरचना acpi_madt_ioapic *)
		acpi_madt_get_typed_item(madt_hdr, ACPI_MADT_TYPE_IOAPIC, idx);
	यदि (ret)
		idx++;

	प्रत्यागम ret;
}

संरचना acpi_madt_lapic * acpi_get_lapic_अग्र(शून्य)
{
	स्थिर अचिह्नित idx = 0;
	स्थिर संरचना acpi_madt_hdr * madt_hdr;

	संरचना acpi_madt_lapic * ret;

	यदि (idx == 0) {
		madt_hdr = (संरचना acpi_madt_hdr *)
			acpi_phys2vir(acpi_get_सारणी_आधारः("APIC"));
		यदि (madt_hdr == रिक्त)
			प्रत्यागम रिक्त;
	}

	प्रति (;;) {
		ret = (संरचना acpi_madt_lapic *)
			acpi_madt_get_typed_item(madt_hdr,
					ACPI_MADT_TYPE_LAPIC, idx);
		यदि (!ret)
			भङ्ग;

		idx++;

		/* report only usable CPUs */
		यदि (ret->flags & 1)
			भङ्ग;
	}

	प्रत्यागम ret;
}

शून्य __k_unpaged_acpi_poweroff(शून्य)
{
	/* NO OP poweroff symbol*/
}

शून्य acpi_poweroff(शून्य)
{
	यदि (pm1a_cnt_blk == 0) {
		प्रत्यागम;
	}
	outw(pm1a_cnt_blk, slp_typa | SLP_EN_संहिता);
	यदि (pm1b_cnt_blk != 0) {
		outw(pm1b_cnt_blk, slp_typb | SLP_EN_संहिता);
	}
}
