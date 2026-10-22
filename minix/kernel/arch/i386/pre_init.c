
#నిర్వచించు UNPAGED 1	/* for proper kmain() prototype */

#అన్తర్భావ <assert.h>
#అన్తర్భావ <stdlib.h>
#అన్తర్భావ <minix/minlib.h>
#అన్తర్భావ <minix/board.h>
#అన్తర్భావ <sys/reboot.h>
#అన్తర్భావ <machine/partition.h>
#అన్తర్భావ "string.h"
#అన్తర్భావ "direct_utils.h"
#అన్తర్భావ "serial.h"
#అన్తర్భావ "glo.h"

#ఒకవేళ USE_SYSDEBUG
#నిర్వచించు MULTIBOOT_VERBOSE 1
#అయితే_ముగింపు

/* to-be-built kinfo struct, diagnostics buffer */
kinfo_t kinfo;
నిర్మాణం kmessages kmessages;

/* pg_utils.c uses this; in this phase, there is a 1:1 mapping. */
phys_బైట్లు vir2phys(శూన్య *addr) { తిరిగి (phys_బైట్లు) addr; } 

/* mb_utils.c uses this; we can reach it directly */
అక్షరం *video_mem = (అక్షరం *) MULTIBOOT_VIDEO_బఫరు;

/* String length used for mb_itoa */
#నిర్వచించు ITOA_బఫరు_కొలత 20

/* Kernel may use memory */
పూర్ణాంకం kernel_may_alloc = 1;

స్థిర పూర్ణాంకం mb_సమితి_param(అక్షరం *bigbuf, అక్షరం *name, అక్షరం *విలువ, kinfo_t *cbi) 
{
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
			అయితే (*q) q++;
			కొరకు (q++; q < bufend; q++, p++)
				*p = *q;
			ఆపు;
		}
		అయితే (*p++)
			;
		p++;
	}
	
	కొరకు (p = bigbuf; p < bufend && (*p || *(p + 1)); p++)
		;
	ఒకవేళ (p > bigbuf) p++;
	
	/* Make sure there's enough space for the new parameter */
	ఒకవేళ (p + namelen + valuelen + 3 > bufend)
		తిరిగి -1;
	
	strcpy(p, name);
	p[namelen] = '=';
	strcpy(p + namelen + 1, విలువ);
	p[namelen + valuelen + 1] = 0;
	p[namelen + valuelen + 2] = 0;
	తిరిగి 0;
}

పూర్ణాంకం overlaps(multiboot_module_t *mod, పూర్ణాంకం n, పూర్ణాంకం cmp_mod)
{
	multiboot_module_t *cmp = &mod[cmp_mod];
	పూర్ణాంకం m;

#నిర్వచించు INRANGE(mod, v) ((v) >= mod->mod_ప్రారంభం && (v) < mod->mod_చివర)
#నిర్వచించు OVERLAP(mod1, mod2) (INRANGE(mod1, mod2->mod_ప్రారంభం) || \
			INRANGE(mod1, mod2->mod_చివర-1))
	కొరకు(m = 0; m < n; m++) {
		multiboot_module_t *thismod = &mod[m];
		ఒకవేళ(m == cmp_mod) కొనసాగు;
		ఒకవేళ(OVERLAP(thismod, cmp))
			తిరిగి 1;
	}
	తిరిగి 0;
}

