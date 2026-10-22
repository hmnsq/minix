#यदि_अपरिभाषितम् __DEBUGREG_H__
#परिभाषय __DEBUGREG_H__

/* DR6: status flags */
#परिभाषय DR6_B(bp)	(1 << (bp))	/* breakpoint was triggered */
#परिभाषय DR6_BD		(1 << 13)	/* debug register access detected */
#परिभाषय DR6_BS		(1 << 14)	/* single step */
#परिभाषय DR6_BT		(1 << 15)	/* task switch */

/* DR7: control flags */
#परिभाषय	DR7_L(bp)	(1 << (2*(bp)))		/* breakpoint armed locally */
#परिभाषय	DR7_G(bp)	(1 << (1+2*(bp)))	/* breakpoint armed globally */
#परिभाषय	DR7_LE		(1 << 8)		/* exact local breakpoints */
#परिभाषय	DR7_GE		(1 << 9)		/* exact global breakpoints */
#परिभाषय	DR7_GD		(1 << 13)		/* detect debug reg movs */

#परिभाषय	DR7_RW_आवरणम्(bp)		(3 << (16+4*(bp)))	
#परिभाषय	DR7_RW_EXEC(bp)		(0 << (16+4*(bp)))	/* execute */
#परिभाषय	DR7_RW_WRITE(bp)	(1 << (16+4*(bp)))	/* write */
#परिभाषय	DR7_RW_IO(bp)		(2 << (16+4*(bp)))	/* IO */
#परिभाषय	DR7_RW_RW(bp)		(3 << (16+4*(bp)))	/* read or write */

#परिभाषय	DR7_LN_आवरणम्(bp)	(3 << (18+4*(bp)))
#परिभाषय	DR7_LN_1(bp)	(0 << (18+4*(bp)))	/* 1 byte */
#परिभाषय	DR7_LN_2(bp)	(1 << (18+4*(bp)))	/* 2 bytes */
#परिभाषय	DR7_LN_8(bp)	(2 << (18+4*(bp)))	/* 8 bytes */
#परिभाषय	DR7_LN_4(bp)	(3 << (18+4*(bp)))	/* 4 bytes */

/* debugreg.S */
शून्य ld_dr0(phys_अष्टकानि मानम्);
शून्य ld_dr1(phys_अष्टकानि मानम्);
शून्य ld_dr2(phys_अष्टकानि मानम्);
शून्य ld_dr3(phys_अष्टकानि मानम्);
शून्य ld_dr6(अचिह्नित दीर्घ मानम्);
शून्य ld_dr7(अचिह्नित दीर्घ मानम्);
phys_अष्टकानि st_dr0(शून्य); 
phys_अष्टकानि st_dr1(शून्य); 
phys_अष्टकानि st_dr2(शून्य); 
phys_अष्टकानि st_dr3(शून्य); 
अचिह्नित दीर्घ st_dr6(शून्य); 
अचिह्नित दीर्घ st_dr7(शून्य); 

#यदि_अन्तः /* __DEBUGREG_H__ */

