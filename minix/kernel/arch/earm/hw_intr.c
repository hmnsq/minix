/* hw_intr handles the hardware dependent part of the interrupts */
#ಅನ್ತರ್ಭಾವ "hw_intr.h"
#ಅನ್ತರ್ಭಾವ "bsp_intr.h"

ಶೂನ್ಯ hw_intr_mask(ಪೂರ್ಣಾಙ್ಕ irq){
	bsp_irq_mask(irq);
}

ಶೂನ್ಯ hw_intr_unmask(ಪೂರ್ಣಾಙ್ಕ irq){
	bsp_irq_unmask(irq);
}

ಶೂನ್ಯ hw_intr_ack(ಪೂರ್ಣಾಙ್ಕ irq){};
ಶೂನ್ಯ hw_intr_used(ಪೂರ್ಣಾಙ್ಕ irq){};
ಶೂನ್ಯ hw_intr_not_used(ಪೂರ್ಣಾಙ್ಕ irq){};
ಶೂನ್ಯ hw_intr_disable_all(ಶೂನ್ಯ){};
