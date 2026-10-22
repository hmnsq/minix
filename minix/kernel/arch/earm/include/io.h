#که_نه_تعریف_شوی _ARM_IO_H_
#تعریف_کړه _ARM_IO_H_

#که_نه_تعریف_شوی __ASSEMBLY__

#ضمن <sys/types.h>

/* Access memory-mapped I/O devices */
#تعریف_کړه mmio_read(a)    (*(متطاير u32_t *)(a))
#تعریف_کړه mmio_write(a,v) (*(متطاير u32_t *)(a) = (v))
#تعریف_کړه mmio_سېټ(a,v)   mmio_write((a), mmio_read((a)) | (v))
#تعریف_کړه mmio_clear(a,v) mmio_write((a), mmio_read((a)) & ~(v))

#د_که_پای /* __ASSEMBLY__ */

#د_که_پای /* _ARM_IO_H_ */
