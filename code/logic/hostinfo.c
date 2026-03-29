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
#include "fossil/sys/hostinfo.h"

#if defined(__APPLE__)
// Must define this **before including any headers** to get getloadavg
#ifndef _DARWIN_C_SOURCE
#define _DARWIN_C_SOURCE
#endif
#endif

#include <stdio.h>
#include <stdlib.h> // for getloadavg on Linux and general use
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <errno.h>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <windows.h>
#include <tchar.h>
#include <sysinfoapi.h>
#elif defined(__APPLE__)
#define _DARWIN_C_SOURCE
#include <unistd.h>
#include <sys/utsname.h>
#include <sys/types.h>
#include <sys/sysctl.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/sysctl.h>
#include <mach/mach.h>
#include <mach/mach_host.h>
#include <mach/vm_statistics.h>
#include <mach/vm_types.h>
#else
// Unix/Linux
#include <unistd.h>
#include <sys/utsname.h>
#include <sys/types.h>
#include <sys/sysinfo.h>
#include <sys/statvfs.h>
#include <arpa/inet.h>
#include <locale.h>
#include <netdb.h>

#include <fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <linux/kd.h>
#include <linux/fb.h>
#endif

#ifdef _WIN32
#include <windows.h>
#include <dxgi.h> // DirectX Graphics Infrastructure
#include <initguid.h>
#elif defined(__APPLE__)
#include <IOKit/IOKitLib.h>
#include <IOKit/ps/IOPowerSources.h>
#include <IOKit/ps/IOPSKeys.h>
#include <IOKit/graphics/IOGraphicsLib.h>
#include <CoreFoundation/CoreFoundation.h>
#include <CoreGraphics/CoreGraphics.h>
#endif

#include <fcntl.h> // for open(), O_RDONLY
#include <ctype.h> // for isspace()
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h> // for setlocale and LC_ALL

static void fossil_sys_zero(void *ptr, size_t size)
{
    if (ptr)
    {
        memset(ptr, 0, size);
    }
}

int fossil_sys_hostinfo_get_storage(fossil_sys_hostinfo_storage_t *info)
{
    if (!info)
        return -1;
    memset(info, 0, sizeof(*info));
#ifdef _WIN32
    // Use GetDiskFreeSpaceEx for C:
    ULARGE_INTEGER freeBytesAvailable, totalNumberOfBytes, totalNumberOfFreeBytes;
    if (GetDiskFreeSpaceExA("C:\\", &freeBytesAvailable, &totalNumberOfBytes, &totalNumberOfFreeBytes))
    {
        strncpy(info->device_name, "C:", sizeof(info->device_name) - 1);
        strncpy(info->mount_point, "C:\\", sizeof(info->mount_point) - 1);
        info->total_space = totalNumberOfBytes.QuadPart;
        info->free_space = totalNumberOfFreeBytes.QuadPart;
        info->used_space = info->total_space - info->free_space;
        strncpy(info->filesystem_type, "NTFS", sizeof(info->filesystem_type) - 1); // Guess
    }
    else
    {
        strncpy(info->device_name, "Unknown", sizeof(info->device_name) - 1);
        strncpy(info->mount_point, "Unknown", sizeof(info->mount_point) - 1);
        info->total_space = 0;
        info->free_space = 0;
        info->used_space = 0;
        strncpy(info->filesystem_type, "Unknown", sizeof(info->filesystem_type) - 1);
        return -1;
    }
#elif defined(__APPLE__) || defined(__unix__) || defined(__linux__)
// Use statvfs for "/"
#include <sys/statvfs.h>
    struct statvfs vfs;
    if (statvfs("/", &vfs) == 0)
    {
        strncpy(info->device_name, "/", sizeof(info->device_name) - 1);
        strncpy(info->mount_point, "/", sizeof(info->mount_point) - 1);
        info->total_space = (uint64_t)vfs.f_frsize * vfs.f_blocks;
        info->free_space = (uint64_t)vfs.f_frsize * vfs.f_bfree;
        info->used_space = info->total_space - info->free_space;
        // Try to get filesystem type (Linux only)
#if defined(__linux__)
        FILE *fp = fopen("/proc/mounts", "r");
        if (fp)
        {
            char line[512];
            while (fgets(line, sizeof(line), fp))
            {
                char dev[128], mnt[128], fstype[64];
                if (sscanf(line, "%127s %127s %63s", dev, mnt, fstype) == 3)
                {
                    if (strcmp(mnt, "/") == 0)
                    {
                        snprintf(info->filesystem_type, sizeof(info->filesystem_type), "%s", fstype);
                        break;
                    }
                }
            }
            fclose(fp);
        }
        if (info->filesystem_type[0] == '\0')
            strncpy(info->filesystem_type, "Unknown", sizeof(info->filesystem_type) - 1);
#else
        strncpy(info->filesystem_type, "Unknown", sizeof(info->filesystem_type) - 1);
#endif
    }
    else
    {
        strncpy(info->device_name, "Unknown", sizeof(info->device_name) - 1);
        strncpy(info->mount_point, "Unknown", sizeof(info->mount_point) - 1);
        info->total_space = 0;
        info->free_space = 0;
        info->used_space = 0;
        strncpy(info->filesystem_type, "Unknown", sizeof(info->filesystem_type) - 1);
        return -1;
    }
#else
    strncpy(info->device_name, "Unknown", sizeof(info->device_name) - 1);
    strncpy(info->mount_point, "Unknown", sizeof(info->mount_point) - 1);
    info->total_space = 0;
    info->free_space = 0;
    info->used_space = 0;
    strncpy(info->filesystem_type, "Unknown", sizeof(info->filesystem_type) - 1);
    return -1;
#endif
    return 0;
}

int fossil_sys_hostinfo_get_environment(fossil_sys_hostinfo_environment_t *info)
{
    if (!info)
        return -1;
    memset(info, 0, sizeof(*info));
#ifdef _WIN32
    // Get environment variables
    DWORD len;
    len = GetEnvironmentVariableA("COMSPEC", info->shell, sizeof(info->shell));
    if (len == 0)
        strncpy(info->shell, "Unknown", sizeof(info->shell) - 1);

    len = GetEnvironmentVariableA("USERPROFILE", info->home_dir, sizeof(info->home_dir));
    if (len == 0)
        strncpy(info->home_dir, "Unknown", sizeof(info->home_dir) - 1);

    len = GetEnvironmentVariableA("LANG", info->lang, sizeof(info->lang));
    if (len == 0)
        strncpy(info->lang, "Unknown", sizeof(info->lang) - 1);

    len = GetEnvironmentVariableA("PATH", info->path, sizeof(info->path));
    if (len == 0)
        strncpy(info->path, "Unknown", sizeof(info->path) - 1);

    len = GetEnvironmentVariableA("TERM", info->_term, sizeof(info->_term));
    if (len == 0)
        strncpy(info->_term, "Unknown", sizeof(info->_term) - 1);

    len = GetEnvironmentVariableA("USERNAME", info->user, sizeof(info->user));
    if (len == 0)
        strncpy(info->user, "Unknown", sizeof(info->user) - 1);

#elif defined(__APPLE__) || defined(__unix__) || defined(__linux__)
    const char *val;
    val = getenv("SHELL");
    if (val)
        strncpy(info->shell, val, sizeof(info->shell) - 1);
    else
        strncpy(info->shell, "Unknown", sizeof(info->shell) - 1);

    val = getenv("HOME");
    if (val)
        strncpy(info->home_dir, val, sizeof(info->home_dir) - 1);
    else
        strncpy(info->home_dir, "Unknown", sizeof(info->home_dir) - 1);

    val = getenv("LANG");
    if (val)
        strncpy(info->lang, val, sizeof(info->lang) - 1);
    else
        strncpy(info->lang, "Unknown", sizeof(info->lang) - 1);

    val = getenv("PATH");
    if (val)
        strncpy(info->path, val, sizeof(info->path) - 1);
    else
        strncpy(info->path, "Unknown", sizeof(info->path) - 1);

    val = getenv("TERM");
    if (val)
        strncpy(info->_term, val, sizeof(info->_term) - 1);
    else
        strncpy(info->_term, "Unknown", sizeof(info->_term) - 1);

    val = getenv("USER");
    if (val)
        strncpy(info->user, val, sizeof(info->user) - 1);
    else
        strncpy(info->user, "Unknown", sizeof(info->user) - 1);
#else
    strncpy(info->shell, "Unknown", sizeof(info->shell) - 1);
    strncpy(info->home_dir, "Unknown", sizeof(info->home_dir) - 1);
    strncpy(info->lang, "Unknown", sizeof(info->lang) - 1);
    strncpy(info->path, "Unknown", sizeof(info->path) - 1);
    strncpy(info->_term, "Unknown", sizeof(info->_term) - 1);
    strncpy(info->user, "Unknown", sizeof(info->user) - 1);
#endif
    return 0;
}

