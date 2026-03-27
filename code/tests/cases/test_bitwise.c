/*
 * -----------------------------------------------------------------------------
 * Project: Fossil Logic
 *
 * performance, cross-platform applications and libraries. The code contained
 * This file is part of the Fossil Logic project, which aims to develop high-
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
FOSSIL_SUITE(c_bitwise_suite);

// Setup function for the test suite
FOSSIL_SETUP(c_bitwise_suite)
{
    // Setup code here
}

// Teardown function for the test suite
FOSSIL_TEARDOWN(c_bitwise_suite)
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

// ** Test fossil_sys_bitwise_parse Function **
FOSSIL_TEST(c_test_bitwise_parse)
{
    fossil_sys_bitwise_entry_t entries[] = {
        {"read", 0x1},
        {"write", 0x2},
        {"execute", 0x4},
        {NULL, 0}};

    const fossil_sys_bitwise_table_t table = {entries, sizeof(entries) / sizeof(entries[0]) - 1};

    uint64_t result = fossil_sys_bitwise_parse("read|write", &table);
    ASSUME_ITS_EQUAL_I32(result, 0x3); // 0x1 | 0x2

    result = fossil_sys_bitwise_parse("execute", &table);
    ASSUME_ITS_EQUAL_I32(result, 0x4);

    result = fossil_sys_bitwise_parse("", &table);
    ASSUME_ITS_EQUAL_I32(result, 0x0);
}

// ** Test fossil_sys_bitwise_format Function **
FOSSIL_TEST(c_test_bitwise_format)
{
    fossil_sys_bitwise_entry_t entries[] = {
        {"read", 0x1},
        {"write", 0x2},
        {"execute", 0x4},
        {NULL, 0}};

    const fossil_sys_bitwise_table_t table = {entries, sizeof(entries) / sizeof(entries[0]) - 1};

    char buffer[64];
    int status = fossil_sys_bitwise_format(0x3, &table, buffer, sizeof(buffer));
    ASSUME_ITS_EQUAL_I32(status, 0);
    ASSUME_ITS_EQUAL_CSTR(buffer, "read|write");

    status = fossil_sys_bitwise_format(0x4, &table, buffer, sizeof(buffer));
    ASSUME_ITS_EQUAL_I32(status, 0);
    ASSUME_ITS_EQUAL_CSTR(buffer, "execute");

    status = fossil_sys_bitwise_format(0x0, &table, buffer, sizeof(buffer));
    ASSUME_ITS_EQUAL_I32(status, 0);
    ASSUME_ITS_EQUAL_CSTR(buffer, "");
}

// ** Test fossil_sys_bitwise_lookup Function **
FOSSIL_TEST(c_test_bitwise_lookup)
{
    fossil_sys_bitwise_entry_t entries[] = {
        {"read", 0x1},
        {"write", 0x2},
        {"execute", 0x4},
        {NULL, 0}};
    const fossil_sys_bitwise_table_t table = {entries, sizeof(entries) / sizeof(entries[0]) - 1};
    uint64_t bit;
    int status = fossil_sys_bitwise_lookup("read", &table, &bit);
    ASSUME_ITS_EQUAL_I32(status, 0);
    ASSUME_ITS_EQUAL_I32(bit, 0x1);
    status = fossil_sys_bitwise_lookup("write", &table, &bit);
    ASSUME_ITS_EQUAL_I32(status, 0);
    ASSUME_ITS_EQUAL_I32(bit, 0x2);
    status = fossil_sys_bitwise_lookup("execute", &table, &bit);
    ASSUME_ITS_EQUAL_I32(status, 0);
    ASSUME_ITS_EQUAL_I32(bit, 0x4);
}

// ** Test fossil_sys_bitwise_all Function **
FOSSIL_TEST(c_test_bitwise_all)
{
    fossil_sys_bitwise_entry_t entries[] = {
        {"read", 0x1},
        {"write", 0x2},
        {"execute", 0x4},
        {NULL, 0}};
    const fossil_sys_bitwise_table_t table = {entries, sizeof(entries) / sizeof(entries[0]) - 1};

    uint64_t mask = fossil_sys_bitwise_all(&table);
    ASSUME_ITS_EQUAL_I32(mask, 0x7); // 0x1 | 0x2 | 0x4
}

// ** Test fossil_sys_bitwise_validate Function **
FOSSIL_TEST(c_test_bitwise_validate)
{
    fossil_sys_bitwise_entry_t entries[] = {
        {"read", 0x1},
        {"write", 0x2},
        {"execute", 0x4},
        {NULL, 0}};
    const fossil_sys_bitwise_table_t table = {entries, sizeof(entries) / sizeof(entries[0]) - 1};

    // Known good mask
    int status = fossil_sys_bitwise_validate(0x3, &table);
    ASSUME_ITS_EQUAL_I32(status, 0);

    // Mask with an unknown bit set (0x8)
    status = fossil_sys_bitwise_validate(0x8, &table);
    ASSUME_NOT_EQUAL_I32(status, 0); // Expect failure
}

// ** Test fossil_sys_bitwise_name Function **
FOSSIL_TEST(c_test_bitwise_name)
{
    fossil_sys_bitwise_entry_t entries[] = {
        {"read", 0x1},
        {"write", 0x2},
        {"execute", 0x4},
        {NULL, 0}};
    const fossil_sys_bitwise_table_t table = {entries, sizeof(entries) / sizeof(entries[0]) - 1};

    const char *name = fossil_sys_bitwise_name(0x1, &table);
    ASSUME_ITS_EQUAL_CSTR(name, "read");

    name = fossil_sys_bitwise_name(0x4, &table);
    ASSUME_ITS_EQUAL_CSTR(name, "execute");

    name = fossil_sys_bitwise_name(0x8, &table);
    ASSUME_ITS_CNULL(name); // 0x8 is not in the table
}

// ** Test fossil_sys_bitwise_count Function **
FOSSIL_TEST(c_test_bitwise_count)
{
    uint64_t mask = 0x7; // 3 bits set
    size_t count = fossil_sys_bitwise_count(mask);
    ASSUME_ITS_EQUAL_I32(count, 3);

    mask = 0x0;
    count = fossil_sys_bitwise_count(mask);
    ASSUME_ITS_EQUAL_I32(count, 0);

    mask = 0xFFFFFFFFFFFFFFFFULL;
    count = fossil_sys_bitwise_count(mask);
    ASSUME_ITS_EQUAL_I32(count, 64);
}

// ** Test fossil_sys_bitwise_has Function **
FOSSIL_TEST(c_test_bitwise_has)
{
    uint64_t mask = 0x5; // bits 0 and 2 set

    ASSUME_ITS_TRUE(fossil_sys_bitwise_has(mask, 0x1));  // bit 0 set
    ASSUME_ITS_FALSE(fossil_sys_bitwise_has(mask, 0x2)); // bit 1 NOT set
    ASSUME_ITS_TRUE(fossil_sys_bitwise_has(mask, 0x4));  // bit 2 set
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Pool
// * * * * * * * * * * * * * * * * * * * * * * * *
FOSSIL_TEST_GROUP(c_bitwise_tests)
{
    FOSSIL_TEST_ADD(c_bitwise_suite, c_test_bitwise_parse);
    FOSSIL_TEST_ADD(c_bitwise_suite, c_test_bitwise_format);
    FOSSIL_TEST_ADD(c_bitwise_suite, c_test_bitwise_lookup);
    FOSSIL_TEST_ADD(c_bitwise_suite, c_test_bitwise_all);
    FOSSIL_TEST_ADD(c_bitwise_suite, c_test_bitwise_validate);
    FOSSIL_TEST_ADD(c_bitwise_suite, c_test_bitwise_name);
    FOSSIL_TEST_ADD(c_bitwise_suite, c_test_bitwise_count);
    FOSSIL_TEST_ADD(c_bitwise_suite, c_test_bitwise_has);

    FOSSIL_TEST_REGISTER(c_bitwise_suite);
}
