
#define UNPAGED 1	/* for proper kmain() prototype */

#অন্তর্ভাব <assert.h>
#অন্তর্ভাব <stdlib.h>
#অন্তর্ভাব <minix/minlib.h>
#অন্তর্ভাব <minix/board.h>
#অন্তর্ভাব <sys/reboot.h>
#অন্তর্ভাব <machine/partition.h>
#অন্তর্ভাব "string.h"
#অন্তর্ভাব "direct_utils.h"
#অন্তর্ভাব "serial.h"
#অন্তর্ভাব "glo.h"

#যদি USE_SYSDEBUG
#define MULTIBOOT_VERBOSE 1
#endif

/* to-be-built kinfo struct, diagnostics buffer */
kinfo_t kinfo;
কাঠামো kmessages kmessages;

/* pg_utils.c uses this; in this phase, there is a 1:1 mapping. */
phys_bytes vir2phys(শূন্য *addr) { ফেরত (phys_bytes) addr; } 

/* mb_utils.c uses this; we can reach it directly */
অক্ষর *video_mem = (অক্ষর *) MULTIBOOT_VIDEO_বাফার;

/* String length used for mb_itoa */
#define ITOA_বাফার_আকার 20

/* Kernel may use memory */
পূর্ণসংখ্যা kernel_may_alloc = 1;

স্থির পূর্ণসংখ্যা mb_set_param(অক্ষর *bigbuf, অক্ষর *name, অক্ষর *মান, kinfo_t *cbi) 
{
	অক্ষর *p = bigbuf;
	অক্ষর *bufend = bigbuf + MULTIBOOT_PARAM_BUF_আকার;
	অক্ষর *q;
	পূর্ণসংখ্যা namelen = strlen(name);
	পূর্ণসংখ্যা valuelen = strlen(মান);

	/* Some variables we recognize */
	যদি(!strcmp(name, SERVARNAME)) { cbi->do_serial_debug = 1; }
	যদি(!strcmp(name, SERBAUDVARNAME)) { cbi->serial_debug_baud = atoi(মান); }

	/* Delete the item if already exists */
	যখন (*p) {
		যদি (strncmp(p, name, namelen) == 0 && p[namelen] == '=') {
			q = p;
			যখন (*q) q++;
			জন্য (q++; q < bufend; q++, p++)
				*p = *q;
			বিরতি;
		}
		যখন (*p++)
			;
		p++;
	}
	
	জন্য (p = bigbuf; p < bufend && (*p || *(p + 1)); p++)
		;
	যদি (p > bigbuf) p++;
	
	/* Make sure there's enough space for the new parameter */
	যদি (p + namelen + valuelen + 3 > bufend)
		ফেরত -1;
	
	strcpy(p, name);
	p[namelen] = '=';
	strcpy(p + namelen + 1, মান);
	p[namelen + valuelen + 1] = 0;
	p[namelen + valuelen + 2] = 0;
	ফেরত 0;
}

পূর্ণসংখ্যা overlaps(multiboot_module_t *mod, পূর্ণসংখ্যা n, পূর্ণসংখ্যা cmp_mod)
{
	multiboot_module_t *cmp = &mod[cmp_mod];
	পূর্ণসংখ্যা m;

#define INRANGE(mod, v) ((v) >= mod->mod_start && (v) < mod->mod_end)
#define OVERLAP(mod1, mod2) (INRANGE(mod1, mod2->mod_start) || \
			INRANGE(mod1, mod2->mod_end-1))
	জন্য(m = 0; m < n; m++) {
		multiboot_module_t *thismod = &mod[m];
		যদি(m == cmp_mod) চালু;
		যদি(OVERLAP(thismod, cmp))
			ফেরত 1;
	}
	ফেরত 0;
}

