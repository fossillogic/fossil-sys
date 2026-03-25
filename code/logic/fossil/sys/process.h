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
#ifndef FOSSIL_SYS_PROCESS_H
#define FOSSIL_SYS_PROCESS_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define FOSSIL_SYS_PROCESS_NAME_MAX 256
#define FOSSIL_SYS_PROCESS_ENV_MAX 1024

typedef struct
{
    uint32_t pid;                           // Process ID
    uint32_t ppid;                          // Parent PID
    char name[FOSSIL_SYS_PROCESS_NAME_MAX]; // Process name
    uint64_t memory_bytes;                  // Resident memory
    uint64_t virtual_memory_bytes;          // Virtual memory
    float cpu_percent;                      // CPU usage %
    uint32_t thread_count;                  // Number of threads
} fossil_sys_process_info_t;

/**
 * Process list container
 */
#define FOSSIL_SYS_PROCESS_MAX 1024
typedef struct
{
    size_t count;
    fossil_sys_process_info_t list[FOSSIL_SYS_PROCESS_MAX];
} fossil_sys_process_list_t;

/**
 * Get current process PID
 */
uint32_t fossil_sys_process_get_pid(void);

/**
 * Get process name by PID
 */
int fossil_sys_process_get_name(uint32_t pid, char *name, size_t name_len);

/**
 * Get process info by PID
 */
int fossil_sys_process_get_info(uint32_t pid, fossil_sys_process_info_t *info);

/**
 * Get list of all processes
 */
int fossil_sys_process_list(fossil_sys_process_list_t *plist);

/**
 * Terminate process by PID
 *
 * @param pid Process ID
 * @param force 1 = force kill, 0 = graceful
 */
int fossil_sys_process_terminate(uint32_t pid, int force);

/**
 * Retrieve environment variables of a process (best-effort)
 *
 * @param pid Process ID
 * @param buffer Preallocated buffer of length buf_len
 * @param buf_len Size of buffer
 * @return Number of bytes written, -1 on error
 */
int fossil_sys_process_get_environment(uint32_t pid, char *buffer, size_t buf_len);

/**
 * Check if a process with the given PID exists.
 *
 * @param pid Process ID
 * @return 1 if process exists, 0 if not, -1 on error
 */
int fossil_sys_process_exists(uint32_t pid);

/**
 * Suspend (pause) a process by PID.
 *
 * @param pid Process ID
 * @return 0 on success, negative error code on failure
 */
int fossil_sys_process_suspend(uint32_t pid);

/**
 * Resume a suspended process by PID.
 *
 * @param pid Process ID
 * @return 0 on success, negative error code on failure
 */
int fossil_sys_process_resume(uint32_t pid);

/**
 * Change the priority (nice value) of a process.
 *
 * @param pid Process ID
 * @param priority New priority value (platform-dependent, e.g., -20 to 19 on Unix)
 * @return 0 on success, negative error code on failure
 */
int fossil_sys_process_set_priority(uint32_t pid, int priority);

/**
 * Get the priority (nice value) of a process.
 *
 * @param pid Process ID
 * @param priority Pointer to store the current priority value
 * @return 0 on success, negative error code on failure
 */
int fossil_sys_process_get_priority(uint32_t pid, int *priority);

/**
 * Wait for a process to exit.
 *
 * @param pid Process ID
 * @param exit_code Pointer to store the exit code (can be NULL)
 * @param timeout_ms Timeout in milliseconds, or -1 for infinite
 * @return 0 on success, negative error code on failure
 */
int fossil_sys_process_wait(uint32_t pid, int *exit_code, int timeout_ms);

/**
 * Start a new process.
 *
 * @param path Path to executable
 * @param argv Argument vector (NULL-terminated)
 * @param envp Environment vector (NULL-terminated, can be NULL)
 * @param pid_out Pointer to store new process ID
 * @return 0 on success, negative error code on failure
 */
int fossil_sys_process_spawn(const char *path, char *const argv[], char *const envp[], uint32_t *pid_out);

/**
 * Get the executable path of a process.
 *
 * @param pid Process ID
 * @param buffer Buffer to store the path
 * @param buf_len Length of buffer
 * @return 0 on success, negative error code on failure
 */
int fossil_sys_process_get_exe_path(uint32_t pid, char *buffer, size_t buf_len);

#ifdef __cplusplus
}
#include <string>

/**
 * Fossil namespace.
 */
namespace fossil
{

    /**
     * System namespace.
     */
    namespace sys
    {

        class Process
        {
        public:
            /**
             * @brief Retrieves the process ID (PID) of the current process.
             *
             * @return uint32_t The PID of the current process.
             */
            static uint32_t get_pid()
            {
                return fossil_sys_process_get_pid();
            }

            /**
             * @brief Retrieves the name of the process with the specified PID.
             *
             * @param pid The process ID of the target process.
             * @param name Reference to a std::string where the process name will be stored on success.
             * @return int 0 on success, or a negative error code on failure.
             */
            static int get_name(uint32_t pid, std::string &name)
            {
                char buf[FOSSIL_SYS_PROCESS_NAME_MAX] = {0};
                int ret = fossil_sys_process_get_name(pid, buf, sizeof(buf));
                if (ret == 0)
                {
                    name = buf;
                }
                return ret;
            }

