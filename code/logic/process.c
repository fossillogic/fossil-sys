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
#include "fossil/sys/process.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#if defined(__linux__) || defined(__APPLE__)
#include <unistd.h>
#include <dirent.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <time.h>

static void fossil_sys_zero(void *ptr, size_t size)
{
    if (ptr)
        memset(ptr, 0, size);
}

uint32_t fossil_sys_process_get_pid(void)
{
    return (uint32_t)getpid();
}

int fossil_sys_process_get_name(uint32_t pid, char *name, size_t name_len)
{
    if (!name || name_len == 0)
        return -1;

    char path[256];
    snprintf(path, sizeof(path), "/proc/%u/comm", pid);
    FILE *fp = fopen(path, "r");
    if (!fp)
        return -2;

    if (!fgets(name, name_len, fp))
    {
        fclose(fp);
        return -3;
    }
    fclose(fp);

    size_t len = strlen(name);
    if (len > 0 && name[len - 1] == '\n')
        name[len - 1] = '\0';
    return 0;
}

int fossil_sys_process_get_info(uint32_t pid, fossil_sys_process_info_t *info)
{
    if (!info)
        return -1;
    fossil_sys_zero(info, sizeof(*info));
    info->pid = pid;

    // Name
    if (fossil_sys_process_get_name(pid, info->name, sizeof(info->name)) != 0)
        strncpy(info->name, "unknown", sizeof(info->name));

    // Parent PID
    char path[256];
    snprintf(path, sizeof(path), "/proc/%u/stat", pid);
    FILE *fp = fopen(path, "r");
    if (fp)
    {
        int scanned = fscanf(fp, "%*d %*s %*c %u", &info->ppid);
        fclose(fp);
        if (scanned != 1)
            info->ppid = 0;
    }

    // Memory usage
    snprintf(path, sizeof(path), "/proc/%u/statm", pid);
    fp = fopen(path, "r");
    if (fp)
    {
        unsigned long size = 0, resident = 0;
        if (fscanf(fp, "%lu %lu", &size, &resident) == 2)
        {
            long page_size = sysconf(_SC_PAGESIZE);
            info->virtual_memory_bytes = (uint64_t)size * page_size;
            info->memory_bytes = (uint64_t)resident * page_size;
        }
        fclose(fp);
    }

    // Thread count
    snprintf(path, sizeof(path), "/proc/%u/status", pid);
    fp = fopen(path, "r");
    if (fp)
    {
        char line[512];
        while (fgets(line, sizeof(line), fp))
        {
            if (sscanf(line, "Threads: %u", &info->thread_count) == 1)
                break;
        }
        fclose(fp);
    }

    // CPU usage placeholder
    info->cpu_percent = 0.0f;

    return 0;
}

int fossil_sys_process_list(fossil_sys_process_list_t *plist)
{
    if (!plist)
        return -1;
    plist->count = 0;

    DIR *dir = opendir("/proc");
    if (!dir)
        return -2;

    struct dirent *entry;
    while ((entry = readdir(dir)))
    {
        char *endptr;
        uint32_t pid = strtoul(entry->d_name, &endptr, 10);
        if (*endptr != '\0')
            continue;

        if (plist->count >= FOSSIL_SYS_PROCESS_MAX)
            break;
        fossil_sys_process_get_info(pid, &plist->list[plist->count++]);
    }

    closedir(dir);
    return 0;
}

int fossil_sys_process_terminate(uint32_t pid, int force)
{
    if (pid == (uint32_t)getpid())
        return -1; // Do not allow terminating self
    int sig = force ? SIGKILL : SIGTERM;
    return (kill(pid, sig) == 0) ? 0 : -1;
}

int fossil_sys_process_get_environment(uint32_t pid, char *buffer, size_t buf_len)
{
    if (!buffer || buf_len == 0)
        return -1;
    fossil_sys_zero(buffer, buf_len);

    char path[256];
    snprintf(path, sizeof(path), "/proc/%u/environ", pid);
    FILE *fp = fopen(path, "r");
    if (!fp)
        return -2;

    size_t total_read = fread(buffer, 1, buf_len - 1, fp);
    fclose(fp);

    for (size_t i = 0; i < total_read; i++)
        if (buffer[i] == '\0')
            buffer[i] = ';';

    buffer[total_read] = '\0';
    return (int)total_read;
}

int fossil_sys_process_exists(uint32_t pid)
{
    if (pid == 0)
        return 0;
    if (kill(pid, 0) == 0)
        return 1;
    if (errno == ESRCH)
        return 0;
    return -1;
}

int fossil_sys_process_suspend(uint32_t pid)
{
    if (pid == (uint32_t)getpid())
        return -1;
    return (kill(pid, SIGSTOP) == 0) ? 0 : -1;
}

