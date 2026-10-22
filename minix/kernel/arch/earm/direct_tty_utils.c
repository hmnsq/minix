
#ಅನ್ತರ್ಭಾವ "kernel/kernel.h"
#ಅನ್ತರ್ಭಾವ "direct_utils.h"
#ಅನ್ತರ್ಭಾವ "bsp_serial.h"
#ಅನ್ತರ್ಭಾವ "glo.h"

ಶೂನ್ಯ direct_cls(ಶೂನ್ಯ)
{
    /* Do nothing */
}

ಶೂನ್ಯ direct_print_char(ವರ್ಣ c)
{
	ಯದಿ(c == '\n')
		bsp_ser_putc('\r');
	ser_putc(c);
}

ಶೂನ್ಯ direct_print(ಧ್ರುವ ವರ್ಣ *str)
{
	ಯಾವತ್ (*str) {
		direct_print_char(*str);
		str++;
	}
}

ಪೂರ್ಣಾಙ್ಕ direct_read_char(unsigned ವರ್ಣ *ch)
{
	ಪ್ರತ್ಯಾಗಮ 0;
}
