
#અન્તર્ભાવ <minix/minlib.h>
#અન્તર્ભાવ <minix/cpufeature.h>
#અન્તર્ભાવ <machine/partition.h>
#અન્તર્ભાવ "string.h"
#અન્તર્ભાવ "direct_utils.h"
#અન્તર્ભાવ "serial.h"
#અન્તર્ભાવ "glo.h"

/* Give non-zero values to avoid them in BSS */
સ્થિર પૂર્ણાંક print_લીટી = 1, print_col = 1;

#અન્તર્ભાવ <sys/video.h>

બાહ્ય અક્ષર *video_mem;
#define VIDOFFSET(લીટી, col) ((લીટી) * MULTIBOOT_CONSOLE_COLS * 2 + (col) * 2)
#define VIDSIZE VIDOFFSET(MULTIBOOT_CONSOLE_લીટીઓ-1,MULTIBOOT_CONSOLE_COLS-1)

શૂન્ય direct_put_char(અક્ષર c, પૂર્ણાંક લીટી, પૂર્ણાંક col) 
{
	પૂર્ણાંક offset = VIDOFFSET(લીટી, col);
	video_mem[offset] = c;
	video_mem[offset+1] = 0x07;	/* grey-on-black */
}

સ્થિર અક્ષર direct_get_char(પૂર્ણાંક લીટી, પૂર્ણાંક col) 
{
	પાછા video_mem[VIDOFFSET(લીટી, col)];
}

શૂન્ય direct_cls(શૂન્ય)
{
	/* Clear screen */
	પૂર્ણાંક i,j;

	માટે(i = 0; i < MULTIBOOT_CONSOLE_COLS; i++)
		માટે(j = 0; j < MULTIBOOT_CONSOLE_લીટીઓ; j++)
			direct_put_char(' ', j, i);

	print_લીટી = print_col = 0;

	/* Tell video hardware origin is 0. */
	outb(C_6845+INDEX, VID_ORG);
	outb(C_6845+DATA, 0);
	outb(C_6845+INDEX, VID_ORG+1);
	outb(C_6845+DATA, 0);
}

સ્થિર શૂન્ય direct_scroll_up(પૂર્ણાંક લીટીઓ) 
{
	પૂર્ણાંક i, j;
	માટે (i = 0; i < MULTIBOOT_CONSOLE_લીટીઓ; i++ ) {
		માટે (j = 0; j < MULTIBOOT_CONSOLE_COLS; j++ ) {
			અક્ષર c = 0;
			જો(i < MULTIBOOT_CONSOLE_લીટીઓ-લીટીઓ)
				c = direct_get_char(i + લીટીઓ, j);
			direct_put_char(c, i, j);
		}
	}
	print_લીટી-= લીટીઓ;
}

શૂન્ય direct_print_char(અક્ષર c)
{
	જ્યારે (print_લીટી >= MULTIBOOT_CONSOLE_લીટીઓ)
		direct_scroll_up(1);

#define TABWIDTH 8
	જો(c == '\t') {
		જો(print_col >= MULTIBOOT_CONSOLE_COLS - TABWIDTH) {
			c = '\n';
		} નહીંતર {
			કરો {
				direct_put_char(' ', print_લીટી, print_col++);
			} જ્યારે(print_col % TABWIDTH);
			પાછા;
		}
	}

	જો (c == '\n') {
		જ્યારે (print_col < MULTIBOOT_CONSOLE_COLS)
			direct_put_char(' ', print_લીટી, print_col++);
		print_લીટી++;
		print_col = 0;
		પાછા;
	}

	direct_put_char(c, print_લીટી, print_col++);

	જો (print_col >= MULTIBOOT_CONSOLE_COLS) {
		print_લીટી++;
		print_col = 0;
	}

	જ્યારે (print_લીટી >= MULTIBOOT_CONSOLE_લીટીઓ)
		direct_scroll_up(1);
}

શૂન્ય direct_print(અચલ અક્ષર *str)
{
	જ્યારે (*str) {
		direct_print_char(*str);
		str++;
	}
}

/* Standard and AT keyboard.  (PS/2 MCA implies AT throughout.) */
#define KEYBD		0x60	/* I/O port for keyboard data */
#define KB_STATUS	0x64	/* I/O port for status on AT */
#define KB_OUT_FULL	0x01	/* status bit set when keypress char pending */
#define KB_AUX_BYTE	0x20	/* Auxiliary Device Output Buffer Full */

પૂર્ણાંક direct_read_char(unsigned અક્ષર *ch)
{
	unsigned long sb;

	sb = inb(KB_STATUS);

	જો (!(sb & KB_OUT_FULL)) {
		પાછા 0;
	}

	inb(KEYBD);

	જો (!(sb & KB_AUX_BYTE))
		પાછા 1;

	પાછા 0;
}

