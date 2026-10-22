
#ضمن <string.h>

#ضمن "acpi.h"
#ضمن "arch_proto.h"

تعريفنوع صحيح ((* acpi_read_t)(phys_بائٹاں addr, فراغ * buff, size_t ناپ));

بنية acpi_rsdp acpi_rsdp;

ساكن acpi_read_t read_func;

#تعریف_کرو MAX_RSDT	35 /* ACPI defines 35 signatures */
#تعریف_کرو SLP_EN_کوڈ	(1 << 13) /* ACPI SLP_EN_CODE code */
#تعریف_کرو AMI_PACKAGE_OP_کوڈ (0x12)
#تعریف_کرو AMI_NAME_OP_کوڈ (0x8)
#تعریف_کرو AMI_BYTE_اگیتر_کوڈ (0xA)
#تعریف_کرو AMI_PACKAGE_لمائی_ENCODING_بٹ_نقاب (0xC0)
#تعریف_کرو AMI_PACKAGE_لمائی_ENCODING_بٹ_کھسکاؤ (6)
#تعریف_کرو AMI_MIN_PACKAGE_لمائی (1)
#تعریف_کرو AMI_عدد_ELEMENTS_لمائی (1)
#تعریف_کرو AMI_SLP_TYPA_کھسکاؤ (10)
#تعریف_کرو AMI_SLP_TYPB_کھسکاؤ (10)
#تعریف_کرو AMI_S5_NAME_OP_وتھ_1 (-1)
#تعریف_کرو AMI_S5_NAME_OP_وتھ_2 (-2)
#تعریف_کرو AMI_S5_PACKAGE_OP_وتھ (4)
#تعریف_کرو AMI_S5_PACKET_لمائی_وتھ (5)

ساكن بنية acpi_rsdt {
	بنية acpi_sdt_سرخی	hdr;
	u32_t			انکڑے[MAX_RSDT];
} rsdt;
	
ساكن بنية {
	حرف	signature [ACPI_SDT_SIGNATURE_LEN + 1];
	size_t	لمائی;
} sdt_trans[MAX_RSDT];

ساكن صحيح sdt_گنتی;
ساكن u16_t pm1a_cnt_blk = 0;
ساكن u16_t pm1b_cnt_blk = 0;
ساكن u16_t slp_typa = 0;
ساكن u16_t slp_typb = 0;

ساكن صحيح acpi_check_csum(بنية acpi_sdt_سرخی * tb, size_t ناپ)
{
	u8_t کل = 0;
	صحيح i;
	لكل (i = 0; i < ناپ; i++)
		کل += ((بنانشانی حرف *)tb)[i];
	أرجع کل == 0 ? 0 : -1;
}

ساكن صحيح acpi_check_signature(ثابت حرف * orig, ثابت حرف * میل)
{
	أرجع لڑی_مقابلہ(orig, میل, ACPI_SDT_SIGNATURE_LEN);
}

ساكن u32_t acpi_phys2vir(u32_t p)
{
	إذا(!vm_running) {
		DEBUGEXTRA(("acpi: returning 0x%lx as vir addr\n", p));
		أرجع p;
	}
	panic("acpi: can't get virtual address of arbitrary physical address");
}

ساكن صحيح acpi_phys_copy(phys_بائٹاں phys, فراغ *نشانہ, size_t len)
{
	إذا(!vm_running) {
		حافظہ_نقل(نشانہ, (فراغ *) phys, len);
		أرجع 0;
	}
	panic("can't acpi_phys_copy with vm");
}

ساكن صحيح acpi_read_sdt_at(phys_بائٹاں addr,
				بنية acpi_sdt_سرخی * tb,
				size_t ناپ,
				ثابت حرف * name)
{
	بنية acpi_sdt_سرخی hdr;

	/* if NULL is supplied, we only return the size of the table */
	إذا (tb == عدم) {
		إذا (read_func(addr, &hdr, حجم(بنية acpi_sdt_سرخی))) {
			لکھو("ERROR acpi cannot read %s header\n", name);
			أرجع -1;
		}

		أرجع hdr.لمائی;
	}

	إذا (read_func(addr, tb, حجم(بنية acpi_sdt_سرخی))) {
		لکھو("ERROR acpi cannot read %s header\n", name);
		أرجع -1;
	}

	إذا (acpi_check_signature(tb->signature, name)) {
		لکھو("ERROR acpi %s signature does not match\n", name);
		أرجع -1;
	}

	إذا (ناپ < tb->لمائی) {
		لکھو("ERROR acpi buffer too small for %s\n", name);
		أرجع -1;
	}

	إذا (read_func(addr, tb, ناپ)) {
		لکھو("ERROR acpi cannot read %s\n", name);
		أرجع -1;
	}

	إذا (acpi_check_csum(tb, tb->لمائی)) {
		لکھو("ERROR acpi %s checksum does not match\n", name);
		أرجع -1;
	}

	أرجع tb->لمائی;
}

