
#ضمن <string.h>

#ضمن "acpi.h"
#ضمن "arch_proto.h"

تعريفنوع صحيح ((* acpi_read_t)(phys_بایټونه addr, فراغ * buff, size_t اندازه));

بنية acpi_rsdp acpi_rsdp;

ساكن acpi_read_t read_func;

#تعریف_کړه MAX_RSDT	35 /* ACPI defines 35 signatures */
#تعریف_کړه SLP_EN_کوډ	(1 << 13) /* ACPI SLP_EN_CODE code */
#تعریف_کړه AMI_PACKAGE_OP_کوډ (0x12)
#تعریف_کړه AMI_NAME_OP_کوډ (0x8)
#تعریف_کړه AMI_BYTE_مختاړی_کوډ (0xA)
#تعریف_کړه AMI_PACKAGE_اوږدتیا_ENCODING_بټونه_ماسک (0xC0)
#تعریف_کړه AMI_PACKAGE_اوږدتیا_ENCODING_بټونه_ښويدنه (6)
#تعریف_کړه AMI_MIN_PACKAGE_اوږدتیا (1)
#تعریف_کړه AMI_عدد_ELEMENTS_اوږدتیا (1)
#تعریف_کړه AMI_SLP_TYPA_ښويدنه (10)
#تعریف_کړه AMI_SLP_TYPB_ښويدنه (10)
#تعریف_کړه AMI_S5_NAME_OP_دننه_واټن_1 (-1)
#تعریف_کړه AMI_S5_NAME_OP_دننه_واټن_2 (-2)
#تعریف_کړه AMI_S5_PACKAGE_OP_دننه_واټن (4)
#تعریف_کړه AMI_S5_PACKET_اوږدتیا_دننه_واټن (5)

ساكن بنية acpi_rsdt {
	بنية acpi_sdt_سرلیک	hdr;
	u32_t			معلومات[MAX_RSDT];
} rsdt;
	
ساكن بنية {
	حرف	signature [ACPI_SDT_SIGNATURE_LEN + 1];
	size_t	اوږدتیا;
} sdt_trans[MAX_RSDT];

ساكن صحيح sdt_شمېر;
ساكن u16_t pm1a_cnt_blk = 0;
ساكن u16_t pm1b_cnt_blk = 0;
ساكن u16_t slp_typa = 0;
ساكن u16_t slp_typb = 0;

ساكن صحيح acpi_check_csum(بنية acpi_sdt_سرلیک * tb, size_t اندازه)
{
	u8_t ټول = 0;
	صحيح i;
	لكل (i = 0; i < اندازه; i++)
		ټول += ((بینښه حرف *)tb)[i];
	أرجع ټول == 0 ? 0 : -1;
}

ساكن صحيح acpi_check_signature(ثابت حرف * orig, ثابت حرف * سمون)
{
	أرجع مزي_پرتله(orig, سمون, ACPI_SDT_SIGNATURE_LEN);
}

ساكن u32_t acpi_phys2vir(u32_t p)
{
	إذا(!vm_running) {
		DEBUGEXTRA(("acpi: returning 0x%lx as vir addr\n", p));
		أرجع p;
	}
	panic("acpi: can't get virtual address of arbitrary physical address");
}

ساكن صحيح acpi_phys_copy(phys_بایټونه phys, فراغ *موخه, size_t len)
{
	إذا(!vm_running) {
		حافظه_نقل(موخه, (فراغ *) phys, len);
		أرجع 0;
	}
	panic("can't acpi_phys_copy with vm");
}

ساكن صحيح acpi_read_sdt_at(phys_بایټونه addr,
				بنية acpi_sdt_سرلیک * tb,
				size_t اندازه,
				ثابت حرف * name)
{
	بنية acpi_sdt_سرلیک hdr;

	/* if NULL is supplied, we only return the size of the table */
	إذا (tb == عدم) {
		إذا (read_func(addr, &hdr, حجم(بنية acpi_sdt_سرلیک))) {
			چاپ_کړه("ERROR acpi cannot read %s header\n", name);
			أرجع -1;
		}

		أرجع hdr.اوږدتیا;
	}

	إذا (read_func(addr, tb, حجم(بنية acpi_sdt_سرلیک))) {
		چاپ_کړه("ERROR acpi cannot read %s header\n", name);
		أرجع -1;
	}

	إذا (acpi_check_signature(tb->signature, name)) {
		چاپ_کړه("ERROR acpi %s signature does not match\n", name);
		أرجع -1;
	}

	إذا (اندازه < tb->اوږدتیا) {
		چاپ_کړه("ERROR acpi buffer too small for %s\n", name);
		أرجع -1;
	}

	إذا (read_func(addr, tb, اندازه)) {
		چاپ_کړه("ERROR acpi cannot read %s\n", name);
		أرجع -1;
	}

	إذا (acpi_check_csum(tb, tb->اوږدتیا)) {
		چاپ_کړه("ERROR acpi %s checksum does not match\n", name);
		أرجع -1;
	}

	أرجع tb->اوږدتیا;
}

