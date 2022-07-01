# Multiplatform Command Line MIDI Monitor program using Drumstick::RT
# Copyright (C) 2022, Pedro Lopez-Cabanillas <plcl@users.sf.net>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.

cmake_minimum_required(VERSION 3.14)
project(dumpmidi-rt VERSION 1.0 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt5 Qt6)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core REQUIRED)
find_package(Drumstick COMPONENTS RT REQUIRED)

add_executable(dumpmidi-rt
  dumpmidi.cpp
)

target_compile_definitions( dumpmidi-rt
    PRIVATE VERSION=${PROJECT_VERSION}
)

target_link_libraries(dumpmidi-rt
    Qt${QT_VERSION_MAJOR}::Core
    Drumstick::RT
)
