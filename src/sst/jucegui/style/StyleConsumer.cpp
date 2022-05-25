//
// Created by Paul Walker on 5/25/22.
//

#include <sst/jucegui/style/StyleConsumer.h>

namespace sst::jucegui::style
{
void StyleConsumer::setStyle(const StyleSheet::ptr_t &s)
{
    stylep = s;
    onStyleChanged();

    auto jc = dynamic_cast<juce::Component *>(this);
    std::function<void(juce::Component *)> rec;
    rec = [&rec, s](juce::Component *comp) {
        if (!comp)
            return;
        for (auto c : comp->getChildren())
        {
            auto sc = dynamic_cast<StyleConsumer *>(c);
            if (sc)
            {
                sc->setStyle(s);
            }
            else
            {
                rec(c);
            }
        }
    };
    if (jc)
    {
        rec(jc);
    }
    jc->repaint();
}
}