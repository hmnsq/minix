
#అన్తర్భావ <string.h>

#అన్తర్భావ "acpi.h"
#అన్తర్భావ "arch_proto.h"

రకనిర్వచనం పూర్ణాంకం ((* acpi_read_t)(phys_బైట్లు addr, శూన్య * buff, size_t కొలత));

నిర్మాణం acpi_rsdp acpi_rsdp;

స్థిర acpi_read_t read_func;

#నిర్వచించు MAX_RSDT	35 /* ACPI defines 35 signatures */
#నిర్వచించు SLP_EN_సంకేతం	(1 << 13) /* ACPI SLP_EN_CODE code */
#నిర్వచించు AMI_PACKAGE_OP_సంకేతం (0x12)
#నిర్వచించు AMI_NAME_OP_సంకేతం (0x8)
#నిర్వచించు AMI_BYTE_ఉపసర్గ_సంకేతం (0xA)
#నిర్వచించు AMI_PACKAGE_నిడివి_ENCODING_బిట్లు_ముసుగు (0xC0)
#నిర్వచించు AMI_PACKAGE_నిడివి_ENCODING_బిట్లు_జరుపు (6)
#నిర్వచించు AMI_MIN_PACKAGE_నిడివి (1)
#నిర్వచించు AMI_అంకె_ELEMENTS_నిడివి (1)
#నిర్వచించు AMI_SLP_TYPA_జరుపు (10)
#నిర్వచించు AMI_SLP_TYPB_జరుపు (10)
#నిర్వచించు AMI_S5_NAME_OP_స్థానభ్రంశం_1 (-1)
#నిర్వచించు AMI_S5_NAME_OP_స్థానభ్రంశం_2 (-2)
#నిర్వచించు AMI_S5_PACKAGE_OP_స్థానభ్రంశం (4)
#నిర్వచించు AMI_S5_PACKET_నిడివి_స్థానభ్రంశం (5)

స్థిర నిర్మాణం acpi_rsdt {
	నిర్మాణం acpi_sdt_శీర్షిక	hdr;
	u32_t			దత్తాంశం[MAX_RSDT];
} rsdt;
	
స్థిర నిర్మాణం {
	అక్షరం	signature [ACPI_SDT_SIGNATURE_LEN + 1];
	size_t	నిడివి;
} sdt_trans[MAX_RSDT];

స్థిర పూర్ణాంకం sdt_లెక్క;
స్థిర u16_t pm1a_cnt_blk = 0;
స్థిర u16_t pm1b_cnt_blk = 0;
స్థిర u16_t slp_typa = 0;
స్థిర u16_t slp_typb = 0;

స్థిర పూర్ణాంకం acpi_check_csum(నిర్మాణం acpi_sdt_శీర్షిక * tb, size_t కొలత)
{
	u8_t మొత్తం = 0;
	పూర్ణాంకం i;
	కొరకు (i = 0; i < కొలత; i++)
		మొత్తం += ((గుర్తులేని అక్షరం *)tb)[i];
	తిరిగి మొత్తం == 0 ? 0 : -1;
}

స్థిర పూర్ణాంకం acpi_check_signature(స్థిరరాశి అక్షరం * orig, స్థిరరాశి అక్షరం * సరిపోలిక)
{
	తిరిగి అక్షరశ్రేణి_పోలిక(orig, సరిపోలిక, ACPI_SDT_SIGNATURE_LEN);
}

స్థిర u32_t acpi_phys2vir(u32_t p)
{
	ఒకవేళ(!vm_running) {
		DEBUGEXTRA(("acpi: returning 0x%lx as vir addr\n", p));
		తిరిగి p;
	}
	panic("acpi: can't get virtual address of arbitrary physical address");
}

స్థిర పూర్ణాంకం acpi_phys_copy(phys_బైట్లు phys, శూన్య *లక్ష్యం, size_t len)
{
	ఒకవేళ(!vm_running) {
		జ్ఞాపకం_నకలు(లక్ష్యం, (శూన్య *) phys, len);
		తిరిగి 0;
	}
	panic("can't acpi_phys_copy with vm");
}

