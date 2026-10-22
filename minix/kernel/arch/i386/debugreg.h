#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ __DEBUGREG_H__
#ವ್ಯಾಖ್ಯಾನಿಸು __DEBUGREG_H__

/* DR6: status flags */
#ವ್ಯಾಖ್ಯಾನಿಸು DR6_B(bp)	(1 << (bp))	/* breakpoint was triggered */
#ವ್ಯಾಖ್ಯಾನಿಸು DR6_BD		(1 << 13)	/* debug register access detected */
#ವ್ಯಾಖ್ಯಾನಿಸು DR6_BS		(1 << 14)	/* single step */
#ವ್ಯಾಖ್ಯಾನಿಸು DR6_BT		(1 << 15)	/* task switch */

/* DR7: control flags */
#ವ್ಯಾಖ್ಯಾನಿಸು	DR7_L(bp)	(1 << (2*(bp)))		/* breakpoint armed locally */
#ವ್ಯಾಖ್ಯಾನಿಸು	DR7_G(bp)	(1 << (1+2*(bp)))	/* breakpoint armed globally */
#ವ್ಯಾಖ್ಯಾನಿಸು	DR7_LE		(1 << 8)		/* exact local breakpoints */
#ವ್ಯಾಖ್ಯಾನಿಸು	DR7_GE		(1 << 9)		/* exact global breakpoints */
#ವ್ಯಾಖ್ಯಾನಿಸು	DR7_GD		(1 << 13)		/* detect debug reg movs */

#ವ್ಯಾಖ್ಯಾನಿಸು	DR7_RW_ಮುಖವಾಡ(bp)		(3 << (16+4*(bp)))	
#ವ್ಯಾಖ್ಯಾನಿಸು	DR7_RW_EXEC(bp)		(0 << (16+4*(bp)))	/* execute */
#ವ್ಯಾಖ್ಯಾನಿಸು	DR7_RW_WRITE(bp)	(1 << (16+4*(bp)))	/* write */
#ವ್ಯಾಖ್ಯಾನಿಸು	DR7_RW_IO(bp)		(2 << (16+4*(bp)))	/* IO */
#ವ್ಯಾಖ್ಯಾನಿಸು	DR7_RW_RW(bp)		(3 << (16+4*(bp)))	/* read or write */

#ವ್ಯಾಖ್ಯಾನಿಸು	DR7_LN_ಮುಖವಾಡ(bp)	(3 << (18+4*(bp)))
#ವ್ಯಾಖ್ಯಾನಿಸು	DR7_LN_1(bp)	(0 << (18+4*(bp)))	/* 1 byte */
#ವ್ಯಾಖ್ಯಾನಿಸು	DR7_LN_2(bp)	(1 << (18+4*(bp)))	/* 2 bytes */
#ವ್ಯಾಖ್ಯಾನಿಸು	DR7_LN_8(bp)	(2 << (18+4*(bp)))	/* 8 bytes */
#ವ್ಯಾಖ್ಯಾನಿಸು	DR7_LN_4(bp)	(3 << (18+4*(bp)))	/* 4 bytes */

/* debugreg.S */
ಶೂನ್ಯ ld_dr0(phys_ಬೈಟುಗಳು ಮೌಲ್ಯ);
ಶೂನ್ಯ ld_dr1(phys_ಬೈಟುಗಳು ಮೌಲ್ಯ);
ಶೂನ್ಯ ld_dr2(phys_ಬೈಟುಗಳು ಮೌಲ್ಯ);
ಶೂನ್ಯ ld_dr3(phys_ಬೈಟುಗಳು ಮೌಲ್ಯ);
ಶೂನ್ಯ ld_dr6(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಮೌಲ್ಯ);
ಶೂನ್ಯ ld_dr7(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಮೌಲ್ಯ);
phys_ಬೈಟುಗಳು st_dr0(ಶೂನ್ಯ); 
phys_ಬೈಟುಗಳು st_dr1(ಶೂನ್ಯ); 
phys_ಬೈಟುಗಳು st_dr2(ಶೂನ್ಯ); 
phys_ಬೈಟುಗಳು st_dr3(ಶೂನ್ಯ); 
ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ st_dr6(ಶೂನ್ಯ); 
ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ st_dr7(ಶೂನ್ಯ); 

#ಆದರೆ_ಅಂತ್ಯ /* __DEBUGREG_H__ */

