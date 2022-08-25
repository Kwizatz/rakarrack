# Copyright (C) 2022 Rodrigo Jose Hernandez Cordoba
#
# Licensed under the Apache License, Version 2.0 (the "License"); you may not
# use this file except in compliance with the License. You may obtain a copy of
# the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
# License for the specific language governing permissions and limitations under
# the License.

cmake_minimum_required(VERSION 3.12.0)
if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
  cmake_policy(SET CMP0072 NEW)
  if(POLICY CMP0020)
    cmake_policy(SET CMP0020 NEW)
  endif(POLICY CMP0020)
  if(POLICY CMP0053)
    cmake_policy(SET CMP0053 NEW)
  endif(POLICY CMP0053)
endif(COMMAND cmake_policy)

project(Rakarrack)
enable_testing()
enable_language(CXX)
enable_language(C)

if(NOT ENV{MSYSTEM_PREFIX} STREQUAL "")
  list(APPEND CMAKE_PREFIX_PATH "$ENV{MSYSTEM_PREFIX}")
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_BINARY_DIR}")

include(compiler)

set(VERSION "0.6.2")
set(WEBSITE "rakarrack.sf.net")
set(PACKAGE "rakarrack")
set(HELPDIR ${CMAKE_INSTALL_PREFIX}/share/doc/${PACKAGE})
set(DATA_DIR ${CMAKE_INSTALL_PREFIX}/share/${PACKAGE})

find_path (RAPIDJSON_INCLUDE_PATH rapidjson/rapidjson.h include)

find_package(FLTK)
option(ENABLE_MIDI "Enable MIDI code (requires ALSA)" ON)
if(ENABLE_MIDI)
  find_package(ALSA)
  if(NOT ALSA_FOUND)
    set(ENABLE_MIDI OFF)
  endif()
endif()
find_package(PkgConfig REQUIRED)
pkg_search_module(FFTW REQUIRED fftw3 IMPORTED_TARGET)
pkg_search_module(XPM REQUIRED xpm IMPORTED_TARGET)
pkg_search_module(JACK REQUIRED jack IMPORTED_TARGET)
pkg_search_module(SAMPLERATE REQUIRED samplerate IMPORTED_TARGET)
pkg_search_module(SNDFILE REQUIRED sndfile IMPORTED_TARGET)

configure_file(cmake/config.h.in src/config.h)

add_subdirectory(src)
