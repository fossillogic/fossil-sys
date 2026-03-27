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
#ifndef FOSSIL_SYS_DYNAMIC_H
#define FOSSIL_SYS_DYNAMIC_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>

/* ------------------------------------------------------
    * Platform abstraction
    * ----------------------------------------------------- */

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
typedef HMODULE fossil_sys_dynamic_handle_t;
#else
typedef void *fossil_sys_dynamic_handle_t;
#endif

/* ------------------------------------------------------
    * Types
    * ----------------------------------------------------- */

/* Dynamic library descriptor */
typedef struct
{
    const char *id;   /* string ID for AI/meta tracking */
    const char *path; /* file path to library */
    fossil_sys_dynamic_handle_t handle;
    int status; /* loaded / unloaded / error */
} fossil_sys_dynamic_lib_t;

/* ------------------------------------------------------
    * Lifecycle
    * ----------------------------------------------------- */

/**
 * @brief Load a dynamic library from the specified path.
 *
 * Attempts to load a shared library (.so, .dll, .dylib) and initializes
 * the fossil_sys_dynamic_lib_t structure with the library handle.
 * Sets the status field to indicate success or failure.
 *
 * @param path      Path to the dynamic library file.
 * @param out_lib   Pointer to library descriptor (will be populated).
 * @return          true if library loaded successfully, false otherwise.
 */
bool fossil_sys_dynamic_load(
    const char *path,
    fossil_sys_dynamic_lib_t *out_lib);

/**
 * @brief Unload a previously loaded dynamic library.
 *
 * Releases the library handle and associated resources. Sets the handle
 * to NULL and updates the status field. Safe to call on unloaded libraries.
 *
 * @param lib       Pointer to library descriptor to unload.
 * @return          true if unload succeeded, false on error.
 */
bool fossil_sys_dynamic_unload(
    fossil_sys_dynamic_lib_t *lib);

/* ------------------------------------------------------
    * Symbol resolution
    * ----------------------------------------------------- */

/**
 * @brief Retrieve a function or symbol pointer from the loaded library.
 *
 * Looks up a symbol by name and returns its address. The caller must
 * cast the returned pointer to the appropriate function signature.
 * Returns NULL if the symbol is not found or library is not loaded.
 *
 * @param lib           Pointer to loaded library descriptor.
 * @param symbol_name   Name of the symbol to resolve.
 * @return              Pointer to the symbol, or NULL on failure.
 */
void *fossil_sys_dynamic_symbol(
    fossil_sys_dynamic_lib_t *lib,
    const char *symbol_name);

/* ------------------------------------------------------
    * Introspection / diagnostics
    * ----------------------------------------------------- */

/**
 * @brief Check whether a library is currently loaded.
 *
 * Inspects the library descriptor to determine if the handle is valid
 * and the library is active in memory.
 *
 * @param lib   Pointer to library descriptor.
 * @return      true if library is loaded, false otherwise.
 */
bool fossil_sys_dynamic_is_loaded(
    const fossil_sys_dynamic_lib_t *lib);

/**
 * @brief Retrieve the last error message from dynamic library operations.
 *
 * Returns a platform-specific error string describing the most recent
 * load, unload, or symbol resolution failure. Message is valid until
 * the next dynamic library operation.
 *
 * @return  Error message string, or empty string if no error occurred.
 */
const char *fossil_sys_dynamic_error(void);

#ifdef __cplusplus
}

namespace fossil::sys
{

    class Dynamic
    {
    private:
        fossil_sys_dynamic_lib_t lib_{};
        bool loaded_ = false;

        /* ----------------------------------------------
         * Internal validation hook
         * (You can expand this later)
         * ---------------------------------------------- */
        static bool validate_path(const char *path)
        {
            if (!path || !*path)
                return false;

            /* block obvious traversal attempts */
            if (strstr(path, ".."))
                return false;

            /* optional: forbid relative paths entirely */
            if (path[0] == '.')
                return false;

            return true;
        }

    public:
        /* ----------------------------------------------
         * Constructors / Destructor
         * ---------------------------------------------- */

        /**
         * @brief Default constructor.
         *
         * Initializes an empty Dynamic object with no loaded library.
         */
        Dynamic() = default;

        /**
         * @brief Construct and load a dynamic library from the given path.
         *
         * Attempts to load the library at the specified path.
         * The object will represent the loaded library if successful.
         *
         * @param path Path to the dynamic library file.
         */
        explicit Dynamic(const char *path)
        {
            load(path);
        }

