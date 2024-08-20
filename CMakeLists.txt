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

include(cmake/CmakeRC.cmake)

cmrc_add_resource_library(sst-jucegui-resources NAMESPACE sst::jucegui::resources
        res/glyphs/add-mod.svg
        res/glyphs/arrow-ltor.svg
        res/glyphs/close.svg
        res/glyphs/curve.svg
        res/glyphs/down.svg
        res/glyphs/edit.svg
        res/glyphs/ellipsis-h.svg
        res/glyphs/ellipsis-v.svg
        res/glyphs/favorite.svg
        res/glyphs/forward-backward.svg
        res/glyphs/freeze.svg
        res/glyphs/left-right.svg
        res/glyphs/left.svg
        res/glyphs/link.svg
        res/glyphs/lock.svg
        res/glyphs/memory.svg
        res/glyphs/metronome.svg
        res/glyphs/midi.svg
        res/glyphs/mul-mod.svg
        res/glyphs/note-priority.svg
        res/glyphs/pan.svg
        res/glyphs/pin.svg
        res/glyphs/plus-minus.svg
        res/glyphs/plus.svg
        res/glyphs/polyphony.svg
        res/glyphs/portamento.svg
        res/glyphs/power.svg
        res/glyphs/reverse.svg
        res/glyphs/right.svg
        res/glyphs/routing-post-fader.svg
        res/glyphs/routing-pre-fader.svg
        res/glyphs/routing-pre-fx.svg
        res/glyphs/save.svg
        res/glyphs/search.svg
        res/glyphs/settings.svg
        res/glyphs/shortcircuit-logo.svg
        res/glyphs/speaker.svg
        res/glyphs/step-count.svg
        res/glyphs/surge-logo.svg
        res/glyphs/tuning.svg
        res/glyphs/unpin.svg
        res/glyphs/up-down.svg
        res/glyphs/up.svg
        res/glyphs/volume.svg
)
set_target_properties(sst-jucegui-resources PROPERTIES UNITY_BUILD FALSE)

if (${CMAKE_UNITY_BUILD})
    add_library(sst-jucegui-juce-requirements STATIC)
    target_link_libraries(sst-jucegui-juce-requirements PRIVATE juce::juce_gui_basics)
    get_target_property(incl juce::juce_gui_basics INTERFACE_INCLUDE_DIRECTORIES)
    get_target_property(defn juce::juce_gui_basics INTERFACE_COMPILE_DEFINITIONS)


    target_include_directories(sst-jucegui-juce-requirements PUBLIC ${incl})
    target_compile_definitions(sst-jucegui-juce-requirements PUBLIC
            ${defn}
            JUCE_GLOBAL_MODULE_SETTINGS_INCLUDED=1
            $<IF:$<CONFIG:Debug>,DEBUG=1,NDEBUG=1>
            $<IF:$<CONFIG:Debug>,JUCE_DEBUG=1,JUCE_DEBUG=0>

            JUCE_USE_CURL=0
            JUCE_WEB_BROWSER=0
            JUCE_JACK=0
            JUCE_ALSA=0
            JUCE_WASAPI=0
            JUCE_STANDALONE_APPLICATION=0
            JUCE_DIRECTSOUND=0

    )

    set_target_properties(sst-jucegui-juce-requirements PROPERTIES UNITY_BUILD FALSE)
else()
    add_library(sst-jucegui-juce-requirements INTERFACE)
    target_link_libraries(sst-jucegui-juce-requirements INTERFACE juce::juce_gui_basics)
    target_compile_definitions(sst-jucegui-juce-requirements INTERFACE
            JUCE_USE_CURL=0
            JUCE_WEB_BROWSER=0
            JUCE_JACK=0
            JUCE_ALSA=0
            JUCE_WASAPI=0
            JUCE_STANDALONE_APPLICATION=0
            JUCE_DIRECTSOUND=0)
endif()

add_library(${PROJECT_NAME} STATIC
        src/sst/jucegui/components/ComponentBase.cpp
        src/sst/jucegui/components/ContinuousParamEditor.cpp
        src/sst/jucegui/components/DiscreteParamMenuBuilder.cpp
        src/sst/jucegui/components/DraggableTextEditableValue.cpp
        src/sst/jucegui/components/GlyphButton.cpp
        src/sst/jucegui/components/GlyphPainter.cpp
        src/sst/jucegui/components/HSlider.cpp
        src/sst/jucegui/components/HSliderFilled.cpp
        src/sst/jucegui/components/JogUpDownButton.cpp
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
target_link_libraries(${PROJECT_NAME} PUBLIC
        sst-jucegui-juce-requirements
        sst-jucegui-resources)


set_property(TARGET ${PROJECT_NAME} PROPERTY C_VISIBILITY_PRESET hidden)
set_property(TARGET ${PROJECT_NAME} PROPERTY VISIBILITY_INLINES_HIDDEN ON)

if (APPLE)
    target_compile_definitions(${PROJECT_NAME} PRIVATE JUCEGUI_MAC=1)
elseif (WIN32)
    target_compile_definitions(${PROJECT_NAME} PRIVATE JUCEGUI_WIN=1)
else()
    target_compile_definitions(${PROJECT_NAME} PRIVATE JUCEGUI_LIN=1)
endif()
