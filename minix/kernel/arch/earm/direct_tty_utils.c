
#ଅନ୍ତର୍ଭାଵ "kernel/kernel.h"
#ଅନ୍ତର୍ଭାଵ "direct_utils.h"
#ଅନ୍ତର୍ଭାଵ "bsp_serial.h"
#ଅନ୍ତର୍ଭାଵ "glo.h"

ଶୂନ୍ୟ direct_cls(ଶୂନ୍ୟ)
{
    /* Do nothing */
}

ଶୂନ୍ୟ direct_print_char(ଅକ୍ଷର c)
{
	ଯଦି(c == '\n')
		bsp_ser_putc('\r');
	ser_putc(c);
}

ଶୂନ୍ୟ direct_print(ଅଚଳ ଅକ୍ଷର *str)
{
	ଯେତେବେଳେ (*str) {
		direct_print_char(*str);
		str++;
	}
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା direct_read_char(unsigned ଅକ୍ଷର *ch)
{
	ଫେରନ୍ତୁ 0;
}
