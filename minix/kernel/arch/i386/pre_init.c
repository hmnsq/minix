
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UNPAGED 1	/* for proper kmain() prototype */

#ਅਨ੍ਤਰ੍ਭਾਵ <assert.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdlib.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <minix/minlib.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <minix/board.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/reboot.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <machine/partition.h>
#ਅਨ੍ਤਰ੍ਭਾਵ "string.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "direct_utils.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "serial.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "glo.h"

#ਜੇ USE_SYSDEBUG
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MULTIBOOT_VERBOSE 1
#ਜੇ_ਅੰਤ

/* to-be-built kinfo struct, diagnostics buffer */
kinfo_t kinfo;
ਬਣਤਰ kmessages kmessages;

/* pg_utils.c uses this; in this phase, there is a 1:1 mapping. */
phys_ਬਾਈਟਾਂ vir2phys(ਖਾਲੀ *addr) { ਵਾਪਸ (phys_ਬਾਈਟਾਂ) addr; } 

/* mb_utils.c uses this; we can reach it directly */
ਅੱਖਰ *video_mem = (ਅੱਖਰ *) MULTIBOOT_VIDEO_ਬਫਰ;

/* String length used for mb_itoa */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ITOA_ਬਫਰ_ਮਾਪ 20

/* Kernel may use memory */
ਪੂਰਨ kernel_may_alloc = 1;

ਸਥਿਰ ਪੂਰਨ mb_ਸੈੱਟ_param(ਅੱਖਰ *bigbuf, ਅੱਖਰ *name, ਅੱਖਰ *ਮੁੱਲ, kinfo_t *cbi) 
{
	ਅੱਖਰ *p = bigbuf;
	ਅੱਖਰ *bufend = bigbuf + MULTIBOOT_PARAM_BUF_ਮਾਪ;
	ਅੱਖਰ *q;
	ਪੂਰਨ namelen = ਲੜੀ_ਲੰਬਾਈ(name);
	ਪੂਰਨ valuelen = ਲੜੀ_ਲੰਬਾਈ(ਮੁੱਲ);

	/* Some variables we recognize */
	ਜੇ(!strcmp(name, SERVARNAME)) { cbi->do_serial_debug = 1; }
	ਜੇ(!strcmp(name, SERBAUDVARNAME)) { cbi->serial_debug_baud = atoi(ਮੁੱਲ); }

	/* Delete the item if already exists */
	ਜਦੋਂ (*p) {
		ਜੇ (ਲੜੀ_ਤੁਲਨਾ(p, name, namelen) == 0 && p[namelen] == '=') {
			q = p;
			ਜਦੋਂ (*q) q++;
			ਲਈ (q++; q < bufend; q++, p++)
				*p = *q;
			ਰੋਕੋ;
		}
		ਜਦੋਂ (*p++)
			;
		p++;
	}
	
	ਲਈ (p = bigbuf; p < bufend && (*p || *(p + 1)); p++)
		;
	ਜੇ (p > bigbuf) p++;
	
	/* Make sure there's enough space for the new parameter */
	ਜੇ (p + namelen + valuelen + 3 > bufend)
		ਵਾਪਸ -1;
	
	strcpy(p, name);
	p[namelen] = '=';
	strcpy(p + namelen + 1, ਮੁੱਲ);
	p[namelen + valuelen + 1] = 0;
	p[namelen + valuelen + 2] = 0;
	ਵਾਪਸ 0;
}

ਪੂਰਨ overlaps(multiboot_module_t *mod, ਪੂਰਨ n, ਪੂਰਨ cmp_mod)
{
	multiboot_module_t *cmp = &mod[cmp_mod];
	ਪੂਰਨ m;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ INRANGE(mod, v) ((v) >= mod->mod_ਸ਼ੁਰੂ && (v) < mod->mod_ਸਿਰਾ)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OVERLAP(mod1, mod2) (INRANGE(mod1, mod2->mod_ਸ਼ੁਰੂ) || \
			INRANGE(mod1, mod2->mod_ਸਿਰਾ-1))
	ਲਈ(m = 0; m < n; m++) {
		multiboot_module_t *thismod = &mod[m];
		ਜੇ(m == cmp_mod) ਜਾਰੀ;
		ਜੇ(OVERLAP(thismod, cmp))
			ਵਾਪਸ 1;
	}
	ਵਾਪਸ 0;
}