int fossil_sys_process_resume(uint32_t pid)
{
    if (pid == (uint32_t)getpid())
        return -1;
    return (kill(pid, SIGCONT) == 0) ? 0 : -1;
}

int fossil_sys_process_set_priority(uint32_t pid, int priority)
{
    if (setpriority(PRIO_PROCESS, pid, priority) == 0)
        return 0;
    return -1;
}

int fossil_sys_process_get_priority(uint32_t pid, int *priority)
{
    if (!priority)
        return -1;
    errno = 0;
    int prio = getpriority(PRIO_PROCESS, pid);
    if (errno != 0)
        return -1;
    *priority = prio;
    return 0;
}

int fossil_sys_process_wait(uint32_t pid, int *exit_code, int timeout_ms)
{
    if (pid == 0)
        return -1;
    int status = 0;
    if (timeout_ms < 0)
    {
        // Infinite wait
        if (waitpid(pid, &status, 0) < 0)
            return -1;
    }
    else
    {
        // Timed wait (polling)
        int waited = 0;
        const int interval = 10; // ms
        while (waited < timeout_ms)
        {
            pid_t ret = waitpid(pid, &status, WNOHANG);
            if (ret == pid)
                break;
            if (ret < 0)
                return -1;
            struct timespec ts = {0, interval * 1000000};
            nanosleep(&ts, NULL);
            waited += interval;
        }
        if (waited >= timeout_ms)
            return -2;
    }
    if (exit_code)
    {
        if (WIFEXITED(status))
            *exit_code = WEXITSTATUS(status);
        else
            *exit_code = -1;
    }
    return 0;
}

int fossil_sys_process_spawn(const char *path, char *const argv[], char *const envp[], uint32_t *pid_out)
{
    pid_t pid = fork();
    if (pid < 0)
        return -1;
    if (pid == 0)
    {
        if (envp)
            execve(path, argv, envp);
        else
            execv(path, argv);
        _exit(127);
    }
    if (pid_out)
        *pid_out = (uint32_t)pid;
    return 0;
}

int fossil_sys_process_get_exe_path(uint32_t pid, char *buffer, size_t buf_len)
{
    if (!buffer || buf_len == 0)
        return -1;
    char path[256];
    snprintf(path, sizeof(path), "/proc/%u/exe", pid);
    ssize_t len = readlink(path, buffer, buf_len - 1);
    if (len < 0)
        return -2;
    buffer[len] = '\0';
    return 0;
}

#elif defined(_WIN32)
#include <windows.h>
#include <winternl.h>
#include <string.h>
#include <wchar.h>
#include <tlhelp32.h>
#include <psapi.h>

uint32_t fossil_sys_process_get_pid(void)
{
    return (uint32_t)GetCurrentProcessId();
}

int fossil_sys_process_get_name(uint32_t pid, char *name, size_t name_len)
{
    if (!name || name_len == 0)
        return -1;
    HANDLE h = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!h)
        return -2;
    DWORD sz = (DWORD)name_len;
    if (!QueryFullProcessImageNameA(h, 0, name, &sz))
    {
        CloseHandle(h);
        return -3;
    }
    CloseHandle(h);
    return 0;
}

int fossil_sys_process_get_info(uint32_t pid, fossil_sys_process_info_t *info)
{
    if (!info)
        return -1;
    memset(info, 0, sizeof(*info));
    info->pid = pid;
    // TODO: Fill more fields using Windows APIs if needed
    return 0;
}

int fossil_sys_process_list(fossil_sys_process_list_t *plist)
{
    if (!plist)
        return -1;
    plist->count = 0;
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snap == INVALID_HANDLE_VALUE)
        return -2;
    PROCESSENTRY32 pe;
    pe.dwSize = sizeof(pe);
    if (Process32First(snap, &pe))
    {
        do
        {
            if (plist->count >= FOSSIL_SYS_PROCESS_MAX)
                break;
            fossil_sys_process_info_t *info = &plist->list[plist->count++];
            memset(info, 0, sizeof(*info));
            info->pid = pe.th32ProcessID;
            info->ppid = pe.th32ParentProcessID;
            strncpy(info->name, pe.szExeFile, sizeof(info->name) - 1);
        } while (Process32Next(snap, &pe));
    }
    CloseHandle(snap);
    return 0;
}

int fossil_sys_process_terminate(uint32_t pid, int force)
{
    if (pid == GetCurrentProcessId())
        return -1; // Do not allow terminating self
    HANDLE h = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
    if (!h)
        return -1;
    BOOL ok = TerminateProcess(h, force ? 1 : 0);
    CloseHandle(h);
    return ok ? 0 : -1;
}

typedef NTSTATUS(NTAPI *PFN_NtQueryInformationProcess)(
    HANDLE, PROCESSINFOCLASS, PVOID, ULONG, PULONG);

