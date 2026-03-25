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

// ** Test Process::exists **
FOSSIL_TEST(cpp_test_process_exists_cpp) {
    uint32_t pid = fossil::sys::Process::get_pid();
    int exists = fossil::sys::Process::exists(pid);
    ASSUME_ITS_EQUAL_I32(exists, 1);

    exists = fossil::sys::Process::exists(0);
    ASSUME_ITS_EQUAL_I32(exists, 0);
}

// ** Test Process::suspend and Process::resume (self, should fail) **
FOSSIL_TEST(cpp_test_process_suspend_resume_self_cpp) {
    uint32_t pid = fossil::sys::Process::get_pid();
    int status = fossil::sys::Process::suspend(pid);
    ASSUME_NOT_EQUAL_I32(status, 0);

    status = fossil::sys::Process::resume(pid);
    ASSUME_NOT_EQUAL_I32(status, 0);
}

// ** Test Process::set_priority and Process::get_priority **
FOSSIL_TEST(cpp_test_process_set_get_priority_cpp) {
    uint32_t pid = fossil::sys::Process::get_pid();
    int orig_priority = 0;
    int status = fossil::sys::Process::get_priority(pid, orig_priority);
    ASSUME_ITS_TRUE(status == 0 || status == -1 || status == -2 || status == -3);

    if (status == 0) {
        int set_status = fossil::sys::Process::set_priority(pid, orig_priority);
        ASSUME_ITS_TRUE(set_status == 0 || set_status == -1 || set_status == -2);

        int new_priority = 0;
        int get_status = fossil::sys::Process::get_priority(pid, new_priority);
        ASSUME_ITS_TRUE(get_status == 0 || get_status == -1 || get_status == -2 || get_status == -3);
    }
}

// ** Test Process::wait (self, should fail or timeout) **
FOSSIL_TEST(cpp_test_process_wait_self_cpp) {
    uint32_t pid = fossil::sys::Process::get_pid();
    int exit_code = 0;
    int status = fossil::sys::Process::wait(pid, &exit_code, 100);
    ASSUME_NOT_EQUAL_I32(status, 0);
}

// ** Test Process::spawn, Process::wait, and Process::terminate **
FOSSIL_TEST(cpp_test_process_spawn_wait_terminate_cpp) {
#if defined(__linux__) || defined(__APPLE__)
    char *const argv[] = { (char*)"/bin/echo", (char*)"fossil_test", NULL };
    uint32_t child_pid = 0;
    int spawn_status = fossil::sys::Process::spawn("/bin/echo", argv, NULL, child_pid);
    ASSUME_ITS_EQUAL_I32(spawn_status, 0);
    ASSUME_ITS_TRUE(child_pid > 0);

    int exit_code = -1;
    int wait_status = fossil::sys::Process::wait(child_pid, &exit_code, 2000);
    ASSUME_ITS_TRUE(wait_status == 0 || wait_status == -2);
#elif defined(_WIN32)
    char *const argv[] = { (char*)"cmd.exe", (char*)"/C", (char*)"echo", (char*)"fossil_test", NULL };
    uint32_t child_pid = 0;
    int spawn_status = fossil::sys::Process::spawn("C:\\Windows\\System32\\cmd.exe", argv, NULL, child_pid);
    ASSUME_ITS_EQUAL_I32(spawn_status, 0);
    ASSUME_ITS_TRUE(child_pid > 0);

    int exit_code = -1;
    int wait_status = fossil::sys::Process::wait(child_pid, &exit_code, 2000);
    ASSUME_ITS_TRUE(wait_status == 0 || wait_status == -2);
#else
    ASSUME_ITS_TRUE(1);
#endif
}

// ** Test Process::get_exe_path **
FOSSIL_TEST(cpp_test_process_get_exe_path_cpp) {
    uint32_t pid = fossil::sys::Process::get_pid();
    std::string exe_path;
    int status = fossil::sys::Process::get_exe_path(pid, exe_path);
    ASSUME_ITS_TRUE(status == 0 || status == -2 || status == -3);
    if (status == 0) {
        ASSUME_ITS_TRUE(!exe_path.empty());
    }
}

/* C API tests (original) */

// ** Test fossil_sys_process_exists **
FOSSIL_TEST(cpp_test_process_exists) {
    uint32_t pid = fossil_sys_process_get_pid();
    int exists = fossil_sys_process_exists(pid);
    ASSUME_ITS_EQUAL_I32(exists, 1);

    exists = fossil_sys_process_exists(0);
    ASSUME_ITS_EQUAL_I32(exists, 0);
}

// ** Test fossil_sys_process_suspend and fossil_sys_process_resume (self, should fail) **
FOSSIL_TEST(cpp_test_process_suspend_resume_self) {
    uint32_t pid = fossil_sys_process_get_pid();
    int status = fossil_sys_process_suspend(pid);
    ASSUME_NOT_EQUAL_I32(status, 0);

    status = fossil_sys_process_resume(pid);
    ASSUME_NOT_EQUAL_I32(status, 0);
}

