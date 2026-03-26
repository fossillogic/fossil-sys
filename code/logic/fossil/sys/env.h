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
#ifndef FOSSIL_SYS_ENV_H
#define FOSSIL_SYS_ENV_H

#ifdef __cplusplus
extern "C" {
#endif

/*
===============================================================================
Fossil Sys Environment API

This module defines a portable key/value environment interface using
string-based IDs. These IDs form the canonical configuration surface
for Fossil Sys across all platforms (POSIX, Windows, embedded, WASM).

-------------------------------------------------------------------------------
STRING ID CONVENTION

All keys are lowercase, dot-separated:

    fossil.<domain>.<name>

Domains:
    sys       - system-level properties
    runtime   - runtime configuration
    app       - application-defined values
    user      - user/environment context
    temp      - temporary/runtime paths

-------------------------------------------------------------------------------
CANONICAL STRING IDS

-- System (fossil.sys.*)

"fossil.sys.name"
    Human-readable OS/system name
    Example: "linux", "windows", "darwin"

"fossil.sys.arch"
    CPU architecture
    Example: "x86_64", "arm64"

"fossil.sys.page_size"
    Memory page size (bytes, integer string)

"fossil.sys.cpu_count"
    Number of logical CPUs (integer string)

"fossil.sys.path"
    System PATH equivalent (normalized)

"fossil.sys.home"
    User home directory

"fossil.sys.tmp"
    Temporary directory path


-- Runtime (fossil.runtime.*)

"fossil.runtime.mode"
    Execution mode
    Values: "debug", "release", "test"

"fossil.runtime.debug"
    Debug flag
    Values: "0", "1", "true", "false"

"fossil.runtime.log.level"
    Logging level
    Values: "trace", "debug", "info", "warn", "error"

"fossil.runtime.log.output"
    Log output target
    Example: "stderr", "stdout", "file:<path>"

"fossil.runtime.seed"
    Random seed (integer string)

"fossil.runtime.threads"
    Desired thread count (integer string)


-- Application (fossil.app.*)

"fossil.app.name"
    Application name

"fossil.app.version"
    Application version string

"fossil.app.instance"
    Instance identifier (for multi-instance systems)


-- User (fossil.user.*)

"fossil.user.name"
    Username

"fossil.user.id"
    User identifier (platform-specific string)


-- Temp / Paths (fossil.temp.*)

"fossil.temp.dir"
    Temporary working directory

"fossil.temp.cache"
    Cache directory


-------------------------------------------------------------------------------
NOTES

- All values are UTF-8 null-terminated strings.
- Missing keys return NULL unless using fallback helpers.
- Backends may source values from:
    - OS environment variables
    - platform APIs
    - embedded/static configuration
- Keys are case-sensitive.
- This is NOT a registry or config system—only a flat key/value interface.

===============================================================================
*/

/**
 * Retrieve the value of an environment variable by key.
 *
 * @param key The name of the environment variable to retrieve.
 * @return Pointer to the value string if found, or NULL if not set.
 */
const char *fossil_sys_env_get(const char *key);

/**
 * Set the value of an environment variable.
 *
 * @param key The name of the environment variable to set.
 * @param value The value to assign to the environment variable.
 * @return 0 on success, non-zero on failure.
 */
int fossil_sys_env_set(const char *key, const char *value);

/**
 * Check if an environment variable exists.
 *
 * @param key The name of the environment variable to check.
 * @return 1 if the variable exists, 0 otherwise.
 */
int fossil_sys_env_exists(const char *key);

/**
 * Retrieve the value of an environment variable, or a fallback if not set.
 *
 * @param key The name of the environment variable to retrieve.
 * @param fallback The fallback value to return if the variable is not set.
 * @return Pointer to the value string or the fallback if not set.
 */
const char *fossil_sys_env_get_or(const char *key,
                       const char *fallback);

/**
 * Retrieve the value of an environment variable as an integer.
 *
 * @param key The name of the environment variable to retrieve.
 * @param default_value The value to return if the variable is not set or not an integer.
 * @return The integer value of the variable, or default_value if not set or invalid.
 */
int fossil_sys_env_get_int(const char *key,
                        int default_value);

/**
 * Retrieve the value of an environment variable as a boolean.
 *
 * @param key The name of the environment variable to retrieve.
 * @param default_value The value to return if the variable is not set or not a boolean.
 * @return 1 if true, 0 if false, or default_value if not set or invalid.
 */
int fossil_sys_env_get_bool(const char *key,
                         int default_value);

/**
 * Callback type for iterating over environment variables.
 *
 * @param key The name of the environment variable.
 * @param value The value of the environment variable.
 * @param user_data User-defined data pointer passed to the callback.
 * @return Implementation-defined; typically 0 to continue, non-zero to stop.
 */
typedef int (*fossil_sys_env_iter_cb)(
    const char* key,
    const char* value,
    void* user_data);

/**
 * Iterate over all environment variables, invoking a callback for each.
 *
 * @param cb The callback function to invoke for each environment variable.
 * @param user_data User-defined data pointer passed to the callback.
 */
void fossil_sys_env_foreach(fossil_sys_env_iter_cb cb, void* user_data);

#ifdef __cplusplus
}
#include <string>
#include <functional>

