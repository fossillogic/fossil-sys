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
FOSSIL_SUITE(c_process_suite);

// Setup function for the test suite
FOSSIL_SETUP(c_process_suite) {
    // Setup code here
}

// Teardown function for the test suite
FOSSIL_TEARDOWN(c_process_suite) {
    // Teardown code here
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Cases
// * * * * * * * * * * * * * * * * * * * * * * * *
// The test cases below are provided as samples, inspired
// by the Meson build system's approach of using test cases
// as samples for library usage.
// * * * * * * * * * * * * * * * * * * * * * * * *

// ** Test fossil_sys_process_get_pid **
FOSSIL_TEST(c_test_process_get_pid) {
    uint32_t pid = fossil_sys_process_get_pid();
    ASSUME_ITS_TRUE(pid > 0);
}

// ** Test fossil_sys_process_get_name **
FOSSIL_TEST(c_test_process_get_name) {
    uint32_t pid = fossil_sys_process_get_pid();
    char name[128] = {0};

    int status = fossil_sys_process_get_name(pid, name, sizeof(name));

    ASSUME_ITS_TRUE(status == 0 || status == -2);

    /* Only validate contents if a name was actually returned */
    if (status == 0 && name[0] != '\0') {
        ASSUME_ITS_TRUE(strlen(name) > 0);
    }
}

// ** Test fossil_sys_process_get_info **
FOSSIL_TEST(c_test_process_get_info) {
    uint32_t pid = fossil_sys_process_get_pid();
    fossil_sys_process_info_t info;
    int status = fossil_sys_process_get_info(pid, &info);
    ASSUME_ITS_EQUAL_I32(status, 0);
    ASSUME_ITS_EQUAL_I32(info.pid, pid);
    ASSUME_ITS_TRUE(strlen(info.name) > 0);
    // Optionally check that memory and thread count are nonzero (Linux)
    #if defined(__linux__)
    ASSUME_ITS_TRUE(info.memory_bytes > 0);
    ASSUME_ITS_TRUE(info.virtual_memory_bytes > 0);
    ASSUME_ITS_TRUE(info.thread_count > 0);
    #endif
}

// ** Test fossil_sys_process_list **
FOSSIL_TEST(c_test_process_list) {
    fossil_sys_process_list_t plist = {0};

    int status = fossil_sys_process_list(&plist);

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

// ** Test fossil_sys_process_get_environment **
FOSSIL_TEST(c_test_process_get_environment) {
    uint32_t pid = fossil_sys_process_get_pid();
    char buffer[4096];
    int written = fossil_sys_process_get_environment(pid, buffer, sizeof(buffer));
    ASSUME_ITS_TRUE(written >= 0 || written == -2);
    if (written > 0) {
        ASSUME_ITS_TRUE(strchr(buffer, '=') != NULL || strchr(buffer, ';') != NULL);
    }
}

// ** Test fossil_sys_process_terminate (self-termination, should fail) **
FOSSIL_TEST(c_test_process_terminate_self) {
    uint32_t pid = fossil_sys_process_get_pid();
    int status = fossil_sys_process_terminate(pid, 0);
    // Should fail to terminate self (returns -1)
    ASSUME_NOT_EQUAL_I32(status, 0);
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Pool
// * * * * * * * * * * * * * * * * * * * * * * * *
FOSSIL_TEST_GROUP(c_process_tests) {
    FOSSIL_TEST_ADD(c_process_suite, c_test_process_get_pid);
    FOSSIL_TEST_ADD(c_process_suite, c_test_process_get_name);
    FOSSIL_TEST_ADD(c_process_suite, c_test_process_get_info);
    FOSSIL_TEST_ADD(c_process_suite, c_test_process_list);
    FOSSIL_TEST_ADD(c_process_suite, c_test_process_get_environment);
    FOSSIL_TEST_ADD(c_process_suite, c_test_process_terminate_self);

    FOSSIL_TEST_REGISTER(c_process_suite);
}
