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
FOSSIL_SUITE(cpp_event_suite);

// Setup function for the test suite
FOSSIL_SETUP(cpp_event_suite) {
    // Setup code here
}

// Teardown function for the test suite
FOSSIL_TEARDOWN(cpp_event_suite) {
    // Teardown code here
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Cases
// * * * * * * * * * * * * * * * * * * * * * * * *
// The test cases below are provided as samples, inspired
// by the Meson build system's approach of using test cases
// as samples for library usage.
// * * * * * * * * * * * * * * * * * * * * * * * *

// ** Test fossil::sys::Event::init Function **
FOSSIL_TEST(cpp_test_event_poll) {
    fossil::sys::Event::init();
    
    fossil_sys_event_t event;
    int status = fossil::sys::Event::poll(&event);
    ASSUME_ITS_EQUAL_I32(status, 0); // no events
    
    fossil::sys::Event::post("test_event", NULL, 0);
    status = fossil::sys::Event::poll(&event);
    ASSUME_ITS_EQUAL_I32(status, 1); // event retrieved
    ASSUME_ITS_EQUAL_CSTR(event.id, "test_event");
    
    status = fossil::sys::Event::poll(NULL);
    ASSUME_NOT_EQUAL_I32(status, 0); // null pointer error
    
    fossil::sys::Event::shutdown();
}

// ** Test fossil::sys::Event::post Function **
FOSSIL_TEST(cpp_test_event_post) {
    fossil::sys::Event::init();
    
    int status = fossil::sys::Event::post("event1", NULL, 0);
    ASSUME_ITS_EQUAL_I32(status, 0);
    
    char payload[] = "data";
    status = fossil::sys::Event::post("event2", payload, sizeof(payload));
    ASSUME_ITS_EQUAL_I32(status, 0);
    
    fossil_sys_event_t event;
    fossil::sys::Event::poll(&event);
    ASSUME_ITS_EQUAL_CSTR(event.id, "event1");
    
    fossil::sys::Event::shutdown();
}

// ** Test fossil::sys::Event::wait Function **
FOSSIL_TEST(cpp_test_event_wait) {
    fossil::sys::Event::init();
    
    fossil::sys::Event::post("wait_event", NULL, 0);
    fossil_sys_event_t event;
    int status = fossil::sys::Event::wait(&event, 1000);
    ASSUME_ITS_EQUAL_I32(status, 1);
    
    status = fossil::sys::Event::wait(&event, 100);
    ASSUME_ITS_EQUAL_I32(status, 0); // timeout
    
    status = fossil::sys::Event::wait(NULL, 100);
    ASSUME_NOT_EQUAL_I32(status, 0); // null pointer error
    
    fossil::sys::Event::shutdown();
}

// ** Test fossil::sys::Event::shutdown Function **
FOSSIL_TEST(cpp_test_event_shutdown) {
    fossil::sys::Event::init();
    fossil::sys::Event::post("shutdown_event", NULL, 0);
    fossil::sys::Event::shutdown();
    
    fossil_sys_event_t event;
    int status = fossil::sys::Event::poll(&event);
    ASSUME_ITS_EQUAL_I32(status, 0);
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Pool
// * * * * * * * * * * * * * * * * * * * * * * * *
FOSSIL_TEST_GROUP(cpp_event_tests) {
    FOSSIL_TEST_ADD(cpp_event_suite, cpp_test_event_poll);
    FOSSIL_TEST_ADD(cpp_event_suite, cpp_test_event_post);
    FOSSIL_TEST_ADD(cpp_event_suite, cpp_test_event_wait);
    FOSSIL_TEST_ADD(cpp_event_suite, cpp_test_event_shutdown);

    FOSSIL_TEST_REGISTER(cpp_event_suite);
}
