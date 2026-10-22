
#ଅନ୍ତର୍ଭାଵ <minix/minlib.h>
#ଅନ୍ତର୍ଭାଵ <minix/cpufeature.h>
#ଅନ୍ତର୍ଭାଵ <machine/partition.h>
#ଅନ୍ତର୍ଭାଵ "string.h"
#ଅନ୍ତର୍ଭାଵ "direct_utils.h"
#ଅନ୍ତର୍ଭାଵ "serial.h"
#ଅନ୍ତର୍ଭାଵ "glo.h"

/* Give non-zero values to avoid them in BSS */
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା print_ଧାଡ଼ି = 1, print_col = 1;

#ଅନ୍ତର୍ଭାଵ <sys/video.h>

ବାହ୍ୟ ଅକ୍ଷର *video_mem;
#define VIDOFFSET(ଧାଡ଼ି, col) ((ଧାଡ଼ି) * MULTIBOOT_CONSOLE_COLS * 2 + (col) * 2)
#define VIDSIZE VIDOFFSET(MULTIBOOT_CONSOLE_ଧାଡ଼ିଗୁଡ଼ିକ-1,MULTIBOOT_CONSOLE_COLS-1)

ଶୂନ୍ୟ direct_put_char(ଅକ୍ଷର c, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଧାଡ଼ି, ପୂର୍ଣ୍ଣସଂଖ୍ୟା col) 
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା offset = VIDOFFSET(ଧାଡ଼ି, col);
	video_mem[offset] = c;
	video_mem[offset+1] = 0x07;	/* grey-on-black */
}

ସ୍ଥିର ଅକ୍ଷର direct_get_char(ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଧାଡ଼ି, ପୂର୍ଣ୍ଣସଂଖ୍ୟା col) 
{
	ଫେରନ୍ତୁ video_mem[VIDOFFSET(ଧାଡ଼ି, col)];
}

ଶୂନ୍ୟ direct_cls(ଶୂନ୍ୟ)
{
	/* Clear screen */
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା i,j;

	ପାଇଁ(i = 0; i < MULTIBOOT_CONSOLE_COLS; i++)
		ପାଇଁ(j = 0; j < MULTIBOOT_CONSOLE_ଧାଡ଼ିଗୁଡ଼ିକ; j++)
			direct_put_char(' ', j, i);

	print_ଧାଡ଼ି = print_col = 0;

	/* Tell video hardware origin is 0. */
	outb(C_6845+INDEX, VID_ORG);
	outb(C_6845+DATA, 0);
	outb(C_6845+INDEX, VID_ORG+1);
	outb(C_6845+DATA, 0);
}

ସ୍ଥିର ଶୂନ୍ୟ direct_scroll_up(ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଧାଡ଼ିଗୁଡ଼ିକ) 
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା i, j;
	ପାଇଁ (i = 0; i < MULTIBOOT_CONSOLE_ଧାଡ଼ିଗୁଡ଼ିକ; i++ ) {
		ପାଇଁ (j = 0; j < MULTIBOOT_CONSOLE_COLS; j++ ) {
			ଅକ୍ଷର c = 0;
			ଯଦି(i < MULTIBOOT_CONSOLE_ଧାଡ଼ିଗୁଡ଼ିକ-ଧାଡ଼ିଗୁଡ଼ିକ)
				c = direct_get_char(i + ଧାଡ଼ିଗୁଡ଼ିକ, j);
			direct_put_char(c, i, j);
		}
	}
	print_ଧାଡ଼ି-= ଧାଡ଼ିଗୁଡ଼ିକ;
}

ଶୂନ୍ୟ direct_print_char(ଅକ୍ଷର c)
{
	ଯେତେବେଳେ (print_ଧାଡ଼ି >= MULTIBOOT_CONSOLE_ଧାଡ଼ିଗୁଡ଼ିକ)
		direct_scroll_up(1);

#define TABWIDTH 8
	ଯଦି(c == '\t') {
		ଯଦି(print_col >= MULTIBOOT_CONSOLE_COLS - TABWIDTH) {
			c = '\n';
		} ଅନ୍ୟଥା {
			କର {
				direct_put_char(' ', print_ଧାଡ଼ି, print_col++);
			} ଯେତେବେଳେ(print_col % TABWIDTH);
			ଫେରନ୍ତୁ;
		}
	}

	ଯଦି (c == '\n') {
		ଯେତେବେଳେ (print_col < MULTIBOOT_CONSOLE_COLS)
			direct_put_char(' ', print_ଧାଡ଼ି, print_col++);
		print_ଧାଡ଼ି++;
		print_col = 0;
		ଫେରନ୍ତୁ;
	}

	direct_put_char(c, print_ଧାଡ଼ି, print_col++);

	ଯଦି (print_col >= MULTIBOOT_CONSOLE_COLS) {
		print_ଧାଡ଼ି++;
		print_col = 0;
	}

	ଯେତେବେଳେ (print_ଧାଡ଼ି >= MULTIBOOT_CONSOLE_ଧାଡ଼ିଗୁଡ଼ିକ)
		direct_scroll_up(1);
}

ଶୂନ୍ୟ direct_print(ଅଚଳ ଅକ୍ଷର *str)
{
	ଯେତେବେଳେ (*str) {
		direct_print_char(*str);
		str++;
	}
}

/* Standard and AT keyboard.  (PS/2 MCA implies AT throughout.) */
#define KEYBD		0x60	/* I/O port for keyboard data */
#define KB_STATUS	0x64	/* I/O port for status on AT */
#define KB_OUT_FULL	0x01	/* status bit set when keypress char pending */
#define KB_AUX_BYTE	0x20	/* Auxiliary Device Output Buffer Full */

ପୂର୍ଣ୍ଣସଂଖ୍ୟା direct_read_char(unsigned ଅକ୍ଷର *ch)
{
	unsigned long sb;

	sb = inb(KB_STATUS);

	ଯଦି (!(sb & KB_OUT_FULL)) {
		ଫେରନ୍ତୁ 0;
	}

	inb(KEYBD);

	ଯଦି (!(sb & KB_AUX_BYTE))
		ଫେରନ୍ତୁ 1;

	ଫେରନ୍ତୁ 0;
}

