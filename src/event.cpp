/*******************************************************************************
 * src/event.cpp
 *
 * Event table mapping and event loop dispatcher
 *
 *******************************************************************************
 * Copyright (C) 2014 Timo Bingmann <tb@panthema.net>
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************/

#include "event.h"

//! the global event handler table (called after override event table)
EventLoop::eventtable_type EventLoop::s_eventtable;

//! global (graceful) termination flag
bool EventLoop::s_terminate = false;

//! Event handler for error messages
static void handle_event_error(xcb_generic_event_t* event)
{
    xcb_generic_error_t* e = (xcb_generic_error_t*)event;

    if (!e)
    {
        ERROR << "XCB Error: NULL";
    }
    else
    {
        ERROR << "XCB Error:"
              << " error_code: " << xcb_event_get_error_label(e->error_code)
              << " (" << uint32_t(e->error_code) << ")"
              << " major_code: " << xcb_event_get_request_label(e->major_code)
              << " (" << uint32_t(e->major_code) << ")"
              << " minor_code: " << uint32_t(e->minor_code)
              << " sequence: " << e->sequence
              << " resource_id: " << e->resource_id;
        free(e);
    }
}

//! Populate global event handler table
void EventLoop::setup_global_eventtable()
{
    for (event_handler_type& h : s_eventtable) h = NULL;

    s_eventtable[0] = handle_event_error;
}

/******************************************************************************/
