//	Basic Console Output.

#include <stdint.h>
#include <string.h>
#include <console.h>
#include "include\vgamodes.h"
#include "include\vgadriver.h"

#define DEFAULT_THICKNESS 1

// Video memory
uint8_t* _vgaMemory = (uint8_t *)0xA0000;

unsigned char _scanMemory[1024 * 1024];
size_t _scanMemIndex = 0;

// Current pixel position
uint8_t _pixelX = 0;
uint8_t _pixelY = 0;

// Current color
uint8_t	_vgaColour = 15;

//void* VGA_ScanMalloc(size_t size)
//{
//	void* mem;
//	if (sizeof _scanMemory - _scanMemIndex < size)
//	{
//		return NULL;
//	}
//
//	mem = &_scanMemory[_scanMemIndex];
//	_scanMemIndex += size;
//	return mem;
//}
//
//void VGA_ResetScanMemory()
//{
//	_scanMemIndex = 0;
//}

void VGA_SetPixel(uint16_t x, uint16_t y, const uint8_t c)
{
	uint16_t width = VGA_GetScreenWidth();
	if (x < 0 || x >= width || y < 0 || y >= VGA_GetScreenHeight())
	{
		return;
	}

	*(_vgaMemory + (y * width + x)) = c;
}

uint8_t VGA_GetPixelColour(uint16_t x, uint16_t y)
{
	uint16_t width = VGA_GetScreenWidth();
	if (x < 0 || x >= width || y < 0 || y >= VGA_GetScreenHeight())
	{
		return NULL;
	}

	return *(_vgaMemory + (y * width + x));
}

void VGA_DrawRectangle()
{
	PixelInfo vertices[4] =
	{
		{.x = 10,	.y = 10,.colour = 2 },
		{.x = 100,	.y = 10,.colour = 2 },
		{.x = 100,	.y = 100,.colour = 2 },
		{.x = 10,	.y = 100,.colour = 2 },
	};

	int i = 0;
	while (i < 4)
	{
		PixelInfo p1 = vertices[i];
		PixelInfo p2 = vertices[(i + 1) % 4];
		VGA_DrawLine(p1, p2, 1);
		i++;
	}

	VGA_PixelFloodFill(11, 11, 2);
}

void VGA_DrawLine(PixelInfo start, PixelInfo end, unsigned int thickness)
{
	// Store starting x and y in local scope for maniupulation and re-use
	uint16_t x1 = start.x;
	uint16_t y1 = start.y;

	// Get differentials
	int dx = end.x - x1;
	int dy = end.y - y1;
	uint16_t dxAbs = (dx >= 0) ? dx : (dx * -1);
	uint16_t dyAbs = (dy >= 0) ? dy : (dy * -1);

	// Set the increments for the different octants
	int dx1 = (dx < 0) ? -1 : (dx > 0) ? 1 : 0;
	int dy1 = (dy < 0) ? -1 : (dy > 0) ? 1 : 0;
	int dx2 = (dxAbs <= dyAbs) ? 0 : dx1;
	int dy2 = (dxAbs <= dyAbs) ? dy1 : 0;

	// Use height and width based decisions from the dy and dx
	uint16_t dLarge = (dxAbs <= dyAbs) ? dyAbs : dxAbs; // Longest differential
	uint16_t dSmall = (dxAbs <= dyAbs) ? dxAbs : dyAbs; // Smallest differential 

	// Check whether n == half of l (avoid round off by bit shifting)
	uint16_t epsilon = dLarge >> 1;

	// For every point on this line
	uint16_t i = 0;
	while (i <= dLarge) 
	{
		uint16_t x = x1;
		uint16_t y = y1;

		// Check wether a larger than default thickness is requested
		if (thickness > DEFAULT_THICKNESS)
		{
			// Use the default rounding to int
			uint16_t offset = thickness / 2;

			// Set the start and end indicies for iteration resepctive of swapping the points due to large angle
			int tStart = (dxAbs <= dyAbs) ? (x - offset) : (y - offset);
			int tEnd = (dxAbs <= dyAbs) ? (x + offset) : (y + offset);

			uint16_t j = 1;
			while (j++ < offset)
			{
				int k = tStart;
				while (k < tEnd)
				{
					// Set the pixel resepctive of swapping the points due to large angle
					if (dxAbs <= dyAbs)
					{
						VGA_SetPixel(k++, y, 2);
					}
					else
					{
						VGA_SetPixel(x, k++, 2);
					}
				}
			}
		}
		else
		{
			// Otherwise simply set the pixel here
			VGA_SetPixel(x, y, 2);
		}

		// Increase epsilon by the checked dy
		epsilon += dSmall;
		x1 += (epsilon >= dLarge) ? dx1 : dx2; // Increase X by calculated dx for the current iteration
		y1 += (epsilon >= dLarge) ? dy1 : dy2; // Increase y by calculated dy for the current iteration
		if (epsilon >= dLarge)
		{
			// When epsilon becomes greater than width then we subtract the width but continue to increase the point
			epsilon -= dLarge;
		}

		i++; // Progress drawing the line
	}
}

