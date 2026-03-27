/**
 * -----------------------------------------------------------------------------
 * Project: Fossil Logic
 *
 * This file is part of the Fossil Logic project, which aims to develop
 * high-performance, cross-platform applications and libraries. The code
 * contained herein is licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License. You may obtain
 * a copy of the License at:
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 * Author: Michael Gene Brockus (Dreamer)
 * Date: 04/05/2014
 *
 * Copyright (C) 2014-2025 Fossil Logic. All rights reserved.
 * -----------------------------------------------------------------------------
 */
#include <fossil/pizza/framework.h>

#include "fossil/sys/framework.h"
#include <chrono>

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Utilities
// * * * * * * * * * * * * * * * * * * * * * * * *
// Setup steps for things like test fixtures and
// mock objects are set here.
// * * * * * * * * * * * * * * * * * * * * * * * *

// Define the test suite and add test cases
FOSSIL_SUITE(cpp_syscall_suite);

// Setup function for the test suite
FOSSIL_SETUP(cpp_syscall_suite)
{
    // Setup code here
}

// Teardown function for the test suite
FOSSIL_TEARDOWN(cpp_syscall_suite)
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

FOSSIL_TEST(cpp_test_sys_call_execute)
{
    int result = fossil::sys::Syscall::execute("echo Fossil Logic");
    ASSUME_ITS_TRUE(result == 0);
}

FOSSIL_TEST(cpp_test_sys_call_getpid)
{
    int pid = fossil::sys::Syscall::getpid();
    ASSUME_ITS_TRUE(pid > 0);
}

FOSSIL_TEST(cpp_test_sys_call_create_file)
{
    const std::string filename = "test_file.txt";
    int result = fossil::sys::Syscall::create_file(filename);
    ASSUME_ITS_TRUE(result == 0);
    remove(filename.c_str()); // Clean up the test file
}

FOSSIL_TEST(cpp_test_sys_call_execute_string)
{
    int result = fossil::sys::Syscall::execute("echo Fossil Logic");
    ASSUME_ITS_TRUE(result == 0);
}

FOSSIL_TEST(cpp_test_sys_call_getpid_class)
{
    int pid = fossil::sys::Syscall::getpid();
    ASSUME_ITS_TRUE(pid > 0);
}

FOSSIL_TEST(cpp_test_sys_call_create_file_class)
{
    const std::string filename = "test_file_class.txt";
    int result = fossil::sys::Syscall::create_file(filename);
    ASSUME_ITS_TRUE(result == 0);
    remove(filename.c_str()); // Clean up the test file
}

FOSSIL_TEST(cpp_test_sys_call_create_file_cstr_class)
{
    const char *filename = "test_file_cstr_class.txt";
    int result = fossil::sys::Syscall::create_file(std::string(filename));
    ASSUME_ITS_TRUE(result == 0);
    remove(filename); // Clean up the test file
}

FOSSIL_TEST(cpp_test_sys_call_delete_file)
{
    const std::string filename = "test_delete_file.txt";
    // Create file first
    int create_result = fossil::sys::Syscall::create_file(filename);
    ASSUME_ITS_TRUE(create_result == 0);
    // Delete file
    int delete_result = fossil::sys::Syscall::delete_file(filename);
    ASSUME_ITS_TRUE(delete_result == 0);
}

FOSSIL_TEST(cpp_test_sys_call_file_exists)
{
    const std::string filename = "test_exists_file.txt";
    fossil::sys::Syscall::create_file(filename);
    int exists = fossil::sys::Syscall::file_exists(filename);
    ASSUME_ITS_TRUE(exists == 1);
    fossil::sys::Syscall::delete_file(filename);
    int not_exists = fossil::sys::Syscall::file_exists(filename);
    ASSUME_ITS_TRUE(not_exists == 0);
}

FOSSIL_TEST(cpp_test_sys_call_create_directory)
{
    const std::string dirname = "test_dir";
    int result = fossil::sys::Syscall::create_directory(dirname);
    ASSUME_ITS_TRUE(result == 0);
    fossil::sys::Syscall::delete_directory(dirname, 0);
}

FOSSIL_TEST(cpp_test_sys_call_delete_directory)
{
    const std::string dirname = "test_del_dir";
    fossil::sys::Syscall::create_directory(dirname);
    int result = fossil::sys::Syscall::delete_directory(dirname, 0);
    ASSUME_ITS_TRUE(result == 0);
}

FOSSIL_TEST(cpp_test_sys_call_execute_capture)
{
    std::string buffer(128, '\0');
    int result = fossil::sys::Syscall::execute_capture("echo FossilCapture", &buffer);
    ASSUME_ITS_TRUE(result == 0);
    ASSUME_ITS_TRUE(buffer.find("FossilCapture") != std::string::npos);
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Pool
// * * * * * * * * * * * * * * * * * * * * * * * *

FOSSIL_TEST_GROUP(cpp_syscall_tests)
{
    FOSSIL_TEST_ADD(cpp_syscall_suite, cpp_test_sys_call_execute);
    FOSSIL_TEST_ADD(cpp_syscall_suite, cpp_test_sys_call_getpid);
    FOSSIL_TEST_ADD(cpp_syscall_suite, cpp_test_sys_call_create_file);
    FOSSIL_TEST_ADD(cpp_syscall_suite, cpp_test_sys_call_execute_string);
    FOSSIL_TEST_ADD(cpp_syscall_suite, cpp_test_sys_call_getpid_class);
    FOSSIL_TEST_ADD(cpp_syscall_suite, cpp_test_sys_call_create_file_class);
    FOSSIL_TEST_ADD(cpp_syscall_suite, cpp_test_sys_call_create_file_cstr_class);
    FOSSIL_TEST_ADD(cpp_syscall_suite, cpp_test_sys_call_delete_file);
    FOSSIL_TEST_ADD(cpp_syscall_suite, cpp_test_sys_call_file_exists);
    FOSSIL_TEST_ADD(cpp_syscall_suite, cpp_test_sys_call_create_directory);
    FOSSIL_TEST_ADD(cpp_syscall_suite, cpp_test_sys_call_delete_directory);
    FOSSIL_TEST_ADD(cpp_syscall_suite, cpp_test_sys_call_execute_capture);

    FOSSIL_TEST_REGISTER(cpp_syscall_suite);
}
