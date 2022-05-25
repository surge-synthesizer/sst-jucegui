//
// Created by Paul Walker on 5/24/22.
//

#include <juce_gui_basics/juce_gui_basics.h>

#include <sst/jucegui/components/NamedPanel.h>
#include <sst/jucegui/components/WindowPanel.h>

struct Solid : public juce::Component
{
    juce::Colour color;
    Solid(const juce::Colour &c) : color(c) {}
    void paint(juce::Graphics &g) override
    {
        if (isHover)
            g.fillAll(color);
        else
        {
            // g.setColour(juce::Colour(240, 240, 240));
            // g.drawRect(getLocalBounds());
        }
    }
    void mouseEnter(const juce::MouseEvent &) override
    {
        isHover = true;
        repaint();
    }
    void mouseExit(const juce::MouseEvent &) override
    {
        isHover = false;
        repaint();
    }
    bool isHover{false};
};

struct NamedPanelDemo : public sst::jucegui::components::WindowPanel
{
    NamedPanelDemo()
    {
        panelOne = std::make_unique<sst::jucegui::components::NamedPanel>("Basic Panel");
        panelOne->setContentAreaComponent(std::make_unique<Solid>(juce::Colours::yellow));
        addAndMakeVisible(*panelOne);

        panelTwo = std::make_unique<sst::jucegui::components::NamedPanel>("Panel Two");
        panelTwo->setContentAreaComponent(std::make_unique<Solid>(juce::Colours::cyan));
        addAndMakeVisible(*panelTwo);

        panelThree =
            std::make_unique<sst::jucegui::components::NamedPanel>("Panel Three Long Name");
        panelThree->addStyleSuperclass({"greenpanel"});
        panelThree->setContentAreaComponent(std::make_unique<Solid>(juce::Colours::red));
        addAndMakeVisible(*panelThree);
    }

    void resized() override
    {
        panelOne->setBounds(10, 10, 200, 200);
        panelTwo->setBounds(10, 210, 200, 200);
        panelThree->setBounds(210, 10, 200, 400);
    }

    std::unique_ptr<sst::jucegui::components::NamedPanel> panelOne, panelTwo, panelThree;
};

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
                w->setContentOwned(newt, false);

                w->setBounds(600, 200, 600, 600);

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
