
#அந்தர்பாவ <minix/minlib.h>
#அந்தர்பாவ <minix/cpufeature.h>
#அந்தர்பாவ <machine/partition.h>
#அந்தர்பாவ "string.h"
#அந்தர்பாவ "direct_utils.h"
#அந்தர்பாவ "serial.h"
#அந்தர்பாவ "glo.h"

/* Give non-zero values to avoid them in BSS */
நிலையான முழு print_வரி = 1, print_col = 1;

#அந்தர்பாவ <sys/video.h>

வெளி எழுத்து *video_mem;
#define VIDOFFSET(வரி, col) ((வரி) * MULTIBOOT_CONSOLE_COLS * 2 + (col) * 2)
#define VIDSIZE VIDOFFSET(MULTIBOOT_CONSOLE_வரிகள்-1,MULTIBOOT_CONSOLE_COLS-1)

வெறுமை direct_put_char(எழுத்து c, முழு வரி, முழு col) 
{
	முழு offset = VIDOFFSET(வரி, col);
	video_mem[offset] = c;
	video_mem[offset+1] = 0x07;	/* grey-on-black */
}

நிலையான எழுத்து direct_get_char(முழு வரி, முழு col) 
{
	திரும்பு video_mem[VIDOFFSET(வரி, col)];
}

வெறுமை direct_cls(வெறுமை)
{
	/* Clear screen */
	முழு i,j;

	க்கு(i = 0; i < MULTIBOOT_CONSOLE_COLS; i++)
		க்கு(j = 0; j < MULTIBOOT_CONSOLE_வரிகள்; j++)
			direct_put_char(' ', j, i);

	print_வரி = print_col = 0;

	/* Tell video hardware origin is 0. */
	outb(C_6845+INDEX, VID_ORG);
	outb(C_6845+DATA, 0);
	outb(C_6845+INDEX, VID_ORG+1);
	outb(C_6845+DATA, 0);
}

நிலையான வெறுமை direct_scroll_up(முழு வரிகள்) 
{
	முழு i, j;
	க்கு (i = 0; i < MULTIBOOT_CONSOLE_வரிகள்; i++ ) {
		க்கு (j = 0; j < MULTIBOOT_CONSOLE_COLS; j++ ) {
			எழுத்து c = 0;
			எனில்(i < MULTIBOOT_CONSOLE_வரிகள்-வரிகள்)
				c = direct_get_char(i + வரிகள், j);
			direct_put_char(c, i, j);
		}
	}
	print_வரி-= வரிகள்;
}

வெறுமை direct_print_char(எழுத்து c)
{
	போது (print_வரி >= MULTIBOOT_CONSOLE_வரிகள்)
		direct_scroll_up(1);

#define TABWIDTH 8
	எனில்(c == '\t') {
		எனில்(print_col >= MULTIBOOT_CONSOLE_COLS - TABWIDTH) {
			c = '\n';
		} இல்லையெனில் {
			செய் {
				direct_put_char(' ', print_வரி, print_col++);
			} போது(print_col % TABWIDTH);
			திரும்பு;
		}
	}

	எனில் (c == '\n') {
		போது (print_col < MULTIBOOT_CONSOLE_COLS)
			direct_put_char(' ', print_வரி, print_col++);
		print_வரி++;
		print_col = 0;
		திரும்பு;
	}

	direct_put_char(c, print_வரி, print_col++);

	எனில் (print_col >= MULTIBOOT_CONSOLE_COLS) {
		print_வரி++;
		print_col = 0;
	}

	போது (print_வரி >= MULTIBOOT_CONSOLE_வரிகள்)
		direct_scroll_up(1);
}

வெறுமை direct_print(மாறிலி எழுத்து *str)
{
	போது (*str) {
		direct_print_char(*str);
		str++;
	}
}

/* Standard and AT keyboard.  (PS/2 MCA implies AT throughout.) */
#define KEYBD		0x60	/* I/O port for keyboard data */
#define KB_STATUS	0x64	/* I/O port for status on AT */
#define KB_OUT_FULL	0x01	/* status bit set when keypress char pending */
#define KB_AUX_BYTE	0x20	/* Auxiliary Device Output Buffer Full */

முழு direct_read_char(unsigned எழுத்து *ch)
{
	unsigned long sb;

	sb = inb(KB_STATUS);

	எனில் (!(sb & KB_OUT_FULL)) {
		திரும்பு 0;
	}

	inb(KEYBD);

	எனில் (!(sb & KB_AUX_BYTE))
		திரும்பு 1;

	திரும்பு 0;
}

