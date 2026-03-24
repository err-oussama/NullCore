#include "vga.h"

/*

Video Graphics Array (VGA) -- Text Mode Memory layout
	in text mode, VGA maps a region of memory directly to the screen.
	You can think of it as a 2D grid(typically 80 columns x 25 rows),
	where each cell on screen corresponds to exactly 2 Byte in memory
	- Byte 1: Attribute byte: Controls color.
		>The upper 4 bits set the backgroud color
		>The lower 4 bits set the foregroud color
	- Byte 2: Charachter byte: an ASCII(or code page 437) value for
			the charachter to display in the cell


*/
int						g_row = 0;
int						g_col = 0;
volatile unsigned short	*vga = (unsigned short *)0xB8000;
void	put_char(char c, int fg, int bg, int row, int col)
{
	vga[row * 80 + col] = ((bg << 4 | fg) << 8) | c;
}

void	print(char *str, int fg, int bg)
{
	while (*str)
	{
		if (*str == '\n' || g_col >= 80)
		{
			g_col = 0;
			g_row++;
			if (*str == '\n')
			{
				str++;
				continue ;
			}
		}
		put_char(*str++, fg, bg, g_row, g_col);
		g_col++;
	}
}

void	kmain(void)
{
	volatile unsigned short	*vga;

	vga = (unsigned short *)0xB8000;
	print("oussama\nerrami", VGA_LIGHT_BLUE, VGA_LIGHT_CYAN);
	while (1)
		;
}
