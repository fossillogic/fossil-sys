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
FOSSIL_SUITE(c_hostinfo_suite);

// Setup function for the test suite
FOSSIL_SETUP(c_hostinfo_suite)
{
    // Setup code here
}

// Teardown function for the test suite
FOSSIL_TEARDOWN(c_hostinfo_suite)
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

FOSSIL_TEST(c_test_hostinfo_get_system)
{
    fossil_sys_hostinfo_system_t info;
    int result = fossil_sys_hostinfo_get_system(&info);
    ASSUME_ITS_TRUE(result == 0);
    ASSUME_ITS_TRUE(strlen(info.os_name) > 0);
    ASSUME_ITS_TRUE(strlen(info.os_version) > 0);
    ASSUME_ITS_TRUE(strlen(info.kernel_version) > 0);
    ASSUME_ITS_TRUE(strlen(info.hostname) > 0);
    ASSUME_ITS_TRUE(strlen(info.username) > 0);
    ASSUME_ITS_TRUE(strlen(info.machine_type) > 0);
    ASSUME_ITS_TRUE(strlen(info.platform) > 0);
}

FOSSIL_TEST(c_test_hostinfo_get_memory)
{
    fossil_sys_hostinfo_memory_t info;
    int result = fossil_sys_hostinfo_get_memory(&info);
    ASSUME_ITS_TRUE(result == 0);
    ASSUME_ITS_TRUE(info.total_memory > 0);
    ASSUME_ITS_TRUE(info.free_memory <= info.total_memory);
    ASSUME_ITS_TRUE(info.used_memory <= info.total_memory);
    ASSUME_ITS_TRUE(info.available_memory <= info.total_memory);
    ASSUME_ITS_TRUE(info.total_swap >= info.used_swap);
    ASSUME_ITS_TRUE(info.total_swap >= info.free_swap);
}

FOSSIL_TEST(c_test_hostinfo_get_architecture)
{
    fossil_sys_hostinfo_architecture_t info;
    int result = fossil_sys_hostinfo_get_architecture(&info);
    ASSUME_ITS_TRUE(result == 0);
    ASSUME_ITS_TRUE(strlen(info.architecture) > 0);
    ASSUME_ITS_TRUE(strlen(info.cpu) > 0);
    ASSUME_ITS_TRUE(strlen(info.cpu_cores) > 0);
    ASSUME_ITS_TRUE(strlen(info.cpu_threads) > 0);
    ASSUME_ITS_TRUE(strlen(info.cpu_frequency) > 0);
    ASSUME_ITS_TRUE(strlen(info.cpu_architecture) > 0);
}

FOSSIL_TEST(c_test_hostinfo_get_endianness)
{
    fossil_sys_hostinfo_endianness_t info;
    int result = fossil_sys_hostinfo_get_endianness(&info);
    ASSUME_ITS_TRUE(result == 0);
    ASSUME_ITS_TRUE(info.is_little_endian == 0 || info.is_little_endian == 1);
}

FOSSIL_TEST(c_test_hostinfo_get_storage)
{
    fossil_sys_hostinfo_storage_t info;
    int result = fossil_sys_hostinfo_get_storage(&info);
    ASSUME_ITS_TRUE(result == 0);
    ASSUME_ITS_TRUE(strlen(info.device_name) > 0);
    ASSUME_ITS_TRUE(strlen(info.mount_point) > 0);
    ASSUME_ITS_TRUE(info.total_space >= info.used_space);
    ASSUME_ITS_TRUE(info.total_space >= info.free_space);
    ASSUME_ITS_TRUE(strlen(info.filesystem_type) > 0);
}

