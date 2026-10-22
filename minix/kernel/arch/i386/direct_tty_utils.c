
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
#परिभाषय VIDOFFSET(पङ्क्तिः, col) ((पङ्क्तिः) * MULTIBOOT_CONSOLE_स्तम्भसङ्ख्या * 2 + (col) * 2)
#परिभाषय VIDSIZE VIDOFFSET(MULTIBOOT_CONSOLE_पङ्क्तयः-1,MULTIBOOT_CONSOLE_स्तम्भसङ्ख्या-1)

शून्य प्रत्यक्षम्_put_char(वर्ण c, पूर्णाङ्क पङ्क्तिः, पूर्णाङ्क col) 
{
	पूर्णाङ्क विस्थापनम् = VIDOFFSET(पङ्क्तिः, col);
	video_mem[विस्थापनम्] = c;
	video_mem[विस्थापनम्+1] = 0x07;	/* grey-on-black */
}

स्थिर वर्ण प्रत्यक्षम्_get_char(पूर्णाङ्क पङ्क्तिः, पूर्णाङ्क col) 
{
	प्रत्यागम video_mem[VIDOFFSET(पङ्क्तिः, col)];
}

शून्य प्रत्यक्षम्_cls(शून्य)
{
	/* Clear screen */
	पूर्णाङ्क i,j;

	प्रति(i = 0; i < MULTIBOOT_CONSOLE_स्तम्भसङ्ख्या; i++)
		प्रति(j = 0; j < MULTIBOOT_CONSOLE_पङ्क्तयः; j++)
			प्रत्यक्षम्_put_char(' ', j, i);

	print_पङ्क्तिः = print_col = 0;

	/* Tell video hardware origin is 0. */
	outb(C_6845+सूचकांक_2, VID_ORG);
	outb(C_6845+दत्त_2, 0);
	outb(C_6845+सूचकांक_2, VID_ORG+1);
	outb(C_6845+दत्त_2, 0);
}

स्थिर शून्य प्रत्यक्षम्_scroll_ऊर्ध्वम्(पूर्णाङ्क पङ्क्तयः) 
{
	पूर्णाङ्क i, j;
	प्रति (i = 0; i < MULTIBOOT_CONSOLE_पङ्क्तयः; i++ ) {
		प्रति (j = 0; j < MULTIBOOT_CONSOLE_स्तम्भसङ्ख्या; j++ ) {
			वर्ण c = 0;
			यदि(i < MULTIBOOT_CONSOLE_पङ्क्तयः-पङ्क्तयः)
				c = प्रत्यक्षम्_get_char(i + पङ्क्तयः, j);
			प्रत्यक्षम्_put_char(c, i, j);
		}
	}
	print_पङ्क्तिः-= पङ्क्तयः;
}

शून्य प्रत्यक्षम्_print_char(वर्ण c)
{
	यावत् (print_पङ्क्तिः >= MULTIBOOT_CONSOLE_पङ्क्तयः)
		प्रत्यक्षम्_scroll_ऊर्ध्वम्(1);

#परिभाषय TABWIDTH 8
	यदि(c == '\t') {
		यदि(print_col >= MULTIBOOT_CONSOLE_स्तम्भसङ्ख्या - TABWIDTH) {
			c = '\n';
		} अन्यथा {
			कुरु {
				प्रत्यक्षम्_put_char(' ', print_पङ्क्तिः, print_col++);
			} यावत्(print_col % TABWIDTH);
			प्रत्यागम;
		}
	}

	यदि (c == '\n') {
		यावत् (print_col < MULTIBOOT_CONSOLE_स्तम्भसङ्ख्या)
			प्रत्यक्षम्_put_char(' ', print_पङ्क्तिः, print_col++);
		print_पङ्क्तिः++;
		print_col = 0;
		प्रत्यागम;
	}

	प्रत्यक्षम्_put_char(c, print_पङ्क्तिः, print_col++);

	यदि (print_col >= MULTIBOOT_CONSOLE_स्तम्भसङ्ख्या) {
		print_पङ्क्तिः++;
		print_col = 0;
	}

	यावत् (print_पङ्क्तिः >= MULTIBOOT_CONSOLE_पङ्क्तयः)
		प्रत्यक्षम्_scroll_ऊर्ध्वम्(1);
}

शून्य प्रत्यक्षम्_print(ध्रुव वर्ण *str)
{
	यावत् (*str) {
		प्रत्यक्षम्_print_char(*str);
		str++;
	}
}

/* Standard and AT keyboard.  (PS/2 MCA implies AT throughout.) */
#परिभाषय KEYBD		0x60	/* I/O port for keyboard data */
#परिभाषय KB_स्थितिः	0x64	/* I/O port for status on AT */
#परिभाषय KB_बहिर्गमनम्_पूर्णम्	0x01	/* status bit set when keypress char pending */
#परिभाषय KB_AUX_BYTE	0x20	/* Auxiliary Device Output Buffer Full */

पूर्णाङ्क प्रत्यक्षम्_read_char(अचिह्नित वर्ण *ch)
{
	अचिह्नित दीर्घ sb;

	sb = inb(KB_स्थितिः);

	यदि (!(sb & KB_बहिर्गमनम्_पूर्णम्)) {
		प्रत्यागम 0;
	}

	inb(KEYBD);

	यदि (!(sb & KB_AUX_BYTE))
		प्रत्यागम 1;

	प्रत्यागम 0;
}

