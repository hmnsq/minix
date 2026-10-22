
#ضمن <minix/minlib.h>
#ضمن <minix/cpufeature.h>
#ضمن <machine/partition.h>
#ضمن "string.h"
#ضمن "direct_utils.h"
#ضمن "serial.h"
#ضمن "glo.h"

/* Give non-zero values to avoid them in BSS */
ساكن صحيح print_سطر = 1, print_ستون = 1;

#ضمن <sys/video.h>

خارجي حرف *video_mem;
#تعریف_کن VIDOFFSET(سطر, ستون) ((سطر) * MULTIBOOT_CONSOLE_تعداد_ستون * 2 + (ستون) * 2)
#تعریف_کن VIDSIZE VIDOFFSET(MULTIBOOT_CONSOLE_سطرها-1,MULTIBOOT_CONSOLE_تعداد_ستون-1)

فراغ مستقیم_put_char(حرف c, صحيح سطر, صحيح ستون) 
{
	صحيح جابه_جایی = VIDOFFSET(سطر, ستون);
	video_mem[جابه_جایی] = c;
	video_mem[جابه_جایی+1] = 0x07;	/* grey-on-black */
}

ساكن حرف مستقیم_get_char(صحيح سطر, صحيح ستون) 
{
	أرجع video_mem[VIDOFFSET(سطر, ستون)];
}

فراغ مستقیم_cls(فراغ)
{
	/* Clear screen */
	صحيح i,j;

	لكل(i = 0; i < MULTIBOOT_CONSOLE_تعداد_ستون; i++)
		لكل(j = 0; j < MULTIBOOT_CONSOLE_سطرها; j++)
			مستقیم_put_char(' ', j, i);

	print_سطر = print_ستون = 0;

	/* Tell video hardware origin is 0. */
	outb(C_6845+شاخص_2, VID_ORG);
	outb(C_6845+داده_2, 0);
	outb(C_6845+شاخص_2, VID_ORG+1);
	outb(C_6845+داده_2, 0);
}

ساكن فراغ مستقیم_scroll_به_بالا(صحيح سطرها) 
{
	صحيح i, j;
	لكل (i = 0; i < MULTIBOOT_CONSOLE_سطرها; i++ ) {
		لكل (j = 0; j < MULTIBOOT_CONSOLE_تعداد_ستون; j++ ) {
			حرف c = 0;
			إذا(i < MULTIBOOT_CONSOLE_سطرها-سطرها)
				c = مستقیم_get_char(i + سطرها, j);
			مستقیم_put_char(c, i, j);
		}
	}
	print_سطر-= سطرها;
}

فراغ مستقیم_print_char(حرف c)
{
	طالما (print_سطر >= MULTIBOOT_CONSOLE_سطرها)
		مستقیم_scroll_به_بالا(1);

#تعریف_کن TABWIDTH 8
	إذا(c == '\t') {
		إذا(print_ستون >= MULTIBOOT_CONSOLE_تعداد_ستون - TABWIDTH) {
			c = '\n';
		} وإلا {
			افعل {
				مستقیم_put_char(' ', print_سطر, print_ستون++);
			} طالما(print_ستون % TABWIDTH);
			أرجع;
		}
	}

	إذا (c == '\n') {
		طالما (print_ستون < MULTIBOOT_CONSOLE_تعداد_ستون)
			مستقیم_put_char(' ', print_سطر, print_ستون++);
		print_سطر++;
		print_ستون = 0;
		أرجع;
	}

	مستقیم_put_char(c, print_سطر, print_ستون++);

	إذا (print_ستون >= MULTIBOOT_CONSOLE_تعداد_ستون) {
		print_سطر++;
		print_ستون = 0;
	}

	طالما (print_سطر >= MULTIBOOT_CONSOLE_سطرها)
		مستقیم_scroll_به_بالا(1);
}

فراغ مستقیم_print(ثابت حرف *str)
{
	طالما (*str) {
		مستقیم_print_char(*str);
		str++;
	}
}

/* Standard and AT keyboard.  (PS/2 MCA implies AT throughout.) */
#تعریف_کن KEYBD		0x60	/* I/O port for keyboard data */
#تعریف_کن KB_وضعیت	0x64	/* I/O port for status on AT */
#تعریف_کن KB_بیرون_پر	0x01	/* status bit set when keypress char pending */
#تعریف_کن KB_AUX_BYTE	0x20	/* Auxiliary Device Output Buffer Full */

صحيح مستقیم_read_char(بیعلامت حرف *ch)
{
	بیعلامت دراز sb;

	sb = inb(KB_وضعیت);

	إذا (!(sb & KB_بیرون_پر)) {
		أرجع 0;
	}

	inb(KEYBD);

	إذا (!(sb & KB_AUX_BYTE))
		أرجع 1;

	أرجع 0;
}

