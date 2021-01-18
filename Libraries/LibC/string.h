#pragma once

#include <LibC/stddef.h>
#include <LibC/sys/cdefs.h>
#include <LibC/sys/types.h>

__BEGIN_DECLS

/* Copying */
void* memcpy(void* destination, const void* source, size_t num);
void* memmove(void* destination, const void* source, size_t num);
char* strcpy(char* dest, const char* src);
char* strncpy(char* dest, const char* src, size_t num);

/* Concatenation */
char* strcat(char* dest, const char* src);

/* Comparison */
int memcmp(const void* ptr1, const void* ptr2, size_t num);

/* Other */
void* memset(void* ptr, int value, size_t num);
size_t strlen(const char* str);

__END_DECLS