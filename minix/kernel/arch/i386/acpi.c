
#समावेश <string.h>

#समावेश "acpi.h"
#समावेश "arch_proto.h"

प्रकार पूर्णांक ((* acpi_read_t)(phys_bytes addr, व्योम * buff, माप_प्रकार आकार));

काष्ठा acpi_rsdp acpi_rsdp;

अटल acpi_read_t read_func;

#घोषणा MAX_RSDT	35 /* ACPI defines 35 signatures */
#घोषणा SLP_EN_CODE	(1 << 13) /* ACPI SLP_EN_CODE code */
#घोषणा AMI_PACKAGE_OP_CODE (0x12)
#घोषणा AMI_NAME_OP_CODE (0x8)
#घोषणा AMI_BYTE_PREFIX_CODE (0xA)
#घोषणा AMI_PACKAGE_लंबाई_ENCODING_BITS_MASK (0xC0)
#घोषणा AMI_PACKAGE_लंबाई_ENCODING_BITS_SHIFT (6)
#घोषणा AMI_MIN_PACKAGE_लंबाई (1)
#घोषणा AMI_अंक_ELEMENTS_लंबाई (1)
#घोषणा AMI_SLP_TYPA_SHIFT (10)
#घोषणा AMI_SLP_TYPB_SHIFT (10)
#घोषणा AMI_S5_NAME_OP_OFFSET_1 (-1)
#घोषणा AMI_S5_NAME_OP_OFFSET_2 (-2)
#घोषणा AMI_S5_PACKAGE_OP_OFFSET (4)
#घोषणा AMI_S5_PACKET_लंबाई_OFFSET (5)

अटल काष्ठा acpi_rsdt {
	काष्ठा acpi_sdt_header	hdr;
	u32_t			तथ्य[MAX_RSDT];
} rsdt;
	
अटल काष्ठा {
	अक्षर	signature [ACPI_SDT_SIGNATURE_LEN + 1];
	माप_प्रकार	लंबाई;
} sdt_trans[MAX_RSDT];

अटल पूर्णांक sdt_गिनती;
अटल u16_t pm1a_cnt_blk = 0;
अटल u16_t pm1b_cnt_blk = 0;
अटल u16_t slp_typa = 0;
अटल u16_t slp_typb = 0;

अटल पूर्णांक acpi_check_csum(काष्ठा acpi_sdt_header * tb, माप_प्रकार आकार)
{
	u8_t योग = 0;
	पूर्णांक i;
	क्रम (i = 0; i < आकार; i++)
		योग += ((अचिन्हित अक्षर *)tb)[i];
	वापस योग == 0 ? 0 : -1;
}

अटल पूर्णांक acpi_check_signature(स्थिर अक्षर * orig, स्थिर अक्षर * match)
{
	वापस म_भेदन(orig, match, ACPI_SDT_SIGNATURE_LEN);
}

अटल u32_t acpi_phys2vir(u32_t p)
{
	अगर(!vm_running) {
		DEBUGEXTRA(("acpi: returning 0x%lx as vir addr\n", p));
		वापस p;
	}
	panic("acpi: can't get virtual address of arbitrary physical address");
}

अटल पूर्णांक acpi_phys_copy(phys_bytes phys, व्योम *target, माप_प्रकार len)
{
	अगर(!vm_running) {
		स_नकल(target, (व्योम *) phys, len);
		वापस 0;
	}
	panic("can't acpi_phys_copy with vm");
}

