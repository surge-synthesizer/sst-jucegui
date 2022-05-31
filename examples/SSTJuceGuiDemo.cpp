//
// Created by Paul Walker on 5/24/22.
//

#include <juce_gui_basics/juce_gui_basics.h>

#include "NamedPanelDemo.h"
#include "KnobDemo.h"
#include "VSliderDemo.h"
#include "CoupledControls.h"
#include "ToggleDemo.h"

struct SSTJuceGuiDemo : public juce::JUCEApplication
{
    const juce::String getApplicationName() override { return "SSTJuceGuiDemo"; }
    const juce::String getApplicationVersion() override { return "0.5.0"; }
    void initialise(const juce::String &commandLineParameters) override
    {
        demoWindow = std::make_unique<SSTDemoMainWindow>(getApplicationName());
        demoWindow->toFront(true);
    }
    void shutdown() override {}

    struct SSTMainComponent : public juce::Component
    {
        template <typename T> void show(const juce::String &name)
        {
            {
                auto w = std::make_unique<juce::DocumentWindow>(
                    name + " - Light Skin",
                    juce::Desktop::getInstance().getDefaultLookAndFeel().findColour(
                        juce::ResizableWindow::backgroundColourId),
                    juce::DocumentWindow::allButtons);
                auto newt = new T();
                newt->setStyle(sst::jucegui::style::StyleSheet::getBuiltInStyleSheet(
                    sst::jucegui::style::StyleSheet::LIGHT));
                newt->setSettings(std::make_shared<sst::jucegui::style::Settings>());
                w->setContentOwned(newt, false);

                w->setBounds(200, 200, 600, 600);

                w->setResizable(true, true);
                w->setUsingNativeTitleBar(true);
                w->setVisible(true);
                windows.push_back(std::move(w));
            }

            {
                auto w = std::make_unique<juce::DocumentWindow>(
                    name + " - Dark Skin",
                    juce::Desktop::getInstance().getDefaultLookAndFeel().findColour(
                        juce::ResizableWindow::backgroundColourId),
                    juce::DocumentWindow::allButtons);
                auto newt = new T();
                newt->setStyle(sst::jucegui::style::StyleSheet::getBuiltInStyleSheet(
                    sst::jucegui::style::StyleSheet::DARK));
                newt->setSettings(std::make_shared<sst::jucegui::style::Settings>());
                w->setContentOwned(newt, false);

                w->setBounds(820, 200, 600, 600);

                w->setResizable(true, true);
                w->setUsingNativeTitleBar(true);
                w->setVisible(true);
                windows.push_back(std::move(w));
            }
        }
        SSTMainComponent()
        {
            {
                auto b = std::make_unique<juce::TextButton>("NamedPanel", "Named Panel");
                b->onClick = [this]() { show<NamedPanelDemo>("Named Panel"); };
                addAndMakeVisible(*b);
                buttons.push_back(std::move(b));
            }

            {
                auto b = std::make_unique<juce::TextButton>("KnobDemo", "Knobs");
                b->onClick = [this]() { show<KnobDemo>("Knobs"); };
                addAndMakeVisible(*b);
                buttons.push_back(std::move(b));
            }

            {
                auto b = std::make_unique<juce::TextButton>("VSliderDemo", "VSliders");
                b->onClick = [this]() { show<VSliderDemo>("VSliders"); };
                addAndMakeVisible(*b);
                buttons.push_back(std::move(b));
            }

            {
                auto b = std::make_unique<juce::TextButton>("LinkedControls", "LinkedControls");
                b->onClick = [this]() { show<CoupledControlsDemo>("Coupled Controls"); };
                addAndMakeVisible(*b);
                buttons.push_back(std::move(b));
            }

            {
                auto b = std::make_unique<juce::TextButton>("Toggle Buttons", "Toggle Buttons");
                b->onClick = [this]() { show<ToggleDemo>("Toggle Buttons"); };
                addAndMakeVisible(*b);
                buttons.push_back(std::move(b));
            }
        }
        void paint(juce::Graphics &g) override { g.fillAll(juce::Colours::black); }
        void resized() override
        {
            auto r = juce::Rectangle<int>(0, 0, 150, 25).translated(5, 5);
            for (const auto &b : buttons)
            {
                b->setBounds(r);
                r = r.translated(0, 30);
            }
        }

        std::vector<std::unique_ptr<juce::TextButton>> buttons;
        std::vector<std::unique_ptr<juce::DocumentWindow>> windows;
    };
    class SSTDemoMainWindow : public juce::DocumentWindow
    {
      public:
        SSTDemoMainWindow(const juce::String &name)
            : juce::DocumentWindow(name,
                                   juce::Desktop::getInstance().getDefaultLookAndFeel().findColour(
                                       ResizableWindow::backgroundColourId),
                                   juce::DocumentWindow::allButtons)
        {
            setUsingNativeTitleBar(true);
            setResizable(true, false);
            setResizeLimits(400, 400, 900, 1000);

            setContentOwned(new SSTMainComponent(), false);
            setVisible(true);
        }

        void closeButtonPressed() override
        {
            JUCEApplication::getInstance()->systemRequestedQuit();
        }

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SSTDemoMainWindow)
    };

    std::unique_ptr<SSTDemoMainWindow> demoWindow;
};

//==============================================================================
// This macro generates the main() routine that launches the app.
START_JUCE_APPLICATION(SSTJuceGuiDemo)
