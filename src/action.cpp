/******************************************************************************/
/*! \file src/action.cpp
 *
 * Abstract Action class, KeyEvent and ButtonEvent and many simple actions.
 */
/*******************************************************************************
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

#include "action.h"
#include "log.h"
#include "client.h"
#include "event.h"
#include "tools.h"

#include <unistd.h>
#include <cstring>

//! Virtual destructor called when the binding is released.
Action::~Action()
{ }

//! Virtual operator() stub called for keyboard events.
void Action::operator () (KeyEvent&)
{
    TRACE << "called Action operator(KeyEvent&) stub";
}

//! Virtual operator() stub called for mouse button events.
void Action::operator () (ButtonEvent&)
{
    TRACE << "called Action operator(ButtonEvent&) stub";
}

////////////////////////////////////////////////////////////////////////////////

static void action_key_terminate(KeyEvent&)
{
    TRACE << "action_key_terminate()";

    EventLoop::terminate();
}

static void mouse_move_handler(ButtonEvent& be)
{
    TRACE << "mouse_move_handler()";

    if (!be.client()) {
        ERROR << "Called mouse move handler without client";
        return;
    }

    Client& c = *be.client();

    Point click_pos = be.root_pos();
    Point win_pos = c.m_geometry.origin();

    xcb_grab_pointer_cookie_t gpc =
        xcb_grab_pointer(g_xcb.connection, 0, c.window(),
                         XCB_EVENT_MASK_BUTTON_PRESS |
                         XCB_EVENT_MASK_BUTTON_RELEASE |
                         XCB_EVENT_MASK_BUTTON_MOTION |
                         XCB_EVENT_MASK_POINTER_MOTION,
                         XCB_GRAB_MODE_ASYNC, XCB_GRAB_MODE_ASYNC,
                         XCB_WINDOW_NONE, g_xcb.CR_fleur.cursor,
                         XCB_CURRENT_TIME);

    autofree_ptr<xcb_grab_pointer_reply_t> gpr(
        xcb_grab_pointer_reply(g_xcb.connection, gpc, NULL)
        );

    if (!gpr || gpr->status != XCB_GRAB_STATUS_SUCCESS) {
        ERROR << "Could not grab pointer for receiving mouse movement events";
        return;
    }

    bool moving = true;
    autofree_ptr<xcb_generic_event_t> event;
    xcb_timestamp_t timestamp = 0;

    while (moving && (event = EventLoop::wait()))
    {
        switch (XCB_EVENT_RESPONSE_TYPE(event.get()))
        {
        case XCB_MOTION_NOTIFY: {
            xcb_motion_notify_event_t* ev
                = (xcb_motion_notify_event_t*)event.get();

            TRACE2 << "motion_event handler: " << *ev;

            // calculate new window origin
            c.m_geometry.set_origin(
                win_pos + Point(ev->root_x, ev->root_y) - click_pos
                );

            // only do actual movement every 10 milliseconds
            if ((ev->time - timestamp) * 100 >= 1000) {
                timestamp = ev->time;
                be.client()->m_win.move(c.m_geometry.origin());
            }

            break;
        }
        case XCB_BUTTON_RELEASE: {
            xcb_button_release_event_t* ev
                = (xcb_button_release_event_t*)event.get();

            TRACE2 << "button_release event handler: " << *ev;
            moving = false;
            break;
        }
        case XCB_KEY_PRESS: {
            // ignore key press events during mouse operation.
            xcb_key_press_event_t* ev = (xcb_key_press_event_t*)event.get();
            TRACE2 << "key_press event handler: " << *ev;
            xcb_allow_events(g_xcb.connection, XCB_ALLOW_SYNC_KEYBOARD,
                             ev->time);
            break;
        }
        default:
            // TODO: which other events should be ignored?
            EventLoop::process_global(event.get());
        }
    }

    xcb_ungrab_pointer(g_xcb.connection, XCB_CURRENT_TIME);

    INFO << "end mouse_move_handler()";
}

static void mouse_resize_handler(ButtonEvent& be)
{
    TRACE << "mouse_resize_handler()";

    if (!be.client()) {
        ERROR << "Called mouse resize handler without client";
        return;
    }

    Client& c = *be.client();

    Point click_pos = be.root_pos();
    Rectangle win_geo = c.m_geometry;

    // cursor in left/right or top/bottom halves
    bool left = (be.pos().x < win_geo.w / 2);
    bool top = (be.pos().y < win_geo.h / 2);

    xcb_cursor_t cursor =
        (left && top ? g_xcb.CR_top_left_corner.cursor :
         !left && top ? g_xcb.CR_top_right_corner.cursor :
         left && !top ? g_xcb.CR_bottom_left_corner.cursor :
         !left && !top ? g_xcb.CR_bottom_right_corner.cursor :
         g_xcb.CR_top_left_corner.cursor);

    xcb_grab_pointer_cookie_t gpc =
        xcb_grab_pointer(g_xcb.connection, 0, c.window(),
                         XCB_EVENT_MASK_BUTTON_PRESS |
                         XCB_EVENT_MASK_BUTTON_RELEASE |
                         XCB_EVENT_MASK_BUTTON_MOTION |
                         XCB_EVENT_MASK_POINTER_MOTION,
                         XCB_GRAB_MODE_ASYNC, XCB_GRAB_MODE_ASYNC,
                         XCB_WINDOW_NONE, cursor,
                         XCB_CURRENT_TIME);

    autofree_ptr<xcb_grab_pointer_reply_t> gpr(
        xcb_grab_pointer_reply(g_xcb.connection, gpc, NULL)
        );

    if (!gpr || gpr->status != XCB_GRAB_STATUS_SUCCESS) {
        ERROR << "Could not grab pointer for receiving mouse movement events";
        return;
    }

    bool moving = true;
    autofree_ptr<xcb_generic_event_t> event;
    xcb_timestamp_t timestamp = 0;

    while (moving && (event = EventLoop::wait()))
    {
        switch (XCB_EVENT_RESPONSE_TYPE(event.get()))
        {
        case XCB_MOTION_NOTIFY: {
            xcb_motion_notify_event_t* ev
                = (xcb_motion_notify_event_t*)event.get();

            TRACE2 << "motion_event handler: " << *ev;

            // calculate relative cursor movement
            Point delta = click_pos - Point(ev->root_x, ev->root_y);

            // calculate new window rectangle
            Rectangle& new_geo = c.m_geometry;
            new_geo = win_geo;

            if (left) {
                new_geo.x += -delta.x;
            }
            else {
                delta.x = -delta.x;
            }

            uint16_t width = add_limit_overflow(new_geo.w, delta.x);

            if (top) {
                new_geo.y += -delta.y;
            }
            else {
                delta.y = -delta.y;
            }

            uint16_t height = add_limit_overflow(new_geo.h, delta.y);

            // apply WM_NORMAL_HINTS / WM_SIZE_HINTS
            c.m_wm_size_hints.apply(width, height);

            new_geo.w = width;
            new_geo.h = height;

            // only do actual movement every 10 milliseconds
            if ((ev->time - timestamp) * 100 >= 1000) {
                timestamp = ev->time;
                be.client()->m_win.move_resize(new_geo);
            }

            break;
        }
        case XCB_BUTTON_RELEASE: {
            xcb_button_release_event_t* ev
                = (xcb_button_release_event_t*)event.get();

            TRACE2 << "button_release event handler: " << *ev;
            moving = false;
            break;
        }
        case XCB_KEY_PRESS: {
            // ignore key press events during mouse operation.
            xcb_key_press_event_t* ev = (xcb_key_press_event_t*)event.get();
            TRACE << "key_press event handler: " << *ev;
            xcb_allow_events(g_xcb.connection, XCB_ALLOW_SYNC_KEYBOARD,
                             ev->time);
            break;
        }
        default:
            // TODO: which other events should be ignored?
            EventLoop::process_global(event.get());
        }
    }

    xcb_ungrab_pointer(g_xcb.connection, XCB_CURRENT_TIME);

    INFO << "end mouse_resize_handler()";
}

static void action_key_quit_window(KeyEvent& ke)
{
    TRACE << "action_key_quit_window()";

    if (!ke.client()) {
        ERROR << "Called quit_window without client";
        return;
    }

    Client& c = *ke.client();

    if (c.m_can_delete_window)
        c.m_win.wm_delete_window();
    else {
        INFO << "Window " << c.window() << " does not have WM_DELETE_WINDOW.";
    }
}

/*!
 * An Action class used to spawn children programs on keyboard or mouse events.
 */
