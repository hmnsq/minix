#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ __DEBUGREG_H__
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ __DEBUGREG_H__

/* DR6: status flags */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DR6_B(bp)	(1 << (bp))	/* breakpoint was triggered */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DR6_BD		(1 << 13)	/* debug register access detected */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DR6_BS		(1 << 14)	/* single step */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DR6_BT		(1 << 15)	/* task switch */

/* DR7: control flags */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ	DR7_L(bp)	(1 << (2*(bp)))		/* breakpoint armed locally */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ	DR7_G(bp)	(1 << (1+2*(bp)))	/* breakpoint armed globally */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ	DR7_LE		(1 << 8)		/* exact local breakpoints */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ	DR7_GE		(1 << 9)		/* exact global breakpoints */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ	DR7_GD		(1 << 13)		/* detect debug reg movs */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ	DR7_RW_ਨਕਾਬ(bp)		(3 << (16+4*(bp)))	
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ	DR7_RW_EXEC(bp)		(0 << (16+4*(bp)))	/* execute */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ	DR7_RW_WRITE(bp)	(1 << (16+4*(bp)))	/* write */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ	DR7_RW_IO(bp)		(2 << (16+4*(bp)))	/* IO */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ	DR7_RW_RW(bp)		(3 << (16+4*(bp)))	/* read or write */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ	DR7_LN_ਨਕਾਬ(bp)	(3 << (18+4*(bp)))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ	DR7_LN_1(bp)	(0 << (18+4*(bp)))	/* 1 byte */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ	DR7_LN_2(bp)	(1 << (18+4*(bp)))	/* 2 bytes */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ	DR7_LN_8(bp)	(2 << (18+4*(bp)))	/* 8 bytes */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ	DR7_LN_4(bp)	(3 << (18+4*(bp)))	/* 4 bytes */

/* debugreg.S */
ਖਾਲੀ ld_dr0(phys_ਬਾਈਟਾਂ ਮੁੱਲ);
ਖਾਲੀ ld_dr1(phys_ਬਾਈਟਾਂ ਮੁੱਲ);
ਖਾਲੀ ld_dr2(phys_ਬਾਈਟਾਂ ਮੁੱਲ);
ਖਾਲੀ ld_dr3(phys_ਬਾਈਟਾਂ ਮੁੱਲ);
ਖਾਲੀ ld_dr6(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਮੁੱਲ);
ਖਾਲੀ ld_dr7(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਮੁੱਲ);
phys_ਬਾਈਟਾਂ st_dr0(ਖਾਲੀ); 
phys_ਬਾਈਟਾਂ st_dr1(ਖਾਲੀ); 
phys_ਬਾਈਟਾਂ st_dr2(ਖਾਲੀ); 
phys_ਬਾਈਟਾਂ st_dr3(ਖਾਲੀ); 
ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ st_dr6(ਖਾਲੀ); 
ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ st_dr7(ਖਾਲੀ); 

#ਜੇ_ਅੰਤ /* __DEBUGREG_H__ */

