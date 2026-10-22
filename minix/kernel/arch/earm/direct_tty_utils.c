
#ضمن "kernel/kernel.h"
#ضمن "direct_utils.h"
#ضمن "bsp_serial.h"
#ضمن "glo.h"

فراغ سڌو_cls(فراغ)
{
    /* Do nothing */
}

فراغ سڌو_print_char(حرف c)
{
	إذا(c == '\n')
		bsp_ser_putc('\r');
	ser_putc(c);
}

فراغ سڌو_print(ثابت حرف *str)
{
	طالما (*str) {
		سڌو_print_char(*str);
		str++;
	}
}

صحيح سڌو_read_char(بنانشان حرف *ch)
{
	أرجع 0;
}
