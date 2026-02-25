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
/* ======================================================
 * Fossil OS implementation
 * Feature macros MUST come before any include
 * ====================================================== */

#if defined(__APPLE__)
#ifndef _DARWIN_C_SOURCE
#define _DARWIN_C_SOURCE 1
#endif
#endif

#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200112L
#endif

#include "fossil/sys/os.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#if defined(_WIN32) || defined(_WIN64)

#include <windows.h>
#include <tlhelp32.h>
#include <psapi.h>

#else

#include <sys/types.h>     /* must come early on macOS */
#include <sys/socket.h>    /* defines many BSD types */
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <dirent.h>
#include <errno.h>
#include <sched.h>

#if defined(__APPLE__)

#include <sys/sysctl.h>
#include <libproc.h>

#endif
#endif


/* ------------------------------------------------------
 * internal helper: duplicate string safely
 * ----------------------------------------------------- */
static char* fossil_strdup(const char* s) {
    if (!s) return NULL;
    size_t n = strlen(s) + 1;
    char* d = (char*)malloc(n);
    if (d) memcpy(d, s, n);
    return d;
}

/* ------------------------------------------------------
 * Spawn a new process
 * ----------------------------------------------------- */
bool fossil_sys_os_spawn(const char* command, fossil_sys_os_process_t* out_process) {
    if (!command || !out_process) return false;

#if defined(_WIN32) || defined(_WIN64)

    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    memset(&si, 0, sizeof(si));
    si.cb = sizeof(si);

    char* cmd_copy = fossil_strdup(command);
    if (!cmd_copy) return false;

    if (!CreateProcessA(NULL, cmd_copy, NULL, NULL, FALSE, 0,
                        NULL, NULL, &si, &pi)) {
        free(cmd_copy);
        return false;
    }

    out_process->pid = pi.dwProcessId;
    out_process->name = fossil_strdup(command);
    out_process->status = 1;

    PROCESS_MEMORY_COUNTERS pmc;
    if (GetProcessMemoryInfo(pi.hProcess, &pmc, sizeof(pmc)))
        out_process->memory_usage = pmc.WorkingSetSize;
    else
        out_process->memory_usage = 0;

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    free(cmd_copy);

#else

    pid_t pid = fork();
    if (pid < 0) return false;

    if (pid == 0) {
        execl("/bin/sh", "sh", "-c", command, (char*)NULL);
        _exit(127);
    }

    out_process->pid = (uint32_t)pid;
    out_process->name = fossil_strdup(command);
    out_process->memory_usage = 0;
    out_process->status = 1;

#endif

    out_process->id = out_process->name;
    return true;
}


/* ------------------------------------------------------
 * Terminate a process
 * ----------------------------------------------------- */
bool fossil_sys_os_kill(uint32_t pid) {

#if defined(_WIN32) || defined(_WIN64)

    HANDLE h = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
    if (!h) return false;
    BOOL ok = TerminateProcess(h, 1);
    CloseHandle(h);
    return ok ? true : false;

#else

    return (kill((pid_t)pid, SIGKILL) == 0);

#endif
}


/* ------------------------------------------------------
 * Query memory usage (platform-specific helper)
 * ----------------------------------------------------- */
static size_t fossil_query_memory(uint32_t pid) {

#if defined(_WIN32) || defined(_WIN64)

    HANDLE h = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
                           FALSE, pid);
    if (!h) return 0;

    PROCESS_MEMORY_COUNTERS pmc;
    size_t mem = 0;
    if (GetProcessMemoryInfo(h, &pmc, sizeof(pmc)))
        mem = pmc.WorkingSetSize;

    CloseHandle(h);
    return mem;

#elif defined(__linux__)

    char path[64];
    snprintf(path, sizeof(path), "/proc/%u/statm", pid);

    FILE* f = fopen(path, "r");
    if (!f) return 0;

    long pages = 0;

    /* Ensure fscanf actually read a value */
    if (fscanf(f, "%ld", &pages) != 1 || pages <= 0) {
        fclose(f);
        return 0;
    }

    fclose(f);

    long page_size = sysconf(_SC_PAGESIZE);
    if (page_size <= 0) return 0;

    return (size_t)pages * (size_t)page_size;

