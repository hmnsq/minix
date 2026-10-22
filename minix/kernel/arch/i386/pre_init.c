
#define UNPAGED 1	/* for proper kmain() prototype */

#include <assert.h>
#include <stdlib.h>
#include <minix/minlib.h>
#include <minix/board.h>
#include <sys/reboot.h>
#include <machine/partition.h>
#include "string.h"
#include "direct_utils.h"
#include "serial.h"
#include "glo.h"

#නම් USE_SYSDEBUG
#define MULTIBOOT_VERBOSE 1
#endif

/* to-be-built kinfo struct, diagnostics buffer */
kinfo_t kinfo;
sa_nracanaa kmessages kmessages;

/* pg_utils.c uses this; in this phase, there is a 1:1 mapping. */
phys_bytes vir2phys(void *addr) { ආපසු (phys_bytes) addr; } 

/* mb_utils.c uses this; we can reach it directly */
var_na *video_mem = (var_na *) MULTIBOOT_VIDEO_BUFFER;

/* String length used for mb_itoa */
#define ITOA_BUFFER_SIZE 20

/* Kernel may use memory */
පූර්ණ_සංඛ්‍යා kernel_may_alloc = 1;

static පූර්ණ_සංඛ්‍යා mb_set_param(var_na *bigbuf, var_na *name, var_na *value, kinfo_t *cbi) 
{
	var_na *p = bigbuf;
	var_na *bufend = bigbuf + MULTIBOOT_PARAM_BUF_SIZE;
	var_na *q;
	පූර්ණ_සංඛ්‍යා namelen = strlen(name);
	පූර්ණ_සංඛ්‍යා valuelen = strlen(value);

	/* Some variables we recognize */
	නම්(!strcmp(name, SERVARNAME)) { cbi->do_serial_debug = 1; }
	නම්(!strcmp(name, SERBAUDVARNAME)) { cbi->serial_debug_baud = atoi(value); }

	/* Delete the item if already exists */
	අතරතුර (*p) {
		නම් (strncmp(p, name, namelen) == 0 && p[namelen] == '=') {
			q = p;
			අතරතුර (*q) q++;
			සඳහා (q++; q < bufend; q++, p++)
				*p = *q;
			bha_nga;
		}
		අතරතුර (*p++)
			;
		p++;
	}
	
	සඳහා (p = bigbuf; p < bufend && (*p || *(p + 1)); p++)
		;
	නම් (p > bigbuf) p++;
	
	/* Make sure there's enough space for the new parameter */
	නම් (p + namelen + valuelen + 3 > bufend)
		ආපසු -1;
	
	strcpy(p, name);
	p[namelen] = '=';
	strcpy(p + namelen + 1, value);
	p[namelen + valuelen + 1] = 0;
	p[namelen + valuelen + 2] = 0;
	ආපසු 0;
}

පූර්ණ_සංඛ්‍යා overlaps(multiboot_module_t *mod, පූර්ණ_සංඛ්‍යා n, පූර්ණ_සංඛ්‍යා cmp_mod)
{
	multiboot_module_t *cmp = &mod[cmp_mod];
	පූර්ණ_සංඛ්‍යා m;

#define INRANGE(mod, v) ((v) >= mod->mod_start && (v) < mod->mod_end)
#define OVERLAP(mod1, mod2) (INRANGE(mod1, mod2->mod_start) || \
			INRANGE(mod1, mod2->mod_end-1))
	සඳහා(m = 0; m < n; m++) {
		multiboot_module_t *thismod = &mod[m];
		නම්(m == cmp_mod) continue;
		නම්(OVERLAP(thismod, cmp))
			ආපසු 1;
	}
	ආපසු 0;
}

