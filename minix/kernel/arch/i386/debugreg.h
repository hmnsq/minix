#که_نه_تعریف_شوی __DEBUGREG_H__
#تعریف_کړه __DEBUGREG_H__

/* DR6: status flags */
#تعریف_کړه DR6_B(bp)	(1 << (bp))	/* breakpoint was triggered */
#تعریف_کړه DR6_BD		(1 << 13)	/* debug register access detected */
#تعریف_کړه DR6_BS		(1 << 14)	/* single step */
#تعریف_کړه DR6_BT		(1 << 15)	/* task switch */

/* DR7: control flags */
#تعریف_کړه	DR7_L(bp)	(1 << (2*(bp)))		/* breakpoint armed locally */
#تعریف_کړه	DR7_G(bp)	(1 << (1+2*(bp)))	/* breakpoint armed globally */
#تعریف_کړه	DR7_LE		(1 << 8)		/* exact local breakpoints */
#تعریف_کړه	DR7_GE		(1 << 9)		/* exact global breakpoints */
#تعریف_کړه	DR7_GD		(1 << 13)		/* detect debug reg movs */

#تعریف_کړه	DR7_RW_ماسک(bp)		(3 << (16+4*(bp)))	
#تعریف_کړه	DR7_RW_EXEC(bp)		(0 << (16+4*(bp)))	/* execute */
#تعریف_کړه	DR7_RW_WRITE(bp)	(1 << (16+4*(bp)))	/* write */
#تعریف_کړه	DR7_RW_IO(bp)		(2 << (16+4*(bp)))	/* IO */
#تعریف_کړه	DR7_RW_RW(bp)		(3 << (16+4*(bp)))	/* read or write */

#تعریف_کړه	DR7_LN_ماسک(bp)	(3 << (18+4*(bp)))
#تعریف_کړه	DR7_LN_1(bp)	(0 << (18+4*(bp)))	/* 1 byte */
#تعریف_کړه	DR7_LN_2(bp)	(1 << (18+4*(bp)))	/* 2 bytes */
#تعریف_کړه	DR7_LN_8(bp)	(2 << (18+4*(bp)))	/* 8 bytes */
#تعریف_کړه	DR7_LN_4(bp)	(3 << (18+4*(bp)))	/* 4 bytes */

/* debugreg.S */
فراغ ld_dr0(phys_بایټونه ارزښت);
فراغ ld_dr1(phys_بایټونه ارزښت);
فراغ ld_dr2(phys_بایټونه ارزښت);
فراغ ld_dr3(phys_بایټونه ارزښت);
فراغ ld_dr6(بینښه اوږد ارزښت);
فراغ ld_dr7(بینښه اوږد ارزښت);
phys_بایټونه st_dr0(فراغ); 
phys_بایټونه st_dr1(فراغ); 
phys_بایټونه st_dr2(فراغ); 
phys_بایټونه st_dr3(فراغ); 
بینښه اوږد st_dr6(فراغ); 
بینښه اوږد st_dr7(فراغ); 

#د_که_پای /* __DEBUGREG_H__ */

