#ifndef __HW_INTR_ARM_H__
#define __HW_INTR_ARM_H__


#ಅನ್ತರ್ಭಾವ "kernel/kernel.h"
ಶೂನ್ಯ irq_handle(ಪೂರ್ಣಾಙ್ಕ irq);

ಶೂನ್ಯ hw_intr_mask(ಪೂರ್ಣಾಙ್ಕ irq);
ಶೂನ್ಯ hw_intr_unmask(ಪೂರ್ಣಾಙ್ಕ irq);
ಶೂನ್ಯ hw_intr_ack(ಪೂರ್ಣಾಙ್ಕ irq);
ಶೂನ್ಯ hw_intr_used(ಪೂರ್ಣಾಙ್ಕ irq);
ಶೂನ್ಯ hw_intr_not_used(ಪೂರ್ಣಾಙ್ಕ irq);
ಶೂನ್ಯ hw_intr_disable_all(ಶೂನ್ಯ);

#endif /* __HW_INTR_ARM_H__ */
