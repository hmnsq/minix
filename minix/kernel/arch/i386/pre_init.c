
#define UNPAGED 1	/* for proper kmain() prototype */

#अन्तर्भाव <assert.h>
#अन्तर्भाव <stdlib.h>
#अन्तर्भाव <minix/minlib.h>
#अन्तर्भाव <minix/board.h>
#अन्तर्भाव <sys/reboot.h>
#अन्तर्भाव <machine/partition.h>
#अन्तर्भाव "string.h"
#अन्तर्भाव "direct_utils.h"
#अन्तर्भाव "serial.h"
#अन्तर्भाव "glo.h"

#यदि USE_SYSDEBUG
#define MULTIBOOT_VERBOSE 1
#endif

/* to-be-built kinfo struct, diagnostics buffer */
kinfo_t kinfo;
संरचना kmessages kmessages;

/* pg_utils.c uses this; in this phase, there is a 1:1 mapping. */
phys_bytes vir2phys(शून्य *addr) { प्रत्यागम (phys_bytes) addr; } 

/* mb_utils.c uses this; we can reach it directly */
वर्ण *video_mem = (वर्ण *) MULTIBOOT_VIDEO_BUFFER;

/* String length used for mb_itoa */
#define ITOA_BUFFER_SIZE 20

/* Kernel may use memory */
पूर्णाङ्क kernel_may_alloc = 1;

स्थिर पूर्णाङ्क mb_set_param(वर्ण *bigbuf, वर्ण *name, वर्ण *मूल्य, kinfo_t *cbi) 
{
	वर्ण *p = bigbuf;
	वर्ण *bufend = bigbuf + MULTIBOOT_PARAM_BUF_SIZE;
	वर्ण *q;
	पूर्णाङ्क namelen = strlen(name);
	पूर्णाङ्क valuelen = strlen(मूल्य);

	/* Some variables we recognize */
	यदि(!strcmp(name, SERVARNAME)) { cbi->do_serial_debug = 1; }
	यदि(!strcmp(name, SERBAUDVARNAME)) { cbi->serial_debug_baud = atoi(मूल्य); }

	/* Delete the item if already exists */
	यावत् (*p) {
		यदि (strncmp(p, name, namelen) == 0 && p[namelen] == '=') {
			q = p;
			यावत् (*q) q++;
			प्रति (q++; q < bufend; q++, p++)
				*p = *q;
			भङ्ग;
		}
		यावत् (*p++)
			;
		p++;
	}
	
	प्रति (p = bigbuf; p < bufend && (*p || *(p + 1)); p++)
		;
	यदि (p > bigbuf) p++;
	
	/* Make sure there's enough space for the new parameter */
	यदि (p + namelen + valuelen + 3 > bufend)
		प्रत्यागम -1;
	
	strcpy(p, name);
	p[namelen] = '=';
	strcpy(p + namelen + 1, मूल्य);
	p[namelen + valuelen + 1] = 0;
	p[namelen + valuelen + 2] = 0;
	प्रत्यागम 0;
}

पूर्णाङ्क overlaps(multiboot_module_t *mod, पूर्णाङ्क n, पूर्णाङ्क cmp_mod)
{
	multiboot_module_t *cmp = &mod[cmp_mod];
	पूर्णाङ्क m;

#define INRANGE(mod, v) ((v) >= mod->mod_start && (v) < mod->mod_end)
#define OVERLAP(mod1, mod2) (INRANGE(mod1, mod2->mod_start) || \
			INRANGE(mod1, mod2->mod_end-1))
	प्रति(m = 0; m < n; m++) {
		multiboot_module_t *thismod = &mod[m];
		यदि(m == cmp_mod) अनुवर्त;
		यदि(OVERLAP(thismod, cmp))
			प्रत्यागम 1;
	}
	प्रत्यागम 0;
}

