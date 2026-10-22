/* General macros and constants used by the kernel. */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CONST_H
#ವ್ಯಾಖ್ಯಾನಿಸು CONST_H

#ಅನ್ತರ್ಭಾವ <minix/config.h>
#ಅನ್ತರ್ಭಾವ <minix/bitmap.h>

#ಅನ್ತರ್ಭಾವ "debug.h"

/* Translate an endpoint number to a process number, return success. */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ isokendpt
#ವ್ಯಾಖ್ಯಾನಿಸು isokendpt(e,p) isokendpt_d((e),(p),0)
#ವ್ಯಾಖ್ಯಾನಿಸು okendpt(e,p)   isokendpt_d((e),(p),1)
#ಆದರೆ_ಅಂತ್ಯ

/* Constants used in virtual_copy(). Values must be 0 and 1, respectively. */
#ವ್ಯಾಖ್ಯಾನಿಸು _SRC_	0
#ವ್ಯಾಖ್ಯಾನಿಸು _DST_	1

#ವ್ಯಾಖ್ಯಾನಿಸು get_sys_ಬಿಟ್(map,ಬಿಟ್) \
	( MAP_CHUNK((map).chunk,ಬಿಟ್) & (1 << CHUNK_ಸ್ಥಾನಾಂತರ(ಬಿಟ್) ))
#ವ್ಯಾಖ್ಯಾನಿಸು get_sys_ಬಿಟ್ಟುಗಳು(map,ಬಿಟ್) \
	( MAP_CHUNK((map).chunk,ಬಿಟ್) )
#ವ್ಯಾಖ್ಯಾನಿಸು ಗಣ_sys_ಬಿಟ್(map,ಬಿಟ್) \
	( MAP_CHUNK((map).chunk,ಬಿಟ್) |= (1 << CHUNK_ಸ್ಥಾನಾಂತರ(ಬಿಟ್) ))
#ವ್ಯಾಖ್ಯಾನಿಸು unset_sys_ಬಿಟ್(map,ಬಿಟ್) \
	( MAP_CHUNK((map).chunk,ಬಿಟ್) &= ~(1 << CHUNK_ಸ್ಥಾನಾಂತರ(ಬಿಟ್) ))

/* for kputc() */
#ವ್ಯಾಖ್ಯಾನಿಸು ಕೊನೆ_ದ_KMESS	0

/* User limits. */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ USR_DATATOP
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ _MINIX_MAGIC
#ವ್ಯಾಖ್ಯಾನಿಸು USR_DATATOP 0xF0000000
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು USR_DATATOP 0xE0000000	/* TODO: is this necessary? */
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ USR_STACKTOP
#ವ್ಯಾಖ್ಯಾನಿಸು USR_STACKTOP USR_DATATOP
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ USR_DATATOP_COMPACT
#ವ್ಯಾಖ್ಯಾನಿಸು USR_DATATOP_COMPACT USR_DATATOP
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ USR_STACKTOP_COMPACT
#ವ್ಯಾಖ್ಯಾನಿಸು USR_STACKTOP_COMPACT 0x50000000
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ_ಅಂತ್ಯ /* CONST_H */
