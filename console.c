//	Basic Console Output.

#include <stdint.h>
#include <string.h>
#include <console.h>

// Video memory
uint16_t *_videoMemory = (uint16_t *)0xB8000;

#define CONSOLE_HEIGHT		25
#define CONSOLE_WIDTH		80
#define CHAR_SPACE			32
#define CHAR_LIMIT			127
#define ATT_SHIFT			8

// Current cursor position
uint8_t _cursorX = 0;
uint8_t _cursorY = 0;

// Current color
uint8_t	_colour = 15;

// Write byte to device through port mapped io
void  OutputByteToVideoController(unsigned short portid, unsigned char value) 
{
	asm volatile("movb	%1, %%al \n\t"
				 "movw	%0, %%dx \n\t"
				  "out	%%al, %%dx"
				 : : "m" (portid), "m" (value));
				 
}

// Update hardware cursor position
void UpdateCursorPosition(int x, int y) 
{
	if (x >= CONSOLE_WIDTH)
	{
		x = 0;
		++y;
	}

	if (y >= CONSOLE_HEIGHT)
	{
		--y;
		int i, j;

		i = 1;
		while (i < CONSOLE_HEIGHT)
		{
			j = 0;
			while (j <= CONSOLE_WIDTH)
			{
				*(_videoMemory + ((i - 1) * CONSOLE_WIDTH + j)) = *(_videoMemory + (i * CONSOLE_WIDTH + j));
				++j;
			}

			++i;
		}

		j = 0;
		while (j <= CONSOLE_WIDTH)
		{
			uint16_t attribute = _colour << ATT_SHIFT;
			*(_videoMemory + ((CONSOLE_HEIGHT - 1) * CONSOLE_WIDTH + j++)) = ' ' | attribute;
		}
	}

	_cursorX = x;
	_cursorY = y;
	uint16_t cursorLocation = y * CONSOLE_WIDTH + x;

	// Send location to VGA controller to set cursor
	// Send the high byte
	OutputByteToVideoController(0x3D4, 14);
	OutputByteToVideoController(0x3D5, cursorLocation >> ATT_SHIFT); 
	// Send the low byte
	OutputByteToVideoController(0x3D4, 15);
	OutputByteToVideoController(0x3D5, cursorLocation);      
}

// Displays a character
void ConsoleWriteCharacter(unsigned char c) 
{
	// Write only if printable character
	if ((c >= CHAR_SPACE) && (c <= CHAR_LIMIT))
	{
		uint16_t attribute = _colour << ATT_SHIFT;
		int16_t* location = _videoMemory + (_cursorY * CONSOLE_WIDTH + _cursorX);
		*location = c | attribute;
		UpdateCursorPosition(++_cursorX, _cursorY);
		return;
	}

	//Other character behaviour
	switch (c)
	{
	case 8:						//backspace
		//to be handled
		break;
	case 9:						//horizontal tab
		UpdateCursorPosition(_cursorX + 8, _cursorY);
		break;
	case 10:					//LF
		UpdateCursorPosition(_cursorX, ++_cursorY);
	case 13:					//CR
		UpdateCursorPosition(0, _cursorY);
		break;

	default: break;
	}
}

// Display specified string

void ConsoleWriteString(char* str) 
{
	if (!str)
	{
		return;
	}

	while (*str)
	{
		ConsoleWriteCharacter(*str++);
	}
}

// Write the unsigned integer to the screen, using the specified base, e.g. for
// the number to be displayed as a decimal number, you would specify base 10.  For 
// a number to be displayed as hexadecimal, you would specify base 16.

void ConsoleWriteInt(unsigned int i, unsigned int base) 
{
	unsigned char table[16] = { '0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F' };
	unsigned char result[64] = { NULL };
	int index = 0;

	while (i > 0)
	{
		result[index++] = table[i % base];
		i /= base;
	}

	while (--index >= 0)
	{
		if (result[index] == NULL)
			break;

		ConsoleWriteCharacter(result[index]);
	}
}

// Set the attribute to be used for all subsequent calls to ConsoleWriteXXXX routines

unsigned int ConsoleSetColour(const uint8_t c)
{
	uint8_t old_colour = _colour;
	_colour = c;
	return old_colour;
}

// Position the cursor at the specified X and Y position. All subsequent calls to 
// ConsoleWriteXXXX routines will start at this position.

void ConsoleGotoXY(unsigned int x, unsigned int y)
{
	if ((x < 0) || (x >= CONSOLE_WIDTH) || (y < 0) || (y >= CONSOLE_HEIGHT))
	{
		return;
	}

	UpdateCursorPosition(x, y);
}

// Get the current X and Y position of the cursor

void ConsoleGetXY(unsigned* x, unsigned* y)
{
	*x = _cursorX;
	*y = _cursorY;
}

// Get the width of the console window

int ConsoleGetWidth()
{
	return CONSOLE_WIDTH;
}

// Get the height of the console window

int ConsoleGetHeight()
{
	return CONSOLE_HEIGHT;
}

// Clear the screen.  Set the attribute byte for all characters on the 
// screen to be c. Set the cursor position to 0,0.

void ConsoleClearScreen(const uint8_t c)
{
	ConsoleSetColour(c);

	int i, j;
	i = 0;
	while (i++ < CONSOLE_HEIGHT)
	{
		j = 0;
		while (j++ < CONSOLE_WIDTH)
		{
			ConsoleWriteCharacter(CHAR_SPACE);
		}
	}

	UpdateCursorPosition(0, 0);
}


