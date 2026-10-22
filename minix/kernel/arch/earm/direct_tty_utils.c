
#অন্তর্ভাৱ "kernel/kernel.h"
#অন্তর্ভাৱ "direct_utils.h"
#অন্তর্ভাৱ "bsp_serial.h"
#অন্তর্ভাৱ "glo.h"

শূন্য direct_cls(শূন্য)
{
    /* Do nothing */
}

শূন্য direct_print_char(ৱর্ণ c)
{
	যদি(c == '\n')
		bsp_ser_putc('\r');
	ser_putc(c);
}

শূন্য direct_print(ধ্রুৱ ৱর্ণ *str)
{
	যেতিয়ালৈ (*str) {
		direct_print_char(*str);
		str++;
	}
}

পূর্ণাঙ্ক direct_read_char(unsigned ৱর্ণ *ch)
{
	ঘূৰাই 0;
}
