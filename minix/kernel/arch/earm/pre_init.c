#define UNPAGED 1	/* for proper kmain() prototype */

#अन्तर्भाव "kernel/kernel.h"
#अन्तर्भाव <assert.h>
#अन्तर्भाव <stdlib.h>
#अन्तर्भाव <minix/minlib.h>
#अन्तर्भाव <minix/const.h>
#अन्तर्भाव <minix/type.h>
#अन्तर्भाव <minix/board.h>
#अन्तर्भाव <minix/com.h>
#अन्तर्भाव <sys/types.h>
#अन्तर्भाव <sys/param.h>
#अन्तर्भाव <sys/reboot.h>
#अन्तर्भाव "string.h"
#अन्तर्भाव "arch_proto.h"
#अन्तर्भाव "direct_utils.h"
#अन्तर्भाव "bsp_serial.h"
#अन्तर्भाव "glo.h"
#अन्तर्भाव <machine/multiboot.h>

#यदि USE_SYSDEBUG
#define MULTIBOOT_VERBOSE 1
#endif

/* to-be-built kinfo struct, diagnostics buffer */
kinfo_t kinfo;
संरचना kmessages kmessages;

/* pg_utils.c uses this; in this phase, there is a 1:1 mapping. */
phys_bytes vir2phys(शून्य *addr) { प्रत्यागम (phys_bytes) addr; }

स्थिर शून्य setup_mbi(multiboot_info_t *mbi, वर्ण *bootargs);

/* String length used for mb_itoa */
#define ITOA_BUFFER_SIZE 20

/* Kernel may use memory */
पूर्णाङ्क kernel_may_alloc = 1;

/* kernel bss */
बाह्य u32_t _edata;
बाह्य u32_t _end;

/* kernel unpaged bss */
बाह्य वर्ण _kern_unpaged_edata;
बाह्य वर्ण _kern_unpaged_end;

/**
 *
 * The following function combines a few things together
 * that can well be done using standard libc like strlen/strstr
 * and such but these are not available in pre_init stage. 
 *
 * The function expects content to be in the form of space separated
 * key value pairs.
 * param content the contents to search in
 * param key the key to find (this *should* include the key/value delimiter)
 * param value a pointer to an initialized char * of at least value_max_len length
 * param value_max_len the maximum length of the value to store in value including
 *       the end char
 *
**/
पूर्णाङ्क find_मूल्य(वर्ण * content,वर्ण * किल्ली,वर्ण *मूल्य,पूर्णाङ्क मूल्य_max_len){

	वर्ण *iter,*keyp;
	पूर्णाङ्क किल्ली_len,content_len,match_len,मूल्य_len;

	/* return if the input is invalid */
	यदि  (किल्ली == रिक्त || content == रिक्त || मूल्य == रिक्त) {
		प्रत्यागम 1;
	}

	/* find the key and content length */
	किल्ली_len = content_len =0;
	प्रति(iter = किल्ली ; *iter != '\0'; iter++, किल्ली_len++);
	प्रति(iter = content ; *iter != '\0'; iter++, content_len++);

	/* return if key or content length invalid */
	यदि (किल्ली_len == 0 || content_len == 0) {
		प्रत्यागम 1;
	}

	/* now find the key in the contents */
	match_len =0;
	प्रति (iter = content ,keyp=किल्ली; match_len < किल्ली_len && *iter != '\0' ; iter++) {
		यदि (*iter == *keyp) {
			match_len++;
			keyp++;
			अनुवर्त;
		} 
		/* The current key does not match the value , reset */
		match_len =0;
		keyp=किल्ली;
	}

	यदि (match_len == किल्ली_len) {
		printf("key found at %d %s\n", match_len, &content[match_len]);
		मूल्य_len = 0;
		/* copy the content to the value char iter already points to the first 
		   char value */
		यावत्(*iter != '\0' && *iter != ' ' && मूल्य_len  + 1< मूल्य_max_len) {
			*मूल्य++ = *iter++;
			मूल्य_len++;
		}
		*मूल्य='\0';
		प्रत्यागम 0;
	}
	प्रत्यागम 1; /* not found */
}

