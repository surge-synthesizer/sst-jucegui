juce_add_gui_app(sst-jucegui-demo)
target_sources(sst-jucegui-demo PRIVATE SSTJuceGuiDemo.cpp)
target_compile_definitions(sst-jucegui-demo PUBLIC
        JUCE_USE_CURL=0
        JUCE_WEB_BROWSER=0
        JUCE_JACK=0
        JUCE_ALSA=0
        JUCE_WASAPI=0
        JUCE_DIRECTSOUND=0
        )
target_link_libraries(sst-jucegui-demo PRIVATE
        juce::juce_gui_basics
        sst-jucegui)