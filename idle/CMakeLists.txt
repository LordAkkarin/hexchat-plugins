# Copyright 2015 Johannes Donath <johannesd@torchmind.com>
# and other copyright owners as documented in the project's IP log.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#       http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
cmake_minimum_required (VERSION 3.2)
project (hexchat-idle)

# Find DBUS
find_package (PkgConfig QUIET)
pkg_check_modules(GLIB REQUIRED glib-2.0)

# Make GCC shut up about deprecated conversions ...
add_definitions (-Wno-write-strings)

# Include headers and source files
include_directories (${HEXCHAT_INCLUDE_DIR} ${GLIB_INCLUDE_DIRS})
set (SOURCE_FILES HexchatIdlePlugin.cpp plugin.cpp)

# Link plugin binary
add_library (hexchat-idle SHARED ${SOURCE_FILES})

target_link_libraries(hexchat-idle ${GLIB_LIBRARIES})
set_target_properties(hexchat-idle PROPERTIES
        OUTPUT_NAME "idle"
)
