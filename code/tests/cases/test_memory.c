/*
 * -----------------------------------------------------------------------------
 * Project: Fossil Logic
 *
 * This file is part of the Fossil Logic project, which aims to develop high-
 * performance, cross-platform applications and libraries. The code contained
 * herein is subject to the terms and conditions defined in the project license.
 *
 * Author: Michael Gene Brockus (Dreamer)
 *
 * Copyright (C) 2024 Fossil Logic. All rights reserved.
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
FOSSIL_SUITE(c_memory_suite);

// Setup function for the test suite
FOSSIL_SETUP(c_memory_suite) {
    // Setup code here
}

// Teardown function for the test suite
FOSSIL_TEARDOWN(c_memory_suite) {
    // Teardown code here
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Cases
// * * * * * * * * * * * * * * * * * * * * * * * *
// The test cases below are provided as samples, inspired
// by the Meson build system's approach of using test cases
// as samples for library usage.
// * * * * * * * * * * * * * * * * * * * * * * * *

FOSSIL_TEST(c_test_memory_alloc) {
    size_t size = 10;
    fossil_sys_memory_t ptr = fossil_sys_memory_alloc(size);
    ASSUME_NOT_CNULL(ptr);
    fossil_sys_memory_free(ptr); // Cleanup
}

FOSSIL_TEST(c_test_memory_realloc) {
    size_t size = 10;
    fossil_sys_memory_t ptr = fossil_sys_memory_alloc(size);
    ASSUME_NOT_CNULL(ptr);

    size *= 2;
    ptr = fossil_sys_memory_realloc(ptr, size);
    ASSUME_NOT_CNULL(ptr);

    fossil_sys_memory_free(ptr); // Cleanup
}

FOSSIL_TEST(c_test_memory_dup) {
    size_t size = 10;
    fossil_sys_memory_t src = fossil_sys_memory_alloc(size);
    ASSUME_NOT_CNULL(src);

    fossil_sys_memory_t dest = fossil_sys_memory_dup(src, size);
    ASSUME_NOT_CNULL(dest);

    ASSUME_ITS_TRUE(memcmp(src, dest, size) == 0); // Ensure that both blocks are identical
    fossil_sys_memory_free(src);
    fossil_sys_memory_free(dest); // Cleanup
}

FOSSIL_TEST(c_test_memory_zero) {
    size_t size = 10;
    fossil_sys_memory_t ptr = fossil_sys_memory_alloc(size);
    ASSUME_NOT_CNULL(ptr);

    fossil_sys_memory_zero(ptr, size);
    for (size_t i = 0; i < size; ++i) {
        ASSUME_ITS_TRUE(((unsigned char*)ptr)[i] == 0); // Ensure all bytes are zero
    }

    fossil_sys_memory_free(ptr); // Cleanup
}

FOSSIL_TEST(c_test_memory_compare) {
    size_t size = 10;
    fossil_sys_memory_t ptr1 = fossil_sys_memory_alloc(size);
    fossil_sys_memory_t ptr2 = fossil_sys_memory_alloc(size);
    ASSUME_NOT_CNULL(ptr1);
    ASSUME_NOT_CNULL(ptr2);

    fossil_sys_memory_set(ptr1, 0xAA, size); // Set all bytes to 0xAA
    fossil_sys_memory_set(ptr2, 0xAA, size); // Set all bytes to 0xAA
    ASSUME_ITS_TRUE(fossil_sys_memory_compare(ptr1, ptr2, size) == 0); // Should be equal

    fossil_sys_memory_set(ptr2, 0xBB, size); // Change ptr2
    ASSUME_ITS_TRUE(fossil_sys_memory_compare(ptr1, ptr2, size) != 0); // Should not be equal

    fossil_sys_memory_free(ptr1);
    fossil_sys_memory_free(ptr2); // Cleanup
}

FOSSIL_TEST(c_test_memory_move) {
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

FOSSIL_TEST(c_test_memory_resize) {
    size_t size = 10;
    fossil_sys_memory_t ptr = fossil_sys_memory_alloc(size);
    ASSUME_NOT_CNULL(ptr);

    ptr = fossil_sys_memory_resize(ptr, size, size * 2);
    ASSUME_NOT_CNULL(ptr); // Ensure resizing works

    fossil_sys_memory_free(ptr); // Cleanup
}

FOSSIL_TEST(c_test_memory_is_valid) {
    fossil_sys_memory_t ptr = fossil_sys_memory_alloc(10);
    ASSUME_ITS_TRUE(fossil_sys_memory_is_valid(ptr)); // Should be valid
    ASSUME_ITS_TRUE(!fossil_sys_memory_is_valid(NULL)); // NULL should not be valid

    fossil_sys_memory_free(ptr); // Cleanup
}

FOSSIL_TEST(c_test_memory_calloc) {
    size_t num = 10;
    size_t size = sizeof(int);
    fossil_sys_memory_t ptr = fossil_sys_memory_calloc(num, size);
    ASSUME_NOT_CNULL(ptr);

    for (size_t i = 0; i < num * size; ++i) {
        ASSUME_ITS_TRUE(((unsigned char*)ptr)[i] == 0); // Ensure all bytes are zero
    }

    fossil_sys_memory_free(ptr); // Cleanup
}

FOSSIL_TEST(c_test_memory_init) {
    size_t size = 10;
    int32_t value = 0xAA;
    fossil_sys_memory_t ptr = fossil_sys_memory_alloc(size);
    ASSUME_NOT_CNULL(ptr);

    fossil_sys_memory_init(ptr, size, value);
    for (size_t i = 0; i < size; ++i) {
        ASSUME_ITS_TRUE(((unsigned char*)ptr)[i] == (unsigned char)value); // Ensure all bytes are set to value
    }

    fossil_sys_memory_free(ptr); // Cleanup
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Pool
// * * * * * * * * * * * * * * * * * * * * * * * *

FOSSIL_TEST_GROUP(c_memory_tests) {
    FOSSIL_TEST_ADD(c_memory_suite, c_test_memory_alloc);
    FOSSIL_TEST_ADD(c_memory_suite, c_test_memory_realloc);
    FOSSIL_TEST_ADD(c_memory_suite, c_test_memory_dup);
    FOSSIL_TEST_ADD(c_memory_suite, c_test_memory_zero);
    FOSSIL_TEST_ADD(c_memory_suite, c_test_memory_compare);
    FOSSIL_TEST_ADD(c_memory_suite, c_test_memory_move);
    FOSSIL_TEST_ADD(c_memory_suite, c_test_memory_resize);
    FOSSIL_TEST_ADD(c_memory_suite, c_test_memory_is_valid);
    FOSSIL_TEST_ADD(c_memory_suite, c_test_memory_calloc);
    FOSSIL_TEST_ADD(c_memory_suite, c_test_memory_init);

    FOSSIL_TEST_REGISTER(c_memory_suite);
}
