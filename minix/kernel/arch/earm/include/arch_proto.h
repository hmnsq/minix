
#अगर_अघोषित _ARM_PROTO_H
#घोषणा _ARM_PROTO_H

#समावेश <machine/vm.h>

#घोषणा K_ढेर_आकार	ARM_PAGE_आकार


#अगर_अघोषित __ASSEMBLY__

#समावेश "cpufunc.h"

/* klib */
__dead व्योम reset(व्योम);
phys_bytes vir2phys(व्योम *);
vir_bytes phys_memset(phys_bytes ph, u32_t c, phys_bytes bytes);

व्योम __switch_address_space(काष्ठा proc *p, काष्ठा proc **__ptproc);
#घोषणा switch_address_space(proc)	\
	__switch_address_space(proc, get_cpulocal_var_ptr(ptproc))

व्योम __copy_msg_from_user_end(व्योम);
व्योम __copy_msg_to_user_end(व्योम);
व्योम __user_copy_msg_pointer_failure(व्योम);

/* multiboot.c */
व्योम multiboot_init(व्योम);

/* protect.c */
काष्ठा tss_s {
  reg_t sp0;                    /* stack pointer to use during interrupt */
} __attribute__((packed));
पूर्णांक tss_init(अचिन्हित cpu, व्योम * kernel_ढेर);

व्योम add_memmap(kinfo_t *cbi, u64_t addr, u64_t len);
phys_bytes alloc_lowest(kinfo_t *cbi, phys_bytes len);
व्योम vm_enable_paging(व्योम);
व्योम cut_memmap(kinfo_t *cbi, phys_bytes start, phys_bytes end);
phys_bytes pg_roundup(phys_bytes b);
व्योम pg_info(reg_t *, u32_t **);
व्योम pg_clear(व्योम);
व्योम pg_identity(kinfo_t *);
phys_bytes pg_load(व्योम);
व्योम pg_map(phys_bytes phys, vir_bytes vaddr, vir_bytes vaddr_end, kinfo_t *cbi);
पूर्णांक pg_mapkernel(व्योम);
व्योम pg_mapproc(काष्ठा proc *p, काष्ठा boot_image *ip, kinfo_t *cbi);

EXTERN व्योम * k_stacks_start;
बाह्य व्योम * k_stacks;

#घोषणा get_k_ढेर_शीर्ष(cpu)	((व्योम *)(((अक्षर*)(k_stacks)) \
					+ 2 * ((cpu) + 1) * K_ढेर_आकार))


/*
 * Definition of a callback used when a memory map changed it's base address
 */
प्रकार पूर्णांक (*kern_phys_map_mapped)(vir_bytes id, vir_bytes new_addr );

/*
 * struct used internally by memory.c to keep a list of
 * items to map. These should be statically allocated
 * in the individual files and passed as argument. 
 * The data doesn't need to be initialized. See omap_serial for
 * and example usage.
 */
प्रकार काष्ठा kern_phys_map{
	phys_bytes addr; /* The physical address to map */
	vir_bytes आकार;  /* The size of the mapping */
	vir_bytes id;	 /* an id passed to the callback */
	पूर्णांक vm_flags;	 /* flags to be passed to vm map */
	kern_phys_map_mapped cb; /* the callback itself */
	phys_bytes vir; /* The virtual address once remapped */
	पूर्णांक सूचकांक; 	/*सूचकांक*/
	काष्ठा kern_phys_map *अग्र; /* pointer to the next */
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
पूर्णांक kern_req_phys_map( phys_bytes base_address, vir_bytes io_आकार,
		   पूर्णांक vm_flags, kern_phys_map * priv,
		   kern_phys_map_mapped cb, vir_bytes id);

/*
 * Request a physical mapping and put the result in the given prt
 * Note that ptr will only be valid once the callback happened.
 */
पूर्णांक kern_phys_map_ptr( phys_bytes base_address, vir_bytes io_आकार, 
		       पूर्णांक vm_flags, kern_phys_map * priv, 
		       vir_bytes ptr);

व्योम arch_ser_init(व्योम);

/* functions defined in architecture-independent kernel source. */
#समावेश "kernel/proto.h"

#पूर्ण_अगर /* __ASSEMBLY__ */

#पूर्ण_अगर
