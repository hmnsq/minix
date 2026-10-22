
#घोषणा UNPAGED 1	/* for proper kmain() prototype */

#समावेश <assert.h>
#समावेश <stdlib.h>
#समावेश <minix/minlib.h>
#समावेश <minix/board.h>
#समावेश <sys/reboot.h>
#समावेश <machine/partition.h>
#समावेश "string.h"
#समावेश "direct_utils.h"
#समावेश "serial.h"
#समावेश "glo.h"

#अगर USE_SYSDEBUG
#घोषणा MULTIBOOT_VERBOSE 1
#पूर्ण_अगर

/* to-be-built kinfo struct, diagnostics buffer */
kinfo_t kinfo;
काष्ठा kmessages kmessages;

/* pg_utils.c uses this; in this phase, there is a 1:1 mapping. */
phys_bytes vir2phys(व्योम *addr) { वापस (phys_bytes) addr; } 

/* mb_utils.c uses this; we can reach it directly */
अक्षर *video_mem = (अक्षर *) MULTIBOOT_VIDEO_बफर;

/* String length used for mb_itoa */
#घोषणा ITOA_बफर_आकार 20

/* Kernel may use memory */
पूर्णांक kernel_may_alloc = 1;

अटल पूर्णांक mb_set_param(अक्षर *bigbuf, अक्षर *name, अक्षर *मूल्य, kinfo_t *cbi) 
{
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
			जबतक (*q) q++;
			क्रम (q++; q < bufend; q++, p++)
				*p = *q;
			अवरोध;
		}
		जबतक (*p++)
			;
		p++;
	}
	
	क्रम (p = bigbuf; p < bufend && (*p || *(p + 1)); p++)
		;
	अगर (p > bigbuf) p++;
	
	/* Make sure there's enough space for the new parameter */
	अगर (p + namelen + valuelen + 3 > bufend)
		वापस -1;
	
	म_नकल(p, name);
	p[namelen] = '=';
	म_नकल(p + namelen + 1, मूल्य);
	p[namelen + valuelen + 1] = 0;
	p[namelen + valuelen + 2] = 0;
	वापस 0;
}

पूर्णांक overlaps(multiboot_module_t *mod, पूर्णांक n, पूर्णांक cmp_mod)
{
	multiboot_module_t *cmp = &mod[cmp_mod];
	पूर्णांक m;

#घोषणा INRANGE(mod, v) ((v) >= mod->mod_start && (v) < mod->mod_end)
#घोषणा OVERLAP(mod1, mod2) (INRANGE(mod1, mod2->mod_start) || \
			INRANGE(mod1, mod2->mod_end-1))
	क्रम(m = 0; m < n; m++) {
		multiboot_module_t *thismod = &mod[m];
		अगर(m == cmp_mod) जारी;
		अगर(OVERLAP(thismod, cmp))
			वापस 1;
	}
	वापस 0;
}

