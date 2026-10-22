
#अन्तर्भाव <minix/minlib.h>
#अन्तर्भाव <minix/cpufeature.h>
#अन्तर्भाव <machine/partition.h>
#अन्तर्भाव "string.h"
#अन्तर्भाव "direct_utils.h"
#अन्तर्भाव "serial.h"
#अन्तर्भाव "glo.h"

/* Give non-zero values to avoid them in BSS */
स्थिर पूर्णाङ्क print_पंक्ति = 1, print_col = 1;

#अन्तर्भाव <sys/video.h>

बाह्य वर्ण *video_mem;
#define VIDOFFSET(पंक्ति, col) ((पंक्ति) * MULTIBOOT_CONSOLE_COLS * 2 + (col) * 2)
#define VIDSIZE VIDOFFSET(MULTIBOOT_CONSOLE_पंक्तियाँ-1,MULTIBOOT_CONSOLE_COLS-1)

शून्य direct_put_char(वर्ण c, पूर्णाङ्क पंक्ति, पूर्णाङ्क col) 
{
	पूर्णाङ्क offset = VIDOFFSET(पंक्ति, col);
	video_mem[offset] = c;
	video_mem[offset+1] = 0x07;	/* grey-on-black */
}

स्थिर वर्ण direct_get_char(पूर्णाङ्क पंक्ति, पूर्णाङ्क col) 
{
	प्रत्यागम video_mem[VIDOFFSET(पंक्ति, col)];
}

शून्य direct_cls(शून्य)
{
	/* Clear screen */
	पूर्णाङ्क i,j;

	प्रति(i = 0; i < MULTIBOOT_CONSOLE_COLS; i++)
		प्रति(j = 0; j < MULTIBOOT_CONSOLE_पंक्तियाँ; j++)
			direct_put_char(' ', j, i);

	print_पंक्ति = print_col = 0;

	/* Tell video hardware origin is 0. */
	outb(C_6845+INDEX, VID_ORG);
	outb(C_6845+DATA, 0);
	outb(C_6845+INDEX, VID_ORG+1);
	outb(C_6845+DATA, 0);
}

स्थिर शून्य direct_scroll_up(पूर्णाङ्क पंक्तियाँ) 
{
	पूर्णाङ्क i, j;
	प्रति (i = 0; i < MULTIBOOT_CONSOLE_पंक्तियाँ; i++ ) {
		प्रति (j = 0; j < MULTIBOOT_CONSOLE_COLS; j++ ) {
			वर्ण c = 0;
			यदि(i < MULTIBOOT_CONSOLE_पंक्तियाँ-पंक्तियाँ)
				c = direct_get_char(i + पंक्तियाँ, j);
			direct_put_char(c, i, j);
		}
	}
	print_पंक्ति-= पंक्तियाँ;
}

शून्य direct_print_char(वर्ण c)
{
	यावत् (print_पंक्ति >= MULTIBOOT_CONSOLE_पंक्तियाँ)
		direct_scroll_up(1);

#define TABWIDTH 8
	यदि(c == '\t') {
		यदि(print_col >= MULTIBOOT_CONSOLE_COLS - TABWIDTH) {
			c = '\n';
		} अन्यथा {
			कुरु {
				direct_put_char(' ', print_पंक्ति, print_col++);
			} यावत्(print_col % TABWIDTH);
			प्रत्यागम;
		}
	}

	यदि (c == '\n') {
		यावत् (print_col < MULTIBOOT_CONSOLE_COLS)
			direct_put_char(' ', print_पंक्ति, print_col++);
		print_पंक्ति++;
		print_col = 0;
		प्रत्यागम;
	}

	direct_put_char(c, print_पंक्ति, print_col++);

	यदि (print_col >= MULTIBOOT_CONSOLE_COLS) {
		print_पंक्ति++;
		print_col = 0;
	}

	यावत् (print_पंक्ति >= MULTIBOOT_CONSOLE_पंक्तियाँ)
		direct_scroll_up(1);
}

शून्य direct_print(ध्रुव वर्ण *str)
{
	यावत् (*str) {
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

	यदि (!(sb & KB_OUT_FULL)) {
		प्रत्यागम 0;
	}

	inb(KEYBD);

	यदि (!(sb & KB_AUX_BYTE))
		प्रत्यागम 1;

	प्रत्यागम 0;
}

