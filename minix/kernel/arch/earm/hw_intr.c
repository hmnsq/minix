/* hw_intr handles the hardware dependent part of the interrupts */
#ಅನ್ತರ್ಭಾವ "hw_intr.h"
#ಅನ್ತರ್ಭಾವ "bsp_intr.h"

ಶೂನ್ಯ hw_intr_ಮುಖವಾಡ(ಪೂರ್ಣಾಂಕ irq){
	bsp_irq_ಮುಖವಾಡ(irq);
}

ಶೂನ್ಯ hw_intr_unmask(ಪೂರ್ಣಾಂಕ irq){
	bsp_irq_unmask(irq);
}

ಶೂನ್ಯ hw_intr_ack(ಪೂರ್ಣಾಂಕ irq){};
ಶೂನ್ಯ hw_intr_ಬಳಸಿದ(ಪೂರ್ಣಾಂಕ irq){};
ಶೂನ್ಯ hw_intr_not_ಬಳಸಿದ(ಪೂರ್ಣಾಂಕ irq){};
ಶೂನ್ಯ hw_intr_disable_all(ಶೂನ್ಯ){};
