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
#include "fossil/sys/dynamic.h"

#include <string.h>
#include <stdio.h>

static char fossil_dyn_error_buf[256];

static void fossil_dyn_set_error(const char* msg)
{
    if (!msg) msg = "unknown dynamic loader error";
    strncpy(fossil_dyn_error_buf, msg, sizeof(fossil_dyn_error_buf)-1);
    fossil_dyn_error_buf[sizeof(fossil_dyn_error_buf)-1] = '\0';
}

/* ------------------------------------------------------
 * Platform helpers
 * ----------------------------------------------------- */

static const char* fossil_dyn_default_ext(void)
{
#if defined(_WIN32) || defined(_WIN64)
    return ".dll";
#elif defined(__APPLE__)
    return ".dylib";
#else
    return ".so";
#endif
}

/* If path has no extension, append platform default */
static void fossil_dyn_resolve_path(
    const char* in,
    char* out,
    size_t out_size)
{
    if (!in || !out || out_size == 0)
        return;

    const char* dot = strrchr(in, '.');
    if (dot) {
        snprintf(out, out_size, "%s", in);
    } else {
        snprintf(out, out_size, "%s%s", in, fossil_dyn_default_ext());
    }
}

#if defined(_WIN32) || defined(_WIN64)

/* ======================================================
 * Microslop implementation
 * ====================================================== */

#include <windows.h>

bool fossil_sys_dynamic_load(
    const char* path,
    fossil_sys_dynamic_lib_t* out_lib)
{
    if (!path || !out_lib)
        return false;

    char resolved[512];
    fossil_dyn_resolve_path(path, resolved, sizeof(resolved));

    HMODULE h = LoadLibraryA(resolved);
    if (!h) {
        FormatMessageA(
            FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            GetLastError(),
            0,
            fossil_dyn_error_buf,
            sizeof(fossil_dyn_error_buf),
            NULL);
        return false;
    }

    out_lib->id     = resolved;
    out_lib->path   = resolved;
    out_lib->handle = h;
    out_lib->status = 1;
    return true;
}

bool fossil_sys_dynamic_unload(fossil_sys_dynamic_lib_t* lib)
{
    if (!lib || !lib->handle)
        return false;

    if (!FreeLibrary(lib->handle)) {
        fossil_dyn_set_error("FreeLibrary failed");
        return false;
    }

    lib->handle = NULL;
    lib->status = 0;
    return true;
}

void* fossil_sys_dynamic_symbol(
    fossil_sys_dynamic_lib_t* lib,
    const char* symbol_name)
{
    if (!lib || !lib->handle || !symbol_name)
        return NULL;

    FARPROC proc = GetProcAddress(lib->handle, symbol_name);
    if (!proc) {
        fossil_dyn_set_error("symbol not found");
        return NULL;
    }

    /* legal conversion via memcpy avoids pedantic UB */
    void* out = NULL;
    memcpy(&out, &proc, sizeof(out));
    return out;
}

bool fossil_sys_dynamic_is_loaded(
    const fossil_sys_dynamic_lib_t* lib)
{
    return lib && lib->handle != NULL && lib->status == 1;
}

const char* fossil_sys_dynamic_error(void)
{
    return fossil_dyn_error_buf;
}

#else

/* ======================================================
 * POSIX implementation
 * ====================================================== */

#include <dlfcn.h>

bool fossil_sys_dynamic_load(
    const char* path,
    fossil_sys_dynamic_lib_t* out_lib)
{
    if (!path || !out_lib)
        return false;

    char resolved[512];
    fossil_dyn_resolve_path(path, resolved, sizeof(resolved));

    dlerror();

    void* h = dlopen(resolved, RTLD_NOW | RTLD_LOCAL);
    if (!h) {
        fossil_dyn_set_error(dlerror());
        return false;
    }

    out_lib->id     = resolved;
    out_lib->path   = resolved;
    out_lib->handle = h;
    out_lib->status = 1;
    return true;
}

bool fossil_sys_dynamic_unload(fossil_sys_dynamic_lib_t* lib)
{
    if (!lib || !lib->handle)
        return false;

    if (dlclose(lib->handle) != 0) {
        fossil_dyn_set_error(dlerror());
        return false;
    }

    lib->handle = NULL;
    lib->status = 0;
    return true;
}

void* fossil_sys_dynamic_symbol(
    fossil_sys_dynamic_lib_t* lib,
    const char* symbol_name)
{
    if (!lib || !lib->handle || !symbol_name)
        return NULL;

    dlerror();

    void* sym = dlsym(lib->handle, symbol_name);

    const char* err = dlerror();
    if (err) {
        fossil_dyn_set_error(err);
        return NULL;
    }

    return sym;
}

bool fossil_sys_dynamic_is_loaded(
    const fossil_sys_dynamic_lib_t* lib)
{
    return lib && lib->handle != NULL && lib->status == 1;
}

const char* fossil_sys_dynamic_error(void)
{
    return fossil_dyn_error_buf;
}

#endif
