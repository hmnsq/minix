
#define UNPAGED 1	/* for proper kmain() prototype */

#அந்தர்பாவ <assert.h>
#அந்தர்பாவ <stdlib.h>
#அந்தர்பாவ <minix/minlib.h>
#அந்தர்பாவ <minix/board.h>
#அந்தர்பாவ <sys/reboot.h>
#அந்தர்பாவ <machine/partition.h>
#அந்தர்பாவ "string.h"
#அந்தர்பாவ "direct_utils.h"
#அந்தர்பாவ "serial.h"
#அந்தர்பாவ "glo.h"

#எனில் USE_SYSDEBUG
#define MULTIBOOT_VERBOSE 1
#endif

/* to-be-built kinfo struct, diagnostics buffer */
kinfo_t kinfo;
கட்டமைப்பு kmessages kmessages;

/* pg_utils.c uses this; in this phase, there is a 1:1 mapping. */
phys_bytes vir2phys(வெறுமை *addr) { திரும்பு (phys_bytes) addr; } 

/* mb_utils.c uses this; we can reach it directly */
எழுத்து *video_mem = (எழுத்து *) MULTIBOOT_VIDEO_BUFFER;

/* String length used for mb_itoa */
#define ITOA_BUFFER_SIZE 20

/* Kernel may use memory */
முழு kernel_may_alloc = 1;

நிலையான முழு mb_set_param(எழுத்து *bigbuf, எழுத்து *name, எழுத்து *மதிப்பு, kinfo_t *cbi) 
{
	எழுத்து *p = bigbuf;
	எழுத்து *bufend = bigbuf + MULTIBOOT_PARAM_BUF_SIZE;
	எழுத்து *q;
	முழு namelen = strlen(name);
	முழு valuelen = strlen(மதிப்பு);

	/* Some variables we recognize */
	எனில்(!strcmp(name, SERVARNAME)) { cbi->do_serial_debug = 1; }
	எனில்(!strcmp(name, SERBAUDVARNAME)) { cbi->serial_debug_baud = atoi(மதிப்பு); }

	/* Delete the item if already exists */
	போது (*p) {
		எனில் (strncmp(p, name, namelen) == 0 && p[namelen] == '=') {
			q = p;
			போது (*q) q++;
			க்கு (q++; q < bufend; q++, p++)
				*p = *q;
			நிறுத்து;
		}
		போது (*p++)
			;
		p++;
	}
	
	க்கு (p = bigbuf; p < bufend && (*p || *(p + 1)); p++)
		;
	எனில் (p > bigbuf) p++;
	
	/* Make sure there's enough space for the new parameter */
	எனில் (p + namelen + valuelen + 3 > bufend)
		திரும்பு -1;
	
	strcpy(p, name);
	p[namelen] = '=';
	strcpy(p + namelen + 1, மதிப்பு);
	p[namelen + valuelen + 1] = 0;
	p[namelen + valuelen + 2] = 0;
	திரும்பு 0;
}

முழு overlaps(multiboot_module_t *mod, முழு n, முழு cmp_mod)
{
	multiboot_module_t *cmp = &mod[cmp_mod];
	முழு m;

#define INRANGE(mod, v) ((v) >= mod->mod_start && (v) < mod->mod_end)
#define OVERLAP(mod1, mod2) (INRANGE(mod1, mod2->mod_start) || \
			INRANGE(mod1, mod2->mod_end-1))
	க்கு(m = 0; m < n; m++) {
		multiboot_module_t *thismod = &mod[m];
		எனில்(m == cmp_mod) தொடர்;
		எனில்(OVERLAP(thismod, cmp))
			திரும்பு 1;
	}
	திரும்பு 0;
}

