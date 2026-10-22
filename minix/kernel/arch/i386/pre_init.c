
#define UNPAGED 1	/* for proper kmain() prototype */

#ଅନ୍ତର୍ଭାଵ <assert.h>
#ଅନ୍ତର୍ଭାଵ <stdlib.h>
#ଅନ୍ତର୍ଭାଵ <minix/minlib.h>
#ଅନ୍ତର୍ଭାଵ <minix/board.h>
#ଅନ୍ତର୍ଭାଵ <sys/reboot.h>
#ଅନ୍ତର୍ଭାଵ <machine/partition.h>
#ଅନ୍ତର୍ଭାଵ "string.h"
#ଅନ୍ତର୍ଭାଵ "direct_utils.h"
#ଅନ୍ତର୍ଭାଵ "serial.h"
#ଅନ୍ତର୍ଭାଵ "glo.h"

#ଯଦି USE_SYSDEBUG
#define MULTIBOOT_VERBOSE 1
#endif

/* to-be-built kinfo struct, diagnostics buffer */
kinfo_t kinfo;
ସଂରଚନା kmessages kmessages;

/* pg_utils.c uses this; in this phase, there is a 1:1 mapping. */
phys_bytes vir2phys(ଶୂନ୍ୟ *addr) { ଫେରନ୍ତୁ (phys_bytes) addr; } 

/* mb_utils.c uses this; we can reach it directly */
ଅକ୍ଷର *video_mem = (ଅକ୍ଷର *) MULTIBOOT_VIDEO_BUFFER;

/* String length used for mb_itoa */
#define ITOA_BUFFER_SIZE 20

/* Kernel may use memory */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା kernel_may_alloc = 1;

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା mb_set_param(ଅକ୍ଷର *bigbuf, ଅକ୍ଷର *name, ଅକ୍ଷର *ମୂଲ୍ୟ, kinfo_t *cbi) 
{
	ଅକ୍ଷର *p = bigbuf;
	ଅକ୍ଷର *bufend = bigbuf + MULTIBOOT_PARAM_BUF_SIZE;
	ଅକ୍ଷର *q;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା namelen = strlen(name);
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା valuelen = strlen(ମୂଲ୍ୟ);

	/* Some variables we recognize */
	ଯଦି(!strcmp(name, SERVARNAME)) { cbi->do_serial_debug = 1; }
	ଯଦି(!strcmp(name, SERBAUDVARNAME)) { cbi->serial_debug_baud = atoi(ମୂଲ୍ୟ); }

	/* Delete the item if already exists */
	ଯେତେବେଳେ (*p) {
		ଯଦି (strncmp(p, name, namelen) == 0 && p[namelen] == '=') {
			q = p;
			ଯେତେବେଳେ (*q) q++;
			ପାଇଁ (q++; q < bufend; q++, p++)
				*p = *q;
			ଭାଙ୍ଗ;
		}
		ଯେତେବେଳେ (*p++)
			;
		p++;
	}
	
	ପାଇଁ (p = bigbuf; p < bufend && (*p || *(p + 1)); p++)
		;
	ଯଦି (p > bigbuf) p++;
	
	/* Make sure there's enough space for the new parameter */
	ଯଦି (p + namelen + valuelen + 3 > bufend)
		ଫେରନ୍ତୁ -1;
	
	strcpy(p, name);
	p[namelen] = '=';
	strcpy(p + namelen + 1, ମୂଲ୍ୟ);
	p[namelen + valuelen + 1] = 0;
	p[namelen + valuelen + 2] = 0;
	ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା overlaps(multiboot_module_t *mod, ପୂର୍ଣ୍ଣସଂଖ୍ୟା n, ପୂର୍ଣ୍ଣସଂଖ୍ୟା cmp_mod)
{
	multiboot_module_t *cmp = &mod[cmp_mod];
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା m;

#define INRANGE(mod, v) ((v) >= mod->mod_start && (v) < mod->mod_end)
#define OVERLAP(mod1, mod2) (INRANGE(mod1, mod2->mod_start) || \
			INRANGE(mod1, mod2->mod_end-1))
	ପାଇଁ(m = 0; m < n; m++) {
		multiboot_module_t *thismod = &mod[m];
		ଯଦି(m == cmp_mod) ଜାରି;
		ଯଦି(OVERLAP(thismod, cmp))
			ଫେରନ୍ତୁ 1;
	}
	ଫେରନ୍ତୁ 0;
}