int fossil_sys_hostinfo_get_cpu(fossil_sys_hostinfo_cpu_t *info)
{
    if (!info)
        return -1;
    memset(info, 0, sizeof(*info));
#ifdef _WIN32
    // Windows: limited info
    SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);
    info->cores = sysinfo.dwNumberOfProcessors;
    info->threads = sysinfo.dwNumberOfProcessors;
    strncpy(info->model, "Unknown", sizeof(info->model) - 1);
    strncpy(info->vendor, "Unknown", sizeof(info->vendor) - 1);
    info->frequency_ghz = 0.0f;
    strncpy(info->features, "Unknown", sizeof(info->features) - 1);
#elif defined(__APPLE__)
    size_t size;
    // Model
    size = sizeof(info->model);
    if (sysctlbyname("machdep.cpu.brand_string", info->model, &size, NULL, 0) != 0)
        strncpy(info->model, "Unknown", sizeof(info->model) - 1);
    info->model[sizeof(info->model) - 1] = '\0';
    // Vendor
    size = sizeof(info->vendor);
    if (sysctlbyname("machdep.cpu.vendor", info->vendor, &size, NULL, 0) != 0)
        strncpy(info->vendor, "Unknown", sizeof(info->vendor) - 1);
    info->vendor[sizeof(info->vendor) - 1] = '\0';
    // Cores
    int cores = 0;
    size = sizeof(cores);
    if (sysctlbyname("hw.physicalcpu", &cores, &size, NULL, 0) == 0)
        info->cores = cores;
    else
        info->cores = 0;
    // Threads
    int threads = 0;
    size = sizeof(threads);
    if (sysctlbyname("hw.logicalcpu", &threads, &size, NULL, 0) == 0)
        info->threads = threads;
    else
        info->threads = 0;
    // Frequency
    uint64_t freq = 0;
    size = sizeof(freq);
    if (sysctlbyname("hw.cpufrequency", &freq, &size, NULL, 0) == 0)
        info->frequency_ghz = (float)freq / 1e9f;
    else
        info->frequency_ghz = 0.0f;
    // Features
    size = sizeof(info->features);
    if (sysctlbyname("machdep.cpu.features", info->features, &size, NULL, 0) != 0)
        strncpy(info->features, "Unknown", sizeof(info->features) - 1);
    info->features[sizeof(info->features) - 1] = '\0';
#else
    // Linux
    FILE *fp = fopen("/proc/cpuinfo", "r");
    if (!fp)
        return -1;
    char line[256];
    info->cores = 0;
    info->threads = 0;
    info->frequency_ghz = 0.0f;
    info->model[0] = info->vendor[0] = info->features[0] = '\0';
    int found_model = 0, found_vendor = 0, found_freq = 0, found_flags = 0;
    while (fgets(line, sizeof(line), fp))
    {
        if (!found_model && strncmp(line, "model name", 10) == 0)
        {
            char *colon = strchr(line, ':');
            if (colon)
            {
                colon++;
                while (*colon == ' ')
                    colon++;
                strncpy(info->model, colon, sizeof(info->model) - 1);
                info->model[sizeof(info->model) - 1] = '\0';
                found_model = 1;
            }
        }
        else if (!found_vendor && strncmp(line, "vendor_id", 9) == 0)
        {
            char *colon = strchr(line, ':');
            if (colon)
            {
                colon++;
                while (*colon == ' ')
                    colon++;
                strncpy(info->vendor, colon, sizeof(info->vendor) - 1);
                info->vendor[sizeof(info->vendor) - 1] = '\0';
                found_vendor = 1;
            }
        }
        else if (!found_freq && strncmp(line, "cpu MHz", 7) == 0)
        {
            char *colon = strchr(line, ':');
            if (colon)
            {
                colon++;
                while (*colon == ' ')
                    colon++;
                float mhz = strtof(colon, NULL);
                info->frequency_ghz = mhz / 1000.0f;
                found_freq = 1;
            }
        }
        else if (!found_flags && strncmp(line, "flags", 5) == 0)
        {
            char *colon = strchr(line, ':');
            if (colon)
            {
                colon++;
                while (*colon == ' ')
                    colon++;
                strncpy(info->features, colon, sizeof(info->features) - 1);
                info->features[sizeof(info->features) - 1] = '\0';
                found_flags = 1;
            }
        }
        else if (strncmp(line, "processor", 9) == 0)
        {
            info->threads++;
        }
        else if (strncmp(line, "cpu cores", 9) == 0)
        {
            char *colon = strchr(line, ':');
            if (colon)
            {
                colon++;
                while (*colon == ' ')
                    colon++;
                info->cores = atoi(colon);
            }
        }
    }
    fclose(fp);
    if (info->cores == 0)
        info->cores = info->threads ? info->threads : 1;
    if (info->threads == 0)
        info->threads = info->cores;
    if (!found_model)
        strncpy(info->model, "Unknown", sizeof(info->model) - 1);
    if (!found_vendor)
        strncpy(info->vendor, "Unknown", sizeof(info->vendor) - 1);
    if (!found_freq)
        info->frequency_ghz = 0.0f;
    if (!found_flags)
        strncpy(info->features, "Unknown", sizeof(info->features) - 1);
#endif
    return 0;
}

