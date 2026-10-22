#घोषणा UNPAGED 1	/* for proper kmain() prototype */

#समावेश "kernel/kernel.h"
#समावेश <assert.h>
#समावेश <stdlib.h>
#समावेश <minix/minlib.h>
#समावेश <minix/const.h>
#समावेश <minix/type.h>
#समावेश <minix/board.h>
#समावेश <minix/com.h>
#समावेश <sys/types.h>
#समावेश <sys/param.h>
#समावेश <sys/reboot.h>
#समावेश "string.h"
#समावेश "arch_proto.h"
#समावेश "direct_utils.h"
#समावेश "bsp_serial.h"
#समावेश "glo.h"
#समावेश <machine/multiboot.h>

#अगर USE_SYSDEBUG
#घोषणा MULTIBOOT_VERBOSE 1
#पूर्ण_अगर

/* to-be-built kinfo struct, diagnostics buffer */
kinfo_t kinfo;
काष्ठा kmessages kmessages;

/* pg_utils.c uses this; in this phase, there is a 1:1 mapping. */
phys_bytes vir2phys(व्योम *addr) { वापस (phys_bytes) addr; }

अटल व्योम setup_mbi(multiboot_info_t *mbi, अक्षर *bootargs);

/* String length used for mb_itoa */
#घोषणा ITOA_बफर_आकार 20

/* Kernel may use memory */
पूर्णांक kernel_may_alloc = 1;

/* kernel bss */
बाह्य u32_t _edata;
बाह्य u32_t _end;

/* kernel unpaged bss */
बाह्य अक्षर _kern_unpaged_edata;
बाह्य अक्षर _kern_unpaged_end;

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
पूर्णांक find_मूल्य(अक्षर * content,अक्षर * कुंजी,अक्षर *मूल्य,पूर्णांक मूल्य_max_len){

	अक्षर *iter,*keyp;
	पूर्णांक कुंजी_len,content_len,match_len,मूल्य_len;

	/* return if the input is invalid */
	अगर  (कुंजी == शून्य || content == शून्य || मूल्य == शून्य) {
		वापस 1;
	}

	/* find the key and content length */
	कुंजी_len = content_len =0;
	क्रम(iter = कुंजी ; *iter != '\0'; iter++, कुंजी_len++);
	क्रम(iter = content ; *iter != '\0'; iter++, content_len++);

	/* return if key or content length invalid */
	अगर (कुंजी_len == 0 || content_len == 0) {
		वापस 1;
	}

	/* now find the key in the contents */
	match_len =0;
	क्रम (iter = content ,keyp=कुंजी; match_len < कुंजी_len && *iter != '\0' ; iter++) {
		अगर (*iter == *keyp) {
			match_len++;
			keyp++;
			जारी;
		} 
		/* The current key does not match the value , reset */
		match_len =0;
		keyp=कुंजी;
	}

	अगर (match_len == कुंजी_len) {
		म_लिखो("key found at %d %s\n", match_len, &content[match_len]);
		मूल्य_len = 0;
		/* copy the content to the value char iter already points to the first 
		   char value */
		जबतक(*iter != '\0' && *iter != ' ' && मूल्य_len  + 1< मूल्य_max_len) {
			*मूल्य++ = *iter++;
			मूल्य_len++;
		}
		*मूल्य='\0';
		वापस 0;
	}
	वापस 1; /* not found */
}

