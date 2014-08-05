/******************************************************************************/
/*! \file unittests/test_geometry.cpp
 *
 * Test Point and Rectangle geometry classes.
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

#include "geometry.h"
#include "log.h"

void test_point()
{
    Point p1(10, 10), p2 = Point(5, 0);
    ASSERT(p1 + p2 == Point(15, 10));
}

void test_rectangle()
{
    Point p1(10, 10), p2(100, 100);

    Rectangle r1(p1, p2), r2(10, 20, 50, 50);

    ASSERT(r1.origin() == p1);
    ASSERT(r1.top_left() == p1);
    ASSERT(r1.top_right() == Point(100, 10));
    ASSERT(r1.buttom_left() == Point(10, 100));
    ASSERT(r1.buttom_right() == p2);

    ASSERT(r1.contains(20, 20));
    ASSERT(!r1.contains(Point(5, 20)));

    ASSERT(r1.intersects(r2));
    ASSERT(r2.intersects(r1));

    ASSERT(!r1.intersects(Rectangle(0, 50, 5, 5)));
    ASSERT(!r1.intersects(Rectangle(50, 0, 5, 5)));
    ASSERT(!r1.intersects(Rectangle(50, 110, 5, 5)));
    ASSERT(!r1.intersects(Rectangle(110, 50, 5, 5)));
}

int main()
{
    test_point();
    test_rectangle();
    return 0;
}

/******************************************************************************/
