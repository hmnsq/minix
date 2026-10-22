
#ವ್ಯಾಖ್ಯಾನಿಸು UNPAGED 1	/* for proper kmain() prototype */

#ಅನ್ತರ್ಭಾವ <assert.h>
#ಅನ್ತರ್ಭಾವ <stdlib.h>
#ಅನ್ತರ್ಭಾವ <minix/minlib.h>
#ಅನ್ತರ್ಭಾವ <minix/board.h>
#ಅನ್ತರ್ಭಾವ <sys/reboot.h>
#ಅನ್ತರ್ಭಾವ <machine/partition.h>
#ಅನ್ತರ್ಭಾವ "string.h"
#ಅನ್ತರ್ಭಾವ "direct_utils.h"
#ಅನ್ತರ್ಭಾವ "serial.h"
#ಅನ್ತರ್ಭಾವ "glo.h"

#ಆದರೆ USE_SYSDEBUG
#ವ್ಯಾಖ್ಯಾನಿಸು MULTIBOOT_VERBOSE 1
#ಆದರೆ_ಅಂತ್ಯ

/* to-be-built kinfo struct, diagnostics buffer */
kinfo_t kinfo;
ರಚನೆ kmessages kmessages;

/* pg_utils.c uses this; in this phase, there is a 1:1 mapping. */
phys_ಬೈಟುಗಳು vir2phys(ಶೂನ್ಯ *addr) { ಹಿಂತಿರುಗಿ (phys_ಬೈಟುಗಳು) addr; } 

/* mb_utils.c uses this; we can reach it directly */
ಅಕ್ಷರ *video_mem = (ಅಕ್ಷರ *) MULTIBOOT_VIDEO_ಬಫರ್;

/* String length used for mb_itoa */
#ವ್ಯಾಖ್ಯಾನಿಸು ITOA_ಬಫರ್_ಗಾತ್ರ 20

/* Kernel may use memory */
ಪೂರ್ಣಾಂಕ kernel_may_alloc = 1;

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ mb_ಗಣ_param(ಅಕ್ಷರ *bigbuf, ಅಕ್ಷರ *name, ಅಕ್ಷರ *ಮೌಲ್ಯ, kinfo_t *cbi) 
{
	ಅಕ್ಷರ *p = bigbuf;
	ಅಕ್ಷರ *bufend = bigbuf + MULTIBOOT_PARAM_BUF_ಗಾತ್ರ;
	ಅಕ್ಷರ *q;
	ಪೂರ್ಣಾಂಕ namelen = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(name);
	ಪೂರ್ಣಾಂಕ valuelen = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ಮೌಲ್ಯ);

	/* Some variables we recognize */
	ಆದರೆ(!strcmp(name, SERVARNAME)) { cbi->do_serial_debug = 1; }
	ಆದರೆ(!strcmp(name, SERBAUDVARNAME)) { cbi->serial_debug_baud = atoi(ಮೌಲ್ಯ); }

	/* Delete the item if already exists */
	ಆಗ (*p) {
		ಆದರೆ (ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(p, name, namelen) == 0 && p[namelen] == '=') {
			q = p;
			ಆಗ (*q) q++;
			ಗಾಗಿ (q++; q < bufend; q++, p++)
				*p = *q;
			ನಿಲ್ಲಿಸು;
		}
		ಆಗ (*p++)
			;
		p++;
	}
	
	ಗಾಗಿ (p = bigbuf; p < bufend && (*p || *(p + 1)); p++)
		;
	ಆದರೆ (p > bigbuf) p++;
	
	/* Make sure there's enough space for the new parameter */
	ಆದರೆ (p + namelen + valuelen + 3 > bufend)
		ಹಿಂತಿರುಗಿ -1;
	
	strcpy(p, name);
	p[namelen] = '=';
	strcpy(p + namelen + 1, ಮೌಲ್ಯ);
	p[namelen + valuelen + 1] = 0;
	p[namelen + valuelen + 2] = 0;
	ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ overlaps(multiboot_module_t *mod, ಪೂರ್ಣಾಂಕ n, ಪೂರ್ಣಾಂಕ cmp_mod)
{
	multiboot_module_t *cmp = &mod[cmp_mod];
	ಪೂರ್ಣಾಂಕ m;

#ವ್ಯಾಖ್ಯಾನಿಸು INRANGE(mod, v) ((v) >= mod->mod_ಆರಂಭ && (v) < mod->mod_ಕೊನೆ)
#ವ್ಯಾಖ್ಯಾನಿಸು OVERLAP(mod1, mod2) (INRANGE(mod1, mod2->mod_ಆರಂಭ) || \
			INRANGE(mod1, mod2->mod_ಕೊನೆ-1))
	ಗಾಗಿ(m = 0; m < n; m++) {
		multiboot_module_t *thismod = &mod[m];
		ಆದರೆ(m == cmp_mod) ಮುಂದುವರಿ;
		ಆದರೆ(OVERLAP(thismod, cmp))
			ಹಿಂತಿರುಗಿ 1;
	}
	ಹಿಂತಿರುಗಿ 0;
}

