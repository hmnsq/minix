#تعریف_کرو UNPAGED 1	/* for proper kmain() prototype */

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
#تعریف_کرو MULTIBOOT_VERBOSE 1
#اگر_ختم

/* to-be-built kinfo struct, diagnostics buffer */
kinfo_t kinfo;
ساخت kmessages kmessages;

/* pg_utils.c uses this; in this phase, there is a 1:1 mapping. */
phys_بائٹیں vir2phys(خالی *addr) { واپس (phys_بائٹیں) addr; }

ساکن خالی تیاری_mbi(multiboot_info_t *mbi, حرف *bootargs);

/* String length used for mb_itoa */
#تعریف_کرو ITOA_بفر_سائز 20

/* Kernel may use memory */
صحیح kernel_may_alloc = 1;

/* kernel bss */
خارجی u32_t _edata;
خارجی u32_t انتہا_2;

/* kernel unpaged bss */
خارجی حرف _kern_unpaged_edata;
خارجی حرف kern_unpaged_انتہا;

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
صحیح ڈھونڈو_قدر(حرف * content,حرف * کلید,حرف *قدر,صحیح قدر_max_len){

	حرف *iter,*keyp;
	صحیح کلید_len,content_len,مطابقت_len,قدر_len;

	/* return if the input is invalid */
	اگر  (کلید == تہی || content == تہی || قدر == تہی) {
		واپس 1;
	}

	/* find the key and content length */
	کلید_len = content_len =0;
	برائے(iter = کلید ; *iter != '\0'; iter++, کلید_len++);
	برائے(iter = content ; *iter != '\0'; iter++, content_len++);

	/* return if key or content length invalid */
	اگر (کلید_len == 0 || content_len == 0) {
		واپس 1;
	}

	/* now find the key in the contents */
	مطابقت_len =0;
	برائے (iter = content ,keyp=کلید; مطابقت_len < کلید_len && *iter != '\0' ; iter++) {
		اگر (*iter == *keyp) {
			مطابقت_len++;
			keyp++;
			جاری;
		} 
		/* The current key does not match the value , reset */
		مطابقت_len =0;
		keyp=کلید;
	}

	اگر (مطابقت_len == کلید_len) {
		لکھو("key found at %d %s\n", مطابقت_len, &content[مطابقت_len]);
		قدر_len = 0;
		/* copy the content to the value char iter already points to the first 
		   char value */
		جب(*iter != '\0' && *iter != ' ' && قدر_len  + 1< قدر_max_len) {
			*قدر++ = *iter++;
			قدر_len++;
		}
		*قدر='\0';
		واپس 0;
	}
	واپس 1; /* not found */
}

ساکن صحیح mb_سیٹ_param(حرف *bigbuf,حرف *name,حرف *قدر, kinfo_t *cbi)
{
	/* bigbuf contains a list of key=value pairs separated by \0 char.
	 * The list itself is ended by a second \0 terminator*/
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
			/* let q point to the end of the entry */
			جب (*q) q++; 
			/* now copy the remained of the buffer */
			برائے (q++; q < bufend; q++, p++)
				*p = *q;
			توڑو;
		}

		/* find the end of the buffer */
		جب (*p++);
		p++;
	}
	

	/* find the first empty spot */
	برائے (p = bigbuf; p < bufend && (*p || *(p + 1)); p++);

	/* unless we are the first entry step over the delimiter */
	اگر (p > bigbuf) p++;
	
	/* Make sure there's enough space for the new parameter */
	اگر (p + namelen + valuelen + 3 > bufend) {
		واپس -1;
	}
	
	strcpy(p, name);
	p[namelen] = '=';
	strcpy(p + namelen + 1, قدر);
	p[namelen + valuelen + 1] = 0;
	p[namelen + valuelen + 2] = 0; /* end with a second delimiter */
	واپس 0;
}

صحیح overlaps(multiboot_module_t *mod, صحیح n, صحیح cmp_mod)
{
	multiboot_module_t *cmp = &mod[cmp_mod];
	صحیح m;

#تعریف_کرو INRANGE(mod, v) ((v) >= mod->mod_آغاز && (v) <= thismod->mod_انتہا)
#تعریف_کرو OVERLAP(mod1, mod2) (INRANGE(mod1, mod2->mod_آغاز) || \
	    INRANGE(mod1, mod2->mod_انتہا))
	برائے(m = 0; m < n; m++) {
		multiboot_module_t *thismod = &mod[m];
		اگر(m == cmp_mod) جاری;
		اگر(OVERLAP(thismod, cmp)) {
			واپس 1;
		}
	}
	واپس 0;
}