अटल पूर्णांक acpi_read_sdt_at(phys_bytes addr,
				काष्ठा acpi_sdt_header * tb,
				माप_प्रकार आकार,
				स्थिर अक्षर * name)
{
	काष्ठा acpi_sdt_header hdr;

	/* if NULL is supplied, we only return the size of the table */
	अगर (tb == शून्य) {
		अगर (read_func(addr, &hdr, माप(काष्ठा acpi_sdt_header))) {
			म_लिखो("ERROR acpi cannot read %s header\n", name);
			वापस -1;
		}

		वापस hdr.लंबाई;
	}

	अगर (read_func(addr, tb, माप(काष्ठा acpi_sdt_header))) {
		म_लिखो("ERROR acpi cannot read %s header\n", name);
		वापस -1;
	}

	अगर (acpi_check_signature(tb->signature, name)) {
		म_लिखो("ERROR acpi %s signature does not match\n", name);
		वापस -1;
	}

	अगर (आकार < tb->लंबाई) {
		म_लिखो("ERROR acpi buffer too small for %s\n", name);
		वापस -1;
	}

	अगर (read_func(addr, tb, आकार)) {
		म_लिखो("ERROR acpi cannot read %s\n", name);
		वापस -1;
	}

	अगर (acpi_check_csum(tb, tb->लंबाई)) {
		म_लिखो("ERROR acpi %s checksum does not match\n", name);
		वापस -1;
	}

	वापस tb->लंबाई;
}

phys_bytes acpi_get_table_base(स्थिर अक्षर * name)
{
	पूर्णांक i;

	क्रम(i = 0; i < sdt_गिनती; i++) {
		अगर (म_भेदन(name, sdt_trans[i].signature,
					ACPI_SDT_SIGNATURE_LEN) == 0)
			वापस (phys_bytes) rsdt.तथ्य[i];
	}

	वापस (phys_bytes) शून्य;
}

माप_प्रकार acpi_get_table_लंबाई(स्थिर अक्षर * name)
{
	पूर्णांक i;

	क्रम(i = 0; i < sdt_गिनती; i++) {
		अगर (म_भेदन(name, sdt_trans[i].signature,
					ACPI_SDT_SIGNATURE_LEN) == 0)
			वापस sdt_trans[i].लंबाई;
	}

	वापस 0;
}

अटल व्योम * acpi_madt_get_typed_item(काष्ठा acpi_madt_hdr * hdr,
					अचिन्हित अक्षर type,
					अचिन्हित idx)
{
	u8_t * t, * end;
	पूर्णांक i;

	t = (u8_t *) hdr + माप(काष्ठा acpi_madt_hdr);
	end = (u8_t *) hdr + hdr->hdr.लंबाई;

	i = 0;
	जबतक(t < end) {
		अगर (type == ((काष्ठा acpi_madt_item_hdr *) t)->type) {
			अगर (i == idx)
				वापस t;
			अन्यथा
				i++;
		}
		t += ((काष्ठा acpi_madt_item_hdr *) t)->लंबाई;
	}

	वापस शून्य;
}

#अगर 0
अटल व्योम * acpi_madt_get_item(काष्ठा acpi_madt_hdr * hdr,
				अचिन्हित idx)
{
	u8_t * t, * end;
	पूर्णांक i;

	t = (u8_t *) hdr + माप(काष्ठा acpi_madt_hdr);
	end = (u8_t *) hdr + hdr->hdr.लंबाई;

	क्रम(i = 0 ; i <= idx && t < end; i++) {
		अगर (i == idx)
			वापस t;
		t += ((काष्ठा acpi_madt_item_hdr *) t)->लंबाई;
	}

	वापस शून्य;
}
#पूर्ण_अगर

अटल पूर्णांक acpi_rsdp_test(व्योम * buff)
{
	काष्ठा acpi_rsdp * rsdp = (काष्ठा acpi_rsdp *) buff;

	अगर (!platform_tbl_checksum_ok(buff, 20))
		वापस 0;
	अगर (म_भेदन(rsdp->signature, "RSD PTR ", 8))
		वापस 0;

	वापस 1;
}

