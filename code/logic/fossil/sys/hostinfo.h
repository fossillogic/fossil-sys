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
#ifndef FOSSIL_SYS_HOSTINFO_H
#define FOSSIL_SYS_HOSTINFO_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

// System information structure
typedef struct {
    char os_name[128];
    char os_version[128];
    char kernel_version[128];
    char hostname[128];
    char username[128];
    char domain_name[128];
    char machine_type[128];
    char platform[128];
} fossil_sys_hostinfo_system_t;

// Architecture information structure
typedef struct {
    char architecture[128];
    char cpu[128];
    char cpu_cores[128];
    char cpu_threads[128];
    char cpu_frequency[128];
    char cpu_architecture[128];
} fossil_sys_hostinfo_architecture_t;

// Memory information structure
typedef struct {
    uint64_t total_memory;    // in bytes
    uint64_t free_memory;     // in bytes
    uint64_t used_memory;     // in bytes
    uint64_t available_memory;// in bytes
    uint64_t total_swap;      // in bytes
    uint64_t free_swap;       // in bytes
    uint64_t used_swap;       // in bytes
} fossil_sys_hostinfo_memory_t;

// Endianness information structure
typedef struct {
    int is_little_endian; // 1 if little-endian, 0 if big-endian
} fossil_sys_hostinfo_endianness_t;

/**
 * Power information structure
 */
typedef struct {
    int on_ac_power;         // 1 if on AC power, 0 otherwise
    int battery_present;     // 1 if battery is present, 0 otherwise
    int battery_charging;    // 1 if battery is charging, 0 otherwise
    int battery_percentage;  // Battery charge percentage (0-100), -1 if unknown
    int battery_seconds_left;// Estimated seconds left, -1 if unknown
} fossil_sys_hostinfo_power_t;

/**
 * CPU information structure
 */
typedef struct {
    char model[128];
    char vendor[128];
    int cores;
    int threads;
    float frequency_ghz;
    char features[256];
} fossil_sys_hostinfo_cpu_t;

/**
 * GPU information structure
 */
typedef struct {
    char name[128];
    char vendor[128];
    char driver_version[64];
    uint64_t memory_total;
    uint64_t memory_free;
} fossil_sys_hostinfo_gpu_t;

/**
 * Storage information structure
 */
typedef struct {
    char device_name[128];
    char mount_point[128];
    uint64_t total_space;    // in bytes
    uint64_t free_space;     // in bytes
    uint64_t used_space;     // in bytes
    char filesystem_type[64];
} fossil_sys_hostinfo_storage_t;

/**
 * Environment information structure
 */
typedef struct {
    char shell[128];
    char home_dir[256];
    char lang[64];
    char path[1024];
    char _term[64];
    char user[128];
} fossil_sys_hostinfo_environment_t;

typedef struct {
    int is_virtual_machine;     // 1 if VM detected
    int is_container;           // 1 if running in container
    char hypervisor[128];       // e.g. "KVM", "VMware", "Hyper-V"
    char container_type[64];    // e.g. "docker", "podman", "lxc"
} fossil_sys_hostinfo_virtualization_t;

typedef struct {
    uint64_t uptime_seconds;
    uint64_t boot_time_epoch; // seconds since Unix epoch, 0 if unknown
} fossil_sys_hostinfo_uptime_t;

// new
typedef struct {
    char hostname[128];
    char primary_ip[64];
    char mac_address[64];
    char interface_name[64];
    int is_up;
} fossil_sys_hostinfo_network_t;

typedef struct {
    int pid;
    int ppid;
    char executable_path[256];
    char current_working_dir[256];
    char process_name[128];
    int is_elevated; // admin/root
} fossil_sys_hostinfo_process_t;

typedef struct {
    uint64_t max_open_files;
    uint64_t max_processes;
    uint64_t page_size;
} fossil_sys_hostinfo_limits_t;

typedef struct {
    char timezone[64];
    int utc_offset_seconds;
    char locale[64];
} fossil_sys_hostinfo_time_t;

typedef struct {
    float load_avg_1m;
    float load_avg_5m;
    float load_avg_15m;
    float cpu_usage_percent;
} fossil_sys_hostinfo_load_t;

typedef struct {
    char manufacturer[128];
    char product_name[128];
    char serial_number[128];
    char bios_version[128];
} fossil_sys_hostinfo_hardware_t;

typedef struct {
    int display_count;
    int primary_width;
    int primary_height;
    int primary_refresh_rate;
} fossil_sys_hostinfo_display_t;

/**
 * @brief Retrieves the system uptime information.
 *
 * This function fills the provided fossil_sys_hostinfo_uptime_t structure
 * with the current uptime details of the host system. The uptime typically
 * represents the amount of time (in seconds or other units) that the system
 * has been running since its last boot.
 *
 * @param[out] info Pointer to a fossil_sys_hostinfo_uptime_t structure that
 *                  will be populated with the uptime information.
 *
 * @return 0 on success, or a negative error code on failure.
 */
