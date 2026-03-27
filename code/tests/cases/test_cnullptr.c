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
FOSSIL_SUITE(c_null_suite);

// Setup function for the test suite
FOSSIL_SETUP(c_null_suite)
{
    // Setup code here
}

// Teardown function for the test suite
FOSSIL_TEARDOWN(c_null_suite)
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

// Ensure the necessary macros and types are defined
FOSSIL_TEST(c_test_cnull_definition)
{
    ASSUME_ITS_EQUAL_PTR(cnull, (void *)(0));
    ASSUME_ITS_EQUAL_PTR(cnullptr, (void *)(0));
}

// ** Test cnullify Macro **
FOSSIL_TEST(c_test_cnullify)
{
    void *ptr = (void *)1;
    cnullify(ptr);
    ASSUME_ITS_EQUAL_PTR(ptr, cnull);
}

// ** Test cnotnull Macro **
FOSSIL_TEST(c_test_cnotnull)
{
    void *ptr = (void *)1;
    ASSUME_ITS_TRUE(cnotnull(ptr));
    cnullify(ptr);
    ASSUME_ITS_FALSE(cnotnull(ptr));
}

// ** Test cunwrap_or Macro **
FOSSIL_TEST(c_test_cunwrap_or)
{
    void *ptr = (void *)1;
    void *default_ptr = (void *)99;
    ASSUME_ITS_EQUAL_PTR(cunwrap_or(ptr, default_ptr), ptr);
    cnullify(ptr);
    ASSUME_ITS_EQUAL_PTR(cunwrap_or(ptr, default_ptr), default_ptr);
}

// ** Test cunwrap Macro **
FOSSIL_TEST(c_test_cunwrap)
{
    void *ptr = (void *)1;
    ASSUME_ITS_EQUAL_PTR(cunwrap(ptr), ptr);
    // cnullify(ptr);
    // ASSUME_ITS_EQUAL_PTR(cunwrap(ptr), cnull);  // Should exit on failure in actual code, but we test that it returns cnull
}

// ** Test csafe_cast Macro **
FOSSIL_TEST(c_test_csafe_cast)
{
    void *ptr = (void *)1;
    int *casted_ptr = csafe_cast(int *, ptr);
    ASSUME_ITS_EQUAL_PTR(casted_ptr, (int *)ptr);
    cnullify(ptr);
    casted_ptr = csafe_cast(int *, ptr);
    ASSUME_ITS_EQUAL_PTR(casted_ptr, cnull); // Should return cnull when the input is cnull
}

// ** Test cnullable and cnonnull Attributes **
FOSSIL_TEST(c_test_nullable_nonnull)
{
    // These tests are intended to verify that nullable and nonnull attributes are handled by the compiler, so the actual test might be compiler-specific.
    void *ptr = cnull;
    ASSUME_ITS_TRUE(cnotnull(ptr) == 0);
    ptr = (void *)1;
    ASSUME_ITS_TRUE(cnotnull(ptr) == 1);
}

// ** Test coptional Macro and cnone() and csome() **
FOSSIL_TEST(c_test_coptional)
{
    void *ptr = (void *)1;
    ASSUME_ITS_EQUAL_PTR(c_optional(ptr), ptr);
    cnullify(ptr);
    ASSUME_ITS_EQUAL_PTR(c_optional(ptr), cnull);
}

// ** Test COption structure and cunwrap_option Macro **
FOSSIL_TEST(c_test_cunwrap_option)
{
    COption some_option = csome((void *)1);
    COption none_option = cnone();
    ASSUME_ITS_EQUAL_PTR(cunwrap_option(some_option), (void *)1);

    // Uncommenting the following line will trigger panic due to unwrapping a None value
    // ASSUME_ITS_EQUAL_PTR(cunwrap_option(none_option), cnull); // Should panic in actual code
    cunused(none_option); // avoid unused error
}

// ** Test cunwrap_or_option Macro **
FOSSIL_TEST(c_test_cunwrap_or_option)
{
    COption some_option = csome((void *)1);
    COption none_option = cnone();
    ASSUME_ITS_EQUAL_PTR(cunwrap_or_option(some_option, (void *)99), (void *)1);
    ASSUME_ITS_EQUAL_PTR(cunwrap_or_option(none_option, (void *)99), (void *)99);
}

// ** Test cdrop Macro **
FOSSIL_TEST(c_test_cdrop)
{
    void *ptr = (void *)1;
    cdrop(ptr);
    ASSUME_ITS_EQUAL_PTR(ptr, cnull);
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Pool
// * * * * * * * * * * * * * * * * * * * * * * * *
FOSSIL_TEST_GROUP(c_nullptr_tests)
{
    FOSSIL_TEST_ADD(c_null_suite, c_test_cnull_definition);
    FOSSIL_TEST_ADD(c_null_suite, c_test_cnullify);
    FOSSIL_TEST_ADD(c_null_suite, c_test_cnotnull);
    FOSSIL_TEST_ADD(c_null_suite, c_test_cunwrap_or);
    FOSSIL_TEST_ADD(c_null_suite, c_test_cunwrap);
    FOSSIL_TEST_ADD(c_null_suite, c_test_csafe_cast);
    FOSSIL_TEST_ADD(c_null_suite, c_test_nullable_nonnull);
    FOSSIL_TEST_ADD(c_null_suite, c_test_coptional);
    FOSSIL_TEST_ADD(c_null_suite, c_test_cunwrap_option);
    FOSSIL_TEST_ADD(c_null_suite, c_test_cunwrap_or_option);
    FOSSIL_TEST_ADD(c_null_suite, c_test_cdrop);

    FOSSIL_TEST_REGISTER(c_null_suite);
}
