
#ضمن <string.h>

#ضمن "acpi.h"
#ضمن "arch_proto.h"

تعريفنوع صحيح ((* acpi_read_t)(phys_bytes addr, فراغ * buff, size_t size));

بنية acpi_rsdp acpi_rsdp;

ساكن acpi_read_t read_func;

#define MAX_RSDT	35 /* ACPI defines 35 signatures */
#define SLP_EN_CODE	(1 << 13) /* ACPI SLP_EN_CODE code */
#define AMI_PACKAGE_OP_CODE (0x12)
#define AMI_NAME_OP_CODE (0x8)
#define AMI_BYTE_PREFIX_CODE (0xA)
#define AMI_PACKAGE_LENGTH_ENCODING_BITS_MASK (0xC0)
#define AMI_PACKAGE_LENGTH_ENCODING_BITS_SHIFT (6)
#define AMI_MIN_PACKAGE_LENGTH (1)
#define AMI_NUM_ELEMENTS_LENGTH (1)
#define AMI_SLP_TYPA_SHIFT (10)
#define AMI_SLP_TYPB_SHIFT (10)
#define AMI_S5_NAME_OP_OFFSET_1 (-1)
#define AMI_S5_NAME_OP_OFFSET_2 (-2)
#define AMI_S5_PACKAGE_OP_OFFSET (4)
#define AMI_S5_PACKET_LENGTH_OFFSET (5)

ساكن بنية acpi_rsdt {
	بنية acpi_sdt_header	hdr;
	u32_t			data[MAX_RSDT];
} rsdt;
	
ساكن بنية {
	حرف	signature [ACPI_SDT_SIGNATURE_LEN + 1];
	size_t	length;
} sdt_trans[MAX_RSDT];

ساكن صحيح sdt_count;
ساكن u16_t pm1a_cnt_blk = 0;
ساكن u16_t pm1b_cnt_blk = 0;
ساكن u16_t slp_typa = 0;
ساكن u16_t slp_typb = 0;

ساكن صحيح acpi_check_csum(بنية acpi_sdt_header * tb, size_t size)
{
	u8_t total = 0;
	صحيح i;
	עבור (i = 0; i < size; i++)
		total += ((unsigned حرف *)tb)[i];
	החזר total == 0 ? 0 : -1;
}

ساكن صحيح acpi_check_signature(ثابت حرف * orig, ثابت حرف * match)
{
	החזר strncmp(orig, match, ACPI_SDT_SIGNATURE_LEN);
}

ساكن u32_t acpi_phys2vir(u32_t p)
{
	אם(!vm_running) {
		DEBUGEXTRA(("acpi: returning 0x%lx as vir addr\n", p));
		החזר p;
	}
	panic("acpi: can't get virtual address of arbitrary physical address");
}

ساكن صحيح acpi_phys_copy(phys_bytes phys, فراغ *target, size_t len)
{
	אם(!vm_running) {
		memcpy(target, (فراغ *) phys, len);
		החזר 0;
	}
	panic("can't acpi_phys_copy with vm");
}

ساكن صحيح acpi_read_sdt_at(phys_bytes addr,
				بنية acpi_sdt_header * tb,
				size_t size,
				ثابت حرف * name)
{
	بنية acpi_sdt_header hdr;

	/* if NULL is supplied, we only return the size of the table */
	אם (tb == عدم) {
		אם (read_func(addr, &hdr, حجم(بنية acpi_sdt_header))) {
			printf("ERROR acpi cannot read %s header\n", name);
			החזר -1;
		}

		החזר hdr.length;
	}

	אם (read_func(addr, tb, حجم(بنية acpi_sdt_header))) {
		printf("ERROR acpi cannot read %s header\n", name);
		החזר -1;
	}

	אם (acpi_check_signature(tb->signature, name)) {
		printf("ERROR acpi %s signature does not match\n", name);
		החזר -1;
	}

	אם (size < tb->length) {
		printf("ERROR acpi buffer too small for %s\n", name);
		החזר -1;
	}

	אם (read_func(addr, tb, size)) {
		printf("ERROR acpi cannot read %s\n", name);
		החזר -1;
	}

	אם (acpi_check_csum(tb, tb->length)) {
		printf("ERROR acpi %s checksum does not match\n", name);
		החזר -1;
	}

	החזר tb->length;
}

phys_bytes acpi_get_table_base(ثابت حرف * name)
{
	صحيح i;

	עבור(i = 0; i < sdt_count; i++) {
		אם (strncmp(name, sdt_trans[i].signature,
					ACPI_SDT_SIGNATURE_LEN) == 0)
			החזר (phys_bytes) rsdt.data[i];
	}

	החזר (phys_bytes) عدم;
}

