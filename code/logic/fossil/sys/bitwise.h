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
#ifndef FOSSIL_SYS_BITWISE_H
#define FOSSIL_SYS_BITWISE_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#endif

// A bitwise table entry
typedef struct
{
    const char *name; // e.g., "read"
    uint64_t bit;     // e.g., 1 << 0
} fossil_sys_bitwise_entry_t;

// A table of bitwise names <-> values
typedef struct
{
    const fossil_sys_bitwise_entry_t *entries;
    size_t count;
} fossil_sys_bitwise_table_t;

//
// Flag parsing/formatting
//

/**
 * Parses a string like "read|write" into a bitmask using the given table.
 *
 * @param input The input string to parse. It should contain bitwise names
 *              separated by '|'.
 * @param table The bitwise table that maps names to bit values.
 * @return The resulting bitmask as a uint64_t value.
 */
uint64_t fossil_sys_bitwise_parse(const char *input, const fossil_sys_bitwise_table_t *table);

/**
 * Formats a bitmask into a string like "read|write".
 *
 * @param bits The bitmask to format into a human-readable string.
 * @param table The bitwise table that maps bit values to names.
 * @param out The output buffer where the formatted string will be written.
 * @param out_size The size of the output buffer. The resulting string will
 *                 be null-terminated.
 * @return 0 on success, or a non-zero error code on failure.
 */
int fossil_sys_bitwise_format(uint64_t bits, const fossil_sys_bitwise_table_t *table, char *out, size_t out_size);

/**
 * Checks if a string corresponds to a known bit in the table.
 *
 * @param name The name to look up in the bitwise table.
 * @param table The bitwise table that maps names to bit values.
 * @param out_bit Pointer to a uint64_t where the corresponding bit value
 *                will be stored if the name is found.
 * @return 0 on success, or a non-zero error code if the name is not found.
 */
int fossil_sys_bitwise_lookup(const char *name, const fossil_sys_bitwise_table_t *table, uint64_t *out_bit);

/**
 * Returns a bitmask containing all valid bits from the table.
 *
 * @param table The bitwise table.
 * @return A uint64_t mask where all known bits are set.
 */
uint64_t fossil_sys_bitwise_all(const fossil_sys_bitwise_table_t *table);

/**
 * Validates that a bitmask contains only known bits.
 *
 * @param bits The bitmask to validate.
 * @param table The bitwise table.
 * @return 0 if valid, non-zero if unknown bits are present.
 */
int fossil_sys_bitwise_validate(uint64_t bits, const fossil_sys_bitwise_table_t *table);

/**
 * Finds the name corresponding to a single bit value.
 *
 * @param bit The bit value to look up (must be a single bit).
 * @param table The bitwise table.
 * @return Pointer to the name string, or NULL if not found.
 */
const char *fossil_sys_bitwise_name(uint64_t bit, const fossil_sys_bitwise_table_t *table);

/**
 * Counts how many bits are set in the bitmask.
 *
 * @param bits The bitmask to count.
 * @return The number of bits set.
 */
size_t fossil_sys_bitwise_count(uint64_t bits);

/**
 * Checks whether a specific bit is set in a bitmask.
 *
 * @param bits The bitmask to check.
 * @param bit The bit to test (e.g., 1 << 3).
 * @return 1 if the bit is set, 0 otherwise.
 */
static inline int fossil_sys_bitwise_has(uint64_t bits, uint64_t bit)
{
    return (bits & bit) != 0;
}

#ifdef __cplusplus
}

#include <string>
#include <stdexcept>

/**
 * Fossil namespace.
 */
namespace fossil::sys
{

