
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/kernel.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "direct_utils.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "bsp_serial.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "glo.h"

ਖਾਲੀ ਸਿੱਧਾ_cls(ਖਾਲੀ)
{
    /* Do nothing */
}

ਖਾਲੀ ਸਿੱਧਾ_print_char(ਅੱਖਰ c)
{
	ਜੇ(c == '\n')
		bsp_ser_putc('\r');
	ser_putc(c);
}

ਖਾਲੀ ਸਿੱਧਾ_print(ਅਚਲ ਅੱਖਰ *str)
{
	ਜਦੋਂ (*str) {
		ਸਿੱਧਾ_print_char(*str);
		str++;
	}
}

ਪੂਰਨ ਸਿੱਧਾ_read_char(ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ch)
{
	ਵਾਪਸ 0;
}
