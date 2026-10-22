
#అన్తర్భావ <minix/minlib.h>
#అన్తర్భావ <minix/cpufeature.h>
#అన్తర్భావ <machine/partition.h>
#అన్తర్భావ "string.h"
#అన్తర్భావ "direct_utils.h"
#అన్తర్భావ "serial.h"
#అన్తర్భావ "glo.h"

/* Give non-zero values to avoid them in BSS */
స్థిర పూర్ణాంకం print_పంక్తి = 1, print_నిలువు = 1;

#అన్తర్భావ <sys/video.h>

బాహ్య అక్షరం *video_mem;
#నిర్వచించు VIDOFFSET(పంక్తి, నిలువు) ((పంక్తి) * MULTIBOOT_CONSOLE_నిలువుల_సంఖ్య * 2 + (నిలువు) * 2)
#నిర్వచించు VIDSIZE VIDOFFSET(MULTIBOOT_CONSOLE_పంక్తులు-1,MULTIBOOT_CONSOLE_నిలువుల_సంఖ్య-1)

శూన్య ప్రత్యక్ష_put_char(అక్షరం c, పూర్ణాంకం పంక్తి, పూర్ణాంకం నిలువు) 
{
	పూర్ణాంకం స్థానభ్రంశం = VIDOFFSET(పంక్తి, నిలువు);
	video_mem[స్థానభ్రంశం] = c;
	video_mem[స్థానభ్రంశం+1] = 0x07;	/* grey-on-black */
}

స్థిర అక్షరం ప్రత్యక్ష_get_char(పూర్ణాంకం పంక్తి, పూర్ణాంకం నిలువు) 
{
	తిరిగి video_mem[VIDOFFSET(పంక్తి, నిలువు)];
}

శూన్య ప్రత్యక్ష_cls(శూన్య)
{
	/* Clear screen */
	పూర్ణాంకం i,j;

	కొరకు(i = 0; i < MULTIBOOT_CONSOLE_నిలువుల_సంఖ్య; i++)
		కొరకు(j = 0; j < MULTIBOOT_CONSOLE_పంక్తులు; j++)
			ప్రత్యక్ష_put_char(' ', j, i);

	print_పంక్తి = print_నిలువు = 0;

	/* Tell video hardware origin is 0. */
	outb(C_6845+సూచీ_2, VID_ORG);
	outb(C_6845+దత్తాంశం_2, 0);
	outb(C_6845+సూచీ_2, VID_ORG+1);
	outb(C_6845+దత్తాంశం_2, 0);
}

స్థిర శూన్య ప్రత్యక్ష_scroll_పైకి(పూర్ణాంకం పంక్తులు) 
{
	పూర్ణాంకం i, j;
	కొరకు (i = 0; i < MULTIBOOT_CONSOLE_పంక్తులు; i++ ) {
		కొరకు (j = 0; j < MULTIBOOT_CONSOLE_నిలువుల_సంఖ్య; j++ ) {
			అక్షరం c = 0;
			ఒకవేళ(i < MULTIBOOT_CONSOLE_పంక్తులు-పంక్తులు)
				c = ప్రత్యక్ష_get_char(i + పంక్తులు, j);
			ప్రత్యక్ష_put_char(c, i, j);
		}
	}
	print_పంక్తి-= పంక్తులు;
}

శూన్య ప్రత్యక్ష_print_char(అక్షరం c)
{
	అయితే (print_పంక్తి >= MULTIBOOT_CONSOLE_పంక్తులు)
		ప్రత్యక్ష_scroll_పైకి(1);

#నిర్వచించు TABWIDTH 8
	ఒకవేళ(c == '\t') {
		ఒకవేళ(print_నిలువు >= MULTIBOOT_CONSOLE_నిలువుల_సంఖ్య - TABWIDTH) {
			c = '\n';
		} లేకపోతే {
			చేయి {
				ప్రత్యక్ష_put_char(' ', print_పంక్తి, print_నిలువు++);
			} అయితే(print_నిలువు % TABWIDTH);
			తిరిగి;
		}
	}

	ఒకవేళ (c == '\n') {
		అయితే (print_నిలువు < MULTIBOOT_CONSOLE_నిలువుల_సంఖ్య)
			ప్రత్యక్ష_put_char(' ', print_పంక్తి, print_నిలువు++);
		print_పంక్తి++;
		print_నిలువు = 0;
		తిరిగి;
	}

	ప్రత్యక్ష_put_char(c, print_పంక్తి, print_నిలువు++);

	ఒకవేళ (print_నిలువు >= MULTIBOOT_CONSOLE_నిలువుల_సంఖ్య) {
		print_పంక్తి++;
		print_నిలువు = 0;
	}

	అయితే (print_పంక్తి >= MULTIBOOT_CONSOLE_పంక్తులు)
		ప్రత్యక్ష_scroll_పైకి(1);
}

శూన్య ప్రత్యక్ష_print(స్థిరరాశి అక్షరం *str)
{
	అయితే (*str) {
		ప్రత్యక్ష_print_char(*str);
		str++;
	}
}

/* Standard and AT keyboard.  (PS/2 MCA implies AT throughout.) */
#నిర్వచించు KEYBD		0x60	/* I/O port for keyboard data */
#నిర్వచించు KB_స్థితి	0x64	/* I/O port for status on AT */
#నిర్వచించు KB_బహిర్గతం_నిండిన	0x01	/* status bit set when keypress char pending */
#నిర్వచించు KB_AUX_BYTE	0x20	/* Auxiliary Device Output Buffer Full */

పూర్ణాంకం ప్రత్యక్ష_read_char(గుర్తులేని అక్షరం *ch)
{
	గుర్తులేని దీర్ఘ sb;

	sb = inb(KB_స్థితి);

	ఒకవేళ (!(sb & KB_బహిర్గతం_నిండిన)) {
		తిరిగి 0;
	}

	inb(KEYBD);

	ఒకవేళ (!(sb & KB_AUX_BYTE))
		తిరిగి 1;

	తిరిగి 0;
}

