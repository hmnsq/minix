
#شامل <string.h>

#شامل "acpi.h"
#شامل "arch_proto.h"

قسمتعریف صحیح ((* acpi_read_t)(phys_بائٹیں addr, خالی * buff, size_t سائز));

ساخت acpi_rsdp acpi_rsdp;

ساکن acpi_read_t read_func;

#تعریف_کرو MAX_RSDT	35 /* ACPI defines 35 signatures */
#تعریف_کرو SLP_EN_رمز	(1 << 13) /* ACPI SLP_EN_CODE code */
#تعریف_کرو AMI_PACKAGE_OP_رمز (0x12)
#تعریف_کرو AMI_NAME_OP_رمز (0x8)
#تعریف_کرو AMI_BYTE_سابقہ_رمز (0xA)
#تعریف_کرو AMI_PACKAGE_لمبائی_ENCODING_بٹ_نقاب (0xC0)
#تعریف_کرو AMI_PACKAGE_لمبائی_ENCODING_بٹ_سرکاؤ (6)
#تعریف_کرو AMI_MIN_PACKAGE_لمبائی (1)
#تعریف_کرو AMI_عدد_ELEMENTS_لمبائی (1)
#تعریف_کرو AMI_SLP_TYPA_سرکاؤ (10)
#تعریف_کرو AMI_SLP_TYPB_سرکاؤ (10)
#تعریف_کرو AMI_S5_NAME_OP_انحراف_1 (-1)
#تعریف_کرو AMI_S5_NAME_OP_انحراف_2 (-2)
#تعریف_کرو AMI_S5_PACKAGE_OP_انحراف (4)
#تعریف_کرو AMI_S5_PACKET_لمبائی_انحراف (5)

ساکن ساخت acpi_rsdt {
	ساخت acpi_sdt_سرخی	hdr;
	u32_t			ڈیٹا[MAX_RSDT];
} rsdt;
	
ساکن ساخت {
	حرف	signature [ACPI_SDT_SIGNATURE_LEN + 1];
	size_t	لمبائی;
} sdt_trans[MAX_RSDT];

ساکن صحیح sdt_گنتی;
ساکن u16_t pm1a_cnt_blk = 0;
ساکن u16_t pm1b_cnt_blk = 0;
ساکن u16_t slp_typa = 0;
ساکن u16_t slp_typb = 0;

ساکن صحیح acpi_check_csum(ساخت acpi_sdt_سرخی * tb, size_t سائز)
{
	u8_t کل = 0;
	صحیح i;
	برائے (i = 0; i < سائز; i++)
		کل += ((غیرعلامتی حرف *)tb)[i];
	واپس کل == 0 ? 0 : -1;
}

ساکن صحیح acpi_check_signature(ثابت حرف * orig, ثابت حرف * مطابقت)
{
	واپس لڑی_موازنہ(orig, مطابقت, ACPI_SDT_SIGNATURE_LEN);
}

ساکن u32_t acpi_phys2vir(u32_t p)
{
	اگر(!vm_running) {
		DEBUGEXTRA(("acpi: returning 0x%lx as vir addr\n", p));
		واپس p;
	}
	panic("acpi: can't get virtual address of arbitrary physical address");
}

ساکن صحیح acpi_phys_copy(phys_بائٹیں phys, خالی *ہدف, size_t len)
{
	اگر(!vm_running) {
		حافظہ_نقل(ہدف, (خالی *) phys, len);
		واپس 0;
	}
	panic("can't acpi_phys_copy with vm");
}

