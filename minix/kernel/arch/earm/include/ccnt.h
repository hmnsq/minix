#నిర్వచితం_కాకపోతే _CCNT_H
#నిర్వచించు _CCNT_H

/* ARMV7 PMU (performance monitors) */
/* ARM ARM B4.1.116 */
#నిర్వచించు PMU_PMCNTENSET_C	(1 << 31)  /* Enable PMCCNTR cycle counter */

/* ARM ARM B4.1.117 PMCR */
#నిర్వచించు PMU_PMCR_DP		(1 << 5) /* Disable when ev. cnt. prohibited */
#నిర్వచించు PMU_PMCR_X		(1 << 4) /* Export enable */
#నిర్వచించు PMU_PMCR_D		(1 << 3) /* Clock divider */
#నిర్వచించు PMU_PMCR_C		(1 << 2) /* Cycle counter reset */
#నిర్వచించు PMU_PMCR_P		(1 << 1) /* Event counter reset */
#నిర్వచించు PMU_PMCR_E		(1 << 0) /* Enable event counters */

/* ARM ARM B4.1.119 PMINTENSET */
#నిర్వచించు PMU_PMINTENSET_C	(1 << 31) /* PMCCNTR overflow int req. enable*/

/* ARM ARM B4.1.124 PMUSERENR */
#నిర్వచించు PMU_PMUSERENR_EN	(1 << 0) /* User mode access enable bit */

#అయితే_ముగింపు /* _CCNT_H */
