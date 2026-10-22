#define UNPAGED 1	/* for proper kmain() prototype */

#અન્તર્ભાવ "kernel/kernel.h"
#અન્તર્ભાવ <assert.h>
#અન્તર્ભાવ <stdlib.h>
#અન્તર્ભાવ <minix/minlib.h>
#અન્તર્ભાવ <minix/const.h>
#અન્તર્ભાવ <minix/type.h>
#અન્તર્ભાવ <minix/board.h>
#અન્તર્ભાવ <minix/com.h>
#અન્તર્ભાવ <sys/types.h>
#અન્તર્ભાવ <sys/param.h>
#અન્તર્ભાવ <sys/reboot.h>
#અન્તર્ભાવ "string.h"
#અન્તર્ભાવ "arch_proto.h"
#અન્તર્ભાવ "direct_utils.h"
#અન્તર્ભાવ "bsp_serial.h"
#અન્તર્ભાવ "glo.h"
#અન્તર્ભાવ <machine/multiboot.h>

#જો USE_SYSDEBUG
#define MULTIBOOT_VERBOSE 1
#endif

/* to-be-built kinfo struct, diagnostics buffer */
kinfo_t kinfo;
સંરચના kmessages kmessages;

/* pg_utils.c uses this; in this phase, there is a 1:1 mapping. */
phys_bytes vir2phys(શૂન્ય *addr) { પાછા (phys_bytes) addr; }

સ્થિર શૂન્ય setup_mbi(multiboot_info_t *mbi, અક્ષર *bootargs);

/* String length used for mb_itoa */
#define ITOA_BUFFER_SIZE 20

/* Kernel may use memory */
પૂર્ણાંક kernel_may_alloc = 1;

/* kernel bss */
બાહ્ય u32_t _edata;
બાહ્ય u32_t _end;

/* kernel unpaged bss */
બાહ્ય અક્ષર _kern_unpaged_edata;
બાહ્ય અક્ષર _kern_unpaged_end;

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
પૂર્ણાંક find_કિંમત(અક્ષર * content,અક્ષર * ચાવી,અક્ષર *કિંમત,પૂર્ણાંક કિંમત_max_len){

	અક્ષર *iter,*keyp;
	પૂર્ણાંક ચાવી_len,content_len,match_len,કિંમત_len;

	/* return if the input is invalid */
	જો  (ચાવી == ખાલી || content == ખાલી || કિંમત == ખાલી) {
		પાછા 1;
	}

	/* find the key and content length */
	ચાવી_len = content_len =0;
	માટે(iter = ચાવી ; *iter != '\0'; iter++, ચાવી_len++);
	માટે(iter = content ; *iter != '\0'; iter++, content_len++);

	/* return if key or content length invalid */
	જો (ચાવી_len == 0 || content_len == 0) {
		પાછા 1;
	}

	/* now find the key in the contents */
	match_len =0;
	માટે (iter = content ,keyp=ચાવી; match_len < ચાવી_len && *iter != '\0' ; iter++) {
		જો (*iter == *keyp) {
			match_len++;
			keyp++;
			ચાલુ;
		} 
		/* The current key does not match the value , reset */
		match_len =0;
		keyp=ચાવી;
	}

	જો (match_len == ચાવી_len) {
		printf("key found at %d %s\n", match_len, &content[match_len]);
		કિંમત_len = 0;
		/* copy the content to the value char iter already points to the first 
		   char value */
		જ્યારે(*iter != '\0' && *iter != ' ' && કિંમત_len  + 1< કિંમત_max_len) {
			*કિંમત++ = *iter++;
			કિંમત_len++;
		}
		*કિંમત='\0';
		પાછા 0;
	}
	પાછા 1; /* not found */
}

સ્થિર પૂર્ણાંક mb_set_param(અક્ષર *bigbuf,અક્ષર *name,અક્ષર *કિંમત, kinfo_t *cbi)
{
	/* bigbuf contains a list of key=value pairs separated by \0 char.
	 * The list itself is ended by a second \0 terminator*/
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
			/* let q point to the end of the entry */
			જ્યારે (*q) q++; 
			/* now copy the remained of the buffer */
			માટે (q++; q < bufend; q++, p++)
				*p = *q;
			તોડો;
		}

		/* find the end of the buffer */
		જ્યારે (*p++);
		p++;
	}
	

	/* find the first empty spot */
	માટે (p = bigbuf; p < bufend && (*p || *(p + 1)); p++);

	/* unless we are the first entry step over the delimiter */
	જો (p > bigbuf) p++;
	
	/* Make sure there's enough space for the new parameter */
	જો (p + namelen + valuelen + 3 > bufend) {
		પાછા -1;
	}
	
	strcpy(p, name);
	p[namelen] = '=';
	strcpy(p + namelen + 1, કિંમત);
	p[namelen + valuelen + 1] = 0;
	p[namelen + valuelen + 2] = 0; /* end with a second delimiter */
	પાછા 0;
}

