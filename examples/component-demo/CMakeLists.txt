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
if(MSVC)
    target_compile_options(sst-jucegui-component-demo PUBLIC
        /Zc:__cplusplus /Zc:char8_t-)
else()
    target_compile_options(sst-jucegui-component-demo PUBLIC -fno-char8_t)
endif()
target_link_libraries(sst-jucegui-component-demo PRIVATE
        sst-jucegui)