अटल पूर्णांक mb_set_param(अक्षर *bigbuf,अक्षर *name,अक्षर *मूल्य, kinfo_t *cbi)
{
	/* bigbuf contains a list of key=value pairs separated by \0 char.
	 * The list itself is ended by a second \0 terminator*/
	अक्षर *p = bigbuf;
	अक्षर *bufend = bigbuf + MULTIBOOT_PARAM_BUF_आकार;
	अक्षर *q;
	पूर्णांक namelen = म_माप(name);
	पूर्णांक valuelen = म_माप(मूल्य);

	/* Some variables we recognize */
	अगर(!म_भेद(name, SERVARNAME)) { cbi->do_serial_debug = 1; }
	अगर(!म_भेद(name, SERBAUDVARNAME)) { cbi->serial_debug_baud = म_से_प(मूल्य); }

	/* Delete the item if already exists */
	जबतक (*p) {
		अगर (म_भेदन(p, name, namelen) == 0 && p[namelen] == '=') {
			q = p;
			/* let q point to the end of the entry */
			जबतक (*q) q++; 
			/* now copy the remained of the buffer */
			क्रम (q++; q < bufend; q++, p++)
				*p = *q;
			अवरोध;
		}

		/* find the end of the buffer */
		जबतक (*p++);
		p++;
	}
	

	/* find the first empty spot */
	क्रम (p = bigbuf; p < bufend && (*p || *(p + 1)); p++);

	/* unless we are the first entry step over the delimiter */
	अगर (p > bigbuf) p++;
	
	/* Make sure there's enough space for the new parameter */
	अगर (p + namelen + valuelen + 3 > bufend) {
		वापस -1;
	}
	
	म_नकल(p, name);
	p[namelen] = '=';
	म_नकल(p + namelen + 1, मूल्य);
	p[namelen + valuelen + 1] = 0;
	p[namelen + valuelen + 2] = 0; /* end with a second delimiter */
	वापस 0;
}

पूर्णांक overlaps(multiboot_module_t *mod, पूर्णांक n, पूर्णांक cmp_mod)
{
	multiboot_module_t *cmp = &mod[cmp_mod];
	पूर्णांक m;

#घोषणा INRANGE(mod, v) ((v) >= mod->mod_start && (v) <= thismod->mod_end)
#घोषणा OVERLAP(mod1, mod2) (INRANGE(mod1, mod2->mod_start) || \
	    INRANGE(mod1, mod2->mod_end))
	क्रम(m = 0; m < n; m++) {
		multiboot_module_t *thismod = &mod[m];
		अगर(m == cmp_mod) जारी;
		अगर(OVERLAP(thismod, cmp)) {
			वापस 1;
		}
	}
	वापस 0;
}

/* XXX: hard-coded stuff for modules */
#घोषणा MB_MODS_NR NR_BOOT_MODULES
#घोषणा MB_MODS_BASE  0x82000000
#घोषणा MB_MODS_ALIGN 0x00800000 /* 8 MB */
#घोषणा MB_MMAP_START 0x80000000
#घोषणा MB_MMAP_आकार  0x10000000 /* 256 MB */

multiboot_module_t mb_modlist[MB_MODS_NR];
multiboot_memory_map_t mb_memmap;

व्योम setup_mbi(multiboot_info_t *mbi, अक्षर *bootargs)
{
	स_रखो(mbi, 0, माप(*mbi));
	mbi->flags = MULTIBOOT_INFO_MODS | MULTIBOOT_INFO_MEM_MAP |
			MULTIBOOT_INFO_CMDLINE;
	mbi->mi_mods_गिनती = MB_MODS_NR;
	mbi->mods_addr = (u32_t)&mb_modlist;

	पूर्णांक i;
	क्रम (i = 0; i < MB_MODS_NR; ++i) {
		mb_modlist[i].mod_start = MB_MODS_BASE + i * MB_MODS_ALIGN;
		mb_modlist[i].mod_end = mb_modlist[i].mod_start + MB_MODS_ALIGN
		    - ARM_PAGE_आकार;
		mb_modlist[i].cmdline = 0;
	}

	/* morph the bootargs into multiboot */
	mbi->cmdline = (u32_t) bootargs;

	mbi->mmap_addr =(u32_t)&mb_memmap;
	mbi->mmap_लंबाई = माप(mb_memmap);

	mb_memmap.आकार = माप(multiboot_memory_map_t);
	mb_memmap.mm_base_addr = MB_MMAP_START;
	mb_memmap.mm_लंबाई  = MB_MMAP_आकार;
	mb_memmap.type = MULTIBOOT_MEMORY_AVAILABLE;
}