int fossil_sys_hostinfo_get_gpu(fossil_sys_hostinfo_gpu_t *info)
{
    if (!info)
        return -1;
    memset(info, 0, sizeof(*info));
#ifdef _WIN32
    // Use DXGI to enumerate adapters
    IDXGIFactory *factory = NULL;
    if (SUCCEEDED(CreateDXGIFactory(&IID_IDXGIFactory, (void **)&factory)))
    {
        IDXGIAdapter *adapter = NULL;
        if (factory->lpVtbl->EnumAdapters(factory, 0, &adapter) == S_OK)
        { // first GPU
            DXGI_ADAPTER_DESC desc;
            if (adapter->lpVtbl->GetDesc(adapter, &desc) == S_OK)
            {
                // Copy strings
                wcstombs(info->name, desc.Description, sizeof(info->name) - 1);
                info->name[sizeof(info->name) - 1] = '\0';
                strncpy(info->vendor, "Unknown", sizeof(info->vendor) - 1); // Vendor ID can be parsed if desired
                info->vendor[sizeof(info->vendor) - 1] = '\0';
                snprintf(info->driver_version, sizeof(info->driver_version), "DeviceID: %u", desc.DeviceId);

                info->memory_total = desc.DedicatedVideoMemory;
                info->memory_free = 0; // DXGI doesn't provide free memory directly
            }
            adapter->lpVtbl->Release(adapter);
        }
        factory->lpVtbl->Release(factory);
    }
#elif defined(__APPLE__)
    // macOS: Use IOKit to find GPU devices
    CFMutableDictionaryRef matchDict = IOServiceMatching("IOPCIDevice");
    if (matchDict)
    {
        io_iterator_t iter;
        // Use kIOMainPortDefault for macOS 12+ compatibility
        if (IOServiceGetMatchingServices(kIOMainPortDefault, matchDict, &iter) == KERN_SUCCESS)
        {
            io_object_t service;
            while ((service = IOIteratorNext(iter)))
            {
                CFStringRef model = (CFStringRef)IORegistryEntryCreateCFProperty(
                    service, CFSTR("model"), kCFAllocatorDefault, 0);
                if (model)
                {
                    CFIndex length = CFStringGetLength(model);
                    CFIndex maxSize = CFStringGetMaximumSizeForEncoding(length, kCFStringEncodingUTF8) + 1;
                    char buffer[256];
                    // Cast sizeof(buffer) to CFIndex for comparison
                    if (maxSize < (CFIndex)sizeof(buffer))
                    {
                        if (CFStringGetCString(model, buffer, sizeof(buffer), kCFStringEncodingUTF8))
                        {
                            strncpy(info->name, buffer, sizeof(info->name) - 1);
                            strncpy(info->vendor, "Apple/AMD/NVIDIA", sizeof(info->vendor) - 1);
                            strncpy(info->driver_version, "Unknown", sizeof(info->driver_version) - 1);
                            info->memory_total = 0; // macOS doesn't expose VRAM easily
                            info->memory_free = 0;
                            CFRelease(model);
                            IOObjectRelease(service);
                            break; // just pick the first GPU
                        }
                    }
                    CFRelease(model);
                }
                IOObjectRelease(service);
            }
            IOObjectRelease(iter);
        }
    }
#else
    // Linux: Try to parse lspci and glxinfo output if available
    FILE *fp = popen("lspci -mm | grep VGA", "r");
    if (fp)
    {
        char line[512];
        if (fgets(line, sizeof(line), fp))
        {
            // Example: 00:02.0 "VGA compatible controller" "Intel Corporation" "HD Graphics 620" ...
            char *vendor = strchr(line, '"');
            if (vendor)
            {
                vendor++;
                vendor = strchr(vendor, '"');
                if (vendor)
                {
                    vendor++;
                    vendor = strchr(vendor, '"');
                    if (vendor)
                    {
                        vendor++;
                        char *vendor_end = strchr(vendor, '"');
                        if (vendor_end && vendor_end - vendor < (int)sizeof(info->vendor))
                        {
                            strncpy(info->vendor, vendor, vendor_end - vendor);
                            info->vendor[vendor_end - vendor] = '\0';
                        }
                        char *name = strchr(vendor_end + 1, '"');
                        if (name)
                        {
                            name++;
                            char *name_end = strchr(name, '"');
                            if (name_end && name_end - name < (int)sizeof(info->name))
                            {
                                strncpy(info->name, name, name_end - name);
                                info->name[name_end - name] = '\0';
                            }
                        }
                    }
                }
            }
        }
        pclose(fp);
    }
    if (info->name[0] == '\0')
        strncpy(info->name, "Unknown", sizeof(info->name) - 1);
    if (info->vendor[0] == '\0')
        strncpy(info->vendor, "Unknown", sizeof(info->vendor) - 1);

    // Try to get driver version from glxinfo
    fp = popen("glxinfo -B 2>/dev/null | grep 'OpenGL version string'", "r");
    if (fp)
    {
        char line[256];
        if (fgets(line, sizeof(line), fp))
        {
            char *colon = strchr(line, ':');
            if (colon)
            {
                colon++;
                while (*colon == ' ')
                    colon++;
                strncpy(info->driver_version, colon, sizeof(info->driver_version) - 1);
                info->driver_version[sizeof(info->driver_version) - 1] = '\0';
                // Remove trailing newline
                char *nl = strchr(info->driver_version, '\n');
                if (nl)
                    *nl = '\0';
            }
        }
        pclose(fp);
    }
    if (info->driver_version[0] == '\0')
        strncpy(info->driver_version, "Unknown", sizeof(info->driver_version) - 1);

    // Memory info: not portable, set to 0
    info->memory_total = 0;
    info->memory_free = 0;
#endif
    return 0;
}

int fossil_sys_hostinfo_get_power(fossil_sys_hostinfo_power_t *info)
{
    if (!info)
        return -1;
#ifdef _WIN32
    SYSTEM_POWER_STATUS status;
    if (!GetSystemPowerStatus(&status))
        return -1;

    info->on_ac_power = (status.ACLineStatus == 1) ? 1 : 0;
    info->battery_present = (status.BatteryFlag != 128) ? 1 : 0; // 128 = no battery
    info->battery_charging = (status.BatteryFlag & 8) ? 1 : 0;   // 8 = charging
    info->battery_percentage = (status.BatteryLifePercent != 255) ? status.BatteryLifePercent : -1;
    info->battery_seconds_left = (status.BatteryLifeTime != (DWORD)-1) ? (int)status.BatteryLifeTime : -1;

#elif defined(__APPLE__)
    // On macOS, use IOKit to get battery info
    CFTypeRef infoRef = IOPSCopyPowerSourcesInfo();
    if (!infoRef)
    {
        info->on_ac_power = -1;
        info->battery_present = -1;
        info->battery_charging = -1;
        info->battery_percentage = -1;
        info->battery_seconds_left = -1;
    }
    else
    {
        CFArrayRef powerSourcesList = IOPSCopyPowerSourcesList(infoRef);
        if (!powerSourcesList || CFArrayGetCount(powerSourcesList) == 0)
        {
            info->on_ac_power = -1;
            info->battery_present = 0;
            info->battery_charging = -1;
            info->battery_percentage = -1;
            info->battery_seconds_left = -1;
        }
        else
        {
            info->battery_present = 1;
            CFDictionaryRef ps = IOPSGetPowerSourceDescription(infoRef, CFArrayGetValueAtIndex(powerSourcesList, 0));
            if (ps)
            {
                CFStringRef psType = CFDictionaryGetValue(ps, CFSTR(kIOPSTypeKey));
                if (psType && CFStringCompare(psType, CFSTR(kIOPSInternalBatteryType), 0) == kCFCompareEqualTo)
                {
                    // AC Power
                    CFStringRef powerSourceState = CFDictionaryGetValue(ps, CFSTR(kIOPSPowerSourceStateKey));
                    if (powerSourceState && CFStringCompare(powerSourceState, CFSTR(kIOPSACPowerValue), 0) == kCFCompareEqualTo)
                        info->on_ac_power = 1;
                    else
                        info->on_ac_power = 0;

                    // Charging
                    CFBooleanRef charging = CFDictionaryGetValue(ps, CFSTR(kIOPSIsChargingKey));
                    if (charging)
                        info->battery_charging = CFBooleanGetValue(charging) ? 1 : 0;
                    else
                        info->battery_charging = -1;

                    // Percentage
                    CFNumberRef percent = CFDictionaryGetValue(ps, CFSTR(kIOPSCurrentCapacityKey));
                    CFNumberRef max = CFDictionaryGetValue(ps, CFSTR(kIOPSMaxCapacityKey));
                    int cur = 0, maxcap = 0;
                    if (percent && max && CFNumberGetValue(percent, kCFNumberIntType, &cur) && CFNumberGetValue(max, kCFNumberIntType, &maxcap) && maxcap > 0)
                        info->battery_percentage = (int)((cur * 100) / maxcap);
                    else
                        info->battery_percentage = -1;

                    // Time remaining
                    CFNumberRef timeToEmpty = CFDictionaryGetValue(ps, CFSTR(kIOPSTimeToEmptyKey));
                    int secs = -1;
                    if (timeToEmpty && CFNumberGetValue(timeToEmpty, kCFNumberIntType, &secs))
                        info->battery_seconds_left = secs >= 0 ? secs * 60 : -1;
                    else
                        info->battery_seconds_left = -1;
                }
                else
                {
                    info->on_ac_power = -1;
                    info->battery_present = 0;
                    info->battery_charging = -1;
                    info->battery_percentage = -1;
                    info->battery_seconds_left = -1;
                }
            }
            else
            {
                info->on_ac_power = -1;
                info->battery_present = 0;
                info->battery_charging = -1;
                info->battery_percentage = -1;
                info->battery_seconds_left = -1;
            }
        }
        if (powerSourcesList)
            CFRelease(powerSourcesList);
        CFRelease(infoRef);
    }
#else
    // On Linux, try to read from /sys/class/power_supply
    FILE *fp;
    char path[256];
    int found_battery = 0;
    info->on_ac_power = -1;
    info->battery_present = 0;
    info->battery_charging = -1;
    info->battery_percentage = -1;
    info->battery_seconds_left = -1;

    // Check AC status
    fp = fopen("/sys/class/power_supply/AC/online", "r");
    if (!fp)
        fp = fopen("/sys/class/power_supply/ACAD/online", "r");
    if (!fp)
        fp = fopen("/sys/class/power_supply/Mains/online", "r");
    if (fp)
    {
        int ac = 0;
        if (fscanf(fp, "%d", &ac) == 1)
            info->on_ac_power = ac;
        fclose(fp);
    }

    // Try to find a battery directory
    for (int i = 0; i < 2; ++i)
    {
        snprintf(path, sizeof(path), "/sys/class/power_supply/BAT%d/", i);
        FILE *test = fopen(path, "r");
        if (test)
        {
            found_battery = 1;
            fclose(test);
            snprintf(path, sizeof(path), "/sys/class/power_supply/BAT%d/status", i);
            fp = fopen(path, "r");
            if (fp)
            {
                char status[32];
                if (fgets(status, sizeof(status), fp))
                {
                    if (strstr(status, "Charging"))
                        info->battery_charging = 1;
                    else if (strstr(status, "Discharging"))
                        info->battery_charging = 0;
                    else
                        info->battery_charging = -1;
                }
                fclose(fp);
            }
            snprintf(path, sizeof(path), "/sys/class/power_supply/BAT%d/capacity", i);
            fp = fopen(path, "r");
            if (fp)
            {
                int percent = -1;
                if (fscanf(fp, "%d", &percent) == 1)
                    info->battery_percentage = percent;
                fclose(fp);
            }
            snprintf(path, sizeof(path), "/sys/class/power_supply/BAT%d/time_to_empty_now", i);
            fp = fopen(path, "r");
            if (fp)
            {
                int secs = -1;
                if (fscanf(fp, "%d", &secs) == 1)
                    info->battery_seconds_left = secs;
                fclose(fp);
            }
            break;
        }
    }
    info->battery_present = found_battery ? 1 : 0;
#endif
    return 0;
}