phys_بائٹاں acpi_get_جدول_ادھار(ثابت حرف * name)
{
	صحيح i;

	لكل(i = 0; i < sdt_گنتی; i++) {
		إذا (لڑی_مقابلہ(name, sdt_trans[i].signature,
					ACPI_SDT_SIGNATURE_LEN) == 0)
			أرجع (phys_بائٹاں) rsdt.انکڑے[i];
	}

	أرجع (phys_بائٹاں) عدم;
}

size_t acpi_get_جدول_لمائی(ثابت حرف * name)
{
	صحيح i;

	لكل(i = 0; i < sdt_گنتی; i++) {
		إذا (لڑی_مقابلہ(name, sdt_trans[i].signature,
					ACPI_SDT_SIGNATURE_LEN) == 0)
			أرجع sdt_trans[i].لمائی;
	}

	أرجع 0;
}

ساكن فراغ * acpi_madt_get_typed_شے(بنية acpi_madt_hdr * hdr,
					بنانشانی حرف type,
					بنانشانی idx)
{
	u8_t * t, * سرا;
	صحيح i;

	t = (u8_t *) hdr + حجم(بنية acpi_madt_hdr);
	سرا = (u8_t *) hdr + hdr->hdr.لمائی;

	i = 0;
	طالما(t < سرا) {
		إذا (type == ((بنية acpi_madt_شے_hdr *) t)->type) {
			إذا (i == idx)
				أرجع t;
			وإلا
				i++;
		}
		t += ((بنية acpi_madt_شے_hdr *) t)->لمائی;
	}

	أرجع عدم;
}

#إذا 0
ساكن فراغ * acpi_madt_get_شے(بنية acpi_madt_hdr * hdr,
				بنانشانی idx)
{
	u8_t * t, * سرا;
	صحيح i;

	t = (u8_t *) hdr + حجم(بنية acpi_madt_hdr);
	سرا = (u8_t *) hdr + hdr->hdr.لمائی;

	لكل(i = 0 ; i <= idx && t < سرا; i++) {
		إذا (i == idx)
			أرجع t;
		t += ((بنية acpi_madt_شے_hdr *) t)->لمائی;
	}

	أرجع عدم;
}
#جے_انت

ساكن صحيح acpi_rsdp_test(فراغ * buff)
{
	بنية acpi_rsdp * rsdp = (بنية acpi_rsdp *) buff;

	إذا (!platform_tbl_checksum_ok(buff, 20))
		أرجع 0;
	إذا (لڑی_مقابلہ(rsdp->signature, "RSD PTR ", 8))
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
	u8_t *شروع = عدم;
	u8_t *سرا = عدم;
	بنية acpi_fadt_سرخی *fadt_سرخی = عدم;
	بنية acpi_rsdt * dsdt_سرخی = عدم;
	حرف *پیغام = عدم;

	/* Everything used here existed since ACPI spec 1.0 */
	/* So we can safely use them */
	fadt_سرخی = (بنية acpi_fadt_سرخی *)
		acpi_phys2vir(acpi_get_جدول_ادھار("FACP"));
	إذا (fadt_سرخی == عدم) {
		پیغام = "Could not load FACP";
		اذهب باہر_نکلو;
	}

	dsdt_سرخی = (بنية acpi_rsdt *)
		acpi_phys2vir((phys_بائٹاں) fadt_سرخی->dsdt);
	إذا (dsdt_سرخی == عدم) {
		پیغام = "Could not load DSDT";
		اذهب باہر_نکلو;
	}

	pm1a_cnt_blk = fadt_سرخی->pm1a_cnt_blk;
	pm1b_cnt_blk = fadt_سرخی->pm1b_cnt_blk;

	ptr = شروع = (u8_t *) dsdt_سرخی->انکڑے;
	سرا = شروع + dsdt_سرخی->hdr.لمائی - 4;

	/* See http://forum.osdev.org/viewtopic.php?t=16990 */
	/* for layout of \_S5 */
	طالما (ptr < سرا && memcmp(ptr, "_S5_", 4) != 0)
		ptr++;

	پیغام = "Could not read S5 data. Use default SLP_TYPa and SLP_TYPb";
	إذا (ptr >= سرا || ptr == شروع)
		اذهب باہر_نکلو;

	/* validate AML structure */
	إذا (*(ptr + AMI_S5_PACKAGE_OP_وتھ) != AMI_PACKAGE_OP_کوڈ)
		اذهب باہر_نکلو;

	إذا ((ptr < شروع + (-AMI_S5_NAME_OP_وتھ_2) ||
		(*(ptr + AMI_S5_NAME_OP_وتھ_2) != AMI_NAME_OP_کوڈ ||
		 *(ptr + AMI_S5_NAME_OP_وتھ_2 + 1) != '\\')) &&
		*(ptr + AMI_S5_NAME_OP_وتھ_1) != AMI_NAME_OP_کوڈ)
		اذهب باہر_نکلو;

	ptr += AMI_S5_PACKET_لمائی_وتھ;
	إذا (ptr >= سرا)
		اذهب باہر_نکلو;

	/* package length */
	ptr += ((*ptr & AMI_PACKAGE_لمائی_ENCODING_بٹ_نقاب) >>
		AMI_PACKAGE_لمائی_ENCODING_بٹ_کھسکاؤ) +
		AMI_MIN_PACKAGE_لمائی + AMI_عدد_ELEMENTS_لمائی;
	إذا (ptr >= سرا)
		اذهب باہر_نکلو;

	إذا (*ptr == AMI_BYTE_اگیتر_کوڈ)
		ptr++; /* skip byte prefix */

	slp_typa = (*ptr) << AMI_SLP_TYPA_کھسکاؤ;

	ptr++; /* move to SLP_TYPb */
	إذا (*ptr == AMI_BYTE_اگیتر_کوڈ)
		ptr++; /* skip byte prefix */

	slp_typb = (*ptr) << AMI_SLP_TYPB_کھسکاؤ;

	پیغام = "poweroff initialized";

باہر_نکلو:
	إذا (پیغام) {
		DEBUGBASIC(("acpi: %s\n", پیغام));
	}
}

