/**
 * -----------------------------------------------------------------------------
 * Project: Fossil Logic
 *
 * This file is part of the Fossil Logic project, which aims to develop
 * high-performance, cross-platform applications and libraries. The code
 * contained herein is licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License. You may obtain
 * a copy of the License at:
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 * Author: Michael Gene Brockus (Dreamer)
 * Date: 04/05/2014
 *
 * Copyright (C) 2014-2025 Fossil Logic. All rights reserved.
 * -----------------------------------------------------------------------------
 */
#ifndef FOSSIL_SYS_MEMORY_H
#define FOSSIL_SYS_MEMORY_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif

// Define fossil_sys_memory_t as void*
typedef void *fossil_sys_memory_t;

/**
 * Allocate memory.
 *
 * @param size The size of the memory to allocate.
 * @return A pointer to the allocated memory.
 * @throws Error message and exits if allocation fails.
 */
fossil_sys_memory_t fossil_sys_memory_alloc(size_t size);

/**
 * Reallocate memory.
 *
 * @param ptr A pointer to the previously allocated memory.
 * @param size The new size of the memory to allocate.
 * @return A pointer to the reallocated memory.
 * @throws Error message and exits if reallocation fails or if the pointer is NULL.
 */
fossil_sys_memory_t fossil_sys_memory_realloc(fossil_sys_memory_t ptr, size_t size);

/**
 * Allocate and zero memory.
 *
 * @param num The number of elements to allocate.
 * @param size The size of each element.
 * @return A pointer to the allocated and zeroed memory.
 * @throws Error message and exits if allocation fails.
 */
fossil_sys_memory_t fossil_sys_memory_calloc(size_t num, size_t size);

/**
 * Initialize allocated memory to a known state.
 *
 * @param ptr A pointer to the allocated memory.
 * @param size The size of the memory.
 * @param value The value to initialize the memory with (e.g., zero).
 * @return A pointer to the initialized memory.
 */
fossil_sys_memory_t fossil_sys_memory_init(fossil_sys_memory_t ptr, size_t size, int32_t value);

/**
 * Free memory.
 *
 * @param ptr A pointer to the memory to free.
 * @throws Error message and exits if the pointer is NULL.
 */
void fossil_sys_memory_free(fossil_sys_memory_t ptr);

/**
 * Copy memory.
 *
 * @param dest A pointer to the destination memory.
 * @param src A pointer to the source memory.
 * @param size The size of the memory to copy.
 * @return A pointer to the destination memory.
 * @throws Error message and exits if copying fails or if either source or destination is NULL.
 */
fossil_sys_memory_t fossil_sys_memory_copy(fossil_sys_memory_t dest, const fossil_sys_memory_t src, size_t size);

/**
 * Set memory.
 *
 * @param ptr A pointer to the memory to set.
 * @param value The value to set.
 * @param size The size of the memory to set.
 * @return A pointer to the memory.
 * @throws Error message and exits if setting fails or if the pointer is NULL.
 */
fossil_sys_memory_t fossil_sys_memory_set(fossil_sys_memory_t ptr, int32_t value, size_t size);

/**
 * Duplicate memory.
 *
 * @param src A pointer to the memory to duplicate.
 * @param size The size of the memory to duplicate.
 * @return A pointer to the duplicated memory.
 * @throws Error message and exits if duplication fails or if the source is NULL.
 */
fossil_sys_memory_t fossil_sys_memory_dup(const fossil_sys_memory_t src, size_t size);

/**
 * Zero memory.
 *
 * @param ptr A pointer to the memory to zero.
 * @param size The size of the memory to zero.
 * @throws Error message and exits if the pointer is NULL.
 */
void fossil_sys_memory_zero(fossil_sys_memory_t ptr, size_t size);

/**
 * Compare memory.
 *
 * @param ptr1 A pointer to the first memory.
 * @param ptr2 A pointer to the second memory.
 * @param size The size of the memory to compare.
 * @return The result of the comparison.
 * @throws Error message and exits if the pointers are NULL or if the size is zero.
 */
int fossil_sys_memory_compare(const fossil_sys_memory_t ptr1, const fossil_sys_memory_t ptr2, size_t size);

/**
 * Move memory.
 *
 * @param dest A pointer to the destination memory.
 * @param src A pointer to the source memory.
 * @param size The size of the memory to move.
 * @return A pointer to the destination memory.
 * @throws Error message and exits if moving fails or if either source or destination is NULL.
 */
fossil_sys_memory_t fossil_sys_memory_move(fossil_sys_memory_t dest, const fossil_sys_memory_t src, size_t size);

/**
 * Resize memory.
 *
 * @param ptr A pointer to the memory to resize.
 * @param old_size The old size of the memory.
 * @param new_size The new size of the memory.
 * @return A pointer to the resized memory.
 * @throws Error message and exits if resizing fails or if the pointer is NULL.
 */
fossil_sys_memory_t fossil_sys_memory_resize(fossil_sys_memory_t ptr, size_t old_size, size_t new_size);

