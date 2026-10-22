
#ಅನ್ತರ್ಭಾವ <minix/minlib.h>
#ಅನ್ತರ್ಭಾವ <minix/cpufeature.h>
#ಅನ್ತರ್ಭಾವ <machine/partition.h>
#ಅನ್ತರ್ಭಾವ "string.h"
#ಅನ್ತರ್ಭಾವ "direct_utils.h"
#ಅನ್ತರ್ಭಾವ "serial.h"
#ಅನ್ತರ್ಭಾವ "glo.h"

/* Give non-zero values to avoid them in BSS */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ print_ಸಾಲು = 1, print_ಲಂಬಸಾಲು = 1;

#ಅನ್ತರ್ಭಾವ <sys/video.h>

ಬಾಹ್ಯ ಅಕ್ಷರ *video_mem;
#ವ್ಯಾಖ್ಯಾನಿಸು VIDOFFSET(ಸಾಲು, ಲಂಬಸಾಲು) ((ಸಾಲು) * MULTIBOOT_CONSOLE_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ * 2 + (ಲಂಬಸಾಲು) * 2)
#ವ್ಯಾಖ್ಯಾನಿಸು VIDSIZE VIDOFFSET(MULTIBOOT_CONSOLE_ಸಾಲುಗಳು-1,MULTIBOOT_CONSOLE_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ-1)

ಶೂನ್ಯ ನೇರ_put_char(ಅಕ್ಷರ c, ಪೂರ್ಣಾಂಕ ಸಾಲು, ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು) 
{
	ಪೂರ್ಣಾಂಕ ಸ್ಥಾನಾಂತರ = VIDOFFSET(ಸಾಲು, ಲಂಬಸಾಲು);
	video_mem[ಸ್ಥಾನಾಂತರ] = c;
	video_mem[ಸ್ಥಾನಾಂತರ+1] = 0x07;	/* grey-on-black */
}

ಸ್ಥಿರ ಅಕ್ಷರ ನೇರ_get_char(ಪೂರ್ಣಾಂಕ ಸಾಲು, ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು) 
{
	ಹಿಂತಿರುಗಿ video_mem[VIDOFFSET(ಸಾಲು, ಲಂಬಸಾಲು)];
}

ಶೂನ್ಯ ನೇರ_cls(ಶೂನ್ಯ)
{
	/* Clear screen */
	ಪೂರ್ಣಾಂಕ i,j;

	ಗಾಗಿ(i = 0; i < MULTIBOOT_CONSOLE_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ; i++)
		ಗಾಗಿ(j = 0; j < MULTIBOOT_CONSOLE_ಸಾಲುಗಳು; j++)
			ನೇರ_put_char(' ', j, i);

	print_ಸಾಲು = print_ಲಂಬಸಾಲು = 0;

	/* Tell video hardware origin is 0. */
	outb(C_6845+ಸೂಚ್ಯಂಕ_2, VID_ORG);
	outb(C_6845+ದತ್ತಾಂಶ_2, 0);
	outb(C_6845+ಸೂಚ್ಯಂಕ_2, VID_ORG+1);
	outb(C_6845+ದತ್ತಾಂಶ_2, 0);
}

ಸ್ಥಿರ ಶೂನ್ಯ ನೇರ_scroll_ಮೇಲೆ(ಪೂರ್ಣಾಂಕ ಸಾಲುಗಳು) 
{
	ಪೂರ್ಣಾಂಕ i, j;
	ಗಾಗಿ (i = 0; i < MULTIBOOT_CONSOLE_ಸಾಲುಗಳು; i++ ) {
		ಗಾಗಿ (j = 0; j < MULTIBOOT_CONSOLE_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ; j++ ) {
			ಅಕ್ಷರ c = 0;
			ಆದರೆ(i < MULTIBOOT_CONSOLE_ಸಾಲುಗಳು-ಸಾಲುಗಳು)
				c = ನೇರ_get_char(i + ಸಾಲುಗಳು, j);
			ನೇರ_put_char(c, i, j);
		}
	}
	print_ಸಾಲು-= ಸಾಲುಗಳು;
}

ಶೂನ್ಯ ನೇರ_print_char(ಅಕ್ಷರ c)
{
	ಆಗ (print_ಸಾಲು >= MULTIBOOT_CONSOLE_ಸಾಲುಗಳು)
		ನೇರ_scroll_ಮೇಲೆ(1);

#ವ್ಯಾಖ್ಯಾನಿಸು TABWIDTH 8
	ಆದರೆ(c == '\t') {
		ಆದರೆ(print_ಲಂಬಸಾಲು >= MULTIBOOT_CONSOLE_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ - TABWIDTH) {
			c = '\n';
		} ಇಲ್ಲದಿದ್ದರೆ {
			ಮಾಡು {
				ನೇರ_put_char(' ', print_ಸಾಲು, print_ಲಂಬಸಾಲು++);
			} ಆಗ(print_ಲಂಬಸಾಲು % TABWIDTH);
			ಹಿಂತಿರುಗಿ;
		}
	}

	ಆದರೆ (c == '\n') {
		ಆಗ (print_ಲಂಬಸಾಲು < MULTIBOOT_CONSOLE_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ)
			ನೇರ_put_char(' ', print_ಸಾಲು, print_ಲಂಬಸಾಲು++);
		print_ಸಾಲು++;
		print_ಲಂಬಸಾಲು = 0;
		ಹಿಂತಿರುಗಿ;
	}

	ನೇರ_put_char(c, print_ಸಾಲು, print_ಲಂಬಸಾಲು++);

	ಆದರೆ (print_ಲಂಬಸಾಲು >= MULTIBOOT_CONSOLE_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ) {
		print_ಸಾಲು++;
		print_ಲಂಬಸಾಲು = 0;
	}

	ಆಗ (print_ಸಾಲು >= MULTIBOOT_CONSOLE_ಸಾಲುಗಳು)
		ನೇರ_scroll_ಮೇಲೆ(1);
}

ಶೂನ್ಯ ನೇರ_print(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *str)
{
	ಆಗ (*str) {
		ನೇರ_print_char(*str);
		str++;
	}
}

/* Standard and AT keyboard.  (PS/2 MCA implies AT throughout.) */
#ವ್ಯಾಖ್ಯಾನಿಸು KEYBD		0x60	/* I/O port for keyboard data */
#ವ್ಯಾಖ್ಯಾನಿಸು KB_ಸ್ಥಿತಿ	0x64	/* I/O port for status on AT */
#ವ್ಯಾಖ್ಯಾನಿಸು KB_ಹೊರಹೋಗು_ತುಂಬಿದ	0x01	/* status bit set when keypress char pending */
#ವ್ಯಾಖ್ಯಾನಿಸು KB_AUX_BYTE	0x20	/* Auxiliary Device Output Buffer Full */

ಪೂರ್ಣಾಂಕ ನೇರ_read_char(ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ch)
{
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ sb;

	sb = inb(KB_ಸ್ಥಿತಿ);

	ಆದರೆ (!(sb & KB_ಹೊರಹೋಗು_ತುಂಬಿದ)) {
		ಹಿಂತಿರುಗಿ 0;
	}

	inb(KEYBD);

	ಆದರೆ (!(sb & KB_AUX_BYTE))
		ಹಿಂತಿರುಗಿ 1;

	ಹಿಂತಿರುಗಿ 0;
}

