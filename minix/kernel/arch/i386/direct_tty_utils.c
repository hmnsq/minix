
#ضمن <minix/minlib.h>
#ضمن <minix/cpufeature.h>
#ضمن <machine/partition.h>
#ضمن "string.h"
#ضمن "direct_utils.h"
#ضمن "serial.h"
#ضمن "glo.h"

/* Give non-zero values to avoid them in BSS */
ساكن صحيح print_سطر = 1, print_col = 1;

#ضمن <sys/video.h>

خارجي حرف *video_mem;
#define VIDOFFSET(سطر, col) ((سطر) * MULTIBOOT_CONSOLE_COLS * 2 + (col) * 2)
#define VIDSIZE VIDOFFSET(MULTIBOOT_CONSOLE_أسطر-1,MULTIBOOT_CONSOLE_COLS-1)

فراغ direct_put_char(حرف c, صحيح سطر, صحيح col) 
{
	صحيح offset = VIDOFFSET(سطر, col);
	video_mem[offset] = c;
	video_mem[offset+1] = 0x07;	/* grey-on-black */
}

ساكن حرف direct_get_char(صحيح سطر, صحيح col) 
{
	إرجاع video_mem[VIDOFFSET(سطر, col)];
}

فراغ direct_cls(فراغ)
{
	/* Clear screen */
	صحيح i,j;

	لكل(i = 0; i < MULTIBOOT_CONSOLE_COLS; i++)
		لكل(j = 0; j < MULTIBOOT_CONSOLE_أسطر; j++)
			direct_put_char(' ', j, i);

	print_سطر = print_col = 0;

	/* Tell video hardware origin is 0. */
	outb(C_6845+INDEX, VID_ORG);
	outb(C_6845+DATA, 0);
	outb(C_6845+INDEX, VID_ORG+1);
	outb(C_6845+DATA, 0);
}

ساكن فراغ direct_scroll_up(صحيح أسطر) 
{
	صحيح i, j;
	لكل (i = 0; i < MULTIBOOT_CONSOLE_أسطر; i++ ) {
		لكل (j = 0; j < MULTIBOOT_CONSOLE_COLS; j++ ) {
			حرف c = 0;
			إذا(i < MULTIBOOT_CONSOLE_أسطر-أسطر)
				c = direct_get_char(i + أسطر, j);
			direct_put_char(c, i, j);
		}
	}
	print_سطر-= أسطر;
}

فراغ direct_print_char(حرف c)
{
	بينما (print_سطر >= MULTIBOOT_CONSOLE_أسطر)
		direct_scroll_up(1);

#define TABWIDTH 8
	إذا(c == '\t') {
		إذا(print_col >= MULTIBOOT_CONSOLE_COLS - TABWIDTH) {
			c = '\n';
		} وإلا {
			افعل {
				direct_put_char(' ', print_سطر, print_col++);
			} بينما(print_col % TABWIDTH);
			إرجاع;
		}
	}

	إذا (c == '\n') {
		بينما (print_col < MULTIBOOT_CONSOLE_COLS)
			direct_put_char(' ', print_سطر, print_col++);
		print_سطر++;
		print_col = 0;
		إرجاع;
	}

	direct_put_char(c, print_سطر, print_col++);

	إذا (print_col >= MULTIBOOT_CONSOLE_COLS) {
		print_سطر++;
		print_col = 0;
	}

	بينما (print_سطر >= MULTIBOOT_CONSOLE_أسطر)
		direct_scroll_up(1);
}

فراغ direct_print(ثابت حرف *str)
{
	بينما (*str) {
		direct_print_char(*str);
		str++;
	}
}

/* Standard and AT keyboard.  (PS/2 MCA implies AT throughout.) */
#define KEYBD		0x60	/* I/O port for keyboard data */
#define KB_STATUS	0x64	/* I/O port for status on AT */
#define KB_OUT_FULL	0x01	/* status bit set when keypress char pending */
#define KB_AUX_BYTE	0x20	/* Auxiliary Device Output Buffer Full */

صحيح direct_read_char(unsigned حرف *ch)
{
	unsigned long sb;

	sb = inb(KB_STATUS);

	إذا (!(sb & KB_OUT_FULL)) {
		إرجاع 0;
	}

	inb(KEYBD);

	إذا (!(sb & KB_AUX_BYTE))
		إرجاع 1;

	إرجاع 0;
}

