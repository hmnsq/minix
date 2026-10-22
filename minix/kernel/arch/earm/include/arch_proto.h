
#ifndef _ARM_PROTO_H
#define _ARM_PROTO_H

#அந்தர்பாவ <machine/vm.h>

#define K_அடுக்கு_SIZE	ARM_PAGE_SIZE


#ifndef __ASSEMBLY__

#அந்தர்பாவ "cpufunc.h"

/* klib */
__dead வெறுமை reset(வெறுமை);
phys_bytes vir2phys(வெறுமை *);
vir_bytes phys_memset(phys_bytes ph, u32_t c, phys_bytes bytes);

வெறுமை __switch_address_space(கட்டமைப்பு proc *p, கட்டமைப்பு proc **__ptproc);
#define switch_address_space(proc)	\
	__switch_address_space(proc, get_cpulocal_var_ptr(ptproc))

வெறுமை __copy_msg_from_user_end(வெறுமை);
வெறுமை __copy_msg_to_user_end(வெறுமை);
வெறுமை __user_copy_msg_pointer_failure(வெறுமை);

/* multiboot.c */
வெறுமை multiboot_init(வெறுமை);

/* protect.c */
கட்டமைப்பு tss_s {
  reg_t sp0;                    /* stack pointer to use during interrupt */
} __attribute__((packed));
முழு tss_init(unsigned cpu, வெறுமை * kernel_அடுக்கு);

வெறுமை add_memmap(kinfo_t *cbi, u64_t addr, u64_t len);
phys_bytes alloc_lowest(kinfo_t *cbi, phys_bytes len);
வெறுமை vm_enable_paging(வெறுமை);
வெறுமை cut_memmap(kinfo_t *cbi, phys_bytes start, phys_bytes end);
phys_bytes pg_roundup(phys_bytes b);
வெறுமை pg_info(reg_t *, u32_t **);
வெறுமை pg_clear(வெறுமை);
வெறுமை pg_identity(kinfo_t *);
phys_bytes pg_load(வெறுமை);
வெறுமை pg_map(phys_bytes phys, vir_bytes vaddr, vir_bytes vaddr_end, kinfo_t *cbi);
முழு pg_mapkernel(வெறுமை);
வெறுமை pg_mapproc(கட்டமைப்பு proc *p, கட்டமைப்பு boot_image *ip, kinfo_t *cbi);

EXTERN வெறுமை * k_stacks_start;
வெளி வெறுமை * k_stacks;

#define get_k_அடுக்கு_உச்சி(cpu)	((வெறுமை *)(((எழுத்து*)(k_stacks)) \
					+ 2 * ((cpu) + 1) * K_அடுக்கு_SIZE))


/*
 * Definition of a callback used when a memory map changed it's base address
 */
வகைவரை முழு (*kern_phys_map_mapped)(vir_bytes id, vir_bytes new_addr );

/*
 * struct used internally by memory.c to keep a list of
 * items to map. These should be statically allocated
 * in the individual files and passed as argument. 
 * The data doesn't need to be initialized. See omap_serial for
 * and example usage.
 */
வகைவரை கட்டமைப்பு kern_phys_map{
	phys_bytes addr; /* The physical address to map */
	vir_bytes size;  /* The size of the mapping */
	vir_bytes id;	 /* an id passed to the callback */
	முழு vm_flags;	 /* flags to be passed to vm map */
	kern_phys_map_mapped cb; /* the callback itself */
	phys_bytes vir; /* The virtual address once remapped */
	முழு index; 	/* index */
	கட்டமைப்பு kern_phys_map *next; /* pointer to the next */
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
முழு kern_req_phys_map( phys_bytes base_address, vir_bytes io_size,
		   முழு vm_flags, kern_phys_map * priv,
		   kern_phys_map_mapped cb, vir_bytes id);

/*
 * Request a physical mapping and put the result in the given prt
 * Note that ptr will only be valid once the callback happened.
 */
முழு kern_phys_map_ptr( phys_bytes base_address, vir_bytes io_size, 
		       முழு vm_flags, kern_phys_map * priv, 
		       vir_bytes ptr);

வெறுமை arch_ser_init(வெறுமை);

/* functions defined in architecture-independent kernel source. */
#அந்தர்பாவ "kernel/proto.h"

#endif /* __ASSEMBLY__ */

#endif
