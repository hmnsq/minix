#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UNPAGED 1	/* for proper kmain() prototype */

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/kernel.h"
#ਅਨ੍ਤਰ੍ਭਾਵ <assert.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdlib.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <minix/minlib.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <minix/const.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <minix/type.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <minix/board.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <minix/com.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/types.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/param.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/reboot.h>
#ਅਨ੍ਤਰ੍ਭਾਵ "string.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "arch_proto.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "direct_utils.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "bsp_serial.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "glo.h"
#ਅਨ੍ਤਰ੍ਭਾਵ <machine/multiboot.h>

#ਜੇ USE_SYSDEBUG
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MULTIBOOT_VERBOSE 1
#ਜੇ_ਅੰਤ

/* to-be-built kinfo struct, diagnostics buffer */
kinfo_t kinfo;
ਬਣਤਰ kmessages kmessages;

/* pg_utils.c uses this; in this phase, there is a 1:1 mapping. */
phys_ਬਾਈਟਾਂ vir2phys(ਖਾਲੀ *addr) { ਵਾਪਸ (phys_ਬਾਈਟਾਂ) addr; }

ਸਥਿਰ ਖਾਲੀ ਤਿਆਰੀ_mbi(multiboot_info_t *mbi, ਅੱਖਰ *bootargs);

/* String length used for mb_itoa */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ITOA_ਬਫਰ_ਮਾਪ 20

/* Kernel may use memory */
ਪੂਰਨ kernel_may_alloc = 1;

/* kernel bss */
ਬਾਹਰੀ u32_t _edata;
ਬਾਹਰੀ u32_t ਸਿਰਾ_2;

/* kernel unpaged bss */
ਬਾਹਰੀ ਅੱਖਰ _kern_unpaged_edata;
ਬਾਹਰੀ ਅੱਖਰ kern_unpaged_ਸਿਰਾ;

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
ਪੂਰਨ ਲੱਭੋ_ਮੁੱਲ(ਅੱਖਰ * content,ਅੱਖਰ * ਕੁੰਜੀ,ਅੱਖਰ *ਮੁੱਲ,ਪੂਰਨ ਮੁੱਲ_max_len){

	ਅੱਖਰ *iter,*keyp;
	ਪੂਰਨ ਕੁੰਜੀ_len,content_len,ਮੇਲ_len,ਮੁੱਲ_len;

	/* return if the input is invalid */
	ਜੇ  (ਕੁੰਜੀ == ਨਿਰਾ || content == ਨਿਰਾ || ਮੁੱਲ == ਨਿਰਾ) {
		ਵਾਪਸ 1;
	}

	/* find the key and content length */
	ਕੁੰਜੀ_len = content_len =0;
	ਲਈ(iter = ਕੁੰਜੀ ; *iter != '\0'; iter++, ਕੁੰਜੀ_len++);
	ਲਈ(iter = content ; *iter != '\0'; iter++, content_len++);

	/* return if key or content length invalid */
	ਜੇ (ਕੁੰਜੀ_len == 0 || content_len == 0) {
		ਵਾਪਸ 1;
	}

	/* now find the key in the contents */
	ਮੇਲ_len =0;
	ਲਈ (iter = content ,keyp=ਕੁੰਜੀ; ਮੇਲ_len < ਕੁੰਜੀ_len && *iter != '\0' ; iter++) {
		ਜੇ (*iter == *keyp) {
			ਮੇਲ_len++;
			keyp++;
			ਜਾਰੀ;
		} 
		/* The current key does not match the value , reset */
		ਮੇਲ_len =0;
		keyp=ਕੁੰਜੀ;
	}

	ਜੇ (ਮੇਲ_len == ਕੁੰਜੀ_len) {
		ਲਿਖੋ("key found at %d %s\n", ਮੇਲ_len, &content[ਮੇਲ_len]);
		ਮੁੱਲ_len = 0;
		/* copy the content to the value char iter already points to the first 
		   char value */
		ਜਦੋਂ(*iter != '\0' && *iter != ' ' && ਮੁੱਲ_len  + 1< ਮੁੱਲ_max_len) {
			*ਮੁੱਲ++ = *iter++;
			ਮੁੱਲ_len++;
		}
		*ਮੁੱਲ='\0';
		ਵਾਪਸ 0;
	}
	ਵਾਪਸ 1; /* not found */
}