ساکن صحیح acpi_read_sdt_at(phys_بائٹیں addr,
				ساخت acpi_sdt_سرخی * tb,
				size_t سائز,
				ثابت حرف * name)
{
	ساخت acpi_sdt_سرخی hdr;

	/* if NULL is supplied, we only return the size of the table */
	اگر (tb == تہی) {
		اگر (read_func(addr, &hdr, حجم(ساخت acpi_sdt_سرخی))) {
			لکھو("ERROR acpi cannot read %s header\n", name);
			واپس -1;
		}

		واپس hdr.لمبائی;
	}

	اگر (read_func(addr, tb, حجم(ساخت acpi_sdt_سرخی))) {
		لکھو("ERROR acpi cannot read %s header\n", name);
		واپس -1;
	}

	اگر (acpi_check_signature(tb->signature, name)) {
		لکھو("ERROR acpi %s signature does not match\n", name);
		واپس -1;
	}

	اگر (سائز < tb->لمبائی) {
		لکھو("ERROR acpi buffer too small for %s\n", name);
		واپس -1;
	}

	اگر (read_func(addr, tb, سائز)) {
		لکھو("ERROR acpi cannot read %s\n", name);
		واپس -1;
	}

	اگر (acpi_check_csum(tb, tb->لمبائی)) {
		لکھو("ERROR acpi %s checksum does not match\n", name);
		واپس -1;
	}

	واپس tb->لمبائی;
}

phys_بائٹیں acpi_get_جدول_بنیاد(ثابت حرف * name)
{
	صحیح i;

	برائے(i = 0; i < sdt_گنتی; i++) {
		اگر (لڑی_موازنہ(name, sdt_trans[i].signature,
					ACPI_SDT_SIGNATURE_LEN) == 0)
			واپس (phys_بائٹیں) rsdt.ڈیٹا[i];
	}

	واپس (phys_بائٹیں) تہی;
}

size_t acpi_get_جدول_لمبائی(ثابت حرف * name)
{
	صحیح i;

	برائے(i = 0; i < sdt_گنتی; i++) {
		اگر (لڑی_موازنہ(name, sdt_trans[i].signature,
					ACPI_SDT_SIGNATURE_LEN) == 0)
			واپس sdt_trans[i].لمبائی;
	}

	واپس 0;
}

ساکن خالی * acpi_madt_get_typed_شے(ساخت acpi_madt_hdr * hdr,
					غیرعلامتی حرف type,
					غیرعلامتی idx)
{
	u8_t * t, * انتہا;
	صحیح i;

	t = (u8_t *) hdr + حجم(ساخت acpi_madt_hdr);
	انتہا = (u8_t *) hdr + hdr->hdr.لمبائی;

	i = 0;
	جب(t < انتہا) {
		اگر (type == ((ساخت acpi_madt_شے_hdr *) t)->type) {
			اگر (i == idx)
				واپس t;
			ورنہ
				i++;
		}
		t += ((ساخت acpi_madt_شے_hdr *) t)->لمبائی;
	}

	واپس تہی;
}

#اگر 0
ساکن خالی * acpi_madt_get_شے(ساخت acpi_madt_hdr * hdr,
				غیرعلامتی idx)
{
	u8_t * t, * انتہا;
	صحیح i;

	t = (u8_t *) hdr + حجم(ساخت acpi_madt_hdr);
	انتہا = (u8_t *) hdr + hdr->hdr.لمبائی;

	برائے(i = 0 ; i <= idx && t < انتہا; i++) {
		اگر (i == idx)
			واپس t;
		t += ((ساخت acpi_madt_شے_hdr *) t)->لمبائی;
	}

	واپس تہی;
}
#اگر_ختم

ساکن صحیح acpi_rsdp_test(خالی * buff)
{
	ساخت acpi_rsdp * rsdp = (ساخت acpi_rsdp *) buff;

	اگر (!platform_tbl_checksum_ok(buff, 20))
		واپس 0;
	اگر (لڑی_موازنہ(rsdp->signature, "RSD PTR ", 8))
		واپس 0;

	واپس 1;
}