phys_بایټونه acpi_get_جدول_بنسټ(ثابت حرف * name)
{
	صحيح i;

	لكل(i = 0; i < sdt_شمېر; i++) {
		إذا (مزي_پرتله(name, sdt_trans[i].signature,
					ACPI_SDT_SIGNATURE_LEN) == 0)
			أرجع (phys_بایټونه) rsdt.معلومات[i];
	}

	أرجع (phys_بایټونه) عدم;
}

size_t acpi_get_جدول_اوږدتیا(ثابت حرف * name)
{
	صحيح i;

	لكل(i = 0; i < sdt_شمېر; i++) {
		إذا (مزي_پرتله(name, sdt_trans[i].signature,
					ACPI_SDT_SIGNATURE_LEN) == 0)
			أرجع sdt_trans[i].اوږدتیا;
	}

	أرجع 0;
}

ساكن فراغ * acpi_madt_get_typed_توکی(بنية acpi_madt_hdr * hdr,
					بینښه حرف type,
					بینښه idx)
{
	u8_t * t, * پای_ټکی;
	صحيح i;

	t = (u8_t *) hdr + حجم(بنية acpi_madt_hdr);
	پای_ټکی = (u8_t *) hdr + hdr->hdr.اوږدتیا;

	i = 0;
	طالما(t < پای_ټکی) {
		إذا (type == ((بنية acpi_madt_توکی_hdr *) t)->type) {
			إذا (i == idx)
				أرجع t;
			وإلا
				i++;
		}
		t += ((بنية acpi_madt_توکی_hdr *) t)->اوږدتیا;
	}

	أرجع عدم;
}

#إذا 0
ساكن فراغ * acpi_madt_get_توکی(بنية acpi_madt_hdr * hdr,
				بینښه idx)
{
	u8_t * t, * پای_ټکی;
	صحيح i;

	t = (u8_t *) hdr + حجم(بنية acpi_madt_hdr);
	پای_ټکی = (u8_t *) hdr + hdr->hdr.اوږدتیا;

	لكل(i = 0 ; i <= idx && t < پای_ټکی; i++) {
		إذا (i == idx)
			أرجع t;
		t += ((بنية acpi_madt_توکی_hdr *) t)->اوږدتیا;
	}

	أرجع عدم;
}
#د_که_پای

ساكن صحيح acpi_rsdp_test(فراغ * buff)
{
	بنية acpi_rsdp * rsdp = (بنية acpi_rsdp *) buff;

	إذا (!platform_tbl_checksum_ok(buff, 20))
		أرجع 0;
	إذا (مزي_پرتله(rsdp->signature, "RSD PTR ", 8))
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
	u8_t *پیل = عدم;
	u8_t *پای_ټکی = عدم;
	بنية acpi_fadt_سرلیک *fadt_سرلیک = عدم;
	بنية acpi_rsdt * dsdt_سرلیک = عدم;
	حرف *پیغام_2 = عدم;

	/* Everything used here existed since ACPI spec 1.0 */
	/* So we can safely use them */
	fadt_سرلیک = (بنية acpi_fadt_سرلیک *)
		acpi_phys2vir(acpi_get_جدول_بنسټ("FACP"));
	إذا (fadt_سرلیک == عدم) {
		پیغام_2 = "Could not load FACP";
		اذهب وتل;
	}

	dsdt_سرلیک = (بنية acpi_rsdt *)
		acpi_phys2vir((phys_بایټونه) fadt_سرلیک->dsdt);
	إذا (dsdt_سرلیک == عدم) {
		پیغام_2 = "Could not load DSDT";
		اذهب وتل;
	}

	pm1a_cnt_blk = fadt_سرلیک->pm1a_cnt_blk;
	pm1b_cnt_blk = fadt_سرلیک->pm1b_cnt_blk;

	ptr = پیل = (u8_t *) dsdt_سرلیک->معلومات;
	پای_ټکی = پیل + dsdt_سرلیک->hdr.اوږدتیا - 4;

	/* See http://forum.osdev.org/viewtopic.php?t=16990 */
	/* for layout of \_S5 */
	طالما (ptr < پای_ټکی && memcmp(ptr, "_S5_", 4) != 0)
		ptr++;

	پیغام_2 = "Could not read S5 data. Use default SLP_TYPa and SLP_TYPb";
	إذا (ptr >= پای_ټکی || ptr == پیل)
		اذهب وتل;

	/* validate AML structure */
	إذا (*(ptr + AMI_S5_PACKAGE_OP_دننه_واټن) != AMI_PACKAGE_OP_کوډ)
		اذهب وتل;

	إذا ((ptr < پیل + (-AMI_S5_NAME_OP_دننه_واټن_2) ||
		(*(ptr + AMI_S5_NAME_OP_دننه_واټن_2) != AMI_NAME_OP_کوډ ||
		 *(ptr + AMI_S5_NAME_OP_دننه_واټن_2 + 1) != '\\')) &&
		*(ptr + AMI_S5_NAME_OP_دننه_واټن_1) != AMI_NAME_OP_کوډ)
		اذهب وتل;

	ptr += AMI_S5_PACKET_اوږدتیا_دننه_واټن;
	إذا (ptr >= پای_ټکی)
		اذهب وتل;

	/* package length */
	ptr += ((*ptr & AMI_PACKAGE_اوږدتیا_ENCODING_بټونه_ماسک) >>
		AMI_PACKAGE_اوږدتیا_ENCODING_بټونه_ښويدنه) +
		AMI_MIN_PACKAGE_اوږدتیا + AMI_عدد_ELEMENTS_اوږدتیا;
	إذا (ptr >= پای_ټکی)
		اذهب وتل;

	إذا (*ptr == AMI_BYTE_مختاړی_کوډ)
		ptr++; /* skip byte prefix */

	slp_typa = (*ptr) << AMI_SLP_TYPA_ښويدنه;

	ptr++; /* move to SLP_TYPb */
	إذا (*ptr == AMI_BYTE_مختاړی_کوډ)
		ptr++; /* skip byte prefix */

	slp_typb = (*ptr) << AMI_SLP_TYPB_ښويدنه;

	پیغام_2 = "poweroff initialized";

وتل:
	إذا (پیغام_2) {
		DEBUGBASIC(("acpi: %s\n", پیغام_2));
	}
}

