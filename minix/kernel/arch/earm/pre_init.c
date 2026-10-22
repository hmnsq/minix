#ವ್ಯಾಖ್ಯಾನಿಸು UNPAGED 1	/* for proper kmain() prototype */

#ಅನ್ತರ್ಭಾವ "kernel/kernel.h"
#ಅನ್ತರ್ಭಾವ <assert.h>
#ಅನ್ತರ್ಭಾವ <stdlib.h>
#ಅನ್ತರ್ಭಾವ <minix/minlib.h>
#ಅನ್ತರ್ಭಾವ <minix/const.h>
#ಅನ್ತರ್ಭಾವ <minix/type.h>
#ಅನ್ತರ್ಭಾವ <minix/board.h>
#ಅನ್ತರ್ಭಾವ <minix/com.h>
#ಅನ್ತರ್ಭಾವ <sys/types.h>
#ಅನ್ತರ್ಭಾವ <sys/param.h>
#ಅನ್ತರ್ಭಾವ <sys/reboot.h>
#ಅನ್ತರ್ಭಾವ "string.h"
#ಅನ್ತರ್ಭಾವ "arch_proto.h"
#ಅನ್ತರ್ಭಾವ "direct_utils.h"
#ಅನ್ತರ್ಭಾವ "bsp_serial.h"
#ಅನ್ತರ್ಭಾವ "glo.h"
#ಅನ್ತರ್ಭಾವ <machine/multiboot.h>

#ಆದರೆ USE_SYSDEBUG
#ವ್ಯಾಖ್ಯಾನಿಸು MULTIBOOT_VERBOSE 1
#ಆದರೆ_ಅಂತ್ಯ

/* to-be-built kinfo struct, diagnostics buffer */
kinfo_t kinfo;
ರಚನೆ kmessages kmessages;

/* pg_utils.c uses this; in this phase, there is a 1:1 mapping. */
phys_ಬೈಟುಗಳು vir2phys(ಶೂನ್ಯ *addr) { ಹಿಂತಿರುಗಿ (phys_ಬೈಟುಗಳು) addr; }

ಸ್ಥಿರ ಶೂನ್ಯ ಸಿದ್ಧತೆ_mbi(multiboot_info_t *mbi, ಅಕ್ಷರ *bootargs);

/* String length used for mb_itoa */
#ವ್ಯಾಖ್ಯಾನಿಸು ITOA_ಬಫರ್_ಗಾತ್ರ 20

/* Kernel may use memory */
ಪೂರ್ಣಾಂಕ kernel_may_alloc = 1;

/* kernel bss */
ಬಾಹ್ಯ u32_t _edata;
ಬಾಹ್ಯ u32_t ಕೊನೆ_2;