#elif defined(__APPLE__)

    struct proc_taskinfo info;
    if (proc_pidinfo(pid, PROC_PIDTASKINFO, 0,
                     &info, sizeof(info)) == sizeof(info))
        return (size_t)info.pti_resident_size;

    return 0;

#else
    return 0;
#endif
}

/* ------------------------------------------------------
 * List running processes
 * ----------------------------------------------------- */
size_t fossil_sys_os_list(fossil_sys_os_process_t* buffer, size_t max_count) {
    if (!buffer || max_count == 0) return 0;

    size_t count = 0;

#if defined(_WIN32) || defined(_WIN64)

    PROCESSENTRY32 pe;
    pe.dwSize = sizeof(pe);

    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snap == INVALID_HANDLE_VALUE) return 0;

    if (Process32First(snap, &pe)) {
        do {
            if (count >= max_count) break;

            buffer[count].pid = pe.th32ProcessID;
            buffer[count].name = fossil_strdup(pe.szExeFile);
            buffer[count].memory_usage =
                fossil_query_memory(pe.th32ProcessID);
            buffer[count].status = 1;
            buffer[count].id = buffer[count].name;

            count++;

        } while (Process32Next(snap, &pe));
    }

    CloseHandle(snap);

#elif defined(__linux__)

    DIR* dir = opendir("/proc");
    if (!dir) return 0;

    struct dirent* e;
    while ((e = readdir(dir)) && count < max_count) {

        char* end;
        long pid = strtol(e->d_name, &end, 10);
        if (*end != '\0') continue;
        if (pid <= 0) continue;

        buffer[count].pid = (uint32_t)pid;
        buffer[count].name = fossil_strdup(e->d_name);
        buffer[count].memory_usage = fossil_query_memory(pid);
        buffer[count].status = 1;
        buffer[count].id = buffer[count].name;
        count++;
    }

    closedir(dir);

#elif defined(__APPLE__)

    int mib[4] = { CTL_KERN, KERN_PROC, KERN_PROC_ALL, 0 };
    size_t size = 0;

    if (sysctl(mib, 4, NULL, &size, NULL, 0) < 0)
        return 0;

    struct kinfo_proc* procs = malloc(size);
    if (!procs) return 0;

    if (sysctl(mib, 4, procs, &size, NULL, 0) == 0) {
        size_t n = size / sizeof(struct kinfo_proc);

        for (size_t i = 0; i < n && count < max_count; ++i) {
            buffer[count].pid = procs[i].kp_proc.p_pid;
            buffer[count].name =
                fossil_strdup(procs[i].kp_proc.p_comm);
            buffer[count].memory_usage =
                fossil_query_memory(buffer[count].pid);
            buffer[count].status = 1;
            buffer[count].id = buffer[count].name;
            count++;
        }
    }

    free(procs);

#endif

    return count;
}


/* ------------------------------------------------------
 * System uptime (robust fallback chain)
 * ----------------------------------------------------- */
uint64_t fossil_sys_os_uptime(void) {

#if defined(_WIN32) || defined(_WIN64)

    return GetTickCount64() / 1000ULL;

#else

#if defined(CLOCK_BOOTTIME)
    struct timespec ts;
    if (clock_gettime(CLOCK_BOOTTIME, &ts) == 0)
        return (uint64_t)ts.tv_sec;
#endif

    /* fallback */
    return (uint64_t)time(NULL);

#endif
}


/* ------------------------------------------------------
 * Yield current thread
 * ----------------------------------------------------- */
void fossil_sys_os_yield(void) {

#if defined(_WIN32) || defined(_WIN64)
    Sleep(0);
#else
    sched_yield();
#endif

}
