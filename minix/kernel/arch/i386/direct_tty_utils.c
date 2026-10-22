
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
	प्रत्यागम video_mem[VIDOFFSET(पङ्क्तिः, col)];
}

शून्य direct_cls(शून्य)
{
	/* Clear screen */
	पूर्णाङ्क i,j;

	प्रति(i = 0; i < MULTIBOOT_CONSOLE_COLS; i++)
		प्रति(j = 0; j < MULTIBOOT_CONSOLE_पङ्क्तयः; j++)
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
	प्रति (i = 0; i < MULTIBOOT_CONSOLE_पङ्क्तयः; i++ ) {
		प्रति (j = 0; j < MULTIBOOT_CONSOLE_COLS; j++ ) {
			वर्ण c = 0;
			जइ(i < MULTIBOOT_CONSOLE_पङ्क्तयः-पङ्क्तयः)
				c = direct_get_char(i + पङ्क्तयः, j);
			direct_put_char(c, i, j);
		}
	}
	print_पङ्क्तिः-= पङ्क्तयः;
}

शून्य direct_print_char(वर्ण c)
{
	जाव (print_पङ्क्तिः >= MULTIBOOT_CONSOLE_पङ्क्तयः)
		direct_scroll_up(1);

#define TABWIDTH 8
	जइ(c == '\t') {
		जइ(print_col >= MULTIBOOT_CONSOLE_COLS - TABWIDTH) {
			c = '\n';
		} अण्णहा {
			करेहि {
				direct_put_char(' ', print_पङ्क्तिः, print_col++);
			} जाव(print_col % TABWIDTH);
			प्रत्यागम;
		}
	}

	जइ (c == '\n') {
		जाव (print_col < MULTIBOOT_CONSOLE_COLS)
			direct_put_char(' ', print_पङ्क्तिः, print_col++);
		print_पङ्क्तिः++;
		print_col = 0;
		प्रत्यागम;
	}

	direct_put_char(c, print_पङ्क्तिः, print_col++);

	जइ (print_col >= MULTIBOOT_CONSOLE_COLS) {
		print_पङ्क्तिः++;
		print_col = 0;
	}

	जाव (print_पङ्क्तिः >= MULTIBOOT_CONSOLE_पङ्क्तयः)
		direct_scroll_up(1);
}

शून्य direct_print(ध्रुव वर्ण *str)
{
	जाव (*str) {
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

	जइ (!(sb & KB_OUT_FULL)) {
		प्रत्यागम 0;
	}

	inb(KEYBD);

	जइ (!(sb & KB_AUX_BYTE))
		प्रत्यागम 1;

	प्रत्यागम 0;
}

