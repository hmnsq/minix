
#ضمن <string.h>

#ضمن "acpi.h"
#ضمن "arch_proto.h"

تعريفنوع صحيح ((* acpi_read_t)(phys_بایتها addr, فراغ * buff, size_t اندازه));

بنية acpi_rsdp acpi_rsdp;

ساكن acpi_read_t read_func;

#تعریف_کن MAX_RSDT	35 /* ACPI defines 35 signatures */
#تعریف_کن SLP_EN_کد	(1 << 13) /* ACPI SLP_EN_CODE code */
#تعریف_کن AMI_PACKAGE_OP_کد (0x12)
#تعریف_کن AMI_NAME_OP_کد (0x8)
#تعریف_کن AMI_BYTE_پیشوند_کد (0xA)
#تعریف_کن AMI_PACKAGE_درازی_ENCODING_بیتها_ماسک (0xC0)
#تعریف_کن AMI_PACKAGE_درازی_ENCODING_بیتها_لغزش (6)
#تعریف_کن AMI_MIN_PACKAGE_درازی (1)
#تعریف_کن AMI_رقم_ELEMENTS_درازی (1)
#تعریف_کن AMI_SLP_TYPA_لغزش (10)
#تعریف_کن AMI_SLP_TYPB_لغزش (10)
#تعریف_کن AMI_S5_NAME_OP_جابه_جایی_1 (-1)
#تعریف_کن AMI_S5_NAME_OP_جابه_جایی_2 (-2)
#تعریف_کن AMI_S5_PACKAGE_OP_جابه_جایی (4)
#تعریف_کن AMI_S5_PACKET_درازی_جابه_جایی (5)

ساكن بنية acpi_rsdt {
	بنية acpi_sdt_سرآیند	hdr;
	u32_t			داده[MAX_RSDT];
} rsdt;
	
ساكن بنية {
	حرف	signature [ACPI_SDT_SIGNATURE_LEN + 1];
	size_t	درازی;
} sdt_trans[MAX_RSDT];

ساكن صحيح sdt_شمار;
ساكن u16_t pm1a_cnt_blk = 0;
ساكن u16_t pm1b_cnt_blk = 0;
ساكن u16_t slp_typa = 0;
ساكن u16_t slp_typb = 0;

ساكن صحيح acpi_check_csum(بنية acpi_sdt_سرآیند * tb, size_t اندازه)
{
	u8_t مجموع = 0;
	صحيح i;
	لكل (i = 0; i < اندازه; i++)
		مجموع += ((بیعلامت حرف *)tb)[i];
	أرجع مجموع == 0 ? 0 : -1;
}

ساكن صحيح acpi_check_signature(ثابت حرف * orig, ثابت حرف * تطابق)
{
	أرجع مقایسه_رشته(orig, تطابق, ACPI_SDT_SIGNATURE_LEN);
}

ساكن u32_t acpi_phys2vir(u32_t p)
{
	إذا(!vm_running) {
		DEBUGEXTRA(("acpi: returning 0x%lx as vir addr\n", p));
		أرجع p;
	}
	panic("acpi: can't get virtual address of arbitrary physical address");
}

ساكن صحيح acpi_phys_copy(phys_بایتها phys, فراغ *هدف, size_t len)
{
	إذا(!vm_running) {
		کپی_حافظه(هدف, (فراغ *) phys, len);
		أرجع 0;
	}
	panic("can't acpi_phys_copy with vm");
}

ساكن صحيح acpi_read_sdt_at(phys_بایتها addr,
				بنية acpi_sdt_سرآیند * tb,
				size_t اندازه,
				ثابت حرف * name)
{
	بنية acpi_sdt_سرآیند hdr;

	/* if NULL is supplied, we only return the size of the table */
	إذا (tb == عدم) {
		إذا (read_func(addr, &hdr, حجم(بنية acpi_sdt_سرآیند))) {
			چاپ_کن("ERROR acpi cannot read %s header\n", name);
			أرجع -1;
		}

		أرجع hdr.درازی;
	}

	إذا (read_func(addr, tb, حجم(بنية acpi_sdt_سرآیند))) {
		چاپ_کن("ERROR acpi cannot read %s header\n", name);
		أرجع -1;
	}

	إذا (acpi_check_signature(tb->signature, name)) {
		چاپ_کن("ERROR acpi %s signature does not match\n", name);
		أرجع -1;
	}

	إذا (اندازه < tb->درازی) {
		چاپ_کن("ERROR acpi buffer too small for %s\n", name);
		أرجع -1;
	}

	إذا (read_func(addr, tb, اندازه)) {
		چاپ_کن("ERROR acpi cannot read %s\n", name);
		أرجع -1;
	}

	إذا (acpi_check_csum(tb, tb->درازی)) {
		چاپ_کن("ERROR acpi %s checksum does not match\n", name);
		أرجع -1;
	}

	أرجع tb->درازی;
}

