#include <string.h>

void* memcpy(void* destination, const void* source, size_t num)
{
	unsigned char* dst = (unsigned char*)destination;
	const unsigned char* src = (const unsigned char*)source;
	for (size_t i = 0; i < num; i++)
		dst[i] = src[i];
	return destination;
}

void* memmove(void* destination, const void* source, size_t num)
{
	unsigned char* dst = (unsigned char*)destination;
	const unsigned char* src = (const unsigned char*)source;
	if (destination < source)
		for (size_t i = 0; i < num; i++)
			dst[i] = src[i];
	else
		for (size_t i = num; i != 0; i--)
			dst[i - 1] = src[i - 1];
	return destination;
}

char* strcpy(char* dest, const char* src)
{
	char* originalDest = dest;
	while ((*dest++ = *src++) != '\0');
	return originalDest;
}

char* strncpy(char* dest, const char* src, size_t num)
{
	size_t i;
	for (i = 0; i < num && src[i] != '\0'; ++i)
		dest[i] = src[i];
	for (; i < num; ++i)
		dest[i] = '\0';
	return dest;
}

char* strcat(char* dest, const char* src)
{
	size_t dest_length = strlen(dest);
	size_t i;
	for (i = 0; src[i] != '\0'; i++)
		dest[dest_length + i] = src[i];
	dest[dest_length + i] = '\0';
	return dest;
}

int memcmp(const void* ptr1, const void* ptr2, size_t num)
{
	const unsigned char* a = (const unsigned char*)ptr1;
	const unsigned char* b = (const unsigned char*)ptr2;
	for (size_t i = 0; i < num; i++)
		if (a[i] < b[i])
			return -1;
		else if (b[i] < a[i])
			return 1;
	return 0;
}

void* memset(void* ptr, int value, size_t num)
{
	unsigned char* buffer = (unsigned char*)ptr;
	for (size_t i = 0; i < num; i++)
		buffer[i] = (unsigned char)value;
	return ptr;
}

size_t strlen(const char* str)
{
	size_t length = 0;
	while (str[length])
		length++;
	return length;
}