
#include <minix/minlib.h>
#include <minix/cpufeature.h>
#include <machine/partition.h>
#include "string.h"
#include "direct_utils.h"
#include "serial.h"
#include "glo.h"

/* Give non-zero values to avoid them in BSS */
static ចំនួនគត់ print_line = 1, print_col = 1;

#include <sys/video.h>

extern var_na *video_mem;
#define VIDOFFSET(line, col) ((line) * MULTIBOOT_CONSOLE_COLS * 2 + (col) * 2)
#define VIDSIZE VIDOFFSET(MULTIBOOT_CONSOLE_LINES-1,MULTIBOOT_CONSOLE_COLS-1)

void direct_put_char(var_na c, ចំនួនគត់ line, ចំនួនគត់ col) 
{
	ចំនួនគត់ offset = VIDOFFSET(line, col);
	video_mem[offset] = c;
	video_mem[offset+1] = 0x07;	/* grey-on-black */
}

static var_na direct_get_char(ចំនួនគត់ line, ចំនួនគត់ col) 
{
	ត្រឡប់ video_mem[VIDOFFSET(line, col)];
}

void direct_cls(void)
{
	/* Clear screen */
	ចំនួនគត់ i,j;

	សម្រាប់(i = 0; i < MULTIBOOT_CONSOLE_COLS; i++)
		សម្រាប់(j = 0; j < MULTIBOOT_CONSOLE_LINES; j++)
			direct_put_char(' ', j, i);

	print_line = print_col = 0;

	/* Tell video hardware origin is 0. */
	outb(C_6845+INDEX, VID_ORG);
	outb(C_6845+DATA, 0);
	outb(C_6845+INDEX, VID_ORG+1);
	outb(C_6845+DATA, 0);
}

static void direct_scroll_up(ចំនួនគត់ lines) 
{
	ចំនួនគត់ i, j;
	សម្រាប់ (i = 0; i < MULTIBOOT_CONSOLE_LINES; i++ ) {
		សម្រាប់ (j = 0; j < MULTIBOOT_CONSOLE_COLS; j++ ) {
			var_na c = 0;
			ប្រសិនបើ(i < MULTIBOOT_CONSOLE_LINES-lines)
				c = direct_get_char(i + lines, j);
			direct_put_char(c, i, j);
		}
	}
	print_line-= lines;
}

void direct_print_char(var_na c)
{
	ខណៈពេល (print_line >= MULTIBOOT_CONSOLE_LINES)
		direct_scroll_up(1);

#define TABWIDTH 8
	ប្រសិនបើ(c == '\t') {
		ប្រសិនបើ(print_col >= MULTIBOOT_CONSOLE_COLS - TABWIDTH) {
			c = '\n';
		} បើមិនដូច្នេះ {
			do {
				direct_put_char(' ', print_line, print_col++);
			} ខណៈពេល(print_col % TABWIDTH);
			ត្រឡប់;
		}
	}

	ប្រសិនបើ (c == '\n') {
		ខណៈពេល (print_col < MULTIBOOT_CONSOLE_COLS)
			direct_put_char(' ', print_line, print_col++);
		print_line++;
		print_col = 0;
		ត្រឡប់;
	}

	direct_put_char(c, print_line, print_col++);

	ប្រសិនបើ (print_col >= MULTIBOOT_CONSOLE_COLS) {
		print_line++;
		print_col = 0;
	}

	ខណៈពេល (print_line >= MULTIBOOT_CONSOLE_LINES)
		direct_scroll_up(1);
}

void direct_print(const var_na *str)
{
	ខណៈពេល (*str) {
		direct_print_char(*str);
		str++;
	}
}

/* Standard and AT keyboard.  (PS/2 MCA implies AT throughout.) */
#define KEYBD		0x60	/* I/O port for keyboard data */
#define KB_STATUS	0x64	/* I/O port for status on AT */
#define KB_OUT_FULL	0x01	/* status bit set when keypress char pending */
#define KB_AUX_BYTE	0x20	/* Auxiliary Device Output Buffer Full */

ចំនួនគត់ direct_read_char(unsigned var_na *ch)
{
	unsigned long sb;

	sb = inb(KB_STATUS);

	ប្រសិនបើ (!(sb & KB_OUT_FULL)) {
		ត្រឡប់ 0;
	}

	inb(KEYBD);

	ប្រសិនបើ (!(sb & KB_AUX_BYTE))
		ត្រឡប់ 1;

	ត្រឡប់ 0;
}

