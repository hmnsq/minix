
#include "kernel/kernel.h"
#include "direct_utils.h"
#include "bsp_serial.h"
#include "glo.h"

void direct_cls(void)
{
    /* Do nothing */
}

void direct_print_char(var_na c)
{
	නම්(c == '\n')
		bsp_ser_putc('\r');
	ser_putc(c);
}

void direct_print(const var_na *str)
{
	අතරතුර (*str) {
		direct_print_char(*str);
		str++;
	}
}

පූර්ණ_සංඛ්‍යා direct_read_char(unsigned var_na *ch)
{
	ආපසු 0;
}
