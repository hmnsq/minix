
#ضمن <minix/minlib.h>
#ضمن <minix/cpufeature.h>
#ضمن <machine/partition.h>
#ضمن "string.h"
#ضمن "direct_utils.h"
#ضمن "serial.h"
#ضمن "glo.h"

/* Give non-zero values to avoid them in BSS */
ساكن صحيح print_سطر = 1, print_خانہ = 1;

#ضمن <sys/video.h>

خارجي حرف *video_mem;
#تعریف_کرو VIDOFFSET(سطر, خانہ) ((سطر) * MULTIBOOT_CONSOLE_خانے_گنتی * 2 + (خانہ) * 2)
#تعریف_کرو VIDSIZE VIDOFFSET(MULTIBOOT_CONSOLE_سطریں-1,MULTIBOOT_CONSOLE_خانے_گنتی-1)

فراغ سدھا_put_char(حرف c, صحيح سطر, صحيح خانہ) 
{
	صحيح وتھ = VIDOFFSET(سطر, خانہ);
	video_mem[وتھ] = c;
	video_mem[وتھ+1] = 0x07;	/* grey-on-black */
}

ساكن حرف سدھا_get_char(صحيح سطر, صحيح خانہ) 
{
	أرجع video_mem[VIDOFFSET(سطر, خانہ)];
}

فراغ سدھا_cls(فراغ)
{
	/* Clear screen */
	صحيح i,j;

	لكل(i = 0; i < MULTIBOOT_CONSOLE_خانے_گنتی; i++)
		لكل(j = 0; j < MULTIBOOT_CONSOLE_سطریں; j++)
			سدھا_put_char(' ', j, i);

	print_سطر = print_خانہ = 0;

	/* Tell video hardware origin is 0. */
	outb(C_6845+اشاریہ_2, VID_ORG);
	outb(C_6845+انکڑے_2, 0);
	outb(C_6845+اشاریہ_2, VID_ORG+1);
	outb(C_6845+انکڑے_2, 0);
}

ساكن فراغ سدھا_scroll_اتے(صحيح سطریں) 
{
	صحيح i, j;
	لكل (i = 0; i < MULTIBOOT_CONSOLE_سطریں; i++ ) {
		لكل (j = 0; j < MULTIBOOT_CONSOLE_خانے_گنتی; j++ ) {
			حرف c = 0;
			إذا(i < MULTIBOOT_CONSOLE_سطریں-سطریں)
				c = سدھا_get_char(i + سطریں, j);
			سدھا_put_char(c, i, j);
		}
	}
	print_سطر-= سطریں;
}

فراغ سدھا_print_char(حرف c)
{
	طالما (print_سطر >= MULTIBOOT_CONSOLE_سطریں)
		سدھا_scroll_اتے(1);

#تعریف_کرو TABWIDTH 8
	إذا(c == '\t') {
		إذا(print_خانہ >= MULTIBOOT_CONSOLE_خانے_گنتی - TABWIDTH) {
			c = '\n';
		} وإلا {
			افعل {
				سدھا_put_char(' ', print_سطر, print_خانہ++);
			} طالما(print_خانہ % TABWIDTH);
			أرجع;
		}
	}

	إذا (c == '\n') {
		طالما (print_خانہ < MULTIBOOT_CONSOLE_خانے_گنتی)
			سدھا_put_char(' ', print_سطر, print_خانہ++);
		print_سطر++;
		print_خانہ = 0;
		أرجع;
	}

	سدھا_put_char(c, print_سطر, print_خانہ++);

	إذا (print_خانہ >= MULTIBOOT_CONSOLE_خانے_گنتی) {
		print_سطر++;
		print_خانہ = 0;
	}

	طالما (print_سطر >= MULTIBOOT_CONSOLE_سطریں)
		سدھا_scroll_اتے(1);
}

فراغ سدھا_print(ثابت حرف *str)
{
	طالما (*str) {
		سدھا_print_char(*str);
		str++;
	}
}

/* Standard and AT keyboard.  (PS/2 MCA implies AT throughout.) */
#تعریف_کرو KEYBD		0x60	/* I/O port for keyboard data */
#تعریف_کرو KB_حالت	0x64	/* I/O port for status on AT */
#تعریف_کرو KB_باہرلا_بھریا	0x01	/* status bit set when keypress char pending */
#تعریف_کرو KB_AUX_BYTE	0x20	/* Auxiliary Device Output Buffer Full */

صحيح سدھا_read_char(بنانشانی حرف *ch)
{
	بنانشانی لما sb;

	sb = inb(KB_حالت);

	إذا (!(sb & KB_باہرلا_بھریا)) {
		أرجع 0;
	}

	inb(KEYBD);

	إذا (!(sb & KB_AUX_BYTE))
		أرجع 1;

	أرجع 0;
}