स्थिर पूर्णाङ्क mb_set_param(वर्ण *bigbuf,वर्ण *name,वर्ण *मूल्य, kinfo_t *cbi)
{
	/* bigbuf contains a list of key=value pairs separated by \0 char.
	 * The list itself is ended by a second \0 terminator*/
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
			/* let q point to the end of the entry */
			यावत् (*q) q++; 
			/* now copy the remained of the buffer */
			प्रति (q++; q < bufend; q++, p++)
				*p = *q;
			भङ्ग;
		}

		/* find the end of the buffer */
		यावत् (*p++);
		p++;
	}
	

	/* find the first empty spot */
	प्रति (p = bigbuf; p < bufend && (*p || *(p + 1)); p++);

	/* unless we are the first entry step over the delimiter */
	यदि (p > bigbuf) p++;
	
	/* Make sure there's enough space for the new parameter */
	यदि (p + namelen + valuelen + 3 > bufend) {
		प्रत्यागम -1;
	}
	
	strcpy(p, name);
	p[namelen] = '=';
	strcpy(p + namelen + 1, मूल्य);
	p[namelen + valuelen + 1] = 0;
	p[namelen + valuelen + 2] = 0; /* end with a second delimiter */
	प्रत्यागम 0;
}

पूर्णाङ्क overlaps(multiboot_module_t *mod, पूर्णाङ्क n, पूर्णाङ्क cmp_mod)
{
	multiboot_module_t *cmp = &mod[cmp_mod];
	पूर्णाङ्क m;

#define INRANGE(mod, v) ((v) >= mod->mod_start && (v) <= thismod->mod_end)
#define OVERLAP(mod1, mod2) (INRANGE(mod1, mod2->mod_start) || \
	    INRANGE(mod1, mod2->mod_end))
	प्रति(m = 0; m < n; m++) {
		multiboot_module_t *thismod = &mod[m];
		यदि(m == cmp_mod) अनुवर्त;
		यदि(OVERLAP(thismod, cmp)) {
			प्रत्यागम 1;
		}
	}
	प्रत्यागम 0;
}

/* XXX: hard-coded stuff for modules */
#define MB_MODS_NR NR_BOOT_MODULES
#define MB_MODS_BASE  0x82000000
#define MB_MODS_ALIGN 0x00800000 /* 8 MB */
#define MB_MMAP_START 0x80000000
#define MB_MMAP_SIZE  0x10000000 /* 256 MB */

multiboot_module_t mb_modlist[MB_MODS_NR];
multiboot_memory_map_t mb_memmap;

शून्य setup_mbi(multiboot_info_t *mbi, वर्ण *bootargs)
{
	memset(mbi, 0, परिमाण(*mbi));
	mbi->flags = MULTIBOOT_INFO_MODS | MULTIBOOT_INFO_MEM_MAP |
			MULTIBOOT_INFO_CMDLINE;
	mbi->mi_mods_मोजणी = MB_MODS_NR;
	mbi->mods_addr = (u32_t)&mb_modlist;

	पूर्णाङ्क i;
	प्रति (i = 0; i < MB_MODS_NR; ++i) {
		mb_modlist[i].mod_start = MB_MODS_BASE + i * MB_MODS_ALIGN;
		mb_modlist[i].mod_end = mb_modlist[i].mod_start + MB_MODS_ALIGN
		    - ARM_PAGE_SIZE;
		mb_modlist[i].cmdline = 0;
	}

	/* morph the bootargs into multiboot */
	mbi->cmdline = (u32_t) bootargs;

	mbi->mmap_addr =(u32_t)&mb_memmap;
	mbi->mmap_length = परिमाण(mb_memmap);

	mb_memmap.size = परिमाण(multiboot_memory_map_t);
	mb_memmap.mm_base_addr = MB_MMAP_START;
	mb_memmap.mm_length  = MB_MMAP_SIZE;
	mb_memmap.type = MULTIBOOT_MEMORY_AVAILABLE;
}

