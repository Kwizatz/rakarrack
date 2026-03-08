find_package(Qt6 REQUIRED COMPONENTS Widgets)
qt_standard_project_setup()

add_executable(rakplay
    main.cpp
)

target_link_libraries(rakplay PRIVATE
    Qt6::Widgets
    PkgConfig::JACK
    PkgConfig::SNDFILE
)

target_compile_definitions(rakplay PRIVATE
    QT_NO_KEYWORDS
)

install(TARGETS rakplay
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
