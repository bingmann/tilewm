/*******************************************************************************
 * src/xcb-window.h
 *
 * Crude C++ abstraction of an XCB Window object in the X window server.
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

#ifndef TILEWM_XCB_WINDOW_HEADER
#define TILEWM_XCB_WINDOW_HEADER

#include "xcb.h"
#include "geometry.h"

/*!
 * The XcbWindow is a direct wrapper for xcb_window_t and provides a number of
 * convenience functions, all with zero overhead.
 */
class XcbWindow
{
protected:
    //! The referenced X window
    xcb_window_t m_window;

public:
    //! Constructor for a specific existing window.
    XcbWindow(xcb_window_t window)
        : m_window(window)
    { }

    //! Return the referenced window id.
    xcb_window_t window()
    {
        return m_window;
    }

    // *** xcb_configure_window()

    //! Move a window to (x,y).
    void move(int16_t x, int16_t y)
    {
        uint32_t values[2] = { (uint32_t)x, (uint32_t)y };
        xcb_configure_window(g_xcb.connection, m_window,
                             XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y,
                             values);
    }

    //! Move a window to point p.
    void move(const Point& p)
    {
        return move(p.x, p.y);
    }

    //! Resize a window to to (w,h).
    void resize(uint16_t w, uint16_t h)
    {
        uint32_t values[2] = { w, h };
        xcb_configure_window(g_xcb.connection, m_window,
                             XCB_CONFIG_WINDOW_WIDTH | XCB_CONFIG_WINDOW_HEIGHT,
                             values);
    }

    //! Move a window to (x,y) and resize to (w,h).
    void move_resize(int16_t x, int16_t y, uint16_t w, uint16_t h)
    {
        uint32_t values[4] = { (uint32_t)x, (uint32_t)y, w, h };
        xcb_configure_window(g_xcb.connection, m_window,
                             XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y |
                             XCB_CONFIG_WINDOW_WIDTH | XCB_CONFIG_WINDOW_HEIGHT,
                             values);
    }

    //! Move and resize a window to the given Rectangle.
    void move_resize(const Rectangle& r)
    {
        return move_resize(r.x, r.y, r.w, r.h);
    }

    //! Set the windows border width.
    void set_border_width(uint32_t b)
    {
        xcb_configure_window(g_xcb.connection, m_window,
                             XCB_CONFIG_WINDOW_BORDER_WIDTH,
                             &b);
    }

    //! Set the windows border pixel.
    void set_border_pixel(uint32_t p)
    {
        xcb_change_window_attributes(g_xcb.connection, m_window,
                                     XCB_CW_BORDER_PIXEL,
                                     &p);
    }

    //! Change window stacking order.
    void stack(xcb_stack_mode_t stack)
    {
        uint32_t value = stack;
        xcb_configure_window(g_xcb.connection, m_window,
                             XCB_CONFIG_WINDOW_STACK_MODE,
                             &value);
    }

    //! Change window stacking order: raise this window to the top.
    void stack_above()
    {
        stack(XCB_STACK_MODE_ABOVE);
    }

    //! Change window stacking order: lower this window to the bottom.
    void stack_below()
    {
        stack(XCB_STACK_MODE_BELOW);
    }

    // *** xcb_map/unmap_window()

    //! Map the window to the screen.
    void map_window()
    {
        xcb_map_window(g_xcb.connection, m_window);
    }

    //! Unmap the window from the screen.
    void unmap_window()
    {
        xcb_unmap_window(g_xcb.connection, m_window);
    }

    // *** other window functions

    //! Close the window by either sending a client message or terminating the
    //! X connection.
    void kill_client()
    {
        xcb_kill_client(g_xcb.connection, m_window);
    }

    // *** ICCCM properties

    //! Change ICCCM WM_STATE property.
    void set_wm_state(uint32_t state)
    {
        uint32_t values[2] = { state, XCB_ATOM_NONE };

        INFO << "setting WM_STATE to " << state;

        xcb_change_property(g_xcb.connection, XCB_PROP_MODE_REPLACE,
                            m_window, g_xcb.WM_STATE.atom,
                            g_xcb.WM_STATE.atom, 32, 2, values);
    }

    //! Retrieve ICCCM WM_STATE property.
    uint32_t get_wm_state()
    {
        xcb_get_property_cookie_t gpc
            = xcb_get_property(g_xcb.connection, 0, m_window,
                               g_xcb.WM_STATE.atom, g_xcb.WM_STATE.atom, 0, 2);

        autofree_ptr<xcb_get_property_reply_t> gpr(
            xcb_get_property_reply(g_xcb.connection, gpc, NULL)
            );

        if (!gpr) {
            ERROR << "Could not retrieve WM_STATE for window " << m_window;
            return 0;
        }

        TRACE << *gpr;

        if (gpr->type != g_xcb.WM_STATE.atom ||
            gpr->format != 32 || gpr->length != 2)
        {
            ERROR << "Invalid answer to WM_STATE request for window "
                  << m_window;
            return 0;
        }

        uint32_t result = *((uint32_t*)xcb_get_property_value(gpr.get()));

        DEBUG << "WM_STATE of window " << m_window << " is " << result;

        return result;
    }

    //! Send a ICCCM WM_DELETE_WINDOW client message.
    void wm_delete_window()
    {
        xcb_client_message_event_t ev;

        ev.response_type = XCB_CLIENT_MESSAGE;
        ev.format = 32;
        ev.sequence = 0;
        ev.window = m_window;
        ev.type = g_xcb.WM_PROTOCOLS.atom;
        ev.data.data32[0] = g_xcb.WM_DELETE_WINDOW.atom;
        ev.data.data32[1] = XCB_CURRENT_TIME;
        ev.data.data32[2] = ev.data.data32[3] = ev.data.data32[4] = 0;

        TRACE << "Sending " << ev;

        xcb_send_event(g_xcb.connection, 0, m_window,
                       XCB_EVENT_MASK_NO_EVENT, (char*)&ev);
    }
};

#endif // !TILEWM_XCB_WINDOW_HEADER

/******************************************************************************/
