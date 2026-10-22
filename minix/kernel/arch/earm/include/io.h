#ifndef _ARM_IO_H_
#define _ARM_IO_H_

#ifndef __ASSEMBLY__

#অন্তর্ভাব <sys/types.h>

/* Access memory-mapped I/O devices */
#define mmio_read(a)    (*(চঞ্চল u32_t *)(a))
#define mmio_write(a,v) (*(চঞ্চল u32_t *)(a) = (v))
#define mmio_set(a,v)   mmio_write((a), mmio_read((a)) | (v))
#define mmio_clear(a,v) mmio_write((a), mmio_read((a)) & ~(v))

#endif /* __ASSEMBLY__ */

#endif /* _ARM_IO_H_ */
