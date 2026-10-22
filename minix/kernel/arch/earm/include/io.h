#اگر_تعریف_نشده _ARM_IO_H_
#تعریف_کن _ARM_IO_H_

#اگر_تعریف_نشده __ASSEMBLY__

#ضمن <sys/types.h>

/* Access memory-mapped I/O devices */
#تعریف_کن mmio_read(a)    (*(متطاير u32_t *)(a))
#تعریف_کن mmio_write(a,v) (*(متطاير u32_t *)(a) = (v))
#تعریف_کن mmio_مجموعه(a,v)   mmio_write((a), mmio_read((a)) | (v))
#تعریف_کن mmio_clear(a,v) mmio_write((a), mmio_read((a)) & ~(v))

#پایان_اگر /* __ASSEMBLY__ */

#پایان_اگر /* _ARM_IO_H_ */
