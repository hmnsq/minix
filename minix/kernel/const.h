/* General macros and constants used by the kernel. */
#నిర్వచితం_కాకపోతే CONST_H
#నిర్వచించు CONST_H

#అన్తర్భావ <minix/config.h>
#అన్తర్భావ <minix/bitmap.h>

#అన్తర్భావ "debug.h"

/* Translate an endpoint number to a process number, return success. */
#నిర్వచితం_కాకపోతే isokendpt
#నిర్వచించు isokendpt(e,p) isokendpt_d((e),(p),0)
#నిర్వచించు okendpt(e,p)   isokendpt_d((e),(p),1)
#అయితే_ముగింపు

/* Constants used in virtual_copy(). Values must be 0 and 1, respectively. */
#నిర్వచించు _SRC_	0
#నిర్వచించు _DST_	1

#నిర్వచించు get_sys_బిట్టు(map,బిట్టు) \
	( MAP_CHUNK((map).chunk,బిట్టు) & (1 << CHUNK_స్థానభ్రంశం(బిట్టు) ))
#నిర్వచించు get_sys_బిట్లు(map,బిట్టు) \
	( MAP_CHUNK((map).chunk,బిట్టు) )
#నిర్వచించు సమితి_sys_బిట్టు(map,బిట్టు) \
	( MAP_CHUNK((map).chunk,బిట్టు) |= (1 << CHUNK_స్థానభ్రంశం(బిట్టు) ))
#నిర్వచించు unset_sys_బిట్టు(map,బిట్టు) \
	( MAP_CHUNK((map).chunk,బిట్టు) &= ~(1 << CHUNK_స్థానభ్రంశం(బిట్టు) ))

/* for kputc() */
#నిర్వచించు చివర_యొక్క_KMESS	0

/* User limits. */
#నిర్వచితం_కాకపోతే USR_DATATOP
#నిర్వచితం_కాకపోతే _MINIX_MAGIC
#నిర్వచించు USR_DATATOP 0xF0000000
#లేకపోతే
#నిర్వచించు USR_DATATOP 0xE0000000	/* TODO: is this necessary? */
#అయితే_ముగింపు
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే USR_STACKTOP
#నిర్వచించు USR_STACKTOP USR_DATATOP
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే USR_DATATOP_COMPACT
#నిర్వచించు USR_DATATOP_COMPACT USR_DATATOP
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే USR_STACKTOP_COMPACT
#నిర్వచించు USR_STACKTOP_COMPACT 0x50000000
#అయితే_ముగింపు

#అయితే_ముగింపు /* CONST_H */
