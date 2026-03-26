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
#include "fossil/sys/env.h"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#if defined(_WIN32)
#include <windows.h>
#endif

/* ============================================================================
   INTERNAL HELPERS
============================================================================ */

static int fossil_sys_env_stricmp(const char* a, const char* b) {
    if (!a || !b) return -1;

    while (*a && *b) {
        char ca = (char)tolower((unsigned char)*a);
        char cb = (char)tolower((unsigned char)*b);
        if (ca != cb) return (int)(unsigned char)ca - (int)(unsigned char)cb;
        a++; b++;
    }
    return *a - *b;
}

static int fossil_sys_env_parse_bool(const char* v, int default_value) {
    if (!v) return default_value;

    if (fossil_sys_env_stricmp(v, "1") == 0) return 1;
    if (fossil_sys_env_stricmp(v, "true") == 0) return 1;
    if (fossil_sys_env_stricmp(v, "yes") == 0) return 1;

    if (fossil_sys_env_stricmp(v, "0") == 0) return 0;
    if (fossil_sys_env_stricmp(v, "false") == 0) return 0;
    if (fossil_sys_env_stricmp(v, "no") == 0) return 0;

    return default_value;
}

/* ============================================================================
    CORE API
============================================================================ */

/*
 * Canonical String ID Handler
 *
 * This function maps canonical Fossil Sys string IDs to their platform-specific
 * environment variable names or retrieves their values using platform APIs.
 * If the key is not recognized, it falls back to the OS environment.
 */
