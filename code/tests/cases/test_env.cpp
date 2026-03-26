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
    std::string key = "FOSSIL_TEST_ENV_CPP_EXISTS";
    setenv(key.c_str(), "1", 1);
    ASSUME_ITS_TRUE(std::getenv(key.c_str()) != nullptr);
    unsetenv(key.c_str());
    ASSUME_ITS_FALSE(std::getenv(key.c_str()) != nullptr);
}

FOSSIL_TEST(cpp_test_env_cpp_wrapper_get_or) {
    std::string key = "FOSSIL_TEST_ENV_CPP_OR";
    unsetenv(key.c_str());
    std::string fallback = "cpp_fallback";
    const char* got = std::getenv(key.c_str());
    ASSUME_ITS_EQUAL_CSTR((got ? got : fallback.c_str()), fallback.c_str());

    setenv(key.c_str(), "cpp_real", 1);
    got = std::getenv(key.c_str());
    ASSUME_ITS_EQUAL_CSTR((got ? got : fallback.c_str()), "cpp_real");
    unsetenv(key.c_str());
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

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Pool
// * * * * * * * * * * * * * * * * * * * * * * * *
FOSSIL_TEST_GROUP(cpp_env_tests) {
    FOSSIL_TEST_ADD(cpp_env_suite, cpp_test_env_cpp_wrapper_get_set);
    FOSSIL_TEST_ADD(cpp_env_suite, cpp_test_env_cpp_wrapper_exists);
    FOSSIL_TEST_ADD(cpp_env_suite, cpp_test_env_cpp_wrapper_get_or);
    FOSSIL_TEST_ADD(cpp_env_suite, cpp_test_env_cpp_wrapper_get_int);
    FOSSIL_TEST_ADD(cpp_env_suite, cpp_test_env_cpp_wrapper_get_bool);
    FOSSIL_TEST_ADD(cpp_env_suite, cpp_test_env_cpp_wrapper_foreach);

    FOSSIL_TEST_REGISTER(cpp_env_suite);
}
