
#شامل <minix/minlib.h>
#شامل <minix/cpufeature.h>
#شامل <machine/partition.h>
#شامل "string.h"
#شامل "direct_utils.h"
#شامل "serial.h"
#شامل "glo.h"

/* Give non-zero values to avoid them in BSS */
ایستا صحیح print_سطر = 1, print_col = 1;

#شامل <sys/video.h>

بیرونی نویسه *video_mem;
#define VIDOFFSET(سطر, col) ((سطر) * MULTIBOOT_CONSOLE_COLS * 2 + (col) * 2)
#define VIDSIZE VIDOFFSET(MULTIBOOT_CONSOLE_سطرها-1,MULTIBOOT_CONSOLE_COLS-1)

تهی direct_put_char(نویسه c, صحیح سطر, صحیح col) 
{
	صحیح offset = VIDOFFSET(سطر, col);
	video_mem[offset] = c;
	video_mem[offset+1] = 0x07;	/* grey-on-black */
}

ایستا نویسه direct_get_char(صحیح سطر, صحیح col) 
{
	بازگشت video_mem[VIDOFFSET(سطر, col)];
}

تهی direct_cls(تهی)
{
	/* Clear screen */
	صحیح i,j;

	برای(i = 0; i < MULTIBOOT_CONSOLE_COLS; i++)
		برای(j = 0; j < MULTIBOOT_CONSOLE_سطرها; j++)
			direct_put_char(' ', j, i);

	print_سطر = print_col = 0;

	/* Tell video hardware origin is 0. */
	outb(C_6845+INDEX, VID_ORG);
	outb(C_6845+DATA, 0);
	outb(C_6845+INDEX, VID_ORG+1);
	outb(C_6845+DATA, 0);
}

ایستا تهی direct_scroll_up(صحیح سطرها) 
{
	صحیح i, j;
	برای (i = 0; i < MULTIBOOT_CONSOLE_سطرها; i++ ) {
		برای (j = 0; j < MULTIBOOT_CONSOLE_COLS; j++ ) {
			نویسه c = 0;
			اگر(i < MULTIBOOT_CONSOLE_سطرها-سطرها)
				c = direct_get_char(i + سطرها, j);
			direct_put_char(c, i, j);
		}
	}
	print_سطر-= سطرها;
}

تهی direct_print_char(نویسه c)
{
	وقتی (print_سطر >= MULTIBOOT_CONSOLE_سطرها)
		direct_scroll_up(1);

#define TABWIDTH 8
	اگر(c == '\t') {
		اگر(print_col >= MULTIBOOT_CONSOLE_COLS - TABWIDTH) {
			c = '\n';
		} وگرنه {
			انجامبده {
				direct_put_char(' ', print_سطر, print_col++);
			} وقتی(print_col % TABWIDTH);
			بازگشت;
		}
	}

	اگر (c == '\n') {
		وقتی (print_col < MULTIBOOT_CONSOLE_COLS)
			direct_put_char(' ', print_سطر, print_col++);
		print_سطر++;
		print_col = 0;
		بازگشت;
	}

	direct_put_char(c, print_سطر, print_col++);

	اگر (print_col >= MULTIBOOT_CONSOLE_COLS) {
		print_سطر++;
		print_col = 0;
	}

	وقتی (print_سطر >= MULTIBOOT_CONSOLE_سطرها)
		direct_scroll_up(1);
}

تهی direct_print(ثابت نویسه *str)
{
	وقتی (*str) {
		direct_print_char(*str);
		str++;
	}
}

/* Standard and AT keyboard.  (PS/2 MCA implies AT throughout.) */
#define KEYBD		0x60	/* I/O port for keyboard data */
#define KB_STATUS	0x64	/* I/O port for status on AT */
#define KB_OUT_FULL	0x01	/* status bit set when keypress char pending */
#define KB_AUX_BYTE	0x20	/* Auxiliary Device Output Buffer Full */

صحیح direct_read_char(unsigned نویسه *ch)
{
	unsigned long sb;

	sb = inb(KB_STATUS);

	اگر (!(sb & KB_OUT_FULL)) {
		بازگشت 0;
	}

	inb(KEYBD);

	اگر (!(sb & KB_AUX_BYTE))
		بازگشت 1;

	بازگشت 0;
}