static const char* fossil_sys_env_resolve_id(const char* key) {
     if (!key) return NULL;

     /* System properties */
     if (strcmp(key, "fossil.sys.name") == 0) {
#if defined(_WIN32)
          return "windows";
#elif defined(__APPLE__)
          return "darwin";
#elif defined(__linux__)
          return "linux";
#elif defined(__unix__)
          return "unix";
#else
          return "unknown";
#endif
     }
     if (strcmp(key, "fossil.sys.arch") == 0) {
#if defined(_WIN64)
          return "x86_64";
#elif defined(_WIN32)
          return "x86";
#elif defined(__x86_64__) || defined(__amd64__)
          return "x86_64";
#elif defined(__aarch64__)
          return "arm64";
#elif defined(__arm__)
          return "arm";
#elif defined(__i386__)
          return "x86";
#else
          return "unknown";
#endif
     }
     if (strcmp(key, "fossil.sys.page_size") == 0) {
          static char buf[32];
#if defined(_WIN32)
          SYSTEM_INFO si;
          GetSystemInfo(&si);
          snprintf(buf, sizeof(buf), "%lu", (unsigned long)si.dwPageSize);
#elif defined(_SC_PAGESIZE)
          snprintf(buf, sizeof(buf), "%ld", sysconf(_SC_PAGESIZE));
#else
          snprintf(buf, sizeof(buf), "4096");
#endif
          return buf;
     }
     if (strcmp(key, "fossil.sys.cpu_count") == 0) {
          static char buf[32];
#if defined(_WIN32)
          SYSTEM_INFO si;
          GetSystemInfo(&si);
          snprintf(buf, sizeof(buf), "%lu", (unsigned long)si.dwNumberOfProcessors);
#elif defined(_SC_NPROCESSORS_ONLN)
          snprintf(buf, sizeof(buf), "%ld", sysconf(_SC_NPROCESSORS_ONLN));
#else
          snprintf(buf, sizeof(buf), "1");
#endif
          return buf;
     }
     if (strcmp(key, "fossil.sys.path") == 0) {
#if defined(_WIN32)
          return fossil_sys_env_get("PATH");
#else
          return fossil_sys_env_get("PATH");
#endif
     }
     if (strcmp(key, "fossil.sys.home") == 0) {
#if defined(_WIN32)
          const char* home = fossil_sys_env_get("USERPROFILE");
          if (!home) home = fossil_sys_env_get("HOMEPATH");
          return home;
#else
          return fossil_sys_env_get("HOME");
#endif
     }
     if (strcmp(key, "fossil.sys.tmp") == 0) {
#if defined(_WIN32)
          const char* tmp = fossil_sys_env_get("TEMP");
          if (!tmp) tmp = fossil_sys_env_get("TMP");
          return tmp;
#else
          const char* tmp = fossil_sys_env_get("TMPDIR");
          if (!tmp) tmp = "/tmp";
          return tmp;
#endif
     }

     /* Runtime properties */
     if (strcmp(key, "fossil.runtime.mode") == 0) {
          return fossil_sys_env_get("FOSSIL_RUNTIME_MODE");
     }
     if (strcmp(key, "fossil.runtime.debug") == 0) {
          return fossil_sys_env_get("FOSSIL_RUNTIME_DEBUG");
     }
     if (strcmp(key, "fossil.runtime.log.level") == 0) {
          return fossil_sys_env_get("FOSSIL_RUNTIME_LOG_LEVEL");
     }
     if (strcmp(key, "fossil.runtime.log.output") == 0) {
          return fossil_sys_env_get("FOSSIL_RUNTIME_LOG_OUTPUT");
     }
     if (strcmp(key, "fossil.runtime.seed") == 0) {
          return fossil_sys_env_get("FOSSIL_RUNTIME_SEED");
     }
     if (strcmp(key, "fossil.runtime.threads") == 0) {
          return fossil_sys_env_get("FOSSIL_RUNTIME_THREADS");
     }

     /* Application properties */
     if (strcmp(key, "fossil.app.name") == 0) {
          return fossil_sys_env_get("FOSSIL_APP_NAME");
     }
     if (strcmp(key, "fossil.app.version") == 0) {
          return fossil_sys_env_get("FOSSIL_APP_VERSION");
     }
     if (strcmp(key, "fossil.app.instance") == 0) {
          return fossil_sys_env_get("FOSSIL_APP_INSTANCE");
     }

     /* User properties */
     if (strcmp(key, "fossil.user.name") == 0) {
#if defined(_WIN32)
          return fossil_sys_env_get("USERNAME");
#else
          return fossil_sys_env_get("USER");
#endif
     }
     if (strcmp(key, "fossil.user.id") == 0) {
#if defined(_WIN32)
          return fossil_sys_env_get("USERDOMAIN");
#else
          return fossil_sys_env_get("UID");
#endif
     }

     /* Temp/Path properties */
     if (strcmp(key, "fossil.temp.dir") == 0) {
#if defined(_WIN32)
          const char* tmp = fossil_sys_env_get("TEMP");
          if (!tmp) tmp = fossil_sys_env_get("TMP");
          return tmp;
#else
          const char* tmp = fossil_sys_env_get("TMPDIR");
          if (!tmp) tmp = "/tmp";
          return tmp;
#endif
     }
     if (strcmp(key, "fossil.temp.cache") == 0) {
          return fossil_sys_env_get("FOSSIL_TEMP_CACHE");
     }

     /* Fallback: try as OS environment variable */
     return fossil_sys_env_get(key);
}

const char*
fossil_sys_env_get(const char* key) {
     if (!key) return NULL;
     return fossil_sys_env_resolve_id(key);
}