ਸਥਿਰ ਪੂਰਨ mb_ਸੈੱਟ_param(ਅੱਖਰ *bigbuf,ਅੱਖਰ *name,ਅੱਖਰ *ਮੁੱਲ, kinfo_t *cbi)
{
	/* bigbuf contains a list of key=value pairs separated by \0 char.
	 * The list itself is ended by a second \0 terminator*/
	ਅੱਖਰ *p = bigbuf;
	ਅੱਖਰ *bufend = bigbuf + MULTIBOOT_PARAM_BUF_ਮਾਪ;
	ਅੱਖਰ *q;
	ਪੂਰਨ namelen = ਲੜੀ_ਲੰਬਾਈ(name);
	ਪੂਰਨ valuelen = ਲੜੀ_ਲੰਬਾਈ(ਮੁੱਲ);

	/* Some variables we recognize */
	ਜੇ(!strcmp(name, SERVARNAME)) { cbi->do_serial_debug = 1; }
	ਜੇ(!strcmp(name, SERBAUDVARNAME)) { cbi->serial_debug_baud = atoi(ਮੁੱਲ); }

	/* Delete the item if already exists */
	ਜਦੋਂ (*p) {
		ਜੇ (ਲੜੀ_ਤੁਲਨਾ(p, name, namelen) == 0 && p[namelen] == '=') {
			q = p;
			/* let q point to the end of the entry */
			ਜਦੋਂ (*q) q++; 
			/* now copy the remained of the buffer */
			ਲਈ (q++; q < bufend; q++, p++)
				*p = *q;
			ਰੋਕੋ;
		}

		/* find the end of the buffer */
		ਜਦੋਂ (*p++);
		p++;
	}
	

	/* find the first empty spot */
	ਲਈ (p = bigbuf; p < bufend && (*p || *(p + 1)); p++);

	/* unless we are the first entry step over the delimiter */
	ਜੇ (p > bigbuf) p++;
	
	/* Make sure there's enough space for the new parameter */
	ਜੇ (p + namelen + valuelen + 3 > bufend) {
		ਵਾਪਸ -1;
	}
	
	strcpy(p, name);
	p[namelen] = '=';
	strcpy(p + namelen + 1, ਮੁੱਲ);
	p[namelen + valuelen + 1] = 0;
	p[namelen + valuelen + 2] = 0; /* end with a second delimiter */
	ਵਾਪਸ 0;
}

ਪੂਰਨ overlaps(multiboot_module_t *mod, ਪੂਰਨ n, ਪੂਰਨ cmp_mod)
{
	multiboot_module_t *cmp = &mod[cmp_mod];
	ਪੂਰਨ m;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ INRANGE(mod, v) ((v) >= mod->mod_ਸ਼ੁਰੂ && (v) <= thismod->mod_ਸਿਰਾ)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OVERLAP(mod1, mod2) (INRANGE(mod1, mod2->mod_ਸ਼ੁਰੂ) || \
	    INRANGE(mod1, mod2->mod_ਸਿਰਾ))
	ਲਈ(m = 0; m < n; m++) {
		multiboot_module_t *thismod = &mod[m];
		ਜੇ(m == cmp_mod) ਜਾਰੀ;
		ਜੇ(OVERLAP(thismod, cmp)) {
			ਵਾਪਸ 1;
		}
	}
	ਵਾਪਸ 0;
}

/* XXX: hard-coded stuff for modules */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MB_MODS_NR NR_BOOT_MODULES
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MB_MODS_ਅਧਾਰ  0x82000000
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MB_MODS_ALIGN 0x00800000 /* 8 MB */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MB_MMAP_ਸ਼ੁਰੂ 0x80000000
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MB_MMAP_ਮਾਪ  0x10000000 /* 256 MB */

multiboot_module_t mb_modlist[MB_MODS_NR];
multiboot_ਸਮ੍ਰਿਤੀ_map_t mb_memmap;

