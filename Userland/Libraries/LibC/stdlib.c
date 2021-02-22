#include <stdlib.h>

#include <stdio.h>
#include <string.h>
//#include <AK/Panic.h>
//#include <Kernel/Memory/MemoryDefines.h>
//#include <Kernel/Memory/PhysicalMemoryManager.h>

struct HeapHeader
{
	uint64_t pages;
	uint64_t size;
};

void abort()
{
	// TODO: Abnormally terminate the process as if by SIGABRT.
	//panic("abort()");

	while (1);
}

void* malloc(size_t size)
{
	(void)size;
	/*
	size_t page_count = (size + (PAGE_SIZE - 1)) / PAGE_SIZE;

	void* ptr = physical_memory_manager_callocate_pages((uintptr_t)(page_count + 1));

	if (!ptr)
		return NULL;

	ptr = (void*)((uint64_t)ptr + KERNEL_BASE_ADDRESS);

	struct HeapHeader* metadata = (struct HeapHeader*)ptr;
	ptr = (void*)((uint64_t)ptr + PAGE_SIZE);

	metadata->pages = page_count;
	metadata->size = size;

	return ptr;
	*/

	return NULL;
}

void* calloc(size_t num, size_t size)
{
	void* ptr = malloc(num * size);
	memset(ptr, 0, num * size);
	return ptr;
}

void* realloc(void* ptr, size_t size)
{
	(void)ptr;
	(void)size;
	/*
	if (!ptr)
		return malloc(size);

	struct HeapHeader* metadata = (struct HeapHeader*)((uint64_t)ptr - PAGE_SIZE);

	size_t current_size = (metadata->size + (PAGE_SIZE - 1)) / PAGE_SIZE;
	size_t new_size = (size + (PAGE_SIZE - 1)) / PAGE_SIZE;
	if (current_size == new_size)
	{
		metadata->size = new_size;
		return ptr;
	}

	void* new_ptr = malloc(new_size);
	if (new_ptr == NULL)
		return NULL;

	if (metadata->size > new_size)
		memcpy(new_ptr, ptr, new_size);
	else
		memcpy(new_ptr, ptr, metadata->size);

	free(ptr);

	return new_ptr;
	*/
	return NULL;
}

void free(void* ptr)
{
	(void)ptr;
	//struct HeapHeader* metadata = (struct HeapHeader*)((uint64_t)ptr - PAGE_SIZE);
	//physical_memory_manager_free_pages((void*)((uint64_t)metadata - KERNEL_BASE_ADDRESS), metadata->pages + 1);
}