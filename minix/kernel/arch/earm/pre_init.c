#تعریف_کرو UNPAGED 1	/* for proper kmain() prototype */

#ضمن "kernel/kernel.h"
#ضمن <assert.h>
#ضمن <stdlib.h>
#ضمن <minix/minlib.h>
#ضمن <minix/const.h>
#ضمن <minix/type.h>
#ضمن <minix/board.h>
#ضمن <minix/com.h>
#ضمن <sys/types.h>
#ضمن <sys/param.h>
#ضمن <sys/reboot.h>
#ضمن "string.h"
#ضمن "arch_proto.h"
#ضمن "direct_utils.h"
#ضمن "bsp_serial.h"
#ضمن "glo.h"
#ضمن <machine/multiboot.h>

#إذا USE_SYSDEBUG
#تعریف_کرو MULTIBOOT_VERBOSE 1
#جے_انت

/* to-be-built kinfo struct, diagnostics buffer */
kinfo_t kinfo;
بنية kmessages kmessages;

/* pg_utils.c uses this; in this phase, there is a 1:1 mapping. */
phys_بائٹاں vir2phys(فراغ *addr) { أرجع (phys_بائٹاں) addr; }

ساكن فراغ تیاری_mbi(multiboot_info_t *mbi, حرف *bootargs);

/* String length used for mb_itoa */
#تعریف_کرو ITOA_بفر_ناپ 20

/* Kernel may use memory */
صحيح kernel_may_alloc = 1;

/* kernel bss */
خارجي u32_t _edata;
خارجي u32_t سرا_2;

/* kernel unpaged bss */
خارجي حرف _kern_unpaged_edata;
خارجي حرف kern_unpaged_سرا;

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
صحيح لبھو_قدر(حرف * content,حرف * کلید,حرف *قدر,صحيح قدر_max_len){

	حرف *iter,*keyp;
	صحيح کلید_len,content_len,میل_len,قدر_len;

	/* return if the input is invalid */
	إذا  (کلید == عدم || content == عدم || قدر == عدم) {
		أرجع 1;
	}

	/* find the key and content length */
	کلید_len = content_len =0;
	لكل(iter = کلید ; *iter != '\0'; iter++, کلید_len++);
	لكل(iter = content ; *iter != '\0'; iter++, content_len++);

	/* return if key or content length invalid */
	إذا (کلید_len == 0 || content_len == 0) {
		أرجع 1;
	}

	/* now find the key in the contents */
	میل_len =0;
	لكل (iter = content ,keyp=کلید; میل_len < کلید_len && *iter != '\0' ; iter++) {
		إذا (*iter == *keyp) {
			میل_len++;
			keyp++;
			استمر;
		} 
		/* The current key does not match the value , reset */
		میل_len =0;
		keyp=کلید;
	}

	إذا (میل_len == کلید_len) {
		لکھو("key found at %d %s\n", میل_len, &content[میل_len]);
		قدر_len = 0;
		/* copy the content to the value char iter already points to the first 
		   char value */
		طالما(*iter != '\0' && *iter != ' ' && قدر_len  + 1< قدر_max_len) {
			*قدر++ = *iter++;
			قدر_len++;
		}
		*قدر='\0';
		أرجع 0;
	}
	أرجع 1; /* not found */
}

ساكن صحيح mb_سیٹ_param(حرف *bigbuf,حرف *name,حرف *قدر, kinfo_t *cbi)
{
	/* bigbuf contains a list of key=value pairs separated by \0 char.
	 * The list itself is ended by a second \0 terminator*/
	حرف *p = bigbuf;
	حرف *bufend = bigbuf + MULTIBOOT_PARAM_BUF_ناپ;
	حرف *q;
	صحيح namelen = لڑی_لمبائی(name);
	صحيح valuelen = لڑی_لمبائی(قدر);

	/* Some variables we recognize */
	إذا(!strcmp(name, SERVARNAME)) { cbi->do_serial_debug = 1; }
	إذا(!strcmp(name, SERBAUDVARNAME)) { cbi->serial_debug_baud = atoi(قدر); }

	/* Delete the item if already exists */
	طالما (*p) {
		إذا (لڑی_مقابلہ(p, name, namelen) == 0 && p[namelen] == '=') {
			q = p;
			/* let q point to the end of the entry */
			طالما (*q) q++; 
			/* now copy the remained of the buffer */
			لكل (q++; q < bufend; q++, p++)
				*p = *q;
			اكسر;
		}

		/* find the end of the buffer */
		طالما (*p++);
		p++;
	}
	

	/* find the first empty spot */
	لكل (p = bigbuf; p < bufend && (*p || *(p + 1)); p++);

	/* unless we are the first entry step over the delimiter */
	إذا (p > bigbuf) p++;
	
	/* Make sure there's enough space for the new parameter */
	إذا (p + namelen + valuelen + 3 > bufend) {
		أرجع -1;
	}
	
	strcpy(p, name);
	p[namelen] = '=';
	strcpy(p + namelen + 1, قدر);
	p[namelen + valuelen + 1] = 0;
	p[namelen + valuelen + 2] = 0; /* end with a second delimiter */
	أرجع 0;
}

صحيح overlaps(multiboot_module_t *mod, صحيح n, صحيح cmp_mod)
{
	multiboot_module_t *cmp = &mod[cmp_mod];
	صحيح m;

#تعریف_کرو INRANGE(mod, v) ((v) >= mod->mod_شروع && (v) <= thismod->mod_سرا)
#تعریف_کرو OVERLAP(mod1, mod2) (INRANGE(mod1, mod2->mod_شروع) || \
	    INRANGE(mod1, mod2->mod_سرا))
	لكل(m = 0; m < n; m++) {
		multiboot_module_t *thismod = &mod[m];
		إذا(m == cmp_mod) استمر;
		إذا(OVERLAP(thismod, cmp)) {
			أرجع 1;
		}
	}
	أرجع 0;
}

/* XXX: hard-coded stuff for modules */
#تعریف_کرو MB_MODS_NR NR_BOOT_MODULES
#تعریف_کرو MB_MODS_ادھار  0x82000000
#تعریف_کرو MB_MODS_ALIGN 0x00800000 /* 8 MB */
#تعریف_کرو MB_MMAP_شروع 0x80000000
#تعریف_کرو MB_MMAP_ناپ  0x10000000 /* 256 MB */

multiboot_module_t mb_modlist[MB_MODS_NR];
multiboot_حافظہ_map_t mb_memmap;

فراغ تیاری_mbi(multiboot_info_t *mbi, حرف *bootargs)
{
	memset(mbi, 0, حجم(*mbi));
	mbi->flags = MULTIBOOT_INFO_MODS | MULTIBOOT_INFO_MEM_MAP |
			MULTIBOOT_INFO_CMDLINE;
	mbi->mi_mods_گنتی = MB_MODS_NR;
	mbi->mods_addr = (u32_t)&mb_modlist;

	صحيح i;
	لكل (i = 0; i < MB_MODS_NR; ++i) {
		mb_modlist[i].mod_شروع = MB_MODS_ادھار + i * MB_MODS_ALIGN;
		mb_modlist[i].mod_سرا = mb_modlist[i].mod_شروع + MB_MODS_ALIGN
		    - ARM_ورقہ_ناپ;
		mb_modlist[i].cmdline = 0;
	}

	/* morph the bootargs into multiboot */
	mbi->cmdline = (u32_t) bootargs;

	mbi->mmap_addr =(u32_t)&mb_memmap;
	mbi->mmap_لمائی = حجم(mb_memmap);

	mb_memmap.ناپ = حجم(multiboot_حافظہ_map_t);
	mb_memmap.mm_ادھار_addr = MB_MMAP_شروع;
	mb_memmap.mm_لمائی  = MB_MMAP_ناپ;
	mb_memmap.type = MULTIBOOT_حافظہ_دستیاب;
}

فراغ get_parameters(kinfo_t *cbi, حرف *bootargs)
{
	multiboot_حافظہ_map_t *mmap;
	multiboot_info_t *mbi = &cbi->mbi;
	صحيح var_i,قدر_i, m, k;
	حرف *p;
	خارجي حرف kern_phys_ادھار, kern_vir_ادھار, kern_ناپ,
	    kern_unpaged_شروع, kern_unpaged_سرا;
	phys_بائٹاں kernbase = (phys_بائٹاں) &kern_phys_ادھار,
	    kernsize = (phys_بائٹاں) &kern_ناپ;
#تعریف_کرو BUF 1024
	ساكن حرف cmdline[BUF];

	/* get our own copy of the multiboot info struct and module list */
	تیاری_mbi(mbi, bootargs);

	/* Set various bits of info for the higher-level kernel. */
	cbi->mem_بلند_phys = 0;
	cbi->user_sp = (vir_بائٹاں) &kern_vir_ادھار;
	cbi->vir_kern_شروع = (vir_بائٹاں) &kern_vir_ادھار;
	cbi->bootstrap_شروع = (vir_بائٹاں) &kern_unpaged_شروع;
	cbi->bootstrap_len = (vir_بائٹاں) &kern_unpaged_سرا -
		cbi->bootstrap_شروع;
	cbi->kmess = &kmess;

	/* set some configurable defaults */
	cbi->do_serial_debug = 1;
	cbi->serial_debug_baud = 115200;

	/* parse boot command line */
	إذا (mbi->flags&MULTIBOOT_INFO_CMDLINE) {
		ساكن حرف var[BUF];
		ساكن حرف قدر[BUF];

		/* Override values with cmdline argument */
		حافظہ_نقل(cmdline, (فراغ *) mbi->cmdline, BUF);
		p = cmdline;
		طالما (*p) {
			var_i = 0;
			قدر_i = 0;
			طالما (*p == ' ') p++; /* skip spaces */
			إذا (!*p) اكسر; /* is this the end? */
			طالما (*p && *p != '=' && *p != ' ' && var_i < BUF - 1)
				var[var_i++] = *p++ ;
			var[var_i] = 0;
			إذا (*p++ != '=') استمر; /* skip if not name=value */
			طالما (*p && *p != ' ' && قدر_i < BUF - 1) {
				قدر[قدر_i++] = *p++ ;
			}
			قدر[قدر_i] = 0;
			
			mb_سیٹ_param(cbi->param_buf, var, قدر, cbi);
		}
	}

	/* let higher levels know what we are booting on */
	mb_سیٹ_param(cbi->param_buf, ARCHVARNAME, (حرف *)get_board_arch_name(machine.board_id), cbi);
	mb_سیٹ_param(cbi->param_buf, BOARDVARNAME,(حرف *)get_board_name(machine.board_id) , cbi);
	

	/* move user stack/data down to leave a gap to catch kernel
	 * stack overflow; and to distinguish kernel and user addresses
	 * at a glance (0xf.. vs 0xe..) 
	 */
	cbi->user_sp = USR_STACKTOP;
	cbi->user_سرا = USR_DATATOP;

	/* kernel bytes without bootstrap code/data that is currently
	 * still needed but will be freed after bootstrapping.
	 */
	kinfo.kernel_allocated_بائٹاں = (phys_بائٹاں) &kern_ناپ;
	kinfo.kernel_allocated_بائٹاں -= cbi->bootstrap_len;

	أكد(!(cbi->bootstrap_شروع % ARM_ورقہ_ناپ));
	cbi->bootstrap_len = rounddown(cbi->bootstrap_len, ARM_ورقہ_ناپ);
	أكد(mbi->flags & MULTIBOOT_INFO_MODS);
	أكد(mbi->mi_mods_گنتی < MULTIBOOT_MAX_MODS);
	أكد(mbi->mi_mods_گنتی > 0);
	حافظہ_نقل(&cbi->module_list, (فراغ *) mbi->mods_addr,
		mbi->mi_mods_گنتی * حجم(multiboot_module_t));
	
	memset(cbi->memmap, 0, حجم(cbi->memmap));
	/* mem_map has a variable layout */
	إذا(mbi->flags & MULTIBOOT_INFO_MEM_MAP) {
		cbi->mmap_ناپ = 0;
	        لكل (mmap = (multiboot_حافظہ_map_t *) mbi->mmap_addr;
       	     (بنانشانی لما) mmap < mbi->mmap_addr + mbi->mmap_لمائی;
       	       mmap = (multiboot_حافظہ_map_t *) 
		      	((بنانشانی لما) mmap + mmap->ناپ + حجم(mmap->ناپ))) {
			إذا(mmap->type != MULTIBOOT_حافظہ_دستیاب) استمر;
			جوڑو_memmap(cbi, mmap->mm_ادھار_addr, mmap->mm_لمائی);
		}
	} وإلا {
		أكد(mbi->flags & MULTIBOOT_INFO_حافظہ);
		جوڑو_memmap(cbi, 0, mbi->mem_ہیٹھلا_بن_ورتیا*1024);
		جوڑو_memmap(cbi, 0x100000, mbi->mem_اتلا_بن_ورتیا*1024);
	}

	/* Sanity check: the kernel nor any of the modules may overlap
	 * with each other. Pretend the kernel is an extra module for a
	 * second.
	 */
	k = mbi->mi_mods_گنتی;
	أكد(k < MULTIBOOT_MAX_MODS);
	cbi->module_list[k].mod_شروع = kernbase;
	cbi->module_list[k].mod_سرا = kernbase + kernsize;
	cbi->mods_with_kernel = mbi->mi_mods_گنتی+1;
	cbi->kern_mod = k;

	لكل(m = 0; m < cbi->mods_with_kernel; m++) {
#إذا 0
		لکھو("checking overlap of module %08lx-%08lx\n",
		  cbi->module_list[m].mod_شروع, cbi->module_list[m].mod_سرا);
#جے_انت
		إذا(overlaps(cbi->module_list, cbi->mods_with_kernel, m))
			panic("overlapping boot modules/kernel");
		/* We cut out the bits of memory that we know are
		 * occupied by the kernel and boot modules.
		 */
		cut_memmap(cbi,
			cbi->module_list[m].mod_شروع,
			cbi->module_list[m].mod_سرا);
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
#تعریف_کرو POORMANS_FAILURE_NOTIFICATION  asm متطاير("svc #00\n")

/* use the passed cmdline argument to determine the machine id */
فراغ سیٹ_machine_id(حرف *cmdline)
{

	حرف boardname[20];
	memset(boardname,'\0',20);
	إذا (لبھو_قدر(cmdline,"board_name=",boardname,20)){
		/* we expect the bootloader to pass a board_name as argument
		 * this however did not happen and given we still are in early
		 * boot we can't use the serial. We therefore generate an interrupt
		 * and hope the bootloader will do something nice with it */
		POORMANS_FAILURE_NOTIFICATION;
	}  
	machine.board_id = get_board_id_by_short_name(boardname);

	إذا (machine.board_id ==0){
		/* same thing as above there is no safe escape */
		POORMANS_FAILURE_NOTIFICATION;
	}
}

kinfo_t *pre_init(صحيح argc, حرف **argv)
{
	حرف *bootargs;
	/* This is the main "c" entry point into the kernel. It gets called
	   from head.S */
	   
	/* Clear BSS */
	memset(&_edata, 0, (u32_t)&سرا_2 - (u32_t)&_edata);
        memset(&_kern_unpaged_edata, 0, (u32_t)&kern_unpaged_سرا - (u32_t)&_kern_unpaged_edata);

	/* we get called in a c like fashion where the first arg
         * is the program name (load address) and the rest are
	 * arguments. by convention the second argument is the
	 *  command line */
	إذا (argc != 2) {
		POORMANS_FAILURE_NOTIFICATION;
	}

	bootargs = argv[1];
	سیٹ_machine_id(bootargs);
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
	pg_پچھان_تت(&kinfo);
	kinfo.freepde_شروع = pg_mapkernel();
	pg_لدو();
	vm_enable_paging();

	/* Done, return boot info so it can be passed to kmain(). */
	أرجع &kinfo;
}

/* pre_init gets executed at the memory location where the kernel was loaded by the boot loader.
 * at that stage we only have a minimum set of functionality present (all symbols gets renamed to
 * ensure this). The following methods are used in that context. Once we jump to kmain they are no
 * longer used and the "real" implementations are visible
 */
فراغ send_diag_sig(فراغ) { }
فراغ minix_shutdown(صحيح how) { arch_shutdown(how); }
فراغ busy_delay_ms(صحيح x) { }
صحيح ارفع(صحيح n) { panic("raise(%d)\n", n); }
صحيح kern_phys_map_ptr( phys_بائٹاں ادھار_پتہ, vir_بائٹاں io_ناپ, صحيح vm_flags,
بنية kern_phys_map * priv, vir_بائٹاں ptr) { أرجع -1; };
بنية machine machine; /* pre init stage machine */