शून्य get_parameters(u32_t ebx, kinfo_t *cbi) 
{
	multiboot_memory_map_t *mmap;
	multiboot_info_t *mbi = &cbi->mbi;
	पूर्णाङ्क var_i,मूल्य_i, m, k;
	वर्ण *p;
	बाह्य वर्ण _kern_phys_base, _kern_vir_base, _kern_size,
		_kern_unpaged_start, _kern_unpaged_end;
	phys_bytes kernbase = (phys_bytes) &_kern_phys_base,
		kernsize = (phys_bytes) &_kern_size;
#define BUF 1024
	स्थिर वर्ण cmdline[BUF];

	/* get our own copy of the multiboot info struct and module list */
	memcpy((शून्य *) mbi, (शून्य *) ebx, परिमाण(*mbi));

	/* Set various bits of info for the higher-level kernel. */
	cbi->mem_ऊपरी_phys = 0;
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
	यदि (mbi->mi_flags & MULTIBOOT_INFO_HAS_CMDLINE) {
		स्थिर वर्ण var[BUF];
		स्थिर वर्ण मूल्य[BUF];

		/* Override values with cmdline argument */
		memcpy(cmdline, (शून्य *) mbi->mi_cmdline, BUF);
		p = cmdline;
		यावत् (*p) {
			var_i = 0;
			मूल्य_i = 0;
			यावत् (*p == ' ') p++;
			यदि (!*p) भङ्ग;
			यावत् (*p && *p != '=' && *p != ' ' && var_i < BUF - 1) 
				var[var_i++] = *p++ ;
			var[var_i] = 0;
			यदि (*p++ != '=') अनुवर्त; /* skip if not name=value */
			यावत् (*p && *p != ' ' && मूल्य_i < BUF - 1) 
				मूल्य[मूल्य_i++] = *p++ ;
			मूल्य[मूल्य_i] = 0;
			
			mb_set_param(cbi->param_buf, var, मूल्य, cbi);
		}
	}

        /* let higher levels know what we are booting on */
        mb_set_param(cbi->param_buf, ARCHVARNAME, (वर्ण *)get_board_arch_name(BOARD_ID_INTEL), cbi);
	mb_set_param(cbi->param_buf, BOARDVARNAME,(वर्ण *)get_board_name(BOARD_ID_INTEL) , cbi);

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

	प्रतिज्ञा(!(cbi->bootstrap_start % I386_PAGE_SIZE));
	cbi->bootstrap_len = rounddown(cbi->bootstrap_len, I386_PAGE_SIZE);
	प्रतिज्ञा(mbi->mi_flags & MULTIBOOT_INFO_HAS_MODS);
	प्रतिज्ञा(mbi->mi_mods_गिनती < MULTIBOOT_MAX_MODS);
	प्रतिज्ञा(mbi->mi_mods_गिनती > 0);
	memcpy(&cbi->module_list, (शून्य *) mbi->mi_mods_addr,
		mbi->mi_mods_गिनती * परिमाण(multiboot_module_t));
	
	memset(cbi->memmap, 0, परिमाण(cbi->memmap));
	/* mem_map has a variable layout */
	यदि(mbi->mi_flags & MULTIBOOT_INFO_HAS_MMAP) {
		cbi->mmap_size = 0;
	        प्रति (mmap = (multiboot_memory_map_t *) mbi->mmap_addr;
       	     (unsigned long) mmap < mbi->mmap_addr + mbi->mmap_length;
       	       mmap = (multiboot_memory_map_t *) 
		      	((unsigned long) mmap + mmap->mm_size + परिमाण(mmap->mm_size))) {
			यदि(mmap->mm_type != MULTIBOOT_MEMORY_AVAILABLE) अनुवर्त;
			add_memmap(cbi, mmap->mm_base_addr, mmap->mm_length);
		}
	} अन्यथा {
		प्रतिज्ञा(mbi->mi_flags & MULTIBOOT_INFO_HAS_MEMORY);
		add_memmap(cbi, 0, mbi->mi_mem_lower*1024);
		add_memmap(cbi, 0x100000, mbi->mi_mem_upper*1024);
	}

	/* Sanity check: the kernel nor any of the modules may overlap
	 * with each other. Pretend the kernel is an extra module for a
	 * second.
	 */
	k = mbi->mi_mods_गिनती;
	प्रतिज्ञा(k < MULTIBOOT_MAX_MODS);
	cbi->module_list[k].mod_start = kernbase;
	cbi->module_list[k].mod_end = kernbase + kernsize;
	cbi->mods_with_kernel = mbi->mi_mods_गिनती+1;
	cbi->kern_mod = k;

	प्रति(m = 0; m < cbi->mods_with_kernel; m++) {
#यदि 0
		printf("checking overlap of module %08lx-%08lx\n",
		  cbi->module_list[m].mod_start, cbi->module_list[m].mod_end);
#endif
		यदि(overlaps(cbi->module_list, cbi->mods_with_kernel, m))
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
	प्रतिज्ञा(magic == MULTIBOOT_INFO_MAGIC);

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
	प्रत्यागम &kinfo;
}

शून्य send_diag_sig(शून्य) { }
शून्य minix_shutdown(पूर्णाङ्क how) { arch_shutdown(how); }
शून्य busy_delay_ms(पूर्णाङ्क x) { }
पूर्णाङ्क उत्थापय(पूर्णाङ्क sig) { panic("raise(%d)\n", sig); }