/* kernel unpaged bss */
ಬಾಹ್ಯ ಅಕ್ಷರ _kern_unpaged_edata;
ಬಾಹ್ಯ ಅಕ್ಷರ kern_unpaged_ಕೊನೆ;

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
ಪೂರ್ಣಾಂಕ ಕಂಡುಹಿಡಿ_ಮೌಲ್ಯ(ಅಕ್ಷರ * content,ಅಕ್ಷರ * ಕೀಲಿ,ಅಕ್ಷರ *ಮೌಲ್ಯ,ಪೂರ್ಣಾಂಕ ಮೌಲ್ಯ_max_len){

	ಅಕ್ಷರ *iter,*keyp;
	ಪೂರ್ಣಾಂಕ ಕೀಲಿ_len,content_len,ಹೊಂದಿಕೆ_len,ಮೌಲ್ಯ_len;

	/* return if the input is invalid */
	ಆದರೆ  (ಕೀಲಿ == ಖಾಲಿ || content == ಖಾಲಿ || ಮೌಲ್ಯ == ಖಾಲಿ) {
		ಹಿಂತಿರುಗಿ 1;
	}

	/* find the key and content length */
	ಕೀಲಿ_len = content_len =0;
	ಗಾಗಿ(iter = ಕೀಲಿ ; *iter != '\0'; iter++, ಕೀಲಿ_len++);
	ಗಾಗಿ(iter = content ; *iter != '\0'; iter++, content_len++);

	/* return if key or content length invalid */
	ಆದರೆ (ಕೀಲಿ_len == 0 || content_len == 0) {
		ಹಿಂತಿರುಗಿ 1;
	}

	/* now find the key in the contents */
	ಹೊಂದಿಕೆ_len =0;
	ಗಾಗಿ (iter = content ,keyp=ಕೀಲಿ; ಹೊಂದಿಕೆ_len < ಕೀಲಿ_len && *iter != '\0' ; iter++) {
		ಆದರೆ (*iter == *keyp) {
			ಹೊಂದಿಕೆ_len++;
			keyp++;
			ಮುಂದುವರಿ;
		} 
		/* The current key does not match the value , reset */
		ಹೊಂದಿಕೆ_len =0;
		keyp=ಕೀಲಿ;
	}

	ಆದರೆ (ಹೊಂದಿಕೆ_len == ಕೀಲಿ_len) {
		ಬರೆ("key found at %d %s\n", ಹೊಂದಿಕೆ_len, &content[ಹೊಂದಿಕೆ_len]);
		ಮೌಲ್ಯ_len = 0;
		/* copy the content to the value char iter already points to the first 
		   char value */
		ಆಗ(*iter != '\0' && *iter != ' ' && ಮೌಲ್ಯ_len  + 1< ಮೌಲ್ಯ_max_len) {
			*ಮೌಲ್ಯ++ = *iter++;
			ಮೌಲ್ಯ_len++;
		}
		*ಮೌಲ್ಯ='\0';
		ಹಿಂತಿರುಗಿ 0;
	}
	ಹಿಂತಿರುಗಿ 1; /* not found */
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ mb_ಗಣ_param(ಅಕ್ಷರ *bigbuf,ಅಕ್ಷರ *name,ಅಕ್ಷರ *ಮೌಲ್ಯ, kinfo_t *cbi)
{
	/* bigbuf contains a list of key=value pairs separated by \0 char.
	 * The list itself is ended by a second \0 terminator*/
	ಅಕ್ಷರ *p = bigbuf;
	ಅಕ್ಷರ *bufend = bigbuf + MULTIBOOT_PARAM_BUF_ಗಾತ್ರ;
	ಅಕ್ಷರ *q;
	ಪೂರ್ಣಾಂಕ namelen = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(name);
	ಪೂರ್ಣಾಂಕ valuelen = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ಮೌಲ್ಯ);

	/* Some variables we recognize */
	ಆದರೆ(!strcmp(name, SERVARNAME)) { cbi->do_serial_debug = 1; }
	ಆದರೆ(!strcmp(name, SERBAUDVARNAME)) { cbi->serial_debug_baud = atoi(ಮೌಲ್ಯ); }

	/* Delete the item if already exists */
	ಆಗ (*p) {
		ಆದರೆ (ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(p, name, namelen) == 0 && p[namelen] == '=') {
			q = p;
			/* let q point to the end of the entry */
			ಆಗ (*q) q++; 
			/* now copy the remained of the buffer */
			ಗಾಗಿ (q++; q < bufend; q++, p++)
				*p = *q;
			ನಿಲ್ಲಿಸು;
		}

		/* find the end of the buffer */
		ಆಗ (*p++);
		p++;
	}
	

	/* find the first empty spot */
	ಗಾಗಿ (p = bigbuf; p < bufend && (*p || *(p + 1)); p++);

	/* unless we are the first entry step over the delimiter */
	ಆದರೆ (p > bigbuf) p++;
	
	/* Make sure there's enough space for the new parameter */
	ಆದರೆ (p + namelen + valuelen + 3 > bufend) {
		ಹಿಂತಿರುಗಿ -1;
	}
	
	strcpy(p, name);
	p[namelen] = '=';
	strcpy(p + namelen + 1, ಮೌಲ್ಯ);
	p[namelen + valuelen + 1] = 0;
	p[namelen + valuelen + 2] = 0; /* end with a second delimiter */
	ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ overlaps(multiboot_module_t *mod, ಪೂರ್ಣಾಂಕ n, ಪೂರ್ಣಾಂಕ cmp_mod)
{
	multiboot_module_t *cmp = &mod[cmp_mod];
	ಪೂರ್ಣಾಂಕ m;

#ವ್ಯಾಖ್ಯಾನಿಸು INRANGE(mod, v) ((v) >= mod->mod_ಆರಂಭ && (v) <= thismod->mod_ಕೊನೆ)
#ವ್ಯಾಖ್ಯಾನಿಸು OVERLAP(mod1, mod2) (INRANGE(mod1, mod2->mod_ಆರಂಭ) || \
	    INRANGE(mod1, mod2->mod_ಕೊನೆ))
	ಗಾಗಿ(m = 0; m < n; m++) {
		multiboot_module_t *thismod = &mod[m];
		ಆದರೆ(m == cmp_mod) ಮುಂದುವರಿ;
		ಆದರೆ(OVERLAP(thismod, cmp)) {
			ಹಿಂತಿರುಗಿ 1;
		}
	}
	ಹಿಂತಿರುಗಿ 0;
}

