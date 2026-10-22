/* General macros and constants used by the kernel. */
#جے_بنا_تعریف CONST_H
#تعریف_کرو CONST_H

#ضمن <minix/config.h>
#ضمن <minix/bitmap.h>

#ضمن "debug.h"

/* Translate an endpoint number to a process number, return success. */
#جے_بنا_تعریف isokendpt
#تعریف_کرو isokendpt(e,p) isokendpt_d((e),(p),0)
#تعریف_کرو okendpt(e,p)   isokendpt_d((e),(p),1)
#جے_انت

/* Constants used in virtual_copy(). Values must be 0 and 1, respectively. */
#تعریف_کرو _SRC_	0
#تعریف_کرو _DST_	1

#تعریف_کرو get_sys_ثنائی_ہندسہ(map,ثنائی_ہندسہ) \
	( MAP_CHUNK((map).chunk,ثنائی_ہندسہ) & (1 << CHUNK_وتھ(ثنائی_ہندسہ) ))
#تعریف_کرو get_sys_بٹ(map,ثنائی_ہندسہ) \
	( MAP_CHUNK((map).chunk,ثنائی_ہندسہ) )
#تعریف_کرو سیٹ_sys_ثنائی_ہندسہ(map,ثنائی_ہندسہ) \
	( MAP_CHUNK((map).chunk,ثنائی_ہندسہ) |= (1 << CHUNK_وتھ(ثنائی_ہندسہ) ))
#تعریف_کرو unset_sys_ثنائی_ہندسہ(map,ثنائی_ہندسہ) \
	( MAP_CHUNK((map).chunk,ثنائی_ہندسہ) &= ~(1 << CHUNK_وتھ(ثنائی_ہندسہ) ))

/* for kputc() */
#تعریف_کرو سرا_دا_KMESS	0

/* User limits. */
#جے_بنا_تعریف USR_DATATOP
#جے_بنا_تعریف _MINIX_MAGIC
#تعریف_کرو USR_DATATOP 0xF0000000
#وإلا
#تعریف_کرو USR_DATATOP 0xE0000000	/* TODO: is this necessary? */
#جے_انت
#جے_انت

#جے_بنا_تعریف USR_STACKTOP
#تعریف_کرو USR_STACKTOP USR_DATATOP
#جے_انت

#جے_بنا_تعریف USR_DATATOP_COMPACT
#تعریف_کرو USR_DATATOP_COMPACT USR_DATATOP
#جے_انت

#جے_بنا_تعریف USR_STACKTOP_COMPACT
#تعریف_کرو USR_STACKTOP_COMPACT 0x50000000
#جے_انت

#جے_انت /* CONST_H */