int fossil_sys_hostinfo_get_system(fossil_sys_hostinfo_system_t *info)
{
    if (!info)
        return -1;
#ifdef _WIN32
    OSVERSIONINFO osvi;
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    if (!GetVersionEx(&osvi))
        return -1;
    snprintf(info->os_name, sizeof(info->os_name), "Windows");
    snprintf(info->os_version, sizeof(info->os_version), "%lu.%lu", osvi.dwMajorVersion, osvi.dwMinorVersion);
    snprintf(info->kernel_version, sizeof(info->kernel_version), "%lu", osvi.dwBuildNumber);

    DWORD size = sizeof(info->hostname);
    GetComputerNameA(info->hostname, &size);

    size = sizeof(info->username);
    GetUserNameA(info->username, &size);

    // Domain name
    size = sizeof(info->domain_name);
    if (!GetEnvironmentVariableA("USERDOMAIN", info->domain_name, size))
    {
        strncpy(info->domain_name, "Unknown", sizeof(info->domain_name) - 1);
        info->domain_name[sizeof(info->domain_name) - 1] = '\0';
    }

    SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);
    snprintf(info->machine_type, sizeof(info->machine_type), "%u", sysinfo.wProcessorArchitecture);
    snprintf(info->platform, sizeof(info->platform), "Win32");

#elif defined(__APPLE__)
    struct utsname sysinfo;
    if (uname(&sysinfo) != 0)
        return -1;
    strncpy(info->os_name, sysinfo.sysname, sizeof(info->os_name) - 1);
    info->os_name[sizeof(info->os_name) - 1] = '\0';
    strncpy(info->os_version, sysinfo.version, sizeof(info->os_version) - 1);
    info->os_version[sizeof(info->os_version) - 1] = '\0';
    strncpy(info->kernel_version, sysinfo.release, sizeof(info->kernel_version) - 1);
    info->kernel_version[sizeof(info->kernel_version) - 1] = '\0';

    if (gethostname(info->hostname, sizeof(info->hostname)) != 0)
        strncpy(info->hostname, "Unknown", sizeof(info->hostname) - 1);
    info->hostname[sizeof(info->hostname) - 1] = '\0';

    const char *user = getenv("USER");
    if (user)
        strncpy(info->username, user, sizeof(info->username) - 1);
    else
        strncpy(info->username, "Unknown", sizeof(info->username) - 1);
    info->username[sizeof(info->username) - 1] = '\0';

    strncpy(info->domain_name, "Unknown", sizeof(info->domain_name) - 1);
    info->domain_name[sizeof(info->domain_name) - 1] = '\0';

    strncpy(info->machine_type, sysinfo.machine, sizeof(info->machine_type) - 1);
    info->machine_type[sizeof(info->machine_type) - 1] = '\0';

    strncpy(info->platform, "Apple", sizeof(info->platform) - 1);
    info->platform[sizeof(info->platform) - 1] = '\0';

#else
    struct utsname sysinfo;
    if (uname(&sysinfo) != 0)
        return -1;
    strncpy(info->os_name, sysinfo.sysname, sizeof(info->os_name) - 1);
    info->os_name[sizeof(info->os_name) - 1] = '\0';
    strncpy(info->os_version, sysinfo.version, sizeof(info->os_version) - 1);
    info->os_version[sizeof(info->os_version) - 1] = '\0';
    strncpy(info->kernel_version, sysinfo.release, sizeof(info->kernel_version) - 1);
    info->kernel_version[sizeof(info->kernel_version) - 1] = '\0';

    if (gethostname(info->hostname, sizeof(info->hostname)) != 0)
        strncpy(info->hostname, "Unknown", sizeof(info->hostname) - 1);
    info->hostname[sizeof(info->hostname) - 1] = '\0';

    const char *user = getenv("USER");
    if (user)
        strncpy(info->username, user, sizeof(info->username) - 1);
    else
        strncpy(info->username, "Unknown", sizeof(info->username) - 1);
    info->username[sizeof(info->username) - 1] = '\0';

    const char *domain = getenv("DOMAINNAME");
    if (domain)
        strncpy(info->domain_name, domain, sizeof(info->domain_name) - 1);
    else
        strncpy(info->domain_name, "Unknown", sizeof(info->domain_name) - 1);
    info->domain_name[sizeof(info->domain_name) - 1] = '\0';

    strncpy(info->machine_type, sysinfo.machine, sizeof(info->machine_type) - 1);
    info->machine_type[sizeof(info->machine_type) - 1] = '\0';

    strncpy(info->platform, "Unix", sizeof(info->platform) - 1);
    info->platform[sizeof(info->platform) - 1] = '\0';

#endif
    return 0;
}

int fossil_sys_hostinfo_get_architecture(fossil_sys_hostinfo_architecture_t *info)
{
    if (!info)
        return -1;

#ifdef _WIN32
    SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);

    // Architecture
    switch (sysinfo.wProcessorArchitecture)
    {
    case PROCESSOR_ARCHITECTURE_AMD64:
        strncpy(info->architecture, "x86_64", sizeof(info->architecture) - 1);
        break;
    case PROCESSOR_ARCHITECTURE_INTEL:
        strncpy(info->architecture, "x86", sizeof(info->architecture) - 1);
        break;
    case PROCESSOR_ARCHITECTURE_ARM:
        strncpy(info->architecture, "ARM", sizeof(info->architecture) - 1);
        break;
    default:
        strncpy(info->architecture, "Unknown", sizeof(info->architecture) - 1);
        break;
    }
    info->architecture[sizeof(info->architecture) - 1] = '\0';

    // CPU info (limited on Windows)
    strncpy(info->cpu, "Unknown", sizeof(info->cpu) - 1);
    info->cpu[sizeof(info->cpu) - 1] = '\0';

    snprintf(info->cpu_cores, sizeof(info->cpu_cores), "%lu", sysinfo.dwNumberOfProcessors);
    snprintf(info->cpu_threads, sizeof(info->cpu_threads), "%lu", sysinfo.dwNumberOfProcessors);
    strncpy(info->cpu_frequency, "Unknown", sizeof(info->cpu_frequency) - 1);
    info->cpu_frequency[sizeof(info->cpu_frequency) - 1] = '\0';
    strncpy(info->cpu_architecture, info->architecture, sizeof(info->cpu_architecture) - 1);
    info->cpu_architecture[sizeof(info->cpu_architecture) - 1] = '\0';