/* XXX: hard-coded stuff for modules */
#ವ್ಯಾಖ್ಯಾನಿಸು MB_MODS_NR NR_BOOT_MODULES
#ವ್ಯಾಖ್ಯಾನಿಸು MB_MODS_ತಳ  0x82000000
#ವ್ಯಾಖ್ಯಾನಿಸು MB_MODS_ALIGN 0x00800000 /* 8 MB */
#ವ್ಯಾಖ್ಯಾನಿಸು MB_MMAP_ಆರಂಭ 0x80000000
#ವ್ಯಾಖ್ಯಾನಿಸು MB_MMAP_ಗಾತ್ರ  0x10000000 /* 256 MB */

multiboot_module_t mb_modlist[MB_MODS_NR];
multiboot_ಸ್ಮರಣೆ_map_t mb_memmap;

ಶೂನ್ಯ ಸಿದ್ಧತೆ_mbi(multiboot_info_t *mbi, ಅಕ್ಷರ *bootargs)
{
	memset(mbi, 0, ಗಾತ್ರ(*mbi));
	mbi->flags = MULTIBOOT_INFO_MODS | MULTIBOOT_INFO_MEM_MAP |
			MULTIBOOT_INFO_CMDLINE;
	mbi->mi_mods_ಎಣಿಕೆ = MB_MODS_NR;
	mbi->mods_addr = (u32_t)&mb_modlist;

	ಪೂರ್ಣಾಂಕ i;
	ಗಾಗಿ (i = 0; i < MB_MODS_NR; ++i) {
		mb_modlist[i].mod_ಆರಂಭ = MB_MODS_ತಳ + i * MB_MODS_ALIGN;
		mb_modlist[i].mod_ಕೊನೆ = mb_modlist[i].mod_ಆರಂಭ + MB_MODS_ALIGN
		    - ARM_ಪುಟ_ಗಾತ್ರ;
		mb_modlist[i].cmdline = 0;
	}

	/* morph the bootargs into multiboot */
	mbi->cmdline = (u32_t) bootargs;

	mbi->mmap_addr =(u32_t)&mb_memmap;
	mbi->mmap_ಉದ್ದಳತೆ = ಗಾತ್ರ(mb_memmap);

	mb_memmap.ಗಾತ್ರ_2 = ಗಾತ್ರ(multiboot_ಸ್ಮರಣೆ_map_t);
	mb_memmap.mm_ತಳ_addr = MB_MMAP_ಆರಂಭ;
	mb_memmap.mm_ಉದ್ದಳತೆ  = MB_MMAP_ಗಾತ್ರ;
	mb_memmap.type = MULTIBOOT_ಸ್ಮರಣೆ_ಲಭ್ಯ;
}