size_t acpi_get_table_length(ثابت حرف * name)
{
	صحيح i;

	עבור(i = 0; i < sdt_count; i++) {
		אם (strncmp(name, sdt_trans[i].signature,
					ACPI_SDT_SIGNATURE_LEN) == 0)
			החזר sdt_trans[i].length;
	}

	החזר 0;
}

ساكن فراغ * acpi_madt_get_typed_item(بنية acpi_madt_hdr * hdr,
					unsigned حرف type,
					unsigned idx)
{
	u8_t * t, * end;
	صحيح i;

	t = (u8_t *) hdr + حجم(بنية acpi_madt_hdr);
	end = (u8_t *) hdr + hdr->hdr.length;

	i = 0;
	כלעוד(t < end) {
		אם (type == ((بنية acpi_madt_item_hdr *) t)->type) {
			אם (i == idx)
				החזר t;
			אחרת
				i++;
		}
		t += ((بنية acpi_madt_item_hdr *) t)->length;
	}

	החזר عدم;
}

#אם 0
ساكن فراغ * acpi_madt_get_item(بنية acpi_madt_hdr * hdr,
				unsigned idx)
{
	u8_t * t, * end;
	صحيح i;

	t = (u8_t *) hdr + حجم(بنية acpi_madt_hdr);
	end = (u8_t *) hdr + hdr->hdr.length;

	עבור(i = 0 ; i <= idx && t < end; i++) {
		אם (i == idx)
			החזר t;
		t += ((بنية acpi_madt_item_hdr *) t)->length;
	}

	החזר عدم;
}
#endif

ساكن صحيح acpi_rsdp_test(فراغ * buff)
{
	بنية acpi_rsdp * rsdp = (بنية acpi_rsdp *) buff;

	אם (!platform_tbl_checksum_ok(buff, 20))
		החזר 0;
	אם (strncmp(rsdp->signature, "RSD PTR ", 8))
		החזר 0;

	החזר 1;
}

ساكن صحيح get_acpi_rsdp(فراغ)
{
	u16_t ebda;
	/*
	 * Read 40:0Eh - to find the starting address of the EBDA.
	 */
	acpi_phys_copy (0x40E, &ebda, حجم(ebda));
	אם (ebda) {
		ebda <<= 4;
		אם(platform_tbl_ptr(ebda, ebda + 0x400, 16, &acpi_rsdp,
					حجم(acpi_rsdp), &machine.acpi_rsdp,
					acpi_rsdp_test))
			החזר 1;
	} 

	/* try BIOS read only mem space */
	אם(platform_tbl_ptr(0xE0000, 0x100000, 16, &acpi_rsdp,
				حجم(acpi_rsdp), &machine.acpi_rsdp,
				acpi_rsdp_test))
		החזר 1;
	
	machine.acpi_rsdp = 0; /* RSDP cannot be found at this address therefore
				  it is a valid negative value */
	החזר 0;
}

ساكن فراغ acpi_init_poweroff(فراغ)
{
	u8_t *ptr = عدم;
	u8_t *start = عدم;
	u8_t *end = عدم;
	بنية acpi_fadt_header *fadt_header = عدم;
	بنية acpi_rsdt * dsdt_header = عدم;
	حرف *msg = عدم;

	/* Everything used here existed since ACPI spec 1.0 */
	/* So we can safely use them */
	fadt_header = (بنية acpi_fadt_header *)
		acpi_phys2vir(acpi_get_table_base("FACP"));
	אם (fadt_header == عدم) {
		msg = "Could not load FACP";
		اذهب exit;
	}

	dsdt_header = (بنية acpi_rsdt *)
		acpi_phys2vir((phys_bytes) fadt_header->dsdt);
	אם (dsdt_header == عدم) {
		msg = "Could not load DSDT";
		اذهب exit;
	}

	pm1a_cnt_blk = fadt_header->pm1a_cnt_blk;
	pm1b_cnt_blk = fadt_header->pm1b_cnt_blk;

	ptr = start = (u8_t *) dsdt_header->data;
	end = start + dsdt_header->hdr.length - 4;

	/* See http://forum.osdev.org/viewtopic.php?t=16990 */
	/* for layout of \_S5 */
	כלעוד (ptr < end && memcmp(ptr, "_S5_", 4) != 0)
		ptr++;

	msg = "Could not read S5 data. Use default SLP_TYPa and SLP_TYPb";
	אם (ptr >= end || ptr == start)
		اذهب exit;

	/* validate AML structure */
	אם (*(ptr + AMI_S5_PACKAGE_OP_OFFSET) != AMI_PACKAGE_OP_CODE)
		اذهب exit;

	אם ((ptr < start + (-AMI_S5_NAME_OP_OFFSET_2) ||
		(*(ptr + AMI_S5_NAME_OP_OFFSET_2) != AMI_NAME_OP_CODE ||
		 *(ptr + AMI_S5_NAME_OP_OFFSET_2 + 1) != '\\')) &&
		*(ptr + AMI_S5_NAME_OP_OFFSET_1) != AMI_NAME_OP_CODE)
		اذهب exit;

	ptr += AMI_S5_PACKET_LENGTH_OFFSET;
	אם (ptr >= end)
		اذهب exit;

	/* package length */
	ptr += ((*ptr & AMI_PACKAGE_LENGTH_ENCODING_BITS_MASK) >>
		AMI_PACKAGE_LENGTH_ENCODING_BITS_SHIFT) +
		AMI_MIN_PACKAGE_LENGTH + AMI_NUM_ELEMENTS_LENGTH;
	אם (ptr >= end)
		اذهب exit;

	אם (*ptr == AMI_BYTE_PREFIX_CODE)
		ptr++; /* skip byte prefix */

	slp_typa = (*ptr) << AMI_SLP_TYPA_SHIFT;

	ptr++; /* move to SLP_TYPb */
	אם (*ptr == AMI_BYTE_PREFIX_CODE)
		ptr++; /* skip byte prefix */

	slp_typb = (*ptr) << AMI_SLP_TYPB_SHIFT;

	msg = "poweroff initialized";

exit:
	אם (msg) {
		DEBUGBASIC(("acpi: %s\n", msg));
	}
}

