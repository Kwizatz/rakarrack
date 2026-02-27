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

# --- Static libraries for conversion routines ---

add_library(rakconvert_lib STATIC rakconvert_lib.cpp rakconvert_lib.hpp)
target_include_directories(rakconvert_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_library(rakverb_lib STATIC rakverb_lib.cpp rakverb_lib.hpp)
target_include_directories(rakverb_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(rakverb_lib PUBLIC PkgConfig::SNDFILE)

# --- Standalone command-line tools (POSIX only, require getopt.h) ---

if(NOT MSVC)

add_executable(rakconvert rakconvert.cpp)
target_link_libraries(rakconvert PRIVATE rakconvert_lib)

add_executable(rakverb rakverb.cpp)
target_link_libraries(rakverb PRIVATE rakverb_lib)

add_executable(rakverb2 rakverb2.cpp)
target_link_libraries(rakverb2 PkgConfig::SNDFILE)

add_executable(rakgit2new rakgit2new.cpp)

  install(TARGETS rakconvert rakverb rakverb2 rakgit2new
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  )
endif()