స్థిర పూర్ణాంకం acpi_read_sdt_at(phys_బైట్లు addr,
				నిర్మాణం acpi_sdt_శీర్షిక * tb,
				size_t కొలత,
				స్థిరరాశి అక్షరం * name)
{
	నిర్మాణం acpi_sdt_శీర్షిక hdr;

	/* if NULL is supplied, we only return the size of the table */
	ఒకవేళ (tb == ఖాళీ) {
		ఒకవేళ (read_func(addr, &hdr, పరిమాణం(నిర్మాణం acpi_sdt_శీర్షిక))) {
			రాయి("ERROR acpi cannot read %s header\n", name);
			తిరిగి -1;
		}

		తిరిగి hdr.నిడివి;
	}

	ఒకవేళ (read_func(addr, tb, పరిమాణం(నిర్మాణం acpi_sdt_శీర్షిక))) {
		రాయి("ERROR acpi cannot read %s header\n", name);
		తిరిగి -1;
	}

	ఒకవేళ (acpi_check_signature(tb->signature, name)) {
		రాయి("ERROR acpi %s signature does not match\n", name);
		తిరిగి -1;
	}

	ఒకవేళ (కొలత < tb->నిడివి) {
		రాయి("ERROR acpi buffer too small for %s\n", name);
		తిరిగి -1;
	}

	ఒకవేళ (read_func(addr, tb, కొలత)) {
		రాయి("ERROR acpi cannot read %s\n", name);
		తిరిగి -1;
	}

	ఒకవేళ (acpi_check_csum(tb, tb->నిడివి)) {
		రాయి("ERROR acpi %s checksum does not match\n", name);
		తిరిగి -1;
	}

	తిరిగి tb->నిడివి;
}

phys_బైట్లు acpi_get_పట్టిక_పునాది(స్థిరరాశి అక్షరం * name)
{
	పూర్ణాంకం i;

	కొరకు(i = 0; i < sdt_లెక్క; i++) {
		ఒకవేళ (అక్షరశ్రేణి_పోలిక(name, sdt_trans[i].signature,
					ACPI_SDT_SIGNATURE_LEN) == 0)
			తిరిగి (phys_బైట్లు) rsdt.దత్తాంశం[i];
	}

	తిరిగి (phys_బైట్లు) ఖాళీ;
}

size_t acpi_get_పట్టిక_నిడివి(స్థిరరాశి అక్షరం * name)
{
	పూర్ణాంకం i;

	కొరకు(i = 0; i < sdt_లెక్క; i++) {
		ఒకవేళ (అక్షరశ్రేణి_పోలిక(name, sdt_trans[i].signature,
					ACPI_SDT_SIGNATURE_LEN) == 0)
			తిరిగి sdt_trans[i].నిడివి;
	}

	తిరిగి 0;
}

స్థిర శూన్య * acpi_madt_get_typed_అంశం(నిర్మాణం acpi_madt_hdr * hdr,
					గుర్తులేని అక్షరం type,
					గుర్తులేని idx)
{
	u8_t * t, * చివర;
	పూర్ణాంకం i;

	t = (u8_t *) hdr + పరిమాణం(నిర్మాణం acpi_madt_hdr);
	చివర = (u8_t *) hdr + hdr->hdr.నిడివి;

	i = 0;
	అయితే(t < చివర) {
		ఒకవేళ (type == ((నిర్మాణం acpi_madt_అంశం_hdr *) t)->type) {
			ఒకవేళ (i == idx)
				తిరిగి t;
			లేకపోతే
				i++;
		}
		t += ((నిర్మాణం acpi_madt_అంశం_hdr *) t)->నిడివి;
	}

	తిరిగి ఖాళీ;
}

#ఒకవేళ 0
స్థిర శూన్య * acpi_madt_get_అంశం(నిర్మాణం acpi_madt_hdr * hdr,
				గుర్తులేని idx)
{
	u8_t * t, * చివర;
	పూర్ణాంకం i;

	t = (u8_t *) hdr + పరిమాణం(నిర్మాణం acpi_madt_hdr);
	చివర = (u8_t *) hdr + hdr->hdr.నిడివి;

	కొరకు(i = 0 ; i <= idx && t < చివర; i++) {
		ఒకవేళ (i == idx)
			తిరిగి t;
		t += ((నిర్మాణం acpi_madt_అంశం_hdr *) t)->నిడివి;
	}

	తిరిగి ఖాళీ;
}
#అయితే_ముగింపు

స్థిర పూర్ణాంకం acpi_rsdp_test(శూన్య * buff)
{
	నిర్మాణం acpi_rsdp * rsdp = (నిర్మాణం acpi_rsdp *) buff;

	ఒకవేళ (!platform_tbl_checksum_ok(buff, 20))
		తిరిగి 0;
	ఒకవేళ (అక్షరశ్రేణి_పోలిక(rsdp->signature, "RSD PTR ", 8))
		తిరిగి 0;

	తిరిగి 1;
}

