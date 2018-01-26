#ifndef _VGADRIVER_H
#define _VGADRIVER_H
#include <stdint.h>

typedef struct _PixelInfo
{
	uint16_t x;
	uint16_t y;
	uint8_t colour;
} PixelInfo;

typedef struct _VGAColour
{
	uint8_t colourIndex;
	unsigned int r;
	unsigned int g;
	unsigned int b;
} VGAColour;

typedef struct _ScanlineItem //10bytes
{
	uint8_t colour; // 8btyes
	unsigned short x; // 2bytes
} ScanlineItem;

typedef struct _ScanlineClip
{
	unsigned int lowest;
	unsigned int highest;
} ScanlineClip;

void VGA_SetPixel(uint16_t x, uint16_t y, const uint8_t c);

uint8_t VGA_GetPixelColour(uint16_t x, uint16_t y);

void VGA_DrawLine(PixelInfo start, PixelInfo end, unsigned int thickness);

void VGA_ClearScreen();

void VGA_DrawRectangle();

//void VGA_ScanlineFillPolygon2D(const PixelInfo* vertices, const unsigned int count);

void VGA_PixelFloodFill(uint16_t x, uint16_t y, const uint8_t c);

#endif