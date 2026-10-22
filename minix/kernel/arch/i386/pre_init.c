
#define UNPAGED 1	/* for proper kmain() prototype */

#شامل <assert.h>
#شامل <stdlib.h>
#شامل <minix/minlib.h>
#شامل <minix/board.h>
#شامل <sys/reboot.h>
#شامل <machine/partition.h>
#شامل "string.h"
#شامل "direct_utils.h"
#شامل "serial.h"
#شامل "glo.h"

#اگر USE_SYSDEBUG
#define MULTIBOOT_VERBOSE 1
#endif

/* to-be-built kinfo struct, diagnostics buffer */
kinfo_t kinfo;
ساختار kmessages kmessages;

/* pg_utils.c uses this; in this phase, there is a 1:1 mapping. */
phys_bytes vir2phys(تهی *addr) { بازگشت (phys_bytes) addr; } 

/* mb_utils.c uses this; we can reach it directly */
نویسه *video_mem = (نویسه *) MULTIBOOT_VIDEO_BUFFER;

/* String length used for mb_itoa */
#define ITOA_BUFFER_SIZE 20

/* Kernel may use memory */
صحیح kernel_may_alloc = 1;

ایستا صحیح mb_set_param(نویسه *bigbuf, نویسه *name, نویسه *مقدار, kinfo_t *cbi) 
{
	نویسه *p = bigbuf;
	نویسه *bufend = bigbuf + MULTIBOOT_PARAM_BUF_SIZE;
	نویسه *q;
	صحیح namelen = strlen(name);
	صحیح valuelen = strlen(مقدار);

	/* Some variables we recognize */
	اگر(!strcmp(name, SERVARNAME)) { cbi->do_serial_debug = 1; }
	اگر(!strcmp(name, SERBAUDVARNAME)) { cbi->serial_debug_baud = atoi(مقدار); }

	/* Delete the item if already exists */
	وقتی (*p) {
		اگر (strncmp(p, name, namelen) == 0 && p[namelen] == '=') {
			q = p;
			وقتی (*q) q++;
			برای (q++; q < bufend; q++, p++)
				*p = *q;
			بشکن;
		}
		وقتی (*p++)
			;
		p++;
	}
	
	برای (p = bigbuf; p < bufend && (*p || *(p + 1)); p++)
		;
	اگر (p > bigbuf) p++;
	
	/* Make sure there's enough space for the new parameter */
	اگر (p + namelen + valuelen + 3 > bufend)
		بازگشت -1;
	
	strcpy(p, name);
	p[namelen] = '=';
	strcpy(p + namelen + 1, مقدار);
	p[namelen + valuelen + 1] = 0;
	p[namelen + valuelen + 2] = 0;
	بازگشت 0;
}

صحیح overlaps(multiboot_module_t *mod, صحیح n, صحیح cmp_mod)
{
	multiboot_module_t *cmp = &mod[cmp_mod];
	صحیح m;

#define INRANGE(mod, v) ((v) >= mod->mod_start && (v) < mod->mod_end)
#define OVERLAP(mod1, mod2) (INRANGE(mod1, mod2->mod_start) || \
			INRANGE(mod1, mod2->mod_end-1))
	برای(m = 0; m < n; m++) {
		multiboot_module_t *thismod = &mod[m];
		اگر(m == cmp_mod) ادامه;
		اگر(OVERLAP(thismod, cmp))
			بازگشت 1;
	}
	بازگشت 0;
}