ਖਾਲੀ ਤਿਆਰੀ_mbi(multiboot_info_t *mbi, ਅੱਖਰ *bootargs)
{
	memset(mbi, 0, ਆਕਾਰ(*mbi));
	mbi->flags = MULTIBOOT_INFO_MODS | MULTIBOOT_INFO_MEM_MAP |
			MULTIBOOT_INFO_CMDLINE;
	mbi->mi_mods_ਗਿਣਤੀ = MB_MODS_NR;
	mbi->mods_addr = (u32_t)&mb_modlist;

	ਪੂਰਨ i;
	ਲਈ (i = 0; i < MB_MODS_NR; ++i) {
		mb_modlist[i].mod_ਸ਼ੁਰੂ = MB_MODS_ਅਧਾਰ + i * MB_MODS_ALIGN;
		mb_modlist[i].mod_ਸਿਰਾ = mb_modlist[i].mod_ਸ਼ੁਰੂ + MB_MODS_ALIGN
		    - ARM_ਪੰਨਾ_ਮਾਪ;
		mb_modlist[i].cmdline = 0;
	}

	/* morph the bootargs into multiboot */
	mbi->cmdline = (u32_t) bootargs;

	mbi->mmap_addr =(u32_t)&mb_memmap;
	mbi->mmap_ਲੰਮਾਈ = ਆਕਾਰ(mb_memmap);

	mb_memmap.ਮਾਪ = ਆਕਾਰ(multiboot_ਸਮ੍ਰਿਤੀ_map_t);
	mb_memmap.mm_ਅਧਾਰ_addr = MB_MMAP_ਸ਼ੁਰੂ;
	mb_memmap.mm_ਲੰਮਾਈ  = MB_MMAP_ਮਾਪ;
	mb_memmap.type = MULTIBOOT_ਸਮ੍ਰਿਤੀ_ਉਪਲਬਧ;
}

