/* General macros and constants used by the kernel. */
#अगर_अघोषित CONST_H
#घोषणा CONST_H

#समावेश <minix/config.h>
#समावेश <minix/bitmap.h>

#समावेश "debug.h"

/* Translate an endpoint number to a process number, return success. */
#अगर_अघोषित isokendpt
#घोषणा isokendpt(e,p) isokendpt_d((e),(p),0)
#घोषणा okendpt(e,p)   isokendpt_d((e),(p),1)
#पूर्ण_अगर

/* Constants used in virtual_copy(). Values must be 0 and 1, respectively. */
#घोषणा _SRC_	0
#घोषणा _DST_	1

#घोषणा get_sys_bit(map,bit) \
	( MAP_CHUNK((map).chunk,bit) & (1 << CHUNK_OFFSET(bit) ))
#घोषणा get_sys_bits(map,bit) \
	( MAP_CHUNK((map).chunk,bit) )
#घोषणा set_sys_bit(map,bit) \
	( MAP_CHUNK((map).chunk,bit) |= (1 << CHUNK_OFFSET(bit) ))
#घोषणा unset_sys_bit(map,bit) \
	( MAP_CHUNK((map).chunk,bit) &= ~(1 << CHUNK_OFFSET(bit) ))

/* for kputc() */
#घोषणा END_OF_KMESS	0

/* User limits. */
#अगर_अघोषित USR_DATATOP
#अगर_अघोषित _MINIX_MAGIC
#घोषणा USR_DATATOP 0xF0000000
#अन्यथा
#घोषणा USR_DATATOP 0xE0000000	/* TODO: is this necessary? */
#पूर्ण_अगर
#पूर्ण_अगर

#अगर_अघोषित USR_STACKTOP
#घोषणा USR_STACKTOP USR_DATATOP
#पूर्ण_अगर

#अगर_अघोषित USR_DATATOP_COMPACT
#घोषणा USR_DATATOP_COMPACT USR_DATATOP
#पूर्ण_अगर

#अगर_अघोषित USR_STACKTOP_COMPACT
#घोषणा USR_STACKTOP_COMPACT 0x50000000
#पूर्ण_अगर

#पूर्ण_अगर /* CONST_H */
