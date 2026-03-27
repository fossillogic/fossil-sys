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

#ifndef cnull
#error "cnull is not defined."
#endif

#ifndef cnullptr
#error "cnullptr is not defined."
#endif

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Utilities
// * * * * * * * * * * * * * * * * * * * * * * * *
// Setup steps for things like test fixtures and
// mock objects are set here.
// * * * * * * * * * * * * * * * * * * * * * * * *

// Define the test suite and add test cases
FOSSIL_SUITE(cpp_null_suite);

// Setup function for the test suite
FOSSIL_SETUP(cpp_null_suite)
{
    // Setup code if needed
}

// Teardown function for the test suite
FOSSIL_TEARDOWN(cpp_null_suite)
{
    // Cleanup if needed
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Cases
// * * * * * * * * * * * * * * * * * * * * * * * *
// The test cases below are provided as samples, inspired
// by the Meson build system's approach of using test cases
// as samples for library usage.
// * * * * * * * * * * * * * * * * * * * * * * * *

// ** Test csafe_cast Macro **
FOSSIL_TEST(cpp_test_csafe_cast)
{
    void *ptr = reinterpret_cast<void *>(1);   // Use safe_cast instead of C cast
    int *casted_ptr = static_cast<int *>(ptr); // Using the safe_cast macro
    ASSUME_ITS_EQUAL_PTR(casted_ptr, reinterpret_cast<int *>(ptr));

    // Now let's handle cnull scenario
    cnullify(ptr);
    casted_ptr = static_cast<int *>(ptr); // Should return cnull when the input is cnull
    ASSUME_ITS_EQUAL_PTR(casted_ptr, cnull);
}

// ** Test cnullify Macro **
FOSSIL_TEST(cpp_test_cnullify)
{
    void *ptr = reinterpret_cast<void *>(1);
    cnullify(ptr);
    ASSUME_ITS_EQUAL_PTR(ptr, cnull);
}

// ** Test cnotnull Macro **
FOSSIL_TEST(cpp_test_cnotnull)
{
    void *ptr = reinterpret_cast<void *>(1);
    ASSUME_ITS_TRUE(cnotnull(ptr));
    cnullify(ptr);
    ASSUME_ITS_FALSE(cnotnull(ptr));
}

// ** Test cunwrap_or Macro **
FOSSIL_TEST(cpp_test_cunwrap_or)
{
    void *ptr = reinterpret_cast<void *>(1);
    void *default_ptr = reinterpret_cast<void *>(99);
    ASSUME_ITS_EQUAL_PTR(cunwrap_or(ptr, default_ptr), ptr);
    cnullify(ptr);
    ASSUME_ITS_EQUAL_PTR(cunwrap_or(ptr, default_ptr), default_ptr);
}

// ** Test cunwrap Macro **
FOSSIL_TEST(cpp_test_cunwrap)
{
    void *ptr = reinterpret_cast<void *>(1);
    ASSUME_ITS_EQUAL_PTR(cunwrap(ptr), ptr);
    // cnullify(ptr);
    // ASSUME_ITS_EQUAL_PTR(cunwrap(ptr), cnull);  // Should return cnull when it's cnull
}

// ** Test cnullable and cnonnull Attributes **
FOSSIL_TEST(cpp_test_nullable_nonnull)
{
    void *ptr = cnull;
    ASSUME_ITS_TRUE(cnotnull(ptr) == 0);
    ptr = reinterpret_cast<void *>(1);
    ASSUME_ITS_TRUE(cnotnull(ptr) == 1);
}

// ** Test coptional Macro and cnone() and csome() **
FOSSIL_TEST(cpp_test_coptional)
{
    void *ptr = reinterpret_cast<void *>(1);
    ASSUME_ITS_EQUAL_PTR(c_optional(ptr), ptr);
    cnullify(ptr);
    ASSUME_ITS_EQUAL_PTR(c_optional(ptr), cnull);
}

// ** Test COption structure and cunwrap_option Macro **
FOSSIL_TEST(cpp_test_cunwrap_option)
{
    COption some_option = csome(reinterpret_cast<void *>(1));
    COption none_option = cnone();
    ASSUME_ITS_EQUAL_PTR(cunwrap_option(some_option), reinterpret_cast<void *>(1));

    // Uncommenting the following line will trigger panic due to unwrapping a None value
    // ASSUME_ITS_EQUAL_PTR(cunwrap_option(none_option), cnull); // Should panic in actual code
    cunused(none_option); // avoid unused error
}

// ** Test cunwrap_or_option Macro **
FOSSIL_TEST(cpp_test_cunwrap_or_option)
{
    COption some_option = csome(reinterpret_cast<void *>(1));
    COption none_option = cnone();
    ASSUME_ITS_EQUAL_PTR(cunwrap_or_option(some_option, reinterpret_cast<void *>(99)), reinterpret_cast<void *>(1));
    ASSUME_ITS_EQUAL_PTR(cunwrap_or_option(none_option, reinterpret_cast<void *>(99)), reinterpret_cast<void *>(99));
}

// ** Test cdrop Macro **
FOSSIL_TEST(cpp_test_cdrop)
{
    void *ptr = reinterpret_cast<void *>(1);
    cdrop(ptr);
    ASSUME_ITS_EQUAL_PTR(ptr, cnull);
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Pool
// * * * * * * * * * * * * * * * * * * * * * * * *

FOSSIL_TEST_GROUP(cpp_nullptr_tests)
{
    FOSSIL_TEST_ADD(cpp_null_suite, cpp_test_csafe_cast);
    FOSSIL_TEST_ADD(cpp_null_suite, cpp_test_cnullify);
    FOSSIL_TEST_ADD(cpp_null_suite, cpp_test_cnotnull);
    FOSSIL_TEST_ADD(cpp_null_suite, cpp_test_cunwrap_or);
    FOSSIL_TEST_ADD(cpp_null_suite, cpp_test_cunwrap);
    FOSSIL_TEST_ADD(cpp_null_suite, cpp_test_nullable_nonnull);
    FOSSIL_TEST_ADD(cpp_null_suite, cpp_test_coptional);
    FOSSIL_TEST_ADD(cpp_null_suite, cpp_test_cunwrap_option);
    FOSSIL_TEST_ADD(cpp_null_suite, cpp_test_cunwrap_or_option);
    FOSSIL_TEST_ADD(cpp_null_suite, cpp_test_cdrop);
    FOSSIL_TEST_ADD(cpp_null_suite, cpp_test_cnotnull);
    FOSSIL_TEST_ADD(cpp_null_suite, cpp_test_nullable_nonnull);
    FOSSIL_TEST_ADD(cpp_null_suite, cpp_test_coptional);
    FOSSIL_TEST_ADD(cpp_null_suite, cpp_test_cunwrap_option);
    FOSSIL_TEST_ADD(cpp_null_suite, cpp_test_cunwrap_or_option);
    FOSSIL_TEST_ADD(cpp_null_suite, cpp_test_cdrop);

    FOSSIL_TEST_REGISTER(cpp_null_suite);
}