class ActionSpawn : public Action
{
protected:
    //! Program with command line arguments to spawn
    std::vector<std::string> m_progargs;

public:
    //! Launch a program with command line arguments
    ActionSpawn(const std::vector<std::string>& progargs)
        : m_progargs(progargs)
    { }

    //! Launch a program with no command line arguments
    ActionSpawn(const std::string& program)
    {
        m_progargs.emplace_back(program);
    }

    //! Action on keyboard press events
    void operator () (KeyEvent&)
    {
        TRACE << "action_swap()";

        // fork into child program
        if (fork()) return;

        // close the X11 connection in child
        if (g_xcb.connection) {
            close(g_xcb.get_file_descriptor());
        }

        // start a new session in process tree
        if (setsid() == -1) {
            WARN << "spawn: setsid() failed: " << strerror(errno);
        }

        // construct arguments for execvp():
        std::vector<const char*> args(m_progargs.size() + 1);
        for (size_t i = 0; i < m_progargs.size(); ++i)
            args[i] = m_progargs[i].c_str();
        args.back() = NULL;

        execvp(args[0], (char* const*)args.data());
        //ERROR << "execvp() running \"" << terminal << "\" : " <<
        // strerror(errno);
        exit(EXIT_FAILURE);
    }
};

#include "binding.h"
#include <X11/keysym.h>

void BindingList::add_test_bindings()
{
    // add a test key binding

    s_kblist.emplace_back(
        BIND_CLIENTS, XCB_MOD_MASK_CONTROL, XK_q, action_key_quit_window
        );

    ActionSpawn* as_dmenu = new ActionSpawn("/usr/bin/dmenu_run");

    s_kblist.emplace_back(
        BIND_ROOT, XCB_MOD_MASK_CONTROL, XK_s, as_dmenu
        );

    s_kblist.emplace_back(
        BIND_ROOT, XCB_MOD_MASK_CONTROL | XCB_MOD_MASK_SHIFT, XK_q,
        action_key_terminate
        );

    // add a test mouse binding

    s_bblist.emplace_back(
        BIND_CLIENTS, XCB_MOD_MASK_CONTROL, XCB_BUTTON_INDEX_1,
        mouse_move_handler
        );

    s_bblist.emplace_back(
        BIND_CLIENTS, XCB_MOD_MASK_CONTROL, XCB_BUTTON_INDEX_3,
        mouse_resize_handler
        );
}

/******************************************************************************/
