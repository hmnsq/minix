#جيڪڏهن_وصف_نه_ٿيل _CCNT_H
#وصف_ڪريو _CCNT_H

/* ARMV7 PMU (performance monitors) */
/* ARM ARM B4.1.116 */
#وصف_ڪريو PMU_PMCNTENSET_C	(1 << 31)  /* Enable PMCCNTR cycle counter */

/* ARM ARM B4.1.117 PMCR */
#وصف_ڪريو PMU_PMCR_DP		(1 << 5) /* Disable when ev. cnt. prohibited */
#وصف_ڪريو PMU_PMCR_X		(1 << 4) /* Export enable */
#وصف_ڪريو PMU_PMCR_D		(1 << 3) /* Clock divider */
#وصف_ڪريو PMU_PMCR_C		(1 << 2) /* Cycle counter reset */
#وصف_ڪريو PMU_PMCR_P		(1 << 1) /* Event counter reset */
#وصف_ڪريو PMU_PMCR_E		(1 << 0) /* Enable event counters */

/* ARM ARM B4.1.119 PMINTENSET */
#وصف_ڪريو PMU_PMINTENSET_C	(1 << 31) /* PMCCNTR overflow int req. enable*/

/* ARM ARM B4.1.124 PMUSERENR */
#وصف_ڪريو PMU_PMUSERENR_EN	(1 << 0) /* User mode access enable bit */

#جيڪڏهن_پورو /* _CCNT_H */