int fossil_sys_hostinfo_get_uptime(fossil_sys_hostinfo_uptime_t *info);

/**
 * @brief Retrieves virtualization information about the host system.
 *
 * This function populates the given fossil_sys_hostinfo_virtualization_t
 * structure with details regarding the virtualization environment, if any,
 * in which the host system is running. This may include information such as
 * the type of hypervisor detected or whether the system is running on bare metal.
 *
 * @param[out] info Pointer to a fossil_sys_hostinfo_virtualization_t structure
 *                  that will be filled with virtualization details.
 *
 * @return 0 on success, or a negative error code on failure.
 */
int fossil_sys_hostinfo_get_virtualization(
    fossil_sys_hostinfo_virtualization_t *info
);

/**
 * @brief Retrieves storage information about the host system.
 *
 * This function fills the provided fossil_sys_hostinfo_storage_t structure
 * with details about the system's storage device, such as device name,
 * mount point, total space, free space, used space, and filesystem type.
 *
 * @param[out] info Pointer to a fossil_sys_hostinfo_storage_t structure that
 *                  will be populated with storage information.
 * @return 0 on success, or a negative error code on failure.
 */
int fossil_sys_hostinfo_get_storage(fossil_sys_hostinfo_storage_t *info);

/**
 * @brief Retrieves environment information for the current user session.
 *
 * This function populates the fossil_sys_hostinfo_environment_t structure
 * with details such as the user's shell, home directory, language, PATH,
 * terminal type, and username.
 *
 * @param[out] info Pointer to a fossil_sys_hostinfo_environment_t structure
 *                  that will be filled with environment information.
 * @return 0 on success, or a negative error code on failure.
 */
int fossil_sys_hostinfo_get_environment(fossil_sys_hostinfo_environment_t *info);

/**
 * @brief Retrieves CPU information about the host system.
 *
 * This function fills the fossil_sys_hostinfo_cpu_t structure with details
 * about the CPU, including model, vendor, number of cores, threads,
 * frequency, and supported features.
 *
 * @param[out] info Pointer to a fossil_sys_hostinfo_cpu_t structure that
 *                  will be populated with CPU information.
 * @return 0 on success, or a negative error code on failure.
 */
int fossil_sys_hostinfo_get_cpu(fossil_sys_hostinfo_cpu_t *info);

/**
 * @brief Retrieves GPU information about the host system.
 *
 * This function fills the fossil_sys_hostinfo_gpu_t structure with details
 * about the GPU, such as name, vendor, driver version, and memory statistics.
 *
 * @param[out] info Pointer to a fossil_sys_hostinfo_gpu_t structure that
 *                  will be populated with GPU information.
 * @return 0 on success, or a negative error code on failure.
 */
int fossil_sys_hostinfo_get_gpu(fossil_sys_hostinfo_gpu_t *info);

/**
 * @brief Retrieves power information about the host system.
 *
 * This function fills the fossil_sys_hostinfo_power_t structure with details
 * about the system's power state, including AC power status, battery presence,
 * charging state, battery percentage, and estimated time remaining.
 *
 * @param[out] info Pointer to a fossil_sys_hostinfo_power_t structure that
 *                  will be populated with power information.
 * @return 0 on success, or a negative error code on failure.
 */
int fossil_sys_hostinfo_get_power(fossil_sys_hostinfo_power_t *info);

/**
 * @brief Retrieves general system information.
 *
 * This function fills the fossil_sys_hostinfo_system_t structure with
 * information about the operating system, kernel version, hostname,
 * username, domain name, machine type, and platform.
 *
 * @param[out] info Pointer to a fossil_sys_hostinfo_system_t structure that
 *                  will be populated with system information.
 * @return 0 on success, or a negative error code on failure.
 */
int fossil_sys_hostinfo_get_system(fossil_sys_hostinfo_system_t *info);

/**
 * @brief Retrieves architecture information about the host system.
 *
 * This function fills the fossil_sys_hostinfo_architecture_t structure with
 * details about the system's architecture, such as architecture name, CPU,
 * number of cores and threads, frequency, and CPU architecture.
 *
 * @param[out] info Pointer to a fossil_sys_hostinfo_architecture_t structure
 *                  that will be populated with architecture information.
 * @return 0 on success, or a negative error code on failure.
 */
int fossil_sys_hostinfo_get_architecture(fossil_sys_hostinfo_architecture_t *info);

/**
 * @brief Retrieves memory information about the host system.
 *
 * This function fills the fossil_sys_hostinfo_memory_t structure with
 * details about the system's memory, including total, free, used, and
 * available memory, as well as swap statistics.
 *
 * @param[out] info Pointer to a fossil_sys_hostinfo_memory_t structure that
 *                  will be populated with memory information.
 * @return 0 on success, or a negative error code on failure.
 */
int fossil_sys_hostinfo_get_memory(fossil_sys_hostinfo_memory_t *info);

/**
 * @brief Retrieves endianness information about the host system.
 *
 * This function fills the fossil_sys_hostinfo_endianness_t structure with
 * information indicating whether the system is little-endian or big-endian.
 *
 * @param[out] info Pointer to a fossil_sys_hostinfo_endianness_t structure
 *                  that will be populated with endianness information.
 * @return 0 on success, or a negative error code on failure.
 */
int fossil_sys_hostinfo_get_endianness(fossil_sys_hostinfo_endianness_t *info);

/**
 * @brief Retrieves network information about the host system.
 *
 * This function fills the fossil_sys_hostinfo_network_t structure with
 * details about the primary network interface, including hostname,
 * IP address, MAC address, interface name, and link status.
 *
 * @param[out] info Pointer to a fossil_sys_hostinfo_network_t structure
 *                  that will be populated with network information.
 * @return 0 on success, or a negative error code on failure.
 */
int fossil_sys_hostinfo_get_network(fossil_sys_hostinfo_network_t *info);

/**
 * @brief Retrieves information about the current process.
 *
 * This function fills the fossil_sys_hostinfo_process_t structure with
 * details about the running process, including process ID, parent process ID,
 * executable path, working directory, process name, and privilege level.
 *
 * @param[out] info Pointer to a fossil_sys_hostinfo_process_t structure
 *                  that will be populated with process information.
 * @return 0 on success, or a negative error code on failure.
 */
int fossil_sys_hostinfo_get_process(fossil_sys_hostinfo_process_t *info);

/**
 * @brief Retrieves system resource limits.
 *
 * This function fills the fossil_sys_hostinfo_limits_t structure with
 * information about system-imposed limits such as maximum open files,
 * maximum processes, and memory page size.
 *
 * @param[out] info Pointer to a fossil_sys_hostinfo_limits_t structure
 *                  that will be populated with limits information.
 * @return 0 on success, or a negative error code on failure.
 */
int fossil_sys_hostinfo_get_limits(fossil_sys_hostinfo_limits_t *info);

/**
 * @brief Retrieves time and locale information.
 *
 * This function fills the fossil_sys_hostinfo_time_t structure with
 * details about the system timezone, UTC offset, and locale settings.
 *
 * @param[out] info Pointer to a fossil_sys_hostinfo_time_t structure
 *                  that will be populated with time and locale information.
 * @return 0 on success, or a negative error code on failure.
 */
int fossil_sys_hostinfo_get_time(fossil_sys_hostinfo_time_t *info);

/**
 * @brief Retrieves system load and performance metrics.
 *
 * This function fills the fossil_sys_hostinfo_load_t structure with
 * system load averages and CPU usage statistics.
 *
 * @param[out] info Pointer to a fossil_sys_hostinfo_load_t structure
 *                  that will be populated with load information.
 * @return 0 on success, or a negative error code on failure.
 */
int fossil_sys_hostinfo_get_load(fossil_sys_hostinfo_load_t *info);

/**
 * @brief Retrieves hardware information about the host system.
 *
 * This function fills the fossil_sys_hostinfo_hardware_t structure with
 * details about the system manufacturer, product name, serial number,
 * and BIOS or firmware version.
 *
 * @param[out] info Pointer to a fossil_sys_hostinfo_hardware_t structure
 *                  that will be populated with hardware information.
 * @return 0 on success, or a negative error code on failure.
 */
int fossil_sys_hostinfo_get_hardware(fossil_sys_hostinfo_hardware_t *info);

/**
 * @brief Retrieves display information about the host system.
 *
 * This function fills the fossil_sys_hostinfo_display_t structure with
 * details about connected displays, including display count, primary
 * resolution, and refresh rate.
 *
 * @param[out] info Pointer to a fossil_sys_hostinfo_display_t structure
 *                  that will be populated with display information.
 * @return 0 on success, or a negative error code on failure.
 */
int fossil_sys_hostinfo_get_display(fossil_sys_hostinfo_display_t *info);

#ifdef __cplusplus
}

/**
 * Fossil namespace.
 */
namespace fossil {

    /**
     * System namespace.
     */
    namespace sys {

        /**
         * Hostinfo management class.
         */
        class Hostinfo {
        public:

            /**
             * @brief Retrieves general system information.
             *
             * This function returns a structure containing details about the
             * operating system, kernel version, hostname, username, domain name,
             * machine type, and platform of the host system.
             *
             * @return A structure containing system information.
             */
            static fossil_sys_hostinfo_system_t get_system() {
                fossil_sys_hostinfo_system_t info;
                fossil_sys_hostinfo_get_system(&info);
                return info;
            }

