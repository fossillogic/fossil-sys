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
#ifndef FOSSIL_SYS_EVENT_H
#define FOSSIL_SYS_EVENT_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ------------------------------------------------------
 * Event Types
 * ----------------------------------------------------- */
typedef enum {
    FOSSIL_EVENT_NONE,
    FOSSIL_EVENT_IO,
    FOSSIL_EVENT_TIMER,
    FOSSIL_EVENT_SIGNAL,
    FOSSIL_EVENT_CUSTOM
} fossil_sys_event_type_t;

/* ------------------------------------------------------
 * Event Structure
 * ----------------------------------------------------- */
typedef struct {
    const char* id;                // string ID for AI/tracking
    fossil_sys_event_type_t type;
    void* payload;                 // user-defined data
    size_t size;                   // payload size
} fossil_sys_event_t;

/* ------------------------------------------------------
 * Event API
 * ----------------------------------------------------- */

/**
 * Initialize the event subsystem.
 * Must be called before using any other event functions.
 * 
 * @return 0 on success, negative value on failure
 */
int fossil_sys_event_init(void);

/**
 * Poll for events without blocking.
 * Retrieves the next available event if one exists.
 * 
 * @param out_event Pointer to event structure to fill with event data
 * @return 0 if event was retrieved, 1 if no event available, negative on error
 */
int fossil_sys_event_poll(fossil_sys_event_t* out_event);

/**
 * Wait for the next event with optional timeout.
 * Blocks until an event is available or timeout expires.
 * 
 * @param out_event Pointer to event structure to fill with event data
 * @param timeout_ms Maximum time to wait in milliseconds (0 = infinite)
 * @return 0 on success, 1 on timeout, negative on error
 */
int fossil_sys_event_wait(fossil_sys_event_t* out_event, uint32_t timeout_ms);

/**
 * Post a custom event to the event queue.
 * The payload is copied internally if necessary.
 * 
 * @param id String identifier for the event
 * @param payload User-defined data (can be NULL)
 * @param size Size of payload in bytes
 * @return 0 on success, negative on failure
 */
int fossil_sys_event_post(const char* id, void* payload, size_t size);

/**
 * Shutdown the event subsystem and release all resources.
 * Should be called when event system is no longer needed.
 */
void fossil_sys_event_shutdown(void);

#ifdef __cplusplus
}

namespace fossil::sys {

class Event {
public:
    /**
     * Initialize the event subsystem.
     * Must be called before using any other event functions.
     * 
     * @return 0 on success, negative value on failure
     */
    static int init(void) {
        return fossil_sys_event_init();
    }

    /**
     * Poll for events without blocking.
     * Retrieves the next available event if one exists.
     * 
     * @param out_event Pointer to event structure to fill with event data
     * @return 0 if event was retrieved, 1 if no event available, negative on error
     */
    static int poll(fossil_sys_event_t* out_event) {
        return fossil_sys_event_poll(out_event);
    }

    /**
     * Wait for the next event with optional timeout.
     * Blocks until an event is available or timeout expires.
     * 
     * @param out_event Pointer to event structure to fill with event data
     * @param timeout_ms Maximum time to wait in milliseconds (0 = infinite)
     * @return 0 on success, 1 on timeout, negative on error
     */
    static int wait(fossil_sys_event_t* out_event, uint32_t timeout_ms) {
        return fossil_sys_event_wait(out_event, timeout_ms);
    }

    /**
     * Post a custom event to the event queue.
     * The payload is copied internally if necessary.
     * 
     * @param id String identifier for the event
     * @param payload User-defined data (can be NULL)
     * @param size Size of payload in bytes
     * @return 0 on success, negative on failure
     */
    static int post(const char* id, void* payload, size_t size) {
        return fossil_sys_event_post(id, payload, size);
    }

    /**
     * Shutdown the event subsystem and release all resources.
     * Should be called when event system is no longer needed.
     */
    static void shutdown(void) {
        fossil_sys_event_shutdown();
    }
};

} // namespace fossil::sys

#endif

#endif /* FOSSIL_SYS_EVENT_H */
