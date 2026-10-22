
#समावेश "kernel/kernel.h"
#समावेश "direct_utils.h"
#समावेश "bsp_serial.h"
#समावेश "glo.h"

व्योम direct_cls(व्योम)
{
    /* Do nothing */
}

व्योम direct_print_char(अक्षर c)
{
	अगर(c == '\n')
		bsp_ser_putc('\r');
	ser_putc(c);
}

व्योम direct_print(स्थिर अक्षर *str)
{
	जबतक (*str) {
		direct_print_char(*str);
		str++;
	}
}

पूर्णांक direct_read_char(अचिन्हित अक्षर *ch)
{
	वापस 0;
}