            /**
             * @brief Retrieves architecture information about the host system.
             *
             * This function returns a structure with details about the system's
             * architecture, such as architecture name, CPU, number of cores and
             * threads, frequency, and CPU architecture.
             *
             * @return A structure containing architecture information.
             */
            static fossil_sys_hostinfo_architecture_t get_architecture() {
                fossil_sys_hostinfo_architecture_t info;
                fossil_sys_hostinfo_get_architecture(&info);
                return info;
            }

            /**
             * @brief Retrieves memory information about the host system.
             *
             * This function returns a structure with details about the system's
             * memory, including total, free, used, and available memory, as well
             * as swap statistics.
             *
             * @return A structure containing memory information.
             */
            static fossil_sys_hostinfo_memory_t get_memory() {
                fossil_sys_hostinfo_memory_t info;
                fossil_sys_hostinfo_get_memory(&info);
                return info;
            }

            /**
             * @brief Retrieves endianness information about the host system.
             *
             * This function returns a structure indicating whether the system is
             * little-endian or big-endian.
             *
             * @return A structure containing endianness information.
             */
            static fossil_sys_hostinfo_endianness_t get_endianness() {
                fossil_sys_hostinfo_endianness_t info;
                fossil_sys_hostinfo_get_endianness(&info);
                return info;
            }

            /**
             * @brief Retrieves CPU information about the host system.
             *
             * This function returns a structure with details about the CPU,
             * including model, vendor, number of cores, threads, frequency, and
             * supported features.
             *
             * @return A structure containing CPU information.
             */
            static fossil_sys_hostinfo_cpu_t get_cpu() {
                fossil_sys_hostinfo_cpu_t info;
                fossil_sys_hostinfo_get_cpu(&info);
                return info;
            }

            /**
             * @brief Retrieves GPU information about the host system.
             *
             * This function returns a structure with details about the GPU,
             * such as name, vendor, driver version, and memory statistics.
             *
             * @return A structure containing GPU information.
             */
            static fossil_sys_hostinfo_gpu_t get_gpu() {
                fossil_sys_hostinfo_gpu_t info;
                fossil_sys_hostinfo_get_gpu(&info);
                return info;
            }

            /**
             * @brief Retrieves power information about the host system.
             *
             * This function returns a structure with details about the system's
             * power state, including AC power status, battery presence, charging
             * state, battery percentage, and estimated time remaining.
             *
             * @return A structure containing power information.
             */
            static fossil_sys_hostinfo_power_t get_power() {
                fossil_sys_hostinfo_power_t info;
                fossil_sys_hostinfo_get_power(&info);
                return info;
            }

            /**
             * @brief Retrieves storage information about the host system.
             *
             * This function returns a structure with details about the system's
             * storage device, such as device name, mount point, total space,
             * free space, used space, and filesystem type.
             *
             * @return A structure containing storage information.
             */
            static fossil_sys_hostinfo_storage_t get_storage() {
                fossil_sys_hostinfo_storage_t info;
                fossil_sys_hostinfo_get_storage(&info);
                return info;
            }

            /**
             * @brief Retrieves environment information for the current user session.
             *
             * This function returns a structure with details such as the user's
             * shell, home directory, language, PATH, terminal type, and username.
             *
             * @return A structure containing environment information.
             */
            static fossil_sys_hostinfo_environment_t get_environment() {
                fossil_sys_hostinfo_environment_t info;
                fossil_sys_hostinfo_get_environment(&info);
                return info;
            }

            /**
             * @brief Retrieves the system uptime information.
             *
             * This function returns a structure with the current uptime details
             * of the host system, including the amount of time (in seconds) that
             * the system has been running since its last boot and the boot time
             * in epoch seconds.
             *
             * @return A structure containing uptime information.
             */
            static fossil_sys_hostinfo_uptime_t get_uptime() {
                fossil_sys_hostinfo_uptime_t info;
                fossil_sys_hostinfo_get_uptime(&info);
                return info;
            }

            /**
             * @brief Retrieves virtualization information about the host system.
             *
             * This function returns a structure with details regarding the
             * virtualization environment, if any, in which the host system is
             * running. This may include information such as the type of hypervisor
             * detected or whether the system is running on bare metal or in a
             * container.
             *
             * @return A structure containing virtualization information.
             */
            static fossil_sys_hostinfo_virtualization_t get_virtualization() {
                fossil_sys_hostinfo_virtualization_t info;
                fossil_sys_hostinfo_get_virtualization(&info);
                return info;
            }

        };

    }

}

#endif

#endif /* FOSSIL_SYS_FRAMEWORK_H */
