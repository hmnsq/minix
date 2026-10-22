/* General macros and constants used by the kernel. */
#यदि_अपरिभाषितम् CONST_H
#परिभाषय CONST_H

#अन्तर्भाव <minix/config.h>
#अन्तर्भाव <minix/bitmap.h>

#अन्तर्भाव "debug.h"

/* Translate an endpoint number to a process number, return success. */
#यदि_अपरिभाषितम् isokendpt
#परिभाषय isokendpt(e,p) isokendpt_d((e),(p),0)
#परिभाषय okendpt(e,p)   isokendpt_d((e),(p),1)
#यदि_अन्तः

/* Constants used in virtual_copy(). Values must be 0 and 1, respectively. */
#परिभाषय _SRC_	0
#परिभाषय _DST_	1

#परिभाषय get_sys_bit(map,bit) \
	( MAP_CHUNK((map).chunk,bit) & (1 << CHUNK_विस्थापनम्(bit) ))
#परिभाषय get_sys_द्वयङ्काः(map,bit) \
	( MAP_CHUNK((map).chunk,bit) )
#परिभाषय समुच्चयः_sys_bit(map,bit) \
	( MAP_CHUNK((map).chunk,bit) |= (1 << CHUNK_विस्थापनम्(bit) ))
#परिभाषय unset_sys_bit(map,bit) \
	( MAP_CHUNK((map).chunk,bit) &= ~(1 << CHUNK_विस्थापनम्(bit) ))

/* for kputc() */
#परिभाषय अन्तः_सम्बन्धि_KMESS	0

/* User limits. */
#यदि_अपरिभाषितम् USR_DATATOP
#यदि_अपरिभाषितम् _MINIX_MAGIC
#परिभाषय USR_DATATOP 0xF0000000
#अन्यथा
#परिभाषय USR_DATATOP 0xE0000000	/* TODO: is this necessary? */
#यदि_अन्तः
#यदि_अन्तः

#यदि_अपरिभाषितम् USR_STACKTOP
#परिभाषय USR_STACKTOP USR_DATATOP
#यदि_अन्तः

#यदि_अपरिभाषितम् USR_DATATOP_COMPACT
#परिभाषय USR_DATATOP_COMPACT USR_DATATOP
#यदि_अन्तः

#यदि_अपरिभाषितम् USR_STACKTOP_COMPACT
#परिभाषय USR_STACKTOP_COMPACT 0x50000000
#यदि_अन्तः

#यदि_अन्तः /* CONST_H */
