
#ضمن <minix/minlib.h>
#ضمن <minix/cpufeature.h>
#ضمن <machine/partition.h>
#ضمن "string.h"
#ضمن "direct_utils.h"
#ضمن "serial.h"
#ضمن "glo.h"

/* Give non-zero values to avoid them in BSS */
ساكن صحيح اطبع_سطر = 1, اطبع_col = 1;

#ضمن <sys/video.h>

خارجي حرف *video_mem;
#define VIDOFFSET(سطر, col) ((سطر) * MULTIBOOT_CONSOLE_COLS * 2 + (col) * 2)
#define VIDSIZE VIDOFFSET(MULTIBOOT_CONSOLE_أسطر-1,MULTIBOOT_CONSOLE_COLS-1)

فراغ direct_put_char(حرف c, صحيح سطر, صحيح col) 
{
	صحيح offset = VIDOFFSET(سطر, col);
	video_mem[offset] = c;
	video_mem[offset+1] = 0x07;	/* grey-on-black */
}

ساكن حرف direct_get_char(صحيح سطر, صحيح col) 
{
	أرجع video_mem[VIDOFFSET(سطر, col)];
}

فراغ direct_cls(فراغ)
{
	/* Clear screen */
	صحيح i,j;

	لكل(i = 0; i < MULTIBOOT_CONSOLE_COLS; i++)
		لكل(j = 0; j < MULTIBOOT_CONSOLE_أسطر; j++)
			direct_put_char(' ', j, i);

	اطبع_سطر = اطبع_col = 0;

	/* Tell video hardware origin is 0. */
	outb(C_6845+INDEX, VID_ORG);
	outb(C_6845+DATA, 0);
	outb(C_6845+INDEX, VID_ORG+1);
	outb(C_6845+DATA, 0);
}

ساكن فراغ direct_scroll_up(صحيح أسطر) 
{
	صحيح i, j;
	لكل (i = 0; i < MULTIBOOT_CONSOLE_أسطر; i++ ) {
		لكل (j = 0; j < MULTIBOOT_CONSOLE_COLS; j++ ) {
			حرف c = 0;
			إذا(i < MULTIBOOT_CONSOLE_أسطر-أسطر)
				c = direct_get_char(i + أسطر, j);
			direct_put_char(c, i, j);
		}
	}
	اطبع_سطر-= أسطر;
}

فراغ direct_اطبع_char(حرف c)
{
	طالما (اطبع_سطر >= MULTIBOOT_CONSOLE_أسطر)
		direct_scroll_up(1);

#define TABWIDTH 8
	إذا(c == '\t') {
		إذا(اطبع_col >= MULTIBOOT_CONSOLE_COLS - TABWIDTH) {
			c = '\n';
		} وإلا {
			افعل {
				direct_put_char(' ', اطبع_سطر, اطبع_col++);
			} طالما(اطبع_col % TABWIDTH);
			أرجع;
		}
	}

	إذا (c == '\n') {
		طالما (اطبع_col < MULTIBOOT_CONSOLE_COLS)
			direct_put_char(' ', اطبع_سطر, اطبع_col++);
		اطبع_سطر++;
		اطبع_col = 0;
		أرجع;
	}

	direct_put_char(c, اطبع_سطر, اطبع_col++);

	إذا (اطبع_col >= MULTIBOOT_CONSOLE_COLS) {
		اطبع_سطر++;
		اطبع_col = 0;
	}

	طالما (اطبع_سطر >= MULTIBOOT_CONSOLE_أسطر)
		direct_scroll_up(1);
}

فراغ direct_اطبع(ثابت حرف *str)
{
	طالما (*str) {
		direct_اطبع_char(*str);
		str++;
	}
}

/* Standard and AT keyboard.  (PS/2 MCA implies AT throughout.) */
#define KEYBD		0x60	/* I/O port for keyboard data */
#define KB_STATUS	0x64	/* I/O port for status on AT */
#define KB_OUT_FULL	0x01	/* status bit set when keypress char pending */
#define KB_AUX_BYTE	0x20	/* Auxiliary Device Output Buffer Full */

صحيح direct_read_char(unsigned حرف *ch)
{
	unsigned long sb;

	sb = inb(KB_STATUS);

	إذا (!(sb & KB_OUT_FULL)) {
		أرجع 0;
	}

	inb(KEYBD);

	إذا (!(sb & KB_AUX_BYTE))
		أرجع 1;

	أرجع 0;
}