تهی get_parameters(u32_t ebx, kinfo_t *cbi) 
{
	multiboot_memory_map_t *mmap;
	multiboot_info_t *mbi = &cbi->mbi;
	صحیح var_i,مقدار_i, m, k;
	نویسه *p;
	بیرونی نویسه _kern_phys_base, _kern_vir_base, _kern_size,
		_kern_unpaged_start, _kern_unpaged_end;
	phys_bytes kernbase = (phys_bytes) &_kern_phys_base,
		kernsize = (phys_bytes) &_kern_size;
#define BUF 1024
	ایستا نویسه cmdline[BUF];

	/* get our own copy of the multiboot info struct and module list */
	memcpy((تهی *) mbi, (تهی *) ebx, اندازه(*mbi));

	/* Set various bits of info for the higher-level kernel. */
	cbi->mem_بالا_phys = 0;
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
	اگر (mbi->mi_flags & MULTIBOOT_INFO_HAS_CMDLINE) {
		ایستا نویسه var[BUF];
		ایستا نویسه مقدار[BUF];

		/* Override values with cmdline argument */
		memcpy(cmdline, (تهی *) mbi->mi_cmdline, BUF);
		p = cmdline;
		وقتی (*p) {
			var_i = 0;
			مقدار_i = 0;
			وقتی (*p == ' ') p++;
			اگر (!*p) بشکن;
			وقتی (*p && *p != '=' && *p != ' ' && var_i < BUF - 1) 
				var[var_i++] = *p++ ;
			var[var_i] = 0;
			اگر (*p++ != '=') ادامه; /* skip if not name=value */
			وقتی (*p && *p != ' ' && مقدار_i < BUF - 1) 
				مقدار[مقدار_i++] = *p++ ;
			مقدار[مقدار_i] = 0;
			
			mb_set_param(cbi->param_buf, var, مقدار, cbi);
		}
	}

        /* let higher levels know what we are booting on */
        mb_set_param(cbi->param_buf, ARCHVARNAME, (نویسه *)get_board_arch_name(BOARD_ID_INTEL), cbi);
	mb_set_param(cbi->param_buf, BOARDVARNAME,(نویسه *)get_board_name(BOARD_ID_INTEL) , cbi);

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

	أكد(!(cbi->bootstrap_start % I386_PAGE_SIZE));
	cbi->bootstrap_len = rounddown(cbi->bootstrap_len, I386_PAGE_SIZE);
	أكد(mbi->mi_flags & MULTIBOOT_INFO_HAS_MODS);
	أكد(mbi->mi_mods_شمار < MULTIBOOT_MAX_MODS);
	أكد(mbi->mi_mods_شمار > 0);
	memcpy(&cbi->module_list, (تهی *) mbi->mi_mods_addr,
		mbi->mi_mods_شمار * اندازه(multiboot_module_t));
	
	memset(cbi->memmap, 0, اندازه(cbi->memmap));
	/* mem_map has a variable layout */
	اگر(mbi->mi_flags & MULTIBOOT_INFO_HAS_MMAP) {
		cbi->mmap_size = 0;
	        برای (mmap = (multiboot_memory_map_t *) mbi->mmap_addr;
       	     (unsigned long) mmap < mbi->mmap_addr + mbi->mmap_length;
       	       mmap = (multiboot_memory_map_t *) 
		      	((unsigned long) mmap + mmap->mm_size + اندازه(mmap->mm_size))) {
			اگر(mmap->mm_type != MULTIBOOT_MEMORY_AVAILABLE) ادامه;
			add_memmap(cbi, mmap->mm_base_addr, mmap->mm_length);
		}
	} وگرنه {
		أكد(mbi->mi_flags & MULTIBOOT_INFO_HAS_MEMORY);
		add_memmap(cbi, 0, mbi->mi_mem_lower*1024);
		add_memmap(cbi, 0x100000, mbi->mi_mem_upper*1024);
	}

	/* Sanity check: the kernel nor any of the modules may overlap
	 * with each other. Pretend the kernel is an extra module for a
	 * second.
	 */
	k = mbi->mi_mods_شمار;
	أكد(k < MULTIBOOT_MAX_MODS);
	cbi->module_list[k].mod_start = kernbase;
	cbi->module_list[k].mod_end = kernbase + kernsize;
	cbi->mods_with_kernel = mbi->mi_mods_شمار+1;
	cbi->kern_mod = k;

	برای(m = 0; m < cbi->mods_with_kernel; m++) {
#اگر 0
		printf("checking overlap of module %08lx-%08lx\n",
		  cbi->module_list[m].mod_start, cbi->module_list[m].mod_end);
#endif
		اگر(overlaps(cbi->module_list, cbi->mods_with_kernel, m))
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
	أكد(magic == MULTIBOOT_INFO_MAGIC);

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
	بازگشت &kinfo;
}

تهی send_diag_sig(تهی) { }
تهی minix_shutdown(صحیح how) { arch_shutdown(how); }
تهی busy_delay_ms(صحیح x) { }
صحیح بالاببر(صحیح sig) { panic("raise(%d)\n", sig); }
