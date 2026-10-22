#جے_بنا_تعریف __DEBUGREG_H__
#تعریف_کرو __DEBUGREG_H__

/* DR6: status flags */
#تعریف_کرو DR6_B(bp)	(1 << (bp))	/* breakpoint was triggered */
#تعریف_کرو DR6_BD		(1 << 13)	/* debug register access detected */
#تعریف_کرو DR6_BS		(1 << 14)	/* single step */
#تعریف_کرو DR6_BT		(1 << 15)	/* task switch */

/* DR7: control flags */
#تعریف_کرو	DR7_L(bp)	(1 << (2*(bp)))		/* breakpoint armed locally */
#تعریف_کرو	DR7_G(bp)	(1 << (1+2*(bp)))	/* breakpoint armed globally */
#تعریف_کرو	DR7_LE		(1 << 8)		/* exact local breakpoints */
#تعریف_کرو	DR7_GE		(1 << 9)		/* exact global breakpoints */
#تعریف_کرو	DR7_GD		(1 << 13)		/* detect debug reg movs */

#تعریف_کرو	DR7_RW_نقاب(bp)		(3 << (16+4*(bp)))	
#تعریف_کرو	DR7_RW_EXEC(bp)		(0 << (16+4*(bp)))	/* execute */
#تعریف_کرو	DR7_RW_WRITE(bp)	(1 << (16+4*(bp)))	/* write */
#تعریف_کرو	DR7_RW_IO(bp)		(2 << (16+4*(bp)))	/* IO */
#تعریف_کرو	DR7_RW_RW(bp)		(3 << (16+4*(bp)))	/* read or write */

#تعریف_کرو	DR7_LN_نقاب(bp)	(3 << (18+4*(bp)))
#تعریف_کرو	DR7_LN_1(bp)	(0 << (18+4*(bp)))	/* 1 byte */
#تعریف_کرو	DR7_LN_2(bp)	(1 << (18+4*(bp)))	/* 2 bytes */
#تعریف_کرو	DR7_LN_8(bp)	(2 << (18+4*(bp)))	/* 8 bytes */
#تعریف_کرو	DR7_LN_4(bp)	(3 << (18+4*(bp)))	/* 4 bytes */

/* debugreg.S */
فراغ ld_dr0(phys_بائٹاں قدر);
فراغ ld_dr1(phys_بائٹاں قدر);
فراغ ld_dr2(phys_بائٹاں قدر);
فراغ ld_dr3(phys_بائٹاں قدر);
فراغ ld_dr6(بنانشانی لما قدر);
فراغ ld_dr7(بنانشانی لما قدر);
phys_بائٹاں st_dr0(فراغ); 
phys_بائٹاں st_dr1(فراغ); 
phys_بائٹاں st_dr2(فراغ); 
phys_بائٹاں st_dr3(فراغ); 
بنانشانی لما st_dr6(فراغ); 
بنانشانی لما st_dr7(فراغ); 

#جے_انت /* __DEBUGREG_H__ */

