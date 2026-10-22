
#অন্তর্ভাব "kernel/kernel.h"
#অন্তর্ভাব "direct_utils.h"
#অন্তর্ভাব "bsp_serial.h"
#অন্তর্ভাব "glo.h"

শূন্য direct_cls(শূন্য)
{
    /* Do nothing */
}

শূন্য direct_print_char(বর্ণ c)
{
	যদি(c == '\n')
		bsp_ser_putc('\r');
	ser_putc(c);
}

শূন্য direct_print(ধ্রুব বর্ণ *str)
{
	যাবত্ (*str) {
		direct_print_char(*str);
		str++;
	}
}

পূর্ণাঙ্ক direct_read_char(unsigned বর্ণ *ch)
{
	প্রত্যাগম 0;
}
