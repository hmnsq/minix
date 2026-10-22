
#ਅਨ੍ਤਰ੍ਭਾਵ <minix/minlib.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <minix/cpufeature.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <machine/partition.h>
#ਅਨ੍ਤਰ੍ਭਾਵ "string.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "direct_utils.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "serial.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "glo.h"

/* Give non-zero values to avoid them in BSS */
ਸਥਿਰ ਪੂਰਨ print_ਸਤਰ = 1, print_ਖਾਨਾ = 1;

#ਅਨ੍ਤਰ੍ਭਾਵ <sys/video.h>

ਬਾਹਰੀ ਅੱਖਰ *video_mem;
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VIDOFFSET(ਸਤਰ, ਖਾਨਾ) ((ਸਤਰ) * MULTIBOOT_CONSOLE_ਖਾਨੇ_ਗਿਣਤੀ * 2 + (ਖਾਨਾ) * 2)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VIDSIZE VIDOFFSET(MULTIBOOT_CONSOLE_ਸਤਰਾਂ-1,MULTIBOOT_CONSOLE_ਖਾਨੇ_ਗਿਣਤੀ-1)

ਖਾਲੀ ਸਿੱਧਾ_put_char(ਅੱਖਰ c, ਪੂਰਨ ਸਤਰ, ਪੂਰਨ ਖਾਨਾ) 
{
	ਪੂਰਨ ਵਿੱਥ = VIDOFFSET(ਸਤਰ, ਖਾਨਾ);
	video_mem[ਵਿੱਥ] = c;
	video_mem[ਵਿੱਥ+1] = 0x07;	/* grey-on-black */
}

ਸਥਿਰ ਅੱਖਰ ਸਿੱਧਾ_get_char(ਪੂਰਨ ਸਤਰ, ਪੂਰਨ ਖਾਨਾ) 
{
	ਵਾਪਸ video_mem[VIDOFFSET(ਸਤਰ, ਖਾਨਾ)];
}

ਖਾਲੀ ਸਿੱਧਾ_cls(ਖਾਲੀ)
{
	/* Clear screen */
	ਪੂਰਨ i,j;

	ਲਈ(i = 0; i < MULTIBOOT_CONSOLE_ਖਾਨੇ_ਗਿਣਤੀ; i++)
		ਲਈ(j = 0; j < MULTIBOOT_CONSOLE_ਸਤਰਾਂ; j++)
			ਸਿੱਧਾ_put_char(' ', j, i);

	print_ਸਤਰ = print_ਖਾਨਾ = 0;

	/* Tell video hardware origin is 0. */
	outb(C_6845+ਸੂਚਕਾਂਕ_2, VID_ORG);
	outb(C_6845+ਅੰਕੜੇ_2, 0);
	outb(C_6845+ਸੂਚਕਾਂਕ_2, VID_ORG+1);
	outb(C_6845+ਅੰਕੜੇ_2, 0);
}

ਸਥਿਰ ਖਾਲੀ ਸਿੱਧਾ_scroll_ਉੱਪਰ(ਪੂਰਨ ਸਤਰਾਂ) 
{
	ਪੂਰਨ i, j;
	ਲਈ (i = 0; i < MULTIBOOT_CONSOLE_ਸਤਰਾਂ; i++ ) {
		ਲਈ (j = 0; j < MULTIBOOT_CONSOLE_ਖਾਨੇ_ਗਿਣਤੀ; j++ ) {
			ਅੱਖਰ c = 0;
			ਜੇ(i < MULTIBOOT_CONSOLE_ਸਤਰਾਂ-ਸਤਰਾਂ)
				c = ਸਿੱਧਾ_get_char(i + ਸਤਰਾਂ, j);
			ਸਿੱਧਾ_put_char(c, i, j);
		}
	}
	print_ਸਤਰ-= ਸਤਰਾਂ;
}

ਖਾਲੀ ਸਿੱਧਾ_print_char(ਅੱਖਰ c)
{
	ਜਦੋਂ (print_ਸਤਰ >= MULTIBOOT_CONSOLE_ਸਤਰਾਂ)
		ਸਿੱਧਾ_scroll_ਉੱਪਰ(1);

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TABWIDTH 8
	ਜੇ(c == '\t') {
		ਜੇ(print_ਖਾਨਾ >= MULTIBOOT_CONSOLE_ਖਾਨੇ_ਗਿਣਤੀ - TABWIDTH) {
			c = '\n';
		} ਨਹੀਂਤਾਂ {
			ਕਰੋ {
				ਸਿੱਧਾ_put_char(' ', print_ਸਤਰ, print_ਖਾਨਾ++);
			} ਜਦੋਂ(print_ਖਾਨਾ % TABWIDTH);
			ਵਾਪਸ;
		}
	}

	ਜੇ (c == '\n') {
		ਜਦੋਂ (print_ਖਾਨਾ < MULTIBOOT_CONSOLE_ਖਾਨੇ_ਗਿਣਤੀ)
			ਸਿੱਧਾ_put_char(' ', print_ਸਤਰ, print_ਖਾਨਾ++);
		print_ਸਤਰ++;
		print_ਖਾਨਾ = 0;
		ਵਾਪਸ;
	}

	ਸਿੱਧਾ_put_char(c, print_ਸਤਰ, print_ਖਾਨਾ++);

	ਜੇ (print_ਖਾਨਾ >= MULTIBOOT_CONSOLE_ਖਾਨੇ_ਗਿਣਤੀ) {
		print_ਸਤਰ++;
		print_ਖਾਨਾ = 0;
	}

	ਜਦੋਂ (print_ਸਤਰ >= MULTIBOOT_CONSOLE_ਸਤਰਾਂ)
		ਸਿੱਧਾ_scroll_ਉੱਪਰ(1);
}

ਖਾਲੀ ਸਿੱਧਾ_print(ਅਚਲ ਅੱਖਰ *str)
{
	ਜਦੋਂ (*str) {
		ਸਿੱਧਾ_print_char(*str);
		str++;
	}
}

/* Standard and AT keyboard.  (PS/2 MCA implies AT throughout.) */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ KEYBD		0x60	/* I/O port for keyboard data */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ KB_ਸਥਿਤੀ	0x64	/* I/O port for status on AT */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ KB_ਬਾਹਰਲਾ_ਭਰਿਆ	0x01	/* status bit set when keypress char pending */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ KB_AUX_BYTE	0x20	/* Auxiliary Device Output Buffer Full */

ਪੂਰਨ ਸਿੱਧਾ_read_char(ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ch)
{
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ sb;

	sb = inb(KB_ਸਥਿਤੀ);

	ਜੇ (!(sb & KB_ਬਾਹਰਲਾ_ਭਰਿਆ)) {
		ਵਾਪਸ 0;
	}

	inb(KEYBD);

	ਜੇ (!(sb & KB_AUX_BYTE))
		ਵਾਪਸ 1;

	ਵਾਪਸ 0;
}

