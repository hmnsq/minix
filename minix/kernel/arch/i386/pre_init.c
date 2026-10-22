
#تعریف_کرو UNPAGED 1	/* for proper kmain() prototype */

#شامل <assert.h>
#شامل <stdlib.h>
#شامل <minix/minlib.h>
#شامل <minix/board.h>
#شامل <sys/reboot.h>
#شامل <machine/partition.h>
#شامل "string.h"
#شامل "direct_utils.h"
#شامل "serial.h"
#شامل "glo.h"

#اگر USE_SYSDEBUG
#تعریف_کرو MULTIBOOT_VERBOSE 1
#اگر_ختم

/* to-be-built kinfo struct, diagnostics buffer */
kinfo_t kinfo;
ساخت kmessages kmessages;

/* pg_utils.c uses this; in this phase, there is a 1:1 mapping. */
phys_بائٹیں vir2phys(خالی *addr) { واپس (phys_بائٹیں) addr; } 

/* mb_utils.c uses this; we can reach it directly */
حرف *video_mem = (حرف *) MULTIBOOT_VIDEO_بفر;

/* String length used for mb_itoa */
#تعریف_کرو ITOA_بفر_سائز 20

/* Kernel may use memory */
صحیح kernel_may_alloc = 1;

ساکن صحیح mb_سیٹ_param(حرف *bigbuf, حرف *name, حرف *قدر, kinfo_t *cbi) 
{
	حرف *p = bigbuf;
	حرف *bufend = bigbuf + MULTIBOOT_PARAM_BUF_سائز;
	حرف *q;
	صحیح namelen = لڑی_لمبائی(name);
	صحیح valuelen = لڑی_لمبائی(قدر);

	/* Some variables we recognize */
	اگر(!strcmp(name, SERVARNAME)) { cbi->do_serial_debug = 1; }
	اگر(!strcmp(name, SERBAUDVARNAME)) { cbi->serial_debug_baud = atoi(قدر); }

	/* Delete the item if already exists */
	جب (*p) {
		اگر (لڑی_موازنہ(p, name, namelen) == 0 && p[namelen] == '=') {
			q = p;
			جب (*q) q++;
			برائے (q++; q < bufend; q++, p++)
				*p = *q;
			توڑو;
		}
		جب (*p++)
			;
		p++;
	}
	
	برائے (p = bigbuf; p < bufend && (*p || *(p + 1)); p++)
		;
	اگر (p > bigbuf) p++;
	
	/* Make sure there's enough space for the new parameter */
	اگر (p + namelen + valuelen + 3 > bufend)
		واپس -1;
	
	strcpy(p, name);
	p[namelen] = '=';
	strcpy(p + namelen + 1, قدر);
	p[namelen + valuelen + 1] = 0;
	p[namelen + valuelen + 2] = 0;
	واپس 0;
}

صحیح overlaps(multiboot_module_t *mod, صحیح n, صحیح cmp_mod)
{
	multiboot_module_t *cmp = &mod[cmp_mod];
	صحیح m;

#تعریف_کرو INRANGE(mod, v) ((v) >= mod->mod_آغاز && (v) < mod->mod_انتہا)
#تعریف_کرو OVERLAP(mod1, mod2) (INRANGE(mod1, mod2->mod_آغاز) || \
			INRANGE(mod1, mod2->mod_انتہا-1))
	برائے(m = 0; m < n; m++) {
		multiboot_module_t *thismod = &mod[m];
		اگر(m == cmp_mod) جاری;
		اگر(OVERLAP(thismod, cmp))
			واپس 1;
	}
	واپس 0;
}

