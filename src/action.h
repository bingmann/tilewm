/*******************************************************************************
 * src/action.h
 *
 * Abstract Action class, KeyEvent and ButtonEvent and many simple actions.
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

#ifndef TILEWM_ACTION_HEADER
#define TILEWM_ACTION_HEADER

#include <memory>
#include <xcb/xcb.h>
#include "geometry.h"

//! Struct to abstract all parameters passed to keyboard handlers.
struct KeyEvent
{
protected:
    //! Pointer to client on which the key was pressed.
    class Client* m_client;

    //! Reference to currently active key press event.
    xcb_key_press_event_t& m_event;

public:
    //! Initializing constructor
    KeyEvent(class Client* client, xcb_key_press_event_t& event)
        : m_client(client), m_event(event)
    { }

    //! Return pointer to client of event (or NULL).
    class Client * client()
    {
        return m_client;
    }

    //! Return mouse point the event occurred, relative to window geometry.
    Point pos()
    {
        return Point(m_event.event_x, m_event.event_y);
    }

    //! Return mouse point the event occurred, relative to the root window.
    Point root_pos()
    {
        return Point(m_event.root_x, m_event.root_y);
    }
};

//! Struct to abstract all parameters passed to mouse button handlers.
struct ButtonEvent
{
protected:
    //! Pointer to client on which the button was pressed.
    class Client* m_client;

    //! Reference to currently active press event.
    xcb_button_press_event_t& m_event;

public:
    //! Initializing constructor
    ButtonEvent(class Client* client, xcb_button_press_event_t& event)
        : m_client(client), m_event(event)
    { }

    //! Return pointer to client of event (or NULL).
    class Client * client()
    {
        return m_client;
    }

    //! Return mouse point the event occurred, relative to window geometry.
    Point pos()
    {
        return Point(m_event.event_x, m_event.event_y);
    }

    //! Return mouse point the event occurred, relative to the root window.
    Point root_pos()
    {
        return Point(m_event.root_x, m_event.root_y);
    }
};

//! All keyboard binding handlers must have this type
typedef void (* key_handler_type)(KeyEvent&);

//! All mouse button binding handlers must have this type.
typedef void (* button_handler_type)(ButtonEvent&);

//! Base class of an action class, mainly used for virtual destruction.
class Action
{
public:
    //! Virtual destructor called when the binding is released.
    virtual ~Action();

    //! Virtual operator() stub called for keyboard events.
    virtual void operator () (KeyEvent&);

    //! Virtual operator() stub called for mouse button events.
    virtual void operator () (ButtonEvent&);
};

//! memory reference to optional Action class
typedef std::unique_ptr<Action> ActionPtr;

#endif // !TILEWM_ACTION_HEADER

/******************************************************************************/
