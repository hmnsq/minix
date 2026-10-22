#यदि_अपरिभाषितम् _ARM_IO_H_
#परिभाषय _ARM_IO_H_

#यदि_अपरिभाषितम् __ASSEMBLY__

#अन्तर्भाव <sys/types.h>

/* Access memory-mapped I/O devices */
#परिभाषय mmio_read(a)    (*(चञ्चल u32_t *)(a))
#परिभाषय mmio_write(a,v) (*(चञ्चल u32_t *)(a) = (v))
#परिभाषय mmio_समुच्चयः(a,v)   mmio_write((a), mmio_read((a)) | (v))
#परिभाषय mmio_clear(a,v) mmio_write((a), mmio_read((a)) & ~(v))

#यदि_अन्तः /* __ASSEMBLY__ */

#यदि_अन्तः /* _ARM_IO_H_ */
