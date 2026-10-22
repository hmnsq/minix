
#define UNPAGED 1	/* for proper kmain() prototype */

#അന്തര്ഭാവ <assert.h>
#അന്തര്ഭാവ <stdlib.h>
#അന്തര്ഭാവ <minix/minlib.h>
#അന്തര്ഭാവ <minix/board.h>
#അന്തര്ഭാവ <sys/reboot.h>
#അന്തര്ഭാവ <machine/partition.h>
#അന്തര്ഭാവ "string.h"
#അന്തര്ഭാവ "direct_utils.h"
#അന്തര്ഭാവ "serial.h"
#അന്തര്ഭാവ "glo.h"

#എങ്കിൽ USE_SYSDEBUG
#define MULTIBOOT_VERBOSE 1
#endif

/* to-be-built kinfo struct, diagnostics buffer */
kinfo_t kinfo;
ഘടന kmessages kmessages;

/* pg_utils.c uses this; in this phase, there is a 1:1 mapping. */
phys_bytes vir2phys(ശൂന്യം *addr) { തിരികെ (phys_bytes) addr; } 

/* mb_utils.c uses this; we can reach it directly */
അക്ഷരം *video_mem = (അക്ഷരം *) MULTIBOOT_VIDEO_BUFFER;

/* String length used for mb_itoa */
#define ITOA_BUFFER_SIZE 20

/* Kernel may use memory */
പൂർണ്ണസംഖ്യ kernel_may_alloc = 1;

സ്ഥിതം പൂർണ്ണസംഖ്യ mb_set_param(അക്ഷരം *bigbuf, അക്ഷരം *name, അക്ഷരം *മൂല്യം, kinfo_t *cbi) 
{
	അക്ഷരം *p = bigbuf;
	അക്ഷരം *bufend = bigbuf + MULTIBOOT_PARAM_BUF_SIZE;
	അക്ഷരം *q;
	പൂർണ്ണസംഖ്യ namelen = strlen(name);
	പൂർണ്ണസംഖ്യ valuelen = strlen(മൂല്യം);

	/* Some variables we recognize */
	എങ്കിൽ(!strcmp(name, SERVARNAME)) { cbi->do_serial_debug = 1; }
	എങ്കിൽ(!strcmp(name, SERBAUDVARNAME)) { cbi->serial_debug_baud = atoi(മൂല്യം); }

	/* Delete the item if already exists */
	ആകുമ്പോൾ (*p) {
		എങ്കിൽ (strncmp(p, name, namelen) == 0 && p[namelen] == '=') {
			q = p;
			ആകുമ്പോൾ (*q) q++;
			വേണ്ടി (q++; q < bufend; q++, p++)
				*p = *q;
			നിർത്തുക;
		}
		ആകുമ്പോൾ (*p++)
			;
		p++;
	}
	
	വേണ്ടി (p = bigbuf; p < bufend && (*p || *(p + 1)); p++)
		;
	എങ്കിൽ (p > bigbuf) p++;
	
	/* Make sure there's enough space for the new parameter */
	എങ്കിൽ (p + namelen + valuelen + 3 > bufend)
		തിരികെ -1;
	
	strcpy(p, name);
	p[namelen] = '=';
	strcpy(p + namelen + 1, മൂല്യം);
	p[namelen + valuelen + 1] = 0;
	p[namelen + valuelen + 2] = 0;
	തിരികെ 0;
}

പൂർണ്ണസംഖ്യ overlaps(multiboot_module_t *mod, പൂർണ്ണസംഖ്യ n, പൂർണ്ണസംഖ്യ cmp_mod)
{
	multiboot_module_t *cmp = &mod[cmp_mod];
	പൂർണ്ണസംഖ്യ m;

#define INRANGE(mod, v) ((v) >= mod->mod_start && (v) < mod->mod_end)
#define OVERLAP(mod1, mod2) (INRANGE(mod1, mod2->mod_start) || \
			INRANGE(mod1, mod2->mod_end-1))
	വേണ്ടി(m = 0; m < n; m++) {
		multiboot_module_t *thismod = &mod[m];
		എങ്കിൽ(m == cmp_mod) തുടരുക;
		എങ്കിൽ(OVERLAP(thismod, cmp))
			തിരികെ 1;
	}
	തിരികെ 0;
}