    /**
     * @class Bitwise
     *
     * @brief Utility class providing C++-style access to bitmask parsing and
     *        manipulation functions.
     *
     * This class is a thin wrapper around the C API for working with bitwise
     * tables. It allows you to convert between string representations of flags
     * (like "read|write") and their numeric bitmask values, look up individual
     * flag names, and validate or count bits in a mask. Where possible, the
     * interface is made more idiomatic for C++ by using std::string, bool, and
     * exceptions for error handling.
     */
    class Bitwise
    {
    public:
        /**
         * @brief Parse a string into a bitmask.
         *
         * Given a string containing one or more symbolic flag names separated
         * by the pipe character ('|'), this function returns the corresponding
         * numeric bitmask.
         *
         * Example:
         * @code
         * uint64_t mask = Bitwise::parse("read|write", table);
         * @endcode
         *
         * @param input  The input string to parse (e.g., "read|write").
         * @param table  Pointer to the bitwise table that defines valid flags.
         * @return A 64-bit bitmask with the corresponding bits set.
         */
        static uint64_t parse(const std::string &input, const fossil_sys_bitwise_table_t *table)
        {
            return fossil_sys_bitwise_parse(input.c_str(), table);
        }

        /**
         * @brief Format a bitmask into a string using a caller-provided buffer.
         *
         * This is a direct wrapper around the C API. The caller is responsible
         * for providing a buffer of sufficient size. The formatted string will
         * list all set bits separated by '|'.
         *
         * Example:
         * @code
         * char buf[128];
         * Bitwise::format(mask, table, buf, sizeof(buf));
         * printf("Mask = %s\n", buf);
         * @endcode
         *
         * @param bits      The bitmask to format.
         * @param table     Pointer to the bitwise table that defines valid flags.
         * @param out       Caller-provided output buffer.
         * @param out_size  Size of the output buffer, including space for null terminator.
         * @return 0 on success, or a non-zero error code on failure.
         */
        static int format(uint64_t bits, const fossil_sys_bitwise_table_t *table, char *out, size_t out_size)
        {
            return fossil_sys_bitwise_format(bits, table, out, out_size);
        }

        /**
         * @brief Format a bitmask into a std::string.
         *
         * Unlike the buffer-based version, this overload allocates a temporary
         * buffer and returns the result as a std::string. If the buffer is not
         * large enough, an exception is thrown. This is the preferred C++-style
         * interface for most users.
         *
         * Example:
         * @code
         * std::string result = Bitwise::format(mask, table);
         * std::cout << "Mask = " << result << std::endl;
         * @endcode
         *
         * @param bits   The bitmask to format.
         * @param table  Pointer to the bitwise table that defines valid flags.
         * @return A std::string containing the formatted representation.
         *
         * @throws std::runtime_error if formatting fails.
         */
        static std::string format(uint64_t bits, const fossil_sys_bitwise_table_t *table)
        {
            char buffer[1024]; // Reasonable default; adjust if tables can be very large
            int rc = fossil_sys_bitwise_format(bits, table, buffer, sizeof(buffer));
            if (rc != 0)
            {
                throw std::runtime_error("fossil_sys_bitwise_format failed");
            }
            return std::string(buffer);
        }

        /**
         * @brief Look up a single flag name and return its corresponding bit value.
         *
         * Example:
         * @code
         * uint64_t bit;
         * if (Bitwise::lookup("read", table, bit) == 0) {
         *     // bit now contains 1 << 0
         * }
         * @endcode
         *
         * @param name     The flag name to look up.
         * @param table    Pointer to the bitwise table.
         * @param out_bit  Reference that will receive the bit value.
         * @return 0 if found, or non-zero if the name was not recognized.
         */
        static int lookup(const std::string &name, const fossil_sys_bitwise_table_t *table, uint64_t &out_bit)
        {
            return fossil_sys_bitwise_lookup(name.c_str(), table, &out_bit);
        }

        /**
         * @brief Return a bitmask containing all valid bits in the table.
         *
         * This is useful for quickly constructing masks that represent "everything".
         *
         * Example:
         * @code
         * uint64_t all_flags = Bitwise::all(table);
         * @endcode
         *
         * @param table  Pointer to the bitwise table.
         * @return A mask with all defined bits set.
         */
        static uint64_t all(const fossil_sys_bitwise_table_t *table)
        {
            return fossil_sys_bitwise_all(table);
        }

