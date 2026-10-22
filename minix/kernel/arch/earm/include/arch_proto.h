
#ifndef _ARM_PROTO_H
#define _ARM_PROTO_H

#अन्तर्भाव <machine/vm.h>

#define K_ढेर_SIZE	ARM_PAGE_SIZE


#ifndef __ASSEMBLY__

#अन्तर्भाव "cpufunc.h"

/* klib */
__dead शून्य reset(शून्य);
phys_bytes vir2phys(शून्य *);
vir_bytes phys_memset(phys_bytes ph, u32_t c, phys_bytes bytes);

शून्य __switch_address_space(संरचना proc *p, संरचना proc **__ptproc);
#define switch_address_space(proc)	\
	__switch_address_space(proc, get_cpulocal_var_ptr(ptproc))

शून्य __copy_msg_from_user_end(शून्य);
शून्य __copy_msg_to_user_end(शून्य);
शून्य __user_copy_msg_pointer_failure(शून्य);

/* multiboot.c */
शून्य multiboot_init(शून्य);

/* protect.c */
संरचना tss_s {
  reg_t sp0;                    /* stack pointer to use during interrupt */
} __attribute__((packed));
पूर्णाङ्क tss_init(अचिन्हित cpu, शून्य * kernel_ढेर);

शून्य add_memmap(kinfo_t *cbi, u64_t addr, u64_t len);
phys_bytes alloc_lowest(kinfo_t *cbi, phys_bytes len);
शून्य vm_enable_paging(शून्य);
शून्य cut_memmap(kinfo_t *cbi, phys_bytes start, phys_bytes end);
phys_bytes pg_roundup(phys_bytes b);
शून्य pg_info(reg_t *, u32_t **);
शून्य pg_clear(शून्य);
शून्य pg_identity(kinfo_t *);
phys_bytes pg_load(शून्य);
शून्य pg_map(phys_bytes phys, vir_bytes vaddr, vir_bytes vaddr_end, kinfo_t *cbi);
पूर्णाङ्क pg_mapkernel(शून्य);
शून्य pg_mapproc(संरचना proc *p, संरचना boot_image *ip, kinfo_t *cbi);

EXTERN शून्य * k_stacks_start;
बाह्य शून्य * k_stacks;

#define get_k_ढेर_शीर्ष(cpu)	((शून्य *)(((वर्ण*)(k_stacks)) \
					+ 2 * ((cpu) + 1) * K_ढेर_SIZE))


/*
 * Definition of a callback used when a memory map changed it's base address
 */
प्रकारनिर्देश पूर्णाङ्क (*kern_phys_map_mapped)(vir_bytes id, vir_bytes new_addr );

/*
 * struct used internally by memory.c to keep a list of
 * items to map. These should be statically allocated
 * in the individual files and passed as argument. 
 * The data doesn't need to be initialized. See omap_serial for
 * and example usage.
 */
प्रकारनिर्देश संरचना kern_phys_map{
	phys_bytes addr; /* The physical address to map */
	vir_bytes size;  /* The size of the mapping */
	vir_bytes id;	 /* an id passed to the callback */
	पूर्णाङ्क vm_flags;	 /* flags to be passed to vm map */
	kern_phys_map_mapped cb; /* the callback itself */
	phys_bytes vir; /* The virtual address once remapped */
	पूर्णाङ्क index; 	/* index */
	संरचना kern_phys_map *next; /* pointer to the next */
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
पूर्णाङ्क kern_req_phys_map( phys_bytes base_address, vir_bytes io_size,
		   पूर्णाङ्क vm_flags, kern_phys_map * priv,
		   kern_phys_map_mapped cb, vir_bytes id);

/*
 * Request a physical mapping and put the result in the given prt
 * Note that ptr will only be valid once the callback happened.
 */
पूर्णाङ्क kern_phys_map_ptr( phys_bytes base_address, vir_bytes io_size, 
		       पूर्णाङ्क vm_flags, kern_phys_map * priv, 
		       vir_bytes ptr);

शून्य arch_ser_init(शून्य);

/* functions defined in architecture-independent kernel source. */
#अन्तर्भाव "kernel/proto.h"

#endif /* __ASSEMBLY__ */

#endif
