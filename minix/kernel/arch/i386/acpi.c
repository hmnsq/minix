
#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "acpi.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "arch_proto.h"

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਪੂਰਨ ((* acpi_read_t)(phys_ਬਾਈਟਾਂ addr, ਖਾਲੀ * buff, size_t ਮਾਪ));

ਬਣਤਰ acpi_rsdp acpi_rsdp;

ਸਥਿਰ acpi_read_t read_func;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAX_RSDT	35 /* ACPI defines 35 signatures */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SLP_EN_ਕੋਡ	(1 << 13) /* ACPI SLP_EN_CODE code */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AMI_PACKAGE_OP_ਕੋਡ (0x12)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AMI_NAME_OP_ਕੋਡ (0x8)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AMI_BYTE_ਅਗੇਤਰ_ਕੋਡ (0xA)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AMI_PACKAGE_ਲੰਮਾਈ_ENCODING_ਬਿੱਟ_ਨਕਾਬ (0xC0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AMI_PACKAGE_ਲੰਮਾਈ_ENCODING_ਬਿੱਟ_ਖਿਸਕਾਓ (6)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AMI_MIN_PACKAGE_ਲੰਮਾਈ (1)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AMI_ਅੰਕ_ELEMENTS_ਲੰਮਾਈ (1)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AMI_SLP_TYPA_ਖਿਸਕਾਓ (10)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AMI_SLP_TYPB_ਖਿਸਕਾਓ (10)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AMI_S5_NAME_OP_ਵਿੱਥ_1 (-1)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AMI_S5_NAME_OP_ਵਿੱਥ_2 (-2)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AMI_S5_PACKAGE_OP_ਵਿੱਥ (4)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AMI_S5_PACKET_ਲੰਮਾਈ_ਵਿੱਥ (5)

ਸਥਿਰ ਬਣਤਰ acpi_rsdt {
	ਬਣਤਰ acpi_sdt_ਸਿਰਲੇਖ	hdr;
	u32_t			ਅੰਕੜੇ[MAX_RSDT];
} rsdt;
	
ਸਥਿਰ ਬਣਤਰ {
	ਅੱਖਰ	signature [ACPI_SDT_SIGNATURE_LEN + 1];
	size_t	ਲੰਮਾਈ;
} sdt_trans[MAX_RSDT];

ਸਥਿਰ ਪੂਰਨ sdt_ਗਿਣਤੀ;
ਸਥਿਰ u16_t pm1a_cnt_blk = 0;
ਸਥਿਰ u16_t pm1b_cnt_blk = 0;
ਸਥਿਰ u16_t slp_typa = 0;
ਸਥਿਰ u16_t slp_typb = 0;

ਸਥਿਰ ਪੂਰਨ acpi_check_csum(ਬਣਤਰ acpi_sdt_ਸਿਰਲੇਖ * tb, size_t ਮਾਪ)
{
	u8_t ਕੁੱਲ = 0;
	ਪੂਰਨ i;
	ਲਈ (i = 0; i < ਮਾਪ; i++)
		ਕੁੱਲ += ((ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)tb)[i];
	ਵਾਪਸ ਕੁੱਲ == 0 ? 0 : -1;
}

ਸਥਿਰ ਪੂਰਨ acpi_check_signature(ਅਚਲ ਅੱਖਰ * orig, ਅਚਲ ਅੱਖਰ * ਮੇਲ)
{
	ਵਾਪਸ ਲੜੀ_ਤੁਲਨਾ(orig, ਮੇਲ, ACPI_SDT_SIGNATURE_LEN);
}

ਸਥਿਰ u32_t acpi_phys2vir(u32_t p)
{
	ਜੇ(!vm_running) {
		DEBUGEXTRA(("acpi: returning 0x%lx as vir addr\n", p));
		ਵਾਪਸ p;
	}
	panic("acpi: can't get virtual address of arbitrary physical address");
}

ਸਥਿਰ ਪੂਰਨ acpi_phys_copy(phys_ਬਾਈਟਾਂ phys, ਖਾਲੀ *ਨਿਸ਼ਾਨਾ, size_t len)
{
	ਜੇ(!vm_running) {
		ਸਮ੍ਰਿਤੀ_ਨਕਲ(ਨਿਸ਼ਾਨਾ, (ਖਾਲੀ *) phys, len);
		ਵਾਪਸ 0;
	}
	panic("can't acpi_phys_copy with vm");
}

ਸਥਿਰ ਪੂਰਨ acpi_read_sdt_at(phys_ਬਾਈਟਾਂ addr,
				ਬਣਤਰ acpi_sdt_ਸਿਰਲੇਖ * tb,
				size_t ਮਾਪ,
				ਅਚਲ ਅੱਖਰ * name)
{
	ਬਣਤਰ acpi_sdt_ਸਿਰਲੇਖ hdr;

	/* if NULL is supplied, we only return the size of the table */
	ਜੇ (tb == ਨਿਰਾ) {
		ਜੇ (read_func(addr, &hdr, ਆਕਾਰ(ਬਣਤਰ acpi_sdt_ਸਿਰਲੇਖ))) {
			ਲਿਖੋ("ERROR acpi cannot read %s header\n", name);
			ਵਾਪਸ -1;
		}

		ਵਾਪਸ hdr.ਲੰਮਾਈ;
	}

	ਜੇ (read_func(addr, tb, ਆਕਾਰ(ਬਣਤਰ acpi_sdt_ਸਿਰਲੇਖ))) {
		ਲਿਖੋ("ERROR acpi cannot read %s header\n", name);
		ਵਾਪਸ -1;
	}

	ਜੇ (acpi_check_signature(tb->signature, name)) {
		ਲਿਖੋ("ERROR acpi %s signature does not match\n", name);
		ਵਾਪਸ -1;
	}

	ਜੇ (ਮਾਪ < tb->ਲੰਮਾਈ) {
		ਲਿਖੋ("ERROR acpi buffer too small for %s\n", name);
		ਵਾਪਸ -1;
	}

	ਜੇ (read_func(addr, tb, ਮਾਪ)) {
		ਲਿਖੋ("ERROR acpi cannot read %s\n", name);
		ਵਾਪਸ -1;
	}

	ਜੇ (acpi_check_csum(tb, tb->ਲੰਮਾਈ)) {
		ਲਿਖੋ("ERROR acpi %s checksum does not match\n", name);
		ਵਾਪਸ -1;
	}

	ਵਾਪਸ tb->ਲੰਮਾਈ;
}

phys_ਬਾਈਟਾਂ acpi_get_ਸਾਰਣੀ_ਅਧਾਰ(ਅਚਲ ਅੱਖਰ * name)
{
	ਪੂਰਨ i;

	ਲਈ(i = 0; i < sdt_ਗਿਣਤੀ; i++) {
		ਜੇ (ਲੜੀ_ਤੁਲਨਾ(name, sdt_trans[i].signature,
					ACPI_SDT_SIGNATURE_LEN) == 0)
			ਵਾਪਸ (phys_ਬਾਈਟਾਂ) rsdt.ਅੰਕੜੇ[i];
	}

	ਵਾਪਸ (phys_ਬਾਈਟਾਂ) ਨਿਰਾ;
}

size_t acpi_get_ਸਾਰਣੀ_ਲੰਮਾਈ(ਅਚਲ ਅੱਖਰ * name)
{
	ਪੂਰਨ i;

	ਲਈ(i = 0; i < sdt_ਗਿਣਤੀ; i++) {
		ਜੇ (ਲੜੀ_ਤੁਲਨਾ(name, sdt_trans[i].signature,
					ACPI_SDT_SIGNATURE_LEN) == 0)
			ਵਾਪਸ sdt_trans[i].ਲੰਮਾਈ;
	}

	ਵਾਪਸ 0;
}

ਸਥਿਰ ਖਾਲੀ * acpi_madt_get_typed_ਵਸਤੂ(ਬਣਤਰ acpi_madt_hdr * hdr,
					ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ type,
					ਬਿਨਾਨਿਸ਼ਾਨੀ idx)
{
	u8_t * t, * ਸਿਰਾ;
	ਪੂਰਨ i;

	t = (u8_t *) hdr + ਆਕਾਰ(ਬਣਤਰ acpi_madt_hdr);
	ਸਿਰਾ = (u8_t *) hdr + hdr->hdr.ਲੰਮਾਈ;

	i = 0;
	ਜਦੋਂ(t < ਸਿਰਾ) {
		ਜੇ (type == ((ਬਣਤਰ acpi_madt_ਵਸਤੂ_hdr *) t)->type) {
			ਜੇ (i == idx)
				ਵਾਪਸ t;
			ਨਹੀਂਤਾਂ
				i++;
		}
		t += ((ਬਣਤਰ acpi_madt_ਵਸਤੂ_hdr *) t)->ਲੰਮਾਈ;
	}

	ਵਾਪਸ ਨਿਰਾ;
}

#ਜੇ 0
ਸਥਿਰ ਖਾਲੀ * acpi_madt_get_ਵਸਤੂ(ਬਣਤਰ acpi_madt_hdr * hdr,
				ਬਿਨਾਨਿਸ਼ਾਨੀ idx)
{
	u8_t * t, * ਸਿਰਾ;
	ਪੂਰਨ i;

	t = (u8_t *) hdr + ਆਕਾਰ(ਬਣਤਰ acpi_madt_hdr);
	ਸਿਰਾ = (u8_t *) hdr + hdr->hdr.ਲੰਮਾਈ;

	ਲਈ(i = 0 ; i <= idx && t < ਸਿਰਾ; i++) {
		ਜੇ (i == idx)
			ਵਾਪਸ t;
		t += ((ਬਣਤਰ acpi_madt_ਵਸਤੂ_hdr *) t)->ਲੰਮਾਈ;
	}

	ਵਾਪਸ ਨਿਰਾ;
}
#ਜੇ_ਅੰਤ

ਸਥਿਰ ਪੂਰਨ acpi_rsdp_test(ਖਾਲੀ * buff)
{
	ਬਣਤਰ acpi_rsdp * rsdp = (ਬਣਤਰ acpi_rsdp *) buff;

	ਜੇ (!platform_tbl_checksum_ok(buff, 20))
		ਵਾਪਸ 0;
	ਜੇ (ਲੜੀ_ਤੁਲਨਾ(rsdp->signature, "RSD PTR ", 8))
		ਵਾਪਸ 0;

	ਵਾਪਸ 1;
}

ਸਥਿਰ ਪੂਰਨ get_acpi_rsdp(ਖਾਲੀ)
{
	u16_t ebda;
	/*
	 * Read 40:0Eh - to find the starting address of the EBDA.
	 */
	acpi_phys_copy (0x40E, &ebda, ਆਕਾਰ(ebda));
	ਜੇ (ebda) {
		ebda <<= 4;
		ਜੇ(platform_tbl_ptr(ebda, ebda + 0x400, 16, &acpi_rsdp,
					ਆਕਾਰ(acpi_rsdp), &machine.acpi_rsdp,
					acpi_rsdp_test))
			ਵਾਪਸ 1;
	} 

	/* try BIOS read only mem space */
	ਜੇ(platform_tbl_ptr(0xE0000, 0x100000, 16, &acpi_rsdp,
				ਆਕਾਰ(acpi_rsdp), &machine.acpi_rsdp,
				acpi_rsdp_test))
		ਵਾਪਸ 1;
	
	machine.acpi_rsdp = 0; /* RSDP cannot be found at this address therefore
				  it is a valid negative value */
	ਵਾਪਸ 0;
}

ਸਥਿਰ ਖਾਲੀ acpi_init_poweroff(ਖਾਲੀ)
{
	u8_t *ptr = ਨਿਰਾ;
	u8_t *ਸ਼ੁਰੂ = ਨਿਰਾ;
	u8_t *ਸਿਰਾ = ਨਿਰਾ;
	ਬਣਤਰ acpi_fadt_ਸਿਰਲੇਖ *fadt_ਸਿਰਲੇਖ = ਨਿਰਾ;
	ਬਣਤਰ acpi_rsdt * dsdt_ਸਿਰਲੇਖ = ਨਿਰਾ;
	ਅੱਖਰ *ਸੁਨੇਹਾ_2 = ਨਿਰਾ;

	/* Everything used here existed since ACPI spec 1.0 */
	/* So we can safely use them */
	fadt_ਸਿਰਲੇਖ = (ਬਣਤਰ acpi_fadt_ਸਿਰਲੇਖ *)
		acpi_phys2vir(acpi_get_ਸਾਰਣੀ_ਅਧਾਰ("FACP"));
	ਜੇ (fadt_ਸਿਰਲੇਖ == ਨਿਰਾ) {
		ਸੁਨੇਹਾ_2 = "Could not load FACP";
		ਜਾਓ ਬਾਹਰ_ਨਿਕਲੋ;
	}

	dsdt_ਸਿਰਲੇਖ = (ਬਣਤਰ acpi_rsdt *)
		acpi_phys2vir((phys_ਬਾਈਟਾਂ) fadt_ਸਿਰਲੇਖ->dsdt);
	ਜੇ (dsdt_ਸਿਰਲੇਖ == ਨਿਰਾ) {
		ਸੁਨੇਹਾ_2 = "Could not load DSDT";
		ਜਾਓ ਬਾਹਰ_ਨਿਕਲੋ;
	}

	pm1a_cnt_blk = fadt_ਸਿਰਲੇਖ->pm1a_cnt_blk;
	pm1b_cnt_blk = fadt_ਸਿਰਲੇਖ->pm1b_cnt_blk;

	ptr = ਸ਼ੁਰੂ = (u8_t *) dsdt_ਸਿਰਲੇਖ->ਅੰਕੜੇ;
	ਸਿਰਾ = ਸ਼ੁਰੂ + dsdt_ਸਿਰਲੇਖ->hdr.ਲੰਮਾਈ - 4;

	/* See http://forum.osdev.org/viewtopic.php?t=16990 */
	/* for layout of \_S5 */
	ਜਦੋਂ (ptr < ਸਿਰਾ && memcmp(ptr, "_S5_", 4) != 0)
		ptr++;

	ਸੁਨੇਹਾ_2 = "Could not read S5 data. Use default SLP_TYPa and SLP_TYPb";
	ਜੇ (ptr >= ਸਿਰਾ || ptr == ਸ਼ੁਰੂ)
		ਜਾਓ ਬਾਹਰ_ਨਿਕਲੋ;

	/* validate AML structure */
	ਜੇ (*(ptr + AMI_S5_PACKAGE_OP_ਵਿੱਥ) != AMI_PACKAGE_OP_ਕੋਡ)
		ਜਾਓ ਬਾਹਰ_ਨਿਕਲੋ;

	ਜੇ ((ptr < ਸ਼ੁਰੂ + (-AMI_S5_NAME_OP_ਵਿੱਥ_2) ||
		(*(ptr + AMI_S5_NAME_OP_ਵਿੱਥ_2) != AMI_NAME_OP_ਕੋਡ ||
		 *(ptr + AMI_S5_NAME_OP_ਵਿੱਥ_2 + 1) != '\\')) &&
		*(ptr + AMI_S5_NAME_OP_ਵਿੱਥ_1) != AMI_NAME_OP_ਕੋਡ)
		ਜਾਓ ਬਾਹਰ_ਨਿਕਲੋ;

	ptr += AMI_S5_PACKET_ਲੰਮਾਈ_ਵਿੱਥ;
	ਜੇ (ptr >= ਸਿਰਾ)
		ਜਾਓ ਬਾਹਰ_ਨਿਕਲੋ;

	/* package length */
	ptr += ((*ptr & AMI_PACKAGE_ਲੰਮਾਈ_ENCODING_ਬਿੱਟ_ਨਕਾਬ) >>
		AMI_PACKAGE_ਲੰਮਾਈ_ENCODING_ਬਿੱਟ_ਖਿਸਕਾਓ) +
		AMI_MIN_PACKAGE_ਲੰਮਾਈ + AMI_ਅੰਕ_ELEMENTS_ਲੰਮਾਈ;
	ਜੇ (ptr >= ਸਿਰਾ)
		ਜਾਓ ਬਾਹਰ_ਨਿਕਲੋ;

	ਜੇ (*ptr == AMI_BYTE_ਅਗੇਤਰ_ਕੋਡ)
		ptr++; /* skip byte prefix */

	slp_typa = (*ptr) << AMI_SLP_TYPA_ਖਿਸਕਾਓ;

	ptr++; /* move to SLP_TYPb */
	ਜੇ (*ptr == AMI_BYTE_ਅਗੇਤਰ_ਕੋਡ)
		ptr++; /* skip byte prefix */

	slp_typb = (*ptr) << AMI_SLP_TYPB_ਖਿਸਕਾਓ;

	ਸੁਨੇਹਾ_2 = "poweroff initialized";

ਬਾਹਰ_ਨਿਕਲੋ:
	ਜੇ (ਸੁਨੇਹਾ_2) {
		DEBUGBASIC(("acpi: %s\n", ਸੁਨੇਹਾ_2));
	}
}

ਖਾਲੀ acpi_init(ਖਾਲੀ)
{
	ਪੂਰਨ s, i;
	read_func = acpi_phys_copy;

	ਜੇ (!get_acpi_rsdp()) {
		ਲਿਖੋ("WARNING : Cannot configure ACPI\n");
		ਵਾਪਸ;
	}
	
	s = acpi_read_sdt_at(acpi_rsdp.rsdt_addr, (ਬਣਤਰ acpi_sdt_ਸਿਰਲੇਖ *) &rsdt,
			ਆਕਾਰ(ਬਣਤਰ acpi_rsdt), ACPI_SDT_SIGNATURE(RSDT));

	sdt_ਗਿਣਤੀ = (s - ਆਕਾਰ(ਬਣਤਰ acpi_sdt_ਸਿਰਲੇਖ)) / ਆਕਾਰ(u32_t);

	ਲਈ (i = 0; i < sdt_ਗਿਣਤੀ; i++) {
		ਬਣਤਰ acpi_sdt_ਸਿਰਲੇਖ hdr;
		ਪੂਰਨ j;
		ਜੇ (read_func(rsdt.ਅੰਕੜੇ[i], &hdr, ਆਕਾਰ(ਬਣਤਰ acpi_sdt_ਸਿਰਲੇਖ))) {
			ਲਿਖੋ("ERROR acpi cannot read header at 0x%x\n",
								rsdt.ਅੰਕੜੇ[i]);
			ਵਾਪਸ;
		}

		ਲਈ (j = 0 ; j < ACPI_SDT_SIGNATURE_LEN; j++)
			sdt_trans[i].signature[j] = hdr.signature[j];
		sdt_trans[i].signature[ACPI_SDT_SIGNATURE_LEN] = '\0';
		sdt_trans[i].ਲੰਮਾਈ = hdr.ਲੰਮਾਈ;
	}

	acpi_init_poweroff();
}

ਬਣਤਰ acpi_madt_ioapic * acpi_get_ioapic_ਅਗਲਾ(ਖਾਲੀ)
{
	ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0;
	ਸਥਿਰ ਬਣਤਰ acpi_madt_hdr * madt_hdr;

	ਬਣਤਰ acpi_madt_ioapic * ret;

	ਜੇ (idx == 0) {
		madt_hdr = (ਬਣਤਰ acpi_madt_hdr *)
			acpi_phys2vir(acpi_get_ਸਾਰਣੀ_ਅਧਾਰ("APIC"));
		ਜੇ (madt_hdr == ਨਿਰਾ)
			ਵਾਪਸ ਨਿਰਾ;
	}

	ret = (ਬਣਤਰ acpi_madt_ioapic *)
		acpi_madt_get_typed_ਵਸਤੂ(madt_hdr, ACPI_MADT_TYPE_IOAPIC, idx);
	ਜੇ (ret)
		idx++;

	ਵਾਪਸ ret;
}

ਬਣਤਰ acpi_madt_lapic * acpi_get_lapic_ਅਗਲਾ(ਖਾਲੀ)
{
	ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0;
	ਸਥਿਰ ਬਣਤਰ acpi_madt_hdr * madt_hdr;

	ਬਣਤਰ acpi_madt_lapic * ret;

	ਜੇ (idx == 0) {
		madt_hdr = (ਬਣਤਰ acpi_madt_hdr *)
			acpi_phys2vir(acpi_get_ਸਾਰਣੀ_ਅਧਾਰ("APIC"));
		ਜੇ (madt_hdr == ਨਿਰਾ)
			ਵਾਪਸ ਨਿਰਾ;
	}

	ਲਈ (;;) {
		ret = (ਬਣਤਰ acpi_madt_lapic *)
			acpi_madt_get_typed_ਵਸਤੂ(madt_hdr,
					ACPI_MADT_TYPE_LAPIC, idx);
		ਜੇ (!ret)
			ਰੋਕੋ;

		idx++;

		/* report only usable CPUs */
		ਜੇ (ret->flags & 1)
			ਰੋਕੋ;
	}

	ਵਾਪਸ ret;
}

ਖਾਲੀ __k_unpaged_acpi_poweroff(ਖਾਲੀ)
{
	/* NO OP poweroff symbol*/
}

ਖਾਲੀ acpi_poweroff(ਖਾਲੀ)
{
	ਜੇ (pm1a_cnt_blk == 0) {
		ਵਾਪਸ;
	}
	outw(pm1a_cnt_blk, slp_typa | SLP_EN_ਕੋਡ);
	ਜੇ (pm1b_cnt_blk != 0) {
		outw(pm1b_cnt_blk, slp_typb | SLP_EN_ਕੋਡ);
	}
}
