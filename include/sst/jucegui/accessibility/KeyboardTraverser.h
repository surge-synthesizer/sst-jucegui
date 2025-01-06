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

#ifndef INCLUDE_SST_JUCEGUI_ACCESSIBILITY_KEYBOARDTRAVERSER_H
#define INCLUDE_SST_JUCEGUI_ACCESSIBILITY_KEYBOARDTRAVERSER_H

#include <iostream>
#include <limits>

namespace sst::jucegui::accessibility
{
// #define KLG(...) std::cout << __FILE__ << ":" << __LINE__ << " " << __func__ << " " <<
// __VA_ARGS__ << std::endl;
#define KLG(...)
struct KeyboardTraverser : juce::KeyboardFocusTraverser
{
    struct IssueIDIfMissingMarker
    {
        virtual ~IssueIDIfMissingMarker() = default;
    };

    ~KeyboardTraverser() override = default;
    juce::Component *getDefaultComponent(juce::Component *parentComponent) override
    {
        KLG(nm(parentComponent));
        auto tn = findNeighborByIndex(parentComponent, START);
        if (tn)
            return tn;

        return juce::KeyboardFocusTraverser::getDefaultComponent(parentComponent);
    }

    static const juce::Identifier &idIndex()
    {
        static juce::Identifier idIndex{"sstjucegui-idIndex"};
        return idIndex;
    }

    static const juce::Identifier &fullIdIndex()
    {
        static juce::Identifier idIndex{"sstjucegui-fullIdIndex"};
        return idIndex;
    }

    static std::string nm(juce::Component *c)
    {
        if (!c)
            return "nullptr";
        std::string res = "'" + c->getTitle().toStdString();
        res += "' (tid=";
        res += std::to_string(traversalId(c)) + ")";
        return res;
    }

    enum Mode
    {
        START,
        PRIOR,
        NEXT,
        END
    };
    const char *dirName[4] = {"start", "prior", "next", "end"};
    juce::Component *findNeighborByIndex(juce::Component *from, Mode dir) const
    {
        auto c = from->getParentComponent();
        while (c)
        {
            if (c->isKeyboardFocusContainer())
            {
                break;
            }
            c = c->getParentComponent();
        }

        if (!c)
            return nullptr;

        std::function<void(juce::Component *)> rec;

        int lim = (dir == NEXT || dir == START ? std::numeric_limits<int>::max()
                                               : std::numeric_limits<int>::min());
        int midx = traversalId(from);
        if (midx == 0)
        {
            return nullptr;
        }

        KLG("Finding neighbor " << dirName[(int)dir] << " from " << nm(from))

        // also find extrma for wrap

        // careful
        juce::Component *res{nullptr};
        rec = [&, this](juce::Component *c) {
            if (!c->isVisible() || !c->isEnabled())
                return;

            if (c->isAccessible())
            {
                auto fidx = traversalId(c);
                if (fidx != 0)
                {
                    if (dir == PRIOR)
                    {
                        if (fidx < midx && fidx > lim)
                        {
                            lim = fidx;
                            res = c;
                        }
                    }
                    else if (dir == NEXT)
                    {
                        if (fidx > midx && fidx < lim)
                        {
                            lim = fidx;
                            res = c;
                        }
                    }
                    else if (dir == START)
                    {
                        if (fidx < lim)
                        {
                            lim = fidx;
                            res = c;
                        }
                    }
                    else if (dir == END)
                    {
                        if (fidx > lim)
                        {
                            lim = fidx;
                            res = c;
                        }
                    }
                }
            }
            for (auto k : c->getChildren())
            {
                rec(k);
            }
        };
        if (c)
            rec(c);

        KLG("Returning neighbor " << nm(res));
        return res;
    }

    static void issueTraversalId(juce::Component *c)
    {
        int newID{3};
        auto p = c->getParentComponent();
        if (!p)
            return;
        for (auto k : p->getChildren())
        {
            if (k == c)
                continue;
            auto hasidx = k->getProperties().getVarPointer(idIndex());
            if (hasidx)
                newID = std::max(newID, (int)*hasidx + 1);
        }
        c->getProperties().set(idIndex(), newID);
        KLG("Issued " << nm(c) << " " << newID);
    }
    static int traversalId(juce::Component *c)
    {
        if (!c->isAccessible())
            return 0;

        auto hasidx = c->getProperties().getVarPointer(idIndex());
        if (!hasidx)
        {
            if (dynamic_cast<IssueIDIfMissingMarker *>(c))
            {
                issueTraversalId(c);
            }
            else
            {
                return 0;
            }
        }

        auto fullidx = c->getProperties().getVarPointer(fullIdIndex());
        if (fullidx)
            return *fullidx;

        auto res = 0;
        auto curr = c;
        while (curr)
        {
            auto idx = curr->getProperties().getVarPointer(idIndex());
            if (idx)
                res += (int)*idx;
            curr = curr->getParentComponent();
        }
        if (res != 0)
        {
            c->getProperties().set(fullIdIndex(), res);
        }
        return res;
    }

    juce::Component *getNextComponent(juce::Component *current) override
    {
        auto tn = findNeighborByIndex(current, NEXT);
        if (tn)
        {
            return tn;
        }
        KLG("** NULL NEXT ** " << nm(current));
        auto ln = findNeighborByIndex(current, START);
        if (ln)
        {
            KLG("Returning start " << nm(ln));
            return ln;
        }

        return juce::KeyboardFocusTraverser::getNextComponent(current);
    }
    juce::Component *getPreviousComponent(juce::Component *current) override
    {
        auto tn = findNeighborByIndex(current, PRIOR);
        if (tn)
        {
            return tn;
        }
        KLG("** NULL PREV ** " << nm(current));
        auto ln = findNeighborByIndex(current, END);
        if (ln)
        {
            KLG("Returning end " << nm(ln));
            return ln;
        }
        // shrug
        return juce::KeyboardFocusTraverser::getPreviousComponent(current);
    }
    std::vector<juce::Component *> getAllComponents(juce::Component *parentComponent) override
    {
        return juce::KeyboardFocusTraverser::getAllComponents(parentComponent);
    }

    static void assignTraversalIndex(juce::Component *c, int idx)
    {
        c->getProperties().set(idIndex(), idx);
    }
};
#undef KLG
} // namespace sst::jucegui::accessibility
#endif // KEYBOARDTRAVERSER_H
