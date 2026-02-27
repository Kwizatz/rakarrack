if(CMAKE_VERSION VERSION_LESS "3.12.4")
  add_definitions(-D_USE_MATH_DEFINES)
else()
  add_compile_definitions(_USE_MATH_DEFINES)
endif()

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to 'Debug' as none was specified.")
  set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE
               PROPERTY STRINGS
                        "Debug"
                        "Release"
                        "MinSizeRel"
                        "RelWithDebInfo")
endif()

# Use C++23 as the base standard (universally supported by CMake).
# Compilers that support C++26 features get the flag added manually below.
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  # GCC 14+ and Clang 17+ support -std=c++2c
  add_compile_options(-std=c++2c)
  option(USE_ASAN "Instrument the build to use ASAN sanitizers" OFF)
  option(PROFILING "Instrument the build to generate profiling binaries" OFF)
  if(USE_ASAN)
    # -fsanitize=leak
    set(ASAN_SANITIZERS "-fsanitize=undefined -fsanitize=null -fsanitize=return -fsanitize=address -fsanitize=vptr -fsanitize-address-use-after-scope")
  endif()
  if(PROFILING)
    set(PROFILING_FLAGS "-pg")
  endif()
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DNOMINMAX -D_FORTIFY_SOURCE=2 -Wall -Wextra -Werror -Wfatal-errors -Wpsabi -Og -ggdb ${ASAN_SANITIZERS} ${PROFILING_FLAGS}")
  set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} ${ASAN_SANITIZERS} ${PROFILING_FLAGS}")
  set(CMAKE_CXX_FLAGS_RELEASE
      "${CMAKE_CXX_FLAGS_RELEASE} -DNOMINMAX -D_FORTIFY_SOURCE=2 -Wall -Wextra -Werror -Wfatal-errors -Wpsabi -O3 -fomit-frame-pointer ${PROFILING_FLAGS}")
  set(CMAKE_EXE_LINKER_FLAGS_RELEASE
      "${CMAKE_EXE_LINKER_FLAGS_RELEASE} ${PROFILING_FLAGS}")
      set(PREPROCESS_ONLY_FLAGS -x c++ -E)
      set(PREPROCESS_OUTPUT_FLAG -o)
      set(PREPROCESS_VA_OPT "__VA_OPT__(,)")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  # MSVC: /std:c++latest enables the latest C++ features
  add_compile_options(/std:c++latest)
endif()

option(USE_CPPCHECK "Use cppcheck static code analisys" OFF)
find_program(CPPCHECK_PROGRAM NAMES cppcheck)
if(CPPCHECK_PROGRAM AND USE_CPPCHECK)
  set(CMAKE_CXX_CPPCHECK ${CPPCHECK_PROGRAM} --quiet --suppress=unknownMacro)
endif()

if(NOT ENV{MSYSTEM_PREFIX} STREQUAL "")
  set(PkgConfig_ROOT $ENV{MSYSTEM_PREFIX})
endif()
find_package(PkgConfig)
if(PKG_CONFIG_FOUND AND BASH_EXECUTABLE AND MSYS)
  # In order to properly use pkg-config in a msys2 environment we need to jump
  # through some hoops. CMake seems to call pkg-config without any of the bash
  # environment set up, so we need to set the proper pkg config path inside the
  # cmake process address space.
  execute_process(COMMAND ${BASH_EXECUTABLE} --login -c "echo $PKG_CONFIG_PATH"
                  OUTPUT_VARIABLE PKG_CONFIG_PATH)
  set(ENV{PKG_CONFIG_PATH} ${PKG_CONFIG_PATH})
endif()
