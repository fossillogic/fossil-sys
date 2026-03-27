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
FOSSIL_SUITE(cpp_bitwise_suite);

// Setup function for the test suite
FOSSIL_SETUP(cpp_bitwise_suite)
{
    // Setup code here
}

// Teardown function for the test suite
FOSSIL_TEARDOWN(cpp_bitwise_suite)
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

// ** Test fossil::sys::Bitwise::parse Function **
FOSSIL_TEST(cpp_test_class_bitwise_parse)
{
    fossil_sys_bitwise_entry_t entries[] = {
        {"read", 0x1},
        {"write", 0x2},
        {"execute", 0x4},
        {nullptr, 0}};

    fossil_sys_bitwise_table_t table = {entries, sizeof(entries) / sizeof(entries[0]) - 1};

    uint64_t result = fossil::sys::Bitwise::parse("read|write", &table);
    ASSUME_ITS_EQUAL_I32(result, 0x3); // 0x1 | 0x2

    result = fossil::sys::Bitwise::parse("execute", &table);
    ASSUME_ITS_EQUAL_I32(result, 0x4);

    result = fossil::sys::Bitwise::parse("", &table);
    ASSUME_ITS_EQUAL_I32(result, 0x0);
}

// ** Test fossil::sys::Bitwise::format Function **
FOSSIL_TEST(cpp_test_class_bitwise_format)
{
    fossil_sys_bitwise_entry_t entries[] = {
        {"read", 0x1},
        {"write", 0x2},
        {"execute", 0x4},
        {nullptr, 0}};

    fossil_sys_bitwise_table_t table = {entries, sizeof(entries) / sizeof(entries[0]) - 1};

    char buffer[64];
    int status = fossil::sys::Bitwise::format(0x3, &table, buffer, sizeof(buffer));
    ASSUME_ITS_EQUAL_I32(status, 0);
    ASSUME_ITS_EQUAL_CSTR(buffer, "read|write");

    status = fossil::sys::Bitwise::format(0x4, &table, buffer, sizeof(buffer));
    ASSUME_ITS_EQUAL_I32(status, 0);
    ASSUME_ITS_EQUAL_CSTR(buffer, "execute");

    status = fossil::sys::Bitwise::format(0x0, &table, buffer, sizeof(buffer));
    ASSUME_ITS_EQUAL_I32(status, 0);
    ASSUME_ITS_EQUAL_CSTR(buffer, "");
}

// ** Test fossil::sys::Bitwise::lookup Function **
FOSSIL_TEST(cpp_test_class_bitwise_lookup)
{
    fossil_sys_bitwise_entry_t entries[] = {
        {"read", 0x1},
        {"write", 0x2},
        {"execute", 0x4},
        {nullptr, 0}};

    fossil_sys_bitwise_table_t table = {entries, sizeof(entries) / sizeof(entries[0]) - 1};

    uint64_t bit;
    int status = fossil::sys::Bitwise::lookup("read", &table, bit);
    ASSUME_ITS_EQUAL_I32(status, 0);
    ASSUME_ITS_EQUAL_I32(bit, 0x1);

    status = fossil::sys::Bitwise::lookup("write", &table, bit);
    ASSUME_ITS_EQUAL_I32(status, 0);
    ASSUME_ITS_EQUAL_I32(bit, 0x2);

    status = fossil::sys::Bitwise::lookup("unknown", &table, bit);
    ASSUME_NOT_EQUAL_I32(status, 0);
}

// ** Test fossil::sys::Bitwise::all Function **
FOSSIL_TEST(cpp_test_class_bitwise_all)
{
    fossil_sys_bitwise_entry_t entries[] = {
        {"read", 0x1},
        {"write", 0x2},
        {"execute", 0x4},
        {nullptr, 0}};
    fossil_sys_bitwise_table_t table = {entries, sizeof(entries) / sizeof(entries[0]) - 1};

    uint64_t mask = fossil::sys::Bitwise::all(&table);
    ASSUME_ITS_EQUAL_I32(mask, 0x7); // 0x1 | 0x2 | 0x4
}

