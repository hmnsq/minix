#جيڪڏهن_وصف_نه_ٿيل __DEBUGREG_H__
#وصف_ڪريو __DEBUGREG_H__

/* DR6: status flags */
#وصف_ڪريو DR6_B(bp)	(1 << (bp))	/* breakpoint was triggered */
#وصف_ڪريو DR6_BD		(1 << 13)	/* debug register access detected */
#وصف_ڪريو DR6_BS		(1 << 14)	/* single step */
#وصف_ڪريو DR6_BT		(1 << 15)	/* task switch */

/* DR7: control flags */
#وصف_ڪريو	DR7_L(bp)	(1 << (2*(bp)))		/* breakpoint armed locally */
#وصف_ڪريو	DR7_G(bp)	(1 << (1+2*(bp)))	/* breakpoint armed globally */
#وصف_ڪريو	DR7_LE		(1 << 8)		/* exact local breakpoints */
#وصف_ڪريو	DR7_GE		(1 << 9)		/* exact global breakpoints */
#وصف_ڪريو	DR7_GD		(1 << 13)		/* detect debug reg movs */

#وصف_ڪريو	DR7_RW_نقاب(bp)		(3 << (16+4*(bp)))	
#وصف_ڪريو	DR7_RW_EXEC(bp)		(0 << (16+4*(bp)))	/* execute */
#وصف_ڪريو	DR7_RW_WRITE(bp)	(1 << (16+4*(bp)))	/* write */
#وصف_ڪريو	DR7_RW_IO(bp)		(2 << (16+4*(bp)))	/* IO */
#وصف_ڪريو	DR7_RW_RW(bp)		(3 << (16+4*(bp)))	/* read or write */

#وصف_ڪريو	DR7_LN_نقاب(bp)	(3 << (18+4*(bp)))
#وصف_ڪريو	DR7_LN_1(bp)	(0 << (18+4*(bp)))	/* 1 byte */
#وصف_ڪريو	DR7_LN_2(bp)	(1 << (18+4*(bp)))	/* 2 bytes */
#وصف_ڪريو	DR7_LN_8(bp)	(2 << (18+4*(bp)))	/* 8 bytes */
#وصف_ڪريو	DR7_LN_4(bp)	(3 << (18+4*(bp)))	/* 4 bytes */

/* debugreg.S */
فراغ ld_dr0(phys_بائيٽون قدر);
فراغ ld_dr1(phys_بائيٽون قدر);
فراغ ld_dr2(phys_بائيٽون قدر);
فراغ ld_dr3(phys_بائيٽون قدر);
فراغ ld_dr6(بنانشان ڊگهو قدر);
فراغ ld_dr7(بنانشان ڊگهو قدر);
phys_بائيٽون st_dr0(فراغ); 
phys_بائيٽون st_dr1(فراغ); 
phys_بائيٽون st_dr2(فراغ); 
phys_بائيٽون st_dr3(فراغ); 
بنانشان ڊگهو st_dr6(فراغ); 
بنانشان ڊگهو st_dr7(فراغ); 

#جيڪڏهن_پورو /* __DEBUGREG_H__ */