व्योम get_parameters(u32_t ebx, kinfo_t *cbi) 
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
	स_नकल((व्योम *) mbi, (व्योम *) ebx, माप(*mbi));

	/* Set various bits of info for the higher-level kernel. */
	cbi->mem_ऊपरी_phys = 0;
	cbi->user_sp = (vir_bytes) &_kern_vir_base;
	cbi->vir_kern_start = (vir_bytes) &_kern_vir_base;
	cbi->bootstrap_start = (vir_bytes) &_kern_unpaged_start;
	cbi->bootstrap_len = (vir_bytes) &_kern_unpaged_end -
		cbi->bootstrap_start;
	cbi->kmess = &kmess;

	/* set some configurable defaults */
	cbi->do_serial_debug = 0;
	cbi->serial_debug_baud = 115200;

	/* parse boot command line */
	अगर (mbi->mi_flags & MULTIBOOT_INFO_HAS_CMDLINE) {
		अटल अक्षर var[BUF];
		अटल अक्षर मूल्य[BUF];

		/* Override values with cmdline argument */
		स_नकल(cmdline, (व्योम *) mbi->mi_cmdline, BUF);
		p = cmdline;
		जबतक (*p) {
			var_i = 0;
			मूल्य_i = 0;
			जबतक (*p == ' ') p++;
			अगर (!*p) अवरोध;
			जबतक (*p && *p != '=' && *p != ' ' && var_i < BUF - 1) 
				var[var_i++] = *p++ ;
			var[var_i] = 0;
			अगर (*p++ != '=') जारी; /* skip if not name=value */
			जबतक (*p && *p != ' ' && मूल्य_i < BUF - 1) 
				मूल्य[मूल्य_i++] = *p++ ;
			मूल्य[मूल्य_i] = 0;
			
			mb_set_param(cbi->param_buf, var, मूल्य, cbi);
		}
	}

        /* let higher levels know what we are booting on */
        mb_set_param(cbi->param_buf, ARCHVARNAME, (अक्षर *)get_board_arch_name(BOARD_ID_INTEL), cbi);
	mb_set_param(cbi->param_buf, BOARDVARNAME,(अक्षर *)get_board_name(BOARD_ID_INTEL) , cbi);

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

	निश्चित(!(cbi->bootstrap_start % I386_PAGE_आकार));
	cbi->bootstrap_len = rounddown(cbi->bootstrap_len, I386_PAGE_आकार);
	निश्चित(mbi->mi_flags & MULTIBOOT_INFO_HAS_MODS);
	निश्चित(mbi->mi_mods_गिनती < MULTIBOOT_MAX_MODS);
	निश्चित(mbi->mi_mods_गिनती > 0);
	स_नकल(&cbi->module_list, (व्योम *) mbi->mi_mods_addr,
		mbi->mi_mods_गिनती * माप(multiboot_module_t));
	
	स_रखो(cbi->memmap, 0, माप(cbi->memmap));
	/* mem_map has a variable layout */
	अगर(mbi->mi_flags & MULTIBOOT_INFO_HAS_MMAP) {
		cbi->mmap_आकार = 0;
	        क्रम (mmap = (multiboot_memory_map_t *) mbi->mmap_addr;
       	     (अचिन्हित दीर्घ) mmap < mbi->mmap_addr + mbi->mmap_लंबाई;
       	       mmap = (multiboot_memory_map_t *) 
		      	((अचिन्हित दीर्घ) mmap + mmap->mm_आकार + माप(mmap->mm_आकार))) {
			अगर(mmap->mm_type != MULTIBOOT_MEMORY_AVAILABLE) जारी;
			add_memmap(cbi, mmap->mm_base_addr, mmap->mm_लंबाई);
		}
	} अन्यथा {
		निश्चित(mbi->mi_flags & MULTIBOOT_INFO_HAS_MEMORY);
		add_memmap(cbi, 0, mbi->mi_mem_lower*1024);
		add_memmap(cbi, 0x100000, mbi->mi_mem_upper*1024);
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

kinfo_t *pre_init(u32_t magic, u32_t ebx)
{
	निश्चित(magic == MULTIBOOT_INFO_MAGIC);

	/* Get our own copy boot params pointed to by ebx.
	 * Here we find out whether we should do serial output.
	 */
	get_parameters(ebx, &kinfo);

	/* Make and load a pagetable that will map the kernel
	 * to where it should be; but first a 1:1 mapping so
	 * this code stays where it should be.
	 */
	pg_clear();
	pg_identity(&kinfo);
	kinfo.freepde_start = pg_mapkernel();
	pg_load();
	vm_enable_paging();

	/* Done, return boot info so it can be passed to kmain(). */
	वापस &kinfo;
}

व्योम send_diag_sig(व्योम) { }
व्योम minix_shutdown(पूर्णांक how) { arch_shutdown(how); }
व्योम busy_delay_ms(पूर्णांक x) { }
पूर्णांक उठाओ(पूर्णांक sig) { panic("raise(%d)\n", sig); }
