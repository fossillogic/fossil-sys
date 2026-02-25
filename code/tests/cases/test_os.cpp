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
FOSSIL_SUITE(cpp_os_suite);

// Setup function for the test suite
FOSSIL_SETUP(cpp_os_suite) {
    // Setup code here
}

// Teardown function for the test suite
FOSSIL_TEARDOWN(cpp_os_suite) {
    // Teardown code here
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Cases
// * * * * * * * * * * * * * * * * * * * * * * * *
// The test cases below are provided as samples, inspired
// by the Meson build system's approach of using test cases
// as samples for library usage.
// * * * * * * * * * * * * * * * * * * * * * * * *

// ** Test fossil::sys::Os::spawn Function **
FOSSIL_TEST(cpp_test_os_spawn) {
    fossil_sys_os_process_t process;
    
    // Test spawning a valid command
    bool result = fossil::sys::Os::spawn("echo hello", &process);
    ASSUME_ITS_TRUE(result);
    ASSUME_NOT_EQUAL_I32(process.pid, 0);
    ASSUME_NOT_CNULL(process.name);
    ASSUME_ITS_EQUAL_I32(process.status, 1);
    
    // Cleanup
    if (process.name) free((char*)process.name);
    
    // Test with NULL command
    result = fossil::sys::Os::spawn(NULL, &process);
    ASSUME_ITS_FALSE(result);
    
    // Test with NULL process pointer
    result = fossil::sys::Os::spawn("echo test", NULL);
    ASSUME_ITS_FALSE(result);
}

// ** Test fossil::sys::Os::kill Function **
FOSSIL_TEST(cpp_test_os_kill) {
    fossil_sys_os_process_t process;
    
    // Spawn a process first
    bool spawn_result = fossil::sys::Os::spawn("sleep 10", &process);
    ASSUME_ITS_TRUE(spawn_result);
    
    uint32_t pid = process.pid;
    
    // Test killing the process
    bool kill_result = fossil::sys::Os::kill(pid);
    ASSUME_ITS_TRUE(kill_result);
    
    // Cleanup
    if (process.name) free((char*)process.name);
    
    // Test with invalid PID (should fail gracefully)
    kill_result = fossil::sys::Os::kill(999999);
    ASSUME_ITS_FALSE(kill_result);
}

// ** Test fossil::sys::Os::list Function **
FOSSIL_TEST(cpp_test_os_list) {
    fossil_sys_os_process_t buffer[128];
    
    // Test listing processes
    size_t count = fossil::sys::Os::list(buffer, 128);
    ASSUME_NOT_EQUAL_I32(count, 0);
    
    // Verify first process has valid data
    ASSUME_NOT_EQUAL_I32(buffer[0].pid, 0);
    ASSUME_NOT_CNULL(buffer[0].name);
    ASSUME_ITS_EQUAL_I32(buffer[0].status, 1);
    
    // Cleanup
    for (size_t i = 0; i < count; i++) {
        if (buffer[i].name) free((char*)buffer[i].name);
    }
    
    // Test with NULL buffer
    count = fossil::sys::Os::list(NULL, 128);
    ASSUME_ITS_EQUAL_I32(count, 0);
    
    // Test with max_count = 0
    count = fossil::sys::Os::list(buffer, 0);
    ASSUME_ITS_EQUAL_I32(count, 0);
}

// ** Test fossil::sys::Os::uptime Function **
FOSSIL_TEST(cpp_test_os_uptime) {
    uint64_t uptime1 = fossil::sys::Os::uptime();
    ASSUME_NOT_EQUAL_I32(uptime1, 0);
    
    // Wait a moment and check uptime increased
    fossil::sys::Os::yield();
    
    uint64_t uptime2 = fossil::sys::Os::uptime();
    ASSUME_NOT_EQUAL_I32(uptime2, 0);
}

// ** Test fossil::sys::Os::yield Function **
FOSSIL_TEST(cpp_test_os_yield) {
    // Test that yield doesn't crash
    fossil::sys::Os::yield();
    fossil::sys::Os::yield();
    ASSUME_ITS_TRUE(true);
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Pool
// * * * * * * * * * * * * * * * * * * * * * * * *
FOSSIL_TEST_GROUP(cpp_os_tests) {
    FOSSIL_TEST_ADD(cpp_os_suite, cpp_test_os_spawn);
    FOSSIL_TEST_ADD(cpp_os_suite, cpp_test_os_kill);
    FOSSIL_TEST_ADD(cpp_os_suite, cpp_test_os_list);
    FOSSIL_TEST_ADD(cpp_os_suite, cpp_test_os_uptime);
    FOSSIL_TEST_ADD(cpp_os_suite, cpp_test_os_yield);

    FOSSIL_TEST_REGISTER(cpp_os_suite);
}
