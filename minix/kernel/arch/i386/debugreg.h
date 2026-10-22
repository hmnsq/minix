#अगर_अघोषित __DEBUGREG_H__
#घोषणा __DEBUGREG_H__

/* DR6: status flags */
#घोषणा DR6_B(bp)	(1 << (bp))	/* breakpoint was triggered */
#घोषणा DR6_BD		(1 << 13)	/* debug register access detected */
#घोषणा DR6_BS		(1 << 14)	/* single step */
#घोषणा DR6_BT		(1 << 15)	/* task switch */

/* DR7: control flags */
#घोषणा	DR7_L(bp)	(1 << (2*(bp)))		/* breakpoint armed locally */
#घोषणा	DR7_G(bp)	(1 << (1+2*(bp)))	/* breakpoint armed globally */
#घोषणा	DR7_LE		(1 << 8)		/* exact local breakpoints */
#घोषणा	DR7_GE		(1 << 9)		/* exact global breakpoints */
#घोषणा	DR7_GD		(1 << 13)		/* detect debug reg movs */

#घोषणा	DR7_RW_MASK(bp)		(3 << (16+4*(bp)))	
#घोषणा	DR7_RW_EXEC(bp)		(0 << (16+4*(bp)))	/* execute */
#घोषणा	DR7_RW_WRITE(bp)	(1 << (16+4*(bp)))	/* write */
#घोषणा	DR7_RW_IO(bp)		(2 << (16+4*(bp)))	/* IO */
#घोषणा	DR7_RW_RW(bp)		(3 << (16+4*(bp)))	/* read or write */

#घोषणा	DR7_LN_MASK(bp)	(3 << (18+4*(bp)))
#घोषणा	DR7_LN_1(bp)	(0 << (18+4*(bp)))	/* 1 byte */
#घोषणा	DR7_LN_2(bp)	(1 << (18+4*(bp)))	/* 2 bytes */
#घोषणा	DR7_LN_8(bp)	(2 << (18+4*(bp)))	/* 8 bytes */
#घोषणा	DR7_LN_4(bp)	(3 << (18+4*(bp)))	/* 4 bytes */

/* debugreg.S */
व्योम ld_dr0(phys_bytes मूल्य);
व्योम ld_dr1(phys_bytes मूल्य);
व्योम ld_dr2(phys_bytes मूल्य);
व्योम ld_dr3(phys_bytes मूल्य);
व्योम ld_dr6(अचिन्हित दीर्घ मूल्य);
व्योम ld_dr7(अचिन्हित दीर्घ मूल्य);
phys_bytes st_dr0(व्योम); 
phys_bytes st_dr1(व्योम); 
phys_bytes st_dr2(व्योम); 
phys_bytes st_dr3(व्योम); 
अचिन्हित दीर्घ st_dr6(व्योम); 
अचिन्हित दीर्घ st_dr7(व्योम); 

#पूर्ण_अगर /* __DEBUGREG_H__ */

