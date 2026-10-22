/* General macros and constants used by the kernel. */
#جيڪڏهن_وصف_نه_ٿيل CONST_H
#وصف_ڪريو CONST_H

#ضمن <minix/config.h>
#ضمن <minix/bitmap.h>

#ضمن "debug.h"

/* Translate an endpoint number to a process number, return success. */
#جيڪڏهن_وصف_نه_ٿيل isokendpt
#وصف_ڪريو isokendpt(e,p) isokendpt_d((e),(p),0)
#وصف_ڪريو okendpt(e,p)   isokendpt_d((e),(p),1)
#جيڪڏهن_پورو

/* Constants used in virtual_copy(). Values must be 0 and 1, respectively. */
#وصف_ڪريو _SRC_	0
#وصف_ڪريو _DST_	1

#وصف_ڪريو get_sys_ٻٽو_انگ(map,ٻٽو_انگ) \
	( MAP_CHUNK((map).chunk,ٻٽو_انگ) & (1 << CHUNK_وٿي(ٻٽو_انگ) ))
#وصف_ڪريو get_sys_بٽ(map,ٻٽو_انگ) \
	( MAP_CHUNK((map).chunk,ٻٽو_انگ) )
#وصف_ڪريو سيٽ_sys_ٻٽو_انگ(map,ٻٽو_انگ) \
	( MAP_CHUNK((map).chunk,ٻٽو_انگ) |= (1 << CHUNK_وٿي(ٻٽو_انگ) ))
#وصف_ڪريو unset_sys_ٻٽو_انگ(map,ٻٽو_انگ) \
	( MAP_CHUNK((map).chunk,ٻٽو_انگ) &= ~(1 << CHUNK_وٿي(ٻٽو_انگ) ))

/* for kputc() */
#وصف_ڪريو پڇاڙي_جو_KMESS	0

/* User limits. */
#جيڪڏهن_وصف_نه_ٿيل USR_DATATOP
#جيڪڏهن_وصف_نه_ٿيل _MINIX_MAGIC
#وصف_ڪريو USR_DATATOP 0xF0000000
#وإلا
#وصف_ڪريو USR_DATATOP 0xE0000000	/* TODO: is this necessary? */
#جيڪڏهن_پورو
#جيڪڏهن_پورو

#جيڪڏهن_وصف_نه_ٿيل USR_STACKTOP
#وصف_ڪريو USR_STACKTOP USR_DATATOP
#جيڪڏهن_پورو

#جيڪڏهن_وصف_نه_ٿيل USR_DATATOP_COMPACT
#وصف_ڪريو USR_DATATOP_COMPACT USR_DATATOP
#جيڪڏهن_پورو

#جيڪڏهن_وصف_نه_ٿيل USR_STACKTOP_COMPACT
#وصف_ڪريو USR_STACKTOP_COMPACT 0x50000000
#جيڪڏهن_پورو

#جيڪڏهن_پورو /* CONST_H */
