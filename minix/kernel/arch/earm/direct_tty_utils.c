
#شامل "kernel/kernel.h"
#شامل "direct_utils.h"
#شامل "bsp_serial.h"
#شامل "glo.h"

خالی براہ_راست_cls(خالی)
{
    /* Do nothing */
}

خالی براہ_راست_print_char(حرف c)
{
	اگر(c == '\n')
		bsp_ser_putc('\r');
	ser_putc(c);
}

خالی براہ_راست_print(ثابت حرف *str)
{
	جب (*str) {
		براہ_راست_print_char(*str);
		str++;
	}
}

صحیح براہ_راست_read_char(غیرعلامتی حرف *ch)
{
	واپس 0;
}