void VGA_PixelFloodFill(uint16_t x, uint16_t y, const uint8_t c)
{
	if (x < 0 || x >= VGA_GetScreenWidth() || y < 0 || y >= VGA_GetScreenHeight())
	{
		return;
	}

	if (VGA_GetPixelColour(x, y) == c)
	{
		return;
	}

	VGA_SetPixel(x, y, c);
	VGA_PixelFloodFill(x + 1, y, c);
	VGA_PixelFloodFill(x - 1, y, c);
	VGA_PixelFloodFill(x, y + 1, c);
	VGA_PixelFloodFill(x, y - 1, c);
}

//void VGA_ScanlineFillPolygon2D(const PixelInfo* vertices, const unsigned int count)
//{
//	uint16_t screenHeight = VGA_GetScreenHeight();
//	ScanlineClip clip = { .lowest = screenHeight, .highest = 0 }; // Set up the clip with opposite maximum extents
//
//	VGA_ResetScanMemory();
//	//ScanlineItem (*scanLUT)[8] = (ScanlineItem(*)[8])VGA_ScanMalloc((sizeof(ScanlineItem) * 8) * 1024);
//	//ScanlineItem* scanLUT = (ScanlineItem*)VGA_ScanMalloc(sizeof(ScanlineItem) * 1024); // ROUGHLY ABOUT 8 POINTS ALONG AN X - shouldnt be much more complicated polygons than that
//	//unsigned int scanNumber = 0;
//
//	for (unsigned int i = 0; i < count; i++)
//	{
//		unsigned int i2 = (i + 1) % count;
//		PixelInfo p1 = vertices[i];
//		PixelInfo p2 = vertices[i2];
//
//		if (p1.y < clip.lowest)
//		{
//			clip.lowest = (p1.y < 0) ? 0 : p1.y;
//		}
//
//		if (p1.y > clip.highest)
//		{
//			clip.highest = (p1.y > (screenHeight - 1)) ? (screenHeight - 1) : p1.y;
//		}
//
//		if (p2.y < p1.y)
//		{
//			PixelInfo temp = p1;
//			p1 = p2;
//			p2 = temp;
//		}
//
//		p1.y = (p1.y < 0) ? 0 : ((p1.y > screenHeight - 1) ? (screenHeight - 1) : p1.y);
//		p2.y = (p2.y < 0) ? 0 : ((p2.y > screenHeight - 1) ? (screenHeight - 1) : p2.y);
//
//		unsigned int delta = p2.y - p1.y;
//
//		for (unsigned int y = p1.y; y < p2.y; y++)
//		{
//			unsigned int x = p1.x + (y - p1.y) * ((p1.x - p2.x) / (p1.y - p2.y));
//			ScanlineItem item = { .x = x, .colour = p1.colour };
//			// add to scanline buffer here
//		}
//	}
//
//}

void VGA_ClearScreen()
{
	// Get screen size
	uint16_t size = VGA_GetScreenWidth() * VGA_GetScreenHeight();

	// Take advantage of linear address space to create one loop to set all pixels on screen (no need for nested loops)
	int i = 0;
	while (i < size)
	{
		*(_vgaMemory + i++) = 0;
	}
}