స్థిర పూర్ణాంకం get_acpi_rsdp(శూన్య)
{
	u16_t ebda;
	/*
	 * Read 40:0Eh - to find the starting address of the EBDA.
	 */
	acpi_phys_copy (0x40E, &ebda, పరిమాణం(ebda));
	ఒకవేళ (ebda) {
		ebda <<= 4;
		ఒకవేళ(platform_tbl_ptr(ebda, ebda + 0x400, 16, &acpi_rsdp,
					పరిమాణం(acpi_rsdp), &machine.acpi_rsdp,
					acpi_rsdp_test))
			తిరిగి 1;
	} 

	/* try BIOS read only mem space */
	ఒకవేళ(platform_tbl_ptr(0xE0000, 0x100000, 16, &acpi_rsdp,
				పరిమాణం(acpi_rsdp), &machine.acpi_rsdp,
				acpi_rsdp_test))
		తిరిగి 1;
	
	machine.acpi_rsdp = 0; /* RSDP cannot be found at this address therefore
				  it is a valid negative value */
	తిరిగి 0;
}

స్థిర శూన్య acpi_init_poweroff(శూన్య)
{
	u8_t *ptr = ఖాళీ;
	u8_t *ప్రారంభం = ఖాళీ;
	u8_t *చివర = ఖాళీ;
	నిర్మాణం acpi_fadt_శీర్షిక *fadt_శీర్షిక = ఖాళీ;
	నిర్మాణం acpi_rsdt * dsdt_శీర్షిక = ఖాళీ;
	అక్షరం *సందేశం_2 = ఖాళీ;

	/* Everything used here existed since ACPI spec 1.0 */
	/* So we can safely use them */
	fadt_శీర్షిక = (నిర్మాణం acpi_fadt_శీర్షిక *)
		acpi_phys2vir(acpi_get_పట్టిక_పునాది("FACP"));
	ఒకవేళ (fadt_శీర్షిక == ఖాళీ) {
		సందేశం_2 = "Could not load FACP";
		వెళ్ళు నిష్క్రమించు;
	}

	dsdt_శీర్షిక = (నిర్మాణం acpi_rsdt *)
		acpi_phys2vir((phys_బైట్లు) fadt_శీర్షిక->dsdt);
	ఒకవేళ (dsdt_శీర్షిక == ఖాళీ) {
		సందేశం_2 = "Could not load DSDT";
		వెళ్ళు నిష్క్రమించు;
	}

	pm1a_cnt_blk = fadt_శీర్షిక->pm1a_cnt_blk;
	pm1b_cnt_blk = fadt_శీర్షిక->pm1b_cnt_blk;

	ptr = ప్రారంభం = (u8_t *) dsdt_శీర్షిక->దత్తాంశం;
	చివర = ప్రారంభం + dsdt_శీర్షిక->hdr.నిడివి - 4;

	/* See http://forum.osdev.org/viewtopic.php?t=16990 */
	/* for layout of \_S5 */
	అయితే (ptr < చివర && memcmp(ptr, "_S5_", 4) != 0)
		ptr++;

	సందేశం_2 = "Could not read S5 data. Use default SLP_TYPa and SLP_TYPb";
	ఒకవేళ (ptr >= చివర || ptr == ప్రారంభం)
		వెళ్ళు నిష్క్రమించు;

	/* validate AML structure */
	ఒకవేళ (*(ptr + AMI_S5_PACKAGE_OP_స్థానభ్రంశం) != AMI_PACKAGE_OP_సంకేతం)
		వెళ్ళు నిష్క్రమించు;

	ఒకవేళ ((ptr < ప్రారంభం + (-AMI_S5_NAME_OP_స్థానభ్రంశం_2) ||
		(*(ptr + AMI_S5_NAME_OP_స్థానభ్రంశం_2) != AMI_NAME_OP_సంకేతం ||
		 *(ptr + AMI_S5_NAME_OP_స్థానభ్రంశం_2 + 1) != '\\')) &&
		*(ptr + AMI_S5_NAME_OP_స్థానభ్రంశం_1) != AMI_NAME_OP_సంకేతం)
		వెళ్ళు నిష్క్రమించు;

	ptr += AMI_S5_PACKET_నిడివి_స్థానభ్రంశం;
	ఒకవేళ (ptr >= చివర)
		వెళ్ళు నిష్క్రమించు;

	/* package length */
	ptr += ((*ptr & AMI_PACKAGE_నిడివి_ENCODING_బిట్లు_ముసుగు) >>
		AMI_PACKAGE_నిడివి_ENCODING_బిట్లు_జరుపు) +
		AMI_MIN_PACKAGE_నిడివి + AMI_అంకె_ELEMENTS_నిడివి;
	ఒకవేళ (ptr >= చివర)
		వెళ్ళు నిష్క్రమించు;

	ఒకవేళ (*ptr == AMI_BYTE_ఉపసర్గ_సంకేతం)
		ptr++; /* skip byte prefix */

	slp_typa = (*ptr) << AMI_SLP_TYPA_జరుపు;

	ptr++; /* move to SLP_TYPb */
	ఒకవేళ (*ptr == AMI_BYTE_ఉపసర్గ_సంకేతం)
		ptr++; /* skip byte prefix */

	slp_typb = (*ptr) << AMI_SLP_TYPB_జరుపు;

	సందేశం_2 = "poweroff initialized";

నిష్క్రమించు:
	ఒకవేళ (సందేశం_2) {
		DEBUGBASIC(("acpi: %s\n", సందేశం_2));
	}
}

