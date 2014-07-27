/*******************************************************************************
 * src/screen.h
 *
 * Detect screens (usually monitors) via RandR and Xinerama.
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

#ifndef TILEWM_SCREEN_HEADER
#define TILEWM_SCREEN_HEADER

#include <vector>
#include <string>
#include <memory>
#include <sstream>

/*!
 * A class for a Rectangle, with hopefully many helper functions in the future.
 */
struct Rectangle
{
    //! top left position
    int16_t x, y;

    //! width and height
    uint16_t w, h;

    //! Default constructor leaves members uninitialized!
    Rectangle()
    { }

    //! Construct a rectangle from plain integers.
    Rectangle(int16_t _x, int16_t _y, uint16_t _w, uint16_t _h)
        : x(_x), y(_y), w(_w), h(_h)
    { }

    //! Test if a point (px,py) is the origin of this rectangle.
    bool is_origin(int16_t px, int16_t py) const
    {
        return (x == px) && (y == py);
    }

    //! Test if a point (px,py) is contained in the rectangle.
    bool contains(int16_t px, int16_t py) const
    {
        return (x <= px && px < x + w) && (y <= py && py < y + h);
    }

    //! Return as string "pos X x Y size W x H"
    std::string str_pos_size() const
    {
        std::ostringstream oss;
        oss << "pos " << x << " x " << y << " size " << w << " x " << h;
        return oss.str();
    }
};

//! Save Screen detection mechanism
enum screen_type_t {
    SCREEN_DEFAULT, SCREEN_XINERAMA, SCREEN_RANDR
};

/*!
 * A Screen represents a managed virtual screen area (usually a monitor) onto
 * which windows will be mapped. Each output detected via RandR or Xinerama is
 * associated with a Screen.
 */
class Screen
{
public:
    //! Area of the big virtual X screen occupied by this screen
    Rectangle geometry;

    //! Active flag: whether the output is still attached
    bool active;

    //! Source of information about this screen
    screen_type_t type;

    //! User-readable name of screen
    std::string name;
};

//! Unique pointer to enable explicit memory ownership management.
typedef std::unique_ptr<Screen> ScreenPtr;

/*!
 * The ScreenList contains all active (and some detached) Screen objects.
 */
class ScreenList
{
protected:
    //! The main screen list of detected screens.
    static std::vector<ScreenPtr> m_list;

public:
    //! Detect new screens via Xinerama.
    static bool detect_xinerama();

    //! Detect new screens (and deactivate old) via RandR.
    static bool detect_randr();

    //! Set up a detect screen spanning the whole virtual screen.
    static void detect_default();

    //! Run initial screen detection: RandR, Xinerama and then default.
    static void detect();

public:
    //! Find screen with origin at point (px,py).
    static Screen * find_screen_at(int16_t px, int16_t py)
    {
        for (ScreenPtr& s : m_list)
        {
            if (s->geometry.is_origin(px, py)) return s.get();
        }
        return NULL;
    }
};

#endif // !TILEWM_SCREEN_HEADER

/******************************************************************************/
