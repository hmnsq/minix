
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ _ARM_PROTO_H
#ವ್ಯಾಖ್ಯಾನಿಸು _ARM_PROTO_H

#ಅನ್ತರ್ಭಾವ <machine/vm.h>

#ವ್ಯಾಖ್ಯಾನಿಸು K_ರಾಶಿ_ಗಾತ್ರ	ARM_ಪುಟ_ಗಾತ್ರ


#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ __ASSEMBLY__

#ಅನ್ತರ್ಭಾವ "cpufunc.h"

/* klib */
__dead ಶೂನ್ಯ reset(ಶೂನ್ಯ);
phys_ಬೈಟುಗಳು vir2phys(ಶೂನ್ಯ *);
vir_ಬೈಟುಗಳು phys_memset(phys_ಬೈಟುಗಳು ph, u32_t c, phys_ಬೈಟುಗಳು ಬೈಟುಗಳು);

ಶೂನ್ಯ __switch_address_space(ರಚನೆ proc *p, ರಚನೆ proc **__ptproc);
#ವ್ಯಾಖ್ಯಾನಿಸು switch_ವಿಳಾಸ_space(proc)	\
	__switch_address_space(proc, get_cpulocal_var_ptr(ptproc))

ಶೂನ್ಯ __copy_msg_from_user_end(ಶೂನ್ಯ);
ಶೂನ್ಯ __copy_msg_to_user_end(ಶೂನ್ಯ);
ಶೂನ್ಯ __user_copy_msg_pointer_failure(ಶೂನ್ಯ);

/* multiboot.c */
ಶೂನ್ಯ multiboot_init(ಶೂನ್ಯ);

/* protect.c */
ರಚನೆ tss_s {
  reg_t sp0;                    /* stack pointer to use during interrupt */
} __attribute__((packed));
ಪೂರ್ಣಾಂಕ tss_init(ಚಿಹ್ನೆರಹಿತ cpu, ಶೂನ್ಯ * kernel_ರಾಶಿ);

ಶೂನ್ಯ ಕೂಡಿಸು_memmap(kinfo_t *cbi, u64_t addr, u64_t len);
phys_ಬೈಟುಗಳು alloc_ಅತಿಕಡಿಮೆ(kinfo_t *cbi, phys_ಬೈಟುಗಳು len);
ಶೂನ್ಯ vm_enable_paging(ಶೂನ್ಯ);
ಶೂನ್ಯ cut_memmap(kinfo_t *cbi, phys_ಬೈಟುಗಳು ಆರಂಭ, phys_ಬೈಟುಗಳು ಕೊನೆ);
phys_ಬೈಟುಗಳು pg_roundup(phys_ಬೈಟುಗಳು b);
ಶೂನ್ಯ pg_info(reg_t *, u32_t **);
ಶೂನ್ಯ pg_clear(ಶೂನ್ಯ);
ಶೂನ್ಯ pg_ತಟಸ್ಥ(kinfo_t *);
phys_ಬೈಟುಗಳು pg_ತುಂಬು(ಶೂನ್ಯ);
ಶೂನ್ಯ pg_map(phys_ಬೈಟುಗಳು phys, vir_ಬೈಟುಗಳು vaddr, vir_ಬೈಟುಗಳು vaddr_ಕೊನೆ, kinfo_t *cbi);
ಪೂರ್ಣಾಂಕ pg_mapkernel(ಶೂನ್ಯ);
ಶೂನ್ಯ pg_mapproc(ರಚನೆ proc *p, ರಚನೆ boot_image *ip, kinfo_t *cbi);

EXTERN ಶೂನ್ಯ * k_stacks_ಆರಂಭ;
ಬಾಹ್ಯ ಶೂನ್ಯ * k_stacks;

#ವ್ಯಾಖ್ಯಾನಿಸು get_k_ರಾಶಿ_ತುದಿ(cpu)	((ಶೂನ್ಯ *)(((ಅಕ್ಷರ*)(k_stacks)) \
					+ 2 * ((cpu) + 1) * K_ರಾಶಿ_ಗಾತ್ರ))


/*
 * Definition of a callback used when a memory map changed it's base address
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಪೂರ್ಣಾಂಕ (*kern_phys_map_mapped)(vir_ಬೈಟುಗಳು id, vir_ಬೈಟುಗಳು new_addr );

/*
 * struct used internally by memory.c to keep a list of
 * items to map. These should be statically allocated
 * in the individual files and passed as argument. 
 * The data doesn't need to be initialized. See omap_serial for
 * and example usage.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ kern_phys_map{
	phys_ಬೈಟುಗಳು addr; /* The physical address to map */
	vir_ಬೈಟುಗಳು ಗಾತ್ರ_2;  /* The size of the mapping */
	vir_ಬೈಟುಗಳು id;	 /* an id passed to the callback */
	ಪೂರ್ಣಾಂಕ vm_flags;	 /* flags to be passed to vm map */
	kern_phys_map_mapped cb; /* the callback itself */
	phys_ಬೈಟುಗಳು vir; /* The virtual address once remapped */
	ಪೂರ್ಣಾಂಕ ಸೂಚ್ಯಂಕ; 	/*ಸೂಚ್ಯಂಕ*/
	ರಚನೆ kern_phys_map *ಮುಂದಿನ; /* pointer to the next */
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
ಪೂರ್ಣಾಂಕ kern_req_phys_map( phys_ಬೈಟುಗಳು ತಳ_ವಿಳಾಸ, vir_ಬೈಟುಗಳು io_ಗಾತ್ರ,
		   ಪೂರ್ಣಾಂಕ vm_flags, kern_phys_map * priv,
		   kern_phys_map_mapped cb, vir_ಬೈಟುಗಳು id);

/*
 * Request a physical mapping and put the result in the given prt
 * Note that ptr will only be valid once the callback happened.
 */
ಪೂರ್ಣಾಂಕ kern_phys_map_ptr( phys_ಬೈಟುಗಳು ತಳ_ವಿಳಾಸ, vir_ಬೈಟುಗಳು io_ಗಾತ್ರ, 
		       ಪೂರ್ಣಾಂಕ vm_flags, kern_phys_map * priv, 
		       vir_ಬೈಟುಗಳು ptr);

ಶೂನ್ಯ arch_ser_init(ಶೂನ್ಯ);

/* functions defined in architecture-independent kernel source. */
#ಅನ್ತರ್ಭಾವ "kernel/proto.h"

#ಆದರೆ_ಅಂತ್ಯ /* __ASSEMBLY__ */

#ಆದರೆ_ಅಂತ್ಯ
