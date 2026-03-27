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
#include "fossil/sys/syscall.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#if defined(_WIN32)
#include <windows.h>
#include <direct.h>
#include <io.h>
#define PATH_SEP '\\'
#else
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <limits.h>
#include <sys/wait.h>
#include <fcntl.h>
#define PATH_SEP '/'
#endif

static char *custom_strdup(const char *s)
{
    if (!s)
        return NULL;
    size_t len = strlen(s) + 1;
    char *copy = (char *)malloc(len);
    if (copy)
    {
        memcpy(copy, s, len);
    }
    return copy;
}

/*
 * Function to execute a system command.
 * This function takes a command string as input and executes it using the
 * system's command processor. The return value is the exit status of the
 * command.
 */
int fossil_sys_call_execute(const char *command)
{
#ifdef _WIN32
    return system(command); // On Windows, use the system function to execute the command.
#else
    return system(command); // On Unix-like systems, use the system function to execute the command.
#endif
}

/*
 * Function to get the current process ID.
 * This function returns the process ID of the calling process.
 * The process ID is a unique identifier for the process.
 */
int fossil_sys_call_getpid(void)
{
#ifdef _WIN32
    return GetCurrentProcessId(); // On Windows, use the GetCurrentProcessId function to get the process ID.
#else
    return getpid(); // On Unix-like systems, use the getpid function to get the process ID.
#endif
}

/*
 * Function to sleep for a specified number of milliseconds.
 * This function suspends the execution of the calling thread for the specified
 * number of milliseconds.
 */
void fossil_sys_call_sleep(int milliseconds)
{
#ifdef _WIN32
    Sleep(milliseconds); // On Windows, use the Sleep function to sleep for the specified number of milliseconds.
#else
    sleep(milliseconds * 1000); // On Unix-like systems, use the usleep function to sleep for the specified number of microseconds.
#endif
}

/*
 * Function to create a new file.
 * This function creates a new file with the specified filename.
 * The return value is 0 on success, or a negative error code on failure.
 */
int fossil_sys_call_create_file(const char *filename)
{
#ifdef _WIN32
    HANDLE hFile = CreateFileA(filename, GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE)
        return -1;      // If the file handle is invalid, return an error code.
    CloseHandle(hFile); // Close the file handle.
    return 0;           // Return success.
#else
    int fd = open(filename, O_CREAT | O_WRONLY, 0644); // On Unix-like systems, use the open function to create the file.
    if (fd == -1)
        return -1; // If the file descriptor is invalid, return an error code.
    close(fd);     // Close the file descriptor.
    return 0;      // Return success.
#endif
}

// ----------------------------------------------------
// Delete a file
// ----------------------------------------------------
int fossil_sys_call_delete_file(const char *filename)
{
    if (!filename)
        return -1;
    return remove(filename) == 0 ? 0 : -errno;
}

// ----------------------------------------------------
// Check if a file exists
// ----------------------------------------------------
int fossil_sys_call_file_exists(const char *filename)
{
    if (!filename)
        return 0;
#if defined(_WIN32)
    return _access(filename, 0) == 0;
#else
    return access(filename, F_OK) == 0;
#endif
}

// ----------------------------------------------------
// Create a directory
// ----------------------------------------------------
int fossil_sys_call_create_directory(const char *dirname)
{
    if (!dirname)
        return -1;
#if defined(_WIN32)
    return _mkdir(dirname) == 0 ? 0 : -errno;
#else
    return mkdir(dirname, 0755) == 0 ? 0 : -errno;
#endif
}

// ----------------------------------------------------
// Helper: delete directory recursively (POSIX & Windows)
// ----------------------------------------------------

#define FOSSIL_MAX_PATH (MAX_PATH * 2) // Allow longer paths safely

static int fossil_sys_call_delete_directory_recursive(const char *dirname)
{
#if defined(_WIN32)
    WIN32_FIND_DATA find_data;
    char search_path[FOSSIL_MAX_PATH];
    int ret = snprintf(search_path, sizeof(search_path), "%s\\*", dirname);
    if (ret < 0 || ret >= (int)sizeof(search_path))
    {
        return -ENAMETOOLONG;
    }

    HANDLE hFind = FindFirstFile(search_path, &find_data);
    if (hFind == INVALID_HANDLE_VALUE)
    {
        return -errno;
    }

    do
    {
        if (strcmp(find_data.cFileName, ".") == 0 ||
            strcmp(find_data.cFileName, "..") == 0)
            continue;

        char full_path[FOSSIL_MAX_PATH];
        ret = snprintf(full_path, sizeof(full_path), "%s\\%s", dirname, find_data.cFileName);
        if (ret < 0 || ret >= (int)sizeof(full_path))
        {
            continue; // skip too-long paths safely
        }

        if (find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            fossil_sys_call_delete_directory_recursive(full_path);
        }
        else
        {
            DeleteFile(full_path);
        }
    } while (FindNextFile(hFind, &find_data));

    FindClose(hFind);
    return RemoveDirectory(dirname) ? 0 : -errno;
#else
    // POSIX side unchanged
    DIR *dir = opendir(dirname);
    if (!dir)
        return -errno;

    struct dirent *entry;
    char path[PATH_MAX];

    while ((entry = readdir(dir)))
    {
        if (strcmp(entry->d_name, ".") == 0 ||
            strcmp(entry->d_name, "..") == 0)
            continue;

        snprintf(path, sizeof(path), "%s/%s", dirname, entry->d_name);

        struct stat st;
        if (stat(path, &st) == 0)
        {
            if (S_ISDIR(st.st_mode))
            {
                fossil_sys_call_delete_directory_recursive(path);
            }
            else
            {
                unlink(path);
            }
        }
    }
    closedir(dir);
    return rmdir(dirname) == 0 ? 0 : -errno;
#endif
}

