
#ಅನ್ತರ್ಭಾವ "kernel/kernel.h"
#ಅನ್ತರ್ಭಾವ "direct_utils.h"
#ಅನ್ತರ್ಭಾವ "bsp_serial.h"
#ಅನ್ತರ್ಭಾವ "glo.h"

ಶೂನ್ಯ ನೇರ_cls(ಶೂನ್ಯ)
{
    /* Do nothing */
}

ಶೂನ್ಯ ನೇರ_print_char(ಅಕ್ಷರ c)
{
	ಆದರೆ(c == '\n')
		bsp_ser_putc('\r');
	ser_putc(c);
}

ಶೂನ್ಯ ನೇರ_print(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *str)
{
	ಆಗ (*str) {
		ನೇರ_print_char(*str);
		str++;
	}
}

ಪೂರ್ಣಾಂಕ ನೇರ_read_char(ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ch)
{
	ಹಿಂತಿರುಗಿ 0;
}