/* XXX: hard-coded stuff for modules */
#تعریف_کرو MB_MODS_NR NR_BOOT_MODULES
#تعریف_کرو MB_MODS_بنیاد  0x82000000
#تعریف_کرو MB_MODS_ALIGN 0x00800000 /* 8 MB */
#تعریف_کرو MB_MMAP_آغاز 0x80000000
#تعریف_کرو MB_MMAP_سائز  0x10000000 /* 256 MB */

multiboot_module_t mb_modlist[MB_MODS_NR];
multiboot_حافظہ_map_t mb_memmap;

خالی تیاری_mbi(multiboot_info_t *mbi, حرف *bootargs)
{
	memset(mbi, 0, حجم(*mbi));
	mbi->flags = MULTIBOOT_INFO_MODS | MULTIBOOT_INFO_MEM_MAP |
			MULTIBOOT_INFO_CMDLINE;
	mbi->mi_mods_گنتی = MB_MODS_NR;
	mbi->mods_addr = (u32_t)&mb_modlist;

	صحیح i;
	برائے (i = 0; i < MB_MODS_NR; ++i) {
		mb_modlist[i].mod_آغاز = MB_MODS_بنیاد + i * MB_MODS_ALIGN;
		mb_modlist[i].mod_انتہا = mb_modlist[i].mod_آغاز + MB_MODS_ALIGN
		    - ARM_صفحہ_سائز;
		mb_modlist[i].cmdline = 0;
	}

	/* morph the bootargs into multiboot */
	mbi->cmdline = (u32_t) bootargs;

	mbi->mmap_addr =(u32_t)&mb_memmap;
	mbi->mmap_لمبائی = حجم(mb_memmap);

	mb_memmap.سائز = حجم(multiboot_حافظہ_map_t);
	mb_memmap.mm_بنیاد_addr = MB_MMAP_آغاز;
	mb_memmap.mm_لمبائی  = MB_MMAP_سائز;
	mb_memmap.type = MULTIBOOT_حافظہ_دستیاب;
}