/**
 * Check if a memory pointer is valid.
 *
 * @param ptr A pointer to the memory.
 * @return 1 if the memory is valid, 0 otherwise.
 */
bool fossil_sys_memory_is_valid(const fossil_sys_memory_t ptr);

/**
 * @brief Fill memory with a repeating pattern.
 *
 * Useful for debugging or initializing buffers to known patterns.
 *
 * @param ptr A pointer to the memory to fill.
 * @param pattern Pointer to the pattern to repeat.
 * @param pattern_size Size of the pattern in bytes.
 * @param total_size Total number of bytes to fill.
 * @return A pointer to the memory.
 */
fossil_sys_memory_t fossil_sys_memory_fill(fossil_sys_memory_t ptr,
                                        const void *pattern,
                                        size_t pattern_size,
                                        size_t total_size);

/**
 * @brief Securely zero memory.
 *
 * Unlike fossil_sys_memory_zero, this function prevents
 * the compiler from optimizing out the zeroing operation.
 *
 * @param ptr A pointer to the memory to zero.
 * @param size The size of the memory to zero.
 */
void fossil_sys_memory_secure_zero(fossil_sys_memory_t ptr, size_t size);

/**
 * @brief Swap contents of two memory regions.
 *
 * Swaps memory between two buffers of the same size.
 *
 * @param a Pointer to the first buffer.
 * @param b Pointer to the second buffer.
 * @param size Number of bytes to swap.
 * @throws Error message and exits if pointers are NULL or sizes are zero.
 */
void fossil_sys_memory_swap(fossil_sys_memory_t a, fossil_sys_memory_t b, size_t size);

/**
 * @brief Search memory for a byte value.
 *
 * Scans memory for the first occurrence of the given byte.
 *
 * @param ptr Pointer to the memory to search.
 * @param value Byte value to search for.
 * @param size Size of the memory to search.
 * @return Pointer to the first occurrence of the value, or NULL if not found.
 */
void *fossil_sys_memory_find(const fossil_sys_memory_t ptr, uint8_t value, size_t size);

/**
 * @brief Duplicate a NULL-terminated string using memory API.
 *
 * Similar to strdup, but uses fossil_sys_memory_alloc under the hood.
 *
 * @param str The string to duplicate.
 * @return A pointer to the duplicated string.
 * @throws Error message and exits if allocation fails or str is NULL.
 */
char *fossil_sys_memory_strdup(const char *str);

/**
 * @brief Get memory usage statistics.
 *
 * Returns current allocation count and optionally total allocated bytes.
 * (Useful for leak detection in debug builds.)
 *
 * @param out_allocs Pointer to receive allocation count (can be NULL).
 * @param out_bytes Pointer to receive total allocated bytes (can be NULL).
 */
void fossil_sys_memory_stats(size_t *out_allocs, size_t *out_bytes);

#ifdef __cplusplus
}

/**
 * Fossil namespace.
 */
namespace fossil::sys
{

    /**
     * Memory management class.
     */
    class Memory
    {
    public:
        /**
         * Allocate memory.
         *
         * @param size The size of the memory to allocate.
         * @return A pointer to the allocated memory.
         */
        static fossil_sys_memory_t alloc(size_t size)
        {
            return fossil_sys_memory_alloc(size);
        }

        /**
         * Reallocate memory.
         *
         * @param ptr A pointer to the previously allocated memory.
         * @param size The new size of the memory to allocate.
         * @return A pointer to the reallocated memory.
         */
        static fossil_sys_memory_t realloc(fossil_sys_memory_t ptr, size_t size)
        {
            return fossil_sys_memory_realloc(ptr, size);
        }

        /**
         * Allocate and zero memory.
         *
         * @param num The number of elements to allocate.
         * @param size The size of each element.
         * @return A pointer to the allocated and zeroed memory.
         */
        static fossil_sys_memory_t calloc(size_t num, size_t size)
        {
            return fossil_sys_memory_calloc(num, size);
        }

        /**
         * Initialize allocated memory to a known state.
         *
         * @param ptr A pointer to the allocated memory.
         * @param size The size of the memory.
         * @param value The value to initialize the memory with (e.g., zero).
         * @return A pointer to the initialized memory.
         */
        static fossil_sys_memory_t init(fossil_sys_memory_t ptr, size_t size, int32_t value)
        {
            return fossil_sys_memory_init(ptr, size, value);
        }

        /**
         * Free memory.
         *
         * @param ptr A pointer to the memory to free.
         */
        static void free(fossil_sys_memory_t ptr)
        {
            fossil_sys_memory_free(ptr);
        }

        /**
         * Copy memory.
         *
         * @param dest A pointer to the destination memory.
         * @param src A pointer to the source memory.
         * @param size The size of the memory to copy.
         * @return A pointer to the destination memory.
         */
        static fossil_sys_memory_t copy(fossil_sys_memory_t dest, const fossil_sys_memory_t src, size_t size)
        {
            return fossil_sys_memory_copy(dest, src, size);
        }