phys_بایتها acpi_get_جدول_پایه(ثابت حرف * name)
{
	صحيح i;

	لكل(i = 0; i < sdt_شمار; i++) {
		إذا (مقایسه_رشته(name, sdt_trans[i].signature,
					ACPI_SDT_SIGNATURE_LEN) == 0)
			أرجع (phys_بایتها) rsdt.داده[i];
	}

	أرجع (phys_بایتها) عدم;
}

size_t acpi_get_جدول_درازی(ثابت حرف * name)
{
	صحيح i;

	لكل(i = 0; i < sdt_شمار; i++) {
		إذا (مقایسه_رشته(name, sdt_trans[i].signature,
					ACPI_SDT_SIGNATURE_LEN) == 0)
			أرجع sdt_trans[i].درازی;
	}

	أرجع 0;
}

ساكن فراغ * acpi_madt_get_typed_قلم(بنية acpi_madt_hdr * hdr,
					بیعلامت حرف type,
					بیعلامت idx)
{
	u8_t * t, * انتها;
	صحيح i;

	t = (u8_t *) hdr + حجم(بنية acpi_madt_hdr);
	انتها = (u8_t *) hdr + hdr->hdr.درازی;

	i = 0;
	طالما(t < انتها) {
		إذا (type == ((بنية acpi_madt_قلم_hdr *) t)->type) {
			إذا (i == idx)
				أرجع t;
			وإلا
				i++;
		}
		t += ((بنية acpi_madt_قلم_hdr *) t)->درازی;
	}

	أرجع عدم;
}

#إذا 0
ساكن فراغ * acpi_madt_get_قلم(بنية acpi_madt_hdr * hdr,
				بیعلامت idx)
{
	u8_t * t, * انتها;
	صحيح i;

	t = (u8_t *) hdr + حجم(بنية acpi_madt_hdr);
	انتها = (u8_t *) hdr + hdr->hdr.درازی;

	لكل(i = 0 ; i <= idx && t < انتها; i++) {
		إذا (i == idx)
			أرجع t;
		t += ((بنية acpi_madt_قلم_hdr *) t)->درازی;
	}

	أرجع عدم;
}
#پایان_اگر

ساكن صحيح acpi_rsdp_test(فراغ * buff)
{
	بنية acpi_rsdp * rsdp = (بنية acpi_rsdp *) buff;

	إذا (!platform_tbl_checksum_ok(buff, 20))
		أرجع 0;
	إذا (مقایسه_رشته(rsdp->signature, "RSD PTR ", 8))
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
	u8_t *انتها = عدم;
	بنية acpi_fadt_سرآیند *fadt_سرآیند = عدم;
	بنية acpi_rsdt * dsdt_سرآیند = عدم;
	حرف *پیام_2 = عدم;

	/* Everything used here existed since ACPI spec 1.0 */
	/* So we can safely use them */
	fadt_سرآیند = (بنية acpi_fadt_سرآیند *)
		acpi_phys2vir(acpi_get_جدول_پایه("FACP"));
	إذا (fadt_سرآیند == عدم) {
		پیام_2 = "Could not load FACP";
		اذهب خروج;
	}

	dsdt_سرآیند = (بنية acpi_rsdt *)
		acpi_phys2vir((phys_بایتها) fadt_سرآیند->dsdt);
	إذا (dsdt_سرآیند == عدم) {
		پیام_2 = "Could not load DSDT";
		اذهب خروج;
	}

	pm1a_cnt_blk = fadt_سرآیند->pm1a_cnt_blk;
	pm1b_cnt_blk = fadt_سرآیند->pm1b_cnt_blk;

	ptr = شروع = (u8_t *) dsdt_سرآیند->داده;
	انتها = شروع + dsdt_سرآیند->hdr.درازی - 4;

	/* See http://forum.osdev.org/viewtopic.php?t=16990 */
	/* for layout of \_S5 */
	طالما (ptr < انتها && memcmp(ptr, "_S5_", 4) != 0)
		ptr++;

	پیام_2 = "Could not read S5 data. Use default SLP_TYPa and SLP_TYPb";
	إذا (ptr >= انتها || ptr == شروع)
		اذهب خروج;

	/* validate AML structure */
	إذا (*(ptr + AMI_S5_PACKAGE_OP_جابه_جایی) != AMI_PACKAGE_OP_کد)
		اذهب خروج;

	إذا ((ptr < شروع + (-AMI_S5_NAME_OP_جابه_جایی_2) ||
		(*(ptr + AMI_S5_NAME_OP_جابه_جایی_2) != AMI_NAME_OP_کد ||
		 *(ptr + AMI_S5_NAME_OP_جابه_جایی_2 + 1) != '\\')) &&
		*(ptr + AMI_S5_NAME_OP_جابه_جایی_1) != AMI_NAME_OP_کد)
		اذهب خروج;

	ptr += AMI_S5_PACKET_درازی_جابه_جایی;
	إذا (ptr >= انتها)
		اذهب خروج;

	/* package length */
	ptr += ((*ptr & AMI_PACKAGE_درازی_ENCODING_بیتها_ماسک) >>
		AMI_PACKAGE_درازی_ENCODING_بیتها_لغزش) +
		AMI_MIN_PACKAGE_درازی + AMI_رقم_ELEMENTS_درازی;
	إذا (ptr >= انتها)
		اذهب خروج;

	إذا (*ptr == AMI_BYTE_پیشوند_کد)
		ptr++; /* skip byte prefix */

	slp_typa = (*ptr) << AMI_SLP_TYPA_لغزش;

	ptr++; /* move to SLP_TYPb */
	إذا (*ptr == AMI_BYTE_پیشوند_کد)
		ptr++; /* skip byte prefix */

	slp_typb = (*ptr) << AMI_SLP_TYPB_لغزش;

	پیام_2 = "poweroff initialized";

خروج:
	إذا (پیام_2) {
		DEBUGBASIC(("acpi: %s\n", پیام_2));
	}
}

