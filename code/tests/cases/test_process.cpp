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
extern "C" {
#include "fossil/sys/process.h"
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Utilities
// * * * * * * * * * * * * * * * * * * * * * * * *
// Setup steps for things like test fixtures and
// mock objects are set here.
// * * * * * * * * * * * * * * * * * * * * * * * *

// Define the test suite and add test cases
FOSSIL_SUITE(cpp_process_suite);

// Setup function for the test suite
FOSSIL_SETUP(cpp_process_suite) {
    // Setup code here
}

// Teardown function for the test suite
FOSSIL_TEARDOWN(cpp_process_suite) {
    // Teardown code here
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Cases
// * * * * * * * * * * * * * * * * * * * * * * * *
// The test cases below are provided as samples, inspired
// by the Meson build system's approach of using test cases
// as samples for library usage.
// * * * * * * * * * * * * * * * * * * * * * * * *

// ** Test Process::get_pid **
FOSSIL_TEST(cpp_test_process_get_pid) {
    uint32_t pid = fossil::sys::Process::get_pid();
    ASSUME_ITS_TRUE(pid != 0 && pid != (uint32_t)-1);
}

// ** Test Process::get_name **
FOSSIL_TEST(cpp_test_process_get_name) {
    uint32_t pid = fossil::sys::Process::get_pid();
    std::string name;
    int status = fossil::sys::Process::get_name(pid, name);

    ASSUME_ITS_TRUE(status == 0 || status == -2);

    if (status == 0) {
        ASSUME_ITS_TRUE(!name.empty());
    }
}

// ** Test Process::get_info **
FOSSIL_TEST(cpp_test_process_get_info) {
    uint32_t pid = fossil::sys::Process::get_pid();
    fossil_sys_process_info_t info{};
    int status = fossil::sys::Process::get_info(pid, info);

    ASSUME_ITS_TRUE(status == 0 || status == -2);

    if (status == 0) {
        ASSUME_ITS_EQUAL_I32(info.pid, pid);
        ASSUME_ITS_TRUE(strlen(info.name) > 0);
    }
}

// ** Test Process::list **
FOSSIL_TEST(cpp_test_process_list) {
    fossil_sys_process_list_t plist{};
    int status = fossil::sys::Process::list(plist);

    ASSUME_ITS_TRUE(status == 0 || status == -2);

    if (status == 0) {
        ASSUME_ITS_TRUE(plist.count > 0);

        int found = 0;
        for (size_t i = 0; i < plist.count; ++i) {
            if (plist.list[i].pid > 0) {
                found = 1;
                break;
            }
        }
        ASSUME_ITS_TRUE(found);
    }
}

// ** Test Process::get_environment **
FOSSIL_TEST(cpp_test_process_get_environment) {
    uint32_t pid = fossil::sys::Process::get_pid();
    std::string env;
    int written = fossil::sys::Process::get_environment(pid, env);

    /* Portable contract:
       >=0 success
       -1 runtime failure
       -2 unsupported/restricted */
    ASSUME_ITS_TRUE(written >= 0 || written == -1 || written == -2);

    if (written > 0) {
        ASSUME_ITS_TRUE(env.find('=') != std::string::npos);
    }
}

// ** Test Process::terminate (self-termination, should fail) **
FOSSIL_TEST(cpp_test_process_terminate_self) {
    uint32_t pid = fossil::sys::Process::get_pid();
    int status = fossil::sys::Process::terminate(pid, false);

    /* Should never successfully terminate itself */
    ASSUME_NOT_EQUAL_I32(status, 0);
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Pool
// * * * * * * * * * * * * * * * * * * * * * * * *
FOSSIL_TEST_GROUP(cpp_process_tests) {
    FOSSIL_TEST_ADD(cpp_process_suite, cpp_test_process_get_pid);
    FOSSIL_TEST_ADD(cpp_process_suite, cpp_test_process_get_name);
    FOSSIL_TEST_ADD(cpp_process_suite, cpp_test_process_get_info);
    FOSSIL_TEST_ADD(cpp_process_suite, cpp_test_process_list);
    FOSSIL_TEST_ADD(cpp_process_suite, cpp_test_process_get_environment);
    FOSSIL_TEST_ADD(cpp_process_suite, cpp_test_process_terminate_self);

    FOSSIL_TEST_REGISTER(cpp_process_suite);
}
