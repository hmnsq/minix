
#جيڪڏهن_وصف_نه_ٿيل _ARM_PROTO_H
#وصف_ڪريو _ARM_PROTO_H

#ضمن <machine/vm.h>

#وصف_ڪريو K_ڍير_سائيز	ARM_صفحو_سائيز


#جيڪڏهن_وصف_نه_ٿيل __ASSEMBLY__

#ضمن "cpufunc.h"

/* klib */
__dead فراغ reset(فراغ);
phys_بائيٽون vir2phys(فراغ *);
vir_بائيٽون phys_memset(phys_بائيٽون ph, u32_t c, phys_بائيٽون بائيٽون);

فراغ __switch_address_space(بنية proc *p, بنية proc **__ptproc);
#وصف_ڪريو switch_پتو_space(proc)	\
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
صحيح tss_init(بنانشان cpu, فراغ * kernel_ڍير);

فراغ جوڙيو_memmap(kinfo_t *cbi, u64_t addr, u64_t len);
phys_بائيٽون alloc_سڀ_کان_هيٺ(kinfo_t *cbi, phys_بائيٽون len);
فراغ vm_enable_paging(فراغ);
فراغ cut_memmap(kinfo_t *cbi, phys_بائيٽون شروعات, phys_بائيٽون پڇاڙي);
phys_بائيٽون pg_roundup(phys_بائيٽون b);
فراغ pg_info(reg_t *, u32_t **);
فراغ pg_clear(فراغ);
فراغ pg_سڃاڻپ_عنصر(kinfo_t *);
phys_بائيٽون pg_ڀريو(فراغ);
فراغ pg_map(phys_بائيٽون phys, vir_بائيٽون vaddr, vir_بائيٽون vaddr_پڇاڙي, kinfo_t *cbi);
صحيح pg_mapkernel(فراغ);
فراغ pg_mapproc(بنية proc *p, بنية boot_image *ip, kinfo_t *cbi);

EXTERN فراغ * k_stacks_شروعات;
خارجي فراغ * k_stacks;

#وصف_ڪريو get_k_ڍير_چوٽي(cpu)	((فراغ *)(((حرف*)(k_stacks)) \
					+ 2 * ((cpu) + 1) * K_ڍير_سائيز))


/*
 * Definition of a callback used when a memory map changed it's base address
 */
تعريفنوع صحيح (*kern_phys_map_mapped)(vir_بائيٽون id, vir_بائيٽون new_addr );

/*
 * struct used internally by memory.c to keep a list of
 * items to map. These should be statically allocated
 * in the individual files and passed as argument. 
 * The data doesn't need to be initialized. See omap_serial for
 * and example usage.
 */
تعريفنوع بنية kern_phys_map{
	phys_بائيٽون addr; /* The physical address to map */
	vir_بائيٽون سائيز;  /* The size of the mapping */
	vir_بائيٽون id;	 /* an id passed to the callback */
	صحيح vm_flags;	 /* flags to be passed to vm map */
	kern_phys_map_mapped cb; /* the callback itself */
	phys_بائيٽون vir; /* The virtual address once remapped */
	صحيح اشاريو; 	/*اشاريو*/
	بنية kern_phys_map *ايندڙ; /* pointer to the next */
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
صحيح kern_req_phys_map( phys_بائيٽون بنياد_پتو, vir_بائيٽون io_سائيز,
		   صحيح vm_flags, kern_phys_map * priv,
		   kern_phys_map_mapped cb, vir_بائيٽون id);

/*
 * Request a physical mapping and put the result in the given prt
 * Note that ptr will only be valid once the callback happened.
 */
صحيح kern_phys_map_ptr( phys_بائيٽون بنياد_پتو, vir_بائيٽون io_سائيز, 
		       صحيح vm_flags, kern_phys_map * priv, 
		       vir_بائيٽون ptr);

فراغ arch_ser_init(فراغ);

/* functions defined in architecture-independent kernel source. */
#ضمن "kernel/proto.h"

#جيڪڏهن_پورو /* __ASSEMBLY__ */

#جيڪڏهن_پورو
