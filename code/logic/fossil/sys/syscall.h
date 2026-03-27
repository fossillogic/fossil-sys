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
#ifndef FOSSIL_SYS_CALL_H
#define FOSSIL_SYS_CALL_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*
* Function to execute a system command.
* This function takes a command string as input and executes it using the
* system's command processor. The return value is the exit status of the
* command.
*/
int fossil_sys_call_execute(const char *command);

/*
* Function to get the current process ID.
* This function returns the process ID of the calling process.
* The process ID is a unique identifier for the process.
*/
int fossil_sys_call_getpid(void);

/*
* Function to sleep for a specified number of milliseconds.
* This function suspends the execution of the calling thread for the specified
* number of milliseconds.
*/
void fossil_sys_call_sleep(int milliseconds);

/*
* Function to create a new file.
* This function creates a new file with the specified filename.
* The return value is 0 on success, or a negative error code on failure.
*/
int fossil_sys_call_create_file(const char *filename);

/**
 * @brief Delete a file.
 *
 * Removes a file from the filesystem.
 *
 * @param filename Path to the file to delete.
 * @return 0 on success, negative error code on failure.
 */
int fossil_sys_call_delete_file(const char *filename);

/**
 * @brief Check if a file exists.
 *
 * @param filename Path to the file.
 * @return 1 if the file exists, 0 if not.
 */
int fossil_sys_call_file_exists(const char *filename);

/**
 * @brief Create a directory.
 *
 * @param dirname Path of the directory to create.
 * @return 0 on success, negative error code on failure.
 */
int fossil_sys_call_create_directory(const char *dirname);

/**
 * @brief Delete a directory (optionally recursive).
 *
 * @param dirname Path of the directory to delete.
 * @param recursive If non-zero, delete all contents recursively.
 * @return 0 on success, negative error code on failure.
 */
int fossil_sys_call_delete_directory(const char *dirname, int recursive);

/**
 * @brief Get the current working directory.
 *
 * @param buffer Buffer to receive the current path.
 * @param size Size of the buffer.
 * @return 0 on success, negative error code on failure.
 */
int fossil_sys_call_getcwd(char *buffer, size_t size);

/**
 * @brief Change the current working directory.
 *
 * @param path Path to set as current working directory.
 * @return 0 on success, negative error code on failure.
 */
int fossil_sys_call_chdir(const char *path);

/**
 * @brief List files in a directory.
 *
 * Returns a list of filenames in a given directory.
 * (Could return a dynamically allocated array of strings, or take a callback function.)
 *
 * @param dirname Path to the directory.
 * @param out_list Pointer to array of strings (allocated by function).
 * @param out_count Pointer to receive the number of entries.
 * @return 0 on success, negative error code on failure.
 */
int fossil_sys_call_list_directory(const char *dirname, char ***out_list, size_t *out_count);

/**
 * @brief Check if a path is a directory.
 *
 * @param path Path to check.
 * @return 1 if directory, 0 if not.
 */
int fossil_sys_call_is_directory(const char *path);

/**
 * @brief Check if a path is a regular file.
 *
 * @param path Path to check.
 * @return 1 if regular file, 0 if not.
 */
int fossil_sys_call_is_file(const char *path);

/**
 * @brief Terminate the current process.
 *
 * @param exit_code Exit code to return to the operating system.
 */
void fossil_sys_call_exit(int exit_code);

/**
 * @brief Execute a command and capture output.
 *
 * Runs a command and stores stdout in a buffer (similar to popen).
 *
 * @param command Command string to execute.
 * @param buffer Buffer to store output.
 * @param size Size of the buffer.
 * @return 0 on success, negative error code on failure.
 */
int fossil_sys_call_execute_capture(const char *command, char *buffer, size_t size);

#ifdef __cplusplus
}
#include <string>

/**
 * Fossil namespace.
 */
namespace fossil::sys
{

    /**
     * Syscall class.
     */
    class Syscall
    {
    public:
        /**
         * Execute a system command.
         *
         * @param command The command to execute
         * @return The return value of the system command.
         */
        static int execute(const std::string &command)
        {
            return fossil_sys_call_execute(command.c_str());
        }

