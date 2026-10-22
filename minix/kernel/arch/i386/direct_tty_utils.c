
#অন্তর্ভাব <minix/minlib.h>
#অন্তর্ভাব <minix/cpufeature.h>
#অন্তর্ভাব <machine/partition.h>
#অন্তর্ভাব "string.h"
#অন্তর্ভাব "direct_utils.h"
#অন্তর্ভাব "serial.h"
#অন্তর্ভাব "glo.h"

/* Give non-zero values to avoid them in BSS */
স্থির পূর্ণাঙ্ক print_সারি = 1, print_col = 1;

#অন্তর্ভাব <sys/video.h>

বাহ্য বর্ণ *video_mem;
#define VIDOFFSET(সারি, col) ((সারি) * MULTIBOOT_CONSOLE_COLS * 2 + (col) * 2)
#define VIDSIZE VIDOFFSET(MULTIBOOT_CONSOLE_সারিগুলি-1,MULTIBOOT_CONSOLE_COLS-1)

শূন্য direct_put_char(বর্ণ c, পূর্ণাঙ্ক সারি, পূর্ণাঙ্ক col) 
{
	পূর্ণাঙ্ক offset = VIDOFFSET(সারি, col);
	video_mem[offset] = c;
	video_mem[offset+1] = 0x07;	/* grey-on-black */
}

স্থির বর্ণ direct_get_char(পূর্ণাঙ্ক সারি, পূর্ণাঙ্ক col) 
{
	প্রত্যাগম video_mem[VIDOFFSET(সারি, col)];
}

শূন্য direct_cls(শূন্য)
{
	/* Clear screen */
	পূর্ণাঙ্ক i,j;

	প্রতি(i = 0; i < MULTIBOOT_CONSOLE_COLS; i++)
		প্রতি(j = 0; j < MULTIBOOT_CONSOLE_সারিগুলি; j++)
			direct_put_char(' ', j, i);

	print_সারি = print_col = 0;

	/* Tell video hardware origin is 0. */
	outb(C_6845+INDEX, VID_ORG);
	outb(C_6845+DATA, 0);
	outb(C_6845+INDEX, VID_ORG+1);
	outb(C_6845+DATA, 0);
}

স্থির শূন্য direct_scroll_up(পূর্ণাঙ্ক সারিগুলি) 
{
	পূর্ণাঙ্ক i, j;
	প্রতি (i = 0; i < MULTIBOOT_CONSOLE_সারিগুলি; i++ ) {
		প্রতি (j = 0; j < MULTIBOOT_CONSOLE_COLS; j++ ) {
			বর্ণ c = 0;
			যদি(i < MULTIBOOT_CONSOLE_সারিগুলি-সারিগুলি)
				c = direct_get_char(i + সারিগুলি, j);
			direct_put_char(c, i, j);
		}
	}
	print_সারি-= সারিগুলি;
}

শূন্য direct_print_char(বর্ণ c)
{
	যাবত্ (print_সারি >= MULTIBOOT_CONSOLE_সারিগুলি)
		direct_scroll_up(1);

#define TABWIDTH 8
	যদি(c == '\t') {
		যদি(print_col >= MULTIBOOT_CONSOLE_COLS - TABWIDTH) {
			c = '\n';
		} অন্যথা {
			কুরু {
				direct_put_char(' ', print_সারি, print_col++);
			} যাবত্(print_col % TABWIDTH);
			প্রত্যাগম;
		}
	}

	যদি (c == '\n') {
		যাবত্ (print_col < MULTIBOOT_CONSOLE_COLS)
			direct_put_char(' ', print_সারি, print_col++);
		print_সারি++;
		print_col = 0;
		প্রত্যাগম;
	}

	direct_put_char(c, print_সারি, print_col++);

	যদি (print_col >= MULTIBOOT_CONSOLE_COLS) {
		print_সারি++;
		print_col = 0;
	}

	যাবত্ (print_সারি >= MULTIBOOT_CONSOLE_সারিগুলি)
		direct_scroll_up(1);
}

শূন্য direct_print(ধ্রুব বর্ণ *str)
{
	যাবত্ (*str) {
		direct_print_char(*str);
		str++;
	}
}

/* Standard and AT keyboard.  (PS/2 MCA implies AT throughout.) */
#define KEYBD		0x60	/* I/O port for keyboard data */
#define KB_STATUS	0x64	/* I/O port for status on AT */
#define KB_OUT_FULL	0x01	/* status bit set when keypress char pending */
#define KB_AUX_BYTE	0x20	/* Auxiliary Device Output Buffer Full */

পূর্ণাঙ্ক direct_read_char(unsigned বর্ণ *ch)
{
	unsigned long sb;

	sb = inb(KB_STATUS);

	যদি (!(sb & KB_OUT_FULL)) {
		প্রত্যাগম 0;
	}

	inb(KEYBD);

	যদি (!(sb & KB_AUX_BYTE))
		প্রত্যাগম 1;

	প্রত্যাগম 0;
}

