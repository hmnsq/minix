
#define UNPAGED 1	/* for proper kmain() prototype */

#অন্তর্ভাৱ <assert.h>
#অন্তর্ভাৱ <stdlib.h>
#অন্তর্ভাৱ <minix/minlib.h>
#অন্তর্ভাৱ <minix/board.h>
#অন্তর্ভাৱ <sys/reboot.h>
#অন্তর্ভাৱ <machine/partition.h>
#অন্তর্ভাৱ "string.h"
#অন্তর্ভাৱ "direct_utils.h"
#অন্তর্ভাৱ "serial.h"
#অন্তর্ভাৱ "glo.h"

#যদি USE_SYSDEBUG
#define MULTIBOOT_VERBOSE 1
#endif

/* to-be-built kinfo struct, diagnostics buffer */
kinfo_t kinfo;
সংরচনা kmessages kmessages;

/* pg_utils.c uses this; in this phase, there is a 1:1 mapping. */
phys_bytes vir2phys(শূন্য *addr) { ঘূৰাই (phys_bytes) addr; } 

/* mb_utils.c uses this; we can reach it directly */
ৱর্ণ *video_mem = (ৱর্ণ *) MULTIBOOT_VIDEO_BUFFER;

/* String length used for mb_itoa */
#define ITOA_BUFFER_SIZE 20

/* Kernel may use memory */
পূর্ণাঙ্ক kernel_may_alloc = 1;

স্থির পূর্ণাঙ্ক mb_set_param(ৱর্ণ *bigbuf, ৱর্ণ *name, ৱর্ণ *মান, kinfo_t *cbi) 
{
	ৱর্ণ *p = bigbuf;
	ৱর্ণ *bufend = bigbuf + MULTIBOOT_PARAM_BUF_SIZE;
	ৱর্ণ *q;
	পূর্ণাঙ্ক namelen = strlen(name);
	পূর্ণাঙ্ক valuelen = strlen(মান);

	/* Some variables we recognize */
	যদি(!strcmp(name, SERVARNAME)) { cbi->do_serial_debug = 1; }
	যদি(!strcmp(name, SERBAUDVARNAME)) { cbi->serial_debug_baud = atoi(মান); }

	/* Delete the item if already exists */
	যেতিয়ালৈ (*p) {
		যদি (strncmp(p, name, namelen) == 0 && p[namelen] == '=') {
			q = p;
			যেতিয়ালৈ (*q) q++;
			বাবে (q++; q < bufend; q++, p++)
				*p = *q;
			ভঙ্গ;
		}
		যেতিয়ালৈ (*p++)
			;
		p++;
	}
	
	বাবে (p = bigbuf; p < bufend && (*p || *(p + 1)); p++)
		;
	যদি (p > bigbuf) p++;
	
	/* Make sure there's enough space for the new parameter */
	যদি (p + namelen + valuelen + 3 > bufend)
		ঘূৰাই -1;
	
	strcpy(p, name);
	p[namelen] = '=';
	strcpy(p + namelen + 1, মান);
	p[namelen + valuelen + 1] = 0;
	p[namelen + valuelen + 2] = 0;
	ঘূৰাই 0;
}

পূর্ণাঙ্ক overlaps(multiboot_module_t *mod, পূর্ণাঙ্ক n, পূর্ণাঙ্ক cmp_mod)
{
	multiboot_module_t *cmp = &mod[cmp_mod];
	পূর্ণাঙ্ক m;

#define INRANGE(mod, v) ((v) >= mod->mod_start && (v) < mod->mod_end)
#define OVERLAP(mod1, mod2) (INRANGE(mod1, mod2->mod_start) || \
			INRANGE(mod1, mod2->mod_end-1))
	বাবে(m = 0; m < n; m++) {
		multiboot_module_t *thismod = &mod[m];
		যদি(m == cmp_mod) অনুৱর্ত;
		যদি(OVERLAP(thismod, cmp))
			ঘূৰাই 1;
	}
	ঘূৰাই 0;
}