ಶೂನ್ಯ get_parameters(u32_t ebx, kinfo_t *cbi) 
{
	multiboot_ಸ್ಮರಣೆ_map_t *mmap;
	multiboot_info_t *mbi = &cbi->mbi;
	ಪೂರ್ಣಾಂಕ var_i,ಮೌಲ್ಯ_i, m, k;
	ಅಕ್ಷರ *p;
	ಬಾಹ್ಯ ಅಕ್ಷರ kern_phys_ತಳ, kern_vir_ತಳ, kern_ಗಾತ್ರ,
		kern_unpaged_ಆರಂಭ, kern_unpaged_ಕೊನೆ;
	phys_ಬೈಟುಗಳು kernbase = (phys_ಬೈಟುಗಳು) &kern_phys_ತಳ,
		kernsize = (phys_ಬೈಟುಗಳು) &kern_ಗಾತ್ರ;
#ವ್ಯಾಖ್ಯಾನಿಸು BUF 1024
	ಸ್ಥಿರ ಅಕ್ಷರ cmdline[BUF];

	/* get our own copy of the multiboot info struct and module list */
	ಸ್ಮರಣೆ_ನಕಲು((ಶೂನ್ಯ *) mbi, (ಶೂನ್ಯ *) ebx, ಗಾತ್ರ(*mbi));

	/* Set various bits of info for the higher-level kernel. */
	cbi->mem_ಮೇಲಿನ_phys = 0;
	cbi->user_sp = (vir_ಬೈಟುಗಳು) &kern_vir_ತಳ;
	cbi->vir_kern_ಆರಂಭ = (vir_ಬೈಟುಗಳು) &kern_vir_ತಳ;
	cbi->bootstrap_ಆರಂಭ = (vir_ಬೈಟುಗಳು) &kern_unpaged_ಆರಂಭ;
	cbi->bootstrap_len = (vir_ಬೈಟುಗಳು) &kern_unpaged_ಕೊನೆ -
		cbi->bootstrap_ಆರಂಭ;
	cbi->kmess = &kmess;

	/* set some configurable defaults */
	cbi->do_serial_debug = 0;
	cbi->serial_debug_baud = 115200;

	/* parse boot command line */
	ಆದರೆ (mbi->mi_flags & MULTIBOOT_INFO_HAS_CMDLINE) {
		ಸ್ಥಿರ ಅಕ್ಷರ var[BUF];
		ಸ್ಥಿರ ಅಕ್ಷರ ಮೌಲ್ಯ[BUF];

		/* Override values with cmdline argument */
		ಸ್ಮರಣೆ_ನಕಲು(cmdline, (ಶೂನ್ಯ *) mbi->mi_cmdline, BUF);
		p = cmdline;
		ಆಗ (*p) {
			var_i = 0;
			ಮೌಲ್ಯ_i = 0;
			ಆಗ (*p == ' ') p++;
			ಆದರೆ (!*p) ನಿಲ್ಲಿಸು;
			ಆಗ (*p && *p != '=' && *p != ' ' && var_i < BUF - 1) 
				var[var_i++] = *p++ ;
			var[var_i] = 0;
			ಆದರೆ (*p++ != '=') ಮುಂದುವರಿ; /* skip if not name=value */
			ಆಗ (*p && *p != ' ' && ಮೌಲ್ಯ_i < BUF - 1) 
				ಮೌಲ್ಯ[ಮೌಲ್ಯ_i++] = *p++ ;
			ಮೌಲ್ಯ[ಮೌಲ್ಯ_i] = 0;
			
			mb_ಗಣ_param(cbi->param_buf, var, ಮೌಲ್ಯ, cbi);
		}
	}

        /* let higher levels know what we are booting on */
        mb_ಗಣ_param(cbi->param_buf, ARCHVARNAME, (ಅಕ್ಷರ *)get_board_arch_name(BOARD_ID_INTEL), cbi);
	mb_ಗಣ_param(cbi->param_buf, BOARDVARNAME,(ಅಕ್ಷರ *)get_board_name(BOARD_ID_INTEL) , cbi);

	/* move user stack/data down to leave a gap to catch kernel
	 * stack overflow; and to distinguish kernel and user addresses
	 * at a glance (0xf.. vs 0xe..) 
	 */
	cbi->user_sp = USR_STACKTOP;
	cbi->user_ಕೊನೆ = USR_DATATOP;

	/* kernel bytes without bootstrap code/data that is currently
	 * still needed but will be freed after bootstrapping.
	 */
	kinfo.kernel_allocated_ಬೈಟುಗಳು = (phys_ಬೈಟುಗಳು) &kern_ಗಾತ್ರ;
	kinfo.kernel_allocated_ಬೈಟುಗಳು -= cbi->bootstrap_len;

	ಪ್ರತಿಜ್ಞಾ(!(cbi->bootstrap_ಆರಂಭ % I386_ಪುಟ_ಗಾತ್ರ));
	cbi->bootstrap_len = rounddown(cbi->bootstrap_len, I386_ಪುಟ_ಗಾತ್ರ);
	ಪ್ರತಿಜ್ಞಾ(mbi->mi_flags & MULTIBOOT_INFO_HAS_MODS);
	ಪ್ರತಿಜ್ಞಾ(mbi->mi_mods_ಎಣಿಕೆ < MULTIBOOT_MAX_MODS);
	ಪ್ರತಿಜ್ಞಾ(mbi->mi_mods_ಎಣಿಕೆ > 0);
	ಸ್ಮರಣೆ_ನಕಲು(&cbi->module_list, (ಶೂನ್ಯ *) mbi->mi_mods_addr,
		mbi->mi_mods_ಎಣಿಕೆ * ಗಾತ್ರ(multiboot_module_t));
	
	memset(cbi->memmap, 0, ಗಾತ್ರ(cbi->memmap));
	/* mem_map has a variable layout */
	ಆದರೆ(mbi->mi_flags & MULTIBOOT_INFO_HAS_MMAP) {
		cbi->mmap_ಗಾತ್ರ = 0;
	        ಗಾಗಿ (mmap = (multiboot_ಸ್ಮರಣೆ_map_t *) mbi->mmap_addr;
       	     (ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ) mmap < mbi->mmap_addr + mbi->mmap_ಉದ್ದಳತೆ;
       	       mmap = (multiboot_ಸ್ಮರಣೆ_map_t *) 
		      	((ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ) mmap + mmap->mm_ಗಾತ್ರ + ಗಾತ್ರ(mmap->mm_ಗಾತ್ರ))) {
			ಆದರೆ(mmap->mm_type != MULTIBOOT_ಸ್ಮರಣೆ_ಲಭ್ಯ) ಮುಂದುವರಿ;
			ಕೂಡಿಸು_memmap(cbi, mmap->mm_ತಳ_addr, mmap->mm_ಉದ್ದಳತೆ);
		}
	} ಇಲ್ಲದಿದ್ದರೆ {
		ಪ್ರತಿಜ್ಞಾ(mbi->mi_flags & MULTIBOOT_INFO_HAS_ಸ್ಮರಣೆ);
		ಕೂಡಿಸು_memmap(cbi, 0, mbi->mi_mem_ಅಧೋ*1024);
		ಕೂಡಿಸು_memmap(cbi, 0x100000, mbi->mi_mem_ಊರ್ಧ್ವ*1024);
	}

	/* Sanity check: the kernel nor any of the modules may overlap
	 * with each other. Pretend the kernel is an extra module for a
	 * second.
	 */
	k = mbi->mi_mods_ಎಣಿಕೆ;
	ಪ್ರತಿಜ್ಞಾ(k < MULTIBOOT_MAX_MODS);
	cbi->module_list[k].mod_ಆರಂಭ = kernbase;
	cbi->module_list[k].mod_ಕೊನೆ = kernbase + kernsize;
	cbi->mods_with_kernel = mbi->mi_mods_ಎಣಿಕೆ+1;
	cbi->kern_mod = k;

	ಗಾಗಿ(m = 0; m < cbi->mods_with_kernel; m++) {
#ಆದರೆ 0
		ಬರೆ("checking overlap of module %08lx-%08lx\n",
		  cbi->module_list[m].mod_ಆರಂಭ, cbi->module_list[m].mod_ಕೊನೆ);
#ಆದರೆ_ಅಂತ್ಯ
		ಆದರೆ(overlaps(cbi->module_list, cbi->mods_with_kernel, m))
			panic("overlapping boot modules/kernel");
		/* We cut out the bits of memory that we know are
		 * occupied by the kernel and boot modules.
		 */
		cut_memmap(cbi,
			cbi->module_list[m].mod_ಆರಂಭ, 
			cbi->module_list[m].mod_ಕೊನೆ);
	}
}