శూన్య get_parameters(u32_t ebx, kinfo_t *cbi) 
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
	జ్ఞాపకం_నకలు((శూన్య *) mbi, (శూన్య *) ebx, పరిమాణం(*mbi));

	/* Set various bits of info for the higher-level kernel. */
	cbi->mem_ఎగువ_phys = 0;
	cbi->user_sp = (vir_బైట్లు) &kern_vir_పునాది;
	cbi->vir_kern_ప్రారంభం = (vir_బైట్లు) &kern_vir_పునాది;
	cbi->bootstrap_ప్రారంభం = (vir_బైట్లు) &kern_unpaged_ప్రారంభం;
	cbi->bootstrap_len = (vir_బైట్లు) &kern_unpaged_చివర -
		cbi->bootstrap_ప్రారంభం;
	cbi->kmess = &kmess;

	/* set some configurable defaults */
	cbi->do_serial_debug = 0;
	cbi->serial_debug_baud = 115200;

	/* parse boot command line */
	ఒకవేళ (mbi->mi_flags & MULTIBOOT_INFO_HAS_CMDLINE) {
		స్థిర అక్షరం var[BUF];
		స్థిర అక్షరం విలువ[BUF];

		/* Override values with cmdline argument */
		జ్ఞాపకం_నకలు(cmdline, (శూన్య *) mbi->mi_cmdline, BUF);
		p = cmdline;
		అయితే (*p) {
			var_i = 0;
			విలువ_i = 0;
			అయితే (*p == ' ') p++;
			ఒకవేళ (!*p) ఆపు;
			అయితే (*p && *p != '=' && *p != ' ' && var_i < BUF - 1) 
				var[var_i++] = *p++ ;
			var[var_i] = 0;
			ఒకవేళ (*p++ != '=') కొనసాగు; /* skip if not name=value */
			అయితే (*p && *p != ' ' && విలువ_i < BUF - 1) 
				విలువ[విలువ_i++] = *p++ ;
			విలువ[విలువ_i] = 0;
			
			mb_సమితి_param(cbi->param_buf, var, విలువ, cbi);
		}
	}

        /* let higher levels know what we are booting on */
        mb_సమితి_param(cbi->param_buf, ARCHVARNAME, (అక్షరం *)get_board_arch_name(BOARD_ID_INTEL), cbi);
	mb_సమితి_param(cbi->param_buf, BOARDVARNAME,(అక్షరం *)get_board_name(BOARD_ID_INTEL) , cbi);

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

	ప్రతిజ్ఞా(!(cbi->bootstrap_ప్రారంభం % I386_పుట_కొలత));
	cbi->bootstrap_len = rounddown(cbi->bootstrap_len, I386_పుట_కొలత);
	ప్రతిజ్ఞా(mbi->mi_flags & MULTIBOOT_INFO_HAS_MODS);
	ప్రతిజ్ఞా(mbi->mi_mods_లెక్క < MULTIBOOT_MAX_MODS);
	ప్రతిజ్ఞా(mbi->mi_mods_లెక్క > 0);
	జ్ఞాపకం_నకలు(&cbi->module_list, (శూన్య *) mbi->mi_mods_addr,
		mbi->mi_mods_లెక్క * పరిమాణం(multiboot_module_t));
	
	memset(cbi->memmap, 0, పరిమాణం(cbi->memmap));
	/* mem_map has a variable layout */
	ఒకవేళ(mbi->mi_flags & MULTIBOOT_INFO_HAS_MMAP) {
		cbi->mmap_కొలత = 0;
	        కొరకు (mmap = (multiboot_జ్ఞాపకం_map_t *) mbi->mmap_addr;
       	     (గుర్తులేని దీర్ఘ) mmap < mbi->mmap_addr + mbi->mmap_నిడివి;
       	       mmap = (multiboot_జ్ఞాపకం_map_t *) 
		      	((గుర్తులేని దీర్ఘ) mmap + mmap->mm_కొలత + పరిమాణం(mmap->mm_కొలత))) {
			ఒకవేళ(mmap->mm_type != MULTIBOOT_జ్ఞాపకం_అందుబాటు) కొనసాగు;
			కూడు_memmap(cbi, mmap->mm_పునాది_addr, mmap->mm_నిడివి);
		}
	} లేకపోతే {
		ప్రతిజ్ఞా(mbi->mi_flags & MULTIBOOT_INFO_HAS_జ్ఞాపకం);
		కూడు_memmap(cbi, 0, mbi->mi_mem_క్రింది*1024);
		కూడు_memmap(cbi, 0x100000, mbi->mi_mem_పై*1024);
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

kinfo_t *pre_init(u32_t magic, u32_t ebx)
{
	ప్రతిజ్ఞా(magic == MULTIBOOT_INFO_MAGIC);

	/* Get our own copy boot params pointed to by ebx.
	 * Here we find out whether we should do serial output.
	 */
	get_parameters(ebx, &kinfo);

	/* Make and load a pagetable that will map the kernel
	 * to where it should be; but first a 1:1 mapping so
	 * this code stays where it should be.
	 */
	pg_clear();
	pg_తటస్థం(&kinfo);
	kinfo.freepde_ప్రారంభం = pg_mapkernel();
	pg_ఎక్కించు();
	vm_enable_paging();

	/* Done, return boot info so it can be passed to kmain(). */
	తిరిగి &kinfo;
}

శూన్య send_diag_sig(శూన్య) { }
శూన్య minix_shutdown(పూర్ణాంకం how) { arch_shutdown(how); }
శూన్య busy_delay_ms(పూర్ణాంకం x) { }
పూర్ణాంకం లేపు(పూర్ణాంకం sig) { panic("raise(%d)\n", sig); }