শূন্য get_parameters(u32_t ebx, kinfo_t *cbi) 
{
	multiboot_memory_map_t *mmap;
	multiboot_info_t *mbi = &cbi->mbi;
	পূর্ণাঙ্ক var_i,মান_i, m, k;
	ৱর্ণ *p;
	বাহ্য ৱর্ণ _kern_phys_base, _kern_vir_base, _kern_size,
		_kern_unpaged_start, _kern_unpaged_end;
	phys_bytes kernbase = (phys_bytes) &_kern_phys_base,
		kernsize = (phys_bytes) &_kern_size;
#define BUF 1024
	স্থির ৱর্ণ cmdline[BUF];

	/* get our own copy of the multiboot info struct and module list */
	memcpy((শূন্য *) mbi, (শূন্য *) ebx, পরিমাণ(*mbi));

	/* Set various bits of info for the higher-level kernel. */
	cbi->mem_উপৰেৰ_phys = 0;
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
	যদি (mbi->mi_flags & MULTIBOOT_INFO_HAS_CMDLINE) {
		স্থির ৱর্ণ var[BUF];
		স্থির ৱর্ণ মান[BUF];

		/* Override values with cmdline argument */
		memcpy(cmdline, (শূন্য *) mbi->mi_cmdline, BUF);
		p = cmdline;
		যেতিয়ালৈ (*p) {
			var_i = 0;
			মান_i = 0;
			যেতিয়ালৈ (*p == ' ') p++;
			যদি (!*p) ভঙ্গ;
			যেতিয়ালৈ (*p && *p != '=' && *p != ' ' && var_i < BUF - 1) 
				var[var_i++] = *p++ ;
			var[var_i] = 0;
			যদি (*p++ != '=') অনুৱর্ত; /* skip if not name=value */
			যেতিয়ালৈ (*p && *p != ' ' && মান_i < BUF - 1) 
				মান[মান_i++] = *p++ ;
			মান[মান_i] = 0;
			
			mb_set_param(cbi->param_buf, var, মান, cbi);
		}
	}

        /* let higher levels know what we are booting on */
        mb_set_param(cbi->param_buf, ARCHVARNAME, (ৱর্ণ *)get_board_arch_name(BOARD_ID_INTEL), cbi);
	mb_set_param(cbi->param_buf, BOARDVARNAME,(ৱর্ণ *)get_board_name(BOARD_ID_INTEL) , cbi);

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

	প্রতিজ্ঞা(!(cbi->bootstrap_start % I386_PAGE_SIZE));
	cbi->bootstrap_len = rounddown(cbi->bootstrap_len, I386_PAGE_SIZE);
	প্রতিজ্ঞা(mbi->mi_flags & MULTIBOOT_INFO_HAS_MODS);
	প্রতিজ্ঞা(mbi->mi_mods_গুনতি < MULTIBOOT_MAX_MODS);
	প্রতিজ্ঞা(mbi->mi_mods_গুনতি > 0);
	memcpy(&cbi->module_list, (শূন্য *) mbi->mi_mods_addr,
		mbi->mi_mods_গুনতি * পরিমাণ(multiboot_module_t));
	
	memset(cbi->memmap, 0, পরিমাণ(cbi->memmap));
	/* mem_map has a variable layout */
	যদি(mbi->mi_flags & MULTIBOOT_INFO_HAS_MMAP) {
		cbi->mmap_size = 0;
	        বাবে (mmap = (multiboot_memory_map_t *) mbi->mmap_addr;
       	     (unsigned long) mmap < mbi->mmap_addr + mbi->mmap_length;
       	       mmap = (multiboot_memory_map_t *) 
		      	((unsigned long) mmap + mmap->mm_size + পরিমাণ(mmap->mm_size))) {
			যদি(mmap->mm_type != MULTIBOOT_MEMORY_AVAILABLE) অনুৱর্ত;
			add_memmap(cbi, mmap->mm_base_addr, mmap->mm_length);
		}
	} অন্যথা {
		প্রতিজ্ঞা(mbi->mi_flags & MULTIBOOT_INFO_HAS_MEMORY);
		add_memmap(cbi, 0, mbi->mi_mem_lower*1024);
		add_memmap(cbi, 0x100000, mbi->mi_mem_upper*1024);
	}

	/* Sanity check: the kernel nor any of the modules may overlap
	 * with each other. Pretend the kernel is an extra module for a
	 * second.
	 */
	k = mbi->mi_mods_গুনতি;
	প্রতিজ্ঞা(k < MULTIBOOT_MAX_MODS);
	cbi->module_list[k].mod_start = kernbase;
	cbi->module_list[k].mod_end = kernbase + kernsize;
	cbi->mods_with_kernel = mbi->mi_mods_গুনতি+1;
	cbi->kern_mod = k;

	বাবে(m = 0; m < cbi->mods_with_kernel; m++) {
#যদি 0
		printf("checking overlap of module %08lx-%08lx\n",
		  cbi->module_list[m].mod_start, cbi->module_list[m].mod_end);
#endif
		যদি(overlaps(cbi->module_list, cbi->mods_with_kernel, m))
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
	প্রতিজ্ঞা(magic == MULTIBOOT_INFO_MAGIC);

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
	ঘূৰাই &kinfo;
}

শূন্য send_diag_sig(শূন্য) { }
শূন্য minix_shutdown(পূর্ণাঙ্ক how) { arch_shutdown(how); }
শূন্য busy_delay_ms(পূর্ণাঙ্ক x) { }
পূর্ণাঙ্ক উত্থাপয(পূর্ণাঙ্ক sig) { panic("raise(%d)\n", sig); }
