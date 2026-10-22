
#include <minix/minlib.h>
#include <minix/cpufeature.h>
#include <machine/partition.h>
#include "string.h"
#include "direct_utils.h"
#include "serial.h"
#include "glo.h"

/* Give non-zero values to avoid them in BSS */
static ကိန်းပြည့် print_line = 1, print_col = 1;

#include <sys/video.h>

extern var_na *video_mem;
#define VIDOFFSET(line, col) ((line) * MULTIBOOT_CONSOLE_COLS * 2 + (col) * 2)
#define VIDSIZE VIDOFFSET(MULTIBOOT_CONSOLE_LINES-1,MULTIBOOT_CONSOLE_COLS-1)

void direct_put_char(var_na c, ကိန်းပြည့် line, ကိန်းပြည့် col) 
{
	ကိန်းပြည့် offset = VIDOFFSET(line, col);
	video_mem[offset] = c;
	video_mem[offset+1] = 0x07;	/* grey-on-black */
}

static var_na direct_get_char(ကိန်းပြည့် line, ကိန်းပြည့် col) 
{
	ပြန်ပေး video_mem[VIDOFFSET(line, col)];
}

void direct_cls(void)
{
	/* Clear screen */
	ကိန်းပြည့် i,j;

	အတွက်(i = 0; i < MULTIBOOT_CONSOLE_COLS; i++)
		အတွက်(j = 0; j < MULTIBOOT_CONSOLE_LINES; j++)
			direct_put_char(' ', j, i);

	print_line = print_col = 0;

	/* Tell video hardware origin is 0. */
	outb(C_6845+INDEX, VID_ORG);
	outb(C_6845+DATA, 0);
	outb(C_6845+INDEX, VID_ORG+1);
	outb(C_6845+DATA, 0);
}

static void direct_scroll_up(ကိန်းပြည့် lines) 
{
	ကိန်းပြည့် i, j;
	အတွက် (i = 0; i < MULTIBOOT_CONSOLE_LINES; i++ ) {
		အတွက် (j = 0; j < MULTIBOOT_CONSOLE_COLS; j++ ) {
			var_na c = 0;
			အကယ်၍(i < MULTIBOOT_CONSOLE_LINES-lines)
				c = direct_get_char(i + lines, j);
			direct_put_char(c, i, j);
		}
	}
	print_line-= lines;
}

void direct_print_char(var_na c)
{
	နေစဉ် (print_line >= MULTIBOOT_CONSOLE_LINES)
		direct_scroll_up(1);

#define TABWIDTH 8
	အကယ်၍(c == '\t') {
		အကယ်၍(print_col >= MULTIBOOT_CONSOLE_COLS - TABWIDTH) {
			c = '\n';
		} မဟုတ်ပါက {
			do {
				direct_put_char(' ', print_line, print_col++);
			} နေစဉ်(print_col % TABWIDTH);
			ပြန်ပေး;
		}
	}

	အကယ်၍ (c == '\n') {
		နေစဉ် (print_col < MULTIBOOT_CONSOLE_COLS)
			direct_put_char(' ', print_line, print_col++);
		print_line++;
		print_col = 0;
		ပြန်ပေး;
	}

	direct_put_char(c, print_line, print_col++);

	အကယ်၍ (print_col >= MULTIBOOT_CONSOLE_COLS) {
		print_line++;
		print_col = 0;
	}

	နေစဉ် (print_line >= MULTIBOOT_CONSOLE_LINES)
		direct_scroll_up(1);
}

void direct_print(const var_na *str)
{
	နေစဉ် (*str) {
		direct_print_char(*str);
		str++;
	}
}

/* Standard and AT keyboard.  (PS/2 MCA implies AT throughout.) */
#define KEYBD		0x60	/* I/O port for keyboard data */
#define KB_STATUS	0x64	/* I/O port for status on AT */
#define KB_OUT_FULL	0x01	/* status bit set when keypress char pending */
#define KB_AUX_BYTE	0x20	/* Auxiliary Device Output Buffer Full */

ကိန်းပြည့် direct_read_char(unsigned var_na *ch)
{
	unsigned long sb;

	sb = inb(KB_STATUS);

	အကယ်၍ (!(sb & KB_OUT_FULL)) {
		ပြန်ပေး 0;
	}

	inb(KEYBD);

	အကယ်၍ (!(sb & KB_AUX_BYTE))
		ပြန်ပေး 1;

	ပြန်ပေး 0;
}

