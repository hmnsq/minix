
#यदि_अपरिभाषितम् _ARM_PROTO_H
#परिभाषय _ARM_PROTO_H

#अन्तर्भाव <machine/vm.h>

#परिभाषय K_राशिः_आकार	ARM_PAGE_आकार


#यदि_अपरिभाषितम् __ASSEMBLY__

#अन्तर्भाव "cpufunc.h"

/* klib */
__dead शून्य reset(शून्य);
phys_अष्टकानि vir2phys(शून्य *);
vir_अष्टकानि phys_memset(phys_अष्टकानि ph, u32_t c, phys_अष्टकानि अष्टकानि);

शून्य __switch_address_space(संरचना proc *p, संरचना proc **__ptproc);
#परिभाषय switch_स्थानाङ्कः_space(proc)	\
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
पूर्णाङ्क tss_init(अचिह्नित cpu, शून्य * kernel_राशिः);

शून्य सङ्कलय_memmap(kinfo_t *cbi, u64_t addr, u64_t len);
phys_अष्टकानि alloc_निम्नतमम्(kinfo_t *cbi, phys_अष्टकानि len);
शून्य vm_enable_paging(शून्य);
शून्य cut_memmap(kinfo_t *cbi, phys_अष्टकानि start, phys_अष्टकानि अन्तः_2);
phys_अष्टकानि pg_roundup(phys_अष्टकानि b);
शून्य pg_info(reg_t *, u32_t **);
शून्य pg_clear(शून्य);
शून्य pg_तटस्थः(kinfo_t *);
phys_अष्टकानि pg_आरोपय(शून्य);
शून्य pg_map(phys_अष्टकानि phys, vir_अष्टकानि vaddr, vir_अष्टकानि vaddr_अन्तः, kinfo_t *cbi);
पूर्णाङ्क pg_mapkernel(शून्य);
शून्य pg_mapproc(संरचना proc *p, संरचना boot_image *ip, kinfo_t *cbi);

EXTERN शून्य * k_stacks_start;
बाह्य शून्य * k_stacks;

#परिभाषय get_k_राशिः_शिखरम्(cpu)	((शून्य *)(((वर्ण*)(k_stacks)) \
					+ 2 * ((cpu) + 1) * K_राशिः_आकार))


/*
 * Definition of a callback used when a memory map changed it's base address
 */
प्रकारनिर्देश पूर्णाङ्क (*kern_phys_map_mapped)(vir_अष्टकानि id, vir_अष्टकानि new_addr );

/*
 * struct used internally by memory.c to keep a list of
 * items to map. These should be statically allocated
 * in the individual files and passed as argument. 
 * The data doesn't need to be initialized. See omap_serial for
 * and example usage.
 */
प्रकारनिर्देश संरचना kern_phys_map{
	phys_अष्टकानि addr; /* The physical address to map */
	vir_अष्टकानि आकार;  /* The size of the mapping */
	vir_अष्टकानि id;	 /* an id passed to the callback */
	पूर्णाङ्क vm_flags;	 /* flags to be passed to vm map */
	kern_phys_map_mapped cb; /* the callback itself */
	phys_अष्टकानि vir; /* The virtual address once remapped */
	पूर्णाङ्क सूचकांक; 	/*सूचकांक*/
	संरचना kern_phys_map *अग्र; /* pointer to the next */
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
पूर्णाङ्क kern_req_phys_map( phys_अष्टकानि आधारः_स्थानाङ्कः, vir_अष्टकानि io_आकार,
		   पूर्णाङ्क vm_flags, kern_phys_map * priv,
		   kern_phys_map_mapped cb, vir_अष्टकानि id);

/*
 * Request a physical mapping and put the result in the given prt
 * Note that ptr will only be valid once the callback happened.
 */
पूर्णाङ्क kern_phys_map_ptr( phys_अष्टकानि आधारः_स्थानाङ्कः, vir_अष्टकानि io_आकार, 
		       पूर्णाङ्क vm_flags, kern_phys_map * priv, 
		       vir_अष्टकानि ptr);

शून्य arch_ser_init(शून्य);

/* functions defined in architecture-independent kernel source. */
#अन्तर्भाव "kernel/proto.h"

#यदि_अन्तः /* __ASSEMBLY__ */

#यदि_अन्तः
