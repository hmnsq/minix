/* General macros and constants used by the kernel. */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CONST_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CONST_H

#ਅਨ੍ਤਰ੍ਭਾਵ <minix/config.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <minix/bitmap.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "debug.h"

/* Translate an endpoint number to a process number, return success. */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ isokendpt
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ isokendpt(e,p) isokendpt_d((e),(p),0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ okendpt(e,p)   isokendpt_d((e),(p),1)
#ਜੇ_ਅੰਤ

/* Constants used in virtual_copy(). Values must be 0 and 1, respectively. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _SRC_	0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _DST_	1

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ get_sys_ਦੋਆਧਾਰੀ_ਅੰਕ(map,ਦੋਆਧਾਰੀ_ਅੰਕ) \
	( MAP_CHUNK((map).chunk,ਦੋਆਧਾਰੀ_ਅੰਕ) & (1 << CHUNK_ਵਿੱਥ(ਦੋਆਧਾਰੀ_ਅੰਕ) ))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ get_sys_ਬਿੱਟ(map,ਦੋਆਧਾਰੀ_ਅੰਕ) \
	( MAP_CHUNK((map).chunk,ਦੋਆਧਾਰੀ_ਅੰਕ) )
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸੈੱਟ_sys_ਦੋਆਧਾਰੀ_ਅੰਕ(map,ਦੋਆਧਾਰੀ_ਅੰਕ) \
	( MAP_CHUNK((map).chunk,ਦੋਆਧਾਰੀ_ਅੰਕ) |= (1 << CHUNK_ਵਿੱਥ(ਦੋਆਧਾਰੀ_ਅੰਕ) ))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ unset_sys_ਦੋਆਧਾਰੀ_ਅੰਕ(map,ਦੋਆਧਾਰੀ_ਅੰਕ) \
	( MAP_CHUNK((map).chunk,ਦੋਆਧਾਰੀ_ਅੰਕ) &= ~(1 << CHUNK_ਵਿੱਥ(ਦੋਆਧਾਰੀ_ਅੰਕ) ))

/* for kputc() */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਰਾ_ਦਾ_KMESS	0

/* User limits. */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ USR_DATATOP
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ _MINIX_MAGIC
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USR_DATATOP 0xF0000000
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USR_DATATOP 0xE0000000	/* TODO: is this necessary? */
#ਜੇ_ਅੰਤ
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ USR_STACKTOP
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USR_STACKTOP USR_DATATOP
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ USR_DATATOP_COMPACT
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USR_DATATOP_COMPACT USR_DATATOP
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ USR_STACKTOP_COMPACT
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USR_STACKTOP_COMPACT 0x50000000
#ਜੇ_ਅੰਤ

#ਜੇ_ਅੰਤ /* CONST_H */
