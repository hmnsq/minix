
#અન્તર્ભાવ "kernel/kernel.h"
#અન્તર્ભાવ "direct_utils.h"
#અન્તર્ભાવ "bsp_serial.h"
#અન્તર્ભાવ "glo.h"

શૂન્ય direct_cls(શૂન્ય)
{
    /* Do nothing */
}

શૂન્ય direct_print_char(અક્ષર c)
{
	જો(c == '\n')
		bsp_ser_putc('\r');
	ser_putc(c);
}

શૂન્ય direct_print(અચલ અક્ષર *str)
{
	જ્યારે (*str) {
		direct_print_char(*str);
		str++;
	}
}

પૂર્ણાંક direct_read_char(unsigned અક્ષર *ch)
{
	પાછા 0;
}
