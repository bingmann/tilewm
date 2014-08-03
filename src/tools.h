/******************************************************************************/
/*! \file src/tools.h
 *
 * Various generic helper functions and tools.
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

#ifndef TILEWM_TOOLS_HEADER
#define TILEWM_TOOLS_HEADER

#include <sstream>
#include <string>

/*!
 * Template transformation function which uses std::ostringstream to serialize
 * any ostreamable type into a std::string.
 */
template <typename Type>
static inline std::string to_str(const Type& val)
{
    std::ostringstream os;
    os << val;
    return os.str();
}

/*!
 * Template transformation function which uses std::istringstream to parse any
 * istreamable type from a std::string.
 */
template <typename Type>
static inline bool from_str(const std::string& str, Type& outval)
{
    std::istringstream is(str);
    is >> outval;
    return is.eof();
}

/*!
 * Dump a (binary) memory area as a sequence of hexadecimal pairs.
 *
 * @param data  memory area to output in hex
 * @param size  size of memory area
 * @return      string of hexadecimal pairs
 */
static inline std::string string_hexdump(const void* data, size_t size)
{
    std::string out;
    out.resize(size * 2);

    unsigned char* cdata = (unsigned char*)data;

    static const char xdigits[16] = {
        '0', '1', '2', '3', '4', '5', '6', '7',
        '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
    };

    std::string::iterator oi = out.begin();
    for (size_t i = 0; i < size; ++i)
    {
        *oi++ = xdigits[(cdata[i] & 0xF0) >> 4];
        *oi++ = xdigits[(cdata[i] & 0x0F) >> 0];
    }

    return out;
}

/*!
 * Dump a (binary) string as a sequence of hexadecimal pairs.
 *
 * @param str   string to output in hex
 * @return      string of hexadecimal pairs
 */
static inline std::string string_hexdump(const std::string& str)
{
    return string_hexdump(str.data(), str.size());
}

#endif // !TILEWM_TOOLS_HEADER

/******************************************************************************/
