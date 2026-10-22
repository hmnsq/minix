
#جے_بنا_تعریف _ARM_PROTO_H
#تعریف_کرو _ARM_PROTO_H

#ضمن <machine/vm.h>

#تعریف_کرو K_ڈھیر_ناپ	ARM_ورقہ_ناپ


#جے_بنا_تعریف __ASSEMBLY__

#ضمن "cpufunc.h"

/* klib */
__dead فراغ reset(فراغ);
phys_بائٹاں vir2phys(فراغ *);
vir_بائٹاں phys_memset(phys_بائٹاں ph, u32_t c, phys_بائٹاں بائٹاں);

فراغ __switch_address_space(بنية proc *p, بنية proc **__ptproc);
#تعریف_کرو switch_پتہ_space(proc)	\
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
صحيح tss_init(بنانشانی cpu, فراغ * kernel_ڈھیر);

فراغ جوڑو_memmap(kinfo_t *cbi, u64_t addr, u64_t len);
phys_بائٹاں alloc_سبھ_توں_نیواں(kinfo_t *cbi, phys_بائٹاں len);
فراغ vm_enable_paging(فراغ);
فراغ cut_memmap(kinfo_t *cbi, phys_بائٹاں شروع, phys_بائٹاں سرا);
phys_بائٹاں pg_roundup(phys_بائٹاں b);
فراغ pg_info(reg_t *, u32_t **);
فراغ pg_clear(فراغ);
فراغ pg_پچھان_تت(kinfo_t *);
phys_بائٹاں pg_لدو(فراغ);
فراغ pg_map(phys_بائٹاں phys, vir_بائٹاں vaddr, vir_بائٹاں vaddr_سرا, kinfo_t *cbi);
صحيح pg_mapkernel(فراغ);
فراغ pg_mapproc(بنية proc *p, بنية boot_image *ip, kinfo_t *cbi);

EXTERN فراغ * k_stacks_شروع;
خارجي فراغ * k_stacks;

#تعریف_کرو get_k_ڈھیر_چوٹی(cpu)	((فراغ *)(((حرف*)(k_stacks)) \
					+ 2 * ((cpu) + 1) * K_ڈھیر_ناپ))


/*
 * Definition of a callback used when a memory map changed it's base address
 */
تعريفنوع صحيح (*kern_phys_map_mapped)(vir_بائٹاں id, vir_بائٹاں new_addr );

/*
 * struct used internally by memory.c to keep a list of
 * items to map. These should be statically allocated
 * in the individual files and passed as argument. 
 * The data doesn't need to be initialized. See omap_serial for
 * and example usage.
 */
تعريفنوع بنية kern_phys_map{
	phys_بائٹاں addr; /* The physical address to map */
	vir_بائٹاں ناپ;  /* The size of the mapping */
	vir_بائٹاں id;	 /* an id passed to the callback */
	صحيح vm_flags;	 /* flags to be passed to vm map */
	kern_phys_map_mapped cb; /* the callback itself */
	phys_بائٹاں vir; /* The virtual address once remapped */
	صحيح اشاریہ; 	/*اشاریہ*/
	بنية kern_phys_map *اگلا; /* pointer to the next */
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
صحيح kern_req_phys_map( phys_بائٹاں ادھار_پتہ, vir_بائٹاں io_ناپ,
		   صحيح vm_flags, kern_phys_map * priv,
		   kern_phys_map_mapped cb, vir_بائٹاں id);

/*
 * Request a physical mapping and put the result in the given prt
 * Note that ptr will only be valid once the callback happened.
 */
صحيح kern_phys_map_ptr( phys_بائٹاں ادھار_پتہ, vir_بائٹاں io_ناپ, 
		       صحيح vm_flags, kern_phys_map * priv, 
		       vir_بائٹاں ptr);

فراغ arch_ser_init(فراغ);

/* functions defined in architecture-independent kernel source. */
#ضمن "kernel/proto.h"

#جے_انت /* __ASSEMBLY__ */

#جے_انت
