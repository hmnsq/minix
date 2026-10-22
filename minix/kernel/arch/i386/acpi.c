
#ಅನ್ತರ್ಭಾವ <string.h>

#ಅನ್ತರ್ಭಾವ "acpi.h"
#ಅನ್ತರ್ಭಾವ "arch_proto.h"

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಪೂರ್ಣಾಂಕ ((* acpi_read_t)(phys_ಬೈಟುಗಳು addr, ಶೂನ್ಯ * buff, size_t ಗಾತ್ರ_2));

ರಚನೆ acpi_rsdp acpi_rsdp;

ಸ್ಥಿರ acpi_read_t read_func;

#ವ್ಯಾಖ್ಯಾನಿಸು MAX_RSDT	35 /* ACPI defines 35 signatures */
#ವ್ಯಾಖ್ಯಾನಿಸು SLP_EN_ಸಂಕೇತ	(1 << 13) /* ACPI SLP_EN_CODE code */
#ವ್ಯಾಖ್ಯಾನಿಸು AMI_PACKAGE_OP_ಸಂಕೇತ (0x12)
#ವ್ಯಾಖ್ಯಾನಿಸು AMI_NAME_OP_ಸಂಕೇತ (0x8)
#ವ್ಯಾಖ್ಯಾನಿಸು AMI_BYTE_ಪೂರ್ವಪ್ರತ್ಯಯ_ಸಂಕೇತ (0xA)
#ವ್ಯಾಖ್ಯಾನಿಸು AMI_PACKAGE_ಉದ್ದಳತೆ_ENCODING_ಬಿಟ್ಟುಗಳು_ಮುಖವಾಡ (0xC0)
#ವ್ಯಾಖ್ಯಾನಿಸು AMI_PACKAGE_ಉದ್ದಳತೆ_ENCODING_ಬಿಟ್ಟುಗಳು_ಸರಿಸು (6)
#ವ್ಯಾಖ್ಯಾನಿಸು AMI_MIN_PACKAGE_ಉದ್ದಳತೆ (1)
#ವ್ಯಾಖ್ಯಾನಿಸು AMI_ಅಂಕೆ_ELEMENTS_ಉದ್ದಳತೆ (1)
#ವ್ಯಾಖ್ಯಾನಿಸು AMI_SLP_TYPA_ಸರಿಸು (10)
#ವ್ಯಾಖ್ಯಾನಿಸು AMI_SLP_TYPB_ಸರಿಸು (10)
#ವ್ಯಾಖ್ಯಾನಿಸು AMI_S5_NAME_OP_ಸ್ಥಾನಾಂತರ_1 (-1)
#ವ್ಯಾಖ್ಯಾನಿಸು AMI_S5_NAME_OP_ಸ್ಥಾನಾಂತರ_2 (-2)
#ವ್ಯಾಖ್ಯಾನಿಸು AMI_S5_PACKAGE_OP_ಸ್ಥಾನಾಂತರ (4)
#ವ್ಯಾಖ್ಯಾನಿಸು AMI_S5_PACKET_ಉದ್ದಳತೆ_ಸ್ಥಾನಾಂತರ (5)

ಸ್ಥಿರ ರಚನೆ acpi_rsdt {
	ರಚನೆ acpi_sdt_ಶೀರ್ಷಿಕೆ	hdr;
	u32_t			ದತ್ತಾಂಶ[MAX_RSDT];
} rsdt;
	
