#define UNPAGED 1	/* for proper kmain() prototype */

#شامل "kernel/kernel.h"
#شامل <assert.h>
#شامل <stdlib.h>
#شامل <minix/minlib.h>
#شامل <minix/const.h>
#شامل <minix/type.h>
#شامل <minix/board.h>
#شامل <minix/com.h>
#شامل <sys/types.h>
#شامل <sys/param.h>
#شامل <sys/reboot.h>
#شامل "string.h"
#شامل "arch_proto.h"
#شامل "direct_utils.h"
#شامل "bsp_serial.h"
#شامل "glo.h"
#شامل <machine/multiboot.h>

#اگر USE_SYSDEBUG
#define MULTIBOOT_VERBOSE 1
#endif

/* to-be-built kinfo struct, diagnostics buffer */
kinfo_t kinfo;
ساختار kmessages kmessages;

/* pg_utils.c uses this; in this phase, there is a 1:1 mapping. */
phys_bytes vir2phys(تهی *addr) { بازگشت (phys_bytes) addr; }

ایستا تهی setup_mbi(multiboot_info_t *mbi, نویسه *bootargs);

/* String length used for mb_itoa */
#define ITOA_BUFFER_SIZE 20

/* Kernel may use memory */
صحیح kernel_may_alloc = 1;

/* kernel bss */
بیرونی u32_t _edata;
بیرونی u32_t _end;

/* kernel unpaged bss */
بیرونی نویسه _kern_unpaged_edata;
بیرونی نویسه _kern_unpaged_end;

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
صحیح find_مقدار(نویسه * content,نویسه * کلید,نویسه *مقدار,صحیح مقدار_max_len){

	نویسه *iter,*keyp;
	صحیح کلید_len,content_len,match_len,مقدار_len;

	/* return if the input is invalid */
	اگر  (کلید == پوچ || content == پوچ || مقدار == پوچ) {
		بازگشت 1;
	}

	/* find the key and content length */
	کلید_len = content_len =0;
	برای(iter = کلید ; *iter != '\0'; iter++, کلید_len++);
	برای(iter = content ; *iter != '\0'; iter++, content_len++);

	/* return if key or content length invalid */
	اگر (کلید_len == 0 || content_len == 0) {
		بازگشت 1;
	}

	/* now find the key in the contents */
	match_len =0;
	برای (iter = content ,keyp=کلید; match_len < کلید_len && *iter != '\0' ; iter++) {
		اگر (*iter == *keyp) {
			match_len++;
			keyp++;
			ادامه;
		} 
		/* The current key does not match the value , reset */
		match_len =0;
		keyp=کلید;
	}

	اگر (match_len == کلید_len) {
		printf("key found at %d %s\n", match_len, &content[match_len]);
		مقدار_len = 0;
		/* copy the content to the value char iter already points to the first 
		   char value */
		وقتی(*iter != '\0' && *iter != ' ' && مقدار_len  + 1< مقدار_max_len) {
			*مقدار++ = *iter++;
			مقدار_len++;
		}
		*مقدار='\0';
		بازگشت 0;
	}
	بازگشت 1; /* not found */
}

ایستا صحیح mb_set_param(نویسه *bigbuf,نویسه *name,نویسه *مقدار, kinfo_t *cbi)
{
	/* bigbuf contains a list of key=value pairs separated by \0 char.
	 * The list itself is ended by a second \0 terminator*/
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
			/* let q point to the end of the entry */
			وقتی (*q) q++; 
			/* now copy the remained of the buffer */
			برای (q++; q < bufend; q++, p++)
				*p = *q;
			بشکن;
		}

		/* find the end of the buffer */
		وقتی (*p++);
		p++;
	}
	

	/* find the first empty spot */
	برای (p = bigbuf; p < bufend && (*p || *(p + 1)); p++);

	/* unless we are the first entry step over the delimiter */
	اگر (p > bigbuf) p++;
	
	/* Make sure there's enough space for the new parameter */
	اگر (p + namelen + valuelen + 3 > bufend) {
		بازگشت -1;
	}
	
	strcpy(p, name);
	p[namelen] = '=';
	strcpy(p + namelen + 1, مقدار);
	p[namelen + valuelen + 1] = 0;
	p[namelen + valuelen + 2] = 0; /* end with a second delimiter */
	بازگشت 0;
}

