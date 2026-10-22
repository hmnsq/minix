
#ifndef _ARM_PROTO_H
#define _ARM_PROTO_H

#അന്തര്ഭാവ <machine/vm.h>

#define K_അട്ടി_SIZE	ARM_PAGE_SIZE


#ifndef __ASSEMBLY__

#അന്തര്ഭാവ "cpufunc.h"

/* klib */
__dead ശൂന്യം reset(ശൂന്യം);
phys_bytes vir2phys(ശൂന്യം *);
vir_bytes phys_memset(phys_bytes ph, u32_t c, phys_bytes bytes);

ശൂന്യം __switch_address_space(ഘടന proc *p, ഘടന proc **__ptproc);
#define switch_address_space(proc)	\
	__switch_address_space(proc, get_cpulocal_var_ptr(ptproc))

ശൂന്യം __copy_msg_from_user_end(ശൂന്യം);
ശൂന്യം __copy_msg_to_user_end(ശൂന്യം);
ശൂന്യം __user_copy_msg_pointer_failure(ശൂന്യം);

/* multiboot.c */
ശൂന്യം multiboot_init(ശൂന്യം);

/* protect.c */
ഘടന tss_s {
  reg_t sp0;                    /* stack pointer to use during interrupt */
} __attribute__((packed));
പൂർണ്ണസംഖ്യ tss_init(unsigned cpu, ശൂന്യം * kernel_അട്ടി);

ശൂന്യം add_memmap(kinfo_t *cbi, u64_t addr, u64_t len);
phys_bytes alloc_lowest(kinfo_t *cbi, phys_bytes len);
ശൂന്യം vm_enable_paging(ശൂന്യം);
ശൂന്യം cut_memmap(kinfo_t *cbi, phys_bytes start, phys_bytes end);
phys_bytes pg_roundup(phys_bytes b);
ശൂന്യം pg_info(reg_t *, u32_t **);
ശൂന്യം pg_clear(ശൂന്യം);
ശൂന്യം pg_identity(kinfo_t *);
phys_bytes pg_load(ശൂന്യം);
ശൂന്യം pg_map(phys_bytes phys, vir_bytes vaddr, vir_bytes vaddr_end, kinfo_t *cbi);
പൂർണ്ണസംഖ്യ pg_mapkernel(ശൂന്യം);
ശൂന്യം pg_mapproc(ഘടന proc *p, ഘടന boot_image *ip, kinfo_t *cbi);

EXTERN ശൂന്യം * k_stacks_start;
ബാഹ്യ ശൂന്യം * k_stacks;

#define get_k_അട്ടി_മുകൾ(cpu)	((ശൂന്യം *)(((അക്ഷരം*)(k_stacks)) \
					+ 2 * ((cpu) + 1) * K_അട്ടി_SIZE))


/*
 * Definition of a callback used when a memory map changed it's base address
 */
തരനിർവചനം പൂർണ്ണസംഖ്യ (*kern_phys_map_mapped)(vir_bytes id, vir_bytes new_addr );

/*
 * struct used internally by memory.c to keep a list of
 * items to map. These should be statically allocated
 * in the individual files and passed as argument. 
 * The data doesn't need to be initialized. See omap_serial for
 * and example usage.
 */
തരനിർവചനം ഘടന kern_phys_map{
	phys_bytes addr; /* The physical address to map */
	vir_bytes size;  /* The size of the mapping */
	vir_bytes id;	 /* an id passed to the callback */
	പൂർണ്ണസംഖ്യ vm_flags;	 /* flags to be passed to vm map */
	kern_phys_map_mapped cb; /* the callback itself */
	phys_bytes vir; /* The virtual address once remapped */
	പൂർണ്ണസംഖ്യ index; 	/* index */
	ഘടന kern_phys_map *next; /* pointer to the next */
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
പൂർണ്ണസംഖ്യ kern_req_phys_map( phys_bytes base_address, vir_bytes io_size,
		   പൂർണ്ണസംഖ്യ vm_flags, kern_phys_map * priv,
		   kern_phys_map_mapped cb, vir_bytes id);

/*
 * Request a physical mapping and put the result in the given prt
 * Note that ptr will only be valid once the callback happened.
 */
പൂർണ്ണസംഖ്യ kern_phys_map_ptr( phys_bytes base_address, vir_bytes io_size, 
		       പൂർണ്ണസംഖ്യ vm_flags, kern_phys_map * priv, 
		       vir_bytes ptr);

ശൂന്യം arch_ser_init(ശൂന്യം);

/* functions defined in architecture-independent kernel source. */
#അന്തര്ഭാവ "kernel/proto.h"

#endif /* __ASSEMBLY__ */

#endif