પૂર્ણાંક overlaps(multiboot_module_t *mod, પૂર્ણાંક n, પૂર્ણાંક cmp_mod)
{
	multiboot_module_t *cmp = &mod[cmp_mod];
	પૂર્ણાંક m;

#define INRANGE(mod, v) ((v) >= mod->mod_start && (v) <= thismod->mod_end)
#define OVERLAP(mod1, mod2) (INRANGE(mod1, mod2->mod_start) || \
	    INRANGE(mod1, mod2->mod_end))
	માટે(m = 0; m < n; m++) {
		multiboot_module_t *thismod = &mod[m];
		જો(m == cmp_mod) ચાલુ;
		જો(OVERLAP(thismod, cmp)) {
			પાછા 1;
		}
	}
	પાછા 0;
}

/* XXX: hard-coded stuff for modules */
#define MB_MODS_NR NR_BOOT_MODULES
#define MB_MODS_BASE  0x82000000
#define MB_MODS_ALIGN 0x00800000 /* 8 MB */
#define MB_MMAP_START 0x80000000
#define MB_MMAP_SIZE  0x10000000 /* 256 MB */

multiboot_module_t mb_modlist[MB_MODS_NR];
multiboot_memory_map_t mb_memmap;

શૂન્ય setup_mbi(multiboot_info_t *mbi, અક્ષર *bootargs)
{
	memset(mbi, 0, કદ(*mbi));
	mbi->flags = MULTIBOOT_INFO_MODS | MULTIBOOT_INFO_MEM_MAP |
			MULTIBOOT_INFO_CMDLINE;
	mbi->mi_mods_ગણતરી = MB_MODS_NR;
	mbi->mods_addr = (u32_t)&mb_modlist;

	પૂર્ણાંક i;
	માટે (i = 0; i < MB_MODS_NR; ++i) {
		mb_modlist[i].mod_start = MB_MODS_BASE + i * MB_MODS_ALIGN;
		mb_modlist[i].mod_end = mb_modlist[i].mod_start + MB_MODS_ALIGN
		    - ARM_PAGE_SIZE;
		mb_modlist[i].cmdline = 0;
	}

	/* morph the bootargs into multiboot */
	mbi->cmdline = (u32_t) bootargs;

	mbi->mmap_addr =(u32_t)&mb_memmap;
	mbi->mmap_length = કદ(mb_memmap);

	mb_memmap.size = કદ(multiboot_memory_map_t);
	mb_memmap.mm_base_addr = MB_MMAP_START;
	mb_memmap.mm_length  = MB_MMAP_SIZE;
	mb_memmap.type = MULTIBOOT_MEMORY_AVAILABLE;
}