शून्य get_parameters(kinfo_t *cbi, वर्ण *bootargs)
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
	setup_mbi(mbi, bootargs);

	/* Set various bits of info for the higher-level kernel. */
	cbi->mem_वरचा_phys = 0;
	cbi->user_sp = (vir_bytes) &_kern_vir_base;
	cbi->vir_kern_start = (vir_bytes) &_kern_vir_base;
	cbi->bootstrap_start = (vir_bytes) &_kern_unpaged_start;
	cbi->bootstrap_len = (vir_bytes) &_kern_unpaged_end -
		cbi->bootstrap_start;
	cbi->kmess = &kmess;

	/* set some configurable defaults */
	cbi->do_serial_debug = 1;
	cbi->serial_debug_baud = 115200;

	/* parse boot command line */
	यदि (mbi->flags&MULTIBOOT_INFO_CMDLINE) {
		स्थिर वर्ण var[BUF];
		स्थिर वर्ण मूल्य[BUF];

		/* Override values with cmdline argument */
		memcpy(cmdline, (शून्य *) mbi->cmdline, BUF);
		p = cmdline;
		यावत् (*p) {
			var_i = 0;
			मूल्य_i = 0;
			यावत् (*p == ' ') p++; /* skip spaces */
			यदि (!*p) भङ्ग; /* is this the end? */
			यावत् (*p && *p != '=' && *p != ' ' && var_i < BUF - 1)
				var[var_i++] = *p++ ;
			var[var_i] = 0;
			यदि (*p++ != '=') अनुवर्त; /* skip if not name=value */
			यावत् (*p && *p != ' ' && मूल्य_i < BUF - 1) {
				मूल्य[मूल्य_i++] = *p++ ;
			}
			मूल्य[मूल्य_i] = 0;
			
			mb_set_param(cbi->param_buf, var, मूल्य, cbi);
		}
	}

	/* let higher levels know what we are booting on */
	mb_set_param(cbi->param_buf, ARCHVARNAME, (वर्ण *)get_board_arch_name(machine.board_id), cbi);
	mb_set_param(cbi->param_buf, BOARDVARNAME,(वर्ण *)get_board_name(machine.board_id) , cbi);
	

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

	प्रतिज्ञा(!(cbi->bootstrap_start % ARM_PAGE_SIZE));
	cbi->bootstrap_len = rounddown(cbi->bootstrap_len, ARM_PAGE_SIZE);
	प्रतिज्ञा(mbi->flags & MULTIBOOT_INFO_MODS);
	प्रतिज्ञा(mbi->mi_mods_मोजणी < MULTIBOOT_MAX_MODS);
	प्रतिज्ञा(mbi->mi_mods_मोजणी > 0);
	memcpy(&cbi->module_list, (शून्य *) mbi->mods_addr,
		mbi->mi_mods_मोजणी * परिमाण(multiboot_module_t));
	
	memset(cbi->memmap, 0, परिमाण(cbi->memmap));
	/* mem_map has a variable layout */
	यदि(mbi->flags & MULTIBOOT_INFO_MEM_MAP) {
		cbi->mmap_size = 0;
	        प्रति (mmap = (multiboot_memory_map_t *) mbi->mmap_addr;
       	     (unsigned long) mmap < mbi->mmap_addr + mbi->mmap_length;
       	       mmap = (multiboot_memory_map_t *) 
		      	((unsigned long) mmap + mmap->size + परिमाण(mmap->size))) {
			यदि(mmap->type != MULTIBOOT_MEMORY_AVAILABLE) अनुवर्त;
			add_memmap(cbi, mmap->mm_base_addr, mmap->mm_length);
		}
	} अन्यथा {
		प्रतिज्ञा(mbi->flags & MULTIBOOT_INFO_MEMORY);
		add_memmap(cbi, 0, mbi->mem_lower_unused*1024);
		add_memmap(cbi, 0x100000, mbi->mem_upper_unused*1024);
	}

	/* Sanity check: the kernel nor any of the modules may overlap
	 * with each other. Pretend the kernel is an extra module for a
	 * second.
	 */
	k = mbi->mi_mods_मोजणी;
	प्रतिज्ञा(k < MULTIBOOT_MAX_MODS);
	cbi->module_list[k].mod_start = kernbase;
	cbi->module_list[k].mod_end = kernbase + kernsize;
	cbi->mods_with_kernel = mbi->mi_mods_मोजणी+1;
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