শূন্য get_parameters(u32_t ebx, kinfo_t *cbi) 
{
	multiboot_memory_map_t *mmap;
	multiboot_info_t *mbi = &cbi->mbi;
	পূর্ণসংখ্যা var_i,মান_i, m, k;
	অক্ষর *p;
	বাহ্যিক অক্ষর _kern_phys_base, _kern_vir_base, kern_আকার,
		_kern_unpaged_start, _kern_unpaged_end;
	phys_bytes kernbase = (phys_bytes) &_kern_phys_base,
		kernsize = (phys_bytes) &kern_আকার;
#define BUF 1024
	স্থির অক্ষর cmdline[BUF];

	/* get our own copy of the multiboot info struct and module list */
	memcpy((শূন্য *) mbi, (শূন্য *) ebx, আকার(*mbi));

	/* Set various bits of info for the higher-level kernel. */
	cbi->mem_উপরের_phys = 0;
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
		স্থির অক্ষর var[BUF];
		স্থির অক্ষর মান[BUF];

		/* Override values with cmdline argument */
		memcpy(cmdline, (শূন্য *) mbi->mi_cmdline, BUF);
		p = cmdline;
		যখন (*p) {
			var_i = 0;
			মান_i = 0;
			যখন (*p == ' ') p++;
			যদি (!*p) বিরতি;
			যখন (*p && *p != '=' && *p != ' ' && var_i < BUF - 1) 
				var[var_i++] = *p++ ;
			var[var_i] = 0;
			যদি (*p++ != '=') চালু; /* skip if not name=value */
			যখন (*p && *p != ' ' && মান_i < BUF - 1) 
				মান[মান_i++] = *p++ ;
			মান[মান_i] = 0;
			
			mb_set_param(cbi->param_buf, var, মান, cbi);
		}
	}

        /* let higher levels know what we are booting on */
        mb_set_param(cbi->param_buf, ARCHVARNAME, (অক্ষর *)get_board_arch_name(BOARD_ID_INTEL), cbi);
	mb_set_param(cbi->param_buf, BOARDVARNAME,(অক্ষর *)get_board_name(BOARD_ID_INTEL) , cbi);

	/* move user stack/data down to leave a gap to catch kernel
	 * stack overflow; and to distinguish kernel and user addresses
	 * at a glance (0xf.. vs 0xe..) 
	 */
	cbi->user_sp = USR_STACKTOP;
	cbi->user_end = USR_DATATOP;

	/* kernel bytes without bootstrap code/data that is currently
	 * still needed but will be freed after bootstrapping.
	 */
	kinfo.kernel_allocated_bytes = (phys_bytes) &kern_আকার;
	kinfo.kernel_allocated_bytes -= cbi->bootstrap_len;

	প্রতিজ্ঞা(!(cbi->bootstrap_start % I386_PAGE_আকার));
	cbi->bootstrap_len = rounddown(cbi->bootstrap_len, I386_PAGE_আকার);
	প্রতিজ্ঞা(mbi->mi_flags & MULTIBOOT_INFO_HAS_MODS);
	প্রতিজ্ঞা(mbi->mi_mods_গুনতি < MULTIBOOT_MAX_MODS);
	প্রতিজ্ঞা(mbi->mi_mods_গুনতি > 0);
	memcpy(&cbi->module_list, (শূন্য *) mbi->mi_mods_addr,
		mbi->mi_mods_গুনতি * আকার(multiboot_module_t));
	
	memset(cbi->memmap, 0, আকার(cbi->memmap));
	/* mem_map has a variable layout */
	যদি(mbi->mi_flags & MULTIBOOT_INFO_HAS_MMAP) {
		cbi->mmap_আকার = 0;
	        জন্য (mmap = (multiboot_memory_map_t *) mbi->mmap_addr;
       	     (unsigned long) mmap < mbi->mmap_addr + mbi->mmap_দৈর্ঘ্য;
       	       mmap = (multiboot_memory_map_t *) 
		      	((unsigned long) mmap + mmap->mm_আকার + আকার(mmap->mm_আকার))) {
			যদি(mmap->mm_type != MULTIBOOT_MEMORY_AVAILABLE) চালু;
			add_memmap(cbi, mmap->mm_base_addr, mmap->mm_দৈর্ঘ্য);
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

	জন্য(m = 0; m < cbi->mods_with_kernel; m++) {
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
	ফেরত &kinfo;
}

শূন্য send_diag_sig(শূন্য) { }
শূন্য minix_shutdown(পূর্ণসংখ্যা how) { arch_shutdown(how); }
শূন্য busy_delay_ms(পূর্ণসংখ্যা x) { }
পূর্ণসংখ্যা তোলো(পূর্ণসংখ্যা sig) { panic("raise(%d)\n", sig); }
