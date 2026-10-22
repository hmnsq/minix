/* General macros and constants used by the kernel. */
#اگر_غیر_تعریف_شدہ CONST_H
#تعریف_کرو CONST_H

#شامل <minix/config.h>
#شامل <minix/bitmap.h>

#شامل "debug.h"

/* Translate an endpoint number to a process number, return success. */
#اگر_غیر_تعریف_شدہ isokendpt
#تعریف_کرو isokendpt(e,p) isokendpt_d((e),(p),0)
#تعریف_کرو okendpt(e,p)   isokendpt_d((e),(p),1)
#اگر_ختم

/* Constants used in virtual_copy(). Values must be 0 and 1, respectively. */
#تعریف_کرو _SRC_	0
#تعریف_کرو _DST_	1

#تعریف_کرو get_sys_ثنائی_ہندسہ(map,ثنائی_ہندسہ) \
	( MAP_CHUNK((map).chunk,ثنائی_ہندسہ) & (1 << CHUNK_انحراف(ثنائی_ہندسہ) ))
#تعریف_کرو get_sys_بٹ(map,ثنائی_ہندسہ) \
	( MAP_CHUNK((map).chunk,ثنائی_ہندسہ) )
#تعریف_کرو سیٹ_sys_ثنائی_ہندسہ(map,ثنائی_ہندسہ) \
	( MAP_CHUNK((map).chunk,ثنائی_ہندسہ) |= (1 << CHUNK_انحراف(ثنائی_ہندسہ) ))
#تعریف_کرو unset_sys_ثنائی_ہندسہ(map,ثنائی_ہندسہ) \
	( MAP_CHUNK((map).chunk,ثنائی_ہندسہ) &= ~(1 << CHUNK_انحراف(ثنائی_ہندسہ) ))

/* for kputc() */
#تعریف_کرو انتہا_کا_KMESS	0

/* User limits. */
#اگر_غیر_تعریف_شدہ USR_DATATOP
#اگر_غیر_تعریف_شدہ _MINIX_MAGIC
#تعریف_کرو USR_DATATOP 0xF0000000
#ورنہ
#تعریف_کرو USR_DATATOP 0xE0000000	/* TODO: is this necessary? */
#اگر_ختم
#اگر_ختم

#اگر_غیر_تعریف_شدہ USR_STACKTOP
#تعریف_کرو USR_STACKTOP USR_DATATOP
#اگر_ختم

#اگر_غیر_تعریف_شدہ USR_DATATOP_COMPACT
#تعریف_کرو USR_DATATOP_COMPACT USR_DATATOP
#اگر_ختم

#اگر_غیر_تعریف_شدہ USR_STACKTOP_COMPACT
#تعریف_کرو USR_STACKTOP_COMPACT 0x50000000
#اگر_ختم

#اگر_ختم /* CONST_H */