            /**
             * @brief Retrieves detailed information about the process with the specified PID.
             *
             * @param pid The process ID of the target process.
             * @param info Reference to a fossil_sys_process_info_t structure to be filled with process information.
             * @return int 0 on success, or a negative error code on failure.
             */
            static int get_info(uint32_t pid, fossil_sys_process_info_t &info)
            {
                return fossil_sys_process_get_info(pid, &info);
            }

            /**
             * @brief Retrieves a list of all running processes.
             *
             * @param plist Reference to a fossil_sys_process_list_t structure to be filled with the process list.
             * @return int 0 on success, or a negative error code on failure.
             */
            static int list(fossil_sys_process_list_t &plist)
            {
                return fossil_sys_process_list(&plist);
            }

            /**
             * @brief Terminates the process with the specified PID.
             *
             * @param pid The process ID of the target process.
             * @param force If true, forces termination; otherwise, attempts graceful termination.
             * @return int 0 on success, or a negative error code on failure.
             */
            static int terminate(uint32_t pid, bool force)
            {
                return fossil_sys_process_terminate(pid, force ? 1 : 0);
            }

            /**
             * @brief Retrieves the environment variables of the process with the specified PID.
             *
             * @param pid The process ID of the target process.
             * @param env Reference to a std::string where the environment variables will be stored on success.
             * @return int Number of bytes written on success, or a negative error code on failure.
             */
            static int get_environment(uint32_t pid, std::string &env)
            {
                char buf[FOSSIL_SYS_PROCESS_ENV_MAX] = {0};
                int ret = fossil_sys_process_get_environment(pid, buf, sizeof(buf));
                if (ret >= 0)
                {
                    env.assign(buf, ret);
                }
                return ret;
            }

            /**
             * @brief Checks if a process with the given PID exists.
             *
             * @param pid The process ID to check.
             * @return int 1 if process exists, 0 if not, -1 on error.
             */
            static int exists(uint32_t pid)
            {
                return fossil_sys_process_exists(pid);
            }

            /**
             * @brief Suspends (pauses) a process by PID.
             *
             * @param pid The process ID to suspend.
             * @return int 0 on success, negative error code on failure.
             */
            static int suspend(uint32_t pid)
            {
                return fossil_sys_process_suspend(pid);
            }

            /**
             * @brief Resumes a suspended process by PID.
             *
             * @param pid The process ID to resume.
             * @return int 0 on success, negative error code on failure.
             */
            static int resume(uint32_t pid)
            {
                return fossil_sys_process_resume(pid);
            }

            /**
             * @brief Changes the priority (nice value) of a process.
             *
             * @param pid The process ID.
             * @param priority The new priority value.
             * @return int 0 on success, negative error code on failure.
             */
            static int set_priority(uint32_t pid, int priority)
            {
                return fossil_sys_process_set_priority(pid, priority);
            }

            /**
             * @brief Gets the priority (nice value) of a process.
             *
             * @param pid The process ID.
             * @param priority Reference to store the current priority value.
             * @return int 0 on success, negative error code on failure.
             */
            static int get_priority(uint32_t pid, int &priority)
            {
                return fossil_sys_process_get_priority(pid, &priority);
            }

            /**
             * @brief Waits for a process to exit.
             *
             * @param pid The process ID.
             * @param exit_code Reference to store the exit code (optional).
             * @param timeout_ms Timeout in milliseconds, or -1 for infinite.
             * @return int 0 on success, negative error code on failure.
             */
            static int wait(uint32_t pid, int *exit_code, int timeout_ms)
            {
                return fossil_sys_process_wait(pid, exit_code, timeout_ms);
            }

            /**
             * @brief Starts a new process.
             *
             * @param path Path to executable.
             * @param argv Argument vector (NULL-terminated).
             * @param envp Environment vector (NULL-terminated, can be NULL).
             * @param pid_out Reference to store new process ID.
             * @return int 0 on success, negative error code on failure.
             */
            static int spawn(const char *path, char *const argv[], char *const envp[], uint32_t &pid_out)
            {
                return fossil_sys_process_spawn(path, argv, envp, &pid_out);
            }

            /**
             * @brief Gets the executable path of a process.
             *
             * @param pid The process ID.
             * @param exe_path Reference to a std::string to store the path.
             * @return int 0 on success, negative error code on failure.
             */
            static int get_exe_path(uint32_t pid, std::string &exe_path)
            {
                char buf[FOSSIL_SYS_PROCESS_ENV_MAX] = {0};
                int ret = fossil_sys_process_get_exe_path(pid, buf, sizeof(buf));
                if (ret == 0)
                {
                    exe_path = buf;
                }
                return ret;
            }
        };

    }

}

#endif

#endif /* FOSSIL_SYS_FRAMEWORK_H */