ਖਾਲੀ get_parameters(kinfo_t *cbi, ਅੱਖਰ *bootargs)
{
	multiboot_ਸਮ੍ਰਿਤੀ_map_t *mmap;
	multiboot_info_t *mbi = &cbi->mbi;
	ਪੂਰਨ var_i,ਮੁੱਲ_i, m, k;
	ਅੱਖਰ *p;
	ਬਾਹਰੀ ਅੱਖਰ kern_phys_ਅਧਾਰ, kern_vir_ਅਧਾਰ, kern_ਮਾਪ,
	    kern_unpaged_ਸ਼ੁਰੂ, kern_unpaged_ਸਿਰਾ;
	phys_ਬਾਈਟਾਂ kernbase = (phys_ਬਾਈਟਾਂ) &kern_phys_ਅਧਾਰ,
	    kernsize = (phys_ਬਾਈਟਾਂ) &kern_ਮਾਪ;
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BUF 1024
	ਸਥਿਰ ਅੱਖਰ cmdline[BUF];

	/* get our own copy of the multiboot info struct and module list */
	ਤਿਆਰੀ_mbi(mbi, bootargs);

	/* Set various bits of info for the higher-level kernel. */
	cbi->mem_ਉੱਚਾ_phys = 0;
	cbi->user_sp = (vir_ਬਾਈਟਾਂ) &kern_vir_ਅਧਾਰ;
	cbi->vir_kern_ਸ਼ੁਰੂ = (vir_ਬਾਈਟਾਂ) &kern_vir_ਅਧਾਰ;
	cbi->bootstrap_ਸ਼ੁਰੂ = (vir_ਬਾਈਟਾਂ) &kern_unpaged_ਸ਼ੁਰੂ;
	cbi->bootstrap_len = (vir_ਬਾਈਟਾਂ) &kern_unpaged_ਸਿਰਾ -
		cbi->bootstrap_ਸ਼ੁਰੂ;
	cbi->kmess = &kmess;

	/* set some configurable defaults */
	cbi->do_serial_debug = 1;
	cbi->serial_debug_baud = 115200;

	/* parse boot command line */
	ਜੇ (mbi->flags&MULTIBOOT_INFO_CMDLINE) {
		ਸਥਿਰ ਅੱਖਰ var[BUF];
		ਸਥਿਰ ਅੱਖਰ ਮੁੱਲ[BUF];

		/* Override values with cmdline argument */
		ਸਮ੍ਰਿਤੀ_ਨਕਲ(cmdline, (ਖਾਲੀ *) mbi->cmdline, BUF);
		p = cmdline;
		ਜਦੋਂ (*p) {
			var_i = 0;
			ਮੁੱਲ_i = 0;
			ਜਦੋਂ (*p == ' ') p++; /* skip spaces */
			ਜੇ (!*p) ਰੋਕੋ; /* is this the end? */
			ਜਦੋਂ (*p && *p != '=' && *p != ' ' && var_i < BUF - 1)
				var[var_i++] = *p++ ;
			var[var_i] = 0;
			ਜੇ (*p++ != '=') ਜਾਰੀ; /* skip if not name=value */
			ਜਦੋਂ (*p && *p != ' ' && ਮੁੱਲ_i < BUF - 1) {
				ਮੁੱਲ[ਮੁੱਲ_i++] = *p++ ;
			}
			ਮੁੱਲ[ਮੁੱਲ_i] = 0;
			
			mb_ਸੈੱਟ_param(cbi->param_buf, var, ਮੁੱਲ, cbi);
		}
	}

	/* let higher levels know what we are booting on */
	mb_ਸੈੱਟ_param(cbi->param_buf, ARCHVARNAME, (ਅੱਖਰ *)get_board_arch_name(machine.board_id), cbi);
	mb_ਸੈੱਟ_param(cbi->param_buf, BOARDVARNAME,(ਅੱਖਰ *)get_board_name(machine.board_id) , cbi);
	

	/* move user stack/data down to leave a gap to catch kernel
	 * stack overflow; and to distinguish kernel and user addresses
	 * at a glance (0xf.. vs 0xe..) 
	 */
	cbi->user_sp = USR_STACKTOP;
	cbi->user_ਸਿਰਾ = USR_DATATOP;

	/* kernel bytes without bootstrap code/data that is currently
	 * still needed but will be freed after bootstrapping.
	 */
	kinfo.kernel_allocated_ਬਾਈਟਾਂ = (phys_ਬਾਈਟਾਂ) &kern_ਮਾਪ;
	kinfo.kernel_allocated_ਬਾਈਟਾਂ -= cbi->bootstrap_len;

	ਪ੍ਰਤਿਜ੍ਞਾ(!(cbi->bootstrap_ਸ਼ੁਰੂ % ARM_ਪੰਨਾ_ਮਾਪ));
	cbi->bootstrap_len = rounddown(cbi->bootstrap_len, ARM_ਪੰਨਾ_ਮਾਪ);
	ਪ੍ਰਤਿਜ੍ਞਾ(mbi->flags & MULTIBOOT_INFO_MODS);
	ਪ੍ਰਤਿਜ੍ਞਾ(mbi->mi_mods_ਗਿਣਤੀ < MULTIBOOT_MAX_MODS);
	ਪ੍ਰਤਿਜ੍ਞਾ(mbi->mi_mods_ਗਿਣਤੀ > 0);
	ਸਮ੍ਰਿਤੀ_ਨਕਲ(&cbi->module_list, (ਖਾਲੀ *) mbi->mods_addr,
		mbi->mi_mods_ਗਿਣਤੀ * ਆਕਾਰ(multiboot_module_t));
	
	memset(cbi->memmap, 0, ਆਕਾਰ(cbi->memmap));
	/* mem_map has a variable layout */
	ਜੇ(mbi->flags & MULTIBOOT_INFO_MEM_MAP) {
		cbi->mmap_ਮਾਪ = 0;
	        ਲਈ (mmap = (multiboot_ਸਮ੍ਰਿਤੀ_map_t *) mbi->mmap_addr;
       	     (ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ) mmap < mbi->mmap_addr + mbi->mmap_ਲੰਮਾਈ;
       	       mmap = (multiboot_ਸਮ੍ਰਿਤੀ_map_t *) 
		      	((ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ) mmap + mmap->ਮਾਪ + ਆਕਾਰ(mmap->ਮਾਪ))) {
			ਜੇ(mmap->type != MULTIBOOT_ਸਮ੍ਰਿਤੀ_ਉਪਲਬਧ) ਜਾਰੀ;
			ਜੋੜੋ_memmap(cbi, mmap->mm_ਅਧਾਰ_addr, mmap->mm_ਲੰਮਾਈ);
		}
	} ਨਹੀਂਤਾਂ {
		ਪ੍ਰਤਿਜ੍ਞਾ(mbi->flags & MULTIBOOT_INFO_ਸਮ੍ਰਿਤੀ);
		ਜੋੜੋ_memmap(cbi, 0, mbi->mem_ਹੇਠਲਾ_ਅਣਵਰਤਿਆ*1024);
		ਜੋੜੋ_memmap(cbi, 0x100000, mbi->mem_ਉੱਤਲਾ_ਅਣਵਰਤਿਆ*1024);
	}

	/* Sanity check: the kernel nor any of the modules may overlap
	 * with each other. Pretend the kernel is an extra module for a
	 * second.
	 */
	k = mbi->mi_mods_ਗਿਣਤੀ;
	ਪ੍ਰਤਿਜ੍ਞਾ(k < MULTIBOOT_MAX_MODS);
	cbi->module_list[k].mod_ਸ਼ੁਰੂ = kernbase;
	cbi->module_list[k].mod_ਸਿਰਾ = kernbase + kernsize;
	cbi->mods_with_kernel = mbi->mi_mods_ਗਿਣਤੀ+1;
	cbi->kern_mod = k;

	ਲਈ(m = 0; m < cbi->mods_with_kernel; m++) {
#ਜੇ 0
		ਲਿਖੋ("checking overlap of module %08lx-%08lx\n",
		  cbi->module_list[m].mod_ਸ਼ੁਰੂ, cbi->module_list[m].mod_ਸਿਰਾ);
#ਜੇ_ਅੰਤ
		ਜੇ(overlaps(cbi->module_list, cbi->mods_with_kernel, m))
			panic("overlapping boot modules/kernel");
		/* We cut out the bits of memory that we know are
		 * occupied by the kernel and boot modules.
		 */
		cut_memmap(cbi,
			cbi->module_list[m].mod_ਸ਼ੁਰੂ,
			cbi->module_list[m].mod_ਸਿਰਾ);
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
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ POORMANS_FAILURE_NOTIFICATION  asm ਅਸਥਿਰ("svc #00\n")

/* use the passed cmdline argument to determine the machine id */
ਖਾਲੀ ਸੈੱਟ_machine_id(ਅੱਖਰ *cmdline)
{

	ਅੱਖਰ boardname[20];
	memset(boardname,'\0',20);
	ਜੇ (ਲੱਭੋ_ਮੁੱਲ(cmdline,"board_name=",boardname,20)){
		/* we expect the bootloader to pass a board_name as argument
		 * this however did not happen and given we still are in early
		 * boot we can't use the serial. We therefore generate an interrupt
		 * and hope the bootloader will do something nice with it */
		POORMANS_FAILURE_NOTIFICATION;
	}  
	machine.board_id = get_board_id_by_short_name(boardname);

	ਜੇ (machine.board_id ==0){
		/* same thing as above there is no safe escape */
		POORMANS_FAILURE_NOTIFICATION;
	}
}

kinfo_t *pre_init(ਪੂਰਨ argc, ਅੱਖਰ **argv)
{
	ਅੱਖਰ *bootargs;
	/* This is the main "c" entry point into the kernel. It gets called
	   from head.S */
	   
	/* Clear BSS */
	memset(&_edata, 0, (u32_t)&ਸਿਰਾ_2 - (u32_t)&_edata);
        memset(&_kern_unpaged_edata, 0, (u32_t)&kern_unpaged_ਸਿਰਾ - (u32_t)&_kern_unpaged_edata);

	/* we get called in a c like fashion where the first arg
         * is the program name (load address) and the rest are
	 * arguments. by convention the second argument is the
	 *  command line */
	ਜੇ (argc != 2) {
		POORMANS_FAILURE_NOTIFICATION;
	}

	bootargs = argv[1];
	ਸੈੱਟ_machine_id(bootargs);
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
	pg_ਪਛਾਣ_ਤੱਤ(&kinfo);
	kinfo.freepde_ਸ਼ੁਰੂ = pg_mapkernel();
	pg_ਲੱਦੋ();
	vm_enable_paging();

	/* Done, return boot info so it can be passed to kmain(). */
	ਵਾਪਸ &kinfo;
}

/* pre_init gets executed at the memory location where the kernel was loaded by the boot loader.
 * at that stage we only have a minimum set of functionality present (all symbols gets renamed to
 * ensure this). The following methods are used in that context. Once we jump to kmain they are no
 * longer used and the "real" implementations are visible
 */
ਖਾਲੀ send_diag_sig(ਖਾਲੀ) { }
ਖਾਲੀ minix_shutdown(ਪੂਰਨ how) { arch_shutdown(how); }
ਖਾਲੀ busy_delay_ms(ਪੂਰਨ x) { }
ਪੂਰਨ ਉਠਾਓ(ਪੂਰਨ n) { panic("raise(%d)\n", n); }
ਪੂਰਨ kern_phys_map_ptr( phys_ਬਾਈਟਾਂ ਅਧਾਰ_ਪਤਾ, vir_ਬਾਈਟਾਂ io_ਮਾਪ, ਪੂਰਨ vm_flags,
ਬਣਤਰ kern_phys_map * priv, vir_ਬਾਈਟਾਂ ptr) { ਵਾਪਸ -1; };
ਬਣਤਰ machine machine; /* pre init stage machine */