ಶೂನ್ಯ get_parameters(kinfo_t *cbi, ಅಕ್ಷರ *bootargs)
{
	multiboot_ಸ್ಮರಣೆ_map_t *mmap;
	multiboot_info_t *mbi = &cbi->mbi;
	ಪೂರ್ಣಾಂಕ var_i,ಮೌಲ್ಯ_i, m, k;
	ಅಕ್ಷರ *p;
	ಬಾಹ್ಯ ಅಕ್ಷರ kern_phys_ತಳ, kern_vir_ತಳ, kern_ಗಾತ್ರ,
	    kern_unpaged_ಆರಂಭ, kern_unpaged_ಕೊನೆ;
	phys_ಬೈಟುಗಳು kernbase = (phys_ಬೈಟುಗಳು) &kern_phys_ತಳ,
	    kernsize = (phys_ಬೈಟುಗಳು) &kern_ಗಾತ್ರ;
#ವ್ಯಾಖ್ಯಾನಿಸು BUF 1024
	ಸ್ಥಿರ ಅಕ್ಷರ cmdline[BUF];

	/* get our own copy of the multiboot info struct and module list */
	ಸಿದ್ಧತೆ_mbi(mbi, bootargs);

	/* Set various bits of info for the higher-level kernel. */
	cbi->mem_ಮೇಲಿನ_phys = 0;
	cbi->user_sp = (vir_ಬೈಟುಗಳು) &kern_vir_ತಳ;
	cbi->vir_kern_ಆರಂಭ = (vir_ಬೈಟುಗಳು) &kern_vir_ತಳ;
	cbi->bootstrap_ಆರಂಭ = (vir_ಬೈಟುಗಳು) &kern_unpaged_ಆರಂಭ;
	cbi->bootstrap_len = (vir_ಬೈಟುಗಳು) &kern_unpaged_ಕೊನೆ -
		cbi->bootstrap_ಆರಂಭ;
	cbi->kmess = &kmess;

	/* set some configurable defaults */
	cbi->do_serial_debug = 1;
	cbi->serial_debug_baud = 115200;

	/* parse boot command line */
	ಆದರೆ (mbi->flags&MULTIBOOT_INFO_CMDLINE) {
		ಸ್ಥಿರ ಅಕ್ಷರ var[BUF];
		ಸ್ಥಿರ ಅಕ್ಷರ ಮೌಲ್ಯ[BUF];

		/* Override values with cmdline argument */
		ಸ್ಮರಣೆ_ನಕಲು(cmdline, (ಶೂನ್ಯ *) mbi->cmdline, BUF);
		p = cmdline;
		ಆಗ (*p) {
			var_i = 0;
			ಮೌಲ್ಯ_i = 0;
			ಆಗ (*p == ' ') p++; /* skip spaces */
			ಆದರೆ (!*p) ನಿಲ್ಲಿಸು; /* is this the end? */
			ಆಗ (*p && *p != '=' && *p != ' ' && var_i < BUF - 1)
				var[var_i++] = *p++ ;
			var[var_i] = 0;
			ಆದರೆ (*p++ != '=') ಮುಂದುವರಿ; /* skip if not name=value */
			ಆಗ (*p && *p != ' ' && ಮೌಲ್ಯ_i < BUF - 1) {
				ಮೌಲ್ಯ[ಮೌಲ್ಯ_i++] = *p++ ;
			}
			ಮೌಲ್ಯ[ಮೌಲ್ಯ_i] = 0;
			
			mb_ಗಣ_param(cbi->param_buf, var, ಮೌಲ್ಯ, cbi);
		}
	}

	/* let higher levels know what we are booting on */
	mb_ಗಣ_param(cbi->param_buf, ARCHVARNAME, (ಅಕ್ಷರ *)get_board_arch_name(machine.board_id), cbi);
	mb_ಗಣ_param(cbi->param_buf, BOARDVARNAME,(ಅಕ್ಷರ *)get_board_name(machine.board_id) , cbi);
	

	/* move user stack/data down to leave a gap to catch kernel
	 * stack overflow; and to distinguish kernel and user addresses
	 * at a glance (0xf.. vs 0xe..) 
	 */
	cbi->user_sp = USR_STACKTOP;
	cbi->user_ಕೊನೆ = USR_DATATOP;

	/* kernel bytes without bootstrap code/data that is currently
	 * still needed but will be freed after bootstrapping.
	 */
	kinfo.kernel_allocated_ಬೈಟುಗಳು = (phys_ಬೈಟುಗಳು) &kern_ಗಾತ್ರ;
	kinfo.kernel_allocated_ಬೈಟುಗಳು -= cbi->bootstrap_len;

	ಪ್ರತಿಜ್ಞಾ(!(cbi->bootstrap_ಆರಂಭ % ARM_ಪುಟ_ಗಾತ್ರ));
	cbi->bootstrap_len = rounddown(cbi->bootstrap_len, ARM_ಪುಟ_ಗಾತ್ರ);
	ಪ್ರತಿಜ್ಞಾ(mbi->flags & MULTIBOOT_INFO_MODS);
	ಪ್ರತಿಜ್ಞಾ(mbi->mi_mods_ಎಣಿಕೆ < MULTIBOOT_MAX_MODS);
	ಪ್ರತಿಜ್ಞಾ(mbi->mi_mods_ಎಣಿಕೆ > 0);
	ಸ್ಮರಣೆ_ನಕಲು(&cbi->module_list, (ಶೂನ್ಯ *) mbi->mods_addr,
		mbi->mi_mods_ಎಣಿಕೆ * ಗಾತ್ರ(multiboot_module_t));
	
	memset(cbi->memmap, 0, ಗಾತ್ರ(cbi->memmap));
	/* mem_map has a variable layout */
	ಆದರೆ(mbi->flags & MULTIBOOT_INFO_MEM_MAP) {
		cbi->mmap_ಗಾತ್ರ = 0;
	        ಗಾಗಿ (mmap = (multiboot_ಸ್ಮರಣೆ_map_t *) mbi->mmap_addr;
       	     (ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ) mmap < mbi->mmap_addr + mbi->mmap_ಉದ್ದಳತೆ;
       	       mmap = (multiboot_ಸ್ಮರಣೆ_map_t *) 
		      	((ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ) mmap + mmap->ಗಾತ್ರ_2 + ಗಾತ್ರ(mmap->ಗಾತ್ರ_2))) {
			ಆದರೆ(mmap->type != MULTIBOOT_ಸ್ಮರಣೆ_ಲಭ್ಯ) ಮುಂದುವರಿ;
			ಕೂಡಿಸು_memmap(cbi, mmap->mm_ತಳ_addr, mmap->mm_ಉದ್ದಳತೆ);
		}
	} ಇಲ್ಲದಿದ್ದರೆ {
		ಪ್ರತಿಜ್ಞಾ(mbi->flags & MULTIBOOT_INFO_ಸ್ಮರಣೆ);
		ಕೂಡಿಸು_memmap(cbi, 0, mbi->mem_ಅಧೋ_ಬಳಸದ*1024);
		ಕೂಡಿಸು_memmap(cbi, 0x100000, mbi->mem_ಊರ್ಧ್ವ_ಬಳಸದ*1024);
	}

	/* Sanity check: the kernel nor any of the modules may overlap
	 * with each other. Pretend the kernel is an extra module for a
	 * second.
	 */
	k = mbi->mi_mods_ಎಣಿಕೆ;
	ಪ್ರತಿಜ್ಞಾ(k < MULTIBOOT_MAX_MODS);
	cbi->module_list[k].mod_ಆರಂಭ = kernbase;
	cbi->module_list[k].mod_ಕೊನೆ = kernbase + kernsize;
	cbi->mods_with_kernel = mbi->mi_mods_ಎಣಿಕೆ+1;
	cbi->kern_mod = k;

	ಗಾಗಿ(m = 0; m < cbi->mods_with_kernel; m++) {
#ಆದರೆ 0
		ಬರೆ("checking overlap of module %08lx-%08lx\n",
		  cbi->module_list[m].mod_ಆರಂಭ, cbi->module_list[m].mod_ಕೊನೆ);
#ಆದರೆ_ಅಂತ್ಯ
		ಆದರೆ(overlaps(cbi->module_list, cbi->mods_with_kernel, m))
			panic("overlapping boot modules/kernel");
		/* We cut out the bits of memory that we know are
		 * occupied by the kernel and boot modules.
		 */
		cut_memmap(cbi,
			cbi->module_list[m].mod_ಆರಂಭ,
			cbi->module_list[m].mod_ಕೊನೆ);
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
#ವ್ಯಾಖ್ಯಾನಿಸು POORMANS_FAILURE_NOTIFICATION  asm ಅಸ್ಥಿರ("svc #00\n")

/* use the passed cmdline argument to determine the machine id */
ಶೂನ್ಯ ಗಣ_machine_id(ಅಕ್ಷರ *cmdline)
{

	ಅಕ್ಷರ boardname[20];
	memset(boardname,'\0',20);
	ಆದರೆ (ಕಂಡುಹಿಡಿ_ಮೌಲ್ಯ(cmdline,"board_name=",boardname,20)){
		/* we expect the bootloader to pass a board_name as argument
		 * this however did not happen and given we still are in early
		 * boot we can't use the serial. We therefore generate an interrupt
		 * and hope the bootloader will do something nice with it */
		POORMANS_FAILURE_NOTIFICATION;
	}  
	machine.board_id = get_board_id_by_short_name(boardname);

	ಆದರೆ (machine.board_id ==0){
		/* same thing as above there is no safe escape */
		POORMANS_FAILURE_NOTIFICATION;
	}
}

kinfo_t *pre_init(ಪೂರ್ಣಾಂಕ argc, ಅಕ್ಷರ **argv)
{
	ಅಕ್ಷರ *bootargs;
	/* This is the main "c" entry point into the kernel. It gets called
	   from head.S */
	   
	/* Clear BSS */
	memset(&_edata, 0, (u32_t)&ಕೊನೆ_2 - (u32_t)&_edata);
        memset(&_kern_unpaged_edata, 0, (u32_t)&kern_unpaged_ಕೊನೆ - (u32_t)&_kern_unpaged_edata);

	/* we get called in a c like fashion where the first arg
         * is the program name (load address) and the rest are
	 * arguments. by convention the second argument is the
	 *  command line */
	ಆದರೆ (argc != 2) {
		POORMANS_FAILURE_NOTIFICATION;
	}

	bootargs = argv[1];
	ಗಣ_machine_id(bootargs);
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
	pg_ತಟಸ್ಥ(&kinfo);
	kinfo.freepde_ಆರಂಭ = pg_mapkernel();
	pg_ತುಂಬು();
	vm_enable_paging();

	/* Done, return boot info so it can be passed to kmain(). */
	ಹಿಂತಿರುಗಿ &kinfo;
}

/* pre_init gets executed at the memory location where the kernel was loaded by the boot loader.
 * at that stage we only have a minimum set of functionality present (all symbols gets renamed to
 * ensure this). The following methods are used in that context. Once we jump to kmain they are no
 * longer used and the "real" implementations are visible
 */
ಶೂನ್ಯ send_diag_sig(ಶೂನ್ಯ) { }
ಶೂನ್ಯ minix_shutdown(ಪೂರ್ಣಾಂಕ how) { arch_shutdown(how); }
ಶೂನ್ಯ busy_delay_ms(ಪೂರ್ಣಾಂಕ x) { }
ಪೂರ್ಣಾಂಕ ಎತ್ತು(ಪೂರ್ಣಾಂಕ n) { panic("raise(%d)\n", n); }
ಪೂರ್ಣಾಂಕ kern_phys_map_ptr( phys_ಬೈಟುಗಳು ತಳ_ವಿಳಾಸ, vir_ಬೈಟುಗಳು io_ಗಾತ್ರ, ಪೂರ್ಣಾಂಕ vm_flags,
ರಚನೆ kern_phys_map * priv, vir_ಬೈಟುಗಳು ptr) { ಹಿಂತಿರುಗಿ -1; };
ರಚನೆ machine machine; /* pre init stage machine */
