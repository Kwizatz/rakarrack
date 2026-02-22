# Copyright (C) 2022 Rodrigo Jose Hernandez Cordoba
#
# Licensed under the Apache License, Version 2.0 (the "License"); you may not
# use this file except in compliance with the License. You may obtain a copy of
# the License at
#
# http://www.apache.org/licenses/LICENSE-2.0

find_package(Qt6 REQUIRED COMPONENTS Widgets)
qt_standard_project_setup()

set(QT6_GUI_SOURCES
    main.cpp
    MainWindow.cpp
    widgets/MidiSlider.cpp
    widgets/VUMeter.cpp
    widgets/SpectrumAnalyzer.cpp
    widgets/Oscilloscope.cpp
    widgets/TunerDisplay.cpp
)

set(QT6_GUI_HEADERS
    MainWindow.hpp
    widgets/MidiSlider.hpp
    widgets/VUMeter.hpp
    widgets/SpectrumAnalyzer.hpp
    widgets/Oscilloscope.hpp
    widgets/TunerDisplay.hpp
)

add_executable(rakarrack-qt6
    ${QT6_GUI_SOURCES}
    ${QT6_GUI_HEADERS}
)

target_link_libraries(rakarrack-qt6 PRIVATE
    rakarrack_engine
    Qt6::Widgets
    ${ALSA_LIBRARIES}
    PkgConfig::FFTW
    PkgConfig::XPM
    PkgConfig::JACK
    PkgConfig::SAMPLERATE
    PkgConfig::SNDFILE
    nlohmann_json::nlohmann_json
)

target_include_directories(rakarrack-qt6 PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_BINARY_DIR}/src
    ${ALSA_INCLUDE_DIRS}
)

install(TARGETS rakarrack-qt6 RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