/* 
 * During low level init many things are not supposed to work
 * serial being one of them. We therefore can't rely on the
 * serial to debug. POORMANS_FAILURE_NOTIFICATION can be used
 * before we setup our own vector table and will result in calling
 * the bootloader's debugging methods that will hopefully show some
 * information like the currnet PC at on the serial.
 */
#define POORMANS_FAILURE_NOTIFICATION  asm चञ्चल("svc #00\n")

/* use the passed cmdline argument to determine the machine id */
शून्य set_machine_id(वर्ण *cmdline)
{

	वर्ण boardname[20];
	memset(boardname,'\0',20);
	यदि (find_मूल्य(cmdline,"board_name=",boardname,20)){
		/* we expect the bootloader to pass a board_name as argument
		 * this however did not happen and given we still are in early
		 * boot we can't use the serial. We therefore generate an interrupt
		 * and hope the bootloader will do something nice with it */
		POORMANS_FAILURE_NOTIFICATION;
	}  
	machine.board_id = get_board_id_by_short_name(boardname);

	यदि (machine.board_id ==0){
		/* same thing as above there is no safe escape */
		POORMANS_FAILURE_NOTIFICATION;
	}
}

kinfo_t *pre_init(पूर्णाङ्क argc, वर्ण **argv)
{
	वर्ण *bootargs;
	/* This is the main "c" entry point into the kernel. It gets called
	   from head.S */
	   
	/* Clear BSS */
	memset(&_edata, 0, (u32_t)&_end - (u32_t)&_edata);
        memset(&_kern_unpaged_edata, 0, (u32_t)&_kern_unpaged_end - (u32_t)&_kern_unpaged_edata);

	/* we get called in a c like fashion where the first arg
         * is the program name (load address) and the rest are
	 * arguments. by convention the second argument is the
	 *  command line */
	यदि (argc != 2) {
		POORMANS_FAILURE_NOTIFICATION;
	}

	bootargs = argv[1];
	set_machine_id(bootargs);
	bsp_ser_init();
	/* Get our own copy boot params pointed to by ebx.
	 * Here we find out whether we should do serial output.
	 */
	get_parameters(&kinfo, bootargs);

	/* Make and load a pagetable that will map the kernel
	 * to where it should be; but first a 1:1 mapping so
	 * this code stays where it should be.
	 */
	dcache_clean(); /* clean the caches */
	pg_clear();
	pg_identity(&kinfo);
	kinfo.freepde_start = pg_mapkernel();
	pg_load();
	vm_enable_paging();

	/* Done, return boot info so it can be passed to kmain(). */
	प्रत्यागम &kinfo;
}

/* pre_init gets executed at the memory location where the kernel was loaded by the boot loader.
 * at that stage we only have a minimum set of functionality present (all symbols gets renamed to
 * ensure this). The following methods are used in that context. Once we jump to kmain they are no
 * longer used and the "real" implementations are visible
 */
शून्य send_diag_sig(शून्य) { }
शून्य minix_shutdown(पूर्णाङ्क how) { arch_shutdown(how); }
शून्य busy_delay_ms(पूर्णाङ्क x) { }
पूर्णाङ्क उत्थापय(पूर्णाङ्क n) { panic("raise(%d)\n", n); }
पूर्णाङ्क kern_phys_map_ptr( phys_bytes base_address, vir_bytes io_size, पूर्णाङ्क vm_flags,
संरचना kern_phys_map * priv, vir_bytes ptr) { प्रत्यागम -1; };
संरचना machine machine; /* pre init stage machine */
