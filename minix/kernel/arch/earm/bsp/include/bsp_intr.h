#ifndef _BSP_INTR_H_
#define _BSP_INTR_H_

#ifndef __ASSEMBLY__

void bsp_irq_unmask(ຈຳນວນເຕັມ irq);
void bsp_irq_mask(ຈຳນວນເຕັມ irq);
void bsp_irq_handle(void);

#endif /* __ASSEMBLY__ */

#endif /* _BSP_INTR_H_ */
