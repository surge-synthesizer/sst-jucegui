juce_add_gui_app(scxt-wireframes-demo)
target_sources(scxt-wireframes-demo PRIVATE SCXTWireframesMain.cpp)
target_compile_definitions(scxt-wireframes-demo PUBLIC
        JUCE_USE_CURL=0
        JUCE_WEB_BROWSER=0
        JUCE_JACK=0
        JUCE_ALSA=0
        JUCE_WASAPI=0
        JUCE_DIRECTSOUND=0
        )
target_link_libraries(scxt-wireframes-demo PRIVATE
        sst-jucegui)