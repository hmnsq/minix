
#define UNPAGED 1	/* for proper kmain() prototype */

#અન્તર્ભાવ <assert.h>
#અન્તર્ભાવ <stdlib.h>
#અન્તર્ભાવ <minix/minlib.h>
#અન્તર્ભાવ <minix/board.h>
#અન્તર્ભાવ <sys/reboot.h>
#અન્તર્ભાવ <machine/partition.h>
#અન્તર્ભાવ "string.h"
#અન્તર્ભાવ "direct_utils.h"
#અન્તર્ભાવ "serial.h"
#અન્તર્ભાવ "glo.h"

#જો USE_SYSDEBUG
#define MULTIBOOT_VERBOSE 1
#endif

/* to-be-built kinfo struct, diagnostics buffer */
kinfo_t kinfo;
સંરચના kmessages kmessages;

/* pg_utils.c uses this; in this phase, there is a 1:1 mapping. */
phys_bytes vir2phys(શૂન્ય *addr) { પાછા (phys_bytes) addr; } 

/* mb_utils.c uses this; we can reach it directly */
અક્ષર *video_mem = (અક્ષર *) MULTIBOOT_VIDEO_BUFFER;

/* String length used for mb_itoa */
#define ITOA_BUFFER_SIZE 20

/* Kernel may use memory */
પૂર્ણાંક kernel_may_alloc = 1;

સ્થિર પૂર્ણાંક mb_set_param(અક્ષર *bigbuf, અક્ષર *name, અક્ષર *કિંમત, kinfo_t *cbi) 
{
	અક્ષર *p = bigbuf;
	અક્ષર *bufend = bigbuf + MULTIBOOT_PARAM_BUF_SIZE;
	અક્ષર *q;
	પૂર્ણાંક namelen = strlen(name);
	પૂર્ણાંક valuelen = strlen(કિંમત);

	/* Some variables we recognize */
	જો(!strcmp(name, SERVARNAME)) { cbi->do_serial_debug = 1; }
	જો(!strcmp(name, SERBAUDVARNAME)) { cbi->serial_debug_baud = atoi(કિંમત); }

	/* Delete the item if already exists */
	જ્યારે (*p) {
		જો (strncmp(p, name, namelen) == 0 && p[namelen] == '=') {
			q = p;
			જ્યારે (*q) q++;
			માટે (q++; q < bufend; q++, p++)
				*p = *q;
			તોડો;
		}
		જ્યારે (*p++)
			;
		p++;
	}
	
	માટે (p = bigbuf; p < bufend && (*p || *(p + 1)); p++)
		;
	જો (p > bigbuf) p++;
	
	/* Make sure there's enough space for the new parameter */
	જો (p + namelen + valuelen + 3 > bufend)
		પાછા -1;
	
	strcpy(p, name);
	p[namelen] = '=';
	strcpy(p + namelen + 1, કિંમત);
	p[namelen + valuelen + 1] = 0;
	p[namelen + valuelen + 2] = 0;
	પાછા 0;
}

પૂર્ણાંક overlaps(multiboot_module_t *mod, પૂર્ણાંક n, પૂર્ણાંક cmp_mod)
{
	multiboot_module_t *cmp = &mod[cmp_mod];
	પૂર્ણાંક m;

#define INRANGE(mod, v) ((v) >= mod->mod_start && (v) < mod->mod_end)
#define OVERLAP(mod1, mod2) (INRANGE(mod1, mod2->mod_start) || \
			INRANGE(mod1, mod2->mod_end-1))
	માટે(m = 0; m < n; m++) {
		multiboot_module_t *thismod = &mod[m];
		જો(m == cmp_mod) ચાલુ;
		જો(OVERLAP(thismod, cmp))
			પાછા 1;
	}
	પાછા 0;
}

