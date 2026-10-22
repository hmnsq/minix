#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ _ARM_IO_H_
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _ARM_IO_H_

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ __ASSEMBLY__

#ਅਨ੍ਤਰ੍ਭਾਵ <sys/types.h>

/* Access memory-mapped I/O devices */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ mmio_read(a)    (*(ਅਸਥਿਰ u32_t *)(a))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ mmio_write(a,v) (*(ਅਸਥਿਰ u32_t *)(a) = (v))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ mmio_ਸੈੱਟ(a,v)   mmio_write((a), mmio_read((a)) | (v))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ mmio_clear(a,v) mmio_write((a), mmio_read((a)) & ~(v))

#ਜੇ_ਅੰਤ /* __ASSEMBLY__ */

#ਜੇ_ਅੰਤ /* _ARM_IO_H_ */
