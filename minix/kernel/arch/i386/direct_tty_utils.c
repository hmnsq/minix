
#ضمن <minix/minlib.h>
#ضمن <minix/cpufeature.h>
#ضمن <machine/partition.h>
#ضمن "string.h"
#ضمن "direct_utils.h"
#ضمن "serial.h"
#ضمن "glo.h"

/* Give non-zero values to avoid them in BSS */
ساكن صحيح print_line = 1, print_col = 1;

#ضمن <sys/video.h>

خارجي حرف *video_mem;
#define VIDOFFSET(line, col) ((line) * MULTIBOOT_CONSOLE_COLS * 2 + (col) * 2)
#define VIDSIZE VIDOFFSET(MULTIBOOT_CONSOLE_LINES-1,MULTIBOOT_CONSOLE_COLS-1)

فراغ direct_put_char(حرف c, صحيح line, صحيح col) 
{
	صحيح offset = VIDOFFSET(line, col);
	video_mem[offset] = c;
	video_mem[offset+1] = 0x07;	/* grey-on-black */
}

ساكن حرف direct_get_char(صحيح line, صحيح col) 
{
	החזר video_mem[VIDOFFSET(line, col)];
}

فراغ direct_cls(فراغ)
{
	/* Clear screen */
	صحيح i,j;

	עבור(i = 0; i < MULTIBOOT_CONSOLE_COLS; i++)
		עבור(j = 0; j < MULTIBOOT_CONSOLE_LINES; j++)
			direct_put_char(' ', j, i);

	print_line = print_col = 0;

	/* Tell video hardware origin is 0. */
	outb(C_6845+INDEX, VID_ORG);
	outb(C_6845+DATA, 0);
	outb(C_6845+INDEX, VID_ORG+1);
	outb(C_6845+DATA, 0);
}

ساكن فراغ direct_scroll_up(صحيح lines) 
{
	صحيح i, j;
	עבור (i = 0; i < MULTIBOOT_CONSOLE_LINES; i++ ) {
		עבור (j = 0; j < MULTIBOOT_CONSOLE_COLS; j++ ) {
			حرف c = 0;
			אם(i < MULTIBOOT_CONSOLE_LINES-lines)
				c = direct_get_char(i + lines, j);
			direct_put_char(c, i, j);
		}
	}
	print_line-= lines;
}

فراغ direct_print_char(حرف c)
{
	כלעוד (print_line >= MULTIBOOT_CONSOLE_LINES)
		direct_scroll_up(1);

#define TABWIDTH 8
	אם(c == '\t') {
		אם(print_col >= MULTIBOOT_CONSOLE_COLS - TABWIDTH) {
			c = '\n';
		} אחרת {
			עשה {
				direct_put_char(' ', print_line, print_col++);
			} כלעוד(print_col % TABWIDTH);
			החזר;
		}
	}

	אם (c == '\n') {
		כלעוד (print_col < MULTIBOOT_CONSOLE_COLS)
			direct_put_char(' ', print_line, print_col++);
		print_line++;
		print_col = 0;
		החזר;
	}

	direct_put_char(c, print_line, print_col++);

	אם (print_col >= MULTIBOOT_CONSOLE_COLS) {
		print_line++;
		print_col = 0;
	}

	כלעוד (print_line >= MULTIBOOT_CONSOLE_LINES)
		direct_scroll_up(1);
}

فراغ direct_print(ثابت حرف *str)
{
	כלעוד (*str) {
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

	אם (!(sb & KB_OUT_FULL)) {
		החזר 0;
	}

	inb(KEYBD);

	אם (!(sb & KB_AUX_BYTE))
		החזר 1;

	החזר 0;
}

