find_program(BASH_EXECUTABLE bash HINTS ENV MSYSTEM_PREFIX)
find_program(ZSH_EXECUTABLE zsh HINTS ENV MSYSTEM_PREFIX)

if(Qt6_FOUND)
    list(APPEND INCLIST ${Qt6Core_INCLUDE_DIRS})
    list(APPEND INCLIST ${Qt6Gui_INCLUDE_DIRS})
    list(APPEND INCLIST ${Qt6Widgets_INCLUDE_DIRS})
endif(Qt6_FOUND)

list(REMOVE_DUPLICATES INCLIST)
  set(INCLIST "\"${INCLIST}\"")
  string(REPLACE ";"
                 "\",\n\""
                 INCLIST
                 "${INCLIST}")
  configure_file("${CMAKE_SOURCE_DIR}/cmake/c_cpp_properties.json.in"
                 "${CMAKE_SOURCE_DIR}/.vscode/c_cpp_properties.json" @ONLY)
  configure_file("${CMAKE_SOURCE_DIR}/cmake/settings.json.in"
                 "${CMAKE_SOURCE_DIR}/.vscode/settings.json" @ONLY)
