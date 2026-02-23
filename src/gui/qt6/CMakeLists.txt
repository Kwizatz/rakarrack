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
    ThemeManager.cpp
    SystemTray.cpp
    TopBar.cpp
    EffectSlotBar.cpp
    panels/EffectPanel.cpp
    panels/SliderPanel.cpp
    panels/EQPanel.cpp
    panels/DynamicsPanels.cpp
    panels/DistortionPanels.cpp
    panels/ModulationPanels.cpp
    panels/DelayReverbPanels.cpp
    panels/PitchPanels.cpp
    panels/FilterPanels.cpp
    panels/MiscPanels.cpp
    dialogs/BankDialog.cpp
    dialogs/OrderDialog.cpp
    dialogs/SettingsDialog.cpp
    dialogs/MidiLearnDialog.cpp
    dialogs/AboutDialog.cpp
    dialogs/HelpBrowser.cpp
    dialogs/TriggerDialog.cpp
    widgets/MidiSlider.cpp
    widgets/VUMeter.cpp
    widgets/SpectrumAnalyzer.cpp
    widgets/Oscilloscope.cpp
    widgets/TunerDisplay.cpp
)

set(QT6_GUI_HEADERS
    MainWindow.hpp
    ThemeManager.hpp
    SystemTray.hpp
    TopBar.hpp
    EffectSlotBar.hpp
    panels/EffectPanel.hpp
    panels/ParamDesc.hpp
    panels/SliderPanel.hpp
    panels/EQPanel.hpp
    panels/DynamicsPanels.hpp
    panels/DistortionPanels.hpp
    panels/ModulationPanels.hpp
    panels/DelayReverbPanels.hpp
    panels/PitchPanels.hpp
    panels/FilterPanels.hpp
    panels/MiscPanels.hpp
    dialogs/BankDialog.hpp
    dialogs/OrderDialog.hpp
    dialogs/SettingsDialog.hpp
    dialogs/MidiLearnDialog.hpp
    dialogs/AboutDialog.hpp
    dialogs/HelpBrowser.hpp
    dialogs/TriggerDialog.hpp
    widgets/MidiSlider.hpp
    widgets/VUMeter.hpp
    widgets/SpectrumAnalyzer.hpp
    widgets/Oscilloscope.hpp
    widgets/TunerDisplay.hpp
)

set(QT6_RESOURCES
    resources.qrc
)

add_executable(rakarrack-qt6
    ${QT6_GUI_SOURCES}
    ${QT6_GUI_HEADERS}
    ${QT6_RESOURCES}
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