FOSSIL_TEST(c_test_hostinfo_get_environment)
{
    fossil_sys_hostinfo_environment_t info;
    int result = fossil_sys_hostinfo_get_environment(&info);
    ASSUME_ITS_TRUE(result == 0);
    ASSUME_ITS_TRUE(strlen(info.shell) > 0);
    ASSUME_ITS_TRUE(strlen(info.home_dir) > 0);
    ASSUME_ITS_TRUE(strlen(info.lang) > 0);
    ASSUME_ITS_TRUE(strlen(info.path) > 0);
    ASSUME_ITS_TRUE(strlen(info._term) > 0);
    ASSUME_ITS_TRUE(strlen(info.user) > 0);
}

FOSSIL_TEST(c_test_hostinfo_get_cpu)
{
    fossil_sys_hostinfo_cpu_t info;
    int result = fossil_sys_hostinfo_get_cpu(&info);
    ASSUME_ITS_TRUE(result == 0);
    ASSUME_ITS_TRUE(info.cores > 0);
    ASSUME_ITS_TRUE(info.threads > 0);
    ASSUME_ITS_TRUE(strlen(info.model) > 0);
    ASSUME_ITS_TRUE(strlen(info.vendor) > 0);
    ASSUME_ITS_TRUE(strlen(info.features) > 0);
}

FOSSIL_TEST(c_test_hostinfo_get_gpu)
{
    fossil_sys_hostinfo_gpu_t info;
    int result = fossil_sys_hostinfo_get_gpu(&info);
    ASSUME_ITS_TRUE(result == 0);
    ASSUME_ITS_TRUE(strlen(info.vendor) > 0);
    ASSUME_ITS_TRUE(strlen(info.driver_version) > 0);
    // Memory fields may be zero, but should not be negative (unsigned, so always >= 0)
}

FOSSIL_TEST(c_test_hostinfo_get_power)
{
    fossil_sys_hostinfo_power_t info;
    int result = fossil_sys_hostinfo_get_power(&info);
    ASSUME_ITS_TRUE(result == 0);
    // battery_present: -1 (unknown) or 0/1
    ASSUME_ITS_TRUE(info.battery_present == -1 || info.battery_present == 0 || info.battery_present == 1);
    // battery_charging: -1 (unknown) or 0/1
    ASSUME_ITS_TRUE(info.battery_charging == -1 || info.battery_charging == 0 || info.battery_charging == 1);
    // battery_percentage: -1 (unknown) or 0-100
    ASSUME_ITS_TRUE(info.battery_percentage == -1 || (info.battery_percentage >= 0 && info.battery_percentage <= 100));
    // battery_seconds_left: -1 (unknown) or >= 0
    ASSUME_ITS_TRUE(info.battery_seconds_left == -1 || info.battery_seconds_left >= 0);
}

FOSSIL_TEST(c_test_hostinfo_get_network)
{
    fossil_sys_hostinfo_network_t info;
    int result = fossil_sys_hostinfo_get_network(&info);
    ASSUME_ITS_TRUE(result == 0);
    ASSUME_ITS_TRUE(strlen(info.hostname) > 0);
    ASSUME_ITS_TRUE(strlen(info.primary_ip) > 0);
    ASSUME_ITS_TRUE(strlen(info.mac_address) > 0);
    ASSUME_ITS_TRUE(strlen(info.interface_name) > 0);
    ASSUME_ITS_TRUE(info.is_up == 0 || info.is_up == 1);
}

FOSSIL_TEST(c_test_hostinfo_get_process)
{
    fossil_sys_hostinfo_process_t info;
    int result = fossil_sys_hostinfo_get_process(&info);
    ASSUME_ITS_TRUE(result == 0);
    ASSUME_ITS_TRUE(info.pid > 0);
    ASSUME_ITS_TRUE(info.ppid >= 0);
    ASSUME_ITS_TRUE(strlen(info.executable_path) > 0);
    ASSUME_ITS_TRUE(strlen(info.current_working_dir) > 0);
    ASSUME_ITS_TRUE(strlen(info.process_name) > 0);
    ASSUME_ITS_TRUE(info.is_elevated == 0 || info.is_elevated == 1);
}

