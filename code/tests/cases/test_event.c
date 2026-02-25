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
FOSSIL_SUITE(c_event_suite);

// Setup function for the test suite
FOSSIL_SETUP(c_event_suite) {
    // Setup code here
}

// Teardown function for the test suite
FOSSIL_TEARDOWN(c_event_suite) {
    // Teardown code here
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Cases
// * * * * * * * * * * * * * * * * * * * * * * * *
// The test cases below are provided as samples, inspired
// by the Meson build system's approach of using test cases
// as samples for library usage.
// * * * * * * * * * * * * * * * * * * * * * * * *

// ** Test fossil_sys_event_poll Function **
FOSSIL_TEST(c_test_event_poll) {
    fossil_sys_event_init();
    
    fossil_sys_event_t event;
    int status = fossil_sys_event_poll(&event);
    ASSUME_ITS_EQUAL_I32(status, 0); // no events
    
    fossil_sys_event_post("test_event", NULL, 0);
    status = fossil_sys_event_poll(&event);
    ASSUME_ITS_EQUAL_I32(status, 1); // event retrieved
    ASSUME_ITS_EQUAL_CSTR(event.id, "test_event");
    
    status = fossil_sys_event_poll(NULL);
    ASSUME_NOT_EQUAL_I32(status, 0); // null pointer error
    
    fossil_sys_event_shutdown();
}

// ** Test fossil_sys_event_post Function **
FOSSIL_TEST(c_test_event_post) {
    fossil_sys_event_init();
    
    int status = fossil_sys_event_post("event1", NULL, 0);
    ASSUME_ITS_EQUAL_I32(status, 0);
    
    char payload[] = "data";
    status = fossil_sys_event_post("event2", payload, sizeof(payload));
    ASSUME_ITS_EQUAL_I32(status, 0);
    
    fossil_sys_event_t event;
    fossil_sys_event_poll(&event);
    ASSUME_ITS_EQUAL_CSTR(event.id, "event1");
    
    fossil_sys_event_shutdown();
}

// ** Test fossil_sys_event_wait Function **
FOSSIL_TEST(c_test_event_wait) {
    fossil_sys_event_init();
    
    fossil_sys_event_post("wait_event", NULL, 0);
    fossil_sys_event_t event;
    int status = fossil_sys_event_wait(&event, 1000);
    ASSUME_ITS_EQUAL_I32(status, 1);
    
    status = fossil_sys_event_wait(&event, 100);
    ASSUME_ITS_EQUAL_I32(status, 0); // timeout
    
    status = fossil_sys_event_wait(NULL, 100);
    ASSUME_NOT_EQUAL_I32(status, 0); // null pointer error
    
    fossil_sys_event_shutdown();
}

// ** Test fossil_sys_event_shutdown Function **
FOSSIL_TEST(c_test_event_shutdown) {
    fossil_sys_event_init();
    fossil_sys_event_post("shutdown_event", NULL, 0);
    fossil_sys_event_shutdown();
    
    fossil_sys_event_t event;
    int status = fossil_sys_event_poll(&event);
    ASSUME_ITS_EQUAL_I32(status, 0);
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Pool
// * * * * * * * * * * * * * * * * * * * * * * * *
FOSSIL_TEST_GROUP(c_event_tests) {
    FOSSIL_TEST_ADD(c_event_suite, c_test_event_poll);
    FOSSIL_TEST_ADD(c_event_suite, c_test_event_post);
    FOSSIL_TEST_ADD(c_event_suite, c_test_event_wait);
    FOSSIL_TEST_ADD(c_event_suite, c_test_event_shutdown);

    FOSSIL_TEST_REGISTER(c_event_suite);
}