        /**
         * Get the current process ID.
         *
         * @return The current process ID.
         */
        static int getpid()
        {
            return fossil_sys_call_getpid();
        }

        /**
         * Sleep for a specified number of milliseconds.
         *
         * @param milliseconds The number of milliseconds to sleep.
         */
        static void sleep(int milliseconds)
        {
            fossil_sys_call_sleep(milliseconds);
        }

        /**
         * Create a new file.
         *
         * @param filename The name of the file to create.
         * @return 0 on success, or a negative error code on failure.
         */
        static int create_file(const std::string &filename)
        {
            return fossil_sys_call_create_file(filename.c_str());
        }

        /**
         * Delete a file.
         *
         * @param filename Path to the file to delete.
         * @return 0 on success, negative error code on failure.
         */
        static int delete_file(const std::string &filename)
        {
            return fossil_sys_call_delete_file(filename.c_str());
        }

        /**
         * Check if a file exists.
         *
         * @param filename Path to the file.
         * @return 1 if the file exists, 0 if not.
         */
        static int file_exists(const std::string &filename)
        {
            return fossil_sys_call_file_exists(filename.c_str());
        }

        /**
         * Create a directory.
         *
         * @param dirname Path of the directory to create.
         * @return 0 on success, negative error code on failure.
         */
        static int create_directory(const std::string &dirname)
        {
            return fossil_sys_call_create_directory(dirname.c_str());
        }

        /**
         * Delete a directory (optionally recursive).
         *
         * @param dirname Path of the directory to delete.
         * @param recursive If non-zero, delete all contents recursively.
         * @return 0 on success, negative error code on failure.
         */
        static int delete_directory(const std::string &dirname, int recursive)
        {
            return fossil_sys_call_delete_directory(dirname.c_str(), recursive);
        }

        /**
         * Get the current working directory.
         *
         * @param buffer Buffer to receive the current path.
         * @param size Size of the buffer.
         * @return 0 on success, negative error code on failure.
         */
        static int getcwd(std::string *buffer, size_t size)
        {
            return fossil_sys_call_getcwd(buffer->data(), size);
        }

        /**
         * Change the current working directory.
         *
         * @param path Path to set as current working directory.
         * @return 0 on success, negative error code on failure.
         */
        static int chdir(const std::string &path)
        {
            return fossil_sys_call_chdir(path.c_str());
        }

        /**
         * List files in a directory.
         *
         * @param dirname Path to the directory.
         * @param out_list Pointer to array of strings (allocated by function).
         * @param out_count Pointer to receive the number of entries.
         * @return 0 on success, negative error code on failure.
         */
        static int list_directory(const std::string &dirname, char ***out_list, size_t *out_count)
        {
            return fossil_sys_call_list_directory(dirname.c_str(), out_list, out_count);
        }

        /**
         * Check if a path is a directory.
         *
         * @param path Path to check.
         * @return 1 if directory, 0 if not.
         */
        static int is_directory(const std::string &path)
        {
            return fossil_sys_call_is_directory(path.c_str());
        }

        /**
         * Check if a path is a regular file.
         *
         * @param path Path to check.
         * @return 1 if regular file, 0 if not.
         */
        static int is_file(const std::string &path)
        {
            return fossil_sys_call_is_file(path.c_str());
        }

        /**
         * Terminate the current process.
         *
         * @param exit_code Exit code to return to the operating system.
         */
        static void exit(int exit_code)
        {
            fossil_sys_call_exit(exit_code);
        }

        /**
         * Execute a command and capture output.
         *
         * @param command Command string to execute.
         * @param buffer Buffer to store output.
         * @param size Size of the buffer.
         * @return 0 on success, negative error code on failure.
         */
        static int execute_capture(const std::string &command, std::string *buffer)
        {
            return fossil_sys_call_execute_capture(command.c_str(), buffer->data(), buffer->size());
        }
    };

}

#endif

#endif /* FOSSIL_SYS_FRAMEWORK_H */
