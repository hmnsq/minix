
#ضمن <string.h>

#ضمن "acpi.h"
#ضمن "arch_proto.h"

تعريفنوع صحيح ((* acpi_read_t)(phys_بائيٽون addr, فراغ * buff, size_t سائيز));

بنية acpi_rsdp acpi_rsdp;

ساكن acpi_read_t read_func;

#وصف_ڪريو MAX_RSDT	35 /* ACPI defines 35 signatures */
#وصف_ڪريو SLP_EN_ڪوڊ	(1 << 13) /* ACPI SLP_EN_CODE code */
#وصف_ڪريو AMI_PACKAGE_OP_ڪوڊ (0x12)
#وصف_ڪريو AMI_NAME_OP_ڪوڊ (0x8)
#وصف_ڪريو AMI_BYTE_اڳياڙي_ڪوڊ (0xA)
#وصف_ڪريو AMI_PACKAGE_ڊيگهائي_ENCODING_بٽ_نقاب (0xC0)
#وصف_ڪريو AMI_PACKAGE_ڊيگهائي_ENCODING_بٽ_سرڪايو (6)
#وصف_ڪريو AMI_MIN_PACKAGE_ڊيگهائي (1)
#وصف_ڪريو AMI_انگ_ELEMENTS_ڊيگهائي (1)
#وصف_ڪريو AMI_SLP_TYPA_سرڪايو (10)
#وصف_ڪريو AMI_SLP_TYPB_سرڪايو (10)
#وصف_ڪريو AMI_S5_NAME_OP_وٿي_1 (-1)
#وصف_ڪريو AMI_S5_NAME_OP_وٿي_2 (-2)
#وصف_ڪريو AMI_S5_PACKAGE_OP_وٿي (4)
#وصف_ڪريو AMI_S5_PACKET_ڊيگهائي_وٿي (5)

ساكن بنية acpi_rsdt {
	بنية acpi_sdt_سرو	hdr;
	u32_t			معلومات[MAX_RSDT];
} rsdt;
	
ساكن بنية {
	حرف	signature [ACPI_SDT_SIGNATURE_LEN + 1];
	size_t	ڊيگهائي;
} sdt_trans[MAX_RSDT];

ساكن صحيح sdt_ڳڻپ;
ساكن u16_t pm1a_cnt_blk = 0;
ساكن u16_t pm1b_cnt_blk = 0;
ساكن u16_t slp_typa = 0;
ساكن u16_t slp_typb = 0;

ساكن صحيح acpi_check_csum(بنية acpi_sdt_سرو * tb, size_t سائيز)
{
	u8_t ڪل = 0;
	صحيح i;
	لكل (i = 0; i < سائيز; i++)
		ڪل += ((بنانشان حرف *)tb)[i];
	أرجع ڪل == 0 ? 0 : -1;
}

ساكن صحيح acpi_check_signature(ثابت حرف * orig, ثابت حرف * ميلاپ)
{
	أرجع لڙي_ڀيٽ(orig, ميلاپ, ACPI_SDT_SIGNATURE_LEN);
}

ساكن u32_t acpi_phys2vir(u32_t p)
{
	إذا(!vm_running) {
		DEBUGEXTRA(("acpi: returning 0x%lx as vir addr\n", p));
		أرجع p;
	}
	panic("acpi: can't get virtual address of arbitrary physical address");
}

ساكن صحيح acpi_phys_copy(phys_بائيٽون phys, فراغ *هدف, size_t len)
{
	إذا(!vm_running) {
		يادگيري_نقل(هدف, (فراغ *) phys, len);
		أرجع 0;
	}
	panic("can't acpi_phys_copy with vm");
}

