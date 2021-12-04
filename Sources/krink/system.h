#pragma once

#include <stddef.h>

/// <summary>
/// Needs to be called before using krink.
/// </summary>
/// <param name="memblk">A block of allocated memory for krink to use</param>
/// <param name="size">The size of `memblk`</param>
void krink_init(void* memblk, size_t size);
