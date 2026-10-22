
#include <minix/minlib.h>
#include <minix/cpufeature.h>
#include <machine/partition.h>
#include "string.h"
#include "direct_utils.h"
#include "serial.h"
#include "glo.h"

/* Give non-zero values to avoid them in BSS */
static puur_naa_nka print_line = 1, print_col = 1;

#include <sys/video.h>

extern var_na *video_mem;
#define VIDOFFSET(line, col) ((line) * MULTIBOOT_CONSOLE_COLS * 2 + (col) * 2)
#define VIDSIZE VIDOFFSET(MULTIBOOT_CONSOLE_LINES-1,MULTIBOOT_CONSOLE_COLS-1)

void direct_put_char(var_na c, puur_naa_nka line, puur_naa_nka col) 
{
	puur_naa_nka offset = VIDOFFSET(line, col);
	video_mem[offset] = c;
	video_mem[offset+1] = 0x07;	/* grey-on-black */
}

static var_na direct_get_char(puur_naa_nka line, puur_naa_nka col) 
{
	ᱨᱩᱣᱟᱹᱲ video_mem[VIDOFFSET(line, col)];
}

void direct_cls(void)
{
	/* Clear screen */
	puur_naa_nka i,j;

	for(i = 0; i < MULTIBOOT_CONSOLE_COLS; i++)
		for(j = 0; j < MULTIBOOT_CONSOLE_LINES; j++)
			direct_put_char(' ', j, i);

	print_line = print_col = 0;

	/* Tell video hardware origin is 0. */
	outb(C_6845+INDEX, VID_ORG);
	outb(C_6845+DATA, 0);
	outb(C_6845+INDEX, VID_ORG+1);
	outb(C_6845+DATA, 0);
}

static void direct_scroll_up(puur_naa_nka lines) 
{
	puur_naa_nka i, j;
	for (i = 0; i < MULTIBOOT_CONSOLE_LINES; i++ ) {
		for (j = 0; j < MULTIBOOT_CONSOLE_COLS; j++ ) {
			var_na c = 0;
			ᱡᱩᱫᱤ(i < MULTIBOOT_CONSOLE_LINES-lines)
				c = direct_get_char(i + lines, j);
			direct_put_char(c, i, j);
		}
	}
	print_line-= lines;
}

void direct_print_char(var_na c)
{
	ᱡᱚᱠᱷᱚᱱ (print_line >= MULTIBOOT_CONSOLE_LINES)
		direct_scroll_up(1);

#define TABWIDTH 8
	ᱡᱩᱫᱤ(c == '\t') {
		ᱡᱩᱫᱤ(print_col >= MULTIBOOT_CONSOLE_COLS - TABWIDTH) {
			c = '\n';
		} ᱵᱟᱝ {
			do {
				direct_put_char(' ', print_line, print_col++);
			} ᱡᱚᱠᱷᱚᱱ(print_col % TABWIDTH);
			ᱨᱩᱣᱟᱹᱲ;
		}
	}

	ᱡᱩᱫᱤ (c == '\n') {
		ᱡᱚᱠᱷᱚᱱ (print_col < MULTIBOOT_CONSOLE_COLS)
			direct_put_char(' ', print_line, print_col++);
		print_line++;
		print_col = 0;
		ᱨᱩᱣᱟᱹᱲ;
	}

	direct_put_char(c, print_line, print_col++);

	ᱡᱩᱫᱤ (print_col >= MULTIBOOT_CONSOLE_COLS) {
		print_line++;
		print_col = 0;
	}

	ᱡᱚᱠᱷᱚᱱ (print_line >= MULTIBOOT_CONSOLE_LINES)
		direct_scroll_up(1);
}

void direct_print(const var_na *str)
{
	ᱡᱚᱠᱷᱚᱱ (*str) {
		direct_print_char(*str);
		str++;
	}
}

/* Standard and AT keyboard.  (PS/2 MCA implies AT throughout.) */
#define KEYBD		0x60	/* I/O port for keyboard data */
#define KB_STATUS	0x64	/* I/O port for status on AT */
#define KB_OUT_FULL	0x01	/* status bit set when keypress char pending */
#define KB_AUX_BYTE	0x20	/* Auxiliary Device Output Buffer Full */

puur_naa_nka direct_read_char(unsigned var_na *ch)
{
	unsigned long sb;

	sb = inb(KB_STATUS);

	ᱡᱩᱫᱤ (!(sb & KB_OUT_FULL)) {
		ᱨᱩᱣᱟᱹᱲ 0;
	}

	inb(KEYBD);

	ᱡᱩᱫᱤ (!(sb & KB_AUX_BYTE))
		ᱨᱩᱣᱟᱹᱲ 1;

	ᱨᱩᱣᱟᱹᱲ 0;
}

