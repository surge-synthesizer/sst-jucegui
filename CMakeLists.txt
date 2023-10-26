cmake_minimum_required(VERSION 3.10)
project(sst-jucegui VERSION 0.5 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

option(SST_JUCEGUI_BUILD_EXAMPLES "Add targets for building and running sst-filters examples" FALSE)

if (${SST_JUCEGUI_BUILD_EXAMPLES})
    message(STATUS "Including SST JUCEGUI Examples")

    if (NOT TARGET juce::juce_gui_basics)
        message(STATUS "Downloading JUCE")
        set(FETCHCONTENT_QUIET FALSE)
        include(cmake/CPM.cmake)
        CPMAddPackage("gh:juce-framework/JUCE#7.0.3")

        add_library(JUCE INTERFACE)
    endif ()
    add_subdirectory(examples)
endif ()

if (NOT TARGET juce::juce_gui_basics)
    message(WARNING "juce::juce_gui_basics not found. If you are building top level")
    message(WARNING "rebuild with -DSST_JUCEGUI_BUILD_EXAMPLES=TRUE")
    message(FATAL_ERROR "halting compilation")
endif ()

add_library(${PROJECT_NAME} STATIC
        src/sst/jucegui/components/ComponentBase.cpp
        src/sst/jucegui/components/ContinuousParamEditor.cpp
        src/sst/jucegui/components/DraggableTextEditableValue.cpp
        src/sst/jucegui/components/GlyphButton.cpp
        src/sst/jucegui/components/GlyphPainter.cpp
        src/sst/jucegui/components/HSlider.cpp
        src/sst/jucegui/components/HSliderFilled.cpp
        src/sst/jucegui/components/Knob.cpp
        src/sst/jucegui/components/MenuButton.cpp
        src/sst/jucegui/components/MultiSwitch.cpp
        src/sst/jucegui/components/NamedPanel.cpp
        src/sst/jucegui/components/NamedPanelDivider.cpp
        src/sst/jucegui/components/SevenSegmentControl.cpp
        src/sst/jucegui/components/TabularizedTreeViewer.cpp
        src/sst/jucegui/components/TextPushButton.cpp
        src/sst/jucegui/components/ToggleButton.cpp
        src/sst/jucegui/components/ToggleButtonRadioGroup.cpp
        src/sst/jucegui/components/ToolTip.cpp
        src/sst/jucegui/components/VSlider.cpp

        src/sst/jucegui/data/TreeTable.cpp

        src/sst/jucegui/style/StyleAndSettingsConsumer.cpp
        src/sst/jucegui/style/StyleSheet.cpp
        )
target_include_directories(${PROJECT_NAME} PUBLIC include)
target_link_libraries(${PROJECT_NAME} PUBLIC juce::juce_gui_basics)
target_compile_definitions(${PROJECT_NAME} PRIVATE
        JUCE_USE_CURL=0
        JUCE_WEB_BROWSER=0
        JUCE_JACK=0
        JUCE_ALSA=0
        JUCE_WASAPI=0
        JUCE_STANDALONE_APPLICATION=0
        JUCE_DIRECTSOUND=0)

set_property(TARGET ${PROJECT_NAME} PROPERTY C_VISIBILITY_PRESET hidden)
set_property(TARGET ${PROJECT_NAME} PROPERTY VISIBILITY_INLINES_HIDDEN ON)