static int utf16_env_to_utf8(WCHAR *env, char *buffer, size_t buf_len)
{
    if (!env || !buffer || buf_len == 0)
        return -1;
    size_t total = 0;
    while (*env)
    {
        int len = WideCharToMultiByte(
            CP_UTF8, 0, env, -1, buffer + total, (int)(buf_len - total), NULL, NULL);
        if (len == 0)
            break;
        total += len - 1;
        buffer[total++] = ';';
        env += wcslen(env) + 1;
        if (total >= buf_len - 1)
            break;
    }
    buffer[total] = '\0';
    return (int)total;
}

typedef struct _RTL_USER_PROCESS_PARAMETERS_PARTIAL
{
    BYTE Reserved1[16];
    PVOID Reserved2[10];
#if defined(_WIN64)
    UNICODE_STRING ImagePathName;
    UNICODE_STRING CommandLine;
    BYTE Reserved3[96];
    PVOID Environment;
#else
    UNICODE_STRING ImagePathName;
    UNICODE_STRING CommandLine;
    BYTE Reserved3[48];
    PVOID Environment;
#endif
} RTL_USER_PROCESS_PARAMETERS_PARTIAL;

int fossil_sys_process_get_environment(uint32_t pid, char *buffer, size_t buf_len)
{
    if (!buffer || buf_len == 0)
        return -1;
    memset(buffer, 0, buf_len);

    HMODULE ntdll = GetModuleHandleA("ntdll.dll");
    if (!ntdll)
        return -2;

    PFN_NtQueryInformationProcess NtQueryInformationProcess = NULL;
    NtQueryInformationProcess = (PFN_NtQueryInformationProcess)(uintptr_t)GetProcAddress(ntdll, "NtQueryInformationProcess");
    if (!NtQueryInformationProcess)
        return -3;

    HANDLE hProc = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
    if (!hProc)
        return -4;

    PROCESS_BASIC_INFORMATION pbi;
    NTSTATUS status = NtQueryInformationProcess(hProc, ProcessBasicInformation, &pbi, sizeof(pbi), NULL);
    if (status != 0)
    {
        CloseHandle(hProc);
        return -5;
    }

    PEB peb;
    if (!ReadProcessMemory(hProc, pbi.PebBaseAddress, &peb, sizeof(peb), NULL))
    {
        CloseHandle(hProc);
        return -6;
    }

    RTL_USER_PROCESS_PARAMETERS_PARTIAL procParams;
    if (!ReadProcessMemory(hProc, peb.ProcessParameters, &procParams, sizeof(procParams), NULL))
    {
        CloseHandle(hProc);
        return -7;
    }

    SIZE_T envBlockSize = 65536;
    WCHAR *envBlock = (WCHAR *)malloc(envBlockSize);
    if (!envBlock)
    {
        CloseHandle(hProc);
        return -8;
    }

    if (!ReadProcessMemory(hProc, procParams.Environment, envBlock, envBlockSize, NULL))
    {
        free(envBlock);
        CloseHandle(hProc);
        return -9;
    }

    int written = utf16_env_to_utf8(envBlock, buffer, buf_len);

    free(envBlock);
    CloseHandle(hProc);
    return written;
}

int fossil_sys_process_exists(uint32_t pid)
{
    HANDLE h = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (h)
    {
        CloseHandle(h);
        return 1;
    }
    if (GetLastError() == ERROR_INVALID_PARAMETER)
        return 0;
    return -1;
}

int fossil_sys_process_suspend(uint32_t pid)
{
    HANDLE h = OpenProcess(PROCESS_SUSPEND_RESUME, FALSE, pid);
    if (!h)
        return -1;
    typedef LONG(WINAPI * PFN_NtSuspendProcess)(HANDLE);
    PFN_NtSuspendProcess NtSuspendProcess = (PFN_NtSuspendProcess)GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtSuspendProcess");
    if (!NtSuspendProcess)
    {
        CloseHandle(h);
        return -2;
    }
    LONG status = NtSuspendProcess(h);
    CloseHandle(h);
    return (status == 0) ? 0 : -3;
}

int fossil_sys_process_resume(uint32_t pid)
{
    HANDLE h = OpenProcess(PROCESS_SUSPEND_RESUME, FALSE, pid);
    if (!h)
        return -1;
    typedef LONG(WINAPI * PFN_NtResumeProcess)(HANDLE);
    PFN_NtResumeProcess NtResumeProcess = (PFN_NtResumeProcess)GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtResumeProcess");
    if (!NtResumeProcess)
    {
        CloseHandle(h);
        return -2;
    }
    LONG status = NtResumeProcess(h);
    CloseHandle(h);
    return (status == 0) ? 0 : -3;
}

