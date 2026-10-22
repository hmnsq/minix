
#अन्तर्भाव "kernel/kernel.h"
#अन्तर्भाव "direct_utils.h"
#अन्तर्भाव "bsp_serial.h"
#अन्तर्भाव "glo.h"

शून्य direct_cls(शून्य)
{
    /* Do nothing */
}

शून्य direct_print_char(अक्षर c)
{
	जर(c == '\n')
		bsp_ser_putc('\r');
	ser_putc(c);
}

शून्य direct_print(स्थिरांक अक्षर *str)
{
	पर्यंत (*str) {
		direct_print_char(*str);
		str++;
	}
}

पूर्णांक direct_read_char(अचिन्हित अक्षर *ch)
{
	परत 0;
}