FOSSIL_TEST(c_test_hostinfo_get_limits)
{
    fossil_sys_hostinfo_limits_t info;
    int result = fossil_sys_hostinfo_get_limits(&info);
    ASSUME_ITS_TRUE(result == 0);
    ASSUME_ITS_TRUE(info.max_open_files > 0 || info.max_open_files == (uint64_t)-1);
    ASSUME_ITS_TRUE(info.max_processes > 0 || info.max_processes == (uint64_t)-1);
    ASSUME_ITS_TRUE(info.page_size > 0);
}

FOSSIL_TEST(c_test_hostinfo_get_time)
{
    fossil_sys_hostinfo_time_t info;
    int result = fossil_sys_hostinfo_get_time(&info);
    ASSUME_ITS_TRUE(result == 0);
    ASSUME_ITS_TRUE(strlen(info.timezone) > 0);
    ASSUME_ITS_TRUE(strlen(info.locale) > 0);
    // UTC offset can be negative, zero, or positive
    ASSUME_ITS_TRUE(info.utc_offset_seconds >= -43200 && info.utc_offset_seconds <= 50400);
}

FOSSIL_TEST(c_test_hostinfo_get_hardware)
{
    fossil_sys_hostinfo_hardware_t info;
    int result = fossil_sys_hostinfo_get_hardware(&info);
    ASSUME_ITS_TRUE(result == 0);
    ASSUME_ITS_TRUE(strlen(info.manufacturer) > 0);
    ASSUME_ITS_TRUE(strlen(info.product_name) > 0);
    ASSUME_ITS_TRUE(strlen(info.serial_number) > 0);
    ASSUME_ITS_TRUE(strlen(info.bios_version) > 0);
}

FOSSIL_TEST(c_test_hostinfo_get_display)
{
    fossil_sys_hostinfo_display_t info;
    int result = fossil_sys_hostinfo_get_display(&info);
    ASSUME_ITS_TRUE(result == 0);
    ASSUME_ITS_TRUE(info.display_count >= 0);
    ASSUME_ITS_TRUE(info.primary_width >= 0);
    ASSUME_ITS_TRUE(info.primary_height >= 0);
    ASSUME_ITS_TRUE(info.primary_refresh_rate >= 0);
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Pool
// * * * * * * * * * * * * * * * * * * * * * * * *

FOSSIL_TEST_GROUP(c_hostinfo_tests)
{
    FOSSIL_TEST_ADD(c_hostinfo_suite, c_test_hostinfo_get_system);
    FOSSIL_TEST_ADD(c_hostinfo_suite, c_test_hostinfo_get_memory);
    FOSSIL_TEST_ADD(c_hostinfo_suite, c_test_hostinfo_get_architecture);
    FOSSIL_TEST_ADD(c_hostinfo_suite, c_test_hostinfo_get_endianness);
    FOSSIL_TEST_ADD(c_hostinfo_suite, c_test_hostinfo_get_storage);
    FOSSIL_TEST_ADD(c_hostinfo_suite, c_test_hostinfo_get_environment);
    FOSSIL_TEST_ADD(c_hostinfo_suite, c_test_hostinfo_get_cpu);
    FOSSIL_TEST_ADD(c_hostinfo_suite, c_test_hostinfo_get_gpu);
    FOSSIL_TEST_ADD(c_hostinfo_suite, c_test_hostinfo_get_power);
    FOSSIL_TEST_ADD(c_hostinfo_suite, c_test_hostinfo_get_network);
    FOSSIL_TEST_ADD(c_hostinfo_suite, c_test_hostinfo_get_process);
    FOSSIL_TEST_ADD(c_hostinfo_suite, c_test_hostinfo_get_limits);
    FOSSIL_TEST_ADD(c_hostinfo_suite, c_test_hostinfo_get_time);
    FOSSIL_TEST_ADD(c_hostinfo_suite, c_test_hostinfo_get_hardware);
    FOSSIL_TEST_ADD(c_hostinfo_suite, c_test_hostinfo_get_display);

    FOSSIL_TEST_REGISTER(c_hostinfo_suite);
}
