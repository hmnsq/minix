
#ضمن "kernel/kernel.h"
#ضمن "direct_utils.h"
#ضمن "bsp_serial.h"
#ضمن "glo.h"

فراغ سدھا_cls(فراغ)
{
    /* Do nothing */
}

فراغ سدھا_print_char(حرف c)
{
	إذا(c == '\n')
		bsp_ser_putc('\r');
	ser_putc(c);
}

فراغ سدھا_print(ثابت حرف *str)
{
	طالما (*str) {
		سدھا_print_char(*str);
		str++;
	}
}

صحيح سدھا_read_char(بنانشانی حرف *ch)
{
	أرجع 0;
}