ساکن صحیح get_acpi_rsdp(خالی)
{
	u16_t ebda;
	/*
	 * Read 40:0Eh - to find the starting address of the EBDA.
	 */
	acpi_phys_copy (0x40E, &ebda, حجم(ebda));
	اگر (ebda) {
		ebda <<= 4;
		اگر(platform_tbl_ptr(ebda, ebda + 0x400, 16, &acpi_rsdp,
					حجم(acpi_rsdp), &machine.acpi_rsdp,
					acpi_rsdp_test))
			واپس 1;
	} 

	/* try BIOS read only mem space */
	اگر(platform_tbl_ptr(0xE0000, 0x100000, 16, &acpi_rsdp,
				حجم(acpi_rsdp), &machine.acpi_rsdp,
				acpi_rsdp_test))
		واپس 1;
	
	machine.acpi_rsdp = 0; /* RSDP cannot be found at this address therefore
				  it is a valid negative value */
	واپس 0;
}

ساکن خالی acpi_init_poweroff(خالی)
{
	u8_t *ptr = تہی;
	u8_t *آغاز = تہی;
	u8_t *انتہا = تہی;
	ساخت acpi_fadt_سرخی *fadt_سرخی = تہی;
	ساخت acpi_rsdt * dsdt_سرخی = تہی;
	حرف *پیغام_2 = تہی;

	/* Everything used here existed since ACPI spec 1.0 */
	/* So we can safely use them */
	fadt_سرخی = (ساخت acpi_fadt_سرخی *)
		acpi_phys2vir(acpi_get_جدول_بنیاد("FACP"));
	اگر (fadt_سرخی == تہی) {
		پیغام_2 = "Could not load FACP";
		جاؤ باہر_نکلو;
	}

	dsdt_سرخی = (ساخت acpi_rsdt *)
		acpi_phys2vir((phys_بائٹیں) fadt_سرخی->dsdt);
	اگر (dsdt_سرخی == تہی) {
		پیغام_2 = "Could not load DSDT";
		جاؤ باہر_نکلو;
	}

	pm1a_cnt_blk = fadt_سرخی->pm1a_cnt_blk;
	pm1b_cnt_blk = fadt_سرخی->pm1b_cnt_blk;

	ptr = آغاز = (u8_t *) dsdt_سرخی->ڈیٹا;
	انتہا = آغاز + dsdt_سرخی->hdr.لمبائی - 4;

	/* See http://forum.osdev.org/viewtopic.php?t=16990 */
	/* for layout of \_S5 */
	جب (ptr < انتہا && memcmp(ptr, "_S5_", 4) != 0)
		ptr++;

	پیغام_2 = "Could not read S5 data. Use default SLP_TYPa and SLP_TYPb";
	اگر (ptr >= انتہا || ptr == آغاز)
		جاؤ باہر_نکلو;

	/* validate AML structure */
	اگر (*(ptr + AMI_S5_PACKAGE_OP_انحراف) != AMI_PACKAGE_OP_رمز)
		جاؤ باہر_نکلو;

	اگر ((ptr < آغاز + (-AMI_S5_NAME_OP_انحراف_2) ||
		(*(ptr + AMI_S5_NAME_OP_انحراف_2) != AMI_NAME_OP_رمز ||
		 *(ptr + AMI_S5_NAME_OP_انحراف_2 + 1) != '\\')) &&
		*(ptr + AMI_S5_NAME_OP_انحراف_1) != AMI_NAME_OP_رمز)
		جاؤ باہر_نکلو;

	ptr += AMI_S5_PACKET_لمبائی_انحراف;
	اگر (ptr >= انتہا)
		جاؤ باہر_نکلو;

	/* package length */
	ptr += ((*ptr & AMI_PACKAGE_لمبائی_ENCODING_بٹ_نقاب) >>
		AMI_PACKAGE_لمبائی_ENCODING_بٹ_سرکاؤ) +
		AMI_MIN_PACKAGE_لمبائی + AMI_عدد_ELEMENTS_لمبائی;
	اگر (ptr >= انتہا)
		جاؤ باہر_نکلو;

	اگر (*ptr == AMI_BYTE_سابقہ_رمز)
		ptr++; /* skip byte prefix */

	slp_typa = (*ptr) << AMI_SLP_TYPA_سرکاؤ;

	ptr++; /* move to SLP_TYPb */
	اگر (*ptr == AMI_BYTE_سابقہ_رمز)
		ptr++; /* skip byte prefix */

	slp_typb = (*ptr) << AMI_SLP_TYPB_سرکاؤ;

	پیغام_2 = "poweroff initialized";

باہر_نکلو:
	اگر (پیغام_2) {
		DEBUGBASIC(("acpi: %s\n", پیغام_2));
	}
}