#elif defined(__APPLE__)
    size_t size = sizeof(info->architecture);
    if (sysctlbyname("hw.machine", info->architecture, &size, NULL, 0) != 0)
        strncpy(info->architecture, "Unknown", sizeof(info->architecture) - 1);
    info->architecture[sizeof(info->architecture) - 1] = '\0';

    size = sizeof(info->cpu);
    if (sysctlbyname("machdep.cpu.brand_string", info->cpu, &size, NULL, 0) != 0)
        strncpy(info->cpu, "Unknown", sizeof(info->cpu) - 1);
    info->cpu[sizeof(info->cpu) - 1] = '\0';

    int cores = 0;
    size = sizeof(cores);
    if (sysctlbyname("hw.physicalcpu", &cores, &size, NULL, 0) == 0)
        snprintf(info->cpu_cores, sizeof(info->cpu_cores), "%d", cores);
    else
        strncpy(info->cpu_cores, "Unknown", sizeof(info->cpu_cores) - 1);
    info->cpu_cores[sizeof(info->cpu_cores) - 1] = '\0';

    int threads = 0;
    size = sizeof(threads);
    if (sysctlbyname("hw.logicalcpu", &threads, &size, NULL, 0) == 0)
        snprintf(info->cpu_threads, sizeof(info->cpu_threads), "%d", threads);
    else
        strncpy(info->cpu_threads, "Unknown", sizeof(info->cpu_threads) - 1);
    info->cpu_threads[sizeof(info->cpu_threads) - 1] = '\0';

    uint64_t freq = 0;
    size = sizeof(freq);
    if (sysctlbyname("hw.cpufrequency", &freq, &size, NULL, 0) == 0)
        snprintf(info->cpu_frequency, sizeof(info->cpu_frequency), "%llu", (unsigned long long)freq);
    else
        strncpy(info->cpu_frequency, "Unknown", sizeof(info->cpu_frequency) - 1);
    info->cpu_frequency[sizeof(info->cpu_frequency) - 1] = '\0';

    strncpy(info->cpu_architecture, info->architecture, sizeof(info->cpu_architecture) - 1);
    info->cpu_architecture[sizeof(info->cpu_architecture) - 1] = '\0';

#else
    struct utsname sysinfo;
    if (uname(&sysinfo) != 0)
        return -1;

    strncpy(info->architecture, sysinfo.machine, sizeof(info->architecture) - 1);
    info->architecture[sizeof(info->architecture) - 1] = '\0';

    FILE *fp = fopen("/proc/cpuinfo", "r");
    int found_cores = 0, found_threads = 0;
    if (fp)
    {
        char line[256];
        while (fgets(line, sizeof(line), fp))
        {
            if (strncmp(line, "model name", 10) == 0)
            {
                char *colon = strchr(line, ':');
                if (colon)
                {
                    colon++;
                    while (*colon == ' ')
                        colon++;
                    strncpy(info->cpu, colon, sizeof(info->cpu) - 1);
                    info->cpu[sizeof(info->cpu) - 1] = '\0';
                }
            }
            else if (strncmp(line, "cpu cores", 9) == 0)
            {
                char *colon = strchr(line, ':');
                if (colon)
                {
                    colon++;
                    while (*colon == ' ')
                        colon++;
                    strncpy(info->cpu_cores, colon, sizeof(info->cpu_cores) - 1);
                    info->cpu_cores[sizeof(info->cpu_cores) - 1] = '\0';
                    found_cores = 1;
                }
            }
            else if (strncmp(line, "siblings", 8) == 0)
            {
                char *colon = strchr(line, ':');
                if (colon)
                {
                    colon++;
                    while (*colon == ' ')
                        colon++;
                    strncpy(info->cpu_threads, colon, sizeof(info->cpu_threads) - 1);
                    info->cpu_threads[sizeof(info->cpu_threads) - 1] = '\0';
                    found_threads = 1;
                }
            }
            else if (strncmp(line, "cpu MHz", 7) == 0)
            {
                char *colon = strchr(line, ':');
                if (colon)
                {
                    colon++;
                    while (*colon == ' ')
                        colon++;
                    strncpy(info->cpu_frequency, colon, sizeof(info->cpu_frequency) - 1);
                    info->cpu_frequency[sizeof(info->cpu_frequency) - 1] = '\0';
                }
            }
        }
        fclose(fp);
    }
    if (info->cpu[0] == '\0')
    {
        strncpy(info->cpu, "Unknown", sizeof(info->cpu) - 1);
        info->cpu[sizeof(info->cpu) - 1] = '\0';
    }
    if (!found_cores)
    {
        strncpy(info->cpu_cores, "Unknown", sizeof(info->cpu_cores) - 1);
        info->cpu_cores[sizeof(info->cpu_cores) - 1] = '\0';
    }
    if (!found_threads)
    {
        // Try to count "processor" lines as threads
        int threads = 0;
        fp = fopen("/proc/cpuinfo", "r");
        if (fp)
        {
            char line[256];
            while (fgets(line, sizeof(line), fp))
            {
                if (strncmp(line, "processor", 9) == 0)
                    threads++;
            }
            fclose(fp);
        }
        if (threads > 0)
            snprintf(info->cpu_threads, sizeof(info->cpu_threads), "%d", threads);
        else
        {
            strncpy(info->cpu_threads, "Unknown", sizeof(info->cpu_threads) - 1);
            info->cpu_threads[sizeof(info->cpu_threads) - 1] = '\0';
        }
    }
    if (info->cpu_frequency[0] == '\0')
    {
        strncpy(info->cpu_frequency, "Unknown", sizeof(info->cpu_frequency) - 1);
        info->cpu_frequency[sizeof(info->cpu_frequency) - 1] = '\0';
    }

    strncpy(info->cpu_architecture, info->architecture, sizeof(info->cpu_architecture) - 1);
    info->cpu_architecture[sizeof(info->cpu_architecture) - 1] = '\0';
#endif

    return 0;
}

