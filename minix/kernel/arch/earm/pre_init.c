#నిర్వచించు UNPAGED 1	/* for proper kmain() prototype */

#అన్తర్భావ "kernel/kernel.h"
#అన్తర్భావ <assert.h>
#అన్తర్భావ <stdlib.h>
#అన్తర్భావ <minix/minlib.h>
#అన్తర్భావ <minix/const.h>
#అన్తర్భావ <minix/type.h>
#అన్తర్భావ <minix/board.h>
#అన్తర్భావ <minix/com.h>
#అన్తర్భావ <sys/types.h>
#అన్తర్భావ <sys/param.h>
#అన్తర్భావ <sys/reboot.h>
#అన్తర్భావ "string.h"
#అన్తర్భావ "arch_proto.h"
#అన్తర్భావ "direct_utils.h"
#అన్తర్భావ "bsp_serial.h"
#అన్తర్భావ "glo.h"
#అన్తర్భావ <machine/multiboot.h>

#ఒకవేళ USE_SYSDEBUG
#నిర్వచించు MULTIBOOT_VERBOSE 1
#అయితే_ముగింపు

/* to-be-built kinfo struct, diagnostics buffer */
kinfo_t kinfo;
నిర్మాణం kmessages kmessages;

/* pg_utils.c uses this; in this phase, there is a 1:1 mapping. */
phys_బైట్లు vir2phys(శూన్య *addr) { తిరిగి (phys_బైట్లు) addr; }

స్థిర శూన్య అమరిక_mbi(multiboot_info_t *mbi, అక్షరం *bootargs);

/* String length used for mb_itoa */
#నిర్వచించు ITOA_బఫరు_కొలత 20

/* Kernel may use memory */
పూర్ణాంకం kernel_may_alloc = 1;

/* kernel bss */
బాహ్య u32_t _edata;
బాహ్య u32_t చివర_2;

/* kernel unpaged bss */
బాహ్య అక్షరం _kern_unpaged_edata;
బాహ్య అక్షరం kern_unpaged_చివర;

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
పూర్ణాంకం కనుగొను_విలువ(అక్షరం * content,అక్షరం * కీలకం,అక్షరం *విలువ,పూర్ణాంకం విలువ_max_len){

	అక్షరం *iter,*keyp;
	పూర్ణాంకం కీలకం_len,content_len,సరిపోలిక_len,విలువ_len;

	/* return if the input is invalid */
	ఒకవేళ  (కీలకం == ఖాళీ || content == ఖాళీ || విలువ == ఖాళీ) {
		తిరిగి 1;
	}

	/* find the key and content length */
	కీలకం_len = content_len =0;
	కొరకు(iter = కీలకం ; *iter != '\0'; iter++, కీలకం_len++);
	కొరకు(iter = content ; *iter != '\0'; iter++, content_len++);

	/* return if key or content length invalid */
	ఒకవేళ (కీలకం_len == 0 || content_len == 0) {
		తిరిగి 1;
	}

	/* now find the key in the contents */
	సరిపోలిక_len =0;
	కొరకు (iter = content ,keyp=కీలకం; సరిపోలిక_len < కీలకం_len && *iter != '\0' ; iter++) {
		ఒకవేళ (*iter == *keyp) {
			సరిపోలిక_len++;
			keyp++;
			కొనసాగు;
		} 
		/* The current key does not match the value , reset */
		సరిపోలిక_len =0;
		keyp=కీలకం;
	}

	ఒకవేళ (సరిపోలిక_len == కీలకం_len) {
		రాయి("key found at %d %s\n", సరిపోలిక_len, &content[సరిపోలిక_len]);
		విలువ_len = 0;
		/* copy the content to the value char iter already points to the first 
		   char value */
		అయితే(*iter != '\0' && *iter != ' ' && విలువ_len  + 1< విలువ_max_len) {
			*విలువ++ = *iter++;
			విలువ_len++;
		}
		*విలువ='\0';
		తిరిగి 0;
	}
	తిరిగి 1; /* not found */
}

