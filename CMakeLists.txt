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
project (hexchat-plugins)

# Find the Hexchat include directory (if any)
find_path (HEXCHAT_INCLUDE_DIR NAMES hexchat-plugin.h)
mark_as_advanced (HEXCHAT_INCLUDE_DIR)

# Customize library prefix
set (CMAKE_SHARED_LIBRARY_PREFIX "hexchat-")

# Enable C++11 features
add_definitions (-std=c++11)

# Add submodules
add_subdirectory (idle)
