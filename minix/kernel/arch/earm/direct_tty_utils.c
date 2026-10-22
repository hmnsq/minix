
#ضمن "kernel/kernel.h"
#ضمن "direct_utils.h"
#ضمن "bsp_serial.h"
#ضمن "glo.h"

فراغ direct_cls(فراغ)
{
    /* Do nothing */
}

فراغ direct_print_char(حرف c)
{
	إذا(c == '\n')
		bsp_ser_putc('\r');
	ser_putc(c);
}

فراغ direct_print(ثابت حرف *str)
{
	بينما (*str) {
		direct_print_char(*str);
		str++;
	}
}

صحيح direct_read_char(unsigned حرف *ch)
{
	إرجاع 0;
}
