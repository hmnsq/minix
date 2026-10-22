#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ _CCNT_H
#ವ್ಯಾಖ್ಯಾನಿಸು _CCNT_H

/* ARMV7 PMU (performance monitors) */
/* ARM ARM B4.1.116 */
#ವ್ಯಾಖ್ಯಾನಿಸು PMU_PMCNTENSET_C	(1 << 31)  /* Enable PMCCNTR cycle counter */

/* ARM ARM B4.1.117 PMCR */
#ವ್ಯಾಖ್ಯಾನಿಸು PMU_PMCR_DP		(1 << 5) /* Disable when ev. cnt. prohibited */
#ವ್ಯಾಖ್ಯಾನಿಸು PMU_PMCR_X		(1 << 4) /* Export enable */
#ವ್ಯಾಖ್ಯಾನಿಸು PMU_PMCR_D		(1 << 3) /* Clock divider */
#ವ್ಯಾಖ್ಯಾನಿಸು PMU_PMCR_C		(1 << 2) /* Cycle counter reset */
#ವ್ಯಾಖ್ಯಾನಿಸು PMU_PMCR_P		(1 << 1) /* Event counter reset */
#ವ್ಯಾಖ್ಯಾನಿಸು PMU_PMCR_E		(1 << 0) /* Enable event counters */

/* ARM ARM B4.1.119 PMINTENSET */
#ವ್ಯಾಖ್ಯಾನಿಸು PMU_PMINTENSET_C	(1 << 31) /* PMCCNTR overflow int req. enable*/

/* ARM ARM B4.1.124 PMUSERENR */
#ವ್ಯಾಖ್ಯಾನಿಸು PMU_PMUSERENR_EN	(1 << 0) /* User mode access enable bit */

#ಆದರೆ_ಅಂತ್ಯ /* _CCNT_H */
