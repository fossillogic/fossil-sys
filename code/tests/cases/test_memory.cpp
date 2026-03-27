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
#include <fossil/pizza/framework.h>

#include "fossil/sys/framework.h"

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Utilities
// * * * * * * * * * * * * * * * * * * * * * * * *
// Setup steps for things like test fixtures and
// mock objects are set here.
// * * * * * * * * * * * * * * * * * * * * * * * *

// Define the test suite and add test cases
FOSSIL_SUITE(cpp_memory_suite);

// Setup function for the test suite
FOSSIL_SETUP(cpp_memory_suite)
{
    // Setup code here
}

// Teardown function for the test suite
FOSSIL_TEARDOWN(cpp_memory_suite)
{
    // Teardown code here
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Cases
// * * * * * * * * * * * * * * * * * * * * * * * *
// The test cases below are provided as samples, inspired
// by the Meson build system's approach of using test cases
// as samples for library usage.
// * * * * * * * * * * * * * * * * * * * * * * * *

FOSSIL_TEST(cpp_test_memory_alloc)
{
    size_t size = 10;
    fossil_sys_memory_t ptr = fossil_sys_memory_alloc(size);
    ASSUME_NOT_CNULL(ptr);
    fossil_sys_memory_free(ptr); // Cleanup
}

FOSSIL_TEST(cpp_test_memory_realloc)
{
    size_t size = 10;
    fossil_sys_memory_t ptr = fossil_sys_memory_alloc(size);
    ASSUME_NOT_CNULL(ptr);

    size *= 2;
    ptr = fossil_sys_memory_realloc(ptr, size);
    ASSUME_NOT_CNULL(ptr);

    fossil_sys_memory_free(ptr); // Cleanup
}

FOSSIL_TEST(cpp_test_memory_dup)
{
    size_t size = 10;
    fossil_sys_memory_t src = fossil_sys_memory_alloc(size);
    ASSUME_NOT_CNULL(src);

    fossil_sys_memory_t dest = fossil_sys_memory_dup(src, size);
    ASSUME_NOT_CNULL(dest);

    ASSUME_ITS_TRUE(memcmp(src, dest, size) == 0); // Ensure that both blocks are identical
    fossil_sys_memory_free(src);
    fossil_sys_memory_free(dest); // Cleanup
}

FOSSIL_TEST(cpp_test_memory_zero)
{
    size_t size = 10;
    fossil_sys_memory_t ptr = fossil_sys_memory_alloc(size);
    ASSUME_NOT_CNULL(ptr);

    fossil_sys_memory_zero(ptr, size);
    for (size_t i = 0; i < size; ++i)
    {
        ASSUME_ITS_TRUE(((unsigned char *)ptr)[i] == 0); // Ensure all bytes are zero
    }

    fossil_sys_memory_free(ptr); // Cleanup
}

FOSSIL_TEST(cpp_test_memory_compare)
{
    size_t size = 10;
    fossil_sys_memory_t ptr1 = fossil_sys_memory_alloc(size);
    fossil_sys_memory_t ptr2 = fossil_sys_memory_alloc(size);
    ASSUME_NOT_CNULL(ptr1);
    ASSUME_NOT_CNULL(ptr2);

    fossil_sys_memory_set(ptr1, 0xAA, size);                           // Set all bytes to 0xAA
    fossil_sys_memory_set(ptr2, 0xAA, size);                           // Set all bytes to 0xAA
    ASSUME_ITS_TRUE(fossil_sys_memory_compare(ptr1, ptr2, size) == 0); // Should be equal

    fossil_sys_memory_set(ptr2, 0xBB, size);                           // Change ptr2
    ASSUME_ITS_TRUE(fossil_sys_memory_compare(ptr1, ptr2, size) != 0); // Should not be equal

    fossil_sys_memory_free(ptr1);
    fossil_sys_memory_free(ptr2); // Cleanup
}

FOSSIL_TEST(cpp_test_memory_move)
{
    size_t size = 10;
    fossil_sys_memory_t src = fossil_sys_memory_alloc(size);
    ASSUME_NOT_CNULL(src);

    fossil_sys_memory_t dest = fossil_sys_memory_alloc(size);
    ASSUME_NOT_CNULL(dest);

    fossil_sys_memory_set(src, 0xAA, size);
    fossil_sys_memory_move(dest, src, size);
    ASSUME_ITS_TRUE(memcmp(dest, src, size) == 0); // Ensure memory is moved correctly

    fossil_sys_memory_free(src);
    fossil_sys_memory_free(dest); // Cleanup
}

FOSSIL_TEST(cpp_test_memory_resize)
{
    size_t size = 10;
    fossil_sys_memory_t ptr = fossil_sys_memory_alloc(size);
    ASSUME_NOT_CNULL(ptr);

    ptr = fossil_sys_memory_resize(ptr, size, size * 2);
    ASSUME_NOT_CNULL(ptr); // Ensure resizing works

    fossil_sys_memory_free(ptr); // Cleanup
}

FOSSIL_TEST(cpp_test_memory_is_valid)
{
    fossil_sys_memory_t ptr = fossil_sys_memory_alloc(10);
    ASSUME_ITS_TRUE(fossil_sys_memory_is_valid(ptr));      // Should be valid
    ASSUME_ITS_TRUE(!fossil_sys_memory_is_valid(nullptr)); // nullptr should not be valid

    fossil_sys_memory_free(ptr); // Cleanup
}

FOSSIL_TEST(cpp_test_memory_calloc)
{
    size_t num = 10;
    size_t size = sizeof(int);
    fossil_sys_memory_t ptr = fossil_sys_memory_calloc(num, size);
    ASSUME_NOT_CNULL(ptr);

    for (size_t i = 0; i < num * size; ++i)
    {
        ASSUME_ITS_TRUE(((unsigned char *)ptr)[i] == 0); // Ensure all bytes are zero
    }

    fossil_sys_memory_free(ptr); // Cleanup
}

FOSSIL_TEST(cpp_test_memory_init)
{
    size_t size = 10;
    int32_t value = 0xAA;
    fossil_sys_memory_t ptr = fossil_sys_memory_alloc(size);
    ASSUME_NOT_CNULL(ptr);

    fossil_sys_memory_init(ptr, size, value);
    for (size_t i = 0; i < size; ++i)
    {
        ASSUME_ITS_TRUE(((unsigned char *)ptr)[i] == (unsigned char)value); // Ensure all bytes are set to value
    }

    fossil_sys_memory_free(ptr); // Cleanup
}

FOSSIL_TEST(cpp_test_memory_class_alloc)
{
    size_t size = 10;
    fossil_sys_memory_t ptr = fossil::sys::Memory::alloc(size);
    ASSUME_NOT_CNULL(ptr);
    fossil::sys::Memory::free(ptr); // Cleanup
}

FOSSIL_TEST(cpp_test_memory_class_realloc)
{
    size_t size = 10;
    fossil_sys_memory_t ptr = fossil::sys::Memory::alloc(size);
    ASSUME_NOT_CNULL(ptr);

    size *= 2;
    ptr = fossil::sys::Memory::realloc(ptr, size);
    ASSUME_NOT_CNULL(ptr);

    fossil::sys::Memory::free(ptr); // Cleanup
}

FOSSIL_TEST(cpp_test_memory_class_calloc)
{
    size_t num = 10;
    size_t size = sizeof(int);
    fossil_sys_memory_t ptr = fossil::sys::Memory::calloc(num, size);
    ASSUME_NOT_CNULL(ptr);

    for (size_t i = 0; i < num * size; ++i)
    {
        ASSUME_ITS_TRUE(((unsigned char *)ptr)[i] == 0); // Ensure all bytes are zero
    }

    fossil::sys::Memory::free(ptr); // Cleanup
}

FOSSIL_TEST(cpp_test_memory_class_init)
{
    size_t size = 10;
    int32_t value = 0xAA;
    fossil_sys_memory_t ptr = fossil::sys::Memory::alloc(size);
    ASSUME_NOT_CNULL(ptr);

    ptr = fossil::sys::Memory::init(ptr, size, value);
    for (size_t i = 0; i < size; ++i)
    {
        ASSUME_ITS_TRUE(((unsigned char *)ptr)[i] == (unsigned char)value); // Ensure all bytes are set to value
    }

    fossil::sys::Memory::free(ptr); // Cleanup
}

FOSSIL_TEST(cpp_test_memory_class_copy)
{
    size_t size = 10;
    fossil_sys_memory_t src = fossil::sys::Memory::alloc(size);
    ASSUME_NOT_CNULL(src);

    fossil_sys_memory_t dest = fossil::sys::Memory::alloc(size);
    ASSUME_NOT_CNULL(dest);

    fossil::sys::Memory::set(src, 0xAA, size);
    dest = fossil::sys::Memory::copy(dest, src, size);
    ASSUME_ITS_TRUE(memcmp(dest, src, size) == 0); // Ensure memory is copied correctly

    fossil::sys::Memory::free(src);
    fossil::sys::Memory::free(dest); // Cleanup
}

FOSSIL_TEST(cpp_test_memory_class_set)
{
    size_t size = 10;
    int32_t value = 0xAA;
    fossil_sys_memory_t ptr = fossil::sys::Memory::alloc(size);
    ASSUME_NOT_CNULL(ptr);

    ptr = fossil::sys::Memory::set(ptr, value, size);
    for (size_t i = 0; i < size; ++i)
    {
        ASSUME_ITS_TRUE(((unsigned char *)ptr)[i] == (unsigned char)value); // Ensure all bytes are set to value
    }

    fossil::sys::Memory::free(ptr); // Cleanup
}

FOSSIL_TEST(cpp_test_memory_class_dup)
{
    size_t size = 10;
    fossil_sys_memory_t src = fossil::sys::Memory::alloc(size);
    ASSUME_NOT_CNULL(src);

    fossil_sys_memory_t dest = fossil::sys::Memory::dup(src, size);
    ASSUME_NOT_CNULL(dest);

    ASSUME_ITS_TRUE(memcmp(src, dest, size) == 0); // Ensure that both blocks are identical
    fossil::sys::Memory::free(src);
    fossil::sys::Memory::free(dest); // Cleanup
}

FOSSIL_TEST(cpp_test_memory_class_zero)
{
    size_t size = 10;
    fossil_sys_memory_t ptr = fossil::sys::Memory::alloc(size);
    ASSUME_NOT_CNULL(ptr);

    fossil::sys::Memory::zero(ptr, size);
    for (size_t i = 0; i < size; ++i)
    {
        ASSUME_ITS_TRUE(((unsigned char *)ptr)[i] == 0); // Ensure all bytes are zero
    }

    fossil::sys::Memory::free(ptr); // Cleanup
}

FOSSIL_TEST(cpp_test_memory_class_compare)
{
    size_t size = 10;
    fossil_sys_memory_t ptr1 = fossil::sys::Memory::alloc(size);
    fossil_sys_memory_t ptr2 = fossil::sys::Memory::alloc(size);
    ASSUME_NOT_CNULL(ptr1);
    ASSUME_NOT_CNULL(ptr2);

    fossil::sys::Memory::set(ptr1, 0xAA, size);                           // Set all bytes to 0xAA
    fossil::sys::Memory::set(ptr2, 0xAA, size);                           // Set all bytes to 0xAA
    ASSUME_ITS_TRUE(fossil::sys::Memory::compare(ptr1, ptr2, size) == 0); // Should be equal

    fossil::sys::Memory::set(ptr2, 0xBB, size);                           // Change ptr2
    ASSUME_ITS_TRUE(fossil::sys::Memory::compare(ptr1, ptr2, size) != 0); // Should not be equal

    fossil::sys::Memory::free(ptr1);
    fossil::sys::Memory::free(ptr2); // Cleanup
}

FOSSIL_TEST(cpp_test_memory_class_move)
{
    size_t size = 10;
    fossil_sys_memory_t src = fossil::sys::Memory::alloc(size);
    ASSUME_NOT_CNULL(src);

    fossil_sys_memory_t dest = fossil::sys::Memory::alloc(size);
    ASSUME_NOT_CNULL(dest);

    fossil::sys::Memory::set(src, 0xAA, size);
    dest = fossil::sys::Memory::move(dest, src, size);
    ASSUME_ITS_TRUE(memcmp(dest, src, size) == 0); // Ensure memory is moved correctly

    fossil::sys::Memory::free(src);
    fossil::sys::Memory::free(dest); // Cleanup
}

FOSSIL_TEST(cpp_test_memory_class_resize)
{
    size_t size = 10;
    fossil_sys_memory_t ptr = fossil::sys::Memory::alloc(size);
    ASSUME_NOT_CNULL(ptr);

    ptr = fossil::sys::Memory::resize(ptr, size, size * 2);
    ASSUME_NOT_CNULL(ptr); // Ensure resizing works

    fossil::sys::Memory::free(ptr); // Cleanup
}

FOSSIL_TEST(cpp_test_memory_class_is_valid)
{
    fossil_sys_memory_t ptr = fossil::sys::Memory::alloc(10);
    ASSUME_ITS_TRUE(fossil::sys::Memory::is_valid(ptr));      // Should be valid
    ASSUME_ITS_TRUE(!fossil::sys::Memory::is_valid(nullptr)); // nullptr should not be valid

    fossil::sys::Memory::free(ptr); // Cleanup
}

FOSSIL_TEST(cpp_test_memory_class_fill)
{
    size_t size = 16;
    uint8_t pattern[2] = {0xAB, 0xCD};
    fossil_sys_memory_t ptr = fossil::sys::Memory::alloc(size);
    ASSUME_NOT_CNULL(ptr);

    ptr = fossil::sys::Memory::fill(ptr, pattern, sizeof(pattern), size);
    for (size_t i = 0; i < size; ++i)
    {
        ASSUME_ITS_TRUE(((uint8_t *)ptr)[i] == pattern[i % 2]);
    }

    fossil::sys::Memory::free(ptr); // Cleanup
}

FOSSIL_TEST(cpp_test_memory_class_secure_zero)
{
    size_t size = 16;
    fossil_sys_memory_t ptr = fossil::sys::Memory::alloc(size);
    ASSUME_NOT_CNULL(ptr);

    fossil::sys::Memory::set(ptr, 0xFF, size);
    fossil::sys::Memory::secure_zero(ptr, size);
    for (size_t i = 0; i < size; ++i)
    {
        ASSUME_ITS_TRUE(((uint8_t *)ptr)[i] == 0);
    }

    fossil::sys::Memory::free(ptr); // Cleanup
}

FOSSIL_TEST(cpp_test_memory_class_swap)
{
    size_t size = 8;
    fossil_sys_memory_t a = fossil::sys::Memory::alloc(size);
    fossil_sys_memory_t b = fossil::sys::Memory::alloc(size);
    ASSUME_NOT_CNULL(a);
    ASSUME_NOT_CNULL(b);

    fossil::sys::Memory::set(a, 0x11, size);
    fossil::sys::Memory::set(b, 0x22, size);

    fossil::sys::Memory::swap(a, b, size);

    for (size_t i = 0; i < size; ++i)
    {
        ASSUME_ITS_TRUE(((uint8_t *)a)[i] == 0x22);
        ASSUME_ITS_TRUE(((uint8_t *)b)[i] == 0x11);
    }

    fossil::sys::Memory::free(a);
    fossil::sys::Memory::free(b); // Cleanup
}

FOSSIL_TEST(cpp_test_memory_class_find)
{
    size_t size = 16;
    fossil_sys_memory_t ptr = fossil::sys::Memory::alloc(size);
    ASSUME_NOT_CNULL(ptr);

    fossil::sys::Memory::set(ptr, 0x00, size);
    ((uint8_t *)ptr)[7] = 0xAB;

    void *found = fossil::sys::Memory::find(ptr, 0xAB, size);
    ASSUME_ITS_TRUE(found == &((uint8_t *)ptr)[7]);

    void *not_found = fossil::sys::Memory::find(ptr, 0xCD, size);
    ASSUME_ITS_TRUE(not_found == nullptr);

    fossil::sys::Memory::free(ptr); // Cleanup
}

FOSSIL_TEST(cpp_test_memory_class_strdup)
{
    const char *src = "fossil";
    char *dup = fossil::sys::Memory::strdup(src);
    ASSUME_NOT_CNULL(dup);
    ASSUME_ITS_TRUE(strcmp(src, dup) == 0);
    fossil::sys::Memory::free(dup); // Cleanup
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Pool
// * * * * * * * * * * * * * * * * * * * * * * * *

FOSSIL_TEST_GROUP(cpp_memory_tests)
{
    FOSSIL_TEST_ADD(cpp_memory_suite, cpp_test_memory_alloc);
    FOSSIL_TEST_ADD(cpp_memory_suite, cpp_test_memory_realloc);
    FOSSIL_TEST_ADD(cpp_memory_suite, cpp_test_memory_dup);
    FOSSIL_TEST_ADD(cpp_memory_suite, cpp_test_memory_zero);
    FOSSIL_TEST_ADD(cpp_memory_suite, cpp_test_memory_compare);
    FOSSIL_TEST_ADD(cpp_memory_suite, cpp_test_memory_move);
    FOSSIL_TEST_ADD(cpp_memory_suite, cpp_test_memory_resize);
    FOSSIL_TEST_ADD(cpp_memory_suite, cpp_test_memory_is_valid);
    FOSSIL_TEST_ADD(cpp_memory_suite, cpp_test_memory_calloc);
    FOSSIL_TEST_ADD(cpp_memory_suite, cpp_test_memory_init);
    FOSSIL_TEST_ADD(cpp_memory_suite, cpp_test_memory_class_alloc);
    FOSSIL_TEST_ADD(cpp_memory_suite, cpp_test_memory_class_realloc);
    FOSSIL_TEST_ADD(cpp_memory_suite, cpp_test_memory_class_calloc);
    FOSSIL_TEST_ADD(cpp_memory_suite, cpp_test_memory_class_init);
    FOSSIL_TEST_ADD(cpp_memory_suite, cpp_test_memory_class_copy);
    FOSSIL_TEST_ADD(cpp_memory_suite, cpp_test_memory_class_set);
    FOSSIL_TEST_ADD(cpp_memory_suite, cpp_test_memory_class_dup);
    FOSSIL_TEST_ADD(cpp_memory_suite, cpp_test_memory_class_zero);
    FOSSIL_TEST_ADD(cpp_memory_suite, cpp_test_memory_class_compare);
    FOSSIL_TEST_ADD(cpp_memory_suite, cpp_test_memory_class_move);
    FOSSIL_TEST_ADD(cpp_memory_suite, cpp_test_memory_class_resize);
    FOSSIL_TEST_ADD(cpp_memory_suite, cpp_test_memory_class_is_valid);
    FOSSIL_TEST_ADD(cpp_memory_suite, cpp_test_memory_class_fill);
    FOSSIL_TEST_ADD(cpp_memory_suite, cpp_test_memory_class_secure_zero);
    FOSSIL_TEST_ADD(cpp_memory_suite, cpp_test_memory_class_swap);
    FOSSIL_TEST_ADD(cpp_memory_suite, cpp_test_memory_class_find);
    FOSSIL_TEST_ADD(cpp_memory_suite, cpp_test_memory_class_strdup);

    FOSSIL_TEST_REGISTER(cpp_memory_suite);
}