స్థిర పూర్ణాంకం mb_సమితి_param(అక్షరం *bigbuf,అక్షరం *name,అక్షరం *విలువ, kinfo_t *cbi)
{
	/* bigbuf contains a list of key=value pairs separated by \0 char.
	 * The list itself is ended by a second \0 terminator*/
	అక్షరం *p = bigbuf;
	అక్షరం *bufend = bigbuf + MULTIBOOT_PARAM_BUF_కొలత;
	అక్షరం *q;
	పూర్ణాంకం namelen = అక్షరశ్రేణి_పొడవు(name);
	పూర్ణాంకం valuelen = అక్షరశ్రేణి_పొడవు(విలువ);

	/* Some variables we recognize */
	ఒకవేళ(!strcmp(name, SERVARNAME)) { cbi->do_serial_debug = 1; }
	ఒకవేళ(!strcmp(name, SERBAUDVARNAME)) { cbi->serial_debug_baud = atoi(విలువ); }

	/* Delete the item if already exists */
	అయితే (*p) {
		ఒకవేళ (అక్షరశ్రేణి_పోలిక(p, name, namelen) == 0 && p[namelen] == '=') {
			q = p;
			/* let q point to the end of the entry */
			అయితే (*q) q++; 
			/* now copy the remained of the buffer */
			కొరకు (q++; q < bufend; q++, p++)
				*p = *q;
			ఆపు;
		}

		/* find the end of the buffer */
		అయితే (*p++);
		p++;
	}
	

	/* find the first empty spot */
	కొరకు (p = bigbuf; p < bufend && (*p || *(p + 1)); p++);

	/* unless we are the first entry step over the delimiter */
	ఒకవేళ (p > bigbuf) p++;
	
	/* Make sure there's enough space for the new parameter */
	ఒకవేళ (p + namelen + valuelen + 3 > bufend) {
		తిరిగి -1;
	}
	
	strcpy(p, name);
	p[namelen] = '=';
	strcpy(p + namelen + 1, విలువ);
	p[namelen + valuelen + 1] = 0;
	p[namelen + valuelen + 2] = 0; /* end with a second delimiter */
	తిరిగి 0;
}

పూర్ణాంకం overlaps(multiboot_module_t *mod, పూర్ణాంకం n, పూర్ణాంకం cmp_mod)
{
	multiboot_module_t *cmp = &mod[cmp_mod];
	పూర్ణాంకం m;

#నిర్వచించు INRANGE(mod, v) ((v) >= mod->mod_ప్రారంభం && (v) <= thismod->mod_చివర)
#నిర్వచించు OVERLAP(mod1, mod2) (INRANGE(mod1, mod2->mod_ప్రారంభం) || \
	    INRANGE(mod1, mod2->mod_చివర))
	కొరకు(m = 0; m < n; m++) {
		multiboot_module_t *thismod = &mod[m];
		ఒకవేళ(m == cmp_mod) కొనసాగు;
		ఒకవేళ(OVERLAP(thismod, cmp)) {
			తిరిగి 1;
		}
	}
	తిరిగి 0;
}

/* XXX: hard-coded stuff for modules */
#నిర్వచించు MB_MODS_NR NR_BOOT_MODULES
#నిర్వచించు MB_MODS_పునాది  0x82000000
#నిర్వచించు MB_MODS_ALIGN 0x00800000 /* 8 MB */
#నిర్వచించు MB_MMAP_ప్రారంభం 0x80000000
#నిర్వచించు MB_MMAP_కొలత  0x10000000 /* 256 MB */

multiboot_module_t mb_modlist[MB_MODS_NR];
multiboot_జ్ఞాపకం_map_t mb_memmap;