        /**
         * @brief Validate that a bitmask contains only recognized bits.
         *
         * This is useful for catching bugs where invalid bits are accidentally set.
         *
         * Example:
         * @code
         * if (!Bitwise::validate(mask, table)) {
         *     std::cerr << "Invalid bitmask!" << std::endl;
         * }
         * @endcode
         *
         * @param bits   The bitmask to validate.
         * @param table  Pointer to the bitwise table.
         * @return true if valid, false if any unknown bits are present.
         */
        static bool validate(uint64_t bits, const fossil_sys_bitwise_table_t *table)
        {
            return fossil_sys_bitwise_validate(bits, table) == 0;
        }

        /**
         * @brief Find the name corresponding to a single bit.
         *
         * Example:
         * @code
         * std::string name = Bitwise::name(1 << 0, table);
         * // name == "read"
         * @endcode
         *
         * @param bit    The bit value to look up (must be a single bit, not a mask).
         * @param table  Pointer to the bitwise table.
         * @return The name as a std::string, or an empty string if the bit is unknown.
         */
        static std::string name(uint64_t bit, const fossil_sys_bitwise_table_t *table)
        {
            const char *n = fossil_sys_bitwise_name(bit, table);
            return n ? std::string(n) : std::string();
        }

        /**
         * @brief Count how many bits are set in the mask.
         *
         * Example:
         * @code
         * size_t num_flags = Bitwise::count(mask);
         * @endcode
         *
         * @param bits  The bitmask to count.
         * @return The number of set bits.
         */
        static size_t count(uint64_t bits)
        {
            return fossil_sys_bitwise_count(bits);
        }

        /**
         * @brief Test whether a specific bit is set in the mask.
         *
         * Example:
         * @code
         * if (Bitwise::has(mask, MY_FLAG)) {
         *     // do something
         * }
         * @endcode
         *
         * @param bits  The bitmask to check.
         * @param bit   The bit to test.
         * @return true if the bit is set, false otherwise.
         */
        static bool has(uint64_t bits, uint64_t bit)
        {
            return (bits & bit) != 0;
        }

        /**
         * @brief Bitwise OR operator for combining flags.
         *
         * Example:
         * @code
         * uint64_t mask = Bitwise::OR(READ, WRITE);
         * // Or use the operator directly:
         * uint64_t mask = READ | WRITE;
         * @endcode
         */
        static inline uint64_t OR(uint64_t lhs, uint64_t rhs)
        {
            return lhs | rhs;
        }

        /**
         * @brief Bitwise AND operator for masking flags.
         *
         * Example:
         * @code
         * if (Bitwise::AND(mask, READ)) { ... }
         * // Or use the operator directly:
         * if (mask & READ) { ... }
         * @endcode
         */
        static inline uint64_t AND(uint64_t lhs, uint64_t rhs)
        {
            return lhs & rhs;
        }

        /**
         * @brief Bitwise XOR operator for toggling flags.
         */
        static inline uint64_t XOR(uint64_t lhs, uint64_t rhs)
        {
            return lhs ^ rhs;
        }

        /**
         * @brief Bitwise NOT operator for inverting a mask.
         */
        static inline uint64_t NOT(uint64_t bits)
        {
            return ~bits;
        }
    };

    // ---------- Global Operator Overloads for fossil_sys_bitwise_entry_t ----------
    //
    // These allow you to work with fossil_sys_bitwise_entry_t entries as if
    // they were strongly-typed bitmask constants, producing uint64_t results.
    //

