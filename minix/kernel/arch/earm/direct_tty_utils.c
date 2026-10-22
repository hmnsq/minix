
#अन्तर्भाव "kernel/kernel.h"
#अन्तर्भाव "direct_utils.h"
#अन्तर्भाव "bsp_serial.h"
#अन्तर्भाव "glo.h"

शून्य प्रत्यक्षम्_cls(शून्य)
{
    /* Do nothing */
}

शून्य प्रत्यक्षम्_print_char(वर्ण c)
{
	यदि(c == '\n')
		bsp_ser_putc('\r');
	ser_putc(c);
}

शून्य प्रत्यक्षम्_print(ध्रुव वर्ण *str)
{
	यावत् (*str) {
		प्रत्यक्षम्_print_char(*str);
		str++;
	}
}

पूर्णाङ्क प्रत्यक्षम्_read_char(अचिह्नित वर्ण *ch)
{
	प्रत्यागम 0;
}