ساكن صحيح acpi_read_sdt_at(phys_بائيٽون addr,
				بنية acpi_sdt_سرو * tb,
				size_t سائيز,
				ثابت حرف * name)
{
	بنية acpi_sdt_سرو hdr;

	/* if NULL is supplied, we only return the size of the table */
	إذا (tb == عدم) {
		إذا (read_func(addr, &hdr, حجم(بنية acpi_sdt_سرو))) {
			لکو("ERROR acpi cannot read %s header\n", name);
			أرجع -1;
		}

		أرجع hdr.ڊيگهائي;
	}

	إذا (read_func(addr, tb, حجم(بنية acpi_sdt_سرو))) {
		لکو("ERROR acpi cannot read %s header\n", name);
		أرجع -1;
	}

	إذا (acpi_check_signature(tb->signature, name)) {
		لکو("ERROR acpi %s signature does not match\n", name);
		أرجع -1;
	}

	إذا (سائيز < tb->ڊيگهائي) {
		لکو("ERROR acpi buffer too small for %s\n", name);
		أرجع -1;
	}

	إذا (read_func(addr, tb, سائيز)) {
		لکو("ERROR acpi cannot read %s\n", name);
		أرجع -1;
	}

	إذا (acpi_check_csum(tb, tb->ڊيگهائي)) {
		لکو("ERROR acpi %s checksum does not match\n", name);
		أرجع -1;
	}

	أرجع tb->ڊيگهائي;
}

phys_بائيٽون acpi_get_جدول_بنياد(ثابت حرف * name)
{
	صحيح i;

	لكل(i = 0; i < sdt_ڳڻپ; i++) {
		إذا (لڙي_ڀيٽ(name, sdt_trans[i].signature,
					ACPI_SDT_SIGNATURE_LEN) == 0)
			أرجع (phys_بائيٽون) rsdt.معلومات[i];
	}

	أرجع (phys_بائيٽون) عدم;
}

size_t acpi_get_جدول_ڊيگهائي(ثابت حرف * name)
{
	صحيح i;

	لكل(i = 0; i < sdt_ڳڻپ; i++) {
		إذا (لڙي_ڀيٽ(name, sdt_trans[i].signature,
					ACPI_SDT_SIGNATURE_LEN) == 0)
			أرجع sdt_trans[i].ڊيگهائي;
	}

	أرجع 0;
}

ساكن فراغ * acpi_madt_get_typed_شي(بنية acpi_madt_hdr * hdr,
					بنانشان حرف type,
					بنانشان idx)
{
	u8_t * t, * پڇاڙي;
	صحيح i;

	t = (u8_t *) hdr + حجم(بنية acpi_madt_hdr);
	پڇاڙي = (u8_t *) hdr + hdr->hdr.ڊيگهائي;

	i = 0;
	طالما(t < پڇاڙي) {
		إذا (type == ((بنية acpi_madt_شي_hdr *) t)->type) {
			إذا (i == idx)
				أرجع t;
			وإلا
				i++;
		}
		t += ((بنية acpi_madt_شي_hdr *) t)->ڊيگهائي;
	}

	أرجع عدم;
}

#إذا 0
ساكن فراغ * acpi_madt_get_شي(بنية acpi_madt_hdr * hdr,
				بنانشان idx)
{
	u8_t * t, * پڇاڙي;
	صحيح i;

	t = (u8_t *) hdr + حجم(بنية acpi_madt_hdr);
	پڇاڙي = (u8_t *) hdr + hdr->hdr.ڊيگهائي;

	لكل(i = 0 ; i <= idx && t < پڇاڙي; i++) {
		إذا (i == idx)
			أرجع t;
		t += ((بنية acpi_madt_شي_hdr *) t)->ڊيگهائي;
	}

	أرجع عدم;
}
#جيڪڏهن_پورو

ساكن صحيح acpi_rsdp_test(فراغ * buff)
{
	بنية acpi_rsdp * rsdp = (بنية acpi_rsdp *) buff;

	إذا (!platform_tbl_checksum_ok(buff, 20))
		أرجع 0;
	إذا (لڙي_ڀيٽ(rsdp->signature, "RSD PTR ", 8))
		أرجع 0;

	أرجع 1;
}

