
#شامل "kernel/kernel.h"
#شامل "direct_utils.h"
#شامل "bsp_serial.h"
#شامل "glo.h"

تهی direct_cls(تهی)
{
    /* Do nothing */
}

تهی direct_print_char(نویسه c)
{
	اگر(c == '\n')
		bsp_ser_putc('\r');
	ser_putc(c);
}

تهی direct_print(ثابت نویسه *str)
{
	وقتی (*str) {
		direct_print_char(*str);
		str++;
	}
}

صحیح direct_read_char(unsigned نویسه *ch)
{
	بازگشت 0;
}
