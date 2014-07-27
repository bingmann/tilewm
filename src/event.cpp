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

//! Event handler stub for XCB_KEY_PRESS
static void handle_event_key_press(xcb_generic_event_t* event)
{
    xcb_key_press_event_t* e = (xcb_key_press_event_t*)event;
    TRACE << "Stub event handler: " << *e;
}

//! Event handler stub for XCB_KEY_RELEASE
static void handle_event_key_release(xcb_generic_event_t* event)
{
    xcb_key_release_event_t* e = (xcb_key_release_event_t*)event;
    TRACE << "Stub event handler: " << *e;
}

//! Event handler stub for XCB_BUTTON_PRESS
static void handle_event_button_press(xcb_generic_event_t* event)
{
    xcb_button_press_event_t* e = (xcb_button_press_event_t*)event;
    TRACE << "Stub event handler: " << *e;
}

//! Event handler stub for XCB_BUTTON_RELEASE
static void handle_event_button_release(xcb_generic_event_t* event)
{
    xcb_button_release_event_t* e = (xcb_button_release_event_t*)event;
    TRACE << "Stub event handler: " << *e;
}

//! Event handler stub for XCB_MOTION_NOTIFY
static void handle_event_motion_notify(xcb_generic_event_t* event)
{
    xcb_motion_notify_event_t* e = (xcb_motion_notify_event_t*)event;
    TRACE << "Stub event handler: " << *e;
}

//! Event handler stub for XCB_ENTER_NOTIFY
static void handle_event_enter_notify(xcb_generic_event_t* event)
{
    xcb_enter_notify_event_t* e = (xcb_enter_notify_event_t*)event;
    TRACE << "Stub event handler: " << *e;
}

//! Event handler stub for XCB_LEAVE_NOTIFY
static void handle_event_leave_notify(xcb_generic_event_t* event)
{
    xcb_leave_notify_event_t* e = (xcb_leave_notify_event_t*)event;
    TRACE << "Stub event handler: " << *e;
}

//! Event handler stub for XCB_FOCUS_IN
static void handle_event_focus_in(xcb_generic_event_t* event)
{
    xcb_focus_in_event_t* e = (xcb_focus_in_event_t*)event;
    TRACE << "Stub event handler: " << *e;
}

//! Event handler stub for XCB_FOCUS_OUT
static void handle_event_focus_out(xcb_generic_event_t* event)
{
    xcb_focus_out_event_t* e = (xcb_focus_out_event_t*)event;
    TRACE << "Stub event handler: " << *e;
}

//! Event handler stub for XCB_EXPOSE
static void handle_event_expose(xcb_generic_event_t* event)
{
    xcb_expose_event_t* e = (xcb_expose_event_t*)event;
    TRACE << "Stub event handler: " << *e;
}

//! Event handler stub for XCB_GRAPHICS_EXPOSURE
static void handle_event_graphics_exposure(xcb_generic_event_t* event)
{
    xcb_graphics_exposure_event_t* e = (xcb_graphics_exposure_event_t*)event;
    TRACE << "Stub event handler: " << *e;
}

//! Event handler stub for XCB_NO_EXPOSURE
static void handle_event_no_exposure(xcb_generic_event_t* event)
{
    xcb_no_exposure_event_t* e = (xcb_no_exposure_event_t*)event;
    TRACE << "Stub event handler: " << *e;
}

//! Event handler stub for XCB_VISIBILITY_NOTIFY
static void handle_event_visibility_notify(xcb_generic_event_t* event)
{
    xcb_visibility_notify_event_t* e = (xcb_visibility_notify_event_t*)event;
    TRACE << "Stub event handler: " << *e;
}

//! Event handler stub for XCB_CREATE_NOTIFY
static void handle_event_create_notify(xcb_generic_event_t* event)
{
    xcb_create_notify_event_t* e = (xcb_create_notify_event_t*)event;
    TRACE << "Stub event handler: " << *e;
}

//! Event handler stub for XCB_DESTROY_NOTIFY
static void handle_event_destroy_notify(xcb_generic_event_t* event)
{
    xcb_destroy_notify_event_t* e = (xcb_destroy_notify_event_t*)event;
    TRACE << "Stub event handler: " << *e;
}

//! Event handler stub for XCB_UNMAP_NOTIFY
static void handle_event_unmap_notify(xcb_generic_event_t* event)
{
    xcb_unmap_notify_event_t* e = (xcb_unmap_notify_event_t*)event;
    TRACE << "Stub event handler: " << *e;
}

//! Event handler stub for XCB_MAP_NOTIFY
static void handle_event_map_notify(xcb_generic_event_t* event)
{
    xcb_map_notify_event_t* e = (xcb_map_notify_event_t*)event;
    TRACE << "Stub event handler: " << *e;
}