ساكن صحيح get_acpi_rsdp(فراغ)
{
	u16_t ebda;
	/*
	 * Read 40:0Eh - to find the starting address of the EBDA.
	 */
	acpi_phys_copy (0x40E, &ebda, حجم(ebda));
	إذا (ebda) {
		ebda <<= 4;
		إذا(platform_tbl_ptr(ebda, ebda + 0x400, 16, &acpi_rsdp,
					حجم(acpi_rsdp), &machine.acpi_rsdp,
					acpi_rsdp_test))
			أرجع 1;
	} 

	/* try BIOS read only mem space */
	إذا(platform_tbl_ptr(0xE0000, 0x100000, 16, &acpi_rsdp,
				حجم(acpi_rsdp), &machine.acpi_rsdp,
				acpi_rsdp_test))
		أرجع 1;
	
	machine.acpi_rsdp = 0; /* RSDP cannot be found at this address therefore
				  it is a valid negative value */
	أرجع 0;
}

ساكن فراغ acpi_init_poweroff(فراغ)
{
	u8_t *ptr = عدم;
	u8_t *شروعات = عدم;
	u8_t *پڇاڙي = عدم;
	بنية acpi_fadt_سرو *fadt_سرو = عدم;
	بنية acpi_rsdt * dsdt_سرو = عدم;
	حرف *نياپو = عدم;

	/* Everything used here existed since ACPI spec 1.0 */
	/* So we can safely use them */
	fadt_سرو = (بنية acpi_fadt_سرو *)
		acpi_phys2vir(acpi_get_جدول_بنياد("FACP"));
	إذا (fadt_سرو == عدم) {
		نياپو = "Could not load FACP";
		اذهب ٻاهر_نڪرو;
	}

	dsdt_سرو = (بنية acpi_rsdt *)
		acpi_phys2vir((phys_بائيٽون) fadt_سرو->dsdt);
	إذا (dsdt_سرو == عدم) {
		نياپو = "Could not load DSDT";
		اذهب ٻاهر_نڪرو;
	}

	pm1a_cnt_blk = fadt_سرو->pm1a_cnt_blk;
	pm1b_cnt_blk = fadt_سرو->pm1b_cnt_blk;

	ptr = شروعات = (u8_t *) dsdt_سرو->معلومات;
	پڇاڙي = شروعات + dsdt_سرو->hdr.ڊيگهائي - 4;

	/* See http://forum.osdev.org/viewtopic.php?t=16990 */
	/* for layout of \_S5 */
	طالما (ptr < پڇاڙي && memcmp(ptr, "_S5_", 4) != 0)
		ptr++;

	نياپو = "Could not read S5 data. Use default SLP_TYPa and SLP_TYPb";
	إذا (ptr >= پڇاڙي || ptr == شروعات)
		اذهب ٻاهر_نڪرو;

	/* validate AML structure */
	إذا (*(ptr + AMI_S5_PACKAGE_OP_وٿي) != AMI_PACKAGE_OP_ڪوڊ)
		اذهب ٻاهر_نڪرو;

	إذا ((ptr < شروعات + (-AMI_S5_NAME_OP_وٿي_2) ||
		(*(ptr + AMI_S5_NAME_OP_وٿي_2) != AMI_NAME_OP_ڪوڊ ||
		 *(ptr + AMI_S5_NAME_OP_وٿي_2 + 1) != '\\')) &&
		*(ptr + AMI_S5_NAME_OP_وٿي_1) != AMI_NAME_OP_ڪوڊ)
		اذهب ٻاهر_نڪرو;

	ptr += AMI_S5_PACKET_ڊيگهائي_وٿي;
	إذا (ptr >= پڇاڙي)
		اذهب ٻاهر_نڪرو;

	/* package length */
	ptr += ((*ptr & AMI_PACKAGE_ڊيگهائي_ENCODING_بٽ_نقاب) >>
		AMI_PACKAGE_ڊيگهائي_ENCODING_بٽ_سرڪايو) +
		AMI_MIN_PACKAGE_ڊيگهائي + AMI_انگ_ELEMENTS_ڊيگهائي;
	إذا (ptr >= پڇاڙي)
		اذهب ٻاهر_نڪرو;

	إذا (*ptr == AMI_BYTE_اڳياڙي_ڪوڊ)
		ptr++; /* skip byte prefix */

	slp_typa = (*ptr) << AMI_SLP_TYPA_سرڪايو;

	ptr++; /* move to SLP_TYPb */
	إذا (*ptr == AMI_BYTE_اڳياڙي_ڪوڊ)
		ptr++; /* skip byte prefix */

	slp_typb = (*ptr) << AMI_SLP_TYPB_سرڪايو;

	نياپو = "poweroff initialized";

ٻاهر_نڪرو:
	إذا (نياپو) {
		DEBUGBASIC(("acpi: %s\n", نياپو));
	}
}