فراغ acpi_init(فراغ)
{
	صحيح s, i;
	read_func = acpi_phys_copy;

	إذا (!get_acpi_rsdp()) {
		لکھو("WARNING : Cannot configure ACPI\n");
		أرجع;
	}
	
	s = acpi_read_sdt_at(acpi_rsdp.rsdt_addr, (بنية acpi_sdt_سرخی *) &rsdt,
			حجم(بنية acpi_rsdt), ACPI_SDT_SIGNATURE(RSDT));

	sdt_گنتی = (s - حجم(بنية acpi_sdt_سرخی)) / حجم(u32_t);

	لكل (i = 0; i < sdt_گنتی; i++) {
		بنية acpi_sdt_سرخی hdr;
		صحيح j;
		إذا (read_func(rsdt.انکڑے[i], &hdr, حجم(بنية acpi_sdt_سرخی))) {
			لکھو("ERROR acpi cannot read header at 0x%x\n",
								rsdt.انکڑے[i]);
			أرجع;
		}

		لكل (j = 0 ; j < ACPI_SDT_SIGNATURE_LEN; j++)
			sdt_trans[i].signature[j] = hdr.signature[j];
		sdt_trans[i].signature[ACPI_SDT_SIGNATURE_LEN] = '\0';
		sdt_trans[i].لمائی = hdr.لمائی;
	}

	acpi_init_poweroff();
}

بنية acpi_madt_ioapic * acpi_get_ioapic_اگلا(فراغ)
{
	ساكن بنانشانی idx = 0;
	ساكن بنية acpi_madt_hdr * madt_hdr;

	بنية acpi_madt_ioapic * ret;

	إذا (idx == 0) {
		madt_hdr = (بنية acpi_madt_hdr *)
			acpi_phys2vir(acpi_get_جدول_ادھار("APIC"));
		إذا (madt_hdr == عدم)
			أرجع عدم;
	}

	ret = (بنية acpi_madt_ioapic *)
		acpi_madt_get_typed_شے(madt_hdr, ACPI_MADT_TYPE_IOAPIC, idx);
	إذا (ret)
		idx++;

	أرجع ret;
}

بنية acpi_madt_lapic * acpi_get_lapic_اگلا(فراغ)
{
	ساكن بنانشانی idx = 0;
	ساكن بنية acpi_madt_hdr * madt_hdr;

	بنية acpi_madt_lapic * ret;

	إذا (idx == 0) {
		madt_hdr = (بنية acpi_madt_hdr *)
			acpi_phys2vir(acpi_get_جدول_ادھار("APIC"));
		إذا (madt_hdr == عدم)
			أرجع عدم;
	}

	لكل (;;) {
		ret = (بنية acpi_madt_lapic *)
			acpi_madt_get_typed_شے(madt_hdr,
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
	outw(pm1a_cnt_blk, slp_typa | SLP_EN_کوڈ);
	إذا (pm1b_cnt_blk != 0) {
		outw(pm1b_cnt_blk, slp_typb | SLP_EN_کوڈ);
	}
}