        /**
         * @brief Construct with an ID and load a dynamic library from the given path.
         *
         * Sets the library ID and attempts to load the library at the specified path.
         *
         * @param id   String identifier for the library.
         * @param path Path to the dynamic library file.
         */
        Dynamic(const char *id, const char *path)
        {
            lib_.id = id;
            load(path);
        }

        /**
         * @brief Destructor.
         *
         * Unloads the library if it is currently loaded.
         */
        ~Dynamic()
        {
            if (loaded_)
            {
                fossil_sys_dynamic_unload(&lib_);
                loaded_ = false;
            }
        }

        /**
         * @brief Deleted copy constructor.
         *
         * Prevents copying of Dynamic objects to avoid double-unloading.
         */
        Dynamic(const Dynamic &) = delete;

        /**
         * @brief Deleted copy assignment operator.
         *
         * Prevents copying of Dynamic objects to avoid double-unloading.
         */
        Dynamic &operator=(const Dynamic &) = delete;

        /**
         * @brief Move constructor.
         *
         * Transfers ownership of the loaded library from another Dynamic object.
         * The source object is left in an unloaded state.
         *
         * @param other Dynamic object to move from.
         */
        Dynamic(Dynamic &&other) noexcept
        {
            lib_ = other.lib_;
            loaded_ = other.loaded_;

            other.lib_.handle = nullptr;
            other.loaded_ = false;
        }

        /**
         * @brief Move assignment operator.
         *
         * Transfers ownership of the loaded library from another Dynamic object.
         * Unloads any currently loaded library in this object.
         * The source object is left in an unloaded state.
         *
         * @param other Dynamic object to move from.
         * @return Reference to this object.
         */
        Dynamic &operator=(Dynamic &&other) noexcept
        {
            if (this != &other)
            {
                if (loaded_)
                    fossil_sys_dynamic_unload(&lib_);

                lib_ = other.lib_;
                loaded_ = other.loaded_;

                other.lib_.handle = nullptr;
                other.loaded_ = false;
            }
            return *this;
        }

        /**
         * @brief Load a dynamic library from the specified path.
         *
         * Validates the path and attempts to load the library.
         * Returns true on success, false otherwise.
         *
         * @param path Path to the dynamic library file.
         * @return true if the library was loaded successfully, false otherwise.
         */
        bool load(const char *path)
        {
            if (loaded_)
                return false; /* already loaded */
            if (!validate_path(path))
                return false;

            lib_.path = path;

            if (!fossil_sys_dynamic_load(path, &lib_))
                return false;

            loaded_ = true;
            return true;
        }

        /**
         * @brief Unload the currently loaded dynamic library.
         *
         * Releases the library handle and marks the object as unloaded.
         * Returns true on success, false otherwise.
         *
         * @return true if the library was unloaded successfully, false otherwise.
         */
        bool unload()
        {
            if (!loaded_)
                return false;

            if (!fossil_sys_dynamic_unload(&lib_))
                return false;

            loaded_ = false;
            return true;
        }

        /**
         * @brief Look up a symbol in the loaded dynamic library.
         *
         * Returns a pointer to the symbol with the given name, or nullptr if not found.
         *
         * @param name Name of the symbol to look up.
         * @return Pointer to the symbol, or nullptr if not found or not loaded.
         */
        void *symbol(const char *name)
        {
            if (!loaded_ || !name || !*name)
                return nullptr;

            return fossil_sys_dynamic_symbol(&lib_, name);
        }

        /**
         * @brief Check if a library is currently loaded.
         *
         * @return true if a library is loaded, false otherwise.
         */
        bool is_loaded() const { return loaded_; }

        /**
         * @brief Retrieve the last error message from dynamic library operations.
         *
         * Returns a platform-specific error string describing the most recent
         * load, unload, or symbol resolution failure.
         *
         * @return Error message string, or empty string if no error occurred.
         */
        const char *error() const
        {
            return fossil_sys_dynamic_error();
        }

        /**
         * @brief Get a const pointer to the underlying library descriptor.
         *
         * @return Const pointer to the fossil_sys_dynamic_lib_t structure.
         */
        const fossil_sys_dynamic_lib_t *raw() const { return &lib_; }

        /**
         * @brief Get a pointer to the underlying library descriptor.
         *
         * @return Pointer to the fossil_sys_dynamic_lib_t structure.
         */
        fossil_sys_dynamic_lib_t *raw() { return &lib_; }
    };

}
#endif

#endif /* FOSSIL_SYS_DYNAMIC_H */
