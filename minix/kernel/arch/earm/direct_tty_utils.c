
#అన్తర్భావ "kernel/kernel.h"
#అన్తర్భావ "direct_utils.h"
#అన్తర్భావ "bsp_serial.h"
#అన్తర్భావ "glo.h"

శూన్య ప్రత్యక్ష_cls(శూన్య)
{
    /* Do nothing */
}

శూన్య ప్రత్యక్ష_print_char(అక్షరం c)
{
	ఒకవేళ(c == '\n')
		bsp_ser_putc('\r');
	ser_putc(c);
}

శూన్య ప్రత్యక్ష_print(స్థిరరాశి అక్షరం *str)
{
	అయితే (*str) {
		ప్రత్యక్ష_print_char(*str);
		str++;
	}
}

పూర్ణాంకం ప్రత్యక్ష_read_char(గుర్తులేని అక్షరం *ch)
{
	తిరిగి 0;
}
