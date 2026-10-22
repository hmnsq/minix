
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
	ܐܢ(c == '\n')
		bsp_ser_putc('\r');
	ser_putc(c);
}

فراغ direct_print(ثابت حرف *str)
{
	ܟܡܐ (*str) {
		direct_print_char(*str);
		str++;
	}
}

صحيح direct_read_char(unsigned حرف *ch)
{
	ܦܢܐ 0;
}
