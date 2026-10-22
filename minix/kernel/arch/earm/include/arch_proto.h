
#నిర్వచితం_కాకపోతే _ARM_PROTO_H
#నిర్వచించు _ARM_PROTO_H

#అన్తర్భావ <machine/vm.h>

#నిర్వచించు K_దొంతర_కొలత	ARM_పుట_కొలత


#నిర్వచితం_కాకపోతే __ASSEMBLY__

#అన్తర్భావ "cpufunc.h"

/* klib */
__dead శూన్య reset(శూన్య);
phys_బైట్లు vir2phys(శూన్య *);
vir_బైట్లు phys_memset(phys_బైట్లు ph, u32_t c, phys_బైట్లు బైట్లు);

శూన్య __switch_address_space(నిర్మాణం proc *p, నిర్మాణం proc **__ptproc);
#నిర్వచించు switch_చిరునామా_space(proc)	\
	__switch_address_space(proc, get_cpulocal_var_ptr(ptproc))

శూన్య __copy_msg_from_user_end(శూన్య);
శూన్య __copy_msg_to_user_end(శూన్య);
శూన్య __user_copy_msg_pointer_failure(శూన్య);

/* multiboot.c */
శూన్య multiboot_init(శూన్య);

/* protect.c */
నిర్మాణం tss_s {
  reg_t sp0;                    /* stack pointer to use during interrupt */
} __attribute__((packed));
పూర్ణాంకం tss_init(గుర్తులేని cpu, శూన్య * kernel_దొంతర);

శూన్య కూడు_memmap(kinfo_t *cbi, u64_t addr, u64_t len);
phys_బైట్లు alloc_అత్యల్ప(kinfo_t *cbi, phys_బైట్లు len);
శూన్య vm_enable_paging(శూన్య);
శూన్య cut_memmap(kinfo_t *cbi, phys_బైట్లు ప్రారంభం, phys_బైట్లు చివర);
phys_బైట్లు pg_roundup(phys_బైట్లు b);
శూన్య pg_info(reg_t *, u32_t **);
శూన్య pg_clear(శూన్య);
శూన్య pg_తటస్థం(kinfo_t *);
phys_బైట్లు pg_ఎక్కించు(శూన్య);
శూన్య pg_map(phys_బైట్లు phys, vir_బైట్లు vaddr, vir_బైట్లు vaddr_చివర, kinfo_t *cbi);
పూర్ణాంకం pg_mapkernel(శూన్య);
శూన్య pg_mapproc(నిర్మాణం proc *p, నిర్మాణం boot_image *ip, kinfo_t *cbi);

EXTERN శూన్య * k_stacks_ప్రారంభం;
బాహ్య శూన్య * k_stacks;

#నిర్వచించు get_k_దొంతర_శిఖరం(cpu)	((శూన్య *)(((అక్షరం*)(k_stacks)) \
					+ 2 * ((cpu) + 1) * K_దొంతర_కొలత))


/*
 * Definition of a callback used when a memory map changed it's base address
 */
రకనిర్వచనం పూర్ణాంకం (*kern_phys_map_mapped)(vir_బైట్లు id, vir_బైట్లు new_addr );

/*
 * struct used internally by memory.c to keep a list of
 * items to map. These should be statically allocated
 * in the individual files and passed as argument. 
 * The data doesn't need to be initialized. See omap_serial for
 * and example usage.
 */
రకనిర్వచనం నిర్మాణం kern_phys_map{
	phys_బైట్లు addr; /* The physical address to map */
	vir_బైట్లు కొలత;  /* The size of the mapping */
	vir_బైట్లు id;	 /* an id passed to the callback */
	పూర్ణాంకం vm_flags;	 /* flags to be passed to vm map */
	kern_phys_map_mapped cb; /* the callback itself */
	phys_బైట్లు vir; /* The virtual address once remapped */
	పూర్ణాంకం సూచీ; 	/*సూచీ*/
	నిర్మాణం kern_phys_map *తదుపరి; /* pointer to the next */
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
పూర్ణాంకం kern_req_phys_map( phys_బైట్లు పునాది_చిరునామా, vir_బైట్లు io_కొలత,
		   పూర్ణాంకం vm_flags, kern_phys_map * priv,
		   kern_phys_map_mapped cb, vir_బైట్లు id);

/*
 * Request a physical mapping and put the result in the given prt
 * Note that ptr will only be valid once the callback happened.
 */
పూర్ణాంకం kern_phys_map_ptr( phys_బైట్లు పునాది_చిరునామా, vir_బైట్లు io_కొలత, 
		       పూర్ణాంకం vm_flags, kern_phys_map * priv, 
		       vir_బైట్లు ptr);

శూన్య arch_ser_init(శూన్య);

/* functions defined in architecture-independent kernel source. */
#అన్తర్భావ "kernel/proto.h"

#అయితే_ముగింపు /* __ASSEMBLY__ */

#అయితే_ముగింపు
