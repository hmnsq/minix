#اگر_غیر_تعریف_شدہ _ARM_IO_H_
#تعریف_کرو _ARM_IO_H_

#اگر_غیر_تعریف_شدہ __ASSEMBLY__

#شامل <sys/types.h>

/* Access memory-mapped I/O devices */
#تعریف_کرو mmio_read(a)    (*(متلون u32_t *)(a))
#تعریف_کرو mmio_write(a,v) (*(متلون u32_t *)(a) = (v))
#تعریف_کرو mmio_سیٹ(a,v)   mmio_write((a), mmio_read((a)) | (v))
#تعریف_کرو mmio_clear(a,v) mmio_write((a), mmio_read((a)) & ~(v))

#اگر_ختم /* __ASSEMBLY__ */

#اگر_ختم /* _ARM_IO_H_ */
