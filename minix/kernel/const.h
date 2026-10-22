/* General macros and constants used by the kernel. */
#که_نه_تعریف_شوی CONST_H
#تعریف_کړه CONST_H

#ضمن <minix/config.h>
#ضمن <minix/bitmap.h>

#ضمن "debug.h"

/* Translate an endpoint number to a process number, return success. */
#که_نه_تعریف_شوی isokendpt
#تعریف_کړه isokendpt(e,p) isokendpt_d((e),(p),0)
#تعریف_کړه okendpt(e,p)   isokendpt_d((e),(p),1)
#د_که_پای

/* Constants used in virtual_copy(). Values must be 0 and 1, respectively. */
#تعریف_کړه _SRC_	0
#تعریف_کړه _DST_	1

#تعریف_کړه get_sys_بټ(map,بټ) \
	( MAP_CHUNK((map).chunk,بټ) & (1 << CHUNK_دننه_واټن(بټ) ))
#تعریف_کړه get_sys_بټونه(map,بټ) \
	( MAP_CHUNK((map).chunk,بټ) )
#تعریف_کړه سېټ_sys_بټ(map,بټ) \
	( MAP_CHUNK((map).chunk,بټ) |= (1 << CHUNK_دننه_واټن(بټ) ))
#تعریف_کړه unset_sys_بټ(map,بټ) \
	( MAP_CHUNK((map).chunk,بټ) &= ~(1 << CHUNK_دننه_واټن(بټ) ))

/* for kputc() */
#تعریف_کړه پای_ټکی_د_KMESS	0

/* User limits. */
#که_نه_تعریف_شوی USR_DATATOP
#که_نه_تعریف_شوی _MINIX_MAGIC
#تعریف_کړه USR_DATATOP 0xF0000000
#وإلا
#تعریف_کړه USR_DATATOP 0xE0000000	/* TODO: is this necessary? */
#د_که_پای
#د_که_پای

#که_نه_تعریف_شوی USR_STACKTOP
#تعریف_کړه USR_STACKTOP USR_DATATOP
#د_که_پای

#که_نه_تعریف_شوی USR_DATATOP_COMPACT
#تعریف_کړه USR_DATATOP_COMPACT USR_DATATOP
#د_که_پای

#که_نه_تعریف_شوی USR_STACKTOP_COMPACT
#تعریف_کړه USR_STACKTOP_COMPACT 0x50000000
#د_که_پای

#د_که_پای /* CONST_H */
