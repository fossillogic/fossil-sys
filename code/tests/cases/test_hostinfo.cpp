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

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Utilities
// * * * * * * * * * * * * * * * * * * * * * * * *
// Setup steps for things like test fixtures and
// mock objects are set here.
// * * * * * * * * * * * * * * * * * * * * * * * *

// Define the test suite and add test cases
FOSSIL_SUITE(cpp_hostinfo_suite);

// Setup function for the test suite
FOSSIL_SETUP(cpp_hostinfo_suite)
{
    // Setup code here
}

// Teardown function for the test suite
FOSSIL_TEARDOWN(cpp_hostinfo_suite)
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

FOSSIL_TEST(cpp_test_hostinfo_get_system)
{
    fossil_sys_hostinfo_system_t info;
    int result = fossil_sys_hostinfo_get_system(&info);
    ASSUME_ITS_TRUE(result == 0);
    ASSUME_ITS_TRUE(strlen(info.os_name) > 0);
    ASSUME_ITS_TRUE(strlen(info.os_version) > 0);
    ASSUME_ITS_TRUE(strlen(info.kernel_version) > 0);
}

FOSSIL_TEST(cpp_test_hostinfo_get_memory)
{
    fossil_sys_hostinfo_memory_t info;
    int result = fossil_sys_hostinfo_get_memory(&info);
    ASSUME_ITS_TRUE(result == 0);
    ASSUME_ITS_TRUE(info.total_memory > 0);
    ASSUME_ITS_TRUE(info.free_memory <= info.total_memory);
}

FOSSIL_TEST(cpp_test_hostinfo_get_endianness)
{
    fossil_sys_hostinfo_endianness_t info;
    int result = fossil_sys_hostinfo_get_endianness(&info);
    ASSUME_ITS_TRUE(result == 0);
    ASSUME_ITS_TRUE(info.is_little_endian == 0 || info.is_little_endian == 1);
}

FOSSIL_TEST(cpp_test_hostinfo_class_get_system)
{
    fossil::sys::Hostinfo hostinfo;
    fossil_sys_hostinfo_system_t info = hostinfo.get_system();
    ASSUME_ITS_TRUE(strlen(info.os_name) > 0);
    ASSUME_ITS_TRUE(strlen(info.os_version) > 0);
    ASSUME_ITS_TRUE(strlen(info.kernel_version) > 0);
}

FOSSIL_TEST(cpp_test_hostinfo_class_get_memory)
{
    fossil::sys::Hostinfo hostinfo;
    fossil_sys_hostinfo_memory_t info = hostinfo.get_memory();
    ASSUME_ITS_TRUE(info.total_memory > 0);
    ASSUME_ITS_TRUE(info.free_memory <= info.total_memory);
}

FOSSIL_TEST(cpp_test_hostinfo_class_get_endianness)
{
    fossil::sys::Hostinfo hostinfo;
    fossil_sys_hostinfo_endianness_t info = hostinfo.get_endianness();
    ASSUME_ITS_TRUE(info.is_little_endian == 0 || info.is_little_endian == 1);
}

FOSSIL_TEST(cpp_test_hostinfo_class_get_architecture)
{
    fossil::sys::Hostinfo hostinfo;
    fossil_sys_hostinfo_architecture_t info = hostinfo.get_architecture();
    ASSUME_ITS_TRUE(strlen(info.architecture) > 0);
    ASSUME_ITS_TRUE(strlen(info.cpu) > 0);
    ASSUME_ITS_TRUE(strlen(info.cpu_cores) > 0);
    ASSUME_ITS_TRUE(strlen(info.cpu_threads) > 0);
    ASSUME_ITS_TRUE(strlen(info.cpu_frequency) > 0);
    ASSUME_ITS_TRUE(strlen(info.cpu_architecture) > 0);
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Pool
// * * * * * * * * * * * * * * * * * * * * * * * *

FOSSIL_TEST_GROUP(cpp_hostinfo_tests)
{
    FOSSIL_TEST_ADD(cpp_hostinfo_suite, cpp_test_hostinfo_get_system);
    FOSSIL_TEST_ADD(cpp_hostinfo_suite, cpp_test_hostinfo_get_memory);
    FOSSIL_TEST_ADD(cpp_hostinfo_suite, cpp_test_hostinfo_get_endianness);

    FOSSIL_TEST_ADD(cpp_hostinfo_suite, cpp_test_hostinfo_class_get_system);
    FOSSIL_TEST_ADD(cpp_hostinfo_suite, cpp_test_hostinfo_class_get_memory);
    FOSSIL_TEST_ADD(cpp_hostinfo_suite, cpp_test_hostinfo_class_get_endianness);
    FOSSIL_TEST_ADD(cpp_hostinfo_suite, cpp_test_hostinfo_class_get_architecture);

    FOSSIL_TEST_REGISTER(cpp_hostinfo_suite);
}