kinfo_t *pre_init(u32_t magic, u32_t ebx)
{
	ಪ್ರತಿಜ್ಞಾ(magic == MULTIBOOT_INFO_MAGIC);

	/* Get our own copy boot params pointed to by ebx.
	 * Here we find out whether we should do serial output.
	 */
	get_parameters(ebx, &kinfo);

	/* Make and load a pagetable that will map the kernel
	 * to where it should be; but first a 1:1 mapping so
	 * this code stays where it should be.
	 */
	pg_clear();
	pg_ತಟಸ್ಥ(&kinfo);
	kinfo.freepde_ಆರಂಭ = pg_mapkernel();
	pg_ತುಂಬು();
	vm_enable_paging();

	/* Done, return boot info so it can be passed to kmain(). */
	ಹಿಂತಿರುಗಿ &kinfo;
}

ಶೂನ್ಯ send_diag_sig(ಶೂನ್ಯ) { }
ಶೂನ್ಯ minix_shutdown(ಪೂರ್ಣಾಂಕ how) { arch_shutdown(how); }
ಶೂನ್ಯ busy_delay_ms(ಪೂರ್ಣಾಂಕ x) { }
ಪೂರ್ಣಾಂಕ ಎತ್ತು(ಪೂರ್ಣಾಂಕ sig) { panic("raise(%d)\n", sig); }
