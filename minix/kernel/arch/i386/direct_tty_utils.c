
#ಅನ್ತರ್ಭಾವ <minix/minlib.h>
#ಅನ್ತರ್ಭಾವ <minix/cpufeature.h>
#ಅನ್ತರ್ಭಾವ <machine/partition.h>
#ಅನ್ತರ್ಭಾವ "string.h"
#ಅನ್ತರ್ಭಾವ "direct_utils.h"
#ಅನ್ತರ್ಭಾವ "serial.h"
#ಅನ್ತರ್ಭಾವ "glo.h"

/* Give non-zero values to avoid them in BSS */
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ print_ಸಾಲು = 1, print_col = 1;

#ಅನ್ತರ್ಭಾವ <sys/video.h>

ಬಾಹ್ಯ ವರ್ಣ *video_mem;
#define VIDOFFSET(ಸಾಲು, col) ((ಸಾಲು) * MULTIBOOT_CONSOLE_COLS * 2 + (col) * 2)
#define VIDSIZE VIDOFFSET(MULTIBOOT_CONSOLE_ಸಾಲುಗಳು-1,MULTIBOOT_CONSOLE_COLS-1)

ಶೂನ್ಯ direct_put_char(ವರ್ಣ c, ಪೂರ್ಣಾಙ್ಕ ಸಾಲು, ಪೂರ್ಣಾಙ್ಕ col) 
{
	ಪೂರ್ಣಾಙ್ಕ offset = VIDOFFSET(ಸಾಲು, col);
	video_mem[offset] = c;
	video_mem[offset+1] = 0x07;	/* grey-on-black */
}

ಸ್ಥಿರ ವರ್ಣ direct_get_char(ಪೂರ್ಣಾಙ್ಕ ಸಾಲು, ಪೂರ್ಣಾಙ್ಕ col) 
{
	ಪ್ರತ್ಯಾಗಮ video_mem[VIDOFFSET(ಸಾಲು, col)];
}

ಶೂನ್ಯ direct_cls(ಶೂನ್ಯ)
{
	/* Clear screen */
	ಪೂರ್ಣಾಙ್ಕ i,j;

	ಪ್ರತಿ(i = 0; i < MULTIBOOT_CONSOLE_COLS; i++)
		ಪ್ರತಿ(j = 0; j < MULTIBOOT_CONSOLE_ಸಾಲುಗಳು; j++)
			direct_put_char(' ', j, i);

	print_ಸಾಲು = print_col = 0;

	/* Tell video hardware origin is 0. */
	outb(C_6845+INDEX, VID_ORG);
	outb(C_6845+DATA, 0);
	outb(C_6845+INDEX, VID_ORG+1);
	outb(C_6845+DATA, 0);
}

ಸ್ಥಿರ ಶೂನ್ಯ direct_scroll_up(ಪೂರ್ಣಾಙ್ಕ ಸಾಲುಗಳು) 
{
	ಪೂರ್ಣಾಙ್ಕ i, j;
	ಪ್ರತಿ (i = 0; i < MULTIBOOT_CONSOLE_ಸಾಲುಗಳು; i++ ) {
		ಪ್ರತಿ (j = 0; j < MULTIBOOT_CONSOLE_COLS; j++ ) {
			ವರ್ಣ c = 0;
			ಯದಿ(i < MULTIBOOT_CONSOLE_ಸಾಲುಗಳು-ಸಾಲುಗಳು)
				c = direct_get_char(i + ಸಾಲುಗಳು, j);
			direct_put_char(c, i, j);
		}
	}
	print_ಸಾಲು-= ಸಾಲುಗಳು;
}

ಶೂನ್ಯ direct_print_char(ವರ್ಣ c)
{
	ಯಾವತ್ (print_ಸಾಲು >= MULTIBOOT_CONSOLE_ಸಾಲುಗಳು)
		direct_scroll_up(1);

#define TABWIDTH 8
	ಯದಿ(c == '\t') {
		ಯದಿ(print_col >= MULTIBOOT_CONSOLE_COLS - TABWIDTH) {
			c = '\n';
		} ಅನ್ಯಥಾ {
			ಕುರು {
				direct_put_char(' ', print_ಸಾಲು, print_col++);
			} ಯಾವತ್(print_col % TABWIDTH);
			ಪ್ರತ್ಯಾಗಮ;
		}
	}

	ಯದಿ (c == '\n') {
		ಯಾವತ್ (print_col < MULTIBOOT_CONSOLE_COLS)
			direct_put_char(' ', print_ಸಾಲು, print_col++);
		print_ಸಾಲು++;
		print_col = 0;
		ಪ್ರತ್ಯಾಗಮ;
	}

	direct_put_char(c, print_ಸಾಲು, print_col++);

	ಯದಿ (print_col >= MULTIBOOT_CONSOLE_COLS) {
		print_ಸಾಲು++;
		print_col = 0;
	}

	ಯಾವತ್ (print_ಸಾಲು >= MULTIBOOT_CONSOLE_ಸಾಲುಗಳು)
		direct_scroll_up(1);
}

ಶೂನ್ಯ direct_print(ಧ್ರುವ ವರ್ಣ *str)
{
	ಯಾವತ್ (*str) {
		direct_print_char(*str);
		str++;
	}
}

/* Standard and AT keyboard.  (PS/2 MCA implies AT throughout.) */
#define KEYBD		0x60	/* I/O port for keyboard data */
#define KB_STATUS	0x64	/* I/O port for status on AT */
#define KB_OUT_FULL	0x01	/* status bit set when keypress char pending */
#define KB_AUX_BYTE	0x20	/* Auxiliary Device Output Buffer Full */

ಪೂರ್ಣಾಙ್ಕ direct_read_char(unsigned ವರ್ಣ *ch)
{
	unsigned long sb;

	sb = inb(KB_STATUS);

	ಯದಿ (!(sb & KB_OUT_FULL)) {
		ಪ್ರತ್ಯಾಗಮ 0;
	}

	inb(KEYBD);

	ಯದಿ (!(sb & KB_AUX_BYTE))
		ಪ್ರತ್ಯಾಗಮ 1;

	ಪ್ರತ್ಯಾಗಮ 0;
}