अटल पूर्णांक get_acpi_rsdp(व्योम)
{
	u16_t ebda;
	/*
	 * Read 40:0Eh - to find the starting address of the EBDA.
	 */
	acpi_phys_copy (0x40E, &ebda, माप(ebda));
	अगर (ebda) {
		ebda <<= 4;
		अगर(platform_tbl_ptr(ebda, ebda + 0x400, 16, &acpi_rsdp,
					माप(acpi_rsdp), &machine.acpi_rsdp,
					acpi_rsdp_test))
			वापस 1;
	} 

	/* try BIOS read only mem space */
	अगर(platform_tbl_ptr(0xE0000, 0x100000, 16, &acpi_rsdp,
				माप(acpi_rsdp), &machine.acpi_rsdp,
				acpi_rsdp_test))
		वापस 1;
	
	machine.acpi_rsdp = 0; /* RSDP cannot be found at this address therefore
				  it is a valid negative value */
	वापस 0;
}

अटल व्योम acpi_init_poweroff(व्योम)
{
	u8_t *ptr = शून्य;
	u8_t *start = शून्य;
	u8_t *end = शून्य;
	काष्ठा acpi_fadt_header *fadt_header = शून्य;
	काष्ठा acpi_rsdt * dsdt_header = शून्य;
	अक्षर *संदेश = शून्य;

	/* Everything used here existed since ACPI spec 1.0 */
	/* So we can safely use them */
	fadt_header = (काष्ठा acpi_fadt_header *)
		acpi_phys2vir(acpi_get_table_base("FACP"));
	अगर (fadt_header == शून्य) {
		संदेश = "Could not load FACP";
		जाओ निकास;
	}

	dsdt_header = (काष्ठा acpi_rsdt *)
		acpi_phys2vir((phys_bytes) fadt_header->dsdt);
	अगर (dsdt_header == शून्य) {
		संदेश = "Could not load DSDT";
		जाओ निकास;
	}

	pm1a_cnt_blk = fadt_header->pm1a_cnt_blk;
	pm1b_cnt_blk = fadt_header->pm1b_cnt_blk;

	ptr = start = (u8_t *) dsdt_header->तथ्य;
	end = start + dsdt_header->hdr.लंबाई - 4;

	/* See http://forum.osdev.org/viewtopic.php?t=16990 */
	/* for layout of \_S5 */
	जबतक (ptr < end && स_भेद(ptr, "_S5_", 4) != 0)
		ptr++;

	संदेश = "Could not read S5 data. Use default SLP_TYPa and SLP_TYPb";
	अगर (ptr >= end || ptr == start)
		जाओ निकास;

	/* validate AML structure */
	अगर (*(ptr + AMI_S5_PACKAGE_OP_OFFSET) != AMI_PACKAGE_OP_CODE)
		जाओ निकास;

	अगर ((ptr < start + (-AMI_S5_NAME_OP_OFFSET_2) ||
		(*(ptr + AMI_S5_NAME_OP_OFFSET_2) != AMI_NAME_OP_CODE ||
		 *(ptr + AMI_S5_NAME_OP_OFFSET_2 + 1) != '\\')) &&
		*(ptr + AMI_S5_NAME_OP_OFFSET_1) != AMI_NAME_OP_CODE)
		जाओ निकास;

	ptr += AMI_S5_PACKET_लंबाई_OFFSET;
	अगर (ptr >= end)
		जाओ निकास;

	/* package length */
	ptr += ((*ptr & AMI_PACKAGE_लंबाई_ENCODING_BITS_MASK) >>
		AMI_PACKAGE_लंबाई_ENCODING_BITS_SHIFT) +
		AMI_MIN_PACKAGE_लंबाई + AMI_अंक_ELEMENTS_लंबाई;
	अगर (ptr >= end)
		जाओ निकास;

	अगर (*ptr == AMI_BYTE_PREFIX_CODE)
		ptr++; /* skip byte prefix */

	slp_typa = (*ptr) << AMI_SLP_TYPA_SHIFT;

	ptr++; /* move to SLP_TYPb */
	अगर (*ptr == AMI_BYTE_PREFIX_CODE)
		ptr++; /* skip byte prefix */

	slp_typb = (*ptr) << AMI_SLP_TYPB_SHIFT;

	संदेश = "poweroff initialized";

निकास:
	अगर (संदेश) {
		DEBUGBASIC(("acpi: %s\n", संदेश));
	}
}

