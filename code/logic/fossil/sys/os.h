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
#ifndef FOSSIL_SYS_OS_H
#define FOSSIL_SYS_OS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/* ------------------------------------------------------
 * Types
 * ----------------------------------------------------- */
typedef struct {
    const char* id;         /* string ID for AI/meta usage */
    uint32_t pid;
    const char* name;
    size_t memory_usage;
    int status;             /* platform-independent status */
} fossil_sys_os_process_t;

/* ------------------------------------------------------
 * Functions
 * ----------------------------------------------------- */

/**
 * Spawn a new process with the given command string.
 * 
 * @param command The command string to execute (e.g., "ls -la")
 * @param out_process Pointer to process structure to populate with spawn info
 * @return true if process spawned successfully, false otherwise
 */
bool fossil_sys_os_spawn(const char* command, fossil_sys_os_process_t* out_process);

/**
 * Terminate a running process by its process ID.
 * 
 * @param pid The process ID to terminate
 * @return true if process was terminated successfully, false otherwise
 */
bool fossil_sys_os_kill(uint32_t pid);

/**
 * Retrieve a list of all currently running processes on the system.
 * 
 * @param buffer Pointer to array of process structures to populate
 * @param max_count Maximum number of processes to retrieve
 * @return Number of processes actually retrieved and populated in buffer
 */
size_t fossil_sys_os_list(fossil_sys_os_process_t* buffer, size_t max_count);

/**
 * Get the total system uptime since last boot.
 * 
 * @return System uptime in seconds
 */
uint64_t fossil_sys_os_uptime(void);

/**
 * Yield the current thread's execution time to the scheduler.
 * Allows other threads to run without blocking.
 */
void fossil_sys_os_yield(void);

#ifdef __cplusplus
}

namespace fossil::sys {
    
class Os {
public:
    /** 
     * Spawn a new process with the given command string.
     * 
     * @param command The command string to execute (e.g., "ls -la")
     * @param out_process Pointer to process structure to populate with spawn info
     * @return true if process spawned successfully, false otherwise
     */
    static bool spawn(const char* command, fossil_sys_os_process_t* out_process) {
        return fossil_sys_os_spawn(command, out_process);
    }

    /** 
     * Terminate a running process by its process ID.
     * 
     * @param pid The process ID to terminate
     * @return true if process was terminated successfully, false otherwise
     */
    static bool kill(uint32_t pid) {
        return fossil_sys_os_kill(pid);
    }

    /** 
     * Retrieve a list of all currently running processes on the system.
     * 
     * @param buffer Pointer to array of process structures to populate
     * @param max_count Maximum number of processes to retrieve
     * @return Number of processes actually retrieved and populated in buffer
     */
    static size_t list(fossil_sys_os_process_t* buffer, size_t max_count) {
        return fossil_sys_os_list(buffer, max_count);
    }

    /** 
     * Get the total system uptime since last boot.
     * 
     * @return System uptime in seconds
     */
    static uint64_t uptime() {
        return fossil_sys_os_uptime();
    }

    /** 
     * Yield the current thread's execution time to the scheduler.
     * Allows other threads to run without blocking.
     */
    static void yield() {
        fossil_sys_os_yield();
    }
};

} // namespace fossil::sys

#endif

#endif /* FOSSIL_SYS_OS_H */
