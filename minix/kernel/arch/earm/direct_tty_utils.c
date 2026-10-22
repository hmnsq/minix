
#অন্তর্ভাব "kernel/kernel.h"
#অন্তর্ভাব "direct_utils.h"
#অন্তর্ভাব "bsp_serial.h"
#অন্তর্ভাব "glo.h"

শূন্য direct_cls(শূন্য)
{
    /* Do nothing */
}

শূন্য direct_print_char(অক্ষর c)
{
	যদি(c == '\n')
		bsp_ser_putc('\r');
	ser_putc(c);
}

শূন্য direct_print(ধ্রুবক অক্ষর *str)
{
	যখন (*str) {
		direct_print_char(*str);
		str++;
	}
}

পূর্ণসংখ্যা direct_read_char(unsigned অক্ষর *ch)
{
	ফেরত 0;
}
