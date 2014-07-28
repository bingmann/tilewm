################################################################################
# src/CMakeLists.txt
#
# Copyright (C) 2014 Timo Bingmann <tb@panthema.net>
#
# This program is free software: you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free Software
# Foundation, either version 3 of the License, or (at your option) any later
# version.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
# details.
#
# You should have received a copy of the GNU General Public License along with
# this program.  If not, see <http://www.gnu.org/licenses/>.
################################################################################

include_directories(${XCB_INCLUDE_DIRS})

add_executable(tilewm
  main.cpp
  log.cpp
  xcb.cpp
  xcb-ostream.cpp
  xcb-atom.cpp
  event.cpp
  screen.cpp
  client.cpp
  )

if(PERL_FOUND)
  # auto generate source files using perl scripts
  add_custom_command(
    OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/xcb-ostream.cpp
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMAND perl xcb-ostream-gen.pl > xcb-ostream.cpp
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/xcb-ostream-gen.pl
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/xcb.h
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/screen.cpp
    )

  add_custom_command(
    OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/xcb-atom.cpp
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMAND perl xcb-atom-gen.pl > xcb-atom.cpp
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/xcb-atom-gen.pl
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/xcb.h
    )
endif()

target_link_libraries(tilewm ${XCB_LIBRARIES})
