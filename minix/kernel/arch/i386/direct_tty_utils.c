
#ضمن <minix/minlib.h>
#ضمن <minix/cpufeature.h>
#ضمن <machine/partition.h>
#ضمن "string.h"
#ضمن "direct_utils.h"
#ضمن "serial.h"
#ضمن "glo.h"

/* Give non-zero values to avoid them in BSS */
ساكن صحيح print_کرښه = 1, print_ستون = 1;

#ضمن <sys/video.h>

خارجي حرف *video_mem;
#تعریف_کړه VIDOFFSET(کرښه, ستون) ((کرښه) * MULTIBOOT_CONSOLE_ستون_شمېر * 2 + (ستون) * 2)
#تعریف_کړه VIDSIZE VIDOFFSET(MULTIBOOT_CONSOLE_کرښې-1,MULTIBOOT_CONSOLE_ستون_شمېر-1)

فراغ مستقیم_put_char(حرف c, صحيح کرښه, صحيح ستون) 
{
	صحيح دننه_واټن = VIDOFFSET(کرښه, ستون);
	video_mem[دننه_واټن] = c;
	video_mem[دننه_واټن+1] = 0x07;	/* grey-on-black */
}

ساكن حرف مستقیم_get_char(صحيح کرښه, صحيح ستون) 
{
	أرجع video_mem[VIDOFFSET(کرښه, ستون)];
}

فراغ مستقیم_cls(فراغ)
{
	/* Clear screen */
	صحيح i,j;

	لكل(i = 0; i < MULTIBOOT_CONSOLE_ستون_شمېر; i++)
		لكل(j = 0; j < MULTIBOOT_CONSOLE_کرښې; j++)
			مستقیم_put_char(' ', j, i);

	print_کرښه = print_ستون = 0;

	/* Tell video hardware origin is 0. */
	outb(C_6845+شاخص_2, VID_ORG);
	outb(C_6845+معلومات_2, 0);
	outb(C_6845+شاخص_2, VID_ORG+1);
	outb(C_6845+معلومات_2, 0);
}

ساكن فراغ مستقیم_scroll_پورته(صحيح کرښې) 
{
	صحيح i, j;
	لكل (i = 0; i < MULTIBOOT_CONSOLE_کرښې; i++ ) {
		لكل (j = 0; j < MULTIBOOT_CONSOLE_ستون_شمېر; j++ ) {
			حرف c = 0;
			إذا(i < MULTIBOOT_CONSOLE_کرښې-کرښې)
				c = مستقیم_get_char(i + کرښې, j);
			مستقیم_put_char(c, i, j);
		}
	}
	print_کرښه-= کرښې;
}

فراغ مستقیم_print_char(حرف c)
{
	طالما (print_کرښه >= MULTIBOOT_CONSOLE_کرښې)
		مستقیم_scroll_پورته(1);

#تعریف_کړه TABWIDTH 8
	إذا(c == '\t') {
		إذا(print_ستون >= MULTIBOOT_CONSOLE_ستون_شمېر - TABWIDTH) {
			c = '\n';
		} وإلا {
			افعل {
				مستقیم_put_char(' ', print_کرښه, print_ستون++);
			} طالما(print_ستون % TABWIDTH);
			أرجع;
		}
	}

	إذا (c == '\n') {
		طالما (print_ستون < MULTIBOOT_CONSOLE_ستون_شمېر)
			مستقیم_put_char(' ', print_کرښه, print_ستون++);
		print_کرښه++;
		print_ستون = 0;
		أرجع;
	}

	مستقیم_put_char(c, print_کرښه, print_ستون++);

	إذا (print_ستون >= MULTIBOOT_CONSOLE_ستون_شمېر) {
		print_کرښه++;
		print_ستون = 0;
	}

	طالما (print_کرښه >= MULTIBOOT_CONSOLE_کرښې)
		مستقیم_scroll_پورته(1);
}

فراغ مستقیم_print(ثابت حرف *str)
{
	طالما (*str) {
		مستقیم_print_char(*str);
		str++;
	}
}

/* Standard and AT keyboard.  (PS/2 MCA implies AT throughout.) */
#تعریف_کړه KEYBD		0x60	/* I/O port for keyboard data */
#تعریف_کړه KB_حالت	0x64	/* I/O port for status on AT */
#تعریف_کړه KB_بهر_ډک	0x01	/* status bit set when keypress char pending */
#تعریف_کړه KB_AUX_BYTE	0x20	/* Auxiliary Device Output Buffer Full */

صحيح مستقیم_read_char(بینښه حرف *ch)
{
	بینښه اوږد sb;

	sb = inb(KB_حالت);

	إذا (!(sb & KB_بهر_ډک)) {
		أرجع 0;
	}

	inb(KEYBD);

	إذا (!(sb & KB_AUX_BYTE))
		أرجع 1;

	أرجع 0;
}