int
fossil_sys_env_set(const char* key, const char* value) {
     if (!key) return -1;

     /* Only allow setting for non-canonical keys or mapped env vars */
     /* For canonical keys, set the mapped env var if possible */
     if (strcmp(key, "fossil.sys.path") == 0) key = "PATH";
     else if (strcmp(key, "fossil.sys.home") == 0) {
#if defined(_WIN32)
          key = "USERPROFILE";
#else
          key = "HOME";
#endif
     }
     else if (strcmp(key, "fossil.sys.tmp") == 0) {
#if defined(_WIN32)
          key = "TEMP";
#else
          key = "TMPDIR";
#endif
     }
     else if (strcmp(key, "fossil.user.name") == 0) {
#if defined(_WIN32)
          key = "USERNAME";
#else
          key = "USER";
#endif
     }
     else if (strcmp(key, "fossil.runtime.mode") == 0) key = "FOSSIL_RUNTIME_MODE";
     else if (strcmp(key, "fossil.runtime.debug") == 0) key = "FOSSIL_RUNTIME_DEBUG";
     else if (strcmp(key, "fossil.runtime.log.level") == 0) key = "FOSSIL_RUNTIME_LOG_LEVEL";
     else if (strcmp(key, "fossil.runtime.log.output") == 0) key = "FOSSIL_RUNTIME_LOG_OUTPUT";
     else if (strcmp(key, "fossil.runtime.seed") == 0) key = "FOSSIL_RUNTIME_SEED";
     else if (strcmp(key, "fossil.runtime.threads") == 0) key = "FOSSIL_RUNTIME_THREADS";
     else if (strcmp(key, "fossil.app.name") == 0) key = "FOSSIL_APP_NAME";
     else if (strcmp(key, "fossil.app.version") == 0) key = "FOSSIL_APP_VERSION";
     else if (strcmp(key, "fossil.app.instance") == 0) key = "FOSSIL_APP_INSTANCE";
     else if (strcmp(key, "fossil.temp.cache") == 0) key = "FOSSIL_TEMP_CACHE";

#if defined(_WIN32)
     return SetEnvironmentVariableA(key, value) ? 0 : -1;
#else
     if (!value) {
          return unsetenv(key);
     }
     return setenv(key, value, 1);
#endif
}

int
fossil_sys_env_exists(const char* key) {
     return fossil_sys_env_get(key) != NULL;
}

/* ============================================================================
    HELPERS
============================================================================ */

const char*
fossil_sys_env_get_or(const char* key, const char* fallback) {
     const char* v = fossil_sys_env_get(key);
     return v ? v : fallback;
}

int
fossil_sys_env_get_int(const char* key, int default_value) {
     const char* v = fossil_sys_env_get(key);
     if (!v) return default_value;

     char* end = NULL;
     long result = strtol(v, &end, 10);

     if (end == v || *end != '\0') {
          return default_value;
     }

     return (int)result;
}

int
fossil_sys_env_get_bool(const char* key, int default_value) {
     const char* v = fossil_sys_env_get(key);
     return fossil_sys_env_parse_bool(v, default_value);
}

/* ============================================================================
    ITERATION
============================================================================ */

void
fossil_sys_env_foreach(fossil_sys_env_iter_cb cb, void* user_data) {
     if (!cb) return;

#if defined(_WIN32)

     LPCH env = GetEnvironmentStringsA();
     if (!env) return;

     LPCH cur = env;

     while (*cur) {
          const char* entry = cur;
          const char* eq = strchr(entry, '=');

          if (eq) {
                size_t key_len = (size_t)(eq - entry);

                char key[256];
                if (key_len < sizeof(key)) {
                     memcpy(key, entry, key_len);
                     key[key_len] = '\0';

                     const char* value = eq + 1;

                     if (cb(key, value, user_data) != 0) {
                          break;
                     }
                }
          }

          cur += strlen(cur) + 1;
     }

     FreeEnvironmentStringsA(env);

#else

     extern char** environ;

     for (char** e = environ; *e; ++e) {
          const char* entry = *e;
          const char* eq = strchr(entry, '=');

          if (!eq) continue;

          size_t key_len = (size_t)(eq - entry);

          char key[256];
          if (key_len >= sizeof(key)) continue;

          memcpy(key, entry, key_len);
          key[key_len] = '\0';

          const char* value = eq + 1;

          if (cb(key, value, user_data) != 0) {
                break;
          }
     }

#endif
}
