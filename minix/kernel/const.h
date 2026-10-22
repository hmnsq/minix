/* General macros and constants used by the kernel. */
#اگر_تعریف_نشده CONST_H
#تعریف_کن CONST_H

#ضمن <minix/config.h>
#ضمن <minix/bitmap.h>

#ضمن "debug.h"

/* Translate an endpoint number to a process number, return success. */
#اگر_تعریف_نشده isokendpt
#تعریف_کن isokendpt(e,p) isokendpt_d((e),(p),0)
#تعریف_کن okendpt(e,p)   isokendpt_d((e),(p),1)
#پایان_اگر

/* Constants used in virtual_copy(). Values must be 0 and 1, respectively. */
#تعریف_کن _SRC_	0
#تعریف_کن _DST_	1

#تعریف_کن get_sys_بیت(map,بیت) \
	( MAP_CHUNK((map).chunk,بیت) & (1 << CHUNK_جابه_جایی(بیت) ))
#تعریف_کن get_sys_بیتها(map,بیت) \
	( MAP_CHUNK((map).chunk,بیت) )
#تعریف_کن مجموعه_sys_بیت(map,بیت) \
	( MAP_CHUNK((map).chunk,بیت) |= (1 << CHUNK_جابه_جایی(بیت) ))
#تعریف_کن unset_sys_بیت(map,بیت) \
	( MAP_CHUNK((map).chunk,بیت) &= ~(1 << CHUNK_جابه_جایی(بیت) ))

/* for kputc() */
#تعریف_کن انتها_از_KMESS	0

/* User limits. */
#اگر_تعریف_نشده USR_DATATOP
#اگر_تعریف_نشده _MINIX_MAGIC
#تعریف_کن USR_DATATOP 0xF0000000
#وإلا
#تعریف_کن USR_DATATOP 0xE0000000	/* TODO: is this necessary? */
#پایان_اگر
#پایان_اگر

#اگر_تعریف_نشده USR_STACKTOP
#تعریف_کن USR_STACKTOP USR_DATATOP
#پایان_اگر

#اگر_تعریف_نشده USR_DATATOP_COMPACT
#تعریف_کن USR_DATATOP_COMPACT USR_DATATOP
#پایان_اگر

#اگر_تعریف_نشده USR_STACKTOP_COMPACT
#تعریف_کن USR_STACKTOP_COMPACT 0x50000000
#پایان_اگر

#پایان_اگر /* CONST_H */