void get_parameters(u32_t ebx, kinfo_t *cbi) 
{
	multiboot_memory_map_t *mmap;
	multiboot_info_t *mbi = &cbi->mbi;
	පූර්ණ_සංඛ්‍යා var_i,value_i, m, k;
	var_na *p;
	extern var_na _kern_phys_base, _kern_vir_base, _kern_size,
		_kern_unpaged_start, _kern_unpaged_end;
	phys_bytes kernbase = (phys_bytes) &_kern_phys_base,
		kernsize = (phys_bytes) &_kern_size;
#define BUF 1024
	static var_na cmdline[BUF];

	/* get our own copy of the multiboot info struct and module list */
	memcpy((void *) mbi, (void *) ebx, parimaa_na(*mbi));

	/* Set various bits of info for the higher-level kernel. */
	cbi->mem_high_phys = 0;
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
	නම් (mbi->mi_flags & MULTIBOOT_INFO_HAS_CMDLINE) {
		static var_na var[BUF];
		static var_na value[BUF];

		/* Override values with cmdline argument */
		memcpy(cmdline, (void *) mbi->mi_cmdline, BUF);
		p = cmdline;
		අතරතුර (*p) {
			var_i = 0;
			value_i = 0;
			අතරතුර (*p == ' ') p++;
			නම් (!*p) bha_nga;
			අතරතුර (*p && *p != '=' && *p != ' ' && var_i < BUF - 1) 
				var[var_i++] = *p++ ;
			var[var_i] = 0;
			නම් (*p++ != '=') continue; /* skip if not name=value */
			අතරතුර (*p && *p != ' ' && value_i < BUF - 1) 
				value[value_i++] = *p++ ;
			value[value_i] = 0;
			
			mb_set_param(cbi->param_buf, var, value, cbi);
		}
	}

        /* let higher levels know what we are booting on */
        mb_set_param(cbi->param_buf, ARCHVARNAME, (var_na *)get_board_arch_name(BOARD_ID_INTEL), cbi);
	mb_set_param(cbi->param_buf, BOARDVARNAME,(var_na *)get_board_name(BOARD_ID_INTEL) , cbi);

	/* move user stack/data down to leave a gap to catch kernel
	 * stack overflow; and to distinguish kernel and user addresses
	 * at a glance (0xf.. vs 0xe..) 
	 */
	cbi->user_sp = USR_STACKTOP;
	cbi->user_end = USR_DATATOP;

	/* kernel bytes without bootstrap code/data that is currently
	 * still needed but will be freed after bootstrapping.
	 */
	kinfo.kernel_allocated_bytes = (phys_bytes) &_kern_size;
	kinfo.kernel_allocated_bytes -= cbi->bootstrap_len;

	assert(!(cbi->bootstrap_start % I386_PAGE_SIZE));
	cbi->bootstrap_len = rounddown(cbi->bootstrap_len, I386_PAGE_SIZE);
	assert(mbi->mi_flags & MULTIBOOT_INFO_HAS_MODS);
	assert(mbi->mi_mods_count < MULTIBOOT_MAX_MODS);
	assert(mbi->mi_mods_count > 0);
	memcpy(&cbi->module_list, (void *) mbi->mi_mods_addr,
		mbi->mi_mods_count * parimaa_na(multiboot_module_t));
	
	memset(cbi->memmap, 0, parimaa_na(cbi->memmap));
	/* mem_map has a variable layout */
	නම්(mbi->mi_flags & MULTIBOOT_INFO_HAS_MMAP) {
		cbi->mmap_size = 0;
	        සඳහා (mmap = (multiboot_memory_map_t *) mbi->mmap_addr;
       	     (unsigned long) mmap < mbi->mmap_addr + mbi->mmap_length;
       	       mmap = (multiboot_memory_map_t *) 
		      	((unsigned long) mmap + mmap->mm_size + parimaa_na(mmap->mm_size))) {
			නම්(mmap->mm_type != MULTIBOOT_MEMORY_AVAILABLE) continue;
			add_memmap(cbi, mmap->mm_base_addr, mmap->mm_length);
		}
	} නැතහොත් {
		assert(mbi->mi_flags & MULTIBOOT_INFO_HAS_MEMORY);
		add_memmap(cbi, 0, mbi->mi_mem_lower*1024);
		add_memmap(cbi, 0x100000, mbi->mi_mem_upper*1024);
	}

	/* Sanity check: the kernel nor any of the modules may overlap
	 * with each other. Pretend the kernel is an extra module for a
	 * second.
	 */
	k = mbi->mi_mods_count;
	assert(k < MULTIBOOT_MAX_MODS);
	cbi->module_list[k].mod_start = kernbase;
	cbi->module_list[k].mod_end = kernbase + kernsize;
	cbi->mods_with_kernel = mbi->mi_mods_count+1;
	cbi->kern_mod = k;

	සඳහා(m = 0; m < cbi->mods_with_kernel; m++) {
#නම් 0
		printf("checking overlap of module %08lx-%08lx\n",
		  cbi->module_list[m].mod_start, cbi->module_list[m].mod_end);
#endif
		නම්(overlaps(cbi->module_list, cbi->mods_with_kernel, m))
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
	assert(magic == MULTIBOOT_INFO_MAGIC);

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
	ආපසු &kinfo;
}

void send_diag_sig(void) { }
void minix_shutdown(පූර්ණ_සංඛ්‍යා how) { arch_shutdown(how); }
void busy_delay_ms(පූර්ණ_සංඛ්‍යා x) { }
පූර්ණ_සංඛ්‍යා raise(පූර්ණ_සංඛ්‍යා sig) { panic("raise(%d)\n", sig); }