ശൂന്യം get_parameters(u32_t ebx, kinfo_t *cbi) 
{
	multiboot_memory_map_t *mmap;
	multiboot_info_t *mbi = &cbi->mbi;
	പൂർണ്ണസംഖ്യ var_i,മൂല്യം_i, m, k;
	അക്ഷരം *p;
	ബാഹ്യ അക്ഷരം _kern_phys_base, _kern_vir_base, _kern_size,
		_kern_unpaged_start, _kern_unpaged_end;
	phys_bytes kernbase = (phys_bytes) &_kern_phys_base,
		kernsize = (phys_bytes) &_kern_size;
#define BUF 1024
	സ്ഥിതം അക്ഷരം cmdline[BUF];

	/* get our own copy of the multiboot info struct and module list */
	memcpy((ശൂന്യം *) mbi, (ശൂന്യം *) ebx, വലുപ്പം(*mbi));

	/* Set various bits of info for the higher-level kernel. */
	cbi->mem_മുകളിലെ_phys = 0;
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
	എങ്കിൽ (mbi->mi_flags & MULTIBOOT_INFO_HAS_CMDLINE) {
		സ്ഥിതം അക്ഷരം var[BUF];
		സ്ഥിതം അക്ഷരം മൂല്യം[BUF];

		/* Override values with cmdline argument */
		memcpy(cmdline, (ശൂന്യം *) mbi->mi_cmdline, BUF);
		p = cmdline;
		ആകുമ്പോൾ (*p) {
			var_i = 0;
			മൂല്യം_i = 0;
			ആകുമ്പോൾ (*p == ' ') p++;
			എങ്കിൽ (!*p) നിർത്തുക;
			ആകുമ്പോൾ (*p && *p != '=' && *p != ' ' && var_i < BUF - 1) 
				var[var_i++] = *p++ ;
			var[var_i] = 0;
			എങ്കിൽ (*p++ != '=') തുടരുക; /* skip if not name=value */
			ആകുമ്പോൾ (*p && *p != ' ' && മൂല്യം_i < BUF - 1) 
				മൂല്യം[മൂല്യം_i++] = *p++ ;
			മൂല്യം[മൂല്യം_i] = 0;
			
			mb_set_param(cbi->param_buf, var, മൂല്യം, cbi);
		}
	}

        /* let higher levels know what we are booting on */
        mb_set_param(cbi->param_buf, ARCHVARNAME, (അക്ഷരം *)get_board_arch_name(BOARD_ID_INTEL), cbi);
	mb_set_param(cbi->param_buf, BOARDVARNAME,(അക്ഷരം *)get_board_name(BOARD_ID_INTEL) , cbi);

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

	പ്രതിജ്ഞാ(!(cbi->bootstrap_start % I386_PAGE_SIZE));
	cbi->bootstrap_len = rounddown(cbi->bootstrap_len, I386_PAGE_SIZE);
	പ്രതിജ്ഞാ(mbi->mi_flags & MULTIBOOT_INFO_HAS_MODS);
	പ്രതിജ്ഞാ(mbi->mi_mods_എണ്ണം < MULTIBOOT_MAX_MODS);
	പ്രതിജ്ഞാ(mbi->mi_mods_എണ്ണം > 0);
	memcpy(&cbi->module_list, (ശൂന്യം *) mbi->mi_mods_addr,
		mbi->mi_mods_എണ്ണം * വലുപ്പം(multiboot_module_t));
	
	memset(cbi->memmap, 0, വലുപ്പം(cbi->memmap));
	/* mem_map has a variable layout */
	എങ്കിൽ(mbi->mi_flags & MULTIBOOT_INFO_HAS_MMAP) {
		cbi->mmap_size = 0;
	        വേണ്ടി (mmap = (multiboot_memory_map_t *) mbi->mmap_addr;
       	     (unsigned long) mmap < mbi->mmap_addr + mbi->mmap_length;
       	       mmap = (multiboot_memory_map_t *) 
		      	((unsigned long) mmap + mmap->mm_size + വലുപ്പം(mmap->mm_size))) {
			എങ്കിൽ(mmap->mm_type != MULTIBOOT_MEMORY_AVAILABLE) തുടരുക;
			add_memmap(cbi, mmap->mm_base_addr, mmap->mm_length);
		}
	} അല്ലെങ്കിൽ {
		പ്രതിജ്ഞാ(mbi->mi_flags & MULTIBOOT_INFO_HAS_MEMORY);
		add_memmap(cbi, 0, mbi->mi_mem_lower*1024);
		add_memmap(cbi, 0x100000, mbi->mi_mem_upper*1024);
	}

	/* Sanity check: the kernel nor any of the modules may overlap
	 * with each other. Pretend the kernel is an extra module for a
	 * second.
	 */
	k = mbi->mi_mods_എണ്ണം;
	പ്രതിജ്ഞാ(k < MULTIBOOT_MAX_MODS);
	cbi->module_list[k].mod_start = kernbase;
	cbi->module_list[k].mod_end = kernbase + kernsize;
	cbi->mods_with_kernel = mbi->mi_mods_എണ്ണം+1;
	cbi->kern_mod = k;

	വേണ്ടി(m = 0; m < cbi->mods_with_kernel; m++) {
#എങ്കിൽ 0
		printf("checking overlap of module %08lx-%08lx\n",
		  cbi->module_list[m].mod_start, cbi->module_list[m].mod_end);
#endif
		എങ്കിൽ(overlaps(cbi->module_list, cbi->mods_with_kernel, m))
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
	പ്രതിജ്ഞാ(magic == MULTIBOOT_INFO_MAGIC);

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
	തിരികെ &kinfo;
}

ശൂന്യം send_diag_sig(ശൂന്യം) { }
ശൂന്യം minix_shutdown(പൂർണ്ണസംഖ്യ how) { arch_shutdown(how); }
ശൂന്യം busy_delay_ms(പൂർണ്ണസംഖ്യ x) { }
പൂർണ്ണസംഖ്യ ഉയർത്തുക(പൂർണ്ണസംഖ്യ sig) { panic("raise(%d)\n", sig); }
