
#شامل <minix/minlib.h>
#شامل <minix/cpufeature.h>
#شامل <machine/partition.h>
#شامل "string.h"
#شامل "direct_utils.h"
#شامل "serial.h"
#شامل "glo.h"

/* Give non-zero values to avoid them in BSS */
ساکن صحیح print_سطر = 1, print_ستون = 1;

#شامل <sys/video.h>

خارجی حرف *video_mem;
#تعریف_کرو VIDOFFSET(سطر, ستون) ((سطر) * MULTIBOOT_CONSOLE_ستون_تعداد * 2 + (ستون) * 2)
#تعریف_کرو VIDSIZE VIDOFFSET(MULTIBOOT_CONSOLE_سطریں-1,MULTIBOOT_CONSOLE_ستون_تعداد-1)

خالی براہ_راست_put_char(حرف c, صحیح سطر, صحیح ستون) 
{
	صحیح انحراف = VIDOFFSET(سطر, ستون);
	video_mem[انحراف] = c;
	video_mem[انحراف+1] = 0x07;	/* grey-on-black */
}

ساکن حرف براہ_راست_get_char(صحیح سطر, صحیح ستون) 
{
	واپس video_mem[VIDOFFSET(سطر, ستون)];
}

خالی براہ_راست_cls(خالی)
{
	/* Clear screen */
	صحیح i,j;

	برائے(i = 0; i < MULTIBOOT_CONSOLE_ستون_تعداد; i++)
		برائے(j = 0; j < MULTIBOOT_CONSOLE_سطریں; j++)
			براہ_راست_put_char(' ', j, i);

	print_سطر = print_ستون = 0;

	/* Tell video hardware origin is 0. */
	outb(C_6845+اشاریہ_2, VID_ORG);
	outb(C_6845+ڈیٹا_2, 0);
	outb(C_6845+اشاریہ_2, VID_ORG+1);
	outb(C_6845+ڈیٹا_2, 0);
}

ساکن خالی براہ_راست_scroll_اوپر(صحیح سطریں) 
{
	صحیح i, j;
	برائے (i = 0; i < MULTIBOOT_CONSOLE_سطریں; i++ ) {
		برائے (j = 0; j < MULTIBOOT_CONSOLE_ستون_تعداد; j++ ) {
			حرف c = 0;
			اگر(i < MULTIBOOT_CONSOLE_سطریں-سطریں)
				c = براہ_راست_get_char(i + سطریں, j);
			براہ_راست_put_char(c, i, j);
		}
	}
	print_سطر-= سطریں;
}

خالی براہ_راست_print_char(حرف c)
{
	جب (print_سطر >= MULTIBOOT_CONSOLE_سطریں)
		براہ_راست_scroll_اوپر(1);

#تعریف_کرو TABWIDTH 8
	اگر(c == '\t') {
		اگر(print_ستون >= MULTIBOOT_CONSOLE_ستون_تعداد - TABWIDTH) {
			c = '\n';
		} ورنہ {
			کرو {
				براہ_راست_put_char(' ', print_سطر, print_ستون++);
			} جب(print_ستون % TABWIDTH);
			واپس;
		}
	}

	اگر (c == '\n') {
		جب (print_ستون < MULTIBOOT_CONSOLE_ستون_تعداد)
			براہ_راست_put_char(' ', print_سطر, print_ستون++);
		print_سطر++;
		print_ستون = 0;
		واپس;
	}

	براہ_راست_put_char(c, print_سطر, print_ستون++);

	اگر (print_ستون >= MULTIBOOT_CONSOLE_ستون_تعداد) {
		print_سطر++;
		print_ستون = 0;
	}

	جب (print_سطر >= MULTIBOOT_CONSOLE_سطریں)
		براہ_راست_scroll_اوپر(1);
}

خالی براہ_راست_print(ثابت حرف *str)
{
	جب (*str) {
		براہ_راست_print_char(*str);
		str++;
	}
}

/* Standard and AT keyboard.  (PS/2 MCA implies AT throughout.) */
#تعریف_کرو KEYBD		0x60	/* I/O port for keyboard data */
#تعریف_کرو KB_حالت	0x64	/* I/O port for status on AT */
#تعریف_کرو KB_برآمد_بھرا	0x01	/* status bit set when keypress char pending */
#تعریف_کرو KB_AUX_BYTE	0x20	/* Auxiliary Device Output Buffer Full */

صحیح براہ_راست_read_char(غیرعلامتی حرف *ch)
{
	غیرعلامتی طویل sb;

	sb = inb(KB_حالت);

	اگر (!(sb & KB_برآمد_بھرا)) {
		واپس 0;
	}

	inb(KEYBD);

	اگر (!(sb & KB_AUX_BYTE))
		واپس 1;

	واپس 0;
}