خالی get_parameters(u32_t ebx, kinfo_t *cbi) 
{
	multiboot_حافظہ_map_t *mmap;
	multiboot_info_t *mbi = &cbi->mbi;
	صحیح var_i,قدر_i, m, k;
	حرف *p;
	خارجی حرف kern_phys_بنیاد, kern_vir_بنیاد, kern_سائز,
		kern_unpaged_آغاز, kern_unpaged_انتہا;
	phys_بائٹیں kernbase = (phys_بائٹیں) &kern_phys_بنیاد,
		kernsize = (phys_بائٹیں) &kern_سائز;
#تعریف_کرو BUF 1024
	ساکن حرف cmdline[BUF];

	/* get our own copy of the multiboot info struct and module list */
	حافظہ_نقل((خالی *) mbi, (خالی *) ebx, حجم(*mbi));

	/* Set various bits of info for the higher-level kernel. */
	cbi->mem_بلند_phys = 0;
	cbi->user_sp = (vir_بائٹیں) &kern_vir_بنیاد;
	cbi->vir_kern_آغاز = (vir_بائٹیں) &kern_vir_بنیاد;
	cbi->bootstrap_آغاز = (vir_بائٹیں) &kern_unpaged_آغاز;
	cbi->bootstrap_len = (vir_بائٹیں) &kern_unpaged_انتہا -
		cbi->bootstrap_آغاز;
	cbi->kmess = &kmess;

	/* set some configurable defaults */
	cbi->do_serial_debug = 0;
	cbi->serial_debug_baud = 115200;

	/* parse boot command line */
	اگر (mbi->mi_flags & MULTIBOOT_INFO_HAS_CMDLINE) {
		ساکن حرف var[BUF];
		ساکن حرف قدر[BUF];

		/* Override values with cmdline argument */
		حافظہ_نقل(cmdline, (خالی *) mbi->mi_cmdline, BUF);
		p = cmdline;
		جب (*p) {
			var_i = 0;
			قدر_i = 0;
			جب (*p == ' ') p++;
			اگر (!*p) توڑو;
			جب (*p && *p != '=' && *p != ' ' && var_i < BUF - 1) 
				var[var_i++] = *p++ ;
			var[var_i] = 0;
			اگر (*p++ != '=') جاری; /* skip if not name=value */
			جب (*p && *p != ' ' && قدر_i < BUF - 1) 
				قدر[قدر_i++] = *p++ ;
			قدر[قدر_i] = 0;
			
			mb_سیٹ_param(cbi->param_buf, var, قدر, cbi);
		}
	}

        /* let higher levels know what we are booting on */
        mb_سیٹ_param(cbi->param_buf, ARCHVARNAME, (حرف *)get_board_arch_name(BOARD_ID_INTEL), cbi);
	mb_سیٹ_param(cbi->param_buf, BOARDVARNAME,(حرف *)get_board_name(BOARD_ID_INTEL) , cbi);

	/* move user stack/data down to leave a gap to catch kernel
	 * stack overflow; and to distinguish kernel and user addresses
	 * at a glance (0xf.. vs 0xe..) 
	 */
	cbi->user_sp = USR_STACKTOP;
	cbi->user_انتہا = USR_DATATOP;

	/* kernel bytes without bootstrap code/data that is currently
	 * still needed but will be freed after bootstrapping.
	 */
	kinfo.kernel_allocated_بائٹیں = (phys_بائٹیں) &kern_سائز;
	kinfo.kernel_allocated_بائٹیں -= cbi->bootstrap_len;

	أكد(!(cbi->bootstrap_آغاز % I386_صفحہ_سائز));
	cbi->bootstrap_len = rounddown(cbi->bootstrap_len, I386_صفحہ_سائز);
	أكد(mbi->mi_flags & MULTIBOOT_INFO_HAS_MODS);
	أكد(mbi->mi_mods_گنتی < MULTIBOOT_MAX_MODS);
	أكد(mbi->mi_mods_گنتی > 0);
	حافظہ_نقل(&cbi->module_list, (خالی *) mbi->mi_mods_addr,
		mbi->mi_mods_گنتی * حجم(multiboot_module_t));
	
	memset(cbi->memmap, 0, حجم(cbi->memmap));
	/* mem_map has a variable layout */
	اگر(mbi->mi_flags & MULTIBOOT_INFO_HAS_MMAP) {
		cbi->mmap_سائز = 0;
	        برائے (mmap = (multiboot_حافظہ_map_t *) mbi->mmap_addr;
       	     (غیرعلامتی طویل) mmap < mbi->mmap_addr + mbi->mmap_لمبائی;
       	       mmap = (multiboot_حافظہ_map_t *) 
		      	((غیرعلامتی طویل) mmap + mmap->mm_سائز + حجم(mmap->mm_سائز))) {
			اگر(mmap->mm_type != MULTIBOOT_حافظہ_دستیاب) جاری;
			جوڑو_memmap(cbi, mmap->mm_بنیاد_addr, mmap->mm_لمبائی);
		}
	} ورنہ {
		أكد(mbi->mi_flags & MULTIBOOT_INFO_HAS_حافظہ);
		جوڑو_memmap(cbi, 0, mbi->mi_mem_زیریں*1024);
		جوڑو_memmap(cbi, 0x100000, mbi->mi_mem_بالائی*1024);
	}

	/* Sanity check: the kernel nor any of the modules may overlap
	 * with each other. Pretend the kernel is an extra module for a
	 * second.
	 */
	k = mbi->mi_mods_گنتی;
	أكد(k < MULTIBOOT_MAX_MODS);
	cbi->module_list[k].mod_آغاز = kernbase;
	cbi->module_list[k].mod_انتہا = kernbase + kernsize;
	cbi->mods_with_kernel = mbi->mi_mods_گنتی+1;
	cbi->kern_mod = k;

	برائے(m = 0; m < cbi->mods_with_kernel; m++) {
#اگر 0
		لکھو("checking overlap of module %08lx-%08lx\n",
		  cbi->module_list[m].mod_آغاز, cbi->module_list[m].mod_انتہا);
#اگر_ختم
		اگر(overlaps(cbi->module_list, cbi->mods_with_kernel, m))
			panic("overlapping boot modules/kernel");
		/* We cut out the bits of memory that we know are
		 * occupied by the kernel and boot modules.
		 */
		cut_memmap(cbi,
			cbi->module_list[m].mod_آغاز, 
			cbi->module_list[m].mod_انتہا);
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
	pg_شناختی_عنصر(&kinfo);
	kinfo.freepde_آغاز = pg_mapkernel();
	pg_بھرو();
	vm_enable_paging();

	/* Done, return boot info so it can be passed to kmain(). */
	واپس &kinfo;
}

خالی send_diag_sig(خالی) { }
خالی minix_shutdown(صحیح how) { arch_shutdown(how); }
خالی busy_delay_ms(صحیح x) { }
صحیح اٹھاؤ(صحیح sig) { panic("raise(%d)\n", sig); }
