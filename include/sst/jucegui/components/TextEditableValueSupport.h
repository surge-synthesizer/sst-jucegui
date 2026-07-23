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

#ifndef INCLUDE_SST_JUCEGUI_COMPONENTS_TEXTEDITABLEVALUESUPPORT_H
#define INCLUDE_SST_JUCEGUI_COMPONENTS_TEXTEDITABLEVALUESUPPORT_H

#include <memory>
#include <string>

#include <juce_gui_basics/juce_gui_basics.h>

#include <sst/jucegui/style/StyleSheet.h>
#include <sst/jucegui/components/BaseStyles.h>

namespace sst::jucegui::components
{

// A text editor whose ctrl+return submits a plain return, so a multi-field
// container can distinguish the two. Shared by the draggable text value widgets.
struct TextEditableValueEditor : juce::TextEditor
{
    TextEditableValueEditor() : TextEditor() {}
    bool keyPressed(const juce::KeyPress &key) override
    {
        if (key.getModifiers().isCtrlDown() && (key.getKeyCode() == juce::KeyPress::returnKey))
        {
            auto kp = juce::KeyPress(juce::KeyPress::returnKey);
            return juce::TextEditor::keyPressed(kp);
        }
        return juce::TextEditor::keyPressed(key);
    }
};

/*
 * Style class shared by the continuous and discrete draggable text values.
 * Deliberately declared outside the CRTP template below: SCLASS expands to a
 * static constexpr object, so putting it inside the template would mint one
 * styleClass per instantiation and the two widgets would theme independently
 * despite carrying the same name.
 */
struct TextEditableValueStyles : base_styles::Base,
                                 base_styles::Outlined,
                                 base_styles::BaseLabel,
                                 base_styles::ValueBearing
{
    SCLASS(draggabletexteditor);
    static void initialize()
    {
        style::StyleSheet::addClass(styleClass)
            .withBaseClass(base_styles::Base::styleClass)
            .withBaseClass(base_styles::Outlined::styleClass)
            .withBaseClass(base_styles::BaseLabel::styleClass)
            .withBaseClass(base_styles::ValueBearing::styleClass);
    }
};

/*
 * Shared implementation for the draggable text value widgets. Mixed into a
 * concrete widget T, which must be a juce::Component and a style::StyleConsumer,
 * and must supply:
 *
 *   bool hasSource();                         // is a data source bound
 *   std::string displayString();              // value text, honouring displayUnits
 *   void applyString(const std::string &s);   // commit typed text
 *   void applyDefault();                       // commit on empty string
 *
 * The paint/mouse helpers take `that` explicitly rather than using asT() so a
 * *subclass* of T can reuse them and still resolve style and callbacks against
 * itself -- that is what lets a widget which overrides paint() (e.g. to draw a
 * split n/d display) reuse the background and value-text drawing.
 *
 * Because the continuous and discrete param editors are unrelated CRTP base
 * classes, sharing is by mixin rather than by a common base.
 */
template <typename T> struct TextEditableValueSupport
{
    using Styles = TextEditableValueStyles;

    std::unique_ptr<juce::TextEditor> underlyingEditor;
    bool displayUnits{false};
    bool everDragged{false};
    bool isEditDrag{false};

    bool isSetFromDrag() const { return isEditDrag; }

    struct EditIsDragGuard
    {
        TextEditableValueSupport<T> &w;
        EditIsDragGuard(TextEditableValueSupport<T> &w) : w(w) { w.isEditDrag = true; }
        ~EditIsDragGuard() { w.isEditDrag = false; }
    };

    T *asT() { return static_cast<T *>(this); }

    void initTextEditor()
    {
        auto self = asT();
        underlyingEditor = std::make_unique<TextEditableValueEditor>();
        underlyingEditor->onEscapeKey = [sp = juce::Component::SafePointer(self)] {
            if (sp)
                sp->underlyingEditor->setVisible(false);
        };
        underlyingEditor->onFocusLost = [sp = juce::Component::SafePointer(self)] {
            if (sp && sp->underlyingEditor->isVisible())
                sp->setFromEditor();
        };
        underlyingEditor->onReturnKey = [sp = juce::Component::SafePointer(self)] {
            if (sp)
                sp->setFromEditor();
        };
        self->addChildComponent(*underlyingEditor);
    }

    void activateEditor()
    {
        auto self = asT();
        underlyingEditor->setText(self->displayString());
        underlyingEditor->setVisible(true);
        underlyingEditor->selectAll();
        underlyingEditor->grabKeyboardFocus();
    }

    void setFromEditor()
    {
        auto self = asT();
        jassert(underlyingEditor->isVisible());
        auto t = underlyingEditor->getText();
        if (t.isEmpty())
            self->applyDefault();
        else
            self->applyString(t.toStdString());
        underlyingEditor->setVisible(false);
        self->repaint();
    }

    void restyleTextEditor(const juce::Font &font)
    {
        underlyingEditor->setColour(juce::TextEditor::ColourIds::outlineColourId,
                                    juce::Colours::black.withAlpha(0.f));
        underlyingEditor->setColour(juce::TextEditor::ColourIds::focusedOutlineColourId,
                                    juce::Colours::black.withAlpha(0.f));
        underlyingEditor->setColour(juce::TextEditor::ColourIds::backgroundColourId,
                                    juce::Colours::black.withAlpha(0.f));
        underlyingEditor->setFont(font);
        underlyingEditor->setIndents(0, 0);
        underlyingEditor->setJustification(juce::Justification::centred);
    }

    void layoutTextEditor(juce::Rectangle<int> b) { underlyingEditor->setBounds(b); }

    template <typename W>
    void paintBackgroundFor(W *that, juce::Graphics &g, juce::Rectangle<int> area)
    {
        g.setColour(that->getColour(Styles::background));
        if (that->isHovered)
            g.setColour(that->getColour(Styles::background_hover));
        g.fillRoundedRectangle(area.toFloat(), 3.f);
    }

    template <typename W> void paintBackgroundFor(W *that, juce::Graphics &g)
    {
        paintBackgroundFor(that, g, that->getLocalBounds());
    }

    template <typename W>
    void paintValueTextFor(W *that, juce::Graphics &g, const std::string &text,
                           juce::Rectangle<int> area)
    {
        g.setFont(that->getFont(Styles::labelfont));
        // on hover the text colour is intentionally the same
        auto col = that->getColour(Styles::value);
        g.setColour(that->isEnabled() ? col : col.withAlpha(0.5f));
        g.drawText(text, area, juce::Justification::centred);
    }

    // background plus the centred value, suppressed while the type-in is open
    template <typename W> void paintFor(W *that, juce::Graphics &g)
    {
        paintBackgroundFor(that, g);
        if (that->hasSource() && !underlyingEditor->isVisible())
            paintValueTextFor(that, g, that->displayString(), that->getLocalBounds());
    }

    /*
     * Mouse. Only the shared prologue lives here; the drag body is the one thing
     * each widget genuinely does differently (float delta from an anchor vs
     * residual pixels to whole jogs), so it stays in the widget. Each returns
     * false when the caller should stop -- i.e. the event was a popup.
     */
    template <typename W> bool mouseDownFor(W *that, const juce::MouseEvent &e)
    {
        if (e.mods.isPopupMenu() && that->onPopupMenu)
        {
            that->onPopupMenu(e.mods);
            return false;
        }
        everDragged = false;
        return true;
    }

    template <typename W> void mouseUpFor(W *that, const juce::MouseEvent &e)
    {
        if (e.mods.isPopupMenu() && that->onPopupMenu)
            return;

        if (everDragged)
            that->onEndEdit();
        else
            activateEditor();
    }

    template <typename W> bool mouseDragFor(W *that, const juce::MouseEvent &e)
    {
        if (e.mods.isPopupMenu() && that->onPopupMenu)
            return false;
        if (!everDragged)
            that->onBeginEdit();
        everDragged = true;
        return true;
    }
};

} // namespace sst::jucegui::components

#endif // INCLUDE_SST_JUCEGUI_COMPONENTS_TEXTEDITABLEVALUESUPPORT_H