//! Event handler stub for XCB_MAP_REQUEST
static void handle_event_map_request(xcb_generic_event_t* event)
{
    xcb_map_request_event_t* e = (xcb_map_request_event_t*)event;
    TRACE << "Stub event handler: " << *e;
}

//! Event handler stub for XCB_REPARENT_NOTIFY
static void handle_event_reparent_notify(xcb_generic_event_t* event)
{
    xcb_reparent_notify_event_t* e = (xcb_reparent_notify_event_t*)event;
    TRACE << "Stub event handler: " << *e;
}

//! Event handler stub for XCB_CONFIGURE_NOTIFY
static void handle_event_configure_notify(xcb_generic_event_t* event)
{
    xcb_configure_notify_event_t* e = (xcb_configure_notify_event_t*)event;
    TRACE << "Stub event handler: " << *e;
}

//! Event handler stub for XCB_CONFIGURE_REQUEST
static void handle_event_configure_request(xcb_generic_event_t* event)
{
    xcb_configure_request_event_t* e = (xcb_configure_request_event_t*)event;
    TRACE << "Stub event handler: " << *e;
}

//! Event handler stub for XCB_PROPERTY_NOTIFY
static void handle_event_property_notify(xcb_generic_event_t* event)
{
    xcb_property_notify_event_t* e = (xcb_property_notify_event_t*)event;
    TRACE << "Stub event handler: " << *e;
}

//! Event handler stub for XCB_CLIENT_MESSAGE
static void handle_event_client_message(xcb_generic_event_t* event)
{
    xcb_client_message_event_t* e = (xcb_client_message_event_t*)event;
    TRACE << "Stub event handler: " << *e;
}

//! Event handler stub for XCB_CLIENT_MESSAGE
static void handle_event_mapping_notify(xcb_generic_event_t* event)
{
    xcb_mapping_notify_event_t* e = (xcb_mapping_notify_event_t*)event;
    TRACE << "Stub event handler: " << *e;
}

//! Populate global event handler table
void EventLoop::setup_global_eventtable()
{
    for (event_handler_type& h : s_eventtable) h = NULL;

    s_eventtable[0] = handle_event_error;
    s_eventtable[XCB_KEY_PRESS] = handle_event_key_press;                 // 2
    s_eventtable[XCB_KEY_RELEASE] = handle_event_key_release;             // 3
    s_eventtable[XCB_BUTTON_PRESS] = handle_event_button_press;           // 4
    s_eventtable[XCB_BUTTON_RELEASE] = handle_event_button_release;       // 5
    s_eventtable[XCB_MOTION_NOTIFY] = handle_event_motion_notify;         // 6
    s_eventtable[XCB_ENTER_NOTIFY] = handle_event_enter_notify;           // 7
    s_eventtable[XCB_LEAVE_NOTIFY] = handle_event_leave_notify;           // 8
    s_eventtable[XCB_FOCUS_IN] = handle_event_focus_in;                   // 9
    s_eventtable[XCB_FOCUS_OUT] = handle_event_focus_out;                 // 10
    s_eventtable[XCB_EXPOSE] = handle_event_expose;                       // 12
    s_eventtable[XCB_GRAPHICS_EXPOSURE] = handle_event_graphics_exposure; // 13
    s_eventtable[XCB_NO_EXPOSURE] = handle_event_no_exposure;             // 14
    s_eventtable[XCB_VISIBILITY_NOTIFY] = handle_event_visibility_notify; // 15
    s_eventtable[XCB_CREATE_NOTIFY] = handle_event_create_notify;         // 16
    s_eventtable[XCB_DESTROY_NOTIFY] = handle_event_destroy_notify;       // 17
    s_eventtable[XCB_UNMAP_NOTIFY] = handle_event_unmap_notify;           // 18
    s_eventtable[XCB_MAP_NOTIFY] = handle_event_map_notify;               // 19
    s_eventtable[XCB_MAP_REQUEST] = handle_event_map_request;             // 20
    s_eventtable[XCB_REPARENT_NOTIFY] = handle_event_reparent_notify;     // 21
    s_eventtable[XCB_CONFIGURE_NOTIFY] = handle_event_configure_notify;   // 22
    s_eventtable[XCB_CONFIGURE_REQUEST] = handle_event_configure_request; // 23
    s_eventtable[XCB_PROPERTY_NOTIFY] = handle_event_property_notify;     // 28
    s_eventtable[XCB_CLIENT_MESSAGE] = handle_event_client_message;       // 33
    s_eventtable[XCB_MAPPING_NOTIFY] = handle_event_mapping_notify;       // 34
}

/******************************************************************************/
