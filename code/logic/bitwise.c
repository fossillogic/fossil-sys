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
#include "fossil/sys/bitwise.h"
#include <string.h>

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

uint64_t fossil_sys_bitwise_parse(const char *input, const fossil_sys_bitwise_table_t *table)
{
    uint64_t result = 0;
    size_t input_len = strlen(input);
    char *input_copy = (char *)malloc(input_len + 1);
    if (!input_copy)
    {
        return 0; // Memory allocation failed
    }
    strcpy(input_copy, input);
    char *token = strtok(input_copy, "|");
    while (token != NULL)
    {
        for (size_t i = 0; i < table->count; ++i)
        {
            if (strcmp(token, table->entries[i].name) == 0)
            {
                result |= table->entries[i].bit;
                break;
            }
        }
        token = strtok(NULL, "|");
    }
    free(input_copy);
    return result;
}

int fossil_sys_bitwise_format(uint64_t bits, const fossil_sys_bitwise_table_t *table, char *out, size_t out_size)
{
    size_t offset = 0;
    for (size_t i = 0; i < table->count; ++i)
    {
        if (bits & table->entries[i].bit)
        {
            size_t len = strlen(table->entries[i].name);
            if (offset + len + 1 < out_size)
            {
                if (offset > 0)
                {
                    out[offset++] = '|';
                }
                strcpy(out + offset, table->entries[i].name);
                offset += len;
            }
            else
            {
                return -1; // Buffer too small
            }
        }
    }
    if (offset < out_size)
    {
        out[offset] = '\0'; // Null-terminate the string
    }
    else
    {
        return -1; // Buffer too small
    }
    return 0;
}

int fossil_sys_bitwise_lookup(const char *name, const fossil_sys_bitwise_table_t *table, uint64_t *out_bit)
{
    for (size_t i = 0; i < table->count; ++i)
    {
        if (strcmp(name, table->entries[i].name) == 0)
        {
            *out_bit = table->entries[i].bit;
            return 0; // Found
        }
    }
    return -1; // Not found
}

uint64_t fossil_sys_bitwise_all(const fossil_sys_bitwise_table_t *table)
{
    if (!table || !table->entries)
        return 0;

    uint64_t mask = 0;
    for (size_t i = 0; i < table->count; i++)
    {
        mask |= table->entries[i].bit;
    }
    return mask;
}

int fossil_sys_bitwise_validate(uint64_t bits, const fossil_sys_bitwise_table_t *table)
{
    if (!table)
        return -1; // invalid table pointer

    uint64_t all = fossil_sys_bitwise_all(table);
    return (bits & ~all) ? -1 : 0; // -1 if unknown bits are set
}

const char *fossil_sys_bitwise_name(uint64_t bit, const fossil_sys_bitwise_table_t *table)
{
    if (!table || !table->entries)
        return NULL;

    for (size_t i = 0; i < table->count; i++)
    {
        if (table->entries[i].bit == bit)
        {
            return table->entries[i].name;
        }
    }
    return NULL;
}

size_t fossil_sys_bitwise_count(uint64_t bits)
{
#if defined(__GNUC__) || defined(__clang__)
    return (size_t)__builtin_popcountll(bits);
#else
    // Portable fallback (Brian Kernighan’s bit-counting algorithm)
    size_t count = 0;
    while (bits)
    {
        bits &= (bits - 1); // clear the lowest set bit
        count++;
    }
    return count;
#endif
}