శూన్య అమరిక_mbi(multiboot_info_t *mbi, అక్షరం *bootargs)
{
	memset(mbi, 0, పరిమాణం(*mbi));
	mbi->flags = MULTIBOOT_INFO_MODS | MULTIBOOT_INFO_MEM_MAP |
			MULTIBOOT_INFO_CMDLINE;
	mbi->mi_mods_లెక్క = MB_MODS_NR;
	mbi->mods_addr = (u32_t)&mb_modlist;

	పూర్ణాంకం i;
	కొరకు (i = 0; i < MB_MODS_NR; ++i) {
		mb_modlist[i].mod_ప్రారంభం = MB_MODS_పునాది + i * MB_MODS_ALIGN;
		mb_modlist[i].mod_చివర = mb_modlist[i].mod_ప్రారంభం + MB_MODS_ALIGN
		    - ARM_పుట_కొలత;
		mb_modlist[i].cmdline = 0;
	}

	/* morph the bootargs into multiboot */
	mbi->cmdline = (u32_t) bootargs;

	mbi->mmap_addr =(u32_t)&mb_memmap;
	mbi->mmap_నిడివి = పరిమాణం(mb_memmap);

	mb_memmap.కొలత = పరిమాణం(multiboot_జ్ఞాపకం_map_t);
	mb_memmap.mm_పునాది_addr = MB_MMAP_ప్రారంభం;
	mb_memmap.mm_నిడివి  = MB_MMAP_కొలత;
	mb_memmap.type = MULTIBOOT_జ్ఞాపకం_అందుబాటు;
}

