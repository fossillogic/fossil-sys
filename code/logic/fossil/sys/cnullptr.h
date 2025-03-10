/*
 * -----------------------------------------------------------------------------
 * Project: Fossil Logic
 *
 * This file is part of the Fossil Logic project, which aims to develop high-
 * performance, cross-platform applications and libraries. The code contained
 * herein is subject to the terms and conditions defined in the project license.
 *
 * Author: Michael Gene Brockus (Dreamer)
 *
 * Copyright (C) 2024 Fossil Logic. All rights reserved.
 * -----------------------------------------------------------------------------
 */
#ifndef FOSSIL_SYS_CNULLPTR_H
#define FOSSIL_SYS_CNULLPTR_H

#include <stdint.h>
#include <stddef.h>
#include <assert.h>

#ifdef __cplusplus
extern "C" {
#endif

// Ensure null pointer is defined for compatibility with C11, C23, and C++ standards
#ifndef FOSSIL_CNULL

#if __cplusplus >= 201103L || (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 202311L)
/**
 * @brief Definition for cnull pointers in C++11 and later or C23 and later.
 *
 * In C++11 or later, `cnullptr` is a keyword representing a cnull pointer constant.
 * In C23 or later, `_cnullptr` is recognized in the same way as C++.
 */
    #define cnull    nullptr
    #define cnullptr nullptr
#else
    #if defined(_WIN64) || defined(_WIN32)
/**
 * @brief Definition for cnull pointers on Windows systems.
 *
 * For Windows (both 32-bit and 64-bit), we define `cnull` and `cnullptr` as 0.
 */
    #define cnull    0
    #define cnullptr 0
#else
/**
 * @brief Definition for cnull pointers on POSIX systems, macOS, and embedded systems.
 *
 * For POSIX, macOS, and embedded systems, we define `cnull` and `cnullptr` as a void pointer to 0.
 */
    #define cnull    (void *)(0)
    #define cnullptr (void *)(0)
    #endif
#endif
#endif

/**
 * @brief Macro to nullify a pointer.
 *
 * This macro sets a pointer to `cnull` (`nullptr` in C++ or platform-appropriate null in C).
 * It ensures that the pointer is safely assigned a null value.
 *
 * @param ptr The pointer to be nullified.
 */
#define cnullify(ptr) ((ptr) = cnull)

/**
 * @brief Checks if a pointer is not null.
 *
 * A macro that explicitly verifies if a pointer is not null before using it.
 *
 * @param ptr The pointer to check.
 * @return 1 if not null, 0 otherwise.
 */
#define cnotnull(ptr) ((ptr) != cnull)

/**
 * @brief Represents an optional (nullable) value.
 *
 * If the value is null, it returns a default value instead.
 *
 * @param ptr The pointer to check.
 * @param default_val The default value to return if `ptr` is null.
 * @return `ptr` if not null, otherwise `default_val`.
 */
#define cmaybe(ptr, default_val) ((ptr) ? (ptr) : (default_val))

/**
 * @brief Unwraps a pointer, asserting that it is not null.
 *
 * If the pointer is null, the program will terminate with an assertion failure.
 * Otherwise, it returns the pointer itself for safe dereferencing.
 *
 * @param ptr The pointer to unwrap.
 * @return The unwrapped pointer if it is not null.
 */
#define cunwrap(ptr) (assert((ptr) != cnull), (ptr))

/**
 * @brief Marks a variable as unused to suppress compiler warnings.
 *
 * Some compilers generate warnings when a variable is declared but not used.
 * This macro safely prevents such warnings without affecting the program.
 *
 * @param x The variable that is intentionally unused.
 */
#ifndef cunused
    #if defined(__GNUC__) || defined(__clang__)
        #define cunused(x) (void)(x)
    #else
        #define cunused(x) /* no-op */
    #endif
#endif

/**
 * @brief Annotations for nullable and nonnull pointers.
 *
 * These macros provide compiler hints about pointer validity, improving static analysis and safety.
 *
 * - **GCC/Clang:** Uses `__attribute__((nullable))` and `__attribute__((nonnull))`
 * - **MSVC:** Uses `_Null_terminated_` and `_In_` (though `_In_` is not strictly equivalent to nonnull)
 * - **Fallback:** If the compiler does not support these attributes, it defines empty macros.
 */
#if defined(__clang__) || defined(__GNUC__)
    #define cnullable __attribute__((nullable))
    #define cnonnull  __attribute__((nonnull))
#elif defined(_MSC_VER)
    #define cnullable _Null_terminated_  // Not a perfect match, but useful for MSVC
    #define cnonnull  _In_               // MSVC does not have a direct `nonnull` equivalent
#else
    #define cnullable
    #define cnonnull
#endif

// Termination values for regular and wide strings

/**
 * @brief Null-terminated character for C strings.
 *
 * This is used as a constant for the null character in C strings, typically to represent the end of a string.
 */
#define cterm '\0'

/**
 * @brief Null-terminated wide-character for wide strings.
 *
 * This is used as a constant for the null character in wide strings (`wchar_t` arrays), typically to represent the end of a wide string.
 */
#define wterm L'\0'

// Newline constants for regular and wide strings

/**
 * @brief Defines the newline character for C.
 *
 * This is used in C and C++ environments for regular strings to denote a newline.
 */
#define cnewline    '\n'

/**
 * @brief Defines the newline character for wide strings in C and C++.
 *
 * This is used for wide-character strings (`wchar_t`) to denote a newline.
 */
#define wnewline    L'\n'

/**
 * @brief Defines an empty C string.
 *
 * This represents an empty string (`""`) for use in C and C++ code.
 */
#define cempty      ""

/**
 * @brief Defines an empty wide-character string.
 *
 * This represents an empty wide string (`L""`) for use in C and C++ code.
 */
#define wempty      L""

/**
 * @brief Type-safe compiler attributes for null and nullable types.
 *
 * - `cnull` and `cnullptr` handle null pointers across platforms.
 * - The constants `cterminator`, `wterminator`, `cterm`, `wterm` are used to represent the null terminators
 *   for regular and wide-character strings.
 * - `cnewline` and `wnewline` are used to represent newline characters for regular and wide strings.
 * - `cempty` and `wempty` represent empty strings for regular and wide-character strings.
 *
 * These definitions ensure proper handling of string terminations and special characters across platforms.
 * Compiler-specific attributes:
 * - **GCC/Clang**: The use of `nullptr` for null pointers in C++ and null terminators for strings.
 * - **MSVC**: MSVC compilers do not natively support `nullptr` but handle `cnull` as 0.
 */

#ifdef __cplusplus
}
#endif

#endif