خالی acpi_init(خالی)
{
	صحیح s, i;
	read_func = acpi_phys_copy;

	اگر (!get_acpi_rsdp()) {
		لکھو("WARNING : Cannot configure ACPI\n");
		واپس;
	}
	
	s = acpi_read_sdt_at(acpi_rsdp.rsdt_addr, (ساخت acpi_sdt_سرخی *) &rsdt,
			حجم(ساخت acpi_rsdt), ACPI_SDT_SIGNATURE(RSDT));

	sdt_گنتی = (s - حجم(ساخت acpi_sdt_سرخی)) / حجم(u32_t);

	برائے (i = 0; i < sdt_گنتی; i++) {
		ساخت acpi_sdt_سرخی hdr;
		صحیح j;
		اگر (read_func(rsdt.ڈیٹا[i], &hdr, حجم(ساخت acpi_sdt_سرخی))) {
			لکھو("ERROR acpi cannot read header at 0x%x\n",
								rsdt.ڈیٹا[i]);
			واپس;
		}

		برائے (j = 0 ; j < ACPI_SDT_SIGNATURE_LEN; j++)
			sdt_trans[i].signature[j] = hdr.signature[j];
		sdt_trans[i].signature[ACPI_SDT_SIGNATURE_LEN] = '\0';
		sdt_trans[i].لمبائی = hdr.لمبائی;
	}

	acpi_init_poweroff();
}

ساخت acpi_madt_ioapic * acpi_get_ioapic_اگلا(خالی)
{
	ساکن غیرعلامتی idx = 0;
	ساکن ساخت acpi_madt_hdr * madt_hdr;

	ساخت acpi_madt_ioapic * ret;

	اگر (idx == 0) {
		madt_hdr = (ساخت acpi_madt_hdr *)
			acpi_phys2vir(acpi_get_جدول_بنیاد("APIC"));
		اگر (madt_hdr == تہی)
			واپس تہی;
	}

	ret = (ساخت acpi_madt_ioapic *)
		acpi_madt_get_typed_شے(madt_hdr, ACPI_MADT_TYPE_IOAPIC, idx);
	اگر (ret)
		idx++;

	واپس ret;
}

ساخت acpi_madt_lapic * acpi_get_lapic_اگلا(خالی)
{
	ساکن غیرعلامتی idx = 0;
	ساکن ساخت acpi_madt_hdr * madt_hdr;

	ساخت acpi_madt_lapic * ret;

	اگر (idx == 0) {
		madt_hdr = (ساخت acpi_madt_hdr *)
			acpi_phys2vir(acpi_get_جدول_بنیاد("APIC"));
		اگر (madt_hdr == تہی)
			واپس تہی;
	}

	برائے (;;) {
		ret = (ساخت acpi_madt_lapic *)
			acpi_madt_get_typed_شے(madt_hdr,
					ACPI_MADT_TYPE_LAPIC, idx);
		اگر (!ret)
			توڑو;

		idx++;

		/* report only usable CPUs */
		اگر (ret->flags & 1)
			توڑو;
	}

	واپس ret;
}

خالی __k_unpaged_acpi_poweroff(خالی)
{
	/* NO OP poweroff symbol*/
}

خالی acpi_poweroff(خالی)
{
	اگر (pm1a_cnt_blk == 0) {
		واپس;
	}
	outw(pm1a_cnt_blk, slp_typa | SLP_EN_رمز);
	اگر (pm1b_cnt_blk != 0) {
		outw(pm1b_cnt_blk, slp_typb | SLP_EN_رمز);
	}
}
