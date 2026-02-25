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

using fossil::sys::Dynamic;

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Utilities
// * * * * * * * * * * * * * * * * * * * * * * * *
// Setup steps for things like test fixtures and
// mock objects are set here.
// * * * * * * * * * * * * * * * * * * * * * * * *

// Define the test suite and add test cases
FOSSIL_SUITE(cpp_dynamic_suite);

// Setup function for the test suite
FOSSIL_SETUP(cpp_dynamic_suite) {
    // Setup code here
}

// Teardown function for the test suite
FOSSIL_TEARDOWN(cpp_dynamic_suite) {
    // Teardown code here
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Cases
// * * * * * * * * * * * * * * * * * * * * * * * *
// The test cases below are provided as samples, inspired
// by the Meson build system's approach of using test cases
// as samples for library usage.
// * * * * * * * * * * * * * * * * * * * * * * * *

FOSSIL_TEST(cpp_test_dynamic_default_ctor) {
    Dynamic d;
    ASSUME_ITS_FALSE(d.is_loaded());
}

FOSSIL_TEST(cpp_test_dynamic_path_ctor_invalid) {
    Dynamic d("nonexistent_lib_12345");
    ASSUME_ITS_FALSE(d.is_loaded());
    ASSUME_NOT_CNULL(d.error());
}

FOSSIL_TEST(cpp_test_dynamic_id_path_ctor_invalid) {
    Dynamic d("test.lib", "nonexistent_lib_12345");
    ASSUME_ITS_FALSE(d.is_loaded());
}

FOSSIL_TEST(cpp_test_dynamic_load_invalid) {
    Dynamic d;
    bool result = d.load("nonexistent_lib_12345");
    ASSUME_ITS_FALSE(result);
    ASSUME_ITS_FALSE(d.is_loaded());
}

FOSSIL_TEST(cpp_test_dynamic_unload_not_loaded) {
    Dynamic d;
    bool result = d.unload();
    ASSUME_ITS_FALSE(result);
}

FOSSIL_TEST(cpp_test_dynamic_symbol_unloaded) {
    Dynamic d;
    void* sym = d.symbol("anything");
    ASSUME_ITS_CNULL(sym);
}

FOSSIL_TEST(cpp_test_dynamic_symbol_null_name) {
    Dynamic d;
    void* sym = d.symbol(NULL);
    ASSUME_ITS_CNULL(sym);
}

FOSSIL_TEST(cpp_test_dynamic_move_ctor) {
    Dynamic a("nonexistent_lib_12345");
    bool before = a.is_loaded();

    Dynamic b(std::move(a));

    ASSUME_ITS_FALSE(a.is_loaded());
    ASSUME_ITS_EQUAL_I32(before, b.is_loaded());
}

FOSSIL_TEST(cpp_test_dynamic_move_assign) {
    Dynamic a("nonexistent_lib_12345");
    Dynamic b;

    bool before = a.is_loaded();

    b = std::move(a);

    ASSUME_ITS_FALSE(a.is_loaded());
    ASSUME_ITS_EQUAL_I32(before, b.is_loaded());
}

FOSSIL_TEST(cpp_test_dynamic_error) {
    Dynamic d;
    const char* err = d.error();
    ASSUME_NOT_CNULL(err);
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Pool
// * * * * * * * * * * * * * * * * * * * * * * * *
FOSSIL_TEST_GROUP(cpp_dynamic_tests) {

    FOSSIL_TEST_ADD(cpp_dynamic_suite, cpp_test_dynamic_default_ctor);
    FOSSIL_TEST_ADD(cpp_dynamic_suite, cpp_test_dynamic_path_ctor_invalid);
    FOSSIL_TEST_ADD(cpp_dynamic_suite, cpp_test_dynamic_id_path_ctor_invalid);
    FOSSIL_TEST_ADD(cpp_dynamic_suite, cpp_test_dynamic_load_invalid);
    FOSSIL_TEST_ADD(cpp_dynamic_suite, cpp_test_dynamic_unload_not_loaded);
    FOSSIL_TEST_ADD(cpp_dynamic_suite, cpp_test_dynamic_symbol_unloaded);
    FOSSIL_TEST_ADD(cpp_dynamic_suite, cpp_test_dynamic_symbol_null_name);
    FOSSIL_TEST_ADD(cpp_dynamic_suite, cpp_test_dynamic_move_ctor);
    FOSSIL_TEST_ADD(cpp_dynamic_suite, cpp_test_dynamic_move_assign);
    FOSSIL_TEST_ADD(cpp_dynamic_suite, cpp_test_dynamic_error);

    FOSSIL_TEST_REGISTER(cpp_dynamic_suite);
}
