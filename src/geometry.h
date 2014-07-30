/*******************************************************************************
 * src/geometry.h
 *
 * Some classes to make working with Rectangles more convenient.
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

#ifndef TILEWM_GEOMETRY_HEADER
#define TILEWM_GEOMETRY_HEADER

#include <sstream>

/*!
 * A class for a Point, with hopefully many helper functions in the future.
 */
struct Point
{
    //! The Point's coordinates.
    int16_t x, y;

    //! Default constructor leaves members uninitialized!
    Point()
    { }

    //! Construct a point from plain integers.
    Point(int16_t _x, int16_t _y)
        : x(_x), y(_y)
    { }

    //! Return addition of two point vectors
    Point operator + (const Point& p) const
    {
        return Point(x + p.x, y + p.y);
    }

    //! Return subtraction of two point vectors
    Point operator - (const Point& p) const
    {
        return Point(x - p.x, y - p.y);
    }

    //! Return as string "(x,y)"
    friend std::ostream& operator << (std::ostream& os, const Point& p)
    {
        return os << '(' << p.x << ',' << p.y << ')';
    }
};

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

    //! Return the rectangle's origin point
    Point origin() const
    {
        return Point(x, y);
    }

    //! Change the rectangle's origin point
    void set_origin(const Point& p)
    {
        x = p.x, y = p.y;
    }

    //! Test if a point (px,py) is the origin of this rectangle.
    bool is_origin(int16_t px, int16_t py) const
    {
        return (x == px) && (y == py);
    }

    //! Test if a point p is the origin of this rectangle.
    bool is_origin(const Point& p) const
    {
        return is_origin(p.x, p.y);
    }

    //! Test if a point (px,py) is contained in the rectangle.
    bool contains(int16_t px, int16_t py) const
    {
        return (x <= px && px < x + w) && (y <= py && py < y + h);
    }

    //! Test if a point p is contained in the rectangle.
    bool contains(const Point& p) const
    {
        return contains(p.x, p.y);
    }

    //! Return as string "pos X x Y size W x H"
    std::string str_pos_size() const
    {
        std::ostringstream oss;
        oss << "pos " << x << " x " << y << " size " << w << " x " << h;
        return oss.str();
    }
};

#endif // !TILEWM_GEOMETRY_HEADER

/******************************************************************************/