// ----------------------------------------------------
// Delete a directory (optionally recursive)
// ----------------------------------------------------
int fossil_sys_call_delete_directory(const char *dirname, int recursive)
{
    if (!dirname)
        return -1;
    if (recursive)
        return fossil_sys_call_delete_directory_recursive(dirname);
#if defined(_WIN32)
    return RemoveDirectory(dirname) ? 0 : -errno;
#else
    return rmdir(dirname) == 0 ? 0 : -errno;
#endif
}

// ----------------------------------------------------
// Get current working directory
// ----------------------------------------------------
int fossil_sys_call_getcwd(char *buffer, size_t size)
{
    if (!buffer || size == 0)
        return -1;
#if defined(_WIN32)
    return _getcwd(buffer, (int)size) ? 0 : -errno;
#else
    return getcwd(buffer, size) ? 0 : -errno;
#endif
}

// ----------------------------------------------------
// Change working directory
// ----------------------------------------------------
int fossil_sys_call_chdir(const char *path)
{
    if (!path)
        return -1;
#if defined(_WIN32)
    return _chdir(path) == 0 ? 0 : -errno;
#else
    return chdir(path) == 0 ? 0 : -errno;
#endif
}

// ----------------------------------------------------
// List files in directory
// ----------------------------------------------------
int fossil_sys_call_list_directory(const char *dirname, char ***out_list, size_t *out_count)
{
    if (!dirname || !out_list || !out_count)
        return -1;

    *out_list = NULL;
    *out_count = 0;

#if defined(_WIN32)
    WIN32_FIND_DATA find_data;
    char search_path[MAX_PATH];
    snprintf(search_path, sizeof(search_path), "%s\\*", dirname);

    HANDLE hFind = FindFirstFile(search_path, &find_data);
    if (hFind == INVALID_HANDLE_VALUE)
        return -errno;

    size_t capacity = 10;
    *out_list = malloc(capacity * sizeof(char *));
    if (!*out_list)
    {
        FindClose(hFind);
        return -ENOMEM;
    }

    do
    {
        if (strcmp(find_data.cFileName, ".") != 0 &&
            strcmp(find_data.cFileName, "..") != 0)
        {
            if (*out_count >= capacity)
            {
                capacity *= 2;
                char **tmp = realloc(*out_list, capacity * sizeof(char *));
                if (!tmp)
                {
                    FindClose(hFind);
                    return -ENOMEM;
                }
                *out_list = tmp;
            }
            (*out_list)[*out_count] = custom_strdup(find_data.cFileName);
            (*out_count)++;
        }
    } while (FindNextFile(hFind, &find_data));

    FindClose(hFind);
#else
    DIR *dir = opendir(dirname);
    if (!dir)
        return -errno;

    size_t capacity = 10;
    *out_list = malloc(capacity * sizeof(char *));
    if (!*out_list)
    {
        closedir(dir);
        return -ENOMEM;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)))
    {
        if (strcmp(entry->d_name, ".") != 0 &&
            strcmp(entry->d_name, "..") != 0)
        {
            if (*out_count >= capacity)
            {
                capacity *= 2;
                char **tmp = realloc(*out_list, capacity * sizeof(char *));
                if (!tmp)
                {
                    closedir(dir);
                    return -ENOMEM;
                }
                *out_list = tmp;
            }
            (*out_list)[*out_count] = custom_strdup(entry->d_name);
            (*out_count)++;
        }
    }
    closedir(dir);
#endif
    return 0;
}

// ----------------------------------------------------
// Path type checks
// ----------------------------------------------------
int fossil_sys_call_is_directory(const char *path)
{
    if (!path)
        return 0;
#if defined(_WIN32)
    DWORD attrs = GetFileAttributesA(path);
    return (attrs != INVALID_FILE_ATTRIBUTES && (attrs & FILE_ATTRIBUTE_DIRECTORY));
#else
    struct stat st;
    return (stat(path, &st) == 0 && S_ISDIR(st.st_mode));
#endif
}

int fossil_sys_call_is_file(const char *path)
{
    if (!path)
        return 0;
#if defined(_WIN32)
    DWORD attrs = GetFileAttributesA(path);
    return (attrs != INVALID_FILE_ATTRIBUTES && !(attrs & FILE_ATTRIBUTE_DIRECTORY));
#else
    struct stat st;
    return (stat(path, &st) == 0 && S_ISREG(st.st_mode));
#endif
}

// ----------------------------------------------------
// Process termination
// ----------------------------------------------------
void fossil_sys_call_exit(int exit_code)
{
    exit(exit_code);
}

// ----------------------------------------------------
// Execute a command and capture output
// ----------------------------------------------------
int fossil_sys_call_execute_capture(const char *command, char *buffer, size_t size)
{
    if (!command || !buffer || size == 0)
        return -1;
    buffer[0] = '\0';

#if defined(_WIN32)
    FILE *pipe = _popen(command, "r");
#else
    FILE *pipe = popen(command, "r");
#endif
    if (!pipe)
        return -errno;

    size_t read_bytes = fread(buffer, 1, size - 1, pipe);
    buffer[read_bytes] = '\0';

#if defined(_WIN32)
    _pclose(pipe);
#else
    pclose(pipe);
#endif
    return 0;
}