int fossil_sys_process_set_priority(uint32_t pid, int priority)
{
    HANDLE h = OpenProcess(PROCESS_SET_INFORMATION, FALSE, pid);
    if (!h)
        return -1;
    BOOL ok = SetPriorityClass(h, priority);
    CloseHandle(h);
    return ok ? 0 : -2;
}

int fossil_sys_process_get_priority(uint32_t pid, int *priority)
{
    if (!priority)
        return -1;
    HANDLE h = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pid);
    if (!h)
        return -2;
    DWORD prio = GetPriorityClass(h);
    CloseHandle(h);
    if (!prio)
        return -3;
    *priority = (int)prio;
    return 0;
}

int fossil_sys_process_wait(uint32_t pid, int *exit_code, int timeout_ms)
{
    HANDLE h = OpenProcess(SYNCHRONIZE | PROCESS_QUERY_INFORMATION, FALSE, pid);
    if (!h)
        return -1;
    DWORD ms = (timeout_ms < 0) ? INFINITE : (DWORD)timeout_ms;
    DWORD res = WaitForSingleObject(h, ms);
    if (res != WAIT_OBJECT_0)
    {
        CloseHandle(h);
        return -2;
    }
    if (exit_code)
    {
        DWORD code = 0;
        if (GetExitCodeProcess(h, &code))
            *exit_code = (int)code;
        else
            *exit_code = -1;
    }
    CloseHandle(h);
    return 0;
}

int fossil_sys_process_spawn(const char *path, char *const argv[], char *const envp[], uint32_t *pid_out)
{
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    memset(&si, 0, sizeof(si));
    memset(&pi, 0, sizeof(pi));
    si.cb = sizeof(si);

    // Build command line
    char cmdline[1024] = {0};
    if (argv && argv[0])
    {
        strncpy(cmdline, argv[0], sizeof(cmdline) - 1);
        for (int i = 1; argv[i]; ++i)
        {
            strncat(cmdline, " ", sizeof(cmdline) - strlen(cmdline) - 1);
            strncat(cmdline, argv[i], sizeof(cmdline) - strlen(cmdline) - 1);
        }
    }

    BOOL ok = CreateProcessA(
        path, cmdline[0] ? cmdline : NULL, NULL, NULL, FALSE, 0,
        envp, NULL, &si, &pi);
    if (!ok)
        return -1;
    if (pid_out)
        *pid_out = (uint32_t)pi.dwProcessId;
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
    return 0;
}

int fossil_sys_process_get_exe_path(uint32_t pid, char *buffer, size_t buf_len)
{
    if (!buffer || buf_len == 0)
        return -1;
    HANDLE h = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!h)
        return -2;
    DWORD sz = (DWORD)buf_len;
    BOOL ok = QueryFullProcessImageNameA(h, 0, buffer, &sz);
    CloseHandle(h);
    return ok ? 0 : -3;
}

#else
// stubs for unsupported platforms
uint32_t fossil_sys_process_get_pid(void) { return 0; }

int fossil_sys_process_get_name(uint32_t pid, char *name, size_t name_len)
{
    (void)pid;
    (void)name;
    (void)name_len;
    return -1;
}

int fossil_sys_process_get_info(uint32_t pid, fossil_sys_process_info_t *info)
{
    (void)pid;
    (void)info;
    return -1;
}

int fossil_sys_process_list(fossil_sys_process_list_t *plist)
{
    (void)plist;
    return -1;
}

int fossil_sys_process_terminate(uint32_t pid, int force)
{
    (void)pid;
    (void)force;
    return -1;
}

int fossil_sys_process_get_environment(uint32_t pid, char *buffer, size_t buf_len)
{
    (void)pid;
    (void)buffer;
    (void)buf_len;
    return -1;
}

int fossil_sys_process_exists(uint32_t pid)
{
    (void)pid;
    return -1;
}

int fossil_sys_process_suspend(uint32_t pid)
{
    (void)pid;
    return -1;
}

int fossil_sys_process_resume(uint32_t pid)
{
    (void)pid;
    return -1;
}

int fossil_sys_process_set_priority(uint32_t pid, int priority)
{
    (void)pid;
    (void)priority;
    return -1;
}

int fossil_sys_process_get_priority(uint32_t pid, int *priority)
{
    (void)pid;
    (void)priority;
    return -1;
}

int fossil_sys_process_wait(uint32_t pid, int *exit_code, int timeout_ms)
{
    (void)pid;
    (void)exit_code;
    (void)timeout_ms;
    return -1;
}

int fossil_sys_process_spawn(const char *path, char *const argv[], char *const envp[], uint32_t *pid_out)
{
    (void)path;
    (void)argv;
    (void)envp;
    (void)pid_out;
    return -1;
}

int fossil_sys_process_get_exe_path(uint32_t pid, char *buffer, size_t buf_len)
{
    (void)pid;
    (void)buffer;
    (void)buf_len;
    return -1;
}
#endif
