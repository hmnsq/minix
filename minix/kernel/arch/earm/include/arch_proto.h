
#ifndef _ARM_PROTO_H
#define _ARM_PROTO_H

#شامل <machine/vm.h>

#define K_پشته_SIZE	ARM_PAGE_SIZE


#ifndef __ASSEMBLY__

#شامل "cpufunc.h"

/* klib */
__dead تهی reset(تهی);
phys_bytes vir2phys(تهی *);
vir_bytes phys_memset(phys_bytes ph, u32_t c, phys_bytes bytes);

تهی __switch_address_space(ساختار proc *p, ساختار proc **__ptproc);
#define switch_address_space(proc)	\
	__switch_address_space(proc, get_cpulocal_var_ptr(ptproc))

تهی __copy_msg_from_user_end(تهی);
تهی __copy_msg_to_user_end(تهی);
تهی __user_copy_msg_pointer_failure(تهی);

/* multiboot.c */
تهی multiboot_init(تهی);

/* protect.c */
ساختار tss_s {
  reg_t sp0;                    /* stack pointer to use during interrupt */
} __attribute__((packed));
صحیح tss_init(unsigned cpu, تهی * kernel_پشته);

تهی add_memmap(kinfo_t *cbi, u64_t addr, u64_t len);
phys_bytes alloc_lowest(kinfo_t *cbi, phys_bytes len);
تهی vm_enable_paging(تهی);
تهی cut_memmap(kinfo_t *cbi, phys_bytes start, phys_bytes end);
phys_bytes pg_roundup(phys_bytes b);
تهی pg_info(reg_t *, u32_t **);
تهی pg_clear(تهی);
تهی pg_identity(kinfo_t *);
phys_bytes pg_load(تهی);
تهی pg_map(phys_bytes phys, vir_bytes vaddr, vir_bytes vaddr_end, kinfo_t *cbi);
صحیح pg_mapkernel(تهی);
تهی pg_mapproc(ساختار proc *p, ساختار boot_image *ip, kinfo_t *cbi);

EXTERN تهی * k_stacks_start;
بیرونی تهی * k_stacks;

#define get_k_پشته_بالاترین(cpu)	((تهی *)(((نویسه*)(k_stacks)) \
					+ 2 * ((cpu) + 1) * K_پشته_SIZE))


/*
 * Definition of a callback used when a memory map changed it's base address
 */
تعریفنوع صحیح (*kern_phys_map_mapped)(vir_bytes id, vir_bytes new_addr );

/*
 * struct used internally by memory.c to keep a list of
 * items to map. These should be statically allocated
 * in the individual files and passed as argument. 
 * The data doesn't need to be initialized. See omap_serial for
 * and example usage.
 */
تعریفنوع ساختار kern_phys_map{
	phys_bytes addr; /* The physical address to map */
	vir_bytes size;  /* The size of the mapping */
	vir_bytes id;	 /* an id passed to the callback */
	صحیح vm_flags;	 /* flags to be passed to vm map */
	kern_phys_map_mapped cb; /* the callback itself */
	phys_bytes vir; /* The virtual address once remapped */
	صحیح index; 	/* index */
	ساختار kern_phys_map *next; /* pointer to the next */
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
صحیح kern_req_phys_map( phys_bytes base_address, vir_bytes io_size,
		   صحیح vm_flags, kern_phys_map * priv,
		   kern_phys_map_mapped cb, vir_bytes id);

/*
 * Request a physical mapping and put the result in the given prt
 * Note that ptr will only be valid once the callback happened.
 */
صحیح kern_phys_map_ptr( phys_bytes base_address, vir_bytes io_size, 
		       صحیح vm_flags, kern_phys_map * priv, 
		       vir_bytes ptr);

تهی arch_ser_init(تهی);

/* functions defined in architecture-independent kernel source. */
#شامل "kernel/proto.h"

#endif /* __ASSEMBLY__ */

#endif
