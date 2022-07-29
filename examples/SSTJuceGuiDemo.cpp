//
// Created by Paul Walker on 5/24/22.
//

#include <juce_gui_basics/juce_gui_basics.h>

#include "NamedPanelDemo.h"
#include "KnobDemo.h"
#include "VSliderDemo.h"
#include "HSliderDemo.h"
#include "CoupledControls.h"
#include "ToggleDemo.h"
#include "MixerPrototype.h"
#include "MultiSwitchDemo.h"

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
        struct ClosableDW : public juce::DocumentWindow
        {
            SSTMainComponent *comp{nullptr};
            ClosableDW(SSTMainComponent *c, const juce::String &name, juce::Colour backgroundColour,
                       int requiredButtons)
                : comp(c), juce::DocumentWindow(name, backgroundColour, requiredButtons)
            {
            }
            void closeButtonPressed() override { comp->closeWin(this); }
        };
        template <typename T> void show()
        {
            juce::String name = T::name;
            {
                auto w = std::make_unique<ClosableDW>(
                    this, name + " - Light Skin",
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
                windows.insert(std::move(w));
            }

            {
                auto w = std::make_unique<ClosableDW>(
                    this, name + " - Dark Skin",
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
                windows.insert(std::move(w));
            }
        }
        template <typename T> auto mk()
        {
            auto b = std::make_unique<juce::TextButton>(T::name);
            b->onClick = [this]() { show<T>(); };
            addAndMakeVisible(*b);
            buttons.push_back(std::move(b));
        }

        SSTMainComponent()
        {
            mk<NamedPanelDemo>();
            mk<KnobDemo>();
            mk<VSliderDemo>();
            mk<HSliderDemo>();
            mk<CoupledControlsDemo>();
            mk<ToggleDemo>();
            mk<MixerProto>();
            mk<MultiSwitchDemo>();
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
