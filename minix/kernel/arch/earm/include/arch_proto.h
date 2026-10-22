
#ifndef _ARM_PROTO_H
#define _ARM_PROTO_H

#ଅନ୍ତର୍ଭାଵ <machine/vm.h>

#define K_ଗଦା_SIZE	ARM_PAGE_SIZE


#ifndef __ASSEMBLY__

#ଅନ୍ତର୍ଭାଵ "cpufunc.h"

/* klib */
__dead ଶୂନ୍ୟ reset(ଶୂନ୍ୟ);
phys_bytes vir2phys(ଶୂନ୍ୟ *);
vir_bytes phys_memset(phys_bytes ph, u32_t c, phys_bytes bytes);

ଶୂନ୍ୟ __switch_address_space(ସଂରଚନା proc *p, ସଂରଚନା proc **__ptproc);
#define switch_address_space(proc)	\
	__switch_address_space(proc, get_cpulocal_var_ptr(ptproc))

ଶୂନ୍ୟ __copy_msg_from_user_end(ଶୂନ୍ୟ);
ଶୂନ୍ୟ __copy_msg_to_user_end(ଶୂନ୍ୟ);
ଶୂନ୍ୟ __user_copy_msg_pointer_failure(ଶୂନ୍ୟ);

/* multiboot.c */
ଶୂନ୍ୟ multiboot_init(ଶୂନ୍ୟ);

/* protect.c */
ସଂରଚନା tss_s {
  reg_t sp0;                    /* stack pointer to use during interrupt */
} __attribute__((packed));
ପୂର୍ଣ୍ଣସଂଖ୍ୟା tss_init(unsigned cpu, ଶୂନ୍ୟ * kernel_ଗଦା);

ଶୂନ୍ୟ add_memmap(kinfo_t *cbi, u64_t addr, u64_t len);
phys_bytes alloc_lowest(kinfo_t *cbi, phys_bytes len);
ଶୂନ୍ୟ vm_enable_paging(ଶୂନ୍ୟ);
ଶୂନ୍ୟ cut_memmap(kinfo_t *cbi, phys_bytes start, phys_bytes end);
phys_bytes pg_roundup(phys_bytes b);
ଶୂନ୍ୟ pg_info(reg_t *, u32_t **);
ଶୂନ୍ୟ pg_clear(ଶୂନ୍ୟ);
ଶୂନ୍ୟ pg_identity(kinfo_t *);
phys_bytes pg_load(ଶୂନ୍ୟ);
ଶୂନ୍ୟ pg_map(phys_bytes phys, vir_bytes vaddr, vir_bytes vaddr_end, kinfo_t *cbi);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା pg_mapkernel(ଶୂନ୍ୟ);
ଶୂନ୍ୟ pg_mapproc(ସଂରଚନା proc *p, ସଂରଚନା boot_image *ip, kinfo_t *cbi);

EXTERN ଶୂନ୍ୟ * k_stacks_start;
ବାହ୍ୟ ଶୂନ୍ୟ * k_stacks;

#define get_k_ଗଦା_ଶୀର୍ଷ(cpu)	((ଶୂନ୍ୟ *)(((ଅକ୍ଷର*)(k_stacks)) \
					+ 2 * ((cpu) + 1) * K_ଗଦା_SIZE))


/*
 * Definition of a callback used when a memory map changed it's base address
 */
ପ୍ରକାରବ୍ୟାଖ୍ୟା ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*kern_phys_map_mapped)(vir_bytes id, vir_bytes new_addr );

/*
 * struct used internally by memory.c to keep a list of
 * items to map. These should be statically allocated
 * in the individual files and passed as argument. 
 * The data doesn't need to be initialized. See omap_serial for
 * and example usage.
 */
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା kern_phys_map{
	phys_bytes addr; /* The physical address to map */
	vir_bytes size;  /* The size of the mapping */
	vir_bytes id;	 /* an id passed to the callback */
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା vm_flags;	 /* flags to be passed to vm map */
	kern_phys_map_mapped cb; /* the callback itself */
	phys_bytes vir; /* The virtual address once remapped */
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା index; 	/* index */
	ସଂରଚନା kern_phys_map *next; /* pointer to the next */
} kern_phys_map ; 


/*
 * Request an in kernel physical mapping.
 * 
 * On ARM many devices are memory mapped and some of these devices
 * are used in the kernel. These device can be things like serial 
 * lines, interrupt controller and clocks. The kernel needs to be 
 * able to access these devices at the various stages of booting. 
 * During startup, until arch_enable_paging is called, it is the 
 * kernel whom is controlling the mappings and it often needs to 
 * access the memory using a 1:1 mapping between virtual and 
 * physical memory.
 * 
 * Once processes start to run it is no longer desirable for the 
 * kernel to have devices mapped in the middle of the process
 * address space. 
 *
 * This method requests the memory manager to map base_address/size 
 * in the kernel address space and call back the kernel when this 
 * mapping takes effect (after enable_paging).
 *
 * Before the callback is called it is up to the kernel to use it's
 * own addressing. The callback will happen *after* the kernel lost
 * it's initial mapping. It it therefore not safe to use the initial
 * mapping in the callback. It also is not possible to use printf for
 * the same reason.
 */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା kern_req_phys_map( phys_bytes base_address, vir_bytes io_size,
		   ପୂର୍ଣ୍ଣସଂଖ୍ୟା vm_flags, kern_phys_map * priv,
		   kern_phys_map_mapped cb, vir_bytes id);

/*
 * Request a physical mapping and put the result in the given prt
 * Note that ptr will only be valid once the callback happened.
 */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା kern_phys_map_ptr( phys_bytes base_address, vir_bytes io_size, 
		       ପୂର୍ଣ୍ଣସଂଖ୍ୟା vm_flags, kern_phys_map * priv, 
		       vir_bytes ptr);

ଶୂନ୍ୟ arch_ser_init(ଶୂନ୍ୟ);

/* functions defined in architecture-independent kernel source. */
#ଅନ୍ତର୍ଭାଵ "kernel/proto.h"

#endif /* __ASSEMBLY__ */

#endif