فراغ acpi_init(فراغ)
{
	صحيح s, i;
	read_func = acpi_phys_copy;

	إذا (!get_acpi_rsdp()) {
		چاپ_کړه("WARNING : Cannot configure ACPI\n");
		أرجع;
	}
	
	s = acpi_read_sdt_at(acpi_rsdp.rsdt_addr, (بنية acpi_sdt_سرلیک *) &rsdt,
			حجم(بنية acpi_rsdt), ACPI_SDT_SIGNATURE(RSDT));

	sdt_شمېر = (s - حجم(بنية acpi_sdt_سرلیک)) / حجم(u32_t);

	لكل (i = 0; i < sdt_شمېر; i++) {
		بنية acpi_sdt_سرلیک hdr;
		صحيح j;
		إذا (read_func(rsdt.معلومات[i], &hdr, حجم(بنية acpi_sdt_سرلیک))) {
			چاپ_کړه("ERROR acpi cannot read header at 0x%x\n",
								rsdt.معلومات[i]);
			أرجع;
		}

		لكل (j = 0 ; j < ACPI_SDT_SIGNATURE_LEN; j++)
			sdt_trans[i].signature[j] = hdr.signature[j];
		sdt_trans[i].signature[ACPI_SDT_SIGNATURE_LEN] = '\0';
		sdt_trans[i].اوږدتیا = hdr.اوږدتیا;
	}

	acpi_init_poweroff();
}

بنية acpi_madt_ioapic * acpi_get_ioapic_بل(فراغ)
{
	ساكن بینښه idx = 0;
	ساكن بنية acpi_madt_hdr * madt_hdr;

	بنية acpi_madt_ioapic * ret;

	إذا (idx == 0) {
		madt_hdr = (بنية acpi_madt_hdr *)
			acpi_phys2vir(acpi_get_جدول_بنسټ("APIC"));
		إذا (madt_hdr == عدم)
			أرجع عدم;
	}

	ret = (بنية acpi_madt_ioapic *)
		acpi_madt_get_typed_توکی(madt_hdr, ACPI_MADT_TYPE_IOAPIC, idx);
	إذا (ret)
		idx++;

	أرجع ret;
}

بنية acpi_madt_lapic * acpi_get_lapic_بل(فراغ)
{
	ساكن بینښه idx = 0;
	ساكن بنية acpi_madt_hdr * madt_hdr;

	بنية acpi_madt_lapic * ret;

	إذا (idx == 0) {
		madt_hdr = (بنية acpi_madt_hdr *)
			acpi_phys2vir(acpi_get_جدول_بنسټ("APIC"));
		إذا (madt_hdr == عدم)
			أرجع عدم;
	}

	لكل (;;) {
		ret = (بنية acpi_madt_lapic *)
			acpi_madt_get_typed_توکی(madt_hdr,
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
	outw(pm1a_cnt_blk, slp_typa | SLP_EN_کوډ);
	إذا (pm1b_cnt_blk != 0) {
		outw(pm1b_cnt_blk, slp_typb | SLP_EN_کوډ);
	}
}
