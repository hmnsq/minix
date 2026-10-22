
#অন্তর্ভাৱ <minix/minlib.h>
#অন্তর্ভাৱ <minix/cpufeature.h>
#অন্তর্ভাৱ <machine/partition.h>
#অন্তর্ভাৱ "string.h"
#অন্তর্ভাৱ "direct_utils.h"
#অন্তর্ভাৱ "serial.h"
#অন্তর্ভাৱ "glo.h"

/* Give non-zero values to avoid them in BSS */
স্থির পূর্ণাঙ্ক print_সাৰি = 1, print_col = 1;

#অন্তর্ভাৱ <sys/video.h>

বাহ্য ৱর্ণ *video_mem;
#define VIDOFFSET(সাৰি, col) ((সাৰি) * MULTIBOOT_CONSOLE_COLS * 2 + (col) * 2)
#define VIDSIZE VIDOFFSET(MULTIBOOT_CONSOLE_সাৰিগুলি-1,MULTIBOOT_CONSOLE_COLS-1)

শূন্য direct_put_char(ৱর্ণ c, পূর্ণাঙ্ক সাৰি, পূর্ণাঙ্ক col) 
{
	পূর্ণাঙ্ক offset = VIDOFFSET(সাৰি, col);
	video_mem[offset] = c;
	video_mem[offset+1] = 0x07;	/* grey-on-black */
}

স্থির ৱর্ণ direct_get_char(পূর্ণাঙ্ক সাৰি, পূর্ণাঙ্ক col) 
{
	ঘূৰাই video_mem[VIDOFFSET(সাৰি, col)];
}

শূন্য direct_cls(শূন্য)
{
	/* Clear screen */
	পূর্ণাঙ্ক i,j;

	বাবে(i = 0; i < MULTIBOOT_CONSOLE_COLS; i++)
		বাবে(j = 0; j < MULTIBOOT_CONSOLE_সাৰিগুলি; j++)
			direct_put_char(' ', j, i);

	print_সাৰি = print_col = 0;

	/* Tell video hardware origin is 0. */
	outb(C_6845+INDEX, VID_ORG);
	outb(C_6845+DATA, 0);
	outb(C_6845+INDEX, VID_ORG+1);
	outb(C_6845+DATA, 0);
}

স্থির শূন্য direct_scroll_up(পূর্ণাঙ্ক সাৰিগুলি) 
{
	পূর্ণাঙ্ক i, j;
	বাবে (i = 0; i < MULTIBOOT_CONSOLE_সাৰিগুলি; i++ ) {
		বাবে (j = 0; j < MULTIBOOT_CONSOLE_COLS; j++ ) {
			ৱর্ণ c = 0;
			যদি(i < MULTIBOOT_CONSOLE_সাৰিগুলি-সাৰিগুলি)
				c = direct_get_char(i + সাৰিগুলি, j);
			direct_put_char(c, i, j);
		}
	}
	print_সাৰি-= সাৰিগুলি;
}

শূন্য direct_print_char(ৱর্ণ c)
{
	যেতিয়ালৈ (print_সাৰি >= MULTIBOOT_CONSOLE_সাৰিগুলি)
		direct_scroll_up(1);

#define TABWIDTH 8
	যদি(c == '\t') {
		যদি(print_col >= MULTIBOOT_CONSOLE_COLS - TABWIDTH) {
			c = '\n';
		} অন্যথা {
			কৰা {
				direct_put_char(' ', print_সাৰি, print_col++);
			} যেতিয়ালৈ(print_col % TABWIDTH);
			ঘূৰাই;
		}
	}

	যদি (c == '\n') {
		যেতিয়ালৈ (print_col < MULTIBOOT_CONSOLE_COLS)
			direct_put_char(' ', print_সাৰি, print_col++);
		print_সাৰি++;
		print_col = 0;
		ঘূৰাই;
	}

	direct_put_char(c, print_সাৰি, print_col++);

	যদি (print_col >= MULTIBOOT_CONSOLE_COLS) {
		print_সাৰি++;
		print_col = 0;
	}

	যেতিয়ালৈ (print_সাৰি >= MULTIBOOT_CONSOLE_সাৰিগুলি)
		direct_scroll_up(1);
}

শূন্য direct_print(ধ্রুৱ ৱর্ণ *str)
{
	যেতিয়ালৈ (*str) {
		direct_print_char(*str);
		str++;
	}
}

/* Standard and AT keyboard.  (PS/2 MCA implies AT throughout.) */
#define KEYBD		0x60	/* I/O port for keyboard data */
#define KB_STATUS	0x64	/* I/O port for status on AT */
#define KB_OUT_FULL	0x01	/* status bit set when keypress char pending */
#define KB_AUX_BYTE	0x20	/* Auxiliary Device Output Buffer Full */

পূর্ণাঙ্ক direct_read_char(unsigned ৱর্ণ *ch)
{
	unsigned long sb;

	sb = inb(KB_STATUS);

	যদি (!(sb & KB_OUT_FULL)) {
		ঘূৰাই 0;
	}

	inb(KEYBD);

	যদি (!(sb & KB_AUX_BYTE))
		ঘূৰাই 1;

	ঘূৰাই 0;
}

