
#تعریف_کړه UNPAGED 1	/* for proper kmain() prototype */

#ضمن <assert.h>
#ضمن <stdlib.h>
#ضمن <minix/minlib.h>
#ضمن <minix/board.h>
#ضمن <sys/reboot.h>
#ضمن <machine/partition.h>
#ضمن "string.h"
#ضمن "direct_utils.h"
#ضمن "serial.h"
#ضمن "glo.h"

#إذا USE_SYSDEBUG
#تعریف_کړه MULTIBOOT_VERBOSE 1
#د_که_پای

/* to-be-built kinfo struct, diagnostics buffer */
kinfo_t kinfo;
بنية kmessages kmessages;

/* pg_utils.c uses this; in this phase, there is a 1:1 mapping. */
phys_بایټونه vir2phys(فراغ *addr) { أرجع (phys_بایټونه) addr; } 

/* mb_utils.c uses this; we can reach it directly */
حرف *video_mem = (حرف *) MULTIBOOT_VIDEO_بفر;

/* String length used for mb_itoa */
#تعریف_کړه ITOA_بفر_اندازه 20

/* Kernel may use memory */
صحيح kernel_may_alloc = 1;

ساكن صحيح mb_سېټ_param(حرف *bigbuf, حرف *name, حرف *ارزښت, kinfo_t *cbi) 
{
	حرف *p = bigbuf;
	حرف *bufend = bigbuf + MULTIBOOT_PARAM_BUF_اندازه;
	حرف *q;
	صحيح namelen = مزي_اوږدوالی(name);
	صحيح valuelen = مزي_اوږدوالی(ارزښت);

	/* Some variables we recognize */
	إذا(!strcmp(name, SERVARNAME)) { cbi->do_serial_debug = 1; }
	إذا(!strcmp(name, SERBAUDVARNAME)) { cbi->serial_debug_baud = atoi(ارزښت); }

	/* Delete the item if already exists */
	طالما (*p) {
		إذا (مزي_پرتله(p, name, namelen) == 0 && p[namelen] == '=') {
			q = p;
			طالما (*q) q++;
			لكل (q++; q < bufend; q++, p++)
				*p = *q;
			اكسر;
		}
		طالما (*p++)
			;
		p++;
	}
	
	لكل (p = bigbuf; p < bufend && (*p || *(p + 1)); p++)
		;
	إذا (p > bigbuf) p++;
	
	/* Make sure there's enough space for the new parameter */
	إذا (p + namelen + valuelen + 3 > bufend)
		أرجع -1;
	
	strcpy(p, name);
	p[namelen] = '=';
	strcpy(p + namelen + 1, ارزښت);
	p[namelen + valuelen + 1] = 0;
	p[namelen + valuelen + 2] = 0;
	أرجع 0;
}

صحيح overlaps(multiboot_module_t *mod, صحيح n, صحيح cmp_mod)
{
	multiboot_module_t *cmp = &mod[cmp_mod];
	صحيح m;

#تعریف_کړه INRANGE(mod, v) ((v) >= mod->mod_پیل && (v) < mod->mod_پای_ټکی)
#تعریف_کړه OVERLAP(mod1, mod2) (INRANGE(mod1, mod2->mod_پیل) || \
			INRANGE(mod1, mod2->mod_پای_ټکی-1))
	لكل(m = 0; m < n; m++) {
		multiboot_module_t *thismod = &mod[m];
		إذا(m == cmp_mod) استمر;
		إذا(OVERLAP(thismod, cmp))
			أرجع 1;
	}
	أرجع 0;
}