શૂન્ય get_parameters(kinfo_t *cbi, અક્ષર *bootargs)
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
	setup_mbi(mbi, bootargs);

	/* Set various bits of info for the higher-level kernel. */
	cbi->mem_ઉપલું_phys = 0;
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
	જો (mbi->flags&MULTIBOOT_INFO_CMDLINE) {
		સ્થિર અક્ષર var[BUF];
		સ્થિર અક્ષર કિંમત[BUF];

		/* Override values with cmdline argument */
		memcpy(cmdline, (શૂન્ય *) mbi->cmdline, BUF);
		p = cmdline;
		જ્યારે (*p) {
			var_i = 0;
			કિંમત_i = 0;
			જ્યારે (*p == ' ') p++; /* skip spaces */
			જો (!*p) તોડો; /* is this the end? */
			જ્યારે (*p && *p != '=' && *p != ' ' && var_i < BUF - 1)
				var[var_i++] = *p++ ;
			var[var_i] = 0;
			જો (*p++ != '=') ચાલુ; /* skip if not name=value */
			જ્યારે (*p && *p != ' ' && કિંમત_i < BUF - 1) {
				કિંમત[કિંમત_i++] = *p++ ;
			}
			કિંમત[કિંમત_i] = 0;
			
			mb_set_param(cbi->param_buf, var, કિંમત, cbi);
		}
	}

	/* let higher levels know what we are booting on */
	mb_set_param(cbi->param_buf, ARCHVARNAME, (અક્ષર *)get_board_arch_name(machine.board_id), cbi);
	mb_set_param(cbi->param_buf, BOARDVARNAME,(અક્ષર *)get_board_name(machine.board_id) , cbi);
	

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

	પ્રતિજ્ઞા(!(cbi->bootstrap_start % ARM_PAGE_SIZE));
	cbi->bootstrap_len = rounddown(cbi->bootstrap_len, ARM_PAGE_SIZE);
	પ્રતિજ્ઞા(mbi->flags & MULTIBOOT_INFO_MODS);
	પ્રતિજ્ઞા(mbi->mi_mods_ગણતરી < MULTIBOOT_MAX_MODS);
	પ્રતિજ્ઞા(mbi->mi_mods_ગણતરી > 0);
	memcpy(&cbi->module_list, (શૂન્ય *) mbi->mods_addr,
		mbi->mi_mods_ગણતરી * કદ(multiboot_module_t));
	
	memset(cbi->memmap, 0, કદ(cbi->memmap));
	/* mem_map has a variable layout */
	જો(mbi->flags & MULTIBOOT_INFO_MEM_MAP) {
		cbi->mmap_size = 0;
	        માટે (mmap = (multiboot_memory_map_t *) mbi->mmap_addr;
       	     (unsigned long) mmap < mbi->mmap_addr + mbi->mmap_length;
       	       mmap = (multiboot_memory_map_t *) 
		      	((unsigned long) mmap + mmap->size + કદ(mmap->size))) {
			જો(mmap->type != MULTIBOOT_MEMORY_AVAILABLE) ચાલુ;
			add_memmap(cbi, mmap->mm_base_addr, mmap->mm_length);
		}
	} નહીંતર {
		પ્રતિજ્ઞા(mbi->flags & MULTIBOOT_INFO_MEMORY);
		add_memmap(cbi, 0, mbi->mem_lower_unused*1024);
		add_memmap(cbi, 0x100000, mbi->mem_upper_unused*1024);
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

/* 
 * During low level init many things are not supposed to work
 * serial being one of them. We therefore can't rely on the
 * serial to debug. POORMANS_FAILURE_NOTIFICATION can be used
 * before we setup our own vector table and will result in calling
 * the bootloader's debugging methods that will hopefully show some
 * information like the currnet PC at on the serial.
 */
#define POORMANS_FAILURE_NOTIFICATION  asm અસ્થિર("svc #00\n")

/* use the passed cmdline argument to determine the machine id */
શૂન્ય set_machine_id(અક્ષર *cmdline)
{

	અક્ષર boardname[20];
	memset(boardname,'\0',20);
	જો (find_કિંમત(cmdline,"board_name=",boardname,20)){
		/* we expect the bootloader to pass a board_name as argument
		 * this however did not happen and given we still are in early
		 * boot we can't use the serial. We therefore generate an interrupt
		 * and hope the bootloader will do something nice with it */
		POORMANS_FAILURE_NOTIFICATION;
	}  
	machine.board_id = get_board_id_by_short_name(boardname);

	જો (machine.board_id ==0){
		/* same thing as above there is no safe escape */
		POORMANS_FAILURE_NOTIFICATION;
	}
}

kinfo_t *pre_init(પૂર્ણાંક argc, અક્ષર **argv)
{
	અક્ષર *bootargs;
	/* This is the main "c" entry point into the kernel. It gets called
	   from head.S */
	   
	/* Clear BSS */
	memset(&_edata, 0, (u32_t)&_end - (u32_t)&_edata);
        memset(&_kern_unpaged_edata, 0, (u32_t)&_kern_unpaged_end - (u32_t)&_kern_unpaged_edata);

	/* we get called in a c like fashion where the first arg
         * is the program name (load address) and the rest are
	 * arguments. by convention the second argument is the
	 *  command line */
	જો (argc != 2) {
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
	પાછા &kinfo;
}

/* pre_init gets executed at the memory location where the kernel was loaded by the boot loader.
 * at that stage we only have a minimum set of functionality present (all symbols gets renamed to
 * ensure this). The following methods are used in that context. Once we jump to kmain they are no
 * longer used and the "real" implementations are visible
 */
શૂન્ય send_diag_sig(શૂન્ય) { }
શૂન્ય minix_shutdown(પૂર્ણાંક how) { arch_shutdown(how); }
શૂન્ય busy_delay_ms(પૂર્ણાંક x) { }
પૂર્ણાંક ઉઠાવો(પૂર્ણાંક n) { panic("raise(%d)\n", n); }
પૂર્ણાંક kern_phys_map_ptr( phys_bytes base_address, vir_bytes io_size, પૂર્ણાંક vm_flags,
સંરચના kern_phys_map * priv, vir_bytes ptr) { પાછા -1; };
સંરચના machine machine; /* pre init stage machine */
