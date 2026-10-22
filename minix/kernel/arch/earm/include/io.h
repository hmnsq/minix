#నిర్వచితం_కాకపోతే _ARM_IO_H_
#నిర్వచించు _ARM_IO_H_

#నిర్వచితం_కాకపోతే __ASSEMBLY__

#అన్తర్భావ <sys/types.h>

/* Access memory-mapped I/O devices */
#నిర్వచించు mmio_read(a)    (*(అస్థిర u32_t *)(a))
#నిర్వచించు mmio_write(a,v) (*(అస్థిర u32_t *)(a) = (v))
#నిర్వచించు mmio_సమితి(a,v)   mmio_write((a), mmio_read((a)) | (v))
#నిర్వచించు mmio_clear(a,v) mmio_write((a), mmio_read((a)) & ~(v))

#అయితే_ముగింపు /* __ASSEMBLY__ */

#అయితే_ముగింపు /* _ARM_IO_H_ */
