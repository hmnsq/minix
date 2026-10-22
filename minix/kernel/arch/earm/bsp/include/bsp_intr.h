#ifndef _BSP_INTR_H_
#define _BSP_INTR_H_

#ifndef __ASSEMBLY__

শূন্য bsp_irq_unmask(পূর্ণসংখ্যা irq);
শূন্য bsp_irq_mask(পূর্ণসংখ্যা irq);
শূন্য bsp_irq_handle(শূন্য);

#endif /* __ASSEMBLY__ */

#endif /* _BSP_INTR_H_ */
