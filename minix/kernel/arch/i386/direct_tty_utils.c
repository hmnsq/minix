
#ضمن <minix/minlib.h>
#ضمن <minix/cpufeature.h>
#ضمن <machine/partition.h>
#ضمن "string.h"
#ضمن "direct_utils.h"
#ضمن "serial.h"
#ضمن "glo.h"

/* Give non-zero values to avoid them in BSS */
ساكن صحيح print_باريس = 1, print_col = 1;

#ضمن <sys/video.h>

خارجي حرف *video_mem;
#define VIDOFFSET(باريس, col) ((باريس) * MULTIBOOT_CONSOLE_COLS * 2 + (col) * 2)
#define VIDSIZE VIDOFFSET(MULTIBOOT_CONSOLE_باريس_سموا-1,MULTIBOOT_CONSOLE_COLS-1)

فراغ direct_put_char(حرف c, صحيح باريس, صحيح col) 
{
	صحيح offset = VIDOFFSET(باريس, col);
	video_mem[offset] = c;
	video_mem[offset+1] = 0x07;	/* grey-on-black */
}

ساكن حرف direct_get_char(صحيح باريس, صحيح col) 
{
	أرجع video_mem[VIDOFFSET(باريس, col)];
}

فراغ direct_cls(فراغ)
{
	/* Clear screen */
	صحيح i,j;

	لكل(i = 0; i < MULTIBOOT_CONSOLE_COLS; i++)
		لكل(j = 0; j < MULTIBOOT_CONSOLE_باريس_سموا; j++)
			direct_put_char(' ', j, i);

	print_باريس = print_col = 0;

	/* Tell video hardware origin is 0. */
	outb(C_6845+INDEX, VID_ORG);
	outb(C_6845+DATA, 0);
	outb(C_6845+INDEX, VID_ORG+1);
	outb(C_6845+DATA, 0);
}

ساكن فراغ direct_scroll_up(صحيح باريس_سموا) 
{
	صحيح i, j;
	لكل (i = 0; i < MULTIBOOT_CONSOLE_باريس_سموا; i++ ) {
		لكل (j = 0; j < MULTIBOOT_CONSOLE_COLS; j++ ) {
			حرف c = 0;
			إذا(i < MULTIBOOT_CONSOLE_باريس_سموا-باريس_سموا)
				c = direct_get_char(i + باريس_سموا, j);
			direct_put_char(c, i, j);
		}
	}
	print_باريس-= باريس_سموا;
}

فراغ direct_print_char(حرف c)
{
	طالما (print_باريس >= MULTIBOOT_CONSOLE_باريس_سموا)
		direct_scroll_up(1);

#define TABWIDTH 8
	إذا(c == '\t') {
		إذا(print_col >= MULTIBOOT_CONSOLE_COLS - TABWIDTH) {
			c = '\n';
		} وإلا {
			افعل {
				direct_put_char(' ', print_باريس, print_col++);
			} طالما(print_col % TABWIDTH);
			أرجع;
		}
	}

	إذا (c == '\n') {
		طالما (print_col < MULTIBOOT_CONSOLE_COLS)
			direct_put_char(' ', print_باريس, print_col++);
		print_باريس++;
		print_col = 0;
		أرجع;
	}

	direct_put_char(c, print_باريس, print_col++);

	إذا (print_col >= MULTIBOOT_CONSOLE_COLS) {
		print_باريس++;
		print_col = 0;
	}

	طالما (print_باريس >= MULTIBOOT_CONSOLE_باريس_سموا)
		direct_scroll_up(1);
}

فراغ direct_print(ثابت حرف *str)
{
	طالما (*str) {
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
		أرجع 0;
	}

	inb(KEYBD);

	إذا (!(sb & KB_AUX_BYTE))
		أرجع 1;

	أرجع 0;
}

