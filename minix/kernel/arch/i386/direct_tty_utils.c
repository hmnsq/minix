
#അന്തര്ഭാവ <minix/minlib.h>
#അന്തര്ഭാവ <minix/cpufeature.h>
#അന്തര്ഭാവ <machine/partition.h>
#അന്തര്ഭാവ "string.h"
#അന്തര്ഭാവ "direct_utils.h"
#അന്തര്ഭാവ "serial.h"
#അന്തര്ഭാവ "glo.h"

/* Give non-zero values to avoid them in BSS */
സ്ഥിതം പൂർണ്ണസംഖ്യ print_വരി = 1, print_col = 1;

#അന്തര്ഭാവ <sys/video.h>

ബാഹ്യ അക്ഷരം *video_mem;
#define VIDOFFSET(വരി, col) ((വരി) * MULTIBOOT_CONSOLE_COLS * 2 + (col) * 2)
#define VIDSIZE VIDOFFSET(MULTIBOOT_CONSOLE_വരികൾ-1,MULTIBOOT_CONSOLE_COLS-1)

ശൂന്യം direct_put_char(അക്ഷരം c, പൂർണ്ണസംഖ്യ വരി, പൂർണ്ണസംഖ്യ col) 
{
	പൂർണ്ണസംഖ്യ offset = VIDOFFSET(വരി, col);
	video_mem[offset] = c;
	video_mem[offset+1] = 0x07;	/* grey-on-black */
}

സ്ഥിതം അക്ഷരം direct_get_char(പൂർണ്ണസംഖ്യ വരി, പൂർണ്ണസംഖ്യ col) 
{
	തിരികെ video_mem[VIDOFFSET(വരി, col)];
}

ശൂന്യം direct_cls(ശൂന്യം)
{
	/* Clear screen */
	പൂർണ്ണസംഖ്യ i,j;

	വേണ്ടി(i = 0; i < MULTIBOOT_CONSOLE_COLS; i++)
		വേണ്ടി(j = 0; j < MULTIBOOT_CONSOLE_വരികൾ; j++)
			direct_put_char(' ', j, i);

	print_വരി = print_col = 0;

	/* Tell video hardware origin is 0. */
	outb(C_6845+INDEX, VID_ORG);
	outb(C_6845+DATA, 0);
	outb(C_6845+INDEX, VID_ORG+1);
	outb(C_6845+DATA, 0);
}

സ്ഥിതം ശൂന്യം direct_scroll_up(പൂർണ്ണസംഖ്യ വരികൾ) 
{
	പൂർണ്ണസംഖ്യ i, j;
	വേണ്ടി (i = 0; i < MULTIBOOT_CONSOLE_വരികൾ; i++ ) {
		വേണ്ടി (j = 0; j < MULTIBOOT_CONSOLE_COLS; j++ ) {
			അക്ഷരം c = 0;
			എങ്കിൽ(i < MULTIBOOT_CONSOLE_വരികൾ-വരികൾ)
				c = direct_get_char(i + വരികൾ, j);
			direct_put_char(c, i, j);
		}
	}
	print_വരി-= വരികൾ;
}

ശൂന്യം direct_print_char(അക്ഷരം c)
{
	ആകുമ്പോൾ (print_വരി >= MULTIBOOT_CONSOLE_വരികൾ)
		direct_scroll_up(1);

#define TABWIDTH 8
	എങ്കിൽ(c == '\t') {
		എങ്കിൽ(print_col >= MULTIBOOT_CONSOLE_COLS - TABWIDTH) {
			c = '\n';
		} അല്ലെങ്കിൽ {
			ചെയ്യുക {
				direct_put_char(' ', print_വരി, print_col++);
			} ആകുമ്പോൾ(print_col % TABWIDTH);
			തിരികെ;
		}
	}

	എങ്കിൽ (c == '\n') {
		ആകുമ്പോൾ (print_col < MULTIBOOT_CONSOLE_COLS)
			direct_put_char(' ', print_വരി, print_col++);
		print_വരി++;
		print_col = 0;
		തിരികെ;
	}

	direct_put_char(c, print_വരി, print_col++);

	എങ്കിൽ (print_col >= MULTIBOOT_CONSOLE_COLS) {
		print_വരി++;
		print_col = 0;
	}

	ആകുമ്പോൾ (print_വരി >= MULTIBOOT_CONSOLE_വരികൾ)
		direct_scroll_up(1);
}

ശൂന്യം direct_print(സ്ഥിരം അക്ഷരം *str)
{
	ആകുമ്പോൾ (*str) {
		direct_print_char(*str);
		str++;
	}
}

/* Standard and AT keyboard.  (PS/2 MCA implies AT throughout.) */
#define KEYBD		0x60	/* I/O port for keyboard data */
#define KB_STATUS	0x64	/* I/O port for status on AT */
#define KB_OUT_FULL	0x01	/* status bit set when keypress char pending */
#define KB_AUX_BYTE	0x20	/* Auxiliary Device Output Buffer Full */

പൂർണ്ണസംഖ്യ direct_read_char(unsigned അക്ഷരം *ch)
{
	unsigned long sb;

	sb = inb(KB_STATUS);

	എങ്കിൽ (!(sb & KB_OUT_FULL)) {
		തിരികെ 0;
	}

	inb(KEYBD);

	എങ്കിൽ (!(sb & KB_AUX_BYTE))
		തിരികെ 1;

	തിരികെ 0;
}

