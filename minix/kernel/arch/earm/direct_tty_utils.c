
#ضمن "kernel/kernel.h"
#ضمن "direct_utils.h"
#ضمن "bsp_serial.h"
#ضمن "glo.h"

فراغ مستقیم_cls(فراغ)
{
    /* Do nothing */
}

فراغ مستقیم_print_char(حرف c)
{
	إذا(c == '\n')
		bsp_ser_putc('\r');
	ser_putc(c);
}

فراغ مستقیم_print(ثابت حرف *str)
{
	طالما (*str) {
		مستقیم_print_char(*str);
		str++;
	}
}

صحيح مستقیم_read_char(بیعلامت حرف *ch)
{
	أرجع 0;
}