శూన్య get_parameters(kinfo_t *cbi, అక్షరం *bootargs)
{
	multiboot_జ్ఞాపకం_map_t *mmap;
	multiboot_info_t *mbi = &cbi->mbi;
	పూర్ణాంకం var_i,విలువ_i, m, k;
	అక్షరం *p;
	బాహ్య అక్షరం kern_phys_పునాది, kern_vir_పునాది, kern_కొలత,
	    kern_unpaged_ప్రారంభం, kern_unpaged_చివర;
	phys_బైట్లు kernbase = (phys_బైట్లు) &kern_phys_పునాది,
	    kernsize = (phys_బైట్లు) &kern_కొలత;
#నిర్వచించు BUF 1024
	స్థిర అక్షరం cmdline[BUF];

	/* get our own copy of the multiboot info struct and module list */
	అమరిక_mbi(mbi, bootargs);

	/* Set various bits of info for the higher-level kernel. */
	cbi->mem_ఎగువ_phys = 0;
	cbi->user_sp = (vir_బైట్లు) &kern_vir_పునాది;
	cbi->vir_kern_ప్రారంభం = (vir_బైట్లు) &kern_vir_పునాది;
	cbi->bootstrap_ప్రారంభం = (vir_బైట్లు) &kern_unpaged_ప్రారంభం;
	cbi->bootstrap_len = (vir_బైట్లు) &kern_unpaged_చివర -
		cbi->bootstrap_ప్రారంభం;
	cbi->kmess = &kmess;

	/* set some configurable defaults */
	cbi->do_serial_debug = 1;
	cbi->serial_debug_baud = 115200;

	/* parse boot command line */
	ఒకవేళ (mbi->flags&MULTIBOOT_INFO_CMDLINE) {
		స్థిర అక్షరం var[BUF];
		స్థిర అక్షరం విలువ[BUF];

		/* Override values with cmdline argument */
		జ్ఞాపకం_నకలు(cmdline, (శూన్య *) mbi->cmdline, BUF);
		p = cmdline;
		అయితే (*p) {
			var_i = 0;
			విలువ_i = 0;
			అయితే (*p == ' ') p++; /* skip spaces */
			ఒకవేళ (!*p) ఆపు; /* is this the end? */
			అయితే (*p && *p != '=' && *p != ' ' && var_i < BUF - 1)
				var[var_i++] = *p++ ;
			var[var_i] = 0;
			ఒకవేళ (*p++ != '=') కొనసాగు; /* skip if not name=value */
			అయితే (*p && *p != ' ' && విలువ_i < BUF - 1) {
				విలువ[విలువ_i++] = *p++ ;
			}
			విలువ[విలువ_i] = 0;
			
			mb_సమితి_param(cbi->param_buf, var, విలువ, cbi);
		}
	}

	/* let higher levels know what we are booting on */
	mb_సమితి_param(cbi->param_buf, ARCHVARNAME, (అక్షరం *)get_board_arch_name(machine.board_id), cbi);
	mb_సమితి_param(cbi->param_buf, BOARDVARNAME,(అక్షరం *)get_board_name(machine.board_id) , cbi);
	

	/* move user stack/data down to leave a gap to catch kernel
	 * stack overflow; and to distinguish kernel and user addresses
	 * at a glance (0xf.. vs 0xe..) 
	 */
	cbi->user_sp = USR_STACKTOP;
	cbi->user_చివర = USR_DATATOP;

	/* kernel bytes without bootstrap code/data that is currently
	 * still needed but will be freed after bootstrapping.
	 */
	kinfo.kernel_allocated_బైట్లు = (phys_బైట్లు) &kern_కొలత;
	kinfo.kernel_allocated_బైట్లు -= cbi->bootstrap_len;

	ప్రతిజ్ఞా(!(cbi->bootstrap_ప్రారంభం % ARM_పుట_కొలత));
	cbi->bootstrap_len = rounddown(cbi->bootstrap_len, ARM_పుట_కొలత);
	ప్రతిజ్ఞా(mbi->flags & MULTIBOOT_INFO_MODS);
	ప్రతిజ్ఞా(mbi->mi_mods_లెక్క < MULTIBOOT_MAX_MODS);
	ప్రతిజ్ఞా(mbi->mi_mods_లెక్క > 0);
	జ్ఞాపకం_నకలు(&cbi->module_list, (శూన్య *) mbi->mods_addr,
		mbi->mi_mods_లెక్క * పరిమాణం(multiboot_module_t));
	
	memset(cbi->memmap, 0, పరిమాణం(cbi->memmap));
	/* mem_map has a variable layout */
	ఒకవేళ(mbi->flags & MULTIBOOT_INFO_MEM_MAP) {
		cbi->mmap_కొలత = 0;
	        కొరకు (mmap = (multiboot_జ్ఞాపకం_map_t *) mbi->mmap_addr;
       	     (గుర్తులేని దీర్ఘ) mmap < mbi->mmap_addr + mbi->mmap_నిడివి;
       	       mmap = (multiboot_జ్ఞాపకం_map_t *) 
		      	((గుర్తులేని దీర్ఘ) mmap + mmap->కొలత + పరిమాణం(mmap->కొలత))) {
			ఒకవేళ(mmap->type != MULTIBOOT_జ్ఞాపకం_అందుబాటు) కొనసాగు;
			కూడు_memmap(cbi, mmap->mm_పునాది_addr, mmap->mm_నిడివి);
		}
	} లేకపోతే {
		ప్రతిజ్ఞా(mbi->flags & MULTIBOOT_INFO_జ్ఞాపకం);
		కూడు_memmap(cbi, 0, mbi->mem_క్రింది_వాడని*1024);
		కూడు_memmap(cbi, 0x100000, mbi->mem_పై_వాడని*1024);
	}

	/* Sanity check: the kernel nor any of the modules may overlap
	 * with each other. Pretend the kernel is an extra module for a
	 * second.
	 */
	k = mbi->mi_mods_లెక్క;
	ప్రతిజ్ఞా(k < MULTIBOOT_MAX_MODS);
	cbi->module_list[k].mod_ప్రారంభం = kernbase;
	cbi->module_list[k].mod_చివర = kernbase + kernsize;
	cbi->mods_with_kernel = mbi->mi_mods_లెక్క+1;
	cbi->kern_mod = k;

	కొరకు(m = 0; m < cbi->mods_with_kernel; m++) {
#ఒకవేళ 0
		రాయి("checking overlap of module %08lx-%08lx\n",
		  cbi->module_list[m].mod_ప్రారంభం, cbi->module_list[m].mod_చివర);
#అయితే_ముగింపు
		ఒకవేళ(overlaps(cbi->module_list, cbi->mods_with_kernel, m))
			panic("overlapping boot modules/kernel");
		/* We cut out the bits of memory that we know are
		 * occupied by the kernel and boot modules.
		 */
		cut_memmap(cbi,
			cbi->module_list[m].mod_ప్రారంభం,
			cbi->module_list[m].mod_చివర);
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
#నిర్వచించు POORMANS_FAILURE_NOTIFICATION  asm అస్థిర("svc #00\n")

/* use the passed cmdline argument to determine the machine id */
శూన్య సమితి_machine_id(అక్షరం *cmdline)
{

	అక్షరం boardname[20];
	memset(boardname,'\0',20);
	ఒకవేళ (కనుగొను_విలువ(cmdline,"board_name=",boardname,20)){
		/* we expect the bootloader to pass a board_name as argument
		 * this however did not happen and given we still are in early
		 * boot we can't use the serial. We therefore generate an interrupt
		 * and hope the bootloader will do something nice with it */
		POORMANS_FAILURE_NOTIFICATION;
	}  
	machine.board_id = get_board_id_by_short_name(boardname);

	ఒకవేళ (machine.board_id ==0){
		/* same thing as above there is no safe escape */
		POORMANS_FAILURE_NOTIFICATION;
	}
}

kinfo_t *pre_init(పూర్ణాంకం argc, అక్షరం **argv)
{
	అక్షరం *bootargs;
	/* This is the main "c" entry point into the kernel. It gets called
	   from head.S */
	   
	/* Clear BSS */
	memset(&_edata, 0, (u32_t)&చివర_2 - (u32_t)&_edata);
        memset(&_kern_unpaged_edata, 0, (u32_t)&kern_unpaged_చివర - (u32_t)&_kern_unpaged_edata);

	/* we get called in a c like fashion where the first arg
         * is the program name (load address) and the rest are
	 * arguments. by convention the second argument is the
	 *  command line */
	ఒకవేళ (argc != 2) {
		POORMANS_FAILURE_NOTIFICATION;
	}

	bootargs = argv[1];
	సమితి_machine_id(bootargs);
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
	pg_తటస్థం(&kinfo);
	kinfo.freepde_ప్రారంభం = pg_mapkernel();
	pg_ఎక్కించు();
	vm_enable_paging();

	/* Done, return boot info so it can be passed to kmain(). */
	తిరిగి &kinfo;
}

/* pre_init gets executed at the memory location where the kernel was loaded by the boot loader.
 * at that stage we only have a minimum set of functionality present (all symbols gets renamed to
 * ensure this). The following methods are used in that context. Once we jump to kmain they are no
 * longer used and the "real" implementations are visible
 */
శూన్య send_diag_sig(శూన్య) { }
శూన్య minix_shutdown(పూర్ణాంకం how) { arch_shutdown(how); }
శూన్య busy_delay_ms(పూర్ణాంకం x) { }
పూర్ణాంకం లేపు(పూర్ణాంకం n) { panic("raise(%d)\n", n); }
పూర్ణాంకం kern_phys_map_ptr( phys_బైట్లు పునాది_చిరునామా, vir_బైట్లు io_కొలత, పూర్ణాంకం vm_flags,
నిర్మాణం kern_phys_map * priv, vir_బైట్లు ptr) { తిరిగి -1; };
నిర్మాణం machine machine; /* pre init stage machine */
