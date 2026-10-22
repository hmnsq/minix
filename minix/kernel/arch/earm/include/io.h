#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ _ARM_IO_H_
#ವ್ಯಾಖ್ಯಾನಿಸು _ARM_IO_H_

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ __ASSEMBLY__

#ಅನ್ತರ್ಭಾವ <sys/types.h>

/* Access memory-mapped I/O devices */
#ವ್ಯಾಖ್ಯಾನಿಸು mmio_read(a)    (*(ಅಸ್ಥಿರ u32_t *)(a))
#ವ್ಯಾಖ್ಯಾನಿಸು mmio_write(a,v) (*(ಅಸ್ಥಿರ u32_t *)(a) = (v))
#ವ್ಯಾಖ್ಯಾನಿಸು mmio_ಗಣ(a,v)   mmio_write((a), mmio_read((a)) | (v))
#ವ್ಯಾಖ್ಯಾನಿಸು mmio_clear(a,v) mmio_write((a), mmio_read((a)) & ~(v))

#ಆದರೆ_ಅಂತ್ಯ /* __ASSEMBLY__ */

#ಆದರೆ_ಅಂತ್ಯ /* _ARM_IO_H_ */