فراغ acpi_init(فراغ)
{
	صحيح s, i;
	read_func = acpi_phys_copy;

	إذا (!get_acpi_rsdp()) {
		لکو("WARNING : Cannot configure ACPI\n");
		أرجع;
	}
	
	s = acpi_read_sdt_at(acpi_rsdp.rsdt_addr, (بنية acpi_sdt_سرو *) &rsdt,
			حجم(بنية acpi_rsdt), ACPI_SDT_SIGNATURE(RSDT));

	sdt_ڳڻپ = (s - حجم(بنية acpi_sdt_سرو)) / حجم(u32_t);

	لكل (i = 0; i < sdt_ڳڻپ; i++) {
		بنية acpi_sdt_سرو hdr;
		صحيح j;
		إذا (read_func(rsdt.معلومات[i], &hdr, حجم(بنية acpi_sdt_سرو))) {
			لکو("ERROR acpi cannot read header at 0x%x\n",
								rsdt.معلومات[i]);
			أرجع;
		}

		لكل (j = 0 ; j < ACPI_SDT_SIGNATURE_LEN; j++)
			sdt_trans[i].signature[j] = hdr.signature[j];
		sdt_trans[i].signature[ACPI_SDT_SIGNATURE_LEN] = '\0';
		sdt_trans[i].ڊيگهائي = hdr.ڊيگهائي;
	}

	acpi_init_poweroff();
}

بنية acpi_madt_ioapic * acpi_get_ioapic_ايندڙ(فراغ)
{
	ساكن بنانشان idx = 0;
	ساكن بنية acpi_madt_hdr * madt_hdr;

	بنية acpi_madt_ioapic * ret;

	إذا (idx == 0) {
		madt_hdr = (بنية acpi_madt_hdr *)
			acpi_phys2vir(acpi_get_جدول_بنياد("APIC"));
		إذا (madt_hdr == عدم)
			أرجع عدم;
	}

	ret = (بنية acpi_madt_ioapic *)
		acpi_madt_get_typed_شي(madt_hdr, ACPI_MADT_TYPE_IOAPIC, idx);
	إذا (ret)
		idx++;

	أرجع ret;
}

بنية acpi_madt_lapic * acpi_get_lapic_ايندڙ(فراغ)
{
	ساكن بنانشان idx = 0;
	ساكن بنية acpi_madt_hdr * madt_hdr;

	بنية acpi_madt_lapic * ret;

	إذا (idx == 0) {
		madt_hdr = (بنية acpi_madt_hdr *)
			acpi_phys2vir(acpi_get_جدول_بنياد("APIC"));
		إذا (madt_hdr == عدم)
			أرجع عدم;
	}

	لكل (;;) {
		ret = (بنية acpi_madt_lapic *)
			acpi_madt_get_typed_شي(madt_hdr,
					ACPI_MADT_TYPE_LAPIC, idx);
		إذا (!ret)
			اكسر;

		idx++;

		/* report only usable CPUs */
		إذا (ret->flags & 1)
			اكسر;
	}

	أرجع ret;
}

فراغ __k_unpaged_acpi_poweroff(فراغ)
{
	/* NO OP poweroff symbol*/
}

فراغ acpi_poweroff(فراغ)
{
	إذا (pm1a_cnt_blk == 0) {
		أرجع;
	}
	outw(pm1a_cnt_blk, slp_typa | SLP_EN_ڪوڊ);
	إذا (pm1b_cnt_blk != 0) {
		outw(pm1b_cnt_blk, slp_typb | SLP_EN_ڪوڊ);
	}
}