namespace fossil::sys {
    
    class Env
    {
    public:
        /**
         * Retrieve the value of an environment variable by key.
         * Returns an empty string if the variable is not set.
         *
         * @param key The environment variable key.
         * @return The value as a std::string, or empty if not found.
         */
        static std::string get(const std::string& key) {
            const char* val = fossil_sys_env_get(key.c_str());
            return val ? std::string(val) : std::string();
        }

        /**
         * Set the value of an environment variable.
         *
         * @param key The environment variable key.
         * @param value The value to set.
         * @return true on success, false on failure.
         */
        static bool set(const std::string& key, const std::string& value) {
            return fossil_sys_env_set(key.c_str(), value.c_str()) == 0;
        }

        /**
         * Check if an environment variable exists.
         *
         * @param key The environment variable key.
         * @return true if the variable exists, false otherwise.
         */
        static bool exists(const std::string& key) {
            return fossil_sys_env_exists(key.c_str()) == 1;
        }

        /**
         * Retrieve the value of an environment variable, or a fallback if not set.
         *
         * @param key The environment variable key.
         * @param fallback The fallback value to return if not set.
         * @return The value as a std::string, or the fallback if not found.
         */
        static std::string get_or(const std::string& key, const std::string& fallback) {
            const char* val = fossil_sys_env_get_or(key.c_str(), fallback.c_str());
            return val ? std::string(val) : fallback;
        }

        /**
         * Retrieve the value of an environment variable as an integer.
         *
         * @param key The environment variable key.
         * @param default_value The value to return if not set or invalid.
         * @return The integer value, or default_value if not set or invalid.
         */
        static int get_int(const std::string& key, int default_value) {
            return fossil_sys_env_get_int(key.c_str(), default_value);
        }

        /**
         * Retrieve the value of an environment variable as a boolean.
         *
         * @param key The environment variable key.
         * @param default_value The value to return if not set or invalid.
         * @return true if the value is true, false otherwise.
         */
        static bool get_bool(const std::string& key, bool default_value) {
            return fossil_sys_env_get_bool(key.c_str(), default_value ? 1 : 0) == 1;
        }

        /**
         * Type alias for the iteration callback function.
         * The callback receives the key and value as std::string.
         */
        using iter_callback = std::function<int(const std::string&, const std::string&)>;

        /**
         * Iterate over all environment variables, invoking the callback for each.
         *
         * @param cb The callback function to invoke for each variable.
         */
        static void foreach_var(const iter_callback& cb) {
            struct Wrapper {
            static int trampoline(const char* key, const char* value, void* user_data) {
                auto* func = static_cast<const iter_callback*>(user_data);
                return (*func)(std::string(key), std::string(value));
            }
            };
            fossil_sys_env_foreach(&Wrapper::trampoline, (void*)&cb);
        }
    };

} // namespace fossil::sys

#endif

#endif /* FOSSIL_SYS_OS_H */