ಸ್ಥಿರ ರಚನೆ {
	ಅಕ್ಷರ	signature [ACPI_SDT_SIGNATURE_LEN + 1];
	size_t	ಉದ್ದಳತೆ;
} sdt_trans[MAX_RSDT];

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ sdt_ಎಣಿಕೆ;
ಸ್ಥಿರ u16_t pm1a_cnt_blk = 0;
ಸ್ಥಿರ u16_t pm1b_cnt_blk = 0;
ಸ್ಥಿರ u16_t slp_typa = 0;
ಸ್ಥಿರ u16_t slp_typb = 0;

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ acpi_check_csum(ರಚನೆ acpi_sdt_ಶೀರ್ಷಿಕೆ * tb, size_t ಗಾತ್ರ_2)
{
	u8_t ಒಟ್ಟು = 0;
	ಪೂರ್ಣಾಂಕ i;
	ಗಾಗಿ (i = 0; i < ಗಾತ್ರ_2; i++)
		ಒಟ್ಟು += ((ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)tb)[i];
	ಹಿಂತಿರುಗಿ ಒಟ್ಟು == 0 ? 0 : -1;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ acpi_check_signature(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * orig, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * ಹೊಂದಿಕೆ)
{
	ಹಿಂತಿರುಗಿ ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(orig, ಹೊಂದಿಕೆ, ACPI_SDT_SIGNATURE_LEN);
}

ಸ್ಥಿರ u32_t acpi_phys2vir(u32_t p)
{
	ಆದರೆ(!vm_running) {
		DEBUGEXTRA(("acpi: returning 0x%lx as vir addr\n", p));
		ಹಿಂತಿರುಗಿ p;
	}
	panic("acpi: can't get virtual address of arbitrary physical address");
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ acpi_phys_copy(phys_ಬೈಟುಗಳು phys, ಶೂನ್ಯ *ಗುರಿ, size_t len)
{
	ಆದರೆ(!vm_running) {
		ಸ್ಮರಣೆ_ನಕಲು(ಗುರಿ, (ಶೂನ್ಯ *) phys, len);
		ಹಿಂತಿರುಗಿ 0;
	}
	panic("can't acpi_phys_copy with vm");
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ acpi_read_sdt_at(phys_ಬೈಟುಗಳು addr,
				ರಚನೆ acpi_sdt_ಶೀರ್ಷಿಕೆ * tb,
				size_t ಗಾತ್ರ_2,
				ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * name)
{
	ರಚನೆ acpi_sdt_ಶೀರ್ಷಿಕೆ hdr;

	/* if NULL is supplied, we only return the size of the table */
	ಆದರೆ (tb == ಖಾಲಿ) {
		ಆದರೆ (read_func(addr, &hdr, ಗಾತ್ರ(ರಚನೆ acpi_sdt_ಶೀರ್ಷಿಕೆ))) {
			ಬರೆ("ERROR acpi cannot read %s header\n", name);
			ಹಿಂತಿರುಗಿ -1;
		}

		ಹಿಂತಿರುಗಿ hdr.ಉದ್ದಳತೆ;
	}

	ಆದರೆ (read_func(addr, tb, ಗಾತ್ರ(ರಚನೆ acpi_sdt_ಶೀರ್ಷಿಕೆ))) {
		ಬರೆ("ERROR acpi cannot read %s header\n", name);
		ಹಿಂತಿರುಗಿ -1;
	}

	ಆದರೆ (acpi_check_signature(tb->signature, name)) {
		ಬರೆ("ERROR acpi %s signature does not match\n", name);
		ಹಿಂತಿರುಗಿ -1;
	}

	ಆದರೆ (ಗಾತ್ರ_2 < tb->ಉದ್ದಳತೆ) {
		ಬರೆ("ERROR acpi buffer too small for %s\n", name);
		ಹಿಂತಿರುಗಿ -1;
	}

	ಆದರೆ (read_func(addr, tb, ಗಾತ್ರ_2)) {
		ಬರೆ("ERROR acpi cannot read %s\n", name);
		ಹಿಂತಿರುಗಿ -1;
	}

	ಆದರೆ (acpi_check_csum(tb, tb->ಉದ್ದಳತೆ)) {
		ಬರೆ("ERROR acpi %s checksum does not match\n", name);
		ಹಿಂತಿರುಗಿ -1;
	}

	ಹಿಂತಿರುಗಿ tb->ಉದ್ದಳತೆ;
}

phys_ಬೈಟುಗಳು acpi_get_ಕೋಷ್ಟಕ_ತಳ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * name)
{
	ಪೂರ್ಣಾಂಕ i;

	ಗಾಗಿ(i = 0; i < sdt_ಎಣಿಕೆ; i++) {
		ಆದರೆ (ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(name, sdt_trans[i].signature,
					ACPI_SDT_SIGNATURE_LEN) == 0)
			ಹಿಂತಿರುಗಿ (phys_ಬೈಟುಗಳು) rsdt.ದತ್ತಾಂಶ[i];
	}

	ಹಿಂತಿರುಗಿ (phys_ಬೈಟುಗಳು) ಖಾಲಿ;
}

size_t acpi_get_ಕೋಷ್ಟಕ_ಉದ್ದಳತೆ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * name)
{
	ಪೂರ್ಣಾಂಕ i;

	ಗಾಗಿ(i = 0; i < sdt_ಎಣಿಕೆ; i++) {
		ಆದರೆ (ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(name, sdt_trans[i].signature,
					ACPI_SDT_SIGNATURE_LEN) == 0)
			ಹಿಂತಿರುಗಿ sdt_trans[i].ಉದ್ದಳತೆ;
	}

	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಶೂನ್ಯ * acpi_madt_get_typed_ಅಂಶ(ರಚನೆ acpi_madt_hdr * hdr,
					ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ type,
					ಚಿಹ್ನೆರಹಿತ idx)
{
	u8_t * t, * ಕೊನೆ;
	ಪೂರ್ಣಾಂಕ i;

	t = (u8_t *) hdr + ಗಾತ್ರ(ರಚನೆ acpi_madt_hdr);
	ಕೊನೆ = (u8_t *) hdr + hdr->hdr.ಉದ್ದಳತೆ;

	i = 0;
	ಆಗ(t < ಕೊನೆ) {
		ಆದರೆ (type == ((ರಚನೆ acpi_madt_ಅಂಶ_hdr *) t)->type) {
			ಆದರೆ (i == idx)
				ಹಿಂತಿರುಗಿ t;
			ಇಲ್ಲದಿದ್ದರೆ
				i++;
		}
		t += ((ರಚನೆ acpi_madt_ಅಂಶ_hdr *) t)->ಉದ್ದಳತೆ;
	}

	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

#ಆದರೆ 0
ಸ್ಥಿರ ಶೂನ್ಯ * acpi_madt_get_ಅಂಶ(ರಚನೆ acpi_madt_hdr * hdr,
				ಚಿಹ್ನೆರಹಿತ idx)
{
	u8_t * t, * ಕೊನೆ;
	ಪೂರ್ಣಾಂಕ i;

	t = (u8_t *) hdr + ಗಾತ್ರ(ರಚನೆ acpi_madt_hdr);
	ಕೊನೆ = (u8_t *) hdr + hdr->hdr.ಉದ್ದಳತೆ;

	ಗಾಗಿ(i = 0 ; i <= idx && t < ಕೊನೆ; i++) {
		ಆದರೆ (i == idx)
			ಹಿಂತಿರುಗಿ t;
		t += ((ರಚನೆ acpi_madt_ಅಂಶ_hdr *) t)->ಉದ್ದಳತೆ;
	}

	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ acpi_rsdp_test(ಶೂನ್ಯ * buff)
{
	ರಚನೆ acpi_rsdp * rsdp = (ರಚನೆ acpi_rsdp *) buff;

	ಆದರೆ (!platform_tbl_checksum_ok(buff, 20))
		ಹಿಂತಿರುಗಿ 0;
	ಆದರೆ (ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(rsdp->signature, "RSD PTR ", 8))
		ಹಿಂತಿರುಗಿ 0;

	ಹಿಂತಿರುಗಿ 1;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ get_acpi_rsdp(ಶೂನ್ಯ)
{
	u16_t ebda;
	/*
	 * Read 40:0Eh - to find the starting address of the EBDA.
	 */
	acpi_phys_copy (0x40E, &ebda, ಗಾತ್ರ(ebda));
	ಆದರೆ (ebda) {
		ebda <<= 4;
		ಆದರೆ(platform_tbl_ptr(ebda, ebda + 0x400, 16, &acpi_rsdp,
					ಗಾತ್ರ(acpi_rsdp), &machine.acpi_rsdp,
					acpi_rsdp_test))
			ಹಿಂತಿರುಗಿ 1;
	} 

	/* try BIOS read only mem space */
	ಆದರೆ(platform_tbl_ptr(0xE0000, 0x100000, 16, &acpi_rsdp,
				ಗಾತ್ರ(acpi_rsdp), &machine.acpi_rsdp,
				acpi_rsdp_test))
		ಹಿಂತಿರುಗಿ 1;
	
	machine.acpi_rsdp = 0; /* RSDP cannot be found at this address therefore
				  it is a valid negative value */
	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಶೂನ್ಯ acpi_init_poweroff(ಶೂನ್ಯ)
{
	u8_t *ptr = ಖಾಲಿ;
	u8_t *ಆರಂಭ = ಖಾಲಿ;
	u8_t *ಕೊನೆ = ಖಾಲಿ;
	ರಚನೆ acpi_fadt_ಶೀರ್ಷಿಕೆ *fadt_ಶೀರ್ಷಿಕೆ = ಖಾಲಿ;
	ರಚನೆ acpi_rsdt * dsdt_ಶೀರ್ಷಿಕೆ = ಖಾಲಿ;
	ಅಕ್ಷರ *ಸಂದೇಶ_2 = ಖಾಲಿ;

	/* Everything used here existed since ACPI spec 1.0 */
	/* So we can safely use them */
	fadt_ಶೀರ್ಷಿಕೆ = (ರಚನೆ acpi_fadt_ಶೀರ್ಷಿಕೆ *)
		acpi_phys2vir(acpi_get_ಕೋಷ್ಟಕ_ತಳ("FACP"));
	ಆದರೆ (fadt_ಶೀರ್ಷಿಕೆ == ಖಾಲಿ) {
		ಸಂದೇಶ_2 = "Could not load FACP";
		ಹೋಗು ನಿರ್ಗಮಿಸು;
	}

	dsdt_ಶೀರ್ಷಿಕೆ = (ರಚನೆ acpi_rsdt *)
		acpi_phys2vir((phys_ಬೈಟುಗಳು) fadt_ಶೀರ್ಷಿಕೆ->dsdt);
	ಆದರೆ (dsdt_ಶೀರ್ಷಿಕೆ == ಖಾಲಿ) {
		ಸಂದೇಶ_2 = "Could not load DSDT";
		ಹೋಗು ನಿರ್ಗಮಿಸು;
	}

	pm1a_cnt_blk = fadt_ಶೀರ್ಷಿಕೆ->pm1a_cnt_blk;
	pm1b_cnt_blk = fadt_ಶೀರ್ಷಿಕೆ->pm1b_cnt_blk;

	ptr = ಆರಂಭ = (u8_t *) dsdt_ಶೀರ್ಷಿಕೆ->ದತ್ತಾಂಶ;
	ಕೊನೆ = ಆರಂಭ + dsdt_ಶೀರ್ಷಿಕೆ->hdr.ಉದ್ದಳತೆ - 4;

	/* See http://forum.osdev.org/viewtopic.php?t=16990 */
	/* for layout of \_S5 */
	ಆಗ (ptr < ಕೊನೆ && memcmp(ptr, "_S5_", 4) != 0)
		ptr++;

	ಸಂದೇಶ_2 = "Could not read S5 data. Use default SLP_TYPa and SLP_TYPb";
	ಆದರೆ (ptr >= ಕೊನೆ || ptr == ಆರಂಭ)
		ಹೋಗು ನಿರ್ಗಮಿಸು;

	/* validate AML structure */
	ಆದರೆ (*(ptr + AMI_S5_PACKAGE_OP_ಸ್ಥಾನಾಂತರ) != AMI_PACKAGE_OP_ಸಂಕೇತ)
		ಹೋಗು ನಿರ್ಗಮಿಸು;

	ಆದರೆ ((ptr < ಆರಂಭ + (-AMI_S5_NAME_OP_ಸ್ಥಾನಾಂತರ_2) ||
		(*(ptr + AMI_S5_NAME_OP_ಸ್ಥಾನಾಂತರ_2) != AMI_NAME_OP_ಸಂಕೇತ ||
		 *(ptr + AMI_S5_NAME_OP_ಸ್ಥಾನಾಂತರ_2 + 1) != '\\')) &&
		*(ptr + AMI_S5_NAME_OP_ಸ್ಥಾನಾಂತರ_1) != AMI_NAME_OP_ಸಂಕೇತ)
		ಹೋಗು ನಿರ್ಗಮಿಸು;

	ptr += AMI_S5_PACKET_ಉದ್ದಳತೆ_ಸ್ಥಾನಾಂತರ;
	ಆದರೆ (ptr >= ಕೊನೆ)
		ಹೋಗು ನಿರ್ಗಮಿಸು;

	/* package length */
	ptr += ((*ptr & AMI_PACKAGE_ಉದ್ದಳತೆ_ENCODING_ಬಿಟ್ಟುಗಳು_ಮುಖವಾಡ) >>
		AMI_PACKAGE_ಉದ್ದಳತೆ_ENCODING_ಬಿಟ್ಟುಗಳು_ಸರಿಸು) +
		AMI_MIN_PACKAGE_ಉದ್ದಳತೆ + AMI_ಅಂಕೆ_ELEMENTS_ಉದ್ದಳತೆ;
	ಆದರೆ (ptr >= ಕೊನೆ)
		ಹೋಗು ನಿರ್ಗಮಿಸು;

	ಆದರೆ (*ptr == AMI_BYTE_ಪೂರ್ವಪ್ರತ್ಯಯ_ಸಂಕೇತ)
		ptr++; /* skip byte prefix */

	slp_typa = (*ptr) << AMI_SLP_TYPA_ಸರಿಸು;

	ptr++; /* move to SLP_TYPb */
	ಆದರೆ (*ptr == AMI_BYTE_ಪೂರ್ವಪ್ರತ್ಯಯ_ಸಂಕೇತ)
		ptr++; /* skip byte prefix */

	slp_typb = (*ptr) << AMI_SLP_TYPB_ಸರಿಸು;

	ಸಂದೇಶ_2 = "poweroff initialized";

ನಿರ್ಗಮಿಸು:
	ಆದರೆ (ಸಂದೇಶ_2) {
		DEBUGBASIC(("acpi: %s\n", ಸಂದೇಶ_2));
	}
}

ಶೂನ್ಯ acpi_init(ಶೂನ್ಯ)
{
	ಪೂರ್ಣಾಂಕ s, i;
	read_func = acpi_phys_copy;

	ಆದರೆ (!get_acpi_rsdp()) {
		ಬರೆ("WARNING : Cannot configure ACPI\n");
		ಹಿಂತಿರುಗಿ;
	}
	
	s = acpi_read_sdt_at(acpi_rsdp.rsdt_addr, (ರಚನೆ acpi_sdt_ಶೀರ್ಷಿಕೆ *) &rsdt,
			ಗಾತ್ರ(ರಚನೆ acpi_rsdt), ACPI_SDT_SIGNATURE(RSDT));

	sdt_ಎಣಿಕೆ = (s - ಗಾತ್ರ(ರಚನೆ acpi_sdt_ಶೀರ್ಷಿಕೆ)) / ಗಾತ್ರ(u32_t);

	ಗಾಗಿ (i = 0; i < sdt_ಎಣಿಕೆ; i++) {
		ರಚನೆ acpi_sdt_ಶೀರ್ಷಿಕೆ hdr;
		ಪೂರ್ಣಾಂಕ j;
		ಆದರೆ (read_func(rsdt.ದತ್ತಾಂಶ[i], &hdr, ಗಾತ್ರ(ರಚನೆ acpi_sdt_ಶೀರ್ಷಿಕೆ))) {
			ಬರೆ("ERROR acpi cannot read header at 0x%x\n",
								rsdt.ದತ್ತಾಂಶ[i]);
			ಹಿಂತಿರುಗಿ;
		}

		ಗಾಗಿ (j = 0 ; j < ACPI_SDT_SIGNATURE_LEN; j++)
			sdt_trans[i].signature[j] = hdr.signature[j];
		sdt_trans[i].signature[ACPI_SDT_SIGNATURE_LEN] = '\0';
		sdt_trans[i].ಉದ್ದಳತೆ = hdr.ಉದ್ದಳತೆ;
	}

	acpi_init_poweroff();
}

ರಚನೆ acpi_madt_ioapic * acpi_get_ioapic_ಮುಂದಿನ(ಶೂನ್ಯ)
{
	ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ idx = 0;
	ಸ್ಥಿರ ರಚನೆ acpi_madt_hdr * madt_hdr;

	ರಚನೆ acpi_madt_ioapic * ret;

	ಆದರೆ (idx == 0) {
		madt_hdr = (ರಚನೆ acpi_madt_hdr *)
			acpi_phys2vir(acpi_get_ಕೋಷ್ಟಕ_ತಳ("APIC"));
		ಆದರೆ (madt_hdr == ಖಾಲಿ)
			ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}

	ret = (ರಚನೆ acpi_madt_ioapic *)
		acpi_madt_get_typed_ಅಂಶ(madt_hdr, ACPI_MADT_TYPE_IOAPIC, idx);
	ಆದರೆ (ret)
		idx++;

	ಹಿಂತಿರುಗಿ ret;
}

ರಚನೆ acpi_madt_lapic * acpi_get_lapic_ಮುಂದಿನ(ಶೂನ್ಯ)
{
	ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ idx = 0;
	ಸ್ಥಿರ ರಚನೆ acpi_madt_hdr * madt_hdr;

	ರಚನೆ acpi_madt_lapic * ret;

	ಆದರೆ (idx == 0) {
		madt_hdr = (ರಚನೆ acpi_madt_hdr *)
			acpi_phys2vir(acpi_get_ಕೋಷ್ಟಕ_ತಳ("APIC"));
		ಆದರೆ (madt_hdr == ಖಾಲಿ)
			ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}

	ಗಾಗಿ (;;) {
		ret = (ರಚನೆ acpi_madt_lapic *)
			acpi_madt_get_typed_ಅಂಶ(madt_hdr,
					ACPI_MADT_TYPE_LAPIC, idx);
		ಆದರೆ (!ret)
			ನಿಲ್ಲಿಸು;

		idx++;

		/* report only usable CPUs */
		ಆದರೆ (ret->flags & 1)
			ನಿಲ್ಲಿಸು;
	}

	ಹಿಂತಿರುಗಿ ret;
}

ಶೂನ್ಯ __k_unpaged_acpi_poweroff(ಶೂನ್ಯ)
{
	/* NO OP poweroff symbol*/
}

ಶೂನ್ಯ acpi_poweroff(ಶೂನ್ಯ)
{
	ಆದರೆ (pm1a_cnt_blk == 0) {
		ಹಿಂತಿರುಗಿ;
	}
	outw(pm1a_cnt_blk, slp_typa | SLP_EN_ಸಂಕೇತ);
	ಆದರೆ (pm1b_cnt_blk != 0) {
		outw(pm1b_cnt_blk, slp_typb | SLP_EN_ಸಂಕೇತ);
	}
}