શૂન્ય get_parameters(u32_t ebx, kinfo_t *cbi) 
{
	multiboot_memory_map_t *mmap;
	multiboot_info_t *mbi = &cbi->mbi;
	પૂર્ણાંક var_i,કિંમત_i, m, k;
	અક્ષર *p;
	બાહ્ય અક્ષર _kern_phys_base, _kern_vir_base, _kern_size,
		_kern_unpaged_start, _kern_unpaged_end;
	phys_bytes kernbase = (phys_bytes) &_kern_phys_base,
		kernsize = (phys_bytes) &_kern_size;
#define BUF 1024
	સ્થિર અક્ષર cmdline[BUF];

	/* get our own copy of the multiboot info struct and module list */
	memcpy((શૂન્ય *) mbi, (શૂન્ય *) ebx, કદ(*mbi));

	/* Set various bits of info for the higher-level kernel. */
	cbi->mem_ઉપલું_phys = 0;
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
	જો (mbi->mi_flags & MULTIBOOT_INFO_HAS_CMDLINE) {
		સ્થિર અક્ષર var[BUF];
		સ્થિર અક્ષર કિંમત[BUF];

		/* Override values with cmdline argument */
		memcpy(cmdline, (શૂન્ય *) mbi->mi_cmdline, BUF);
		p = cmdline;
		જ્યારે (*p) {
			var_i = 0;
			કિંમત_i = 0;
			જ્યારે (*p == ' ') p++;
			જો (!*p) તોડો;
			જ્યારે (*p && *p != '=' && *p != ' ' && var_i < BUF - 1) 
				var[var_i++] = *p++ ;
			var[var_i] = 0;
			જો (*p++ != '=') ચાલુ; /* skip if not name=value */
			જ્યારે (*p && *p != ' ' && કિંમત_i < BUF - 1) 
				કિંમત[કિંમત_i++] = *p++ ;
			કિંમત[કિંમત_i] = 0;
			
			mb_set_param(cbi->param_buf, var, કિંમત, cbi);
		}
	}

        /* let higher levels know what we are booting on */
        mb_set_param(cbi->param_buf, ARCHVARNAME, (અક્ષર *)get_board_arch_name(BOARD_ID_INTEL), cbi);
	mb_set_param(cbi->param_buf, BOARDVARNAME,(અક્ષર *)get_board_name(BOARD_ID_INTEL) , cbi);

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

	પ્રતિજ્ઞા(!(cbi->bootstrap_start % I386_PAGE_SIZE));
	cbi->bootstrap_len = rounddown(cbi->bootstrap_len, I386_PAGE_SIZE);
	પ્રતિજ્ઞા(mbi->mi_flags & MULTIBOOT_INFO_HAS_MODS);
	પ્રતિજ્ઞા(mbi->mi_mods_ગણતરી < MULTIBOOT_MAX_MODS);
	પ્રતિજ્ઞા(mbi->mi_mods_ગણતરી > 0);
	memcpy(&cbi->module_list, (શૂન્ય *) mbi->mi_mods_addr,
		mbi->mi_mods_ગણતરી * કદ(multiboot_module_t));
	
	memset(cbi->memmap, 0, કદ(cbi->memmap));
	/* mem_map has a variable layout */
	જો(mbi->mi_flags & MULTIBOOT_INFO_HAS_MMAP) {
		cbi->mmap_size = 0;
	        માટે (mmap = (multiboot_memory_map_t *) mbi->mmap_addr;
       	     (unsigned long) mmap < mbi->mmap_addr + mbi->mmap_length;
       	       mmap = (multiboot_memory_map_t *) 
		      	((unsigned long) mmap + mmap->mm_size + કદ(mmap->mm_size))) {
			જો(mmap->mm_type != MULTIBOOT_MEMORY_AVAILABLE) ચાલુ;
			add_memmap(cbi, mmap->mm_base_addr, mmap->mm_length);
		}
	} નહીંતર {
		પ્રતિજ્ઞા(mbi->mi_flags & MULTIBOOT_INFO_HAS_MEMORY);
		add_memmap(cbi, 0, mbi->mi_mem_lower*1024);
		add_memmap(cbi, 0x100000, mbi->mi_mem_upper*1024);
	}

	/* Sanity check: the kernel nor any of the modules may overlap
	 * with each other. Pretend the kernel is an extra module for a
	 * second.
	 */
	k = mbi->mi_mods_ગણતરી;
	પ્રતિજ્ઞા(k < MULTIBOOT_MAX_MODS);
	cbi->module_list[k].mod_start = kernbase;
	cbi->module_list[k].mod_end = kernbase + kernsize;
	cbi->mods_with_kernel = mbi->mi_mods_ગણતરી+1;
	cbi->kern_mod = k;

	માટે(m = 0; m < cbi->mods_with_kernel; m++) {
#જો 0
		printf("checking overlap of module %08lx-%08lx\n",
		  cbi->module_list[m].mod_start, cbi->module_list[m].mod_end);
#endif
		જો(overlaps(cbi->module_list, cbi->mods_with_kernel, m))
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
	પ્રતિજ્ઞા(magic == MULTIBOOT_INFO_MAGIC);

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
	પાછા &kinfo;
}

શૂન્ય send_diag_sig(શૂન્ય) { }
શૂન્ય minix_shutdown(પૂર્ણાંક how) { arch_shutdown(how); }
શૂન્ય busy_delay_ms(પૂર્ણાંક x) { }
પૂર્ણાંક ઉઠાવો(પૂર્ણાંક sig) { panic("raise(%d)\n", sig); }
