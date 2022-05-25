juce_add_gui_app(sst-jucegui-demo
        )
target_sources(sst-jucegui-demo PRIVATE SSTJuceGuiDemo.cpp)
target_link_libraries(sst-jucegui-demo PRIVATE
        juce::juce_gui_basics

        sst-jucegui)