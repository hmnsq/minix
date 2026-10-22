
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ _ARM_PROTO_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _ARM_PROTO_H

#ਅਨ੍ਤਰ੍ਭਾਵ <machine/vm.h>

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ K_ਢੇਰ_ਮਾਪ	ARM_ਪੰਨਾ_ਮਾਪ


#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ __ASSEMBLY__

#ਅਨ੍ਤਰ੍ਭਾਵ "cpufunc.h"

/* klib */
__dead ਖਾਲੀ reset(ਖਾਲੀ);
phys_ਬਾਈਟਾਂ vir2phys(ਖਾਲੀ *);
vir_ਬਾਈਟਾਂ phys_memset(phys_ਬਾਈਟਾਂ ph, u32_t c, phys_ਬਾਈਟਾਂ ਬਾਈਟਾਂ);

ਖਾਲੀ __switch_address_space(ਬਣਤਰ proc *p, ਬਣਤਰ proc **__ptproc);
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ switch_ਪਤਾ_space(proc)	\
	__switch_address_space(proc, get_cpulocal_var_ptr(ptproc))

ਖਾਲੀ __copy_msg_from_user_end(ਖਾਲੀ);
ਖਾਲੀ __copy_msg_to_user_end(ਖਾਲੀ);
ਖਾਲੀ __user_copy_msg_pointer_failure(ਖਾਲੀ);

/* multiboot.c */
ਖਾਲੀ multiboot_init(ਖਾਲੀ);

/* protect.c */
ਬਣਤਰ tss_s {
  reg_t sp0;                    /* stack pointer to use during interrupt */
} __attribute__((packed));
ਪੂਰਨ tss_init(ਬਿਨਾਨਿਸ਼ਾਨੀ cpu, ਖਾਲੀ * kernel_ਢੇਰ);

ਖਾਲੀ ਜੋੜੋ_memmap(kinfo_t *cbi, u64_t addr, u64_t len);
phys_ਬਾਈਟਾਂ alloc_ਸਭ_ਤੋਂ_ਨੀਵਾਂ(kinfo_t *cbi, phys_ਬਾਈਟਾਂ len);
ਖਾਲੀ vm_enable_paging(ਖਾਲੀ);
ਖਾਲੀ cut_memmap(kinfo_t *cbi, phys_ਬਾਈਟਾਂ ਸ਼ੁਰੂ, phys_ਬਾਈਟਾਂ ਸਿਰਾ);
phys_ਬਾਈਟਾਂ pg_roundup(phys_ਬਾਈਟਾਂ b);
ਖਾਲੀ pg_info(reg_t *, u32_t **);
ਖਾਲੀ pg_clear(ਖਾਲੀ);
ਖਾਲੀ pg_ਪਛਾਣ_ਤੱਤ(kinfo_t *);
phys_ਬਾਈਟਾਂ pg_ਲੱਦੋ(ਖਾਲੀ);
ਖਾਲੀ pg_map(phys_ਬਾਈਟਾਂ phys, vir_ਬਾਈਟਾਂ vaddr, vir_ਬਾਈਟਾਂ vaddr_ਸਿਰਾ, kinfo_t *cbi);
ਪੂਰਨ pg_mapkernel(ਖਾਲੀ);
ਖਾਲੀ pg_mapproc(ਬਣਤਰ proc *p, ਬਣਤਰ boot_image *ip, kinfo_t *cbi);

EXTERN ਖਾਲੀ * k_stacks_ਸ਼ੁਰੂ;
ਬਾਹਰੀ ਖਾਲੀ * k_stacks;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ get_k_ਢੇਰ_ਸਿਖਰ(cpu)	((ਖਾਲੀ *)(((ਅੱਖਰ*)(k_stacks)) \
					+ 2 * ((cpu) + 1) * K_ਢੇਰ_ਮਾਪ))


/*
 * Definition of a callback used when a memory map changed it's base address
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਪੂਰਨ (*kern_phys_map_mapped)(vir_ਬਾਈਟਾਂ id, vir_ਬਾਈਟਾਂ new_addr );

/*
 * struct used internally by memory.c to keep a list of
 * items to map. These should be statically allocated
 * in the individual files and passed as argument. 
 * The data doesn't need to be initialized. See omap_serial for
 * and example usage.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ kern_phys_map{
	phys_ਬਾਈਟਾਂ addr; /* The physical address to map */
	vir_ਬਾਈਟਾਂ ਮਾਪ;  /* The size of the mapping */
	vir_ਬਾਈਟਾਂ id;	 /* an id passed to the callback */
	ਪੂਰਨ vm_flags;	 /* flags to be passed to vm map */
	kern_phys_map_mapped cb; /* the callback itself */
	phys_ਬਾਈਟਾਂ vir; /* The virtual address once remapped */
	ਪੂਰਨ ਸੂਚਕਾਂਕ; 	/*ਸੂਚਕਾਂਕ*/
	ਬਣਤਰ kern_phys_map *ਅਗਲਾ; /* pointer to the next */
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
ਪੂਰਨ kern_req_phys_map( phys_ਬਾਈਟਾਂ ਅਧਾਰ_ਪਤਾ, vir_ਬਾਈਟਾਂ io_ਮਾਪ,
		   ਪੂਰਨ vm_flags, kern_phys_map * priv,
		   kern_phys_map_mapped cb, vir_ਬਾਈਟਾਂ id);

/*
 * Request a physical mapping and put the result in the given prt
 * Note that ptr will only be valid once the callback happened.
 */
ਪੂਰਨ kern_phys_map_ptr( phys_ਬਾਈਟਾਂ ਅਧਾਰ_ਪਤਾ, vir_ਬਾਈਟਾਂ io_ਮਾਪ, 
		       ਪੂਰਨ vm_flags, kern_phys_map * priv, 
		       vir_ਬਾਈਟਾਂ ptr);

ਖਾਲੀ arch_ser_init(ਖਾਲੀ);

/* functions defined in architecture-independent kernel source. */
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/proto.h"

#ਜੇ_ਅੰਤ /* __ASSEMBLY__ */

#ਜੇ_ਅੰਤ