فراغ acpi_init(فراغ)
{
	صحيح s, i;
	read_func = acpi_phys_copy;

	إذا (!get_acpi_rsdp()) {
		چاپ_کن("WARNING : Cannot configure ACPI\n");
		أرجع;
	}
	
	s = acpi_read_sdt_at(acpi_rsdp.rsdt_addr, (بنية acpi_sdt_سرآیند *) &rsdt,
			حجم(بنية acpi_rsdt), ACPI_SDT_SIGNATURE(RSDT));

	sdt_شمار = (s - حجم(بنية acpi_sdt_سرآیند)) / حجم(u32_t);

	لكل (i = 0; i < sdt_شمار; i++) {
		بنية acpi_sdt_سرآیند hdr;
		صحيح j;
		إذا (read_func(rsdt.داده[i], &hdr, حجم(بنية acpi_sdt_سرآیند))) {
			چاپ_کن("ERROR acpi cannot read header at 0x%x\n",
								rsdt.داده[i]);
			أرجع;
		}

		لكل (j = 0 ; j < ACPI_SDT_SIGNATURE_LEN; j++)
			sdt_trans[i].signature[j] = hdr.signature[j];
		sdt_trans[i].signature[ACPI_SDT_SIGNATURE_LEN] = '\0';
		sdt_trans[i].درازی = hdr.درازی;
	}

	acpi_init_poweroff();
}

بنية acpi_madt_ioapic * acpi_get_ioapic_بعدی(فراغ)
{
	ساكن بیعلامت idx = 0;
	ساكن بنية acpi_madt_hdr * madt_hdr;

	بنية acpi_madt_ioapic * ret;

	إذا (idx == 0) {
		madt_hdr = (بنية acpi_madt_hdr *)
			acpi_phys2vir(acpi_get_جدول_پایه("APIC"));
		إذا (madt_hdr == عدم)
			أرجع عدم;
	}

	ret = (بنية acpi_madt_ioapic *)
		acpi_madt_get_typed_قلم(madt_hdr, ACPI_MADT_TYPE_IOAPIC, idx);
	إذا (ret)
		idx++;

	أرجع ret;
}

بنية acpi_madt_lapic * acpi_get_lapic_بعدی(فراغ)
{
	ساكن بیعلامت idx = 0;
	ساكن بنية acpi_madt_hdr * madt_hdr;

	بنية acpi_madt_lapic * ret;

	إذا (idx == 0) {
		madt_hdr = (بنية acpi_madt_hdr *)
			acpi_phys2vir(acpi_get_جدول_پایه("APIC"));
		إذا (madt_hdr == عدم)
			أرجع عدم;
	}

	لكل (;;) {
		ret = (بنية acpi_madt_lapic *)
			acpi_madt_get_typed_قلم(madt_hdr,
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
	outw(pm1a_cnt_blk, slp_typa | SLP_EN_کد);
	إذا (pm1b_cnt_blk != 0) {
		outw(pm1b_cnt_blk, slp_typb | SLP_EN_کد);
	}
}
