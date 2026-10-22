
#अन्तर्भाव <minix/minlib.h>
#अन्तर्भाव <minix/cpufeature.h>
#अन्तर्भाव <machine/partition.h>
#अन्तर्भाव "string.h"
#अन्तर्भाव "direct_utils.h"
#अन्तर्भाव "serial.h"
#अन्तर्भाव "glo.h"

/* Give non-zero values to avoid them in BSS */
स्थिर पूर्णाङ्क print_पङ्क्ति = 1, print_col = 1;

#अन्तर्भाव <sys/video.h>

बाह्य वर्ण *video_mem;
#define VIDOFFSET(पङ्क्ति, col) ((पङ्क्ति) * MULTIBOOT_CONSOLE_COLS * 2 + (col) * 2)
#define VIDSIZE VIDOFFSET(MULTIBOOT_CONSOLE_पङ्क्तिहरू-1,MULTIBOOT_CONSOLE_COLS-1)

शून्य direct_put_char(वर्ण c, पूर्णाङ्क पङ्क्ति, पूर्णाङ्क col) 
{
	पूर्णाङ्क offset = VIDOFFSET(पङ्क्ति, col);
	video_mem[offset] = c;
	video_mem[offset+1] = 0x07;	/* grey-on-black */
}

स्थिर वर्ण direct_get_char(पूर्णाङ्क पङ्क्ति, पूर्णाङ्क col) 
{
	फर्काऊ video_mem[VIDOFFSET(पङ्क्ति, col)];
}

शून्य direct_cls(शून्य)
{
	/* Clear screen */
	पूर्णाङ्क i,j;

	लागि(i = 0; i < MULTIBOOT_CONSOLE_COLS; i++)
		लागि(j = 0; j < MULTIBOOT_CONSOLE_पङ्क्तिहरू; j++)
			direct_put_char(' ', j, i);

	print_पङ्क्ति = print_col = 0;

	/* Tell video hardware origin is 0. */
	outb(C_6845+INDEX, VID_ORG);
	outb(C_6845+DATA, 0);
	outb(C_6845+INDEX, VID_ORG+1);
	outb(C_6845+DATA, 0);
}

स्थिर शून्य direct_scroll_up(पूर्णाङ्क पङ्क्तिहरू) 
{
	पूर्णाङ्क i, j;
	लागि (i = 0; i < MULTIBOOT_CONSOLE_पङ्क्तिहरू; i++ ) {
		लागि (j = 0; j < MULTIBOOT_CONSOLE_COLS; j++ ) {
			वर्ण c = 0;
			यदि(i < MULTIBOOT_CONSOLE_पङ्क्तिहरू-पङ्क्तिहरू)
				c = direct_get_char(i + पङ्क्तिहरू, j);
			direct_put_char(c, i, j);
		}
	}
	print_पङ्क्ति-= पङ्क्तिहरू;
}

शून्य direct_print_char(वर्ण c)
{
	जबसम्म (print_पङ्क्ति >= MULTIBOOT_CONSOLE_पङ्क्तिहरू)
		direct_scroll_up(1);

#define TABWIDTH 8
	यदि(c == '\t') {
		यदि(print_col >= MULTIBOOT_CONSOLE_COLS - TABWIDTH) {
			c = '\n';
		} अन्यथा {
			गर {
				direct_put_char(' ', print_पङ्क्ति, print_col++);
			} जबसम्म(print_col % TABWIDTH);
			फर्काऊ;
		}
	}

	यदि (c == '\n') {
		जबसम्म (print_col < MULTIBOOT_CONSOLE_COLS)
			direct_put_char(' ', print_पङ्क्ति, print_col++);
		print_पङ्क्ति++;
		print_col = 0;
		फर्काऊ;
	}

	direct_put_char(c, print_पङ्क्ति, print_col++);

	यदि (print_col >= MULTIBOOT_CONSOLE_COLS) {
		print_पङ्क्ति++;
		print_col = 0;
	}

	जबसम्म (print_पङ्क्ति >= MULTIBOOT_CONSOLE_पङ्क्तिहरू)
		direct_scroll_up(1);
}

शून्य direct_print(अचर वर्ण *str)
{
	जबसम्म (*str) {
		direct_print_char(*str);
		str++;
	}
}

/* Standard and AT keyboard.  (PS/2 MCA implies AT throughout.) */
#define KEYBD		0x60	/* I/O port for keyboard data */
#define KB_STATUS	0x64	/* I/O port for status on AT */
#define KB_OUT_FULL	0x01	/* status bit set when keypress char pending */
#define KB_AUX_BYTE	0x20	/* Auxiliary Device Output Buffer Full */

पूर्णाङ्क direct_read_char(अचिन्हित वर्ण *ch)
{
	अचिन्हित दीर्घ sb;

	sb = inb(KB_STATUS);

	यदि (!(sb & KB_OUT_FULL)) {
		फर्काऊ 0;
	}

	inb(KEYBD);

	यदि (!(sb & KB_AUX_BYTE))
		फर्काऊ 1;

	फर्काऊ 0;
}