صحیح overlaps(multiboot_module_t *mod, صحیح n, صحیح cmp_mod)
{
	multiboot_module_t *cmp = &mod[cmp_mod];
	صحیح m;

#define INRANGE(mod, v) ((v) >= mod->mod_start && (v) <= thismod->mod_end)
#define OVERLAP(mod1, mod2) (INRANGE(mod1, mod2->mod_start) || \
	    INRANGE(mod1, mod2->mod_end))
	برای(m = 0; m < n; m++) {
		multiboot_module_t *thismod = &mod[m];
		اگر(m == cmp_mod) ادامه;
		اگر(OVERLAP(thismod, cmp)) {
			بازگشت 1;
		}
	}
	بازگشت 0;
}

/* XXX: hard-coded stuff for modules */
#define MB_MODS_NR NR_BOOT_MODULES
#define MB_MODS_BASE  0x82000000
#define MB_MODS_ALIGN 0x00800000 /* 8 MB */
#define MB_MMAP_START 0x80000000
#define MB_MMAP_SIZE  0x10000000 /* 256 MB */

multiboot_module_t mb_modlist[MB_MODS_NR];
multiboot_memory_map_t mb_memmap;

تهی setup_mbi(multiboot_info_t *mbi, نویسه *bootargs)
{
	memset(mbi, 0, اندازه(*mbi));
	mbi->flags = MULTIBOOT_INFO_MODS | MULTIBOOT_INFO_MEM_MAP |
			MULTIBOOT_INFO_CMDLINE;
	mbi->mi_mods_شمار = MB_MODS_NR;
	mbi->mods_addr = (u32_t)&mb_modlist;

	صحیح i;
	برای (i = 0; i < MB_MODS_NR; ++i) {
		mb_modlist[i].mod_start = MB_MODS_BASE + i * MB_MODS_ALIGN;
		mb_modlist[i].mod_end = mb_modlist[i].mod_start + MB_MODS_ALIGN
		    - ARM_PAGE_SIZE;
		mb_modlist[i].cmdline = 0;
	}

	/* morph the bootargs into multiboot */
	mbi->cmdline = (u32_t) bootargs;

	mbi->mmap_addr =(u32_t)&mb_memmap;
	mbi->mmap_length = اندازه(mb_memmap);

	mb_memmap.size = اندازه(multiboot_memory_map_t);
	mb_memmap.mm_base_addr = MB_MMAP_START;
	mb_memmap.mm_length  = MB_MMAP_SIZE;
	mb_memmap.type = MULTIBOOT_MEMORY_AVAILABLE;
}