    // --- Bitwise OR ---
    inline uint64_t operator|(uint64_t lhs, const fossil_sys_bitwise_entry_t &rhs)
    {
        return lhs | rhs.bit;
    }
    inline uint64_t operator|(const fossil_sys_bitwise_entry_t &lhs, uint64_t rhs)
    {
        return lhs.bit | rhs;
    }
    inline uint64_t operator|(const fossil_sys_bitwise_entry_t &lhs, const fossil_sys_bitwise_entry_t &rhs)
    {
        return lhs.bit | rhs.bit;
    }

    // --- Bitwise AND ---
    inline uint64_t operator&(uint64_t lhs, const fossil_sys_bitwise_entry_t &rhs)
    {
        return lhs & rhs.bit;
    }
    inline uint64_t operator&(const fossil_sys_bitwise_entry_t &lhs, uint64_t rhs)
    {
        return lhs.bit & rhs;
    }
    inline uint64_t operator&(const fossil_sys_bitwise_entry_t &lhs, const fossil_sys_bitwise_entry_t &rhs)
    {
        return lhs.bit & rhs.bit;
    }

    // --- Bitwise XOR ---
    inline uint64_t operator^(uint64_t lhs, const fossil_sys_bitwise_entry_t &rhs)
    {
        return lhs ^ rhs.bit;
    }
    inline uint64_t operator^(const fossil_sys_bitwise_entry_t &lhs, uint64_t rhs)
    {
        return lhs.bit ^ rhs;
    }
    inline uint64_t operator^(const fossil_sys_bitwise_entry_t &lhs, const fossil_sys_bitwise_entry_t &rhs)
    {
        return lhs.bit ^ rhs.bit;
    }

    // --- Bitwise NOT ---
    inline uint64_t operator~(const fossil_sys_bitwise_entry_t &entry)
    {
        return ~entry.bit;
    }

    // --- Compound Assignment Operators (modify lhs) ---
    inline uint64_t &operator|=(uint64_t &lhs, const fossil_sys_bitwise_entry_t &rhs)
    {
        lhs |= rhs.bit;
        return lhs;
    }
    inline uint64_t &operator&=(uint64_t &lhs, const fossil_sys_bitwise_entry_t &rhs)
    {
        lhs &= rhs.bit;
        return lhs;
    }
    inline uint64_t &operator^=(uint64_t &lhs, const fossil_sys_bitwise_entry_t &rhs)
    {
        lhs ^= rhs.bit;
        return lhs;
    }

    // --- Comparison Operators (check equality of bits) ---
    inline bool operator==(uint64_t lhs, const fossil_sys_bitwise_entry_t &rhs)
    {
        return lhs == rhs.bit;
    }

    inline bool operator==(const fossil_sys_bitwise_entry_t &lhs, uint64_t rhs)
    {
        return lhs.bit == rhs;
    }

    inline bool operator==(const fossil_sys_bitwise_entry_t &lhs, const fossil_sys_bitwise_entry_t &rhs)
    {
        return lhs.bit == rhs.bit;
    }

    inline bool operator!=(uint64_t lhs, const fossil_sys_bitwise_entry_t &rhs)
    {
        return lhs != rhs.bit;
    }

    inline bool operator!=(const fossil_sys_bitwise_entry_t &lhs, uint64_t rhs)
    {
        return lhs.bit != rhs;
    }

    inline bool operator!=(const fossil_sys_bitwise_entry_t &lhs, const fossil_sys_bitwise_entry_t &rhs)
    {
        return lhs.bit != rhs.bit;
    }

    // --- Convenience Predicate: "Is bit set?" ---
    inline bool operator&&(uint64_t lhs, const fossil_sys_bitwise_entry_t &rhs)
    {
        return (lhs & rhs.bit) != 0;
    }

    inline bool operator&&(const fossil_sys_bitwise_entry_t &lhs, uint64_t rhs)
    {
        return (lhs.bit & rhs) != 0;
    }

} // namespace fossil

#endif

#endif /* FOSSIL_SYS_FRAMEWORK_H */
