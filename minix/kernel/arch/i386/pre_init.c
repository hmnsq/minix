
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

#만약 USE_SYSDEBUG
#define MULTIBOOT_VERBOSE 1
#endif

/* to-be-built kinfo struct, diagnostics buffer */
kinfo_t kinfo;
struct kmessages kmessages;

/* pg_utils.c uses this; in this phase, there is a 1:1 mapping. */
phys_bytes vir2phys(void *addr) { 반환 (phys_bytes) addr; } 

/* mb_utils.c uses this; we can reach it directly */
char *video_mem = (char *) MULTIBOOT_VIDEO_BUFFER;

/* String length used for mb_itoa */
#define ITOA_BUFFER_SIZE 20

/* Kernel may use memory */
정수 kernel_may_alloc = 1;

static 정수 mb_set_param(char *bigbuf, char *name, char *value, kinfo_t *cbi) 
{
	char *p = bigbuf;
	char *bufend = bigbuf + MULTIBOOT_PARAM_BUF_SIZE;
	char *q;
	정수 namelen = strlen(name);
	정수 valuelen = strlen(value);

	/* Some variables we recognize */
	만약(!strcmp(name, SERVARNAME)) { cbi->do_serial_debug = 1; }
	만약(!strcmp(name, SERBAUDVARNAME)) { cbi->serial_debug_baud = atoi(value); }

	/* Delete the item if already exists */
	동안 (*p) {
		만약 (strncmp(p, name, namelen) == 0 && p[namelen] == '=') {
			q = p;
			동안 (*q) q++;
			위해 (q++; q < bufend; q++, p++)
				*p = *q;
			break;
		}
		동안 (*p++)
			;
		p++;
	}
	
	위해 (p = bigbuf; p < bufend && (*p || *(p + 1)); p++)
		;
	만약 (p > bigbuf) p++;
	
	/* Make sure there's enough space for the new parameter */
	만약 (p + namelen + valuelen + 3 > bufend)
		반환 -1;
	
	strcpy(p, name);
	p[namelen] = '=';
	strcpy(p + namelen + 1, value);
	p[namelen + valuelen + 1] = 0;
	p[namelen + valuelen + 2] = 0;
	반환 0;
}

정수 overlaps(multiboot_module_t *mod, 정수 n, 정수 cmp_mod)
{
	multiboot_module_t *cmp = &mod[cmp_mod];
	정수 m;

#define INRANGE(mod, v) ((v) >= mod->mod_start && (v) < mod->mod_end)
#define OVERLAP(mod1, mod2) (INRANGE(mod1, mod2->mod_start) || \
			INRANGE(mod1, mod2->mod_end-1))
	위해(m = 0; m < n; m++) {
		multiboot_module_t *thismod = &mod[m];
		만약(m == cmp_mod) continue;
		만약(OVERLAP(thismod, cmp))
			반환 1;
	}
	반환 0;
}

void get_parameters(u32_t ebx, kinfo_t *cbi) 
{
	multiboot_memory_map_t *mmap;
	multiboot_info_t *mbi = &cbi->mbi;
	정수 var_i,value_i, m, k;
	char *p;
	extern char _kern_phys_base, _kern_vir_base, _kern_size,
		_kern_unpaged_start, _kern_unpaged_end;
	phys_bytes kernbase = (phys_bytes) &_kern_phys_base,
		kernsize = (phys_bytes) &_kern_size;
#define BUF 1024
	static char cmdline[BUF];

	/* get our own copy of the multiboot info struct and module list */
	memcpy((void *) mbi, (void *) ebx, sizeof(*mbi));

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
	만약 (mbi->mi_flags & MULTIBOOT_INFO_HAS_CMDLINE) {
		static char var[BUF];
		static char value[BUF];

		/* Override values with cmdline argument */
		memcpy(cmdline, (void *) mbi->mi_cmdline, BUF);
		p = cmdline;
		동안 (*p) {
			var_i = 0;
			value_i = 0;
			동안 (*p == ' ') p++;
			만약 (!*p) break;
			동안 (*p && *p != '=' && *p != ' ' && var_i < BUF - 1) 
				var[var_i++] = *p++ ;
			var[var_i] = 0;
			만약 (*p++ != '=') continue; /* skip if not name=value */
			동안 (*p && *p != ' ' && value_i < BUF - 1) 
				value[value_i++] = *p++ ;
			value[value_i] = 0;
			
			mb_set_param(cbi->param_buf, var, value, cbi);
		}
	}

        /* let higher levels know what we are booting on */
        mb_set_param(cbi->param_buf, ARCHVARNAME, (char *)get_board_arch_name(BOARD_ID_INTEL), cbi);
	mb_set_param(cbi->param_buf, BOARDVARNAME,(char *)get_board_name(BOARD_ID_INTEL) , cbi);

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
		mbi->mi_mods_count * sizeof(multiboot_module_t));
	
	memset(cbi->memmap, 0, sizeof(cbi->memmap));
	/* mem_map has a variable layout */
	만약(mbi->mi_flags & MULTIBOOT_INFO_HAS_MMAP) {
		cbi->mmap_size = 0;
	        위해 (mmap = (multiboot_memory_map_t *) mbi->mmap_addr;
       	     (unsigned long) mmap < mbi->mmap_addr + mbi->mmap_length;
       	       mmap = (multiboot_memory_map_t *) 
		      	((unsigned long) mmap + mmap->mm_size + sizeof(mmap->mm_size))) {
			만약(mmap->mm_type != MULTIBOOT_MEMORY_AVAILABLE) continue;
			add_memmap(cbi, mmap->mm_base_addr, mmap->mm_length);
		}
	} 아니면 {
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

	위해(m = 0; m < cbi->mods_with_kernel; m++) {
#만약 0
		printf("checking overlap of module %08lx-%08lx\n",
		  cbi->module_list[m].mod_start, cbi->module_list[m].mod_end);
#endif
		만약(overlaps(cbi->module_list, cbi->mods_with_kernel, m))
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
	반환 &kinfo;
}

void send_diag_sig(void) { }
void minix_shutdown(정수 how) { arch_shutdown(how); }
void busy_delay_ms(정수 x) { }
정수 raise(정수 sig) { panic("raise(%d)\n", sig); }
