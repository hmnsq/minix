
#define UNPAGED 1	/* for proper kmain() prototype */

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

#ಯದಿ USE_SYSDEBUG
#define MULTIBOOT_VERBOSE 1
#endif

/* to-be-built kinfo struct, diagnostics buffer */
kinfo_t kinfo;
ಸಂರಚನಾ kmessages kmessages;

/* pg_utils.c uses this; in this phase, there is a 1:1 mapping. */
phys_bytes vir2phys(ಶೂನ್ಯ *addr) { ಪ್ರತ್ಯಾಗಮ (phys_bytes) addr; } 

/* mb_utils.c uses this; we can reach it directly */
ವರ್ಣ *video_mem = (ವರ್ಣ *) MULTIBOOT_VIDEO_BUFFER;

/* String length used for mb_itoa */
#define ITOA_BUFFER_SIZE 20

/* Kernel may use memory */
ಪೂರ್ಣಾಙ್ಕ kernel_may_alloc = 1;

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ mb_set_param(ವರ್ಣ *bigbuf, ವರ್ಣ *name, ವರ್ಣ *ಮೌಲ್ಯ, kinfo_t *cbi) 
{
	ವರ್ಣ *p = bigbuf;
	ವರ್ಣ *bufend = bigbuf + MULTIBOOT_PARAM_BUF_SIZE;
	ವರ್ಣ *q;
	ಪೂರ್ಣಾಙ್ಕ namelen = strlen(name);
	ಪೂರ್ಣಾಙ್ಕ valuelen = strlen(ಮೌಲ್ಯ);

	/* Some variables we recognize */
	ಯದಿ(!strcmp(name, SERVARNAME)) { cbi->do_serial_debug = 1; }
	ಯದಿ(!strcmp(name, SERBAUDVARNAME)) { cbi->serial_debug_baud = atoi(ಮೌಲ್ಯ); }

	/* Delete the item if already exists */
	ಯಾವತ್ (*p) {
		ಯದಿ (strncmp(p, name, namelen) == 0 && p[namelen] == '=') {
			q = p;
			ಯಾವತ್ (*q) q++;
			ಪ್ರತಿ (q++; q < bufend; q++, p++)
				*p = *q;
			ಭಙ್ಗ;
		}
		ಯಾವತ್ (*p++)
			;
		p++;
	}
	
	ಪ್ರತಿ (p = bigbuf; p < bufend && (*p || *(p + 1)); p++)
		;
	ಯದಿ (p > bigbuf) p++;
	
	/* Make sure there's enough space for the new parameter */
	ಯದಿ (p + namelen + valuelen + 3 > bufend)
		ಪ್ರತ್ಯಾಗಮ -1;
	
	strcpy(p, name);
	p[namelen] = '=';
	strcpy(p + namelen + 1, ಮೌಲ್ಯ);
	p[namelen + valuelen + 1] = 0;
	p[namelen + valuelen + 2] = 0;
	ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ overlaps(multiboot_module_t *mod, ಪೂರ್ಣಾಙ್ಕ n, ಪೂರ್ಣಾಙ್ಕ cmp_mod)
{
	multiboot_module_t *cmp = &mod[cmp_mod];
	ಪೂರ್ಣಾಙ್ಕ m;

#define INRANGE(mod, v) ((v) >= mod->mod_start && (v) < mod->mod_end)
#define OVERLAP(mod1, mod2) (INRANGE(mod1, mod2->mod_start) || \
			INRANGE(mod1, mod2->mod_end-1))
	ಪ್ರತಿ(m = 0; m < n; m++) {
		multiboot_module_t *thismod = &mod[m];
		ಯದಿ(m == cmp_mod) ಅನುವರ್ತ;
		ಯದಿ(OVERLAP(thismod, cmp))
			ಪ್ರತ್ಯಾಗಮ 1;
	}
	ಪ್ರತ್ಯಾಗಮ 0;
}