int fossil_sys_hostinfo_get_memory(fossil_sys_hostinfo_memory_t *info)
{
    if (!info)
        return -1;

#ifdef _WIN32
    MEMORYSTATUSEX statex;
    statex.dwLength = sizeof(statex);
    if (!GlobalMemoryStatusEx(&statex))
        return -1;

    info->total_memory     = statex.ullTotalPhys;
    info->free_memory      = statex.ullAvailPhys;
    info->used_memory      = statex.ullTotalPhys - statex.ullAvailPhys;
    info->available_memory = statex.ullAvailPhys;

    info->total_swap = statex.ullTotalPageFile;
    info->free_swap  = statex.ullAvailPageFile;
    info->used_swap  = statex.ullTotalPageFile - statex.ullAvailPageFile;

#elif defined(__APPLE__)
    /* Total physical memory */
    int64_t memsize = 0;
    size_t len = sizeof(memsize);
    if (sysctlbyname("hw.memsize", &memsize, &len, NULL, 0) != 0)
        return -1;
    info->total_memory = (uint64_t)memsize;
    
    /* VM statistics for used/free/available */
    mach_port_t host = mach_host_self();
    vm_statistics64_data_t vmstat;
    mach_msg_type_number_t count = HOST_VM_INFO64_COUNT;
    
    if (host_statistics64(host, HOST_VM_INFO64,
                         (host_info64_t)&vmstat, &count) != KERN_SUCCESS)
    {
        return -1;
    }
    
    vm_size_t page_size;
    host_page_size(host, &page_size);
    
    /* Convert pages → bytes */
    uint64_t free_mem     = (uint64_t)vmstat.free_count     * page_size;
    uint64_t active_mem   = (uint64_t)vmstat.active_count   * page_size;
    uint64_t inactive_mem = (uint64_t)vmstat.inactive_count * page_size;
    uint64_t wired_mem    = (uint64_t)vmstat.wire_count     * page_size;
    
    info->free_memory      = free_mem;
    info->available_memory = free_mem + inactive_mem;
    info->used_memory      = active_mem + wired_mem;
    
    /* Swap usage */
    struct xsw_usage swap;
    len = sizeof(swap);
    if (sysctlbyname("vm.swapusage", &swap, &len, NULL, 0) == 0)
    {
        info->total_swap = swap.xsu_total;
        info->used_swap  = swap.xsu_used;
        info->free_swap  = swap.xsu_avail;
    }
    else
    {
        info->total_swap = 0;
        info->used_swap  = 0;
        info->free_swap  = 0;
    }

#else
    /* Linux / POSIX */
    struct sysinfo sys_info;
    if (sysinfo(&sys_info) != 0)
        return -1;

    info->total_memory     = sys_info.totalram * sys_info.mem_unit;
    info->free_memory      = sys_info.freeram * sys_info.mem_unit;
    info->used_memory      = (sys_info.totalram - sys_info.freeram) * sys_info.mem_unit;
    info->available_memory = sys_info.freeram * sys_info.mem_unit;

    info->total_swap = sys_info.totalswap * sys_info.mem_unit;
    info->free_swap  = sys_info.freeswap * sys_info.mem_unit;
    info->used_swap  = (sys_info.totalswap - sys_info.freeswap) * sys_info.mem_unit;
#endif

    return 0;
}

int fossil_sys_hostinfo_get_endianness(fossil_sys_hostinfo_endianness_t *info)
{
    if (!info)
        return -1;
    uint16_t test = 0x0001;
    info->is_little_endian = (*(uint8_t *)&test) ? 1 : 0;
    return 0;
}

int fossil_sys_hostinfo_get_uptime(fossil_sys_hostinfo_uptime_t *info)
{
    if (!info)
        return -1;
    fossil_sys_zero(info, sizeof(*info));

#if defined(__linux__)
    FILE *fp = fopen("/proc/uptime", "r");
    if (!fp)
        return -2;

    double uptime = 0.0;
    if (fscanf(fp, "%lf", &uptime) != 1)
    {
        fclose(fp);
        return -3;
    }
    fclose(fp);

    info->uptime_seconds = (uint64_t)uptime;
    info->boot_time_epoch = (uint64_t)(time(NULL) - info->uptime_seconds);
    return 0;

#elif defined(__APPLE__) || defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__)
#include <sys/types.h>
#include <sys/sysctl.h>

    struct timeval boottime;
    size_t len = sizeof(boottime);
    int mib[2] = {CTL_KERN, KERN_BOOTTIME};

    if (sysctl(mib, 2, &boottime, &len, NULL, 0) != 0)
    {
        return -2;
    }

    time_t now = time(NULL);
    info->boot_time_epoch = (uint64_t)boottime.tv_sec;
    info->uptime_seconds = (uint64_t)(now - boottime.tv_sec);
    return 0;

#elif defined(_WIN32)
#include <windows.h>

    uint64_t ms = GetTickCount64();
    info->uptime_seconds = ms / 1000;
    info->boot_time_epoch = (uint64_t)(time(NULL) - info->uptime_seconds);
    return 0;

#else
    return -4; // unsupported platform
#endif
}

#if defined(__linux__)
#include <unistd.h>
#endif

#if defined(__x86_64__) || defined(__i386__)
#include <cpuid.h>
#endif

void fossil_sys_strcpy(char *dst, size_t dst_sz, const char *src)
{
    if (!dst || dst_sz == 0)
        return;
    if (!src)
    {
        dst[0] = '\0';
        return;
    }
#if defined(__APPLE__) || defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__)
    // Use strlcpy if available, otherwise fallback to manual copy
    size_t i;
    if (dst_sz == 0)
        return;
    for (i = 0; i + 1 < dst_sz && src[i]; ++i)
    {
        dst[i] = src[i];
    }
    dst[i] = '\0';
#else
    size_t i;
    for (i = 0; i + 1 < dst_sz && src[i]; ++i)
    {
        dst[i] = src[i];
    }
    dst[i] = '\0';
#endif
}

static int fossil_detect_container_linux(char *type, size_t type_sz)
{
#if defined(__linux__)
    FILE *fp = fopen("/proc/1/cgroup", "r");
    if (!fp)
        return 0;

    char line[256];
    while (fgets(line, sizeof(line), fp))
    {
        if (strstr(line, "docker"))
        {
            fossil_sys_strcpy(type, type_sz, "docker");
            fclose(fp);
            return 1;
        }
        if (strstr(line, "podman"))
        {
            fossil_sys_strcpy(type, type_sz, "podman");
            fclose(fp);
            return 1;
        }
        if (strstr(line, "lxc"))
        {
            fossil_sys_strcpy(type, type_sz, "lxc");
            fclose(fp);
            return 1;
        }
        if (strstr(line, "kubepods"))
        {
            fossil_sys_strcpy(type, type_sz, "kubernetes");
            fclose(fp);
            return 1;
        }
    }
    fclose(fp);

    if (access("/.dockerenv", F_OK) == 0)
    {
        fossil_sys_strcpy(type, type_sz, "docker");
        return 1;
    }
    if (access("/run/.containerenv", F_OK) == 0)
    {
        fossil_sys_strcpy(type, type_sz, "podman");
        return 1;
    }

    return 0;

#else
    /* Silence unused parameter warnings on non-Linux */
    (void)type;
    (void)type_sz;
    return 0;
#endif
}

static int fossil_detect_vm_cpuid(char *hypervisor, size_t hv_sz)
{
#if defined(__x86_64__) || defined(__i386__)
    unsigned int eax, ebx, ecx, edx;

    if (!__get_cpuid(1, &eax, &ebx, &ecx, &edx))
        return 0;

    if (!(ecx & (1U << 31)))
        return 0; // no hypervisor bit

    __get_cpuid(0x40000000, &eax, &ebx, &ecx, &edx);

    char hv[13];
    memcpy(hv + 0, &ebx, 4);
    memcpy(hv + 4, &ecx, 4);
    memcpy(hv + 8, &edx, 4);
    hv[12] = '\0';

    fossil_sys_strcpy(hypervisor, hv_sz, hv);
    return 1;
#else
    (void)hypervisor;
    (void)hv_sz;
    return 0;
#endif
}

int fossil_sys_hostinfo_get_virtualization(
    fossil_sys_hostinfo_virtualization_t *info)
{
    if (!info)
        return -1;
    fossil_sys_zero(info, sizeof(*info));

    /* --- Container detection --- */
    if (fossil_detect_container_linux(
            info->container_type,
            sizeof(info->container_type)))
    {
        info->is_container = 1;
    }

    /* --- VM detection --- */
    if (fossil_detect_vm_cpuid(
            info->hypervisor,
            sizeof(info->hypervisor)))
    {
        info->is_virtual_machine = 1;
    }

#if defined(__linux__)
    /* Fallback DMI check */
    if (!info->is_virtual_machine)
    {
        FILE *fp = fopen("/sys/class/dmi/id/product_name", "r");
        if (fp)
        {
            char buf[128];
            if (fgets(buf, sizeof(buf), fp))
            {
                if (strstr(buf, "KVM") ||
                    strstr(buf, "VMware") ||
                    strstr(buf, "VirtualBox") ||
                    strstr(buf, "Hyper-V") ||
                    strstr(buf, "QEMU") ||
                    strstr(buf, "Bochs") ||
                    strstr(buf, "Xen") ||
                    strstr(buf, "Parallels"))
                {
                    info->is_virtual_machine = 1;
                    fossil_sys_strcpy(info->hypervisor,
                                      sizeof(info->hypervisor), buf);
                }
            }
            fclose(fp);
        }
    }
#endif

    return 0;
}

