/*
 * -----------------------------------------------------------------------------
 * Project: Fossil Logic
 *
 * This file is part of the Fossil Logic project, which aims to develop high-
 * performance, cross-platform applications and libraries. The code contained
 * herein is subject to the terms and conditions defined in the project license.
 *
 * Author: Michael Gene Brockus (Dreamer)
 *
 * Copyright (C) 2024 Fossil Logic. All rights reserved.
 * -----------------------------------------------------------------------------
 */
#ifndef FOSSIL_SYS_CALL_H
#define FOSSIL_SYS_CALL_H

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#endif

#include <stdio.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/* 
 * Function to execute a system command.
 * This function takes a command string as input and executes it using the 
 * system's command processor. The return value is the exit status of the 
 * command.
 */
int fossil_sys_call_execute(const char *command) {
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
int fossil_sys_call_getpid() {
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
void fossil_sys_call_sleep(int milliseconds) {
#ifdef _WIN32
    Sleep(milliseconds); // On Windows, use the Sleep function to sleep for the specified number of milliseconds.
#else
    usleep(milliseconds * 1000); // On Unix-like systems, use the usleep function to sleep for the specified number of microseconds.
#endif
}

/* 
 * Function to create a new file.
 * This function creates a new file with the specified filename. 
 * The return value is 0 on success, or a negative error code on failure.
 */
int fossil_sys_call_create_file(const char *filename) {
#ifdef _WIN32
    HANDLE hFile = CreateFileA(filename, GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) return -1; // If the file handle is invalid, return an error code.
    CloseHandle(hFile); // Close the file handle.
    return 0; // Return success.
#else
    int fd = open(filename, O_CREAT | O_WRONLY, 0644); // On Unix-like systems, use the open function to create the file.
    if (fd == -1) return -1; // If the file descriptor is invalid, return an error code.
    close(fd); // Close the file descriptor.
    return 0; // Return success.
#endif
}

#ifdef __cplusplus
}
#include <string>

/**
 * Fossil namespace.
 */
namespace fossil {

    /**
     * System namespace.
     */
    namespace sys {

        /**
         * Syscall class.
         */
        class Syscall {
        public:

            /**
             * Execute a system command.
             *
             * @param command The command to execute.
             * @return The return value of the system command.
             */
            static int execute(const char *command) {
                return fossil_sys_call_execute(command);
            }

            /**
             * Execute a system command.
             *
             * @param command The command to execute
             * @return The return value of the system command.
             */
            static int execute(const std::string &command) {
                return fossil_sys_call_execute(command.c_str());
            }

            /**
             * Get the current process ID.
             *
             * @return The current process ID.
             */
            static int getpid() {
                return fossil_sys_call_getpid();
            }

            /**
             * Sleep for a specified number of milliseconds.
             *
             * @param milliseconds The number of milliseconds to sleep.
             */
            static void sleep(int milliseconds) {
                fossil_sys_call_sleep(milliseconds);
            }

            /**
             * Create a new file.
             *
             * @param filename The name of the file to create.
             * @return 0 on success, or a negative error code on failure.
             */
            static int create_file(const char *filename) {
                return fossil_sys_call_create_file(filename);
            }

            /**
             * Create a new file.
             *
             * @param filename The name of the file to create.
             * @return 0 on success, or a negative error code on failure.
             */
            static int create_file(const std::string &filename) {
                return fossil_sys_call_create_file(filename.c_str());
            }

        };

    }

}

#endif

#endif /* FOSSIL_SYS_FRAMEWORK_H */