        /**
         * Set memory.
         *
         * @param ptr A pointer to the memory to set.
         * @param value The value to set.
         * @param size The size of the memory to set.
         * @return A pointer to the memory.
         */
        static fossil_sys_memory_t set(fossil_sys_memory_t ptr, int32_t value, size_t size)
        {
            return fossil_sys_memory_set(ptr, value, size);
        }

        /**
         * Duplicate memory.
         *
         * @param src A pointer to the memory to duplicate.
         * @param size The size of the memory to duplicate.
         * @return A pointer to the duplicated memory.
         */
        static fossil_sys_memory_t dup(const fossil_sys_memory_t src, size_t size)
        {
            return fossil_sys_memory_dup(src, size);
        }

        /**
         * Zero memory.
         *
         * @param ptr A pointer to the memory to zero.
         * @param size The size of the memory to zero.
         */
        static void zero(fossil_sys_memory_t ptr, size_t size)
        {
            fossil_sys_memory_zero(ptr, size);
        }

        /**
         * Compare memory.
         *
         * @param ptr1 A pointer to the first memory.
         * @param ptr2 A pointer to the second memory.
         * @param size The size of the memory to compare.
         * @return The result of the comparison.
         */
        static int compare(const fossil_sys_memory_t ptr1, const fossil_sys_memory_t ptr2, size_t size)
        {
            return fossil_sys_memory_compare(ptr1, ptr2, size);
        }

        /**
         * Move memory.
         *
         * @param dest A pointer to the destination memory.
         * @param src A pointer to the source memory.
         * @param size The size of the memory to move.
         * @return A pointer to the destination memory.
         */
        static fossil_sys_memory_t move(fossil_sys_memory_t dest, const fossil_sys_memory_t src, size_t size)
        {
            return fossil_sys_memory_move(dest, src, size);
        }

        /**
         * Resize memory.
         *
         * @param ptr A pointer to the memory to resize.
         * @param old_size The old size of the memory.
         * @param new_size The new size of the memory.
         * @return A pointer to the resized memory.
         */
        static fossil_sys_memory_t resize(fossil_sys_memory_t ptr, size_t old_size, size_t new_size)
        {
            return fossil_sys_memory_resize(ptr, old_size, new_size);
        }

        /**
         * Check if a memory pointer is valid.
         *
         * @param ptr A pointer to the memory.
         * @return true if the memory is valid, false otherwise.
         */
        static bool is_valid(const fossil_sys_memory_t ptr)
        {
            return fossil_sys_memory_is_valid(ptr);
        }

        /**
         * Fill memory with a repeating pattern.
         *
         * @param ptr A pointer to the memory to fill.
         * @param pattern Pointer to the pattern to repeat.
         * @param pattern_size Size of the pattern in bytes.
         * @param total_size Total number of bytes to fill.
         * @return A pointer to the memory.
         */
        static fossil_sys_memory_t fill(fossil_sys_memory_t ptr, const void *pattern, size_t pattern_size, size_t total_size)
        {
            return fossil_sys_memory_fill(ptr, pattern, pattern_size, total_size);
        }

        /**
         * Securely zero memory.
         *
         * @param ptr A pointer to the memory to zero.
         * @param size The size of the memory to zero.
         */
        static void secure_zero(fossil_sys_memory_t ptr, size_t size)
        {
            fossil_sys_memory_secure_zero(ptr, size);
        }

        /**
         * Swap contents of two memory regions.
         *
         * @param a Pointer to the first buffer.
         * @param b Pointer to the second buffer.
         * @param size Number of bytes to swap.
         */
        static void swap(fossil_sys_memory_t a, fossil_sys_memory_t b, size_t size)
        {
            fossil_sys_memory_swap(a, b, size);
        }

        /**
         * Search memory for a byte value.
         *
         * @param ptr Pointer to the memory to search.
         * @param value Byte value to search for.
         * @param size Size of the memory to search.
         * @return Pointer to the first occurrence of the value, or NULL if not found.
         */
        static void *find(const fossil_sys_memory_t ptr, uint8_t value, size_t size)
        {
            return fossil_sys_memory_find(ptr, value, size);
        }

        /**
         * Duplicate a NULL-terminated string using memory API.
         *
         * @param str The string to duplicate.
         * @return A pointer to the duplicated string.
         */
        static char *strdup(const char *str)
        {
            return fossil_sys_memory_strdup(str);
        }

        /**
         * Get memory usage statistics.
         *
         * @param out_allocs Pointer to receive allocation count (can be NULL).
         * @param out_bytes Pointer to receive total allocated bytes (can be NULL).
         */
        static void stats(size_t *out_allocs, size_t *out_bytes)
        {
            fossil_sys_memory_stats(out_allocs, out_bytes);
        }
    };

}

#endif

#endif /* FOSSIL_SYS_FRAMEWORK_H */