व्योम get_parameters(kinfo_t *cbi, अक्षर *bootargs)
{
	multiboot_memory_map_t *mmap;
	multiboot_info_t *mbi = &cbi->mbi;
	पूर्णांक var_i,मूल्य_i, m, k;
	अक्षर *p;
	बाह्य अक्षर _kern_phys_base, _kern_vir_base, kern_आकार,
	    _kern_unpaged_start, _kern_unpaged_end;
	phys_bytes kernbase = (phys_bytes) &_kern_phys_base,
	    kernsize = (phys_bytes) &kern_आकार;
#घोषणा BUF 1024
	अटल अक्षर cmdline[BUF];

	/* get our own copy of the multiboot info struct and module list */
	setup_mbi(mbi, bootargs);

	/* Set various bits of info for the higher-level kernel. */
	cbi->mem_ऊपरी_phys = 0;
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
	अगर (mbi->flags&MULTIBOOT_INFO_CMDLINE) {
		अटल अक्षर var[BUF];
		अटल अक्षर मूल्य[BUF];

		/* Override values with cmdline argument */
		स_नकल(cmdline, (व्योम *) mbi->cmdline, BUF);
		p = cmdline;
		जबतक (*p) {
			var_i = 0;
			मूल्य_i = 0;
			जबतक (*p == ' ') p++; /* skip spaces */
			अगर (!*p) अवरोध; /* is this the end? */
			जबतक (*p && *p != '=' && *p != ' ' && var_i < BUF - 1)
				var[var_i++] = *p++ ;
			var[var_i] = 0;
			अगर (*p++ != '=') जारी; /* skip if not name=value */
			जबतक (*p && *p != ' ' && मूल्य_i < BUF - 1) {
				मूल्य[मूल्य_i++] = *p++ ;
			}
			मूल्य[मूल्य_i] = 0;
			
			mb_set_param(cbi->param_buf, var, मूल्य, cbi);
		}
	}

	/* let higher levels know what we are booting on */
	mb_set_param(cbi->param_buf, ARCHVARNAME, (अक्षर *)get_board_arch_name(machine.board_id), cbi);
	mb_set_param(cbi->param_buf, BOARDVARNAME,(अक्षर *)get_board_name(machine.board_id) , cbi);
	

	/* move user stack/data down to leave a gap to catch kernel
	 * stack overflow; and to distinguish kernel and user addresses
	 * at a glance (0xf.. vs 0xe..) 
	 */
	cbi->user_sp = USR_STACKTOP;
	cbi->user_end = USR_DATATOP;

	/* kernel bytes without bootstrap code/data that is currently
	 * still needed but will be freed after bootstrapping.
	 */
	kinfo.kernel_allocated_bytes = (phys_bytes) &kern_आकार;
	kinfo.kernel_allocated_bytes -= cbi->bootstrap_len;

	निश्चित(!(cbi->bootstrap_start % ARM_PAGE_आकार));
	cbi->bootstrap_len = rounddown(cbi->bootstrap_len, ARM_PAGE_आकार);
	निश्चित(mbi->flags & MULTIBOOT_INFO_MODS);
	निश्चित(mbi->mi_mods_गिनती < MULTIBOOT_MAX_MODS);
	निश्चित(mbi->mi_mods_गिनती > 0);
	स_नकल(&cbi->module_list, (व्योम *) mbi->mods_addr,
		mbi->mi_mods_गिनती * माप(multiboot_module_t));
	
	स_रखो(cbi->memmap, 0, माप(cbi->memmap));
	/* mem_map has a variable layout */
	अगर(mbi->flags & MULTIBOOT_INFO_MEM_MAP) {
		cbi->mmap_आकार = 0;
	        क्रम (mmap = (multiboot_memory_map_t *) mbi->mmap_addr;
       	     (अचिन्हित दीर्घ) mmap < mbi->mmap_addr + mbi->mmap_लंबाई;
       	       mmap = (multiboot_memory_map_t *) 
		      	((अचिन्हित दीर्घ) mmap + mmap->आकार + माप(mmap->आकार))) {
			अगर(mmap->type != MULTIBOOT_MEMORY_AVAILABLE) जारी;
			add_memmap(cbi, mmap->mm_base_addr, mmap->mm_लंबाई);
		}
	} अन्यथा {
		निश्चित(mbi->flags & MULTIBOOT_INFO_MEMORY);
		add_memmap(cbi, 0, mbi->mem_lower_unused*1024);
		add_memmap(cbi, 0x100000, mbi->mem_upper_unused*1024);
	}

	/* Sanity check: the kernel nor any of the modules may overlap
	 * with each other. Pretend the kernel is an extra module for a
	 * second.
	 */
	k = mbi->mi_mods_गिनती;
	निश्चित(k < MULTIBOOT_MAX_MODS);
	cbi->module_list[k].mod_start = kernbase;
	cbi->module_list[k].mod_end = kernbase + kernsize;
	cbi->mods_with_kernel = mbi->mi_mods_गिनती+1;
	cbi->kern_mod = k;

	क्रम(m = 0; m < cbi->mods_with_kernel; m++) {
#अगर 0
		म_लिखो("checking overlap of module %08lx-%08lx\n",
		  cbi->module_list[m].mod_start, cbi->module_list[m].mod_end);
#पूर्ण_अगर
		अगर(overlaps(cbi->module_list, cbi->mods_with_kernel, m))
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
#घोषणा POORMANS_FAILURE_NOTIFICATION  यंत्र अस्थिर("svc #00\n")

/* use the passed cmdline argument to determine the machine id */
व्योम set_machine_id(अक्षर *cmdline)
{

	अक्षर boardname[20];
	स_रखो(boardname,'\0',20);
	अगर (find_मूल्य(cmdline,"board_name=",boardname,20)){
		/* we expect the bootloader to pass a board_name as argument
		 * this however did not happen and given we still are in early
		 * boot we can't use the serial. We therefore generate an interrupt
		 * and hope the bootloader will do something nice with it */
		POORMANS_FAILURE_NOTIFICATION;
	}  
	machine.board_id = get_board_id_by_short_name(boardname);

	अगर (machine.board_id ==0){
		/* same thing as above there is no safe escape */
		POORMANS_FAILURE_NOTIFICATION;
	}
}

kinfo_t *pre_init(पूर्णांक argc, अक्षर **argv)
{
	अक्षर *bootargs;
	/* This is the main "c" entry point into the kernel. It gets called
	   from head.S */
	   
	/* Clear BSS */
	स_रखो(&_edata, 0, (u32_t)&_end - (u32_t)&_edata);
        स_रखो(&_kern_unpaged_edata, 0, (u32_t)&_kern_unpaged_end - (u32_t)&_kern_unpaged_edata);

	/* we get called in a c like fashion where the first arg
         * is the program name (load address) and the rest are
	 * arguments. by convention the second argument is the
	 *  command line */
	अगर (argc != 2) {
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
	वापस &kinfo;
}

/* pre_init gets executed at the memory location where the kernel was loaded by the boot loader.
 * at that stage we only have a minimum set of functionality present (all symbols gets renamed to
 * ensure this). The following methods are used in that context. Once we jump to kmain they are no
 * longer used and the "real" implementations are visible
 */
व्योम send_diag_sig(व्योम) { }
व्योम minix_shutdown(पूर्णांक how) { arch_shutdown(how); }
व्योम busy_delay_ms(पूर्णांक x) { }
पूर्णांक उठाओ(पूर्णांक n) { panic("raise(%d)\n", n); }
पूर्णांक kern_phys_map_ptr( phys_bytes base_address, vir_bytes io_आकार, पूर्णांक vm_flags,
काष्ठा kern_phys_map * priv, vir_bytes ptr) { वापस -1; };
काष्ठा machine machine; /* pre init stage machine */
