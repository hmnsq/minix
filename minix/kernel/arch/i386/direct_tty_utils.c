
#include <minix/minlib.h>
#include <minix/cpufeature.h>
#include <machine/partition.h>
#include "string.h"
#include "direct_utils.h"
#include "serial.h"
#include "glo.h"

/* Give non-zero values to avoid them in BSS */
static цяло print_line = 1, print_col = 1;

#include <sys/video.h>

extern char *video_mem;
#define VIDOFFSET(line, col) ((line) * MULTIBOOT_CONSOLE_COLS * 2 + (col) * 2)
#define VIDSIZE VIDOFFSET(MULTIBOOT_CONSOLE_LINES-1,MULTIBOOT_CONSOLE_COLS-1)

void direct_put_char(char c, цяло line, цяло col) 
{
	цяло offset = VIDOFFSET(line, col);
	video_mem[offset] = c;
	video_mem[offset+1] = 0x07;	/* grey-on-black */
}

static char direct_get_char(цяло line, цяло col) 
{
	върни video_mem[VIDOFFSET(line, col)];
}

void direct_cls(void)
{
	/* Clear screen */
	цяло i,j;

	за(i = 0; i < MULTIBOOT_CONSOLE_COLS; i++)
		за(j = 0; j < MULTIBOOT_CONSOLE_LINES; j++)
			direct_put_char(' ', j, i);

	print_line = print_col = 0;

	/* Tell video hardware origin is 0. */
	outb(C_6845+INDEX, VID_ORG);
	outb(C_6845+DATA, 0);
	outb(C_6845+INDEX, VID_ORG+1);
	outb(C_6845+DATA, 0);
}

static void direct_scroll_up(цяло lines) 
{
	цяло i, j;
	за (i = 0; i < MULTIBOOT_CONSOLE_LINES; i++ ) {
		за (j = 0; j < MULTIBOOT_CONSOLE_COLS; j++ ) {
			char c = 0;
			ако(i < MULTIBOOT_CONSOLE_LINES-lines)
				c = direct_get_char(i + lines, j);
			direct_put_char(c, i, j);
		}
	}
	print_line-= lines;
}

void direct_print_char(char c)
{
	докато (print_line >= MULTIBOOT_CONSOLE_LINES)
		direct_scroll_up(1);

#define TABWIDTH 8
	ако(c == '\t') {
		ако(print_col >= MULTIBOOT_CONSOLE_COLS - TABWIDTH) {
			c = '\n';
		} иначе {
			do {
				direct_put_char(' ', print_line, print_col++);
			} докато(print_col % TABWIDTH);
			върни;
		}
	}

	ако (c == '\n') {
		докато (print_col < MULTIBOOT_CONSOLE_COLS)
			direct_put_char(' ', print_line, print_col++);
		print_line++;
		print_col = 0;
		върни;
	}

	direct_put_char(c, print_line, print_col++);

	ако (print_col >= MULTIBOOT_CONSOLE_COLS) {
		print_line++;
		print_col = 0;
	}

	докато (print_line >= MULTIBOOT_CONSOLE_LINES)
		direct_scroll_up(1);
}

void direct_print(const char *str)
{
	докато (*str) {
		direct_print_char(*str);
		str++;
	}
}

/* Standard and AT keyboard.  (PS/2 MCA implies AT throughout.) */
#define KEYBD		0x60	/* I/O port for keyboard data */
#define KB_STATUS	0x64	/* I/O port for status on AT */
#define KB_OUT_FULL	0x01	/* status bit set when keypress char pending */
#define KB_AUX_BYTE	0x20	/* Auxiliary Device Output Buffer Full */

цяло direct_read_char(unsigned char *ch)
{
	unsigned long sb;

	sb = inb(KB_STATUS);

	ако (!(sb & KB_OUT_FULL)) {
		върни 0;
	}

	inb(KEYBD);

	ако (!(sb & KB_AUX_BYTE))
		върни 1;

	върни 0;
}

