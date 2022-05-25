cmake_minimum_required(VERSION 3.10)
project(sst-jucegui VERSION 0.5 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)

include(cmake/CPM.cmake)


get_directory_property(PARENT_DIR PARENT_DIRECTORY)
if ("${PARENT_DIR}" STREQUAL "")
    set(is_toplevel 1)
    CPMAddPackage("gh:juce-framework/JUCE#6.1.6")
else ()
    set(is_toplevel 0)
endif ()

add_library(${PROJECT_NAME} 
  src/sst/jucegui/components/Knob.cpp
  src/sst/jucegui/components/NamedPanel.cpp

  src/sst/jucegui/style/StyleConsumer.cpp
  src/sst/jucegui/style/StyleSheet.cpp
)
target_include_directories(${PROJECT_NAME} PUBLIC include)
target_link_libraries(${PROJECT_NAME} PUBLIC juce::juce_gui_basics)

option(SST_JUCEGUI_BUILD_EXAMPLES "Add targets for building and running sst-filters examples" ${is_toplevel})
if (SST_JUCEGUI_BUILD_EXAMPLES)
    add_library(JUCE INTERFACE)
    add_subdirectory(examples)
endif()
