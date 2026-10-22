#నిర్వచితం_కాకపోతే __DEBUGREG_H__
#నిర్వచించు __DEBUGREG_H__

/* DR6: status flags */
#నిర్వచించు DR6_B(bp)	(1 << (bp))	/* breakpoint was triggered */
#నిర్వచించు DR6_BD		(1 << 13)	/* debug register access detected */
#నిర్వచించు DR6_BS		(1 << 14)	/* single step */
#నిర్వచించు DR6_BT		(1 << 15)	/* task switch */

/* DR7: control flags */
#నిర్వచించు	DR7_L(bp)	(1 << (2*(bp)))		/* breakpoint armed locally */
#నిర్వచించు	DR7_G(bp)	(1 << (1+2*(bp)))	/* breakpoint armed globally */
#నిర్వచించు	DR7_LE		(1 << 8)		/* exact local breakpoints */
#నిర్వచించు	DR7_GE		(1 << 9)		/* exact global breakpoints */
#నిర్వచించు	DR7_GD		(1 << 13)		/* detect debug reg movs */

#నిర్వచించు	DR7_RW_ముసుగు(bp)		(3 << (16+4*(bp)))	
#నిర్వచించు	DR7_RW_EXEC(bp)		(0 << (16+4*(bp)))	/* execute */
#నిర్వచించు	DR7_RW_WRITE(bp)	(1 << (16+4*(bp)))	/* write */
#నిర్వచించు	DR7_RW_IO(bp)		(2 << (16+4*(bp)))	/* IO */
#నిర్వచించు	DR7_RW_RW(bp)		(3 << (16+4*(bp)))	/* read or write */

#నిర్వచించు	DR7_LN_ముసుగు(bp)	(3 << (18+4*(bp)))
#నిర్వచించు	DR7_LN_1(bp)	(0 << (18+4*(bp)))	/* 1 byte */
#నిర్వచించు	DR7_LN_2(bp)	(1 << (18+4*(bp)))	/* 2 bytes */
#నిర్వచించు	DR7_LN_8(bp)	(2 << (18+4*(bp)))	/* 8 bytes */
#నిర్వచించు	DR7_LN_4(bp)	(3 << (18+4*(bp)))	/* 4 bytes */

/* debugreg.S */
శూన్య ld_dr0(phys_బైట్లు విలువ);
శూన్య ld_dr1(phys_బైట్లు విలువ);
శూన్య ld_dr2(phys_బైట్లు విలువ);
శూన్య ld_dr3(phys_బైట్లు విలువ);
శూన్య ld_dr6(గుర్తులేని దీర్ఘ విలువ);
శూన్య ld_dr7(గుర్తులేని దీర్ఘ విలువ);
phys_బైట్లు st_dr0(శూన్య); 
phys_బైట్లు st_dr1(శూన్య); 
phys_బైట్లు st_dr2(శూన్య); 
phys_బైట్లు st_dr3(శూన్య); 
గుర్తులేని దీర్ఘ st_dr6(శూన్య); 
గుర్తులేని దీర్ఘ st_dr7(శూన్య); 

#అయితే_ముగింపు /* __DEBUGREG_H__ */

