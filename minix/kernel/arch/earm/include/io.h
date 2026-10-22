#جے_بنا_تعریف _ARM_IO_H_
#تعریف_کرو _ARM_IO_H_

#جے_بنا_تعریف __ASSEMBLY__

#ضمن <sys/types.h>

/* Access memory-mapped I/O devices */
#تعریف_کرو mmio_read(a)    (*(متطاير u32_t *)(a))
#تعریف_کرو mmio_write(a,v) (*(متطاير u32_t *)(a) = (v))
#تعریف_کرو mmio_سیٹ(a,v)   mmio_write((a), mmio_read((a)) | (v))
#تعریف_کرو mmio_clear(a,v) mmio_write((a), mmio_read((a)) & ~(v))

#جے_انت /* __ASSEMBLY__ */

#جے_انت /* _ARM_IO_H_ */
