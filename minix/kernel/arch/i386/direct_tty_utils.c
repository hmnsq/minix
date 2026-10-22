
#include <minix/minlib.h>
#include <minix/cpufeature.h>
#include <machine/partition.h>
#include "string.h"
#include "direct_utils.h"
#include "serial.h"
#include "glo.h"

/* Give non-zero values to avoid them in BSS */
static số_nguyên print_line = 1, print_col = 1;

#include <sys/video.h>

extern char *video_mem;
#define VIDOFFSET(line, col) ((line) * MULTIBOOT_CONSOLE_COLS * 2 + (col) * 2)
#define VIDSIZE VIDOFFSET(MULTIBOOT_CONSOLE_LINES-1,MULTIBOOT_CONSOLE_COLS-1)

void direct_put_char(char c, số_nguyên line, số_nguyên col) 
{
	số_nguyên offset = VIDOFFSET(line, col);
	video_mem[offset] = c;
	video_mem[offset+1] = 0x07;	/* grey-on-black */
}

static char direct_get_char(số_nguyên line, số_nguyên col) 
{
	trả_về video_mem[VIDOFFSET(line, col)];
}

void direct_cls(void)
{
	/* Clear screen */
	số_nguyên i,j;

	cho(i = 0; i < MULTIBOOT_CONSOLE_COLS; i++)
		cho(j = 0; j < MULTIBOOT_CONSOLE_LINES; j++)
			direct_put_char(' ', j, i);

	print_line = print_col = 0;

	/* Tell video hardware origin is 0. */
	outb(C_6845+INDEX, VID_ORG);
	outb(C_6845+DATA, 0);
	outb(C_6845+INDEX, VID_ORG+1);
	outb(C_6845+DATA, 0);
}

static void direct_scroll_up(số_nguyên lines) 
{
	số_nguyên i, j;
	cho (i = 0; i < MULTIBOOT_CONSOLE_LINES; i++ ) {
		cho (j = 0; j < MULTIBOOT_CONSOLE_COLS; j++ ) {
			char c = 0;
			nếu(i < MULTIBOOT_CONSOLE_LINES-lines)
				c = direct_get_char(i + lines, j);
			direct_put_char(c, i, j);
		}
	}
	print_line-= lines;
}

void direct_print_char(char c)
{
	trong_khi (print_line >= MULTIBOOT_CONSOLE_LINES)
		direct_scroll_up(1);

#define TABWIDTH 8
	nếu(c == '\t') {
		nếu(print_col >= MULTIBOOT_CONSOLE_COLS - TABWIDTH) {
			c = '\n';
		} không_thì {
			do {
				direct_put_char(' ', print_line, print_col++);
			} trong_khi(print_col % TABWIDTH);
			trả_về;
		}
	}

	nếu (c == '\n') {
		trong_khi (print_col < MULTIBOOT_CONSOLE_COLS)
			direct_put_char(' ', print_line, print_col++);
		print_line++;
		print_col = 0;
		trả_về;
	}

	direct_put_char(c, print_line, print_col++);

	nếu (print_col >= MULTIBOOT_CONSOLE_COLS) {
		print_line++;
		print_col = 0;
	}

	trong_khi (print_line >= MULTIBOOT_CONSOLE_LINES)
		direct_scroll_up(1);
}

void direct_print(const char *str)
{
	trong_khi (*str) {
		direct_print_char(*str);
		str++;
	}
}

/* Standard and AT keyboard.  (PS/2 MCA implies AT throughout.) */
#define KEYBD		0x60	/* I/O port for keyboard data */
#define KB_STATUS	0x64	/* I/O port for status on AT */
#define KB_OUT_FULL	0x01	/* status bit set when keypress char pending */
#define KB_AUX_BYTE	0x20	/* Auxiliary Device Output Buffer Full */

số_nguyên direct_read_char(unsigned char *ch)
{
	unsigned long sb;

	sb = inb(KB_STATUS);

	nếu (!(sb & KB_OUT_FULL)) {
		trả_về 0;
	}

	inb(KEYBD);

	nếu (!(sb & KB_AUX_BYTE))
		trả_về 1;

	trả_về 0;
}

