
#অন্তর্ভাব <minix/minlib.h>
#অন্তর্ভাব <minix/cpufeature.h>
#অন্তর্ভাব <machine/partition.h>
#অন্তর্ভাব "string.h"
#অন্তর্ভাব "direct_utils.h"
#অন্তর্ভাব "serial.h"
#অন্তর্ভাব "glo.h"

/* Give non-zero values to avoid them in BSS */
স্থির পূর্ণসংখ্যা print_সারি = 1, print_col = 1;

#অন্তর্ভাব <sys/video.h>

বাহ্যিক অক্ষর *video_mem;
#define VIDOFFSET(সারি, col) ((সারি) * MULTIBOOT_CONSOLE_COLS * 2 + (col) * 2)
#define VIDSIZE VIDOFFSET(MULTIBOOT_CONSOLE_সারিগুলি-1,MULTIBOOT_CONSOLE_COLS-1)

শূন্য direct_put_char(অক্ষর c, পূর্ণসংখ্যা সারি, পূর্ণসংখ্যা col) 
{
	পূর্ণসংখ্যা offset = VIDOFFSET(সারি, col);
	video_mem[offset] = c;
	video_mem[offset+1] = 0x07;	/* grey-on-black */
}

স্থির অক্ষর direct_get_char(পূর্ণসংখ্যা সারি, পূর্ণসংখ্যা col) 
{
	ফেরত video_mem[VIDOFFSET(সারি, col)];
}

শূন্য direct_cls(শূন্য)
{
	/* Clear screen */
	পূর্ণসংখ্যা i,j;

	জন্য(i = 0; i < MULTIBOOT_CONSOLE_COLS; i++)
		জন্য(j = 0; j < MULTIBOOT_CONSOLE_সারিগুলি; j++)
			direct_put_char(' ', j, i);

	print_সারি = print_col = 0;

	/* Tell video hardware origin is 0. */
	outb(C_6845+সূচক_2, VID_ORG);
	outb(C_6845+তথ্য_2, 0);
	outb(C_6845+সূচক_2, VID_ORG+1);
	outb(C_6845+তথ্য_2, 0);
}

স্থির শূন্য direct_scroll_up(পূর্ণসংখ্যা সারিগুলি) 
{
	পূর্ণসংখ্যা i, j;
	জন্য (i = 0; i < MULTIBOOT_CONSOLE_সারিগুলি; i++ ) {
		জন্য (j = 0; j < MULTIBOOT_CONSOLE_COLS; j++ ) {
			অক্ষর c = 0;
			যদি(i < MULTIBOOT_CONSOLE_সারিগুলি-সারিগুলি)
				c = direct_get_char(i + সারিগুলি, j);
			direct_put_char(c, i, j);
		}
	}
	print_সারি-= সারিগুলি;
}

শূন্য direct_print_char(অক্ষর c)
{
	যখন (print_সারি >= MULTIBOOT_CONSOLE_সারিগুলি)
		direct_scroll_up(1);

#define TABWIDTH 8
	যদি(c == '\t') {
		যদি(print_col >= MULTIBOOT_CONSOLE_COLS - TABWIDTH) {
			c = '\n';
		} অন্যথা {
			করো {
				direct_put_char(' ', print_সারি, print_col++);
			} যখন(print_col % TABWIDTH);
			ফেরত;
		}
	}

	যদি (c == '\n') {
		যখন (print_col < MULTIBOOT_CONSOLE_COLS)
			direct_put_char(' ', print_সারি, print_col++);
		print_সারি++;
		print_col = 0;
		ফেরত;
	}

	direct_put_char(c, print_সারি, print_col++);

	যদি (print_col >= MULTIBOOT_CONSOLE_COLS) {
		print_সারি++;
		print_col = 0;
	}

	যখন (print_সারি >= MULTIBOOT_CONSOLE_সারিগুলি)
		direct_scroll_up(1);
}

শূন্য direct_print(ধ্রুবক অক্ষর *str)
{
	যখন (*str) {
		direct_print_char(*str);
		str++;
	}
}

/* Standard and AT keyboard.  (PS/2 MCA implies AT throughout.) */
#define KEYBD		0x60	/* I/O port for keyboard data */
#define KB_STATUS	0x64	/* I/O port for status on AT */
#define KB_OUT_FULL	0x01	/* status bit set when keypress char pending */
#define KB_AUX_BYTE	0x20	/* Auxiliary Device Output Buffer Full */

পূর্ণসংখ্যা direct_read_char(unsigned অক্ষর *ch)
{
	unsigned long sb;

	sb = inb(KB_STATUS);

	যদি (!(sb & KB_OUT_FULL)) {
		ফেরত 0;
	}

	inb(KEYBD);

	যদি (!(sb & KB_AUX_BYTE))
		ফেরত 1;

	ফেরত 0;
}

