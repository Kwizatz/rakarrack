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

project(Rakarrack VERSION 0.6.2)
enable_testing()
enable_language(CXX)
enable_language(C)

include(GNUInstallDirs)

if(NOT ENV{MSYSTEM_PREFIX} STREQUAL "")
  list(APPEND CMAKE_PREFIX_PATH "$ENV{MSYSTEM_PREFIX}")
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_BINARY_DIR}")

include(compiler)

set(VERSION "${PROJECT_VERSION}")
set(WEBSITE "rakarrack.sf.net")
set(PACKAGE "rakarrack")
set(HELPDIR ${CMAKE_INSTALL_FULL_DATADIR}/doc/${PACKAGE})
set(DATA_DIR ${CMAKE_INSTALL_FULL_DATADIR}/${PACKAGE})

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

configure_file(cmake/config.hpp.in src/config.hpp)

add_subdirectory(src)
add_subdirectory(extra)

# --- Install rules ---

# Data files
install(FILES
	data/Default.rkrb
	data/Extra.rkrb
	data/Extra1.rkrb
	data/1.wav data/2.wav data/3.wav data/4.wav data/5.wav
	data/6.wav data/7.wav data/8.wav data/9.wav
	data/1.rvb data/2.rvb data/3.rvb data/4.rvb data/5.rvb
	data/6.rvb data/7.rvb data/8.rvb data/9.rvb data/10.rvb
	data/1.dly data/2.dly data/3.dly data/4.dly data/5.dly
	data/6.dly data/7.dly data/8.dly data/9.dly data/10.dly data/11.dly
	data/bg.png data/bg1.png data/bg2.png data/bg3.png
	data/bg4.png data/bg5.png data/bg6.png data/bg_gray_furr.png
	DESTINATION ${CMAKE_INSTALL_DATADIR}/${PACKAGE}
)

# Desktop file
install(FILES data/rakarrack.desktop
	DESTINATION ${CMAKE_INSTALL_DATADIR}/applications
)

# Man page
install(FILES man/rakarrack.1
	DESTINATION ${CMAKE_INSTALL_MANDIR}/man1
)

# Icons
install(FILES
	icons/icono_rakarrack_128x128.png
	icons/icono_rakarrack_32x32.png
	icons/icono_rakarrack_64x64.png
	DESTINATION ${CMAKE_INSTALL_DATADIR}/pixmaps
)

# Documentation
install(FILES COPYING AUTHORS ChangeLog NEWS README
	DESTINATION ${CMAKE_INSTALL_DOCDIR}
)

# HTML help
install(FILES
	doc/help/help.html doc/help/aci.html doc/help/general.html
	doc/help/effects.html doc/help/midiconverter.html doc/help/tuner.html
	doc/help/midilearn.html doc/help/taptempo.html doc/help/metronome.html
	doc/help/hardware.html doc/help/extra.html doc/help/credits.html
	doc/help/presetlist.html doc/help/midiic.html doc/help/license.html
	DESTINATION ${CMAKE_INSTALL_DOCDIR}/html
)

# HTML help CSS
install(FILES doc/help/css/kde-default.css
	DESTINATION ${CMAKE_INSTALL_DOCDIR}/html/css
)

# HTML help images
file(GLOB HELP_IMAGES doc/help/imagenes/*.jpg doc/help/imagenes/*.png)
install(FILES ${HELP_IMAGES}
	DESTINATION ${CMAKE_INSTALL_DOCDIR}/html/imagenes
)