ಶೂನ್ಯ get_parameters(u32_t ebx, kinfo_t *cbi) 
{
	multiboot_memory_map_t *mmap;
	multiboot_info_t *mbi = &cbi->mbi;
	ಪೂರ್ಣಾಙ್ಕ var_i,ಮೌಲ್ಯ_i, m, k;
	ವರ್ಣ *p;
	ಬಾಹ್ಯ ವರ್ಣ _kern_phys_base, _kern_vir_base, _kern_size,
		_kern_unpaged_start, _kern_unpaged_end;
	phys_bytes kernbase = (phys_bytes) &_kern_phys_base,
		kernsize = (phys_bytes) &_kern_size;
#define BUF 1024
	ಸ್ಥಿರ ವರ್ಣ cmdline[BUF];

	/* get our own copy of the multiboot info struct and module list */
	memcpy((ಶೂನ್ಯ *) mbi, (ಶೂನ್ಯ *) ebx, ಪರಿಮಾಣ(*mbi));

	/* Set various bits of info for the higher-level kernel. */
	cbi->mem_ಮೇಲಿನ_phys = 0;
	cbi->user_sp = (vir_bytes) &_kern_vir_base;
	cbi->vir_kern_start = (vir_bytes) &_kern_vir_base;
	cbi->bootstrap_start = (vir_bytes) &_kern_unpaged_start;
	cbi->bootstrap_len = (vir_bytes) &_kern_unpaged_end -
		cbi->bootstrap_start;
	cbi->kmess = &kmess;

	/* set some configurable defaults */
	cbi->do_serial_debug = 0;
	cbi->serial_debug_baud = 115200;

	/* parse boot command line */
	ಯದಿ (mbi->mi_flags & MULTIBOOT_INFO_HAS_CMDLINE) {
		ಸ್ಥಿರ ವರ್ಣ var[BUF];
		ಸ್ಥಿರ ವರ್ಣ ಮೌಲ್ಯ[BUF];

		/* Override values with cmdline argument */
		memcpy(cmdline, (ಶೂನ್ಯ *) mbi->mi_cmdline, BUF);
		p = cmdline;
		ಯಾವತ್ (*p) {
			var_i = 0;
			ಮೌಲ್ಯ_i = 0;
			ಯಾವತ್ (*p == ' ') p++;
			ಯದಿ (!*p) ಭಙ್ಗ;
			ಯಾವತ್ (*p && *p != '=' && *p != ' ' && var_i < BUF - 1) 
				var[var_i++] = *p++ ;
			var[var_i] = 0;
			ಯದಿ (*p++ != '=') ಅನುವರ್ತ; /* skip if not name=value */
			ಯಾವತ್ (*p && *p != ' ' && ಮೌಲ್ಯ_i < BUF - 1) 
				ಮೌಲ್ಯ[ಮೌಲ್ಯ_i++] = *p++ ;
			ಮೌಲ್ಯ[ಮೌಲ್ಯ_i] = 0;
			
			mb_set_param(cbi->param_buf, var, ಮೌಲ್ಯ, cbi);
		}
	}

        /* let higher levels know what we are booting on */
        mb_set_param(cbi->param_buf, ARCHVARNAME, (ವರ್ಣ *)get_board_arch_name(BOARD_ID_INTEL), cbi);
	mb_set_param(cbi->param_buf, BOARDVARNAME,(ವರ್ಣ *)get_board_name(BOARD_ID_INTEL) , cbi);

	/* move user stack/data down to leave a gap to catch kernel
	 * stack overflow; and to distinguish kernel and user addresses
	 * at a glance (0xf.. vs 0xe..) 
	 */
	cbi->user_sp = USR_STACKTOP;
	cbi->user_end = USR_DATATOP;

	/* kernel bytes without bootstrap code/data that is currently
	 * still needed but will be freed after bootstrapping.
	 */
	kinfo.kernel_allocated_bytes = (phys_bytes) &_kern_size;
	kinfo.kernel_allocated_bytes -= cbi->bootstrap_len;

	ಪ್ರತಿಜ್ಞಾ(!(cbi->bootstrap_start % I386_PAGE_SIZE));
	cbi->bootstrap_len = rounddown(cbi->bootstrap_len, I386_PAGE_SIZE);
	ಪ್ರತಿಜ್ಞಾ(mbi->mi_flags & MULTIBOOT_INFO_HAS_MODS);
	ಪ್ರತಿಜ್ಞಾ(mbi->mi_mods_ಎಣಿಕೆ < MULTIBOOT_MAX_MODS);
	ಪ್ರತಿಜ್ಞಾ(mbi->mi_mods_ಎಣಿಕೆ > 0);
	memcpy(&cbi->module_list, (ಶೂನ್ಯ *) mbi->mi_mods_addr,
		mbi->mi_mods_ಎಣಿಕೆ * ಪರಿಮಾಣ(multiboot_module_t));
	
	memset(cbi->memmap, 0, ಪರಿಮಾಣ(cbi->memmap));
	/* mem_map has a variable layout */
	ಯದಿ(mbi->mi_flags & MULTIBOOT_INFO_HAS_MMAP) {
		cbi->mmap_size = 0;
	        ಪ್ರತಿ (mmap = (multiboot_memory_map_t *) mbi->mmap_addr;
       	     (unsigned long) mmap < mbi->mmap_addr + mbi->mmap_length;
       	       mmap = (multiboot_memory_map_t *) 
		      	((unsigned long) mmap + mmap->mm_size + ಪರಿಮಾಣ(mmap->mm_size))) {
			ಯದಿ(mmap->mm_type != MULTIBOOT_MEMORY_AVAILABLE) ಅನುವರ್ತ;
			add_memmap(cbi, mmap->mm_base_addr, mmap->mm_length);
		}
	} ಅನ್ಯಥಾ {
		ಪ್ರತಿಜ್ಞಾ(mbi->mi_flags & MULTIBOOT_INFO_HAS_MEMORY);
		add_memmap(cbi, 0, mbi->mi_mem_lower*1024);
		add_memmap(cbi, 0x100000, mbi->mi_mem_upper*1024);
	}

	/* Sanity check: the kernel nor any of the modules may overlap
	 * with each other. Pretend the kernel is an extra module for a
	 * second.
	 */
	k = mbi->mi_mods_ಎಣಿಕೆ;
	ಪ್ರತಿಜ್ಞಾ(k < MULTIBOOT_MAX_MODS);
	cbi->module_list[k].mod_start = kernbase;
	cbi->module_list[k].mod_end = kernbase + kernsize;
	cbi->mods_with_kernel = mbi->mi_mods_ಎಣಿಕೆ+1;
	cbi->kern_mod = k;

	ಪ್ರತಿ(m = 0; m < cbi->mods_with_kernel; m++) {
#ಯದಿ 0
		printf("checking overlap of module %08lx-%08lx\n",
		  cbi->module_list[m].mod_start, cbi->module_list[m].mod_end);
#endif
		ಯದಿ(overlaps(cbi->module_list, cbi->mods_with_kernel, m))
			panic("overlapping boot modules/kernel");
		/* We cut out the bits of memory that we know are
		 * occupied by the kernel and boot modules.
		 */
		cut_memmap(cbi,
			cbi->module_list[m].mod_start, 
			cbi->module_list[m].mod_end);
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
	pg_identity(&kinfo);
	kinfo.freepde_start = pg_mapkernel();
	pg_load();
	vm_enable_paging();

	/* Done, return boot info so it can be passed to kmain(). */
	ಪ್ರತ್ಯಾಗಮ &kinfo;
}

ಶೂನ್ಯ send_diag_sig(ಶೂನ್ಯ) { }
ಶೂನ್ಯ minix_shutdown(ಪೂರ್ಣಾಙ್ಕ how) { arch_shutdown(how); }
ಶೂನ್ಯ busy_delay_ms(ಪೂರ್ಣಾಙ್ಕ x) { }
ಪೂರ್ಣಾಙ್ಕ ಉತ್ಥಾಪಯ(ಪೂರ್ಣಾಙ್ಕ sig) { panic("raise(%d)\n", sig); }
