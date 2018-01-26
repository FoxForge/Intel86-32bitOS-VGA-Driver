// Implementation of string and memory functions

#include <string.h>

int strcmp(const char* str1, const char* str2)
{
	int i = 0;
	while (str1[i] == str2[i])
	{
		if (str1[i++] == 0)
		{
			return 0;
		}
	}

	return str1[i] - str2[i];
}

char* strcpy(char* destination, const char* source)
{
	do
	{
		*destination = *source++;
	} while (*destination++ != 0);

	return destination;
}

errno_t strcpy_s(char* destination, size_t numberOfElements, const char* source)
{
	int i = 0;
	while (i++ < numberOfElements)
	{
		*destination++ = *source++;
	}

	*destination = 0;
	return 0;
}

size_t strlen(const char* source)
{
	char* p = (char*)source;
	while (*p != 0)
	{
		p++;
	}

	return p - source;
}

void* memcpy(void* destination, const void* source, size_t count)
{
	char* dest = destination;
	char* src = (char*)source;
	int i = 0;
	while (i++ < count)
	{
		*dest++ = *src++;
	}

	return destination;
}

errno_t memcpy_s(void* destination, size_t destinationSize, const void* source, size_t count)
{
	char* dest = destination;
	char* src = (char*)source;
	int i = 0;
	while (i < count)
	{
		if (i <= destinationSize)
		{
			*dest++ = *src++;
			i++;
		}
	}

	return 0;
}

void* memset(void* destination, char val, size_t count)
{
	char* dest = destination;
	int i = 0;
	while (i++ < count)
	{
		*dest++ = val;
	}

	return destination;
}

unsigned short* memsetw(unsigned short* destination, unsigned short val, size_t count)
{
	int i = 0;
	while (i++ < count)
	{
		*destination++ = val;
	}

	return destination;
}
