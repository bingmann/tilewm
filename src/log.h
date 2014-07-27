/*******************************************************************************
 * src/log.h
 *
 * A set of C++ ostream-compatible logging functions.
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

#ifndef TILEWM_LOG_HEADER
#define TILEWM_LOG_HEADER

#include <sstream>
#include <iostream>

// *****************************************************************************
// *** A set of C++ ostream-compatible logging functions.

//! Log levels available.
enum log_level_t {
    LOG_FATAL, LOG_ERROR, LOG_WARN, LOG_INFO, LOG_DEBUG,
    LOG_TRACE, LOG_TRACE1, LOG_TRACE2, LOG_TRACE3,
    LOG_MAX
};

/*!
 * Logging helper class: an object of this Log class is created for each
 * generated line. On destruction, the collected log line is outputted into the
 * currently configured log sinks.
 */
class Log
{
protected:
    //! The log information is collected into the string ostream.
    std::ostringstream m_os;

    //! Level of this log line
    log_level_t m_level;

    //! Currently configured maximum level written to stderr
    static log_level_t s_stderr_level;

    //! Whether to use ANSI terminal color on stderr
    static bool s_stderr_color;

private:
    Log(const Log&);              //!< non-copyable
    Log& operator = (const Log&); //!< non-copyable

public:
    //! Start constructing a new log line.
    Log(log_level_t level, const char* function, unsigned int line)
        : m_level(level)
    {
        m_os << function << ':' << line << ' ' << level_string(level) << " - ";
    }

    //! Return the ostreamstring to the user to append more information.
    std::ostringstream & get()
    {
        return m_os;
    }

    //! On destruction of the object: output the line to all log sinks.
    ~Log()
    {
        if (m_level <= s_stderr_level)
        {
            if (s_stderr_color)
                std::cerr << ansi_color(m_level);

            std::cerr << m_os.str();

            if (s_stderr_color)
                std::cerr << "\x1B[0m";

            std::cerr << std::endl;
        }

        // TODO: make circular log buffer
    }

    //! Get the current logging level written to stderr.
    static log_level_t get_stderr_level()
    {
        return s_stderr_level;
    }

    //! Change the current logging level written to stderr.
    static void set_stderr_level(log_level_t level)
    {
        s_stderr_level = level;
    }

    //! Change the current logging level written to stderr.
    static bool set_stderr_level(const char* str);

    //! Return the log level as a string.
    static const char * level_string(log_level_t level)
    {
        static const char* const label[LOG_MAX] = {
            "FATAL", "ERROR", "WARN", "INFO", "DEBUG",
            "TRACE", "TRACE1", "TRACE2", "TRACE3"
        };
        return label[level];
    }

    //! Return ANSI color escape sequence for log level.
    static const char * ansi_color(log_level_t level)
    {
        static const char* const color[LOG_MAX] = {
            // FATAL ERROR WARN
            "\x1B[1;31m", "\x1B[1;31m", "\x1B[1;33m",
            // INFO DEBUG
            "\x1B[1;32m", "",
            // TRACE TRACE1 TRACE2 TRACE3
            "\x1B[1;30m", "\x1B[1;30m", "\x1B[1;30m", "\x1B[1;30m"
        };
        return color[level];
    }
};

#define LOG(level, file, line)   Log(level, file, line).get()

// *** Logger Interfaces : INFO << text << var;

#define FATAL           LOG(LOG_FATAL, __func__, __LINE__)
#define ERROR           LOG(LOG_ERROR, __func__, __LINE__)
#define WARN            LOG(LOG_WARN, __func__, __LINE__)
#define INFO            LOG(LOG_INFO, __func__, __LINE__)
#define DEBUG           LOG(LOG_DEBUG, __func__, __LINE__)
#define TRACE           LOG(LOG_TRACE, __func__, __LINE__)
#define TRACE1          LOG(LOG_TRACE1, __func__, __LINE__)
#define TRACE2          LOG(LOG_TRACE2, __func__, __LINE__)
#define TRACE3          LOG(LOG_TRACE3, __func__, __LINE__)

// *****************************************************************************
// *** An always-on ASSERT

//! Fully verbose always-on ASSERT.
#define ASSERT(expr)                                             \
    do {                                                         \
        if (!(expr)) {                                           \
            std::cerr << __func__ << ':' << __LINE__ << ' '      \
                      << ": Assertion '" << #expr << "' failed!" \
                      << std::endl;                              \
            abort();                                             \
        }                                                        \
    } while (0)

#endif // !TILEWM_LOG_HEADER

/******************************************************************************/
