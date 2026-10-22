
#که_نه_تعریف_شوی _ARM_PROTO_H
#تعریف_کړه _ARM_PROTO_H

#ضمن <machine/vm.h>

#تعریف_کړه K_ډېری_اندازه	ARM_پاڼه_اندازه


#که_نه_تعریف_شوی __ASSEMBLY__

#ضمن "cpufunc.h"

/* klib */
__dead فراغ reset(فراغ);
phys_بایټونه vir2phys(فراغ *);
vir_بایټونه phys_memset(phys_بایټونه ph, u32_t c, phys_بایټونه بایټونه);

فراغ __switch_address_space(بنية proc *p, بنية proc **__ptproc);
#تعریف_کړه switch_پته_space(proc)	\
	__switch_address_space(proc, get_cpulocal_var_ptr(ptproc))

فراغ __copy_msg_from_user_end(فراغ);
فراغ __copy_msg_to_user_end(فراغ);
فراغ __user_copy_msg_pointer_failure(فراغ);

/* multiboot.c */
فراغ multiboot_init(فراغ);

/* protect.c */
بنية tss_s {
  reg_t sp0;                    /* stack pointer to use during interrupt */
} __attribute__((packed));
صحيح tss_init(بینښه cpu, فراغ * kernel_ډېری);

فراغ جمع_کړه_memmap(kinfo_t *cbi, u64_t addr, u64_t len);
phys_بایټونه alloc_ټیټ_ترین(kinfo_t *cbi, phys_بایټونه len);
فراغ vm_enable_paging(فراغ);
فراغ cut_memmap(kinfo_t *cbi, phys_بایټونه پیل, phys_بایټونه پای_ټکی);
phys_بایټونه pg_roundup(phys_بایټونه b);
فراغ pg_info(reg_t *, u32_t **);
فراغ pg_clear(فراغ);
فراغ pg_بې_اغېزه_عنصر(kinfo_t *);
phys_بایټونه pg_بارول(فراغ);
فراغ pg_map(phys_بایټونه phys, vir_بایټونه vaddr, vir_بایټونه vaddr_پای_ټکی, kinfo_t *cbi);
صحيح pg_mapkernel(فراغ);
فراغ pg_mapproc(بنية proc *p, بنية boot_image *ip, kinfo_t *cbi);

EXTERN فراغ * k_stacks_پیل;
خارجي فراغ * k_stacks;

#تعریف_کړه get_k_ډېری_سر(cpu)	((فراغ *)(((حرف*)(k_stacks)) \
					+ 2 * ((cpu) + 1) * K_ډېری_اندازه))


/*
 * Definition of a callback used when a memory map changed it's base address
 */
تعريفنوع صحيح (*kern_phys_map_mapped)(vir_بایټونه id, vir_بایټونه new_addr );

/*
 * struct used internally by memory.c to keep a list of
 * items to map. These should be statically allocated
 * in the individual files and passed as argument. 
 * The data doesn't need to be initialized. See omap_serial for
 * and example usage.
 */
تعريفنوع بنية kern_phys_map{
	phys_بایټونه addr; /* The physical address to map */
	vir_بایټونه اندازه;  /* The size of the mapping */
	vir_بایټونه id;	 /* an id passed to the callback */
	صحيح vm_flags;	 /* flags to be passed to vm map */
	kern_phys_map_mapped cb; /* the callback itself */
	phys_بایټونه vir; /* The virtual address once remapped */
	صحيح شاخص; 	/*شاخص*/
	بنية kern_phys_map *بل; /* pointer to the next */
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
صحيح kern_req_phys_map( phys_بایټونه بنسټ_پته, vir_بایټونه io_اندازه,
		   صحيح vm_flags, kern_phys_map * priv,
		   kern_phys_map_mapped cb, vir_بایټونه id);

/*
 * Request a physical mapping and put the result in the given prt
 * Note that ptr will only be valid once the callback happened.
 */
صحيح kern_phys_map_ptr( phys_بایټونه بنسټ_پته, vir_بایټونه io_اندازه, 
		       صحيح vm_flags, kern_phys_map * priv, 
		       vir_بایټونه ptr);

فراغ arch_ser_init(فراغ);

/* functions defined in architecture-independent kernel source. */
#ضمن "kernel/proto.h"

#د_که_پای /* __ASSEMBLY__ */

#د_که_پای
