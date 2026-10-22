
#अन्तर्भाव <minix/minlib.h>
#अन्तर्भाव <minix/cpufeature.h>
#अन्तर्भाव <machine/partition.h>
#अन्तर्भाव "string.h"
#अन्तर्भाव "direct_utils.h"
#अन्तर्भाव "serial.h"
#अन्तर्भाव "glo.h"

/* Give non-zero values to avoid them in BSS */
स्थिर पूर्णांक print_ओळ = 1, print_col = 1;

#अन्तर्भाव <sys/video.h>

बाह्य अक्षर *video_mem;
#define VIDOFFSET(ओळ, col) ((ओळ) * MULTIBOOT_CONSOLE_COLS * 2 + (col) * 2)
#define VIDSIZE VIDOFFSET(MULTIBOOT_CONSOLE_ओळी-1,MULTIBOOT_CONSOLE_COLS-1)

शून्य direct_put_char(अक्षर c, पूर्णांक ओळ, पूर्णांक col) 
{
	पूर्णांक offset = VIDOFFSET(ओळ, col);
	video_mem[offset] = c;
	video_mem[offset+1] = 0x07;	/* grey-on-black */
}

स्थिर अक्षर direct_get_char(पूर्णांक ओळ, पूर्णांक col) 
{
	परत video_mem[VIDOFFSET(ओळ, col)];
}

शून्य direct_cls(शून्य)
{
	/* Clear screen */
	पूर्णांक i,j;

	साठी(i = 0; i < MULTIBOOT_CONSOLE_COLS; i++)
		साठी(j = 0; j < MULTIBOOT_CONSOLE_ओळी; j++)
			direct_put_char(' ', j, i);

	print_ओळ = print_col = 0;

	/* Tell video hardware origin is 0. */
	outb(C_6845+INDEX, VID_ORG);
	outb(C_6845+DATA, 0);
	outb(C_6845+INDEX, VID_ORG+1);
	outb(C_6845+DATA, 0);
}

स्थिर शून्य direct_scroll_up(पूर्णांक ओळी) 
{
	पूर्णांक i, j;
	साठी (i = 0; i < MULTIBOOT_CONSOLE_ओळी; i++ ) {
		साठी (j = 0; j < MULTIBOOT_CONSOLE_COLS; j++ ) {
			अक्षर c = 0;
			जर(i < MULTIBOOT_CONSOLE_ओळी-ओळी)
				c = direct_get_char(i + ओळी, j);
			direct_put_char(c, i, j);
		}
	}
	print_ओळ-= ओळी;
}

शून्य direct_print_char(अक्षर c)
{
	पर्यंत (print_ओळ >= MULTIBOOT_CONSOLE_ओळी)
		direct_scroll_up(1);

#define TABWIDTH 8
	जर(c == '\t') {
		जर(print_col >= MULTIBOOT_CONSOLE_COLS - TABWIDTH) {
			c = '\n';
		} अन्यथा {
			करा {
				direct_put_char(' ', print_ओळ, print_col++);
			} पर्यंत(print_col % TABWIDTH);
			परत;
		}
	}

	जर (c == '\n') {
		पर्यंत (print_col < MULTIBOOT_CONSOLE_COLS)
			direct_put_char(' ', print_ओळ, print_col++);
		print_ओळ++;
		print_col = 0;
		परत;
	}

	direct_put_char(c, print_ओळ, print_col++);

	जर (print_col >= MULTIBOOT_CONSOLE_COLS) {
		print_ओळ++;
		print_col = 0;
	}

	पर्यंत (print_ओळ >= MULTIBOOT_CONSOLE_ओळी)
		direct_scroll_up(1);
}

शून्य direct_print(स्थिरांक अक्षर *str)
{
	पर्यंत (*str) {
		direct_print_char(*str);
		str++;
	}
}

/* Standard and AT keyboard.  (PS/2 MCA implies AT throughout.) */
#define KEYBD		0x60	/* I/O port for keyboard data */
#define KB_STATUS	0x64	/* I/O port for status on AT */
#define KB_OUT_FULL	0x01	/* status bit set when keypress char pending */
#define KB_AUX_BYTE	0x20	/* Auxiliary Device Output Buffer Full */

पूर्णांक direct_read_char(अचिन्हित अक्षर *ch)
{
	अचिन्हित दीर्घ sb;

	sb = inb(KB_STATUS);

	जर (!(sb & KB_OUT_FULL)) {
		परत 0;
	}

	inb(KEYBD);

	जर (!(sb & KB_AUX_BYTE))
		परत 1;

	परत 0;
}

