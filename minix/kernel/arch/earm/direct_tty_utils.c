
#അന്തര്ഭാവ "kernel/kernel.h"
#അന്തര്ഭാവ "direct_utils.h"
#അന്തര്ഭാവ "bsp_serial.h"
#അന്തര്ഭാവ "glo.h"

ശൂന്യം direct_cls(ശൂന്യം)
{
    /* Do nothing */
}

ശൂന്യം direct_print_char(അക്ഷരം c)
{
	എങ്കിൽ(c == '\n')
		bsp_ser_putc('\r');
	ser_putc(c);
}

ശൂന്യം direct_print(സ്ഥിരം അക്ഷരം *str)
{
	ആകുമ്പോൾ (*str) {
		direct_print_char(*str);
		str++;
	}
}

പൂർണ്ണസംഖ്യ direct_read_char(unsigned അക്ഷരം *ch)
{
	തിരികെ 0;
}