ଶୂନ୍ୟ get_parameters(u32_t ebx, kinfo_t *cbi) 
{
	multiboot_memory_map_t *mmap;
	multiboot_info_t *mbi = &cbi->mbi;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା var_i,ମୂଲ୍ୟ_i, m, k;
	ଅକ୍ଷର *p;
	ବାହ୍ୟ ଅକ୍ଷର _kern_phys_base, _kern_vir_base, _kern_size,
		_kern_unpaged_start, _kern_unpaged_end;
	phys_bytes kernbase = (phys_bytes) &_kern_phys_base,
		kernsize = (phys_bytes) &_kern_size;
#define BUF 1024
	ସ୍ଥିର ଅକ୍ଷର cmdline[BUF];

	/* get our own copy of the multiboot info struct and module list */
	memcpy((ଶୂନ୍ୟ *) mbi, (ଶୂନ୍ୟ *) ebx, ଆକାର(*mbi));

	/* Set various bits of info for the higher-level kernel. */
	cbi->mem_ଉପର_phys = 0;
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
	ଯଦି (mbi->mi_flags & MULTIBOOT_INFO_HAS_CMDLINE) {
		ସ୍ଥିର ଅକ୍ଷର var[BUF];
		ସ୍ଥିର ଅକ୍ଷର ମୂଲ୍ୟ[BUF];

		/* Override values with cmdline argument */
		memcpy(cmdline, (ଶୂନ୍ୟ *) mbi->mi_cmdline, BUF);
		p = cmdline;
		ଯେତେବେଳେ (*p) {
			var_i = 0;
			ମୂଲ୍ୟ_i = 0;
			ଯେତେବେଳେ (*p == ' ') p++;
			ଯଦି (!*p) ଭାଙ୍ଗ;
			ଯେତେବେଳେ (*p && *p != '=' && *p != ' ' && var_i < BUF - 1) 
				var[var_i++] = *p++ ;
			var[var_i] = 0;
			ଯଦି (*p++ != '=') ଜାରି; /* skip if not name=value */
			ଯେତେବେଳେ (*p && *p != ' ' && ମୂଲ୍ୟ_i < BUF - 1) 
				ମୂଲ୍ୟ[ମୂଲ୍ୟ_i++] = *p++ ;
			ମୂଲ୍ୟ[ମୂଲ୍ୟ_i] = 0;
			
			mb_set_param(cbi->param_buf, var, ମୂଲ୍ୟ, cbi);
		}
	}

        /* let higher levels know what we are booting on */
        mb_set_param(cbi->param_buf, ARCHVARNAME, (ଅକ୍ଷର *)get_board_arch_name(BOARD_ID_INTEL), cbi);
	mb_set_param(cbi->param_buf, BOARDVARNAME,(ଅକ୍ଷର *)get_board_name(BOARD_ID_INTEL) , cbi);

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

	ପ୍ରତିଜ୍ଞା(!(cbi->bootstrap_start % I386_PAGE_SIZE));
	cbi->bootstrap_len = rounddown(cbi->bootstrap_len, I386_PAGE_SIZE);
	ପ୍ରତିଜ୍ଞା(mbi->mi_flags & MULTIBOOT_INFO_HAS_MODS);
	ପ୍ରତିଜ୍ଞା(mbi->mi_mods_ଗଣନା < MULTIBOOT_MAX_MODS);
	ପ୍ରତିଜ୍ଞା(mbi->mi_mods_ଗଣନା > 0);
	memcpy(&cbi->module_list, (ଶୂନ୍ୟ *) mbi->mi_mods_addr,
		mbi->mi_mods_ଗଣନା * ଆକାର(multiboot_module_t));
	
	memset(cbi->memmap, 0, ଆକାର(cbi->memmap));
	/* mem_map has a variable layout */
	ଯଦି(mbi->mi_flags & MULTIBOOT_INFO_HAS_MMAP) {
		cbi->mmap_size = 0;
	        ପାଇଁ (mmap = (multiboot_memory_map_t *) mbi->mmap_addr;
       	     (unsigned long) mmap < mbi->mmap_addr + mbi->mmap_length;
       	       mmap = (multiboot_memory_map_t *) 
		      	((unsigned long) mmap + mmap->mm_size + ଆକାର(mmap->mm_size))) {
			ଯଦି(mmap->mm_type != MULTIBOOT_MEMORY_AVAILABLE) ଜାରି;
			add_memmap(cbi, mmap->mm_base_addr, mmap->mm_length);
		}
	} ଅନ୍ୟଥା {
		ପ୍ରତିଜ୍ଞା(mbi->mi_flags & MULTIBOOT_INFO_HAS_MEMORY);
		add_memmap(cbi, 0, mbi->mi_mem_lower*1024);
		add_memmap(cbi, 0x100000, mbi->mi_mem_upper*1024);
	}

	/* Sanity check: the kernel nor any of the modules may overlap
	 * with each other. Pretend the kernel is an extra module for a
	 * second.
	 */
	k = mbi->mi_mods_ଗଣନା;
	ପ୍ରତିଜ୍ଞା(k < MULTIBOOT_MAX_MODS);
	cbi->module_list[k].mod_start = kernbase;
	cbi->module_list[k].mod_end = kernbase + kernsize;
	cbi->mods_with_kernel = mbi->mi_mods_ଗଣନା+1;
	cbi->kern_mod = k;

	ପାଇଁ(m = 0; m < cbi->mods_with_kernel; m++) {
#ଯଦି 0
		printf("checking overlap of module %08lx-%08lx\n",
		  cbi->module_list[m].mod_start, cbi->module_list[m].mod_end);
#endif
		ଯଦି(overlaps(cbi->module_list, cbi->mods_with_kernel, m))
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
	ପ୍ରତିଜ୍ଞା(magic == MULTIBOOT_INFO_MAGIC);

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
	ଫେରନ୍ତୁ &kinfo;
}

ଶୂନ୍ୟ send_diag_sig(ଶୂନ୍ୟ) { }
ଶୂନ୍ୟ minix_shutdown(ପୂର୍ଣ୍ଣସଂଖ୍ୟା how) { arch_shutdown(how); }
ଶୂନ୍ୟ busy_delay_ms(ପୂର୍ଣ୍ଣସଂଖ୍ୟା x) { }
ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଉଠାଅ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା sig) { panic("raise(%d)\n", sig); }