// ** Test fossil_sys_process_set_priority and fossil_sys_process_get_priority **
FOSSIL_TEST(cpp_test_process_set_get_priority) {
    uint32_t pid = fossil_sys_process_get_pid();
    int orig_priority = 0;
    int status = fossil_sys_process_get_priority(pid, &orig_priority);
    ASSUME_ITS_TRUE(status == 0 || status == -1 || status == -2 || status == -3);

    // Only try to set if get succeeded
    if (status == 0) {
        int set_status = fossil_sys_process_set_priority(pid, orig_priority);
        ASSUME_ITS_TRUE(set_status == 0 || set_status == -1 || set_status == -2);

        int new_priority = 0;
        int get_status = fossil_sys_process_get_priority(pid, &new_priority);
        ASSUME_ITS_TRUE(get_status == 0 || get_status == -1 || get_status == -2 || get_status == -3);
    }
}

// ** Test fossil_sys_process_wait (self, should fail or timeout) **
FOSSIL_TEST(cpp_test_process_wait_self) {
    uint32_t pid = fossil_sys_process_get_pid();
    int exit_code = 0;
    int status = fossil_sys_process_wait(pid, &exit_code, 100);
    ASSUME_NOT_EQUAL_I32(status, 0);
}

// ** Test fossil_sys_process_spawn, fossil_sys_process_wait, and fossil_sys_process_terminate **
FOSSIL_TEST(cpp_test_process_spawn_wait_terminate) {
#if defined(__linux__) || defined(__APPLE__)
    char *const argv[] = { (char*)"/bin/echo", (char*)"fossil_test", NULL };
    uint32_t child_pid = 0;
    int spawn_status = fossil_sys_process_spawn("/bin/echo", argv, NULL, &child_pid);
    ASSUME_ITS_EQUAL_I32(spawn_status, 0);
    ASSUME_ITS_TRUE(child_pid > 0);

    int exit_code = -1;
    int wait_status = fossil_sys_process_wait(child_pid, &exit_code, 2000);
    ASSUME_ITS_TRUE(wait_status == 0 || wait_status == -2);
#elif defined(_WIN32)
    char *const argv[] = {"cmd.exe", "/C", "echo", "fossil_test", NULL};
    uint32_t child_pid = 0;
    int spawn_status = fossil_sys_process_spawn("C:\\Windows\\System32\\cmd.exe", argv, NULL, &child_pid);
    ASSUME_ITS_EQUAL_I32(spawn_status, 0);
    ASSUME_ITS_TRUE(child_pid > 0);

    int exit_code = -1;
    int wait_status = fossil_sys_process_wait(child_pid, &exit_code, 2000);
    ASSUME_ITS_TRUE(wait_status == 0 || wait_status == -2);
#else
    // Not supported
    ASSUME_ITS_TRUE(1);
#endif
}

// ** Test fossil_sys_process_get_exe_path **
FOSSIL_TEST(cpp_test_process_get_exe_path) {
    uint32_t pid = fossil_sys_process_get_pid();
    char buffer[512] = {0};
    int status = fossil_sys_process_get_exe_path(pid, buffer, sizeof(buffer));
    ASSUME_ITS_TRUE(status == 0 || status == -2 || status == -3);
    if (status == 0) {
        ASSUME_ITS_TRUE(strlen(buffer) > 0);
    }
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
    FOSSIL_TEST_ADD(cpp_process_suite, cpp_test_process_exists_cpp);
    FOSSIL_TEST_ADD(cpp_process_suite, cpp_test_process_suspend_resume_self_cpp);
    FOSSIL_TEST_ADD(cpp_process_suite, cpp_test_process_set_get_priority_cpp);
    FOSSIL_TEST_ADD(cpp_process_suite, cpp_test_process_wait_self_cpp);
    FOSSIL_TEST_ADD(cpp_process_suite, cpp_test_process_spawn_wait_terminate_cpp);
    FOSSIL_TEST_ADD(cpp_process_suite, cpp_test_process_get_exe_path_cpp);
    FOSSIL_TEST_ADD(cpp_process_suite, cpp_test_process_exists);
    FOSSIL_TEST_ADD(cpp_process_suite, cpp_test_process_suspend_resume_self);
    FOSSIL_TEST_ADD(cpp_process_suite, cpp_test_process_set_get_priority);
    FOSSIL_TEST_ADD(cpp_process_suite, cpp_test_process_wait_self);
    FOSSIL_TEST_ADD(cpp_process_suite, cpp_test_process_spawn_wait_terminate);
    FOSSIL_TEST_ADD(cpp_process_suite, cpp_test_process_get_exe_path);

    FOSSIL_TEST_REGISTER(cpp_process_suite);
}
