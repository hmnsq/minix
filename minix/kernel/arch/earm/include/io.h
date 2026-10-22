#अगर_अघोषित _ARM_IO_H_
#घोषणा _ARM_IO_H_

#अगर_अघोषित __ASSEMBLY__

#समावेश <sys/types.h>

/* Access memory-mapped I/O devices */
#घोषणा mmio_read(a)    (*(अस्थिर u32_t *)(a))
#घोषणा mmio_write(a,v) (*(अस्थिर u32_t *)(a) = (v))
#घोषणा mmio_set(a,v)   mmio_write((a), mmio_read((a)) | (v))
#घोषणा mmio_clear(a,v) mmio_write((a), mmio_read((a)) & ~(v))

#पूर्ण_अगर /* __ASSEMBLY__ */

#पूर्ण_अगर /* _ARM_IO_H_ */
