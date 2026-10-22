
#समावेश <minix/minlib.h>
#समावेश <minix/cpufeature.h>
#समावेश <machine/partition.h>
#समावेश "string.h"
#समावेश "direct_utils.h"
#समावेश "serial.h"
#समावेश "glo.h"

/* Give non-zero values to avoid them in BSS */
अटल पूर्णांक print_पंक्ति = 1, print_col = 1;

#समावेश <sys/video.h>

बाह्य अक्षर *video_mem;
#घोषणा VIDOFFSET(पंक्ति_2, col) ((पंक्ति_2) * MULTIBOOT_CONSOLE_COLS * 2 + (col) * 2)
#घोषणा VIDSIZE VIDOFFSET(MULTIBOOT_CONSOLE_पंक्तियाँ-1,MULTIBOOT_CONSOLE_COLS-1)

व्योम direct_put_char(अक्षर c, पूर्णांक पंक्ति_2, पूर्णांक col) 
{
	पूर्णांक offset = VIDOFFSET(पंक्ति_2, col);
	video_mem[offset] = c;
	video_mem[offset+1] = 0x07;	/* grey-on-black */
}

अटल अक्षर direct_get_char(पूर्णांक पंक्ति_2, पूर्णांक col) 
{
	वापस video_mem[VIDOFFSET(पंक्ति_2, col)];
}

व्योम direct_cls(व्योम)
{
	/* Clear screen */
	पूर्णांक i,j;

	क्रम(i = 0; i < MULTIBOOT_CONSOLE_COLS; i++)
		क्रम(j = 0; j < MULTIBOOT_CONSOLE_पंक्तियाँ; j++)
			direct_put_char(' ', j, i);

	print_पंक्ति = print_col = 0;

	/* Tell video hardware origin is 0. */
	outb(C_6845+सूचकांक_2, VID_ORG);
	outb(C_6845+तथ्य_2, 0);
	outb(C_6845+सूचकांक_2, VID_ORG+1);
	outb(C_6845+तथ्य_2, 0);
}

अटल व्योम direct_scroll_up(पूर्णांक पंक्तियाँ) 
{
	पूर्णांक i, j;
	क्रम (i = 0; i < MULTIBOOT_CONSOLE_पंक्तियाँ; i++ ) {
		क्रम (j = 0; j < MULTIBOOT_CONSOLE_COLS; j++ ) {
			अक्षर c = 0;
			अगर(i < MULTIBOOT_CONSOLE_पंक्तियाँ-पंक्तियाँ)
				c = direct_get_char(i + पंक्तियाँ, j);
			direct_put_char(c, i, j);
		}
	}
	print_पंक्ति-= पंक्तियाँ;
}

व्योम direct_print_char(अक्षर c)
{
	जबतक (print_पंक्ति >= MULTIBOOT_CONSOLE_पंक्तियाँ)
		direct_scroll_up(1);

#घोषणा TABWIDTH 8
	अगर(c == '\t') {
		अगर(print_col >= MULTIBOOT_CONSOLE_COLS - TABWIDTH) {
			c = '\n';
		} अन्यथा {
			करो {
				direct_put_char(' ', print_पंक्ति, print_col++);
			} जबतक(print_col % TABWIDTH);
			वापस;
		}
	}

	अगर (c == '\n') {
		जबतक (print_col < MULTIBOOT_CONSOLE_COLS)
			direct_put_char(' ', print_पंक्ति, print_col++);
		print_पंक्ति++;
		print_col = 0;
		वापस;
	}

	direct_put_char(c, print_पंक्ति, print_col++);

	अगर (print_col >= MULTIBOOT_CONSOLE_COLS) {
		print_पंक्ति++;
		print_col = 0;
	}

	जबतक (print_पंक्ति >= MULTIBOOT_CONSOLE_पंक्तियाँ)
		direct_scroll_up(1);
}

व्योम direct_print(स्थिर अक्षर *str)
{
	जबतक (*str) {
		direct_print_char(*str);
		str++;
	}
}

/* Standard and AT keyboard.  (PS/2 MCA implies AT throughout.) */
#घोषणा KEYBD		0x60	/* I/O port for keyboard data */
#घोषणा KB_STATUS	0x64	/* I/O port for status on AT */
#घोषणा KB_OUT_FULL	0x01	/* status bit set when keypress char pending */
#घोषणा KB_AUX_BYTE	0x20	/* Auxiliary Device Output Buffer Full */

पूर्णांक direct_read_char(अचिन्हित अक्षर *ch)
{
	अचिन्हित दीर्घ sb;

	sb = inb(KB_STATUS);

	अगर (!(sb & KB_OUT_FULL)) {
		वापस 0;
	}

	inb(KEYBD);

	अगर (!(sb & KB_AUX_BYTE))
		वापस 1;

	वापस 0;
}

