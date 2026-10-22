
#ضمن <minix/minlib.h>
#ضمن <minix/cpufeature.h>
#ضمن <machine/partition.h>
#ضمن "string.h"
#ضمن "direct_utils.h"
#ضمن "serial.h"
#ضمن "glo.h"

/* Give non-zero values to avoid them in BSS */
ساكن صحيح print_سٽ = 1, print_کانو = 1;

#ضمن <sys/video.h>

خارجي حرف *video_mem;
#وصف_ڪريو VIDOFFSET(سٽ, کانو) ((سٽ) * MULTIBOOT_CONSOLE_کانن_ڳڻپ * 2 + (کانو) * 2)
#وصف_ڪريو VIDSIZE VIDOFFSET(MULTIBOOT_CONSOLE_سٽون-1,MULTIBOOT_CONSOLE_کانن_ڳڻپ-1)

فراغ سڌو_put_char(حرف c, صحيح سٽ, صحيح کانو) 
{
	صحيح وٿي = VIDOFFSET(سٽ, کانو);
	video_mem[وٿي] = c;
	video_mem[وٿي+1] = 0x07;	/* grey-on-black */
}

ساكن حرف سڌو_get_char(صحيح سٽ, صحيح کانو) 
{
	أرجع video_mem[VIDOFFSET(سٽ, کانو)];
}

فراغ سڌو_cls(فراغ)
{
	/* Clear screen */
	صحيح i,j;

	لكل(i = 0; i < MULTIBOOT_CONSOLE_کانن_ڳڻپ; i++)
		لكل(j = 0; j < MULTIBOOT_CONSOLE_سٽون; j++)
			سڌو_put_char(' ', j, i);

	print_سٽ = print_کانو = 0;

	/* Tell video hardware origin is 0. */
	outb(C_6845+اشاريو_2, VID_ORG);
	outb(C_6845+معلومات_2, 0);
	outb(C_6845+اشاريو_2, VID_ORG+1);
	outb(C_6845+معلومات_2, 0);
}

ساكن فراغ سڌو_scroll_مٿي(صحيح سٽون) 
{
	صحيح i, j;
	لكل (i = 0; i < MULTIBOOT_CONSOLE_سٽون; i++ ) {
		لكل (j = 0; j < MULTIBOOT_CONSOLE_کانن_ڳڻپ; j++ ) {
			حرف c = 0;
			إذا(i < MULTIBOOT_CONSOLE_سٽون-سٽون)
				c = سڌو_get_char(i + سٽون, j);
			سڌو_put_char(c, i, j);
		}
	}
	print_سٽ-= سٽون;
}

فراغ سڌو_print_char(حرف c)
{
	طالما (print_سٽ >= MULTIBOOT_CONSOLE_سٽون)
		سڌو_scroll_مٿي(1);

#وصف_ڪريو TABWIDTH 8
	إذا(c == '\t') {
		إذا(print_کانو >= MULTIBOOT_CONSOLE_کانن_ڳڻپ - TABWIDTH) {
			c = '\n';
		} وإلا {
			افعل {
				سڌو_put_char(' ', print_سٽ, print_کانو++);
			} طالما(print_کانو % TABWIDTH);
			أرجع;
		}
	}

	إذا (c == '\n') {
		طالما (print_کانو < MULTIBOOT_CONSOLE_کانن_ڳڻپ)
			سڌو_put_char(' ', print_سٽ, print_کانو++);
		print_سٽ++;
		print_کانو = 0;
		أرجع;
	}

	سڌو_put_char(c, print_سٽ, print_کانو++);

	إذا (print_کانو >= MULTIBOOT_CONSOLE_کانن_ڳڻپ) {
		print_سٽ++;
		print_کانو = 0;
	}

	طالما (print_سٽ >= MULTIBOOT_CONSOLE_سٽون)
		سڌو_scroll_مٿي(1);
}

فراغ سڌو_print(ثابت حرف *str)
{
	طالما (*str) {
		سڌو_print_char(*str);
		str++;
	}
}

/* Standard and AT keyboard.  (PS/2 MCA implies AT throughout.) */
#وصف_ڪريو KEYBD		0x60	/* I/O port for keyboard data */
#وصف_ڪريو KB_حالت	0x64	/* I/O port for status on AT */
#وصف_ڪريو KB_ٻاهريون_ڀريل	0x01	/* status bit set when keypress char pending */
#وصف_ڪريو KB_AUX_BYTE	0x20	/* Auxiliary Device Output Buffer Full */

صحيح سڌو_read_char(بنانشان حرف *ch)
{
	بنانشان ڊگهو sb;

	sb = inb(KB_حالت);

	إذا (!(sb & KB_ٻاهريون_ڀريل)) {
		أرجع 0;
	}

	inb(KEYBD);

	إذا (!(sb & KB_AUX_BYTE))
		أرجع 1;

	أرجع 0;
}