வெறுமை get_parameters(u32_t ebx, kinfo_t *cbi) 
{
	multiboot_memory_map_t *mmap;
	multiboot_info_t *mbi = &cbi->mbi;
	முழு var_i,மதிப்பு_i, m, k;
	எழுத்து *p;
	வெளி எழுத்து _kern_phys_base, _kern_vir_base, _kern_size,
		_kern_unpaged_start, _kern_unpaged_end;
	phys_bytes kernbase = (phys_bytes) &_kern_phys_base,
		kernsize = (phys_bytes) &_kern_size;
#define BUF 1024
	நிலையான எழுத்து cmdline[BUF];

	/* get our own copy of the multiboot info struct and module list */
	memcpy((வெறுமை *) mbi, (வெறுமை *) ebx, அளவு(*mbi));

	/* Set various bits of info for the higher-level kernel. */
	cbi->mem_உயர்வு_phys = 0;
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
	எனில் (mbi->mi_flags & MULTIBOOT_INFO_HAS_CMDLINE) {
		நிலையான எழுத்து var[BUF];
		நிலையான எழுத்து மதிப்பு[BUF];

		/* Override values with cmdline argument */
		memcpy(cmdline, (வெறுமை *) mbi->mi_cmdline, BUF);
		p = cmdline;
		போது (*p) {
			var_i = 0;
			மதிப்பு_i = 0;
			போது (*p == ' ') p++;
			எனில் (!*p) நிறுத்து;
			போது (*p && *p != '=' && *p != ' ' && var_i < BUF - 1) 
				var[var_i++] = *p++ ;
			var[var_i] = 0;
			எனில் (*p++ != '=') தொடர்; /* skip if not name=value */
			போது (*p && *p != ' ' && மதிப்பு_i < BUF - 1) 
				மதிப்பு[மதிப்பு_i++] = *p++ ;
			மதிப்பு[மதிப்பு_i] = 0;
			
			mb_set_param(cbi->param_buf, var, மதிப்பு, cbi);
		}
	}

        /* let higher levels know what we are booting on */
        mb_set_param(cbi->param_buf, ARCHVARNAME, (எழுத்து *)get_board_arch_name(BOARD_ID_INTEL), cbi);
	mb_set_param(cbi->param_buf, BOARDVARNAME,(எழுத்து *)get_board_name(BOARD_ID_INTEL) , cbi);

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

	ப்ரதிஜ்ஞா(!(cbi->bootstrap_start % I386_PAGE_SIZE));
	cbi->bootstrap_len = rounddown(cbi->bootstrap_len, I386_PAGE_SIZE);
	ப்ரதிஜ்ஞா(mbi->mi_flags & MULTIBOOT_INFO_HAS_MODS);
	ப்ரதிஜ்ஞா(mbi->mi_mods_மொத்தம் < MULTIBOOT_MAX_MODS);
	ப்ரதிஜ்ஞா(mbi->mi_mods_மொத்தம் > 0);
	memcpy(&cbi->module_list, (வெறுமை *) mbi->mi_mods_addr,
		mbi->mi_mods_மொத்தம் * அளவு(multiboot_module_t));
	
	memset(cbi->memmap, 0, அளவு(cbi->memmap));
	/* mem_map has a variable layout */
	எனில்(mbi->mi_flags & MULTIBOOT_INFO_HAS_MMAP) {
		cbi->mmap_size = 0;
	        க்கு (mmap = (multiboot_memory_map_t *) mbi->mmap_addr;
       	     (unsigned long) mmap < mbi->mmap_addr + mbi->mmap_length;
       	       mmap = (multiboot_memory_map_t *) 
		      	((unsigned long) mmap + mmap->mm_size + அளவு(mmap->mm_size))) {
			எனில்(mmap->mm_type != MULTIBOOT_MEMORY_AVAILABLE) தொடர்;
			add_memmap(cbi, mmap->mm_base_addr, mmap->mm_length);
		}
	} இல்லையெனில் {
		ப்ரதிஜ்ஞா(mbi->mi_flags & MULTIBOOT_INFO_HAS_MEMORY);
		add_memmap(cbi, 0, mbi->mi_mem_lower*1024);
		add_memmap(cbi, 0x100000, mbi->mi_mem_upper*1024);
	}

	/* Sanity check: the kernel nor any of the modules may overlap
	 * with each other. Pretend the kernel is an extra module for a
	 * second.
	 */
	k = mbi->mi_mods_மொத்தம்;
	ப்ரதிஜ்ஞா(k < MULTIBOOT_MAX_MODS);
	cbi->module_list[k].mod_start = kernbase;
	cbi->module_list[k].mod_end = kernbase + kernsize;
	cbi->mods_with_kernel = mbi->mi_mods_மொத்தம்+1;
	cbi->kern_mod = k;

	க்கு(m = 0; m < cbi->mods_with_kernel; m++) {
#எனில் 0
		printf("checking overlap of module %08lx-%08lx\n",
		  cbi->module_list[m].mod_start, cbi->module_list[m].mod_end);
#endif
		எனில்(overlaps(cbi->module_list, cbi->mods_with_kernel, m))
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
	ப்ரதிஜ்ஞா(magic == MULTIBOOT_INFO_MAGIC);

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
	திரும்பு &kinfo;
}

வெறுமை send_diag_sig(வெறுமை) { }
வெறுமை minix_shutdown(முழு how) { arch_shutdown(how); }
வெறுமை busy_delay_ms(முழு x) { }
முழு எழுப்பு(முழு sig) { panic("raise(%d)\n", sig); }