व्योम acpi_init(व्योम)
{
	पूर्णांक s, i;
	read_func = acpi_phys_copy;

	अगर (!get_acpi_rsdp()) {
		म_लिखो("WARNING : Cannot configure ACPI\n");
		वापस;
	}
	
	s = acpi_read_sdt_at(acpi_rsdp.rsdt_addr, (काष्ठा acpi_sdt_header *) &rsdt,
			माप(काष्ठा acpi_rsdt), ACPI_SDT_SIGNATURE(RSDT));

	sdt_गिनती = (s - माप(काष्ठा acpi_sdt_header)) / माप(u32_t);

	क्रम (i = 0; i < sdt_गिनती; i++) {
		काष्ठा acpi_sdt_header hdr;
		पूर्णांक j;
		अगर (read_func(rsdt.तथ्य[i], &hdr, माप(काष्ठा acpi_sdt_header))) {
			म_लिखो("ERROR acpi cannot read header at 0x%x\n",
								rsdt.तथ्य[i]);
			वापस;
		}

		क्रम (j = 0 ; j < ACPI_SDT_SIGNATURE_LEN; j++)
			sdt_trans[i].signature[j] = hdr.signature[j];
		sdt_trans[i].signature[ACPI_SDT_SIGNATURE_LEN] = '\0';
		sdt_trans[i].लंबाई = hdr.लंबाई;
	}

	acpi_init_poweroff();
}

काष्ठा acpi_madt_ioapic * acpi_get_ioapic_अग्र(व्योम)
{
	अटल अचिन्हित idx = 0;
	अटल काष्ठा acpi_madt_hdr * madt_hdr;

	काष्ठा acpi_madt_ioapic * ret;

	अगर (idx == 0) {
		madt_hdr = (काष्ठा acpi_madt_hdr *)
			acpi_phys2vir(acpi_get_table_base("APIC"));
		अगर (madt_hdr == शून्य)
			वापस शून्य;
	}

	ret = (काष्ठा acpi_madt_ioapic *)
		acpi_madt_get_typed_item(madt_hdr, ACPI_MADT_TYPE_IOAPIC, idx);
	अगर (ret)
		idx++;

	वापस ret;
}

काष्ठा acpi_madt_lapic * acpi_get_lapic_अग्र(व्योम)
{
	अटल अचिन्हित idx = 0;
	अटल काष्ठा acpi_madt_hdr * madt_hdr;

	काष्ठा acpi_madt_lapic * ret;

	अगर (idx == 0) {
		madt_hdr = (काष्ठा acpi_madt_hdr *)
			acpi_phys2vir(acpi_get_table_base("APIC"));
		अगर (madt_hdr == शून्य)
			वापस शून्य;
	}

	क्रम (;;) {
		ret = (काष्ठा acpi_madt_lapic *)
			acpi_madt_get_typed_item(madt_hdr,
					ACPI_MADT_TYPE_LAPIC, idx);
		अगर (!ret)
			अवरोध;

		idx++;

		/* report only usable CPUs */
		अगर (ret->flags & 1)
			अवरोध;
	}

	वापस ret;
}

व्योम __k_unpaged_acpi_poweroff(व्योम)
{
	/* NO OP poweroff symbol*/
}

व्योम acpi_poweroff(व्योम)
{
	अगर (pm1a_cnt_blk == 0) {
		वापस;
	}
	outw(pm1a_cnt_blk, slp_typa | SLP_EN_CODE);
	अगर (pm1b_cnt_blk != 0) {
		outw(pm1b_cnt_blk, slp_typb | SLP_EN_CODE);
	}
}
