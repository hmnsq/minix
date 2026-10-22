#جيڪڏهن_وصف_نه_ٿيل _ARM_IO_H_
#وصف_ڪريو _ARM_IO_H_

#جيڪڏهن_وصف_نه_ٿيل __ASSEMBLY__

#ضمن <sys/types.h>

/* Access memory-mapped I/O devices */
#وصف_ڪريو mmio_read(a)    (*(متطاير u32_t *)(a))
#وصف_ڪريو mmio_write(a,v) (*(متطاير u32_t *)(a) = (v))
#وصف_ڪريو mmio_سيٽ(a,v)   mmio_write((a), mmio_read((a)) | (v))
#وصف_ڪريو mmio_clear(a,v) mmio_write((a), mmio_read((a)) & ~(v))

#جيڪڏهن_پورو /* __ASSEMBLY__ */

#جيڪڏهن_پورو /* _ARM_IO_H_ */
