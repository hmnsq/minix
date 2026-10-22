
#अन्तर्भाव <minix/minlib.h>
#अन्तर्भाव <minix/cpufeature.h>
#अन्तर्भाव <machine/partition.h>
#अन्तर्भाव "string.h"
#अन्तर्भाव "direct_utils.h"
#अन्तर्भाव "serial.h"
#अन्तर्भाव "glo.h"

/* Give non-zero values to avoid them in BSS */
स्थिर पूर्णाङ्क print_पङ्क्तिः = 1, print_col = 1;

#अन्तर्भाव <sys/video.h>

बाह्य वर्ण *video_mem;
#define VIDOFFSET(पङ्क्तिः, col) ((पङ्क्तिः) * MULTIBOOT_CONSOLE_COLS * 2 + (col) * 2)
#define VIDSIZE VIDOFFSET(MULTIBOOT_CONSOLE_पङ्क्तयः-1,MULTIBOOT_CONSOLE_COLS-1)

शून्य direct_put_char(वर्ण c, पूर्णाङ्क पङ्क्तिः, पूर्णाङ्क col) 
{
	पूर्णाङ्क offset = VIDOFFSET(पङ्क्तिः, col);
	video_mem[offset] = c;
	video_mem[offset+1] = 0x07;	/* grey-on-black */
}

स्थिर वर्ण direct_get_char(पूर्णाङ्क पङ्क्तिः, पूर्णाङ्क col) 
{
	पटिनिवत्त video_mem[VIDOFFSET(पङ्क्तिः, col)];
}

शून्य direct_cls(शून्य)
{
	/* Clear screen */
	पूर्णाङ्क i,j;

	अत्थाय(i = 0; i < MULTIBOOT_CONSOLE_COLS; i++)
		अत्थाय(j = 0; j < MULTIBOOT_CONSOLE_पङ्क्तयः; j++)
			direct_put_char(' ', j, i);

	print_पङ्क्तिः = print_col = 0;

	/* Tell video hardware origin is 0. */
	outb(C_6845+INDEX, VID_ORG);
	outb(C_6845+DATA, 0);
	outb(C_6845+INDEX, VID_ORG+1);
	outb(C_6845+DATA, 0);
}

स्थिर शून्य direct_scroll_up(पूर्णाङ्क पङ्क्तयः) 
{
	पूर्णाङ्क i, j;
	अत्थाय (i = 0; i < MULTIBOOT_CONSOLE_पङ्क्तयः; i++ ) {
		अत्थाय (j = 0; j < MULTIBOOT_CONSOLE_COLS; j++ ) {
			वर्ण c = 0;
			सचे(i < MULTIBOOT_CONSOLE_पङ्क्तयः-पङ्क्तयः)
				c = direct_get_char(i + पङ्क्तयः, j);
			direct_put_char(c, i, j);
		}
	}
	print_पङ्क्तिः-= पङ्क्तयः;
}

शून्य direct_print_char(वर्ण c)
{
	याव (print_पङ्क्तिः >= MULTIBOOT_CONSOLE_पङ्क्तयः)
		direct_scroll_up(1);

#define TABWIDTH 8
	सचे(c == '\t') {
		सचे(print_col >= MULTIBOOT_CONSOLE_COLS - TABWIDTH) {
			c = '\n';
		} अञ्ञथा {
			करोहि {
				direct_put_char(' ', print_पङ्क्तिः, print_col++);
			} याव(print_col % TABWIDTH);
			पटिनिवत्त;
		}
	}

	सचे (c == '\n') {
		याव (print_col < MULTIBOOT_CONSOLE_COLS)
			direct_put_char(' ', print_पङ्क्तिः, print_col++);
		print_पङ्क्तिः++;
		print_col = 0;
		पटिनिवत्त;
	}

	direct_put_char(c, print_पङ्क्तिः, print_col++);

	सचे (print_col >= MULTIBOOT_CONSOLE_COLS) {
		print_पङ्क्तिः++;
		print_col = 0;
	}

	याव (print_पङ्क्तिः >= MULTIBOOT_CONSOLE_पङ्क्तयः)
		direct_scroll_up(1);
}

शून्य direct_print(ध्रुव वर्ण *str)
{
	याव (*str) {
		direct_print_char(*str);
		str++;
	}
}

/* Standard and AT keyboard.  (PS/2 MCA implies AT throughout.) */
#define KEYBD		0x60	/* I/O port for keyboard data */
#define KB_STATUS	0x64	/* I/O port for status on AT */
#define KB_OUT_FULL	0x01	/* status bit set when keypress char pending */
#define KB_AUX_BYTE	0x20	/* Auxiliary Device Output Buffer Full */

पूर्णाङ्क direct_read_char(unsigned वर्ण *ch)
{
	unsigned long sb;

	sb = inb(KB_STATUS);

	सचे (!(sb & KB_OUT_FULL)) {
		पटिनिवत्त 0;
	}

	inb(KEYBD);

	सचे (!(sb & KB_AUX_BYTE))
		पटिनिवत्त 1;

	पटिनिवत्त 0;
}

