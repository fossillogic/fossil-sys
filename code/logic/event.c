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
#include "fossil/sys/event.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

#define MAX_EVENTS 256

/* ------------------------------------------------------
 * Internal Event Queue
 * ----------------------------------------------------- */
static fossil_sys_event_t event_queue[MAX_EVENTS];
static size_t event_count = 0;

/* ------------------------------------------------------
 * Initialization
 * ----------------------------------------------------- */
int fossil_sys_event_init(void) {
    event_count = 0;
    memset(event_queue, 0, sizeof(event_queue));
    return 0;
}

/* ------------------------------------------------------
 * Poll events (non-blocking)
 * ----------------------------------------------------- */
int fossil_sys_event_poll(fossil_sys_event_t* out_event) {
    if (!out_event) return -1;

    if (event_count == 0) return 0; // no events

    *out_event = event_queue[0];

    // Shift remaining events
    memmove(event_queue, event_queue + 1, (event_count - 1) * sizeof(fossil_sys_event_t));
    event_count--;

    return 1; // event returned
}

/* ------------------------------------------------------
 * Wait for next event (blocking with timeout)
 * ----------------------------------------------------- */
int fossil_sys_event_wait(fossil_sys_event_t* out_event, uint32_t timeout_ms) {
    if (!out_event) return -1;

    clock_t start = clock();
    while (event_count == 0) {
        clock_t now = clock();
        uint32_t elapsed_ms = (uint32_t)((now - start) * 1000 / CLOCKS_PER_SEC);
        if (elapsed_ms >= timeout_ms) return 0; // timeout
    }

    return fossil_sys_event_poll(out_event);
}

/* ------------------------------------------------------
 * Post a custom event
 * ----------------------------------------------------- */
int fossil_sys_event_post(const char* id, void* payload, size_t size) {
    if (event_count >= MAX_EVENTS) return -1; // queue full

    fossil_sys_event_t* e = &event_queue[event_count];
    e->id = id;
    e->type = FOSSIL_EVENT_CUSTOM;
    e->size = size;

    if (payload && size > 0) {
        e->payload = malloc(size);
        if (!e->payload) return -1;
        memcpy(e->payload, payload, size);
    } else {
        e->payload = NULL;
    }

    event_count++;
    return 0;
}

/* ------------------------------------------------------
 * Shutdown
 * ----------------------------------------------------- */
void fossil_sys_event_shutdown(void) {
    // Free any allocated payloads
    for (size_t i = 0; i < event_count; i++) {
        free(event_queue[i].payload);
        event_queue[i].payload = NULL;
    }
    event_count = 0;
}