خالی get_parameters(kinfo_t *cbi, حرف *bootargs)
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
	تیاری_mbi(mbi, bootargs);

	/* Set various bits of info for the higher-level kernel. */
	cbi->mem_بلند_phys = 0;
	cbi->user_sp = (vir_بائٹیں) &kern_vir_بنیاد;
	cbi->vir_kern_آغاز = (vir_بائٹیں) &kern_vir_بنیاد;
	cbi->bootstrap_آغاز = (vir_بائٹیں) &kern_unpaged_آغاز;
	cbi->bootstrap_len = (vir_بائٹیں) &kern_unpaged_انتہا -
		cbi->bootstrap_آغاز;
	cbi->kmess = &kmess;

	/* set some configurable defaults */
	cbi->do_serial_debug = 1;
	cbi->serial_debug_baud = 115200;

	/* parse boot command line */
	اگر (mbi->flags&MULTIBOOT_INFO_CMDLINE) {
		ساکن حرف var[BUF];
		ساکن حرف قدر[BUF];

		/* Override values with cmdline argument */
		حافظہ_نقل(cmdline, (خالی *) mbi->cmdline, BUF);
		p = cmdline;
		جب (*p) {
			var_i = 0;
			قدر_i = 0;
			جب (*p == ' ') p++; /* skip spaces */
			اگر (!*p) توڑو; /* is this the end? */
			جب (*p && *p != '=' && *p != ' ' && var_i < BUF - 1)
				var[var_i++] = *p++ ;
			var[var_i] = 0;
			اگر (*p++ != '=') جاری; /* skip if not name=value */
			جب (*p && *p != ' ' && قدر_i < BUF - 1) {
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
	cbi->user_انتہا = USR_DATATOP;

	/* kernel bytes without bootstrap code/data that is currently
	 * still needed but will be freed after bootstrapping.
	 */
	kinfo.kernel_allocated_بائٹیں = (phys_بائٹیں) &kern_سائز;
	kinfo.kernel_allocated_بائٹیں -= cbi->bootstrap_len;

	أكد(!(cbi->bootstrap_آغاز % ARM_صفحہ_سائز));
	cbi->bootstrap_len = rounddown(cbi->bootstrap_len, ARM_صفحہ_سائز);
	أكد(mbi->flags & MULTIBOOT_INFO_MODS);
	أكد(mbi->mi_mods_گنتی < MULTIBOOT_MAX_MODS);
	أكد(mbi->mi_mods_گنتی > 0);
	حافظہ_نقل(&cbi->module_list, (خالی *) mbi->mods_addr,
		mbi->mi_mods_گنتی * حجم(multiboot_module_t));
	
	memset(cbi->memmap, 0, حجم(cbi->memmap));
	/* mem_map has a variable layout */
	اگر(mbi->flags & MULTIBOOT_INFO_MEM_MAP) {
		cbi->mmap_سائز = 0;
	        برائے (mmap = (multiboot_حافظہ_map_t *) mbi->mmap_addr;
       	     (غیرعلامتی طویل) mmap < mbi->mmap_addr + mbi->mmap_لمبائی;
       	       mmap = (multiboot_حافظہ_map_t *) 
		      	((غیرعلامتی طویل) mmap + mmap->سائز + حجم(mmap->سائز))) {
			اگر(mmap->type != MULTIBOOT_حافظہ_دستیاب) جاری;
			جوڑو_memmap(cbi, mmap->mm_بنیاد_addr, mmap->mm_لمبائی);
		}
	} ورنہ {
		أكد(mbi->flags & MULTIBOOT_INFO_حافظہ);
		جوڑو_memmap(cbi, 0, mbi->mem_زیریں_غیر_مستعمل*1024);
		جوڑو_memmap(cbi, 0x100000, mbi->mem_بالائی_غیر_مستعمل*1024);
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

/* 
 * During low level init many things are not supposed to work
 * serial being one of them. We therefore can't rely on the
 * serial to debug. POORMANS_FAILURE_NOTIFICATION can be used
 * before we setup our own vector table and will result in calling
 * the bootloader's debugging methods that will hopefully show some
 * information like the currnet PC at on the serial.
 */
#تعریف_کرو POORMANS_FAILURE_NOTIFICATION  asm متلون("svc #00\n")

/* use the passed cmdline argument to determine the machine id */
خالی سیٹ_machine_id(حرف *cmdline)
{

	حرف boardname[20];
	memset(boardname,'\0',20);
	اگر (ڈھونڈو_قدر(cmdline,"board_name=",boardname,20)){
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

kinfo_t *pre_init(صحیح argc, حرف **argv)
{
	حرف *bootargs;
	/* This is the main "c" entry point into the kernel. It gets called
	   from head.S */
	   
	/* Clear BSS */
	memset(&_edata, 0, (u32_t)&انتہا_2 - (u32_t)&_edata);
        memset(&_kern_unpaged_edata, 0, (u32_t)&kern_unpaged_انتہا - (u32_t)&_kern_unpaged_edata);

	/* we get called in a c like fashion where the first arg
         * is the program name (load address) and the rest are
	 * arguments. by convention the second argument is the
	 *  command line */
	اگر (argc != 2) {
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
	pg_شناختی_عنصر(&kinfo);
	kinfo.freepde_آغاز = pg_mapkernel();
	pg_بھرو();
	vm_enable_paging();

	/* Done, return boot info so it can be passed to kmain(). */
	واپس &kinfo;
}

/* pre_init gets executed at the memory location where the kernel was loaded by the boot loader.
 * at that stage we only have a minimum set of functionality present (all symbols gets renamed to
 * ensure this). The following methods are used in that context. Once we jump to kmain they are no
 * longer used and the "real" implementations are visible
 */
خالی send_diag_sig(خالی) { }
خالی minix_shutdown(صحیح how) { arch_shutdown(how); }
خالی busy_delay_ms(صحیح x) { }
صحیح اٹھاؤ(صحیح n) { panic("raise(%d)\n", n); }
صحیح kern_phys_map_ptr( phys_بائٹیں بنیاد_پتہ, vir_بائٹیں io_سائز, صحیح vm_flags,
ساخت kern_phys_map * priv, vir_بائٹیں ptr) { واپس -1; };
ساخت machine machine; /* pre init stage machine */
