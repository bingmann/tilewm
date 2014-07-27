/*******************************************************************************
 * src/event.h
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

#ifndef TILEWM_EVENT_HEADER
#define TILEWM_EVENT_HEADER

#include "xcb.h"
#include "log.h"
#include <array>
#include <xcb/xcb_event.h>

//! All event handlers called by the EventLoop class have this type
typedef void (* event_handler_type)(xcb_generic_event_t* event);

/*!
 * EventLoop is a wrapper for xcb_wait_for_event() calls and contains an event
 * handler table to call the configured handlers.
 */
class EventLoop
{
public:
    //! fixed-size array of event handlers
    typedef std::array<event_handler_type, XCB_NO_OPERATION> eventtable_type;

protected:
    //! the global event handler table (called after override event table)
    static eventtable_type s_eventtable;

    //! global (graceful) termination flag
    static bool s_terminate;

public:
    //! Set global graceful termination flag
    static void terminate()
    {
        s_terminate = true;
    }

    //! Populate global event handler table
    static void setup_global_eventtable();

    //! Process an event according to the global event handler table
    static void process_global(xcb_generic_event_t* event)
    {
        if (!event) return;

        uint8_t evtype = XCB_EVENT_RESPONSE_TYPE(event);

        if (evtype < s_eventtable.size() && s_eventtable[evtype])
            s_eventtable[evtype](event);
        else
            ERROR << "Unknown event type " << uint32_t(evtype);
    }

    //! Process all event until terminate() is called.
    static void loop_global()
    {
        while (!s_terminate)
        {
            g_xcb.flush();

            if (g_xcb.connection_has_error()) {
                FATAL << "X11 connection got interrupted";
                exit(EXIT_FAILURE);
            }

            TRACE << "xcb_wait_for_event()";
            xcb_generic_event_t* event = xcb_wait_for_event(g_xcb.connection);

            if (event) {
                process_global(event);
                free(event);
            }
        }
    }
};

#endif // !TILEWM_EVENT_HEADER

/******************************************************************************/