ਖਾਲੀ get_parameters(u32_t ebx, kinfo_t *cbi) 
{
	multiboot_ਸਮ੍ਰਿਤੀ_map_t *mmap;
	multiboot_info_t *mbi = &cbi->mbi;
	ਪੂਰਨ var_i,ਮੁੱਲ_i, m, k;
	ਅੱਖਰ *p;
	ਬਾਹਰੀ ਅੱਖਰ kern_phys_ਅਧਾਰ, kern_vir_ਅਧਾਰ, kern_ਮਾਪ,
		kern_unpaged_ਸ਼ੁਰੂ, kern_unpaged_ਸਿਰਾ;
	phys_ਬਾਈਟਾਂ kernbase = (phys_ਬਾਈਟਾਂ) &kern_phys_ਅਧਾਰ,
		kernsize = (phys_ਬਾਈਟਾਂ) &kern_ਮਾਪ;
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BUF 1024
	ਸਥਿਰ ਅੱਖਰ cmdline[BUF];

	/* get our own copy of the multiboot info struct and module list */
	ਸਮ੍ਰਿਤੀ_ਨਕਲ((ਖਾਲੀ *) mbi, (ਖਾਲੀ *) ebx, ਆਕਾਰ(*mbi));

	/* Set various bits of info for the higher-level kernel. */
	cbi->mem_ਉੱਚਾ_phys = 0;
	cbi->user_sp = (vir_ਬਾਈਟਾਂ) &kern_vir_ਅਧਾਰ;
	cbi->vir_kern_ਸ਼ੁਰੂ = (vir_ਬਾਈਟਾਂ) &kern_vir_ਅਧਾਰ;
	cbi->bootstrap_ਸ਼ੁਰੂ = (vir_ਬਾਈਟਾਂ) &kern_unpaged_ਸ਼ੁਰੂ;
	cbi->bootstrap_len = (vir_ਬਾਈਟਾਂ) &kern_unpaged_ਸਿਰਾ -
		cbi->bootstrap_ਸ਼ੁਰੂ;
	cbi->kmess = &kmess;

	/* set some configurable defaults */
	cbi->do_serial_debug = 0;
	cbi->serial_debug_baud = 115200;

	/* parse boot command line */
	ਜੇ (mbi->mi_flags & MULTIBOOT_INFO_HAS_CMDLINE) {
		ਸਥਿਰ ਅੱਖਰ var[BUF];
		ਸਥਿਰ ਅੱਖਰ ਮੁੱਲ[BUF];

		/* Override values with cmdline argument */
		ਸਮ੍ਰਿਤੀ_ਨਕਲ(cmdline, (ਖਾਲੀ *) mbi->mi_cmdline, BUF);
		p = cmdline;
		ਜਦੋਂ (*p) {
			var_i = 0;
			ਮੁੱਲ_i = 0;
			ਜਦੋਂ (*p == ' ') p++;
			ਜੇ (!*p) ਰੋਕੋ;
			ਜਦੋਂ (*p && *p != '=' && *p != ' ' && var_i < BUF - 1) 
				var[var_i++] = *p++ ;
			var[var_i] = 0;
			ਜੇ (*p++ != '=') ਜਾਰੀ; /* skip if not name=value */
			ਜਦੋਂ (*p && *p != ' ' && ਮੁੱਲ_i < BUF - 1) 
				ਮੁੱਲ[ਮੁੱਲ_i++] = *p++ ;
			ਮੁੱਲ[ਮੁੱਲ_i] = 0;
			
			mb_ਸੈੱਟ_param(cbi->param_buf, var, ਮੁੱਲ, cbi);
		}
	}

        /* let higher levels know what we are booting on */
        mb_ਸੈੱਟ_param(cbi->param_buf, ARCHVARNAME, (ਅੱਖਰ *)get_board_arch_name(BOARD_ID_INTEL), cbi);
	mb_ਸੈੱਟ_param(cbi->param_buf, BOARDVARNAME,(ਅੱਖਰ *)get_board_name(BOARD_ID_INTEL) , cbi);

	/* move user stack/data down to leave a gap to catch kernel
	 * stack overflow; and to distinguish kernel and user addresses
	 * at a glance (0xf.. vs 0xe..) 
	 */
	cbi->user_sp = USR_STACKTOP;
	cbi->user_ਸਿਰਾ = USR_DATATOP;

	/* kernel bytes without bootstrap code/data that is currently
	 * still needed but will be freed after bootstrapping.
	 */
	kinfo.kernel_allocated_ਬਾਈਟਾਂ = (phys_ਬਾਈਟਾਂ) &kern_ਮਾਪ;
	kinfo.kernel_allocated_ਬਾਈਟਾਂ -= cbi->bootstrap_len;

	ਪ੍ਰਤਿਜ੍ਞਾ(!(cbi->bootstrap_ਸ਼ੁਰੂ % I386_ਪੰਨਾ_ਮਾਪ));
	cbi->bootstrap_len = rounddown(cbi->bootstrap_len, I386_ਪੰਨਾ_ਮਾਪ);
	ਪ੍ਰਤਿਜ੍ਞਾ(mbi->mi_flags & MULTIBOOT_INFO_HAS_MODS);
	ਪ੍ਰਤਿਜ੍ਞਾ(mbi->mi_mods_ਗਿਣਤੀ < MULTIBOOT_MAX_MODS);
	ਪ੍ਰਤਿਜ੍ਞਾ(mbi->mi_mods_ਗਿਣਤੀ > 0);
	ਸਮ੍ਰਿਤੀ_ਨਕਲ(&cbi->module_list, (ਖਾਲੀ *) mbi->mi_mods_addr,
		mbi->mi_mods_ਗਿਣਤੀ * ਆਕਾਰ(multiboot_module_t));
	
	memset(cbi->memmap, 0, ਆਕਾਰ(cbi->memmap));
	/* mem_map has a variable layout */
	ਜੇ(mbi->mi_flags & MULTIBOOT_INFO_HAS_MMAP) {
		cbi->mmap_ਮਾਪ = 0;
	        ਲਈ (mmap = (multiboot_ਸਮ੍ਰਿਤੀ_map_t *) mbi->mmap_addr;
       	     (ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ) mmap < mbi->mmap_addr + mbi->mmap_ਲੰਮਾਈ;
       	       mmap = (multiboot_ਸਮ੍ਰਿਤੀ_map_t *) 
		      	((ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ) mmap + mmap->mm_ਮਾਪ + ਆਕਾਰ(mmap->mm_ਮਾਪ))) {
			ਜੇ(mmap->mm_type != MULTIBOOT_ਸਮ੍ਰਿਤੀ_ਉਪਲਬਧ) ਜਾਰੀ;
			ਜੋੜੋ_memmap(cbi, mmap->mm_ਅਧਾਰ_addr, mmap->mm_ਲੰਮਾਈ);
		}
	} ਨਹੀਂਤਾਂ {
		ਪ੍ਰਤਿਜ੍ਞਾ(mbi->mi_flags & MULTIBOOT_INFO_HAS_ਸਮ੍ਰਿਤੀ);
		ਜੋੜੋ_memmap(cbi, 0, mbi->mi_mem_ਹੇਠਲਾ*1024);
		ਜੋੜੋ_memmap(cbi, 0x100000, mbi->mi_mem_ਉੱਤਲਾ*1024);
	}

	/* Sanity check: the kernel nor any of the modules may overlap
	 * with each other. Pretend the kernel is an extra module for a
	 * second.
	 */
	k = mbi->mi_mods_ਗਿਣਤੀ;
	ਪ੍ਰਤਿਜ੍ਞਾ(k < MULTIBOOT_MAX_MODS);
	cbi->module_list[k].mod_ਸ਼ੁਰੂ = kernbase;
	cbi->module_list[k].mod_ਸਿਰਾ = kernbase + kernsize;
	cbi->mods_with_kernel = mbi->mi_mods_ਗਿਣਤੀ+1;
	cbi->kern_mod = k;

	ਲਈ(m = 0; m < cbi->mods_with_kernel; m++) {
#ਜੇ 0
		ਲਿਖੋ("checking overlap of module %08lx-%08lx\n",
		  cbi->module_list[m].mod_ਸ਼ੁਰੂ, cbi->module_list[m].mod_ਸਿਰਾ);
#ਜੇ_ਅੰਤ
		ਜੇ(overlaps(cbi->module_list, cbi->mods_with_kernel, m))
			panic("overlapping boot modules/kernel");
		/* We cut out the bits of memory that we know are
		 * occupied by the kernel and boot modules.
		 */
		cut_memmap(cbi,
			cbi->module_list[m].mod_ਸ਼ੁਰੂ, 
			cbi->module_list[m].mod_ਸਿਰਾ);
	}
}

kinfo_t *pre_init(u32_t magic, u32_t ebx)
{
	ਪ੍ਰਤਿਜ੍ਞਾ(magic == MULTIBOOT_INFO_MAGIC);

	/* Get our own copy boot params pointed to by ebx.
	 * Here we find out whether we should do serial output.
	 */
	get_parameters(ebx, &kinfo);

	/* Make and load a pagetable that will map the kernel
	 * to where it should be; but first a 1:1 mapping so
	 * this code stays where it should be.
	 */
	pg_clear();
	pg_ਪਛਾਣ_ਤੱਤ(&kinfo);
	kinfo.freepde_ਸ਼ੁਰੂ = pg_mapkernel();
	pg_ਲੱਦੋ();
	vm_enable_paging();

	/* Done, return boot info so it can be passed to kmain(). */
	ਵਾਪਸ &kinfo;
}

ਖਾਲੀ send_diag_sig(ਖਾਲੀ) { }
ਖਾਲੀ minix_shutdown(ਪੂਰਨ how) { arch_shutdown(how); }
ਖਾਲੀ busy_delay_ms(ਪੂਰਨ x) { }
ਪੂਰਨ ਉਠਾਓ(ਪੂਰਨ sig) { panic("raise(%d)\n", sig); }