فراغ acpi_init(فراغ)
{
	صحيح s, i;
	read_func = acpi_phys_copy;

	אם (!get_acpi_rsdp()) {
		printf("WARNING : Cannot configure ACPI\n");
		החזר;
	}
	
	s = acpi_read_sdt_at(acpi_rsdp.rsdt_addr, (بنية acpi_sdt_header *) &rsdt,
			حجم(بنية acpi_rsdt), ACPI_SDT_SIGNATURE(RSDT));

	sdt_count = (s - حجم(بنية acpi_sdt_header)) / حجم(u32_t);

	עבור (i = 0; i < sdt_count; i++) {
		بنية acpi_sdt_header hdr;
		صحيح j;
		אם (read_func(rsdt.data[i], &hdr, حجم(بنية acpi_sdt_header))) {
			printf("ERROR acpi cannot read header at 0x%x\n",
								rsdt.data[i]);
			החזר;
		}

		עבור (j = 0 ; j < ACPI_SDT_SIGNATURE_LEN; j++)
			sdt_trans[i].signature[j] = hdr.signature[j];
		sdt_trans[i].signature[ACPI_SDT_SIGNATURE_LEN] = '\0';
		sdt_trans[i].length = hdr.length;
	}

	acpi_init_poweroff();
}

بنية acpi_madt_ioapic * acpi_get_ioapic_next(فراغ)
{
	ساكن unsigned idx = 0;
	ساكن بنية acpi_madt_hdr * madt_hdr;

	بنية acpi_madt_ioapic * ret;

	אם (idx == 0) {
		madt_hdr = (بنية acpi_madt_hdr *)
			acpi_phys2vir(acpi_get_table_base("APIC"));
		אם (madt_hdr == عدم)
			החזר عدم;
	}

	ret = (بنية acpi_madt_ioapic *)
		acpi_madt_get_typed_item(madt_hdr, ACPI_MADT_TYPE_IOAPIC, idx);
	אם (ret)
		idx++;

	החזר ret;
}

بنية acpi_madt_lapic * acpi_get_lapic_next(فراغ)
{
	ساكن unsigned idx = 0;
	ساكن بنية acpi_madt_hdr * madt_hdr;

	بنية acpi_madt_lapic * ret;

	אם (idx == 0) {
		madt_hdr = (بنية acpi_madt_hdr *)
			acpi_phys2vir(acpi_get_table_base("APIC"));
		אם (madt_hdr == عدم)
			החזר عدم;
	}

	עבור (;;) {
		ret = (بنية acpi_madt_lapic *)
			acpi_madt_get_typed_item(madt_hdr,
					ACPI_MADT_TYPE_LAPIC, idx);
		אם (!ret)
			اكسر;

		idx++;

		/* report only usable CPUs */
		אם (ret->flags & 1)
			اكسر;
	}

	החזר ret;
}

فراغ __k_unpaged_acpi_poweroff(فراغ)
{
	/* NO OP poweroff symbol*/
}

فراغ acpi_poweroff(فراغ)
{
	אם (pm1a_cnt_blk == 0) {
		החזר;
	}
	outw(pm1a_cnt_blk, slp_typa | SLP_EN_CODE);
	אם (pm1b_cnt_blk != 0) {
		outw(pm1b_cnt_blk, slp_typb | SLP_EN_CODE);
	}
}