int fossil_sys_hostinfo_get_network(fossil_sys_hostinfo_network_t *info)
{
    if (!info)
        return -1;
    fossil_sys_zero(info, sizeof(*info));

#if defined(_WIN32)
    // Initialize Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0)
        return -1;

    // Hostname
    char hostname[128] = {0};
    DWORD size = sizeof(hostname);
    if (GetComputerNameA(hostname, &size))
        fossil_sys_strcpy(info->hostname, sizeof(info->hostname), hostname);
    else
        fossil_sys_strcpy(info->hostname, sizeof(info->hostname), "Unknown");

    // Network interfaces
    IP_ADAPTER_ADDRESSES *adapters = NULL;
    ULONG outBufLen = 0;
    ULONG flags = GAA_FLAG_INCLUDE_PREFIX;

    // First call to get buffer size
    GetAdaptersAddresses(AF_UNSPEC, flags, NULL, adapters, &outBufLen);
    adapters = (IP_ADAPTER_ADDRESSES *)malloc(outBufLen);
    if (adapters && GetAdaptersAddresses(AF_UNSPEC, flags, NULL, adapters, &outBufLen) == NO_ERROR)
    {
        IP_ADAPTER_ADDRESSES *adapter = adapters;
        while (adapter)
        {
            // Skip loopback and down interfaces
            if (adapter->OperStatus == IfOperStatusUp && adapter->IfType != IF_TYPE_SOFTWARE_LOOPBACK)
            {
                // Primary interface name
                fossil_sys_strcpy(info->interface_name, sizeof(info->interface_name), adapter->AdapterName);

                // MAC address
                if (adapter->PhysicalAddressLength > 0)
                {
                    char mac[32];
                    snprintf(mac, sizeof(mac), "%02X:%02X:%02X:%02X:%02X:%02X",
                        adapter->PhysicalAddress[0], adapter->PhysicalAddress[1],
                        adapter->PhysicalAddress[2], adapter->PhysicalAddress[3],
                        adapter->PhysicalAddress[4], adapter->PhysicalAddress[5]);
                    fossil_sys_strcpy(info->mac_address, sizeof(info->mac_address), mac);
                }

                // IPv4 address (first available)
                IP_ADAPTER_UNICAST_ADDRESS *ua = adapter->FirstUnicastAddress;
                while (ua)
                {
                    if (ua->Address.lpSockaddr->sa_family == AF_INET)
                    {
                        struct sockaddr_in *sa_in = (struct sockaddr_in *)ua->Address.lpSockaddr;
                        const char *ip = inet_ntoa(sa_in->sin_addr);
                        if (ip)
                            fossil_sys_strcpy(info->primary_ip, sizeof(info->primary_ip), ip);
                        break;
                    }
                    ua = ua->Next;
                }

                break; // Found first active interface
            }
            adapter = adapter->Next;
        }
    }

    if (adapters) free(adapters);
    WSACleanup();
    info->is_up = 1;

#elif defined(__APPLE__)
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <string.h>
#include <net/if_dl.h>

    // Hostname
    if (gethostname(info->hostname, sizeof(info->hostname)) != 0)
        fossil_sys_strcpy(info->hostname, sizeof(info->hostname), "Unknown");

    // Primary IP: first non-loopback IPv4
    struct ifaddrs *ifaddr, *ifa;
    if (getifaddrs(&ifaddr) == 0)
    {
        for (ifa = ifaddr; ifa; ifa = ifa->ifa_next)
        {
            if (ifa->ifa_addr && ifa->ifa_addr->sa_family == AF_INET)
            {
                struct sockaddr_in *sa = (struct sockaddr_in *)ifa->ifa_addr;
                const char *ip = inet_ntoa(sa->sin_addr);
                if (ip)
                    fossil_sys_strcpy(info->primary_ip, sizeof(info->primary_ip), ip);

                if (ifa->ifa_name)
                    fossil_sys_strcpy(info->interface_name, sizeof(info->interface_name), ifa->ifa_name);

                break;
            }
        }
        freeifaddrs(ifaddr);
    }

    // Try to get MAC address using getifaddrs and SIOCGIFADDR
    // On macOS, SIOCGIFADDR returns the MAC address for AF_LINK
    struct ifaddrs *ifaddr2, *ifa2;
    if (getifaddrs(&ifaddr2) == 0)
    {
        for (ifa2 = ifaddr2; ifa2; ifa2 = ifa2->ifa_next)
        {
            if (ifa2->ifa_addr && ifa2->ifa_addr->sa_family == AF_LINK &&
                ifa2->ifa_name && strcmp(ifa2->ifa_name, info->interface_name) == 0)
            {
                struct sockaddr_dl *sdl = (struct sockaddr_dl *)ifa2->ifa_addr;
                unsigned char *mac = (unsigned char *)LLADDR(sdl);
                if (sdl->sdl_alen == 6)
                {
                    char mac_str[18];
                    snprintf(mac_str, sizeof(mac_str), "%02X:%02X:%02X:%02X:%02X:%02X",
                             mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
                    fossil_sys_strcpy(info->mac_address, sizeof(info->mac_address), mac_str);
                    break;
                }
            }
        }
        if (info->mac_address[0] == '\0')
            fossil_sys_strcpy(info->mac_address, sizeof(info->mac_address), "Unknown");
        freeifaddrs(ifaddr2);
    }
    else
    {
        fossil_sys_strcpy(info->mac_address, sizeof(info->mac_address), "Unknown");
    }
    info->is_up = 1;

#elif defined(__linux__)
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <string.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <unistd.h>

    // Hostname
    if (gethostname(info->hostname, sizeof(info->hostname)) != 0)
        fossil_sys_strcpy(info->hostname, sizeof(info->hostname), "Unknown");

    // Primary IP: first non-loopback IPv4
    struct ifaddrs *ifaddr, *ifa;
    if (getifaddrs(&ifaddr) == 0)
    {
        for (ifa = ifaddr; ifa; ifa = ifa->ifa_next)
        {
            if (ifa->ifa_addr && ifa->ifa_addr->sa_family == AF_INET &&
                !(ifa->ifa_flags & IFF_LOOPBACK))
            {
                struct sockaddr_in *sa = (struct sockaddr_in *)ifa->ifa_addr;
                const char *ip = inet_ntoa(sa->sin_addr);
                if (ip)
                    fossil_sys_strcpy(info->primary_ip, sizeof(info->primary_ip), ip);

                if (ifa->ifa_name)
                    fossil_sys_strcpy(info->interface_name, sizeof(info->interface_name), ifa->ifa_name);

                // Try to get MAC address
                int fd = socket(AF_INET, SOCK_DGRAM, 0);
                if (fd >= 0)
                {
                    struct ifreq ifr;
                    memset(&ifr, 0, sizeof(ifr));
                    strncpy(ifr.ifr_name, ifa->ifa_name, IFNAMSIZ - 1);
                    if (ioctl(fd, SIOCGIFHWADDR, &ifr) == 0)
                    {
                        unsigned char *mac = (unsigned char *)ifr.ifr_hwaddr.sa_data;
                        char mac_str[18];
                        snprintf(mac_str, sizeof(mac_str), "%02X:%02X:%02X:%02X:%02X:%02X",
                                 mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
                        fossil_sys_strcpy(info->mac_address, sizeof(info->mac_address), mac_str);
                    }
                    else
                    {
                        fossil_sys_strcpy(info->mac_address, sizeof(info->mac_address), "Unknown");
                    }
                    close(fd);
                }
                else
                {
                    fossil_sys_strcpy(info->mac_address, sizeof(info->mac_address), "Unknown");
                }

                break;
            }
        }
        freeifaddrs(ifaddr);
    }
    else
    {
        fossil_sys_strcpy(info->primary_ip, sizeof(info->primary_ip), "Unknown");
        fossil_sys_strcpy(info->interface_name, sizeof(info->interface_name), "Unknown");
        fossil_sys_strcpy(info->mac_address, sizeof(info->mac_address), "Unknown");
    }
    info->is_up = 1;

#else
    fossil_sys_strcpy(info->hostname, sizeof(info->hostname), "Unknown");
    fossil_sys_strcpy(info->primary_ip, sizeof(info->primary_ip), "Unknown");
    fossil_sys_strcpy(info->mac_address, sizeof(info->mac_address), "Unknown");
    fossil_sys_strcpy(info->interface_name, sizeof(info->interface_name), "Unknown");
    info->is_up = 0;
#endif

    return 0;
}

int fossil_sys_hostinfo_get_process(fossil_sys_hostinfo_process_t *info)
{
    if (!info)
        return -1;
    fossil_sys_zero(info, sizeof(*info));

#if defined(_WIN32)
    info->pid = GetCurrentProcessId();
    info->ppid = 0;

    GetModuleFileNameA(NULL, info->executable_path, sizeof(info->executable_path));

    GetCurrentDirectoryA(sizeof(info->current_working_dir), info->current_working_dir);

    fossil_sys_strcpy(info->process_name, sizeof(info->process_name), "process");

    info->is_elevated = 0;

#elif defined(__unix__) || defined(__APPLE__)
    info->pid = getpid();
    info->ppid = getppid();

    if (getcwd(info->current_working_dir, sizeof(info->current_working_dir)) == NULL)
        fossil_sys_strcpy(info->current_working_dir, sizeof(info->current_working_dir), "Unknown");

#if defined(__linux__)
    ssize_t len = readlink("/proc/self/exe", info->executable_path, sizeof(info->executable_path) - 1);
    if (len != -1)
    {
        info->executable_path[len] = '\0';
    }
    else
    {
        fossil_sys_strcpy(info->executable_path, sizeof(info->executable_path), "Unknown");
    }
#else
    fossil_sys_strcpy(info->executable_path, sizeof(info->executable_path), "Unknown");
#endif

    fossil_sys_strcpy(info->process_name, sizeof(info->process_name), "process");

    info->is_elevated = (geteuid() == 0) ? 1 : 0;

#else
    return -2;
#endif

    return 0;
}

int fossil_sys_hostinfo_get_limits(fossil_sys_hostinfo_limits_t *info)
{
    if (!info)
        return -1;
    fossil_sys_zero(info, sizeof(*info));

#if defined(__unix__) || defined(__APPLE__)
#include <sys/resource.h>

    struct rlimit rl;

    if (getrlimit(RLIMIT_NOFILE, &rl) == 0)
        info->max_open_files = rl.rlim_cur;

    info->page_size = (uint64_t)sysconf(_SC_PAGESIZE);

#elif defined(_WIN32)
    info->max_open_files = 0;
    info->page_size = 4096;
#endif

    info->max_processes = 0; // Not portable

    return 0;
}

int fossil_sys_hostinfo_get_time(fossil_sys_hostinfo_time_t *info)
{
    if (!info)
        return -1;
    fossil_sys_zero(info, sizeof(*info));

    time_t now = time(NULL);
    struct tm local_tm;
    struct tm utc_tm;

#if defined(_WIN32)
    localtime_s(&local_tm, &now);
    gmtime_s(&utc_tm, &now);
#else
    localtime_r(&now, &local_tm);
    gmtime_r(&now, &utc_tm);
#endif

    info->utc_offset_seconds =
        (local_tm.tm_hour - utc_tm.tm_hour) * 3600;

    strftime(info->timezone, sizeof(info->timezone), "%Z", &local_tm);

    const char *loc = setlocale(LC_ALL, NULL);
    if (loc)
        fossil_sys_strcpy(info->locale, sizeof(info->locale), loc);
    else
        fossil_sys_strcpy(info->locale, sizeof(info->locale), "Unknown");

    return 0;
}

int fossil_sys_hostinfo_get_hardware(fossil_sys_hostinfo_hardware_t *info)
{
    if (!info)
        return -1;
    fossil_sys_zero(info, sizeof(*info));

#if defined(__linux__)
    FILE *fp;

    fp = fopen("/sys/class/dmi/id/sys_vendor", "r");
    if (fp)
    {
        if (fgets(info->manufacturer, sizeof(info->manufacturer), fp) == NULL)
        {
            info->manufacturer[0] = '\0';
        }
        fclose(fp);
    }

    fp = fopen("/sys/class/dmi/id/product_name", "r");
    if (fp)
    {
        if (fgets(info->product_name, sizeof(info->product_name), fp) == NULL)
        {
            info->product_name[0] = '\0';
        }
        fclose(fp);
    }

    fp = fopen("/sys/class/dmi/id/product_serial", "r");
    if (fp)
    {
        if (fgets(info->serial_number, sizeof(info->serial_number), fp) == NULL)
        {
            info->serial_number[0] = '\0';
        }
        fclose(fp);
    }

    fp = fopen("/sys/class/dmi/id/bios_version", "r");
    if (fp)
    {
        if (fgets(info->bios_version, sizeof(info->bios_version), fp) == NULL)
        {
            info->bios_version[0] = '\0';
        }
        fclose(fp);
    }
#else
    fossil_sys_strcpy(info->manufacturer, sizeof(info->manufacturer), "Unknown");
    fossil_sys_strcpy(info->product_name, sizeof(info->product_name), "Unknown");
    fossil_sys_strcpy(info->serial_number, sizeof(info->serial_number), "Unknown");
    fossil_sys_strcpy(info->bios_version, sizeof(info->bios_version), "Unknown");
#endif

    return 0;
}

int fossil_sys_hostinfo_get_display(fossil_sys_hostinfo_display_t *info)
{
    if (!info)
        return -1;

    fossil_sys_zero(info, sizeof(*info));

#if defined(_WIN32)
    // Windows
    info->display_count = GetSystemMetrics(SM_CMONITORS);
    info->primary_width = GetSystemMetrics(SM_CXSCREEN);
    info->primary_height = GetSystemMetrics(SM_CYSCREEN);

    // Attempt to get refresh rate of primary monitor
    DEVMODE devmode;
    memset(&devmode, 0, sizeof(devmode));
    devmode.dmSize = sizeof(devmode);
    if (EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &devmode))
        info->primary_refresh_rate = devmode.dmDisplayFrequency;
    else
        info->primary_refresh_rate = 0;

#elif defined(__APPLE__)
    uint32_t display_count = 0;
    CGGetActiveDisplayList(0, NULL, &display_count);
    if (display_count == 0)
        return -1;

    info->display_count = display_count;

    // Get primary display
    CGDirectDisplayID mainDisplay = CGMainDisplayID();
    info->primary_width  = (int)CGDisplayPixelsWide(mainDisplay);
    info->primary_height = (int)CGDisplayPixelsHigh(mainDisplay);

    // Refresh rate in Hz
    CGDisplayModeRef mode = CGDisplayCopyDisplayMode(mainDisplay);
    if (mode)
    {
        info->primary_refresh_rate = (int)CGDisplayModeGetRefreshRate(mode);
        CGDisplayModeRelease(mode);
    }
    else
    {
        info->primary_refresh_rate = 0;
    }

#elif defined(__linux__)
    // Linux: Use framebuffer only, fully native, no libdrm/X11
    int fb = open("/dev/fb0", O_RDONLY);
    if (fb >= 0) {
        struct fb_var_screeninfo vinfo;
        if (ioctl(fb, FBIOGET_VSCREENINFO, &vinfo) == 0) {
            info->display_count = 1;
            info->primary_width = vinfo.xres;
            info->primary_height = vinfo.yres;
            info->primary_refresh_rate = 0; // cannot detect refresh without DRM/Xrandr
        }
        close(fb);
    }

#else
    return -2; // unsupported platform
#endif

    return 0;
}
