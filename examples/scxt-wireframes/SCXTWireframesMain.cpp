//
// Created by Paul Walker on 12/25/22.
//

#include <juce_gui_basics/juce_gui_basics.h>

#include "sst/jucegui/style/StyleSheet.h"

struct SCXTWireframesMain : public juce::JUCEApplication
{
    const juce::String getApplicationName() override { return "SCXTWireframesMain"; }
    const juce::String getApplicationVersion() override { return "0.5.0"; }
    void initialise(const juce::String &commandLineParameters) override
    {
        demoWindow = std::make_unique<SCXTDemoMainWindow>(getApplicationName());
        demoWindow->toFront(true);
    }
    void shutdown() override {}

    struct SCXTWireframeMainComponent : public juce::Component
    {
        struct ClosableDW : public juce::DocumentWindow
        {
            SCXTWireframeMainComponent *comp{nullptr};
            ClosableDW(SCXTWireframeMainComponent *c, const juce::String &name,
                       juce::Colour backgroundColour, int requiredButtons)
                : comp(c), juce::DocumentWindow(name, backgroundColour, requiredButtons)
            {
            }
            void closeButtonPressed() override { comp->closeWin(this); }
        };

        SCXTWireframeMainComponent()
        {
            sst::jucegui::style::StyleSheet::initializeStyleSheets([]() {});
        }
        void paint(juce::Graphics &g) override { g.fillAll(juce::Colours::black); }
        void resized() override {}

        void closeWin(juce::DocumentWindow *t)
        {
            auto q = windows.begin();
            while (q != windows.end() && q->get() != t)
                ++q;
            if (q != windows.end())
                windows.erase(q);
        }
        std::vector<std::unique_ptr<juce::TextButton>> buttons;
        std::set<std::unique_ptr<juce::DocumentWindow>> windows;
    };
    class SCXTDemoMainWindow : public juce::DocumentWindow
    {
      public:
        SCXTDemoMainWindow(const juce::String &name)
            : juce::DocumentWindow(name,
                                   juce::Desktop::getInstance().getDefaultLookAndFeel().findColour(
                                       ResizableWindow::backgroundColourId),
                                   juce::DocumentWindow::allButtons)
        {
            setUsingNativeTitleBar(true);
            setResizable(true, false);
            setResizeLimits(400, 400, 900, 1000);

            setContentOwned(new SCXTWireframeMainComponent(), false);
            setVisible(true);
        }

        void closeButtonPressed() override
        {
            JUCEApplication::getInstance()->systemRequestedQuit();
        }

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SCXTDemoMainWindow)
    };

    std::unique_ptr<SCXTDemoMainWindow> demoWindow;
};

//==============================================================================
// This macro generates the main() routine that launches the app.
START_JUCE_APPLICATION(SCXTWireframesMain)
