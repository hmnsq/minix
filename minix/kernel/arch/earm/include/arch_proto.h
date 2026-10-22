
#اگر_غیر_تعریف_شدہ _ARM_PROTO_H
#تعریف_کرو _ARM_PROTO_H

#شامل <machine/vm.h>

#تعریف_کرو K_ڈھیر_سائز	ARM_صفحہ_سائز


#اگر_غیر_تعریف_شدہ __ASSEMBLY__

#شامل "cpufunc.h"

/* klib */
__dead خالی reset(خالی);
phys_بائٹیں vir2phys(خالی *);
vir_بائٹیں phys_memset(phys_بائٹیں ph, u32_t c, phys_بائٹیں بائٹیں);

خالی __switch_address_space(ساخت proc *p, ساخت proc **__ptproc);
#تعریف_کرو switch_پتہ_space(proc)	\
	__switch_address_space(proc, get_cpulocal_var_ptr(ptproc))

خالی __copy_msg_from_user_end(خالی);
خالی __copy_msg_to_user_end(خالی);
خالی __user_copy_msg_pointer_failure(خالی);

/* multiboot.c */
خالی multiboot_init(خالی);

/* protect.c */
ساخت tss_s {
  reg_t sp0;                    /* stack pointer to use during interrupt */
} __attribute__((packed));
صحیح tss_init(غیرعلامتی cpu, خالی * kernel_ڈھیر);

خالی جوڑو_memmap(kinfo_t *cbi, u64_t addr, u64_t len);
phys_بائٹیں alloc_سب_سے_نیچا(kinfo_t *cbi, phys_بائٹیں len);
خالی vm_enable_paging(خالی);
خالی cut_memmap(kinfo_t *cbi, phys_بائٹیں آغاز, phys_بائٹیں انتہا);
phys_بائٹیں pg_roundup(phys_بائٹیں b);
خالی pg_info(reg_t *, u32_t **);
خالی pg_clear(خالی);
خالی pg_شناختی_عنصر(kinfo_t *);
phys_بائٹیں pg_بھرو(خالی);
خالی pg_map(phys_بائٹیں phys, vir_بائٹیں vaddr, vir_بائٹیں vaddr_انتہا, kinfo_t *cbi);
صحیح pg_mapkernel(خالی);
خالی pg_mapproc(ساخت proc *p, ساخت boot_image *ip, kinfo_t *cbi);

EXTERN خالی * k_stacks_آغاز;
خارجی خالی * k_stacks;

#تعریف_کرو get_k_ڈھیر_چوٹی(cpu)	((خالی *)(((حرف*)(k_stacks)) \
					+ 2 * ((cpu) + 1) * K_ڈھیر_سائز))


/*
 * Definition of a callback used when a memory map changed it's base address
 */
قسمتعریف صحیح (*kern_phys_map_mapped)(vir_بائٹیں id, vir_بائٹیں new_addr );

/*
 * struct used internally by memory.c to keep a list of
 * items to map. These should be statically allocated
 * in the individual files and passed as argument. 
 * The data doesn't need to be initialized. See omap_serial for
 * and example usage.
 */
قسمتعریف ساخت kern_phys_map{
	phys_بائٹیں addr; /* The physical address to map */
	vir_بائٹیں سائز;  /* The size of the mapping */
	vir_بائٹیں id;	 /* an id passed to the callback */
	صحیح vm_flags;	 /* flags to be passed to vm map */
	kern_phys_map_mapped cb; /* the callback itself */
	phys_بائٹیں vir; /* The virtual address once remapped */
	صحیح اشاریہ; 	/*اشاریہ*/
	ساخت kern_phys_map *اگلا; /* pointer to the next */
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
صحیح kern_req_phys_map( phys_بائٹیں بنیاد_پتہ, vir_بائٹیں io_سائز,
		   صحیح vm_flags, kern_phys_map * priv,
		   kern_phys_map_mapped cb, vir_بائٹیں id);

/*
 * Request a physical mapping and put the result in the given prt
 * Note that ptr will only be valid once the callback happened.
 */
صحیح kern_phys_map_ptr( phys_بائٹیں بنیاد_پتہ, vir_بائٹیں io_سائز, 
		       صحیح vm_flags, kern_phys_map * priv, 
		       vir_بائٹیں ptr);

خالی arch_ser_init(خالی);

/* functions defined in architecture-independent kernel source. */
#شامل "kernel/proto.h"

#اگر_ختم /* __ASSEMBLY__ */

#اگر_ختم