శూన్య acpi_init(శూన్య)
{
	పూర్ణాంకం s, i;
	read_func = acpi_phys_copy;

	ఒకవేళ (!get_acpi_rsdp()) {
		రాయి("WARNING : Cannot configure ACPI\n");
		తిరిగి;
	}
	
	s = acpi_read_sdt_at(acpi_rsdp.rsdt_addr, (నిర్మాణం acpi_sdt_శీర్షిక *) &rsdt,
			పరిమాణం(నిర్మాణం acpi_rsdt), ACPI_SDT_SIGNATURE(RSDT));

	sdt_లెక్క = (s - పరిమాణం(నిర్మాణం acpi_sdt_శీర్షిక)) / పరిమాణం(u32_t);

	కొరకు (i = 0; i < sdt_లెక్క; i++) {
		నిర్మాణం acpi_sdt_శీర్షిక hdr;
		పూర్ణాంకం j;
		ఒకవేళ (read_func(rsdt.దత్తాంశం[i], &hdr, పరిమాణం(నిర్మాణం acpi_sdt_శీర్షిక))) {
			రాయి("ERROR acpi cannot read header at 0x%x\n",
								rsdt.దత్తాంశం[i]);
			తిరిగి;
		}

		కొరకు (j = 0 ; j < ACPI_SDT_SIGNATURE_LEN; j++)
			sdt_trans[i].signature[j] = hdr.signature[j];
		sdt_trans[i].signature[ACPI_SDT_SIGNATURE_LEN] = '\0';
		sdt_trans[i].నిడివి = hdr.నిడివి;
	}

	acpi_init_poweroff();
}

నిర్మాణం acpi_madt_ioapic * acpi_get_ioapic_తదుపరి(శూన్య)
{
	స్థిర గుర్తులేని idx = 0;
	స్థిర నిర్మాణం acpi_madt_hdr * madt_hdr;

	నిర్మాణం acpi_madt_ioapic * ret;

	ఒకవేళ (idx == 0) {
		madt_hdr = (నిర్మాణం acpi_madt_hdr *)
			acpi_phys2vir(acpi_get_పట్టిక_పునాది("APIC"));
		ఒకవేళ (madt_hdr == ఖాళీ)
			తిరిగి ఖాళీ;
	}

	ret = (నిర్మాణం acpi_madt_ioapic *)
		acpi_madt_get_typed_అంశం(madt_hdr, ACPI_MADT_TYPE_IOAPIC, idx);
	ఒకవేళ (ret)
		idx++;

	తిరిగి ret;
}

నిర్మాణం acpi_madt_lapic * acpi_get_lapic_తదుపరి(శూన్య)
{
	స్థిర గుర్తులేని idx = 0;
	స్థిర నిర్మాణం acpi_madt_hdr * madt_hdr;

	నిర్మాణం acpi_madt_lapic * ret;

	ఒకవేళ (idx == 0) {
		madt_hdr = (నిర్మాణం acpi_madt_hdr *)
			acpi_phys2vir(acpi_get_పట్టిక_పునాది("APIC"));
		ఒకవేళ (madt_hdr == ఖాళీ)
			తిరిగి ఖాళీ;
	}

	కొరకు (;;) {
		ret = (నిర్మాణం acpi_madt_lapic *)
			acpi_madt_get_typed_అంశం(madt_hdr,
					ACPI_MADT_TYPE_LAPIC, idx);
		ఒకవేళ (!ret)
			ఆపు;

		idx++;

		/* report only usable CPUs */
		ఒకవేళ (ret->flags & 1)
			ఆపు;
	}

	తిరిగి ret;
}

శూన్య __k_unpaged_acpi_poweroff(శూన్య)
{
	/* NO OP poweroff symbol*/
}

శూన్య acpi_poweroff(శూన్య)
{
	ఒకవేళ (pm1a_cnt_blk == 0) {
		తిరిగి;
	}
	outw(pm1a_cnt_blk, slp_typa | SLP_EN_సంకేతం);
	ఒకవేళ (pm1b_cnt_blk != 0) {
		outw(pm1b_cnt_blk, slp_typb | SLP_EN_సంకేతం);
	}
}
