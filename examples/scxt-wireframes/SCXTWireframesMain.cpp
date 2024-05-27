/*
 * sst-jucegui - an open source library of juce widgets
 * built by Surge Synth Team.
 *
 * Copyright 2023-2024, various authors, as described in the GitHub
 * transaction log.
 *
 * sst-jucegui is released under the MIT license, as described
 * by "LICENSE.md" in this repository. This means you may use this
 * in commercial software if you are a JUCE Licensee. If you use JUCE
 * in the open source / GPL3 context, your combined work must be
 * released under GPL3.
 *
 * All source in sst-jucegui available at
 * https://github.com/surge-synthesizer/sst-jucegui
 */

#include <juce_gui_basics/juce_gui_basics.h>

#include "sst/jucegui/style/StyleSheet.h"
#include "MainWindow.h"

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
            setResizable(true, true);
            setSize(1186, 810);

            setContentOwned(new scxt::wireframe::MainWindow(), false);
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