فراغ get_parameters(u32_t ebx, kinfo_t *cbi) 
{
	multiboot_حافظه_map_t *mmap;
	multiboot_info_t *mbi = &cbi->mbi;
	صحيح var_i,ارزښت_i, m, k;
	حرف *p;
	خارجي حرف kern_phys_بنسټ, kern_vir_بنسټ, kern_اندازه,
		kern_unpaged_پیل, kern_unpaged_پای_ټکی;
	phys_بایټونه kernbase = (phys_بایټونه) &kern_phys_بنسټ,
		kernsize = (phys_بایټونه) &kern_اندازه;
#تعریف_کړه BUF 1024
	ساكن حرف cmdline[BUF];

	/* get our own copy of the multiboot info struct and module list */
	حافظه_نقل((فراغ *) mbi, (فراغ *) ebx, حجم(*mbi));

	/* Set various bits of info for the higher-level kernel. */
	cbi->mem_لوړ_phys = 0;
	cbi->user_sp = (vir_بایټونه) &kern_vir_بنسټ;
	cbi->vir_kern_پیل = (vir_بایټونه) &kern_vir_بنسټ;
	cbi->bootstrap_پیل = (vir_بایټونه) &kern_unpaged_پیل;
	cbi->bootstrap_len = (vir_بایټونه) &kern_unpaged_پای_ټکی -
		cbi->bootstrap_پیل;
	cbi->kmess = &kmess;

	/* set some configurable defaults */
	cbi->do_serial_debug = 0;
	cbi->serial_debug_baud = 115200;

	/* parse boot command line */
	إذا (mbi->mi_flags & MULTIBOOT_INFO_HAS_CMDLINE) {
		ساكن حرف var[BUF];
		ساكن حرف ارزښت[BUF];

		/* Override values with cmdline argument */
		حافظه_نقل(cmdline, (فراغ *) mbi->mi_cmdline, BUF);
		p = cmdline;
		طالما (*p) {
			var_i = 0;
			ارزښت_i = 0;
			طالما (*p == ' ') p++;
			إذا (!*p) اكسر;
			طالما (*p && *p != '=' && *p != ' ' && var_i < BUF - 1) 
				var[var_i++] = *p++ ;
			var[var_i] = 0;
			إذا (*p++ != '=') استمر; /* skip if not name=value */
			طالما (*p && *p != ' ' && ارزښت_i < BUF - 1) 
				ارزښت[ارزښت_i++] = *p++ ;
			ارزښت[ارزښت_i] = 0;
			
			mb_سېټ_param(cbi->param_buf, var, ارزښت, cbi);
		}
	}

        /* let higher levels know what we are booting on */
        mb_سېټ_param(cbi->param_buf, ARCHVARNAME, (حرف *)get_board_arch_name(BOARD_ID_INTEL), cbi);
	mb_سېټ_param(cbi->param_buf, BOARDVARNAME,(حرف *)get_board_name(BOARD_ID_INTEL) , cbi);

	/* move user stack/data down to leave a gap to catch kernel
	 * stack overflow; and to distinguish kernel and user addresses
	 * at a glance (0xf.. vs 0xe..) 
	 */
	cbi->user_sp = USR_STACKTOP;
	cbi->user_پای_ټکی = USR_DATATOP;

	/* kernel bytes without bootstrap code/data that is currently
	 * still needed but will be freed after bootstrapping.
	 */
	kinfo.kernel_allocated_بایټونه = (phys_بایټونه) &kern_اندازه;
	kinfo.kernel_allocated_بایټونه -= cbi->bootstrap_len;

	أكد(!(cbi->bootstrap_پیل % I386_پاڼه_اندازه));
	cbi->bootstrap_len = rounddown(cbi->bootstrap_len, I386_پاڼه_اندازه);
	أكد(mbi->mi_flags & MULTIBOOT_INFO_HAS_MODS);
	أكد(mbi->mi_mods_شمېر < MULTIBOOT_MAX_MODS);
	أكد(mbi->mi_mods_شمېر > 0);
	حافظه_نقل(&cbi->module_list, (فراغ *) mbi->mi_mods_addr,
		mbi->mi_mods_شمېر * حجم(multiboot_module_t));
	
	memset(cbi->memmap, 0, حجم(cbi->memmap));
	/* mem_map has a variable layout */
	إذا(mbi->mi_flags & MULTIBOOT_INFO_HAS_MMAP) {
		cbi->mmap_اندازه = 0;
	        لكل (mmap = (multiboot_حافظه_map_t *) mbi->mmap_addr;
       	     (بینښه اوږد) mmap < mbi->mmap_addr + mbi->mmap_اوږدتیا;
       	       mmap = (multiboot_حافظه_map_t *) 
		      	((بینښه اوږد) mmap + mmap->mm_اندازه + حجم(mmap->mm_اندازه))) {
			إذا(mmap->mm_type != MULTIBOOT_حافظه_شته) استمر;
			جمع_کړه_memmap(cbi, mmap->mm_بنسټ_addr, mmap->mm_اوږدتیا);
		}
	} وإلا {
		أكد(mbi->mi_flags & MULTIBOOT_INFO_HAS_حافظه);
		جمع_کړه_memmap(cbi, 0, mbi->mi_mem_ښکتنی*1024);
		جمع_کړه_memmap(cbi, 0x100000, mbi->mi_mem_پاسنی*1024);
	}

	/* Sanity check: the kernel nor any of the modules may overlap
	 * with each other. Pretend the kernel is an extra module for a
	 * second.
	 */
	k = mbi->mi_mods_شمېر;
	أكد(k < MULTIBOOT_MAX_MODS);
	cbi->module_list[k].mod_پیل = kernbase;
	cbi->module_list[k].mod_پای_ټکی = kernbase + kernsize;
	cbi->mods_with_kernel = mbi->mi_mods_شمېر+1;
	cbi->kern_mod = k;

	لكل(m = 0; m < cbi->mods_with_kernel; m++) {
#إذا 0
		چاپ_کړه("checking overlap of module %08lx-%08lx\n",
		  cbi->module_list[m].mod_پیل, cbi->module_list[m].mod_پای_ټکی);
#د_که_پای
		إذا(overlaps(cbi->module_list, cbi->mods_with_kernel, m))
			panic("overlapping boot modules/kernel");
		/* We cut out the bits of memory that we know are
		 * occupied by the kernel and boot modules.
		 */
		cut_memmap(cbi,
			cbi->module_list[m].mod_پیل, 
			cbi->module_list[m].mod_پای_ټکی);
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
	pg_بې_اغېزه_عنصر(&kinfo);
	kinfo.freepde_پیل = pg_mapkernel();
	pg_بارول();
	vm_enable_paging();

	/* Done, return boot info so it can be passed to kmain(). */
	أرجع &kinfo;
}

فراغ send_diag_sig(فراغ) { }
فراغ minix_shutdown(صحيح how) { arch_shutdown(how); }
فراغ busy_delay_ms(صحيح x) { }
صحيح ارفع(صحيح sig) { panic("raise(%d)\n", sig); }