تهی get_parameters(kinfo_t *cbi, نویسه *bootargs)
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
	setup_mbi(mbi, bootargs);

	/* Set various bits of info for the higher-level kernel. */
	cbi->mem_بالا_phys = 0;
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
	اگر (mbi->flags&MULTIBOOT_INFO_CMDLINE) {
		ایستا نویسه var[BUF];
		ایستا نویسه مقدار[BUF];

		/* Override values with cmdline argument */
		memcpy(cmdline, (تهی *) mbi->cmdline, BUF);
		p = cmdline;
		وقتی (*p) {
			var_i = 0;
			مقدار_i = 0;
			وقتی (*p == ' ') p++; /* skip spaces */
			اگر (!*p) بشکن; /* is this the end? */
			وقتی (*p && *p != '=' && *p != ' ' && var_i < BUF - 1)
				var[var_i++] = *p++ ;
			var[var_i] = 0;
			اگر (*p++ != '=') ادامه; /* skip if not name=value */
			وقتی (*p && *p != ' ' && مقدار_i < BUF - 1) {
				مقدار[مقدار_i++] = *p++ ;
			}
			مقدار[مقدار_i] = 0;
			
			mb_set_param(cbi->param_buf, var, مقدار, cbi);
		}
	}

	/* let higher levels know what we are booting on */
	mb_set_param(cbi->param_buf, ARCHVARNAME, (نویسه *)get_board_arch_name(machine.board_id), cbi);
	mb_set_param(cbi->param_buf, BOARDVARNAME,(نویسه *)get_board_name(machine.board_id) , cbi);
	

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

	أكد(!(cbi->bootstrap_start % ARM_PAGE_SIZE));
	cbi->bootstrap_len = rounddown(cbi->bootstrap_len, ARM_PAGE_SIZE);
	أكد(mbi->flags & MULTIBOOT_INFO_MODS);
	أكد(mbi->mi_mods_شمار < MULTIBOOT_MAX_MODS);
	أكد(mbi->mi_mods_شمار > 0);
	memcpy(&cbi->module_list, (تهی *) mbi->mods_addr,
		mbi->mi_mods_شمار * اندازه(multiboot_module_t));
	
	memset(cbi->memmap, 0, اندازه(cbi->memmap));
	/* mem_map has a variable layout */
	اگر(mbi->flags & MULTIBOOT_INFO_MEM_MAP) {
		cbi->mmap_size = 0;
	        برای (mmap = (multiboot_memory_map_t *) mbi->mmap_addr;
       	     (unsigned long) mmap < mbi->mmap_addr + mbi->mmap_length;
       	       mmap = (multiboot_memory_map_t *) 
		      	((unsigned long) mmap + mmap->size + اندازه(mmap->size))) {
			اگر(mmap->type != MULTIBOOT_MEMORY_AVAILABLE) ادامه;
			add_memmap(cbi, mmap->mm_base_addr, mmap->mm_length);
		}
	} وگرنه {
		أكد(mbi->flags & MULTIBOOT_INFO_MEMORY);
		add_memmap(cbi, 0, mbi->mem_lower_unused*1024);
		add_memmap(cbi, 0x100000, mbi->mem_upper_unused*1024);
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

/* 
 * During low level init many things are not supposed to work
 * serial being one of them. We therefore can't rely on the
 * serial to debug. POORMANS_FAILURE_NOTIFICATION can be used
 * before we setup our own vector table and will result in calling
 * the bootloader's debugging methods that will hopefully show some
 * information like the currnet PC at on the serial.
 */
#define POORMANS_FAILURE_NOTIFICATION  asm فرار("svc #00\n")

/* use the passed cmdline argument to determine the machine id */
تهی set_machine_id(نویسه *cmdline)
{

	نویسه boardname[20];
	memset(boardname,'\0',20);
	اگر (find_مقدار(cmdline,"board_name=",boardname,20)){
		/* we expect the bootloader to pass a board_name as argument
		 * this however did not happen and given we still are in early
		 * boot we can't use the serial. We therefore generate an interrupt
		 * and hope the bootloader will do something nice with it */
		POORMANS_FAILURE_NOTIFICATION;
	}  
	machine.board_id = get_board_id_by_short_name(boardname);

	اگر (machine.board_id ==0){
		/* same thing as above there is no safe escape */
		POORMANS_FAILURE_NOTIFICATION;
	}
}

kinfo_t *pre_init(صحیح argc, نویسه **argv)
{
	نویسه *bootargs;
	/* This is the main "c" entry point into the kernel. It gets called
	   from head.S */
	   
	/* Clear BSS */
	memset(&_edata, 0, (u32_t)&_end - (u32_t)&_edata);
        memset(&_kern_unpaged_edata, 0, (u32_t)&_kern_unpaged_end - (u32_t)&_kern_unpaged_edata);

	/* we get called in a c like fashion where the first arg
         * is the program name (load address) and the rest are
	 * arguments. by convention the second argument is the
	 *  command line */
	اگر (argc != 2) {
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
	بازگشت &kinfo;
}

/* pre_init gets executed at the memory location where the kernel was loaded by the boot loader.
 * at that stage we only have a minimum set of functionality present (all symbols gets renamed to
 * ensure this). The following methods are used in that context. Once we jump to kmain they are no
 * longer used and the "real" implementations are visible
 */
تهی send_diag_sig(تهی) { }
تهی minix_shutdown(صحیح how) { arch_shutdown(how); }
تهی busy_delay_ms(صحیح x) { }
صحیح بالاببر(صحیح n) { panic("raise(%d)\n", n); }
صحیح kern_phys_map_ptr( phys_bytes base_address, vir_bytes io_size, صحیح vm_flags,
ساختار kern_phys_map * priv, vir_bytes ptr) { بازگشت -1; };
ساختار machine machine; /* pre init stage machine */
