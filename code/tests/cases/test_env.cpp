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
FOSSIL_SUITE(cpp_env_suite);

// Setup function for the test suite
FOSSIL_SETUP(cpp_env_suite) {
    // Setup code here
}

// Teardown function for the test suite
FOSSIL_TEARDOWN(cpp_env_suite) {
    // Teardown code here
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Cases
// * * * * * * * * * * * * * * * * * * * * * * * *
// The test cases below are provided as samples, inspired
// by the Meson build system's approach of using test cases
// as samples for library usage.
// * * * * * * * * * * * * * * * * * * * * * * * *

// ** Test fossil::sys::Env C++ wrapper **
FOSSIL_TEST(cpp_test_env_cpp_wrapper_get_set) {
    using fossil::sys::Env;
    std::string key = "FOSSIL_TEST_ENV_CPP";
    std::string value = "cpp_value";
    bool set_result = Env::set(key, value);
    ASSUME_ITS_TRUE(set_result);

    std::string got = Env::get(key);
    ASSUME_ITS_EQUAL_CSTR(got.c_str(), value.c_str());

    // Unset and check
    set_result = Env::set(key, "");
    ASSUME_ITS_TRUE(set_result);
    got = Env::get(key);
    ASSUME_ITS_EQUAL_CSTR(got.c_str(), "");
}

FOSSIL_TEST(cpp_test_env_cpp_wrapper_exists) {
    using fossil::sys::Env;
    std::string key = "FOSSIL_TEST_ENV_CPP_EXISTS";
    Env::set(key, "1");
    ASSUME_ITS_TRUE(Env::exists(key));
    Env::set(key, "");
    ASSUME_ITS_FALSE(Env::exists(key));
}

FOSSIL_TEST(cpp_test_env_cpp_wrapper_get_or) {
    using fossil::sys::Env;
    std::string key = "FOSSIL_TEST_ENV_CPP_OR";
    Env::set(key, "");
    std::string fallback = "cpp_fallback";
    std::string got = Env::get_or(key, fallback);
    ASSUME_ITS_EQUAL_CSTR(got.c_str(), fallback.c_str());

    Env::set(key, "cpp_real");
    got = Env::get_or(key, fallback);
    ASSUME_ITS_EQUAL_CSTR(got.c_str(), "cpp_real");
    Env::set(key, "");
}

FOSSIL_TEST(cpp_test_env_cpp_wrapper_get_int) {
    using fossil::sys::Env;
    std::string key = "FOSSIL_TEST_ENV_CPP_INT";
    Env::set(key, "123");
    int val = Env::get_int(key, -1);
    ASSUME_ITS_EQUAL_I32(val, 123);

    Env::set(key, "notanint");
    val = Env::get_int(key, -1);
    ASSUME_ITS_EQUAL_I32(val, -1);

    Env::set(key, "");
    val = Env::get_int(key, 456);
    ASSUME_ITS_EQUAL_I32(val, 456);
}

FOSSIL_TEST(cpp_test_env_cpp_wrapper_get_bool) {
    using fossil::sys::Env;
    std::string key = "FOSSIL_TEST_ENV_CPP_BOOL";
    Env::set(key, "1");
    ASSUME_ITS_TRUE(Env::get_bool(key, false));
    Env::set(key, "true");
    ASSUME_ITS_TRUE(Env::get_bool(key, false));
    Env::set(key, "yes");
    ASSUME_ITS_TRUE(Env::get_bool(key, false));
    Env::set(key, "0");
    ASSUME_ITS_FALSE(Env::get_bool(key, true));
    Env::set(key, "false");
    ASSUME_ITS_FALSE(Env::get_bool(key, true));
    Env::set(key, "no");
    ASSUME_ITS_FALSE(Env::get_bool(key, true));
    Env::set(key, "notabool");
    ASSUME_ITS_TRUE(Env::get_bool(key, true));
    Env::set(key, "");
}

FOSSIL_TEST(cpp_test_env_cpp_wrapper_foreach) {
    using fossil::sys::Env;
    int count = 0;
    Env::foreach_var([&count](const std::string& key, const std::string& value) {
        if (!key.empty() && !value.empty()) ++count;
        return count >= 10 ? 1 : 0;
    });
    ASSUME_ITS_TRUE(count > 0);
}

// ** Test fossil_sys_env_get and fossil_sys_env_set **
FOSSIL_TEST(cpp_test_env_get_set) {
    // Set and get a custom variable
    const char *key = "FOSSIL_TEST_ENV";
    const char *value = "fossil_value";
    int set_result = fossil_sys_env_set(key, value);
    ASSUME_ITS_EQUAL_I32(set_result, 0);

    const char *got = fossil_sys_env_get(key);
    ASSUME_NOT_CNULL(got);
    ASSUME_ITS_EQUAL_CSTR(got, value);

    // Unset and check
    set_result = fossil_sys_env_set(key, NULL);
    ASSUME_ITS_EQUAL_I32(set_result, 0);
    got = fossil_sys_env_get(key);
    ASSUME_ITS_CNULL(got);
}

// ** Test fossil_sys_env_exists **
FOSSIL_TEST(cpp_test_env_exists) {
    const char *key = "FOSSIL_TEST_EXISTS";
    fossil_sys_env_set(key, "1");
    ASSUME_ITS_TRUE(fossil_sys_env_exists(key));
    fossil_sys_env_set(key, NULL);
    ASSUME_ITS_FALSE(fossil_sys_env_exists(key));
}

// ** Test fossil_sys_env_get_or **
FOSSIL_TEST(cpp_test_env_get_or) {
    const char *key = "FOSSIL_TEST_OR";
    fossil_sys_env_set(key, NULL);
    const char *fallback = "fallback_value";
    const char *got = fossil_sys_env_get_or(key, fallback);
    ASSUME_ITS_EQUAL_CSTR(got, fallback);

    fossil_sys_env_set(key, "real_value");
    got = fossil_sys_env_get_or(key, fallback);
    ASSUME_ITS_EQUAL_CSTR(got, "real_value");
    fossil_sys_env_set(key, NULL);
}

// ** Test fossil_sys_env_get_int **
FOSSIL_TEST(cpp_test_env_get_int) {
    const char *key = "FOSSIL_TEST_INT";
    fossil_sys_env_set(key, "42");
    int val = fossil_sys_env_get_int(key, -1);
    ASSUME_ITS_EQUAL_I32(val, 42);

    fossil_sys_env_set(key, "notanint");
    val = fossil_sys_env_get_int(key, -1);
    ASSUME_ITS_EQUAL_I32(val, -1);

    fossil_sys_env_set(key, NULL);
    val = fossil_sys_env_get_int(key, 123);
    ASSUME_ITS_EQUAL_I32(val, 123);
}

// ** Test fossil_sys_env_get_bool **
FOSSIL_TEST(cpp_test_env_get_bool) {
    const char *key = "FOSSIL_TEST_BOOL";
    fossil_sys_env_set(key, "1");
    ASSUME_ITS_TRUE(fossil_sys_env_get_bool(key, 0));
    fossil_sys_env_set(key, "true");
    ASSUME_ITS_TRUE(fossil_sys_env_get_bool(key, 0));
    fossil_sys_env_set(key, "yes");
    ASSUME_ITS_TRUE(fossil_sys_env_get_bool(key, 0));
    fossil_sys_env_set(key, "0");
    ASSUME_ITS_FALSE(fossil_sys_env_get_bool(key, 1));
    fossil_sys_env_set(key, "false");
    ASSUME_ITS_FALSE(fossil_sys_env_get_bool(key, 1));
    fossil_sys_env_set(key, "no");
    ASSUME_ITS_FALSE(fossil_sys_env_get_bool(key, 1));
    fossil_sys_env_set(key, "notabool");
    ASSUME_ITS_TRUE(fossil_sys_env_get_bool(key, 1));
    fossil_sys_env_set(key, NULL);
}

// ** Test canonical keys (fossil.sys.name, fossil.sys.arch, etc.) **
FOSSIL_TEST(cpp_test_env_canonical_keys) {
    const char *os_name = fossil_sys_env_get("fossil.sys.name");
    ASSUME_NOT_CNULL(os_name);

    const char *arch = fossil_sys_env_get("fossil.sys.arch");
    ASSUME_NOT_CNULL(arch);

    const char *page_size = fossil_sys_env_get("fossil.sys.page_size");
    ASSUME_NOT_CNULL(page_size);

    const char *cpu_count = fossil_sys_env_get("fossil.sys.cpu_count");
    ASSUME_NOT_CNULL(cpu_count);

    const char *path = fossil_sys_env_get("fossil.sys.path");
    ASSUME_NOT_CNULL(path);

    const char *home = fossil_sys_env_get("fossil.sys.home");
    ASSUME_NOT_CNULL(home);

    const char *tmp = fossil_sys_env_get("fossil.sys.tmp");
    ASSUME_NOT_CNULL(tmp);
}

// ** Test fossil_sys_env_foreach **
static int test_env_foreach_cb(const char *key, const char *value, void *user_data) {
    int *count = (int *)user_data;
    if (key && value) (*count)++;
    // Stop after 10 for speed
    return *count >= 10 ? 1 : 0;
}

FOSSIL_TEST(cpp_test_env_foreach) {
    int count = 0;
    fossil_sys_env_foreach(test_env_foreach_cb, &count);
    ASSUME_ITS_TRUE(count > 0);
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Pool
// * * * * * * * * * * * * * * * * * * * * * * * *
FOSSIL_TEST_GROUP(cpp_env_tests) {
    FOSSIL_TEST_ADD(cpp_env_suite, cpp_test_env_get_set);
    FOSSIL_TEST_ADD(cpp_env_suite, cpp_test_env_exists);
    FOSSIL_TEST_ADD(cpp_env_suite, cpp_test_env_get_or);
    FOSSIL_TEST_ADD(cpp_env_suite, cpp_test_env_get_int);
    FOSSIL_TEST_ADD(cpp_env_suite, cpp_test_env_get_bool);
    FOSSIL_TEST_ADD(cpp_env_suite, cpp_test_env_canonical_keys);
    FOSSIL_TEST_ADD(cpp_env_suite, cpp_test_env_foreach);

    FOSSIL_TEST_REGISTER(cpp_env_suite);
}
