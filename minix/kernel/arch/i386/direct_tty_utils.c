
#include <minix/minlib.h>
#include <minix/cpufeature.h>
#include <machine/partition.h>
#include "string.h"
#include "direct_utils.h"
#include "serial.h"
#include "glo.h"

/* Give non-zero values to avoid them in BSS */
static පූර්ණ_සංඛ්‍යා print_line = 1, print_col = 1;

#include <sys/video.h>

extern var_na *video_mem;
#define VIDOFFSET(line, col) ((line) * MULTIBOOT_CONSOLE_COLS * 2 + (col) * 2)
#define VIDSIZE VIDOFFSET(MULTIBOOT_CONSOLE_LINES-1,MULTIBOOT_CONSOLE_COLS-1)

void direct_put_char(var_na c, පූර්ණ_සංඛ්‍යා line, පූර්ණ_සංඛ්‍යා col) 
{
	පූර්ණ_සංඛ්‍යා offset = VIDOFFSET(line, col);
	video_mem[offset] = c;
	video_mem[offset+1] = 0x07;	/* grey-on-black */
}

static var_na direct_get_char(පූර්ණ_සංඛ්‍යා line, පූර්ණ_සංඛ්‍යා col) 
{
	ආපසු video_mem[VIDOFFSET(line, col)];
}

void direct_cls(void)
{
	/* Clear screen */
	පූර්ණ_සංඛ්‍යා i,j;

	සඳහා(i = 0; i < MULTIBOOT_CONSOLE_COLS; i++)
		සඳහා(j = 0; j < MULTIBOOT_CONSOLE_LINES; j++)
			direct_put_char(' ', j, i);

	print_line = print_col = 0;

	/* Tell video hardware origin is 0. */
	outb(C_6845+INDEX, VID_ORG);
	outb(C_6845+DATA, 0);
	outb(C_6845+INDEX, VID_ORG+1);
	outb(C_6845+DATA, 0);
}

static void direct_scroll_up(පූර්ණ_සංඛ්‍යා lines) 
{
	පූර්ණ_සංඛ්‍යා i, j;
	සඳහා (i = 0; i < MULTIBOOT_CONSOLE_LINES; i++ ) {
		සඳහා (j = 0; j < MULTIBOOT_CONSOLE_COLS; j++ ) {
			var_na c = 0;
			නම්(i < MULTIBOOT_CONSOLE_LINES-lines)
				c = direct_get_char(i + lines, j);
			direct_put_char(c, i, j);
		}
	}
	print_line-= lines;
}

void direct_print_char(var_na c)
{
	අතරතුර (print_line >= MULTIBOOT_CONSOLE_LINES)
		direct_scroll_up(1);

#define TABWIDTH 8
	නම්(c == '\t') {
		නම්(print_col >= MULTIBOOT_CONSOLE_COLS - TABWIDTH) {
			c = '\n';
		} නැතහොත් {
			do {
				direct_put_char(' ', print_line, print_col++);
			} අතරතුර(print_col % TABWIDTH);
			ආපසු;
		}
	}

	නම් (c == '\n') {
		අතරතුර (print_col < MULTIBOOT_CONSOLE_COLS)
			direct_put_char(' ', print_line, print_col++);
		print_line++;
		print_col = 0;
		ආපසු;
	}

	direct_put_char(c, print_line, print_col++);

	නම් (print_col >= MULTIBOOT_CONSOLE_COLS) {
		print_line++;
		print_col = 0;
	}

	අතරතුර (print_line >= MULTIBOOT_CONSOLE_LINES)
		direct_scroll_up(1);
}

void direct_print(const var_na *str)
{
	අතරතුර (*str) {
		direct_print_char(*str);
		str++;
	}
}

/* Standard and AT keyboard.  (PS/2 MCA implies AT throughout.) */
#define KEYBD		0x60	/* I/O port for keyboard data */
#define KB_STATUS	0x64	/* I/O port for status on AT */
#define KB_OUT_FULL	0x01	/* status bit set when keypress char pending */
#define KB_AUX_BYTE	0x20	/* Auxiliary Device Output Buffer Full */

පූර්ණ_සංඛ්‍යා direct_read_char(unsigned var_na *ch)
{
	unsigned long sb;

	sb = inb(KB_STATUS);

	නම් (!(sb & KB_OUT_FULL)) {
		ආපසු 0;
	}

	inb(KEYBD);

	නම් (!(sb & KB_AUX_BYTE))
		ආපසු 1;

	ආපසු 0;
}

