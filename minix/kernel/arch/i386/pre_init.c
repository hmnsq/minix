
#تعریف_کن UNPAGED 1	/* for proper kmain() prototype */

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
#تعریف_کن MULTIBOOT_VERBOSE 1
#پایان_اگر

/* to-be-built kinfo struct, diagnostics buffer */
kinfo_t kinfo;
بنية kmessages kmessages;

/* pg_utils.c uses this; in this phase, there is a 1:1 mapping. */
phys_بایتها vir2phys(فراغ *addr) { أرجع (phys_بایتها) addr; } 

/* mb_utils.c uses this; we can reach it directly */
حرف *video_mem = (حرف *) MULTIBOOT_VIDEO_میانگیر;

/* String length used for mb_itoa */
#تعریف_کن ITOA_میانگیر_اندازه 20

/* Kernel may use memory */
صحيح kernel_may_alloc = 1;

ساكن صحيح mb_مجموعه_param(حرف *bigbuf, حرف *name, حرف *مقدار, kinfo_t *cbi) 
{
	حرف *p = bigbuf;
	حرف *bufend = bigbuf + MULTIBOOT_PARAM_BUF_اندازه;
	حرف *q;
	صحيح namelen = طول_رشته(name);
	صحيح valuelen = طول_رشته(مقدار);

	/* Some variables we recognize */
	إذا(!strcmp(name, SERVARNAME)) { cbi->do_serial_debug = 1; }
	إذا(!strcmp(name, SERBAUDVARNAME)) { cbi->serial_debug_baud = atoi(مقدار); }

	/* Delete the item if already exists */
	طالما (*p) {
		إذا (مقایسه_رشته(p, name, namelen) == 0 && p[namelen] == '=') {
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
	strcpy(p + namelen + 1, مقدار);
	p[namelen + valuelen + 1] = 0;
	p[namelen + valuelen + 2] = 0;
	أرجع 0;
}

صحيح overlaps(multiboot_module_t *mod, صحيح n, صحيح cmp_mod)
{
	multiboot_module_t *cmp = &mod[cmp_mod];
	صحيح m;

#تعریف_کن INRANGE(mod, v) ((v) >= mod->mod_شروع && (v) < mod->mod_انتها)
#تعریف_کن OVERLAP(mod1, mod2) (INRANGE(mod1, mod2->mod_شروع) || \
			INRANGE(mod1, mod2->mod_انتها-1))
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
	صحيح var_i,مقدار_i, m, k;
	حرف *p;
	خارجي حرف kern_phys_پایه, kern_vir_پایه, kern_اندازه,
		kern_unpaged_شروع, kern_unpaged_انتها;
	phys_بایتها kernbase = (phys_بایتها) &kern_phys_پایه,
		kernsize = (phys_بایتها) &kern_اندازه;
#تعریف_کن BUF 1024
	ساكن حرف cmdline[BUF];

	/* get our own copy of the multiboot info struct and module list */
	کپی_حافظه((فراغ *) mbi, (فراغ *) ebx, حجم(*mbi));

	/* Set various bits of info for the higher-level kernel. */
	cbi->mem_بالا_phys = 0;
	cbi->user_sp = (vir_بایتها) &kern_vir_پایه;
	cbi->vir_kern_شروع = (vir_بایتها) &kern_vir_پایه;
	cbi->bootstrap_شروع = (vir_بایتها) &kern_unpaged_شروع;
	cbi->bootstrap_len = (vir_بایتها) &kern_unpaged_انتها -
		cbi->bootstrap_شروع;
	cbi->kmess = &kmess;

	/* set some configurable defaults */
	cbi->do_serial_debug = 0;
	cbi->serial_debug_baud = 115200;

	/* parse boot command line */
	إذا (mbi->mi_flags & MULTIBOOT_INFO_HAS_CMDLINE) {
		ساكن حرف var[BUF];
		ساكن حرف مقدار[BUF];

		/* Override values with cmdline argument */
		کپی_حافظه(cmdline, (فراغ *) mbi->mi_cmdline, BUF);
		p = cmdline;
		طالما (*p) {
			var_i = 0;
			مقدار_i = 0;
			طالما (*p == ' ') p++;
			إذا (!*p) اكسر;
			طالما (*p && *p != '=' && *p != ' ' && var_i < BUF - 1) 
				var[var_i++] = *p++ ;
			var[var_i] = 0;
			إذا (*p++ != '=') استمر; /* skip if not name=value */
			طالما (*p && *p != ' ' && مقدار_i < BUF - 1) 
				مقدار[مقدار_i++] = *p++ ;
			مقدار[مقدار_i] = 0;
			
			mb_مجموعه_param(cbi->param_buf, var, مقدار, cbi);
		}
	}

        /* let higher levels know what we are booting on */
        mb_مجموعه_param(cbi->param_buf, ARCHVARNAME, (حرف *)get_board_arch_name(BOARD_ID_INTEL), cbi);
	mb_مجموعه_param(cbi->param_buf, BOARDVARNAME,(حرف *)get_board_name(BOARD_ID_INTEL) , cbi);

	/* move user stack/data down to leave a gap to catch kernel
	 * stack overflow; and to distinguish kernel and user addresses
	 * at a glance (0xf.. vs 0xe..) 
	 */
	cbi->user_sp = USR_STACKTOP;
	cbi->user_انتها = USR_DATATOP;

	/* kernel bytes without bootstrap code/data that is currently
	 * still needed but will be freed after bootstrapping.
	 */
	kinfo.kernel_allocated_بایتها = (phys_بایتها) &kern_اندازه;
	kinfo.kernel_allocated_بایتها -= cbi->bootstrap_len;

	أكد(!(cbi->bootstrap_شروع % I386_صفحه_اندازه));
	cbi->bootstrap_len = rounddown(cbi->bootstrap_len, I386_صفحه_اندازه);
	أكد(mbi->mi_flags & MULTIBOOT_INFO_HAS_MODS);
	أكد(mbi->mi_mods_شمار < MULTIBOOT_MAX_MODS);
	أكد(mbi->mi_mods_شمار > 0);
	کپی_حافظه(&cbi->module_list, (فراغ *) mbi->mi_mods_addr,
		mbi->mi_mods_شمار * حجم(multiboot_module_t));
	
	memset(cbi->memmap, 0, حجم(cbi->memmap));
	/* mem_map has a variable layout */
	إذا(mbi->mi_flags & MULTIBOOT_INFO_HAS_MMAP) {
		cbi->mmap_اندازه = 0;
	        لكل (mmap = (multiboot_حافظه_map_t *) mbi->mmap_addr;
       	     (بیعلامت دراز) mmap < mbi->mmap_addr + mbi->mmap_درازی;
       	       mmap = (multiboot_حافظه_map_t *) 
		      	((بیعلامت دراز) mmap + mmap->mm_اندازه + حجم(mmap->mm_اندازه))) {
			إذا(mmap->mm_type != MULTIBOOT_حافظه_موجود) استمر;
			جمع_کن_memmap(cbi, mmap->mm_پایه_addr, mmap->mm_درازی);
		}
	} وإلا {
		أكد(mbi->mi_flags & MULTIBOOT_INFO_HAS_حافظه);
		جمع_کن_memmap(cbi, 0, mbi->mi_mem_پایینی*1024);
		جمع_کن_memmap(cbi, 0x100000, mbi->mi_mem_فوقانی*1024);
	}

	/* Sanity check: the kernel nor any of the modules may overlap
	 * with each other. Pretend the kernel is an extra module for a
	 * second.
	 */
	k = mbi->mi_mods_شمار;
	أكد(k < MULTIBOOT_MAX_MODS);
	cbi->module_list[k].mod_شروع = kernbase;
	cbi->module_list[k].mod_انتها = kernbase + kernsize;
	cbi->mods_with_kernel = mbi->mi_mods_شمار+1;
	cbi->kern_mod = k;

	لكل(m = 0; m < cbi->mods_with_kernel; m++) {
#إذا 0
		چاپ_کن("checking overlap of module %08lx-%08lx\n",
		  cbi->module_list[m].mod_شروع, cbi->module_list[m].mod_انتها);
#پایان_اگر
		إذا(overlaps(cbi->module_list, cbi->mods_with_kernel, m))
			panic("overlapping boot modules/kernel");
		/* We cut out the bits of memory that we know are
		 * occupied by the kernel and boot modules.
		 */
		cut_memmap(cbi,
			cbi->module_list[m].mod_شروع, 
			cbi->module_list[m].mod_انتها);
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
	pg_عنصر_خنثی(&kinfo);
	kinfo.freepde_شروع = pg_mapkernel();
	pg_بارگیری();
	vm_enable_paging();

	/* Done, return boot info so it can be passed to kmain(). */
	أرجع &kinfo;
}

فراغ send_diag_sig(فراغ) { }
فراغ minix_shutdown(صحيح how) { arch_shutdown(how); }
فراغ busy_delay_ms(صحيح x) { }
صحيح ارفع(صحيح sig) { panic("raise(%d)\n", sig); }
