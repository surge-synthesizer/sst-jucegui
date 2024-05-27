juce_add_gui_app(sst-jucegui-component-demo)
target_sources(sst-jucegui-component-demo PRIVATE SSTJuceGuiDemo.cpp)
target_compile_definitions(sst-jucegui-component-demo PUBLIC
        JUCE_USE_CURL=0
        JUCE_WEB_BROWSER=0
        JUCE_JACK=0
        JUCE_ALSA=0
        JUCE_WASAPI=0
        JUCE_DIRECTSOUND=0
        )
target_link_libraries(sst-jucegui-component-demo PRIVATE
        juce::juce_gui_basics
        sst-jucegui)

# Compiler specific choices
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
   target_compile_options(sst-jucegui-component-demo PRIVATE
        $<$<COMPILE_LANGUAGE:CXX>:-fno-char8_t>
    )

endif()

if (MSVC)
    target_compile_options(sst-jucegui-component-demo PRIVATE
        # Set source and executable charsets to UTF-8
        $<$<CXX_COMPILER_ID:MSVC>:/utf-8>
        # Do *not* use the new, breaking char8_t UTF-8 bits in C++20.
        $<$<COMPILE_LANGUAGE:CXX>:/Zc:char8_t->
        # make msvc define __cplulsplus properly
        $<$<COMPILE_LANGUAGE:CXX>:/Zc:__cplusplus>
    )
endif()