// ** Test fossil::sys::Bitwise::validate Function **
FOSSIL_TEST(cpp_test_class_bitwise_validate)
{
    fossil_sys_bitwise_entry_t entries[] = {
        {"read", 0x1},
        {"write", 0x2},
        {"execute", 0x4},
        {nullptr, 0}};
    fossil_sys_bitwise_table_t table = {entries, sizeof(entries) / sizeof(entries[0]) - 1};

    // Known good mask
    ASSUME_ITS_TRUE(fossil::sys::Bitwise::validate(0x3, &table));

    // Mask with an unknown bit set
    ASSUME_ITS_FALSE(fossil::sys::Bitwise::validate(0x8, &table));
}

// ** Test fossil::sys::Bitwise::name Function **
FOSSIL_TEST(cpp_test_class_bitwise_name)
{
    fossil_sys_bitwise_entry_t entries[] = {
        {"read", 0x1},
        {"write", 0x2},
        {"execute", 0x4},
        {nullptr, 0}};
    fossil_sys_bitwise_table_t table = {entries, sizeof(entries) / sizeof(entries[0]) - 1};

    std::string name = fossil::sys::Bitwise::name(0x1, &table);
    ASSUME_ITS_EQUAL_CSTR(name.c_str(), "read");

    name = fossil::sys::Bitwise::name(0x4, &table);
    ASSUME_ITS_EQUAL_CSTR(name.c_str(), "execute");

    name = fossil::sys::Bitwise::name(0x8, &table);
    ASSUME_ITS_EQUAL_CSTR(name.c_str(), "");
}

// ** Test fossil::sys::Bitwise::count Function **
FOSSIL_TEST(cpp_test_class_bitwise_count)
{
    ASSUME_ITS_EQUAL_I32(fossil::sys::Bitwise::count(0x7), 3);
    ASSUME_ITS_EQUAL_I32(fossil::sys::Bitwise::count(0x0), 0);
    ASSUME_ITS_EQUAL_I32(fossil::sys::Bitwise::count(0xFFFFFFFFFFFFFFFFULL), 64);
}

// ** Test fossil::sys::Bitwise::has Function **
FOSSIL_TEST(cpp_test_class_bitwise_has)
{
    uint64_t mask = 0x5; // bits 0 and 2 set
    ASSUME_ITS_TRUE(fossil::sys::Bitwise::has(mask, 0x1));
    ASSUME_ITS_FALSE(fossil::sys::Bitwise::has(mask, 0x2));
    ASSUME_ITS_TRUE(fossil::sys::Bitwise::has(mask, 0x4));
}

// ** Test fossil::sys::Bitwise::format (std::string version) **
FOSSIL_TEST(cpp_test_class_bitwise_format_string)
{
    fossil_sys_bitwise_entry_t entries[] = {
        {"read", 0x1},
        {"write", 0x2},
        {"execute", 0x4},
        {nullptr, 0}};
    fossil_sys_bitwise_table_t table = {entries, sizeof(entries) / sizeof(entries[0]) - 1};

    std::string result = fossil::sys::Bitwise::format(0x3, &table);
    ASSUME_ITS_EQUAL_CSTR(result.c_str(), "read|write");

    result = fossil::sys::Bitwise::format(0x4, &table);
    ASSUME_ITS_EQUAL_CSTR(result.c_str(), "execute");

    result = fossil::sys::Bitwise::format(0x0, &table);
    ASSUME_ITS_EQUAL_CSTR(result.c_str(), "");
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Pool
// * * * * * * * * * * * * * * * * * * * * * * * *
FOSSIL_TEST_GROUP(cpp_bitwise_tests)
{
    FOSSIL_TEST_ADD(cpp_bitwise_suite, cpp_test_class_bitwise_parse);
    FOSSIL_TEST_ADD(cpp_bitwise_suite, cpp_test_class_bitwise_format);
    FOSSIL_TEST_ADD(cpp_bitwise_suite, cpp_test_class_bitwise_lookup);
    FOSSIL_TEST_ADD(cpp_bitwise_suite, cpp_test_class_bitwise_all);
    FOSSIL_TEST_ADD(cpp_bitwise_suite, cpp_test_class_bitwise_validate);
    FOSSIL_TEST_ADD(cpp_bitwise_suite, cpp_test_class_bitwise_name);
    FOSSIL_TEST_ADD(cpp_bitwise_suite, cpp_test_class_bitwise_count);
    FOSSIL_TEST_ADD(cpp_bitwise_suite, cpp_test_class_bitwise_has);
    FOSSIL_TEST_ADD(cpp_bitwise_suite, cpp_test_class_bitwise_format_string);

    FOSSIL_TEST_REGISTER(cpp_bitwise_suite);
}
