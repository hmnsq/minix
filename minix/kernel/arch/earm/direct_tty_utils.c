
#அந்தர்பாவ "kernel/kernel.h"
#அந்தர்பாவ "direct_utils.h"
#அந்தர்பாவ "bsp_serial.h"
#அந்தர்பாவ "glo.h"

வெறுமை direct_cls(வெறுமை)
{
    /* Do nothing */
}

வெறுமை direct_print_char(எழுத்து c)
{
	எனில்(c == '\n')
		bsp_ser_putc('\r');
	ser_putc(c);
}

வெறுமை direct_print(மாறிலி எழுத்து *str)
{
	போது (*str) {
		direct_print_char(*str);
		str++;
	}
}

முழு direct_read_char(unsigned எழுத்து *ch)
{
	திரும்பு 0;
}
