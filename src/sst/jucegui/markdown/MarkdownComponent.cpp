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

#include "sst/jucegui/markdown/MarkdownComponent.h"

#include "md4c/md4c.h"

#include <vector>
#include <cmath>
#include <algorithm>

namespace sst::jucegui::markdown
{

// ============================================================================
// Stage 1 output: a small document model. md4c's enter/leave nesting becomes a
// flat list of leaf blocks (paragraphs, headings, code, ...) carrying the
// indent/quote context they were found in, plus styled inline runs.
// ============================================================================
namespace
{
enum StyleBits : uint8_t
{
    Bold = 1,
    Italic = 2,
    Code = 4,
    Strike = 8
};

struct InlineRun
{
    juce::String text;
    uint8_t style{0};
    juce::String href;     // non-empty => link
    bool hardBreak{false}; // MD_TEXT_BR: force a line break here
};

enum class BlockType
{
    Paragraph,
    Heading,
    CodeBlock,
    Rule,
    Table
};

struct TableCell
{
    std::vector<InlineRun> runs;
    int align{0}; // MD_ALIGN: 0 default(=left), 1 left, 2 center, 3 right
    bool header{false};
};

struct TableRow
{
    std::vector<TableCell> cells;
    bool header{false};
};

struct Block
{
    BlockType type{BlockType::Paragraph};
    int headingLevel{0};
    int indentLevel{0}; // list nesting depth (each == one kListIndent)
    int quoteDepth{0};
    int listGroup{0};    // shared by all items of one top-level list; 0 if not a list item
    juce::String marker; // list marker text, e.g. "3." or the bullet
    std::vector<InlineRun> runs;
    juce::String codeText;           // raw text for CodeBlock
    std::vector<TableRow> tableRows; // for Table
};

// ---------------------------------------------------------------------------
// Entities. md4c hands us entity text verbatim (e.g. "&amp;"); decode the ones
// that matter plus numeric/hex forms.
// ---------------------------------------------------------------------------
juce::String decodeEntity(const juce::String &e)
{
    if (!e.startsWithChar('&') || !e.endsWithChar(';'))
        return e;
    auto body = e.substring(1, e.length() - 1);
    if (body == "amp")
        return "&";
    if (body == "lt")
        return "<";
    if (body == "gt")
        return ">";
    if (body == "quot")
        return "\"";
    if (body == "apos")
        return "'";
    if (body == "nbsp")
        return juce::String::charToString((juce::juce_wchar)0x00A0);
    if (body.startsWithIgnoreCase("#x"))
    {
        auto code = body.substring(2).getHexValue32();
        if (code > 0)
            return juce::String::charToString((juce::juce_wchar)code);
    }
    else if (body.startsWithChar('#'))
    {
        auto code = body.substring(1).getIntValue();
        if (code > 0)
            return juce::String::charToString((juce::juce_wchar)code);
    }
    return e;
}

juce::String attrToString(const MD_ATTRIBUTE &a)
{
    if (a.text == nullptr)
        return {};
    juce::String out;
    int i = 0;
    if (a.substr_offsets != nullptr && a.substr_types != nullptr)
    {
        while (a.substr_offsets[i] < a.size)
        {
            auto start = a.substr_offsets[i];
            auto end = a.substr_offsets[i + 1];
            auto piece = juce::String::fromUTF8(a.text + start, (int)(end - start));
            if (a.substr_types[i] == MD_TEXT_ENTITY)
                out += decodeEntity(piece);
            else
                out += piece;
            ++i;
        }
    }
    if (i == 0)
        out = juce::String::fromUTF8(a.text, (int)a.size);
    return out;
}

// ---------------------------------------------------------------------------
// Parse context threaded through the md4c callbacks as userdata.
// ---------------------------------------------------------------------------
struct ParseCtx
{
    std::vector<Block> blocks;
    Block *current{nullptr}; // leaf block currently accepting runs

    // Where text() appends inline runs. Points at the current leaf block's runs
    // for normal flow, or at the current table cell's runs inside a table.
    std::vector<InlineRun> *runSink{nullptr};

    // Table under construction (a table pushes exactly one block on leave).
    bool headerSection{false};
    Block tableBlk;
    TableRow curRow;
    TableCell curCell;

    int quoteDepth{0};
    struct ListState
    {
        bool ordered;
        int nextIndex;
    };
    std::vector<ListState> lists;

    // Every top-level list gets a fresh id; nested lists inherit it. Used so
    // items of one list pack tightly while separate lists get a full gap.
    int listGroupCounter{0};
    int currentListGroup{0};

    bool pendingMarker{false};
    juce::String pendingMarkerText;

    // Emphasis nests, so count rather than toggle a bit.
    int boldN{0}, italicN{0}, codeN{0}, strikeN{0};
    std::vector<juce::String> hrefStack;

    uint8_t styleBits() const
    {
        uint8_t s = 0;
        if (boldN > 0)
            s |= Bold;
        if (italicN > 0)
            s |= Italic;
        if (codeN > 0)
            s |= Code;
        if (strikeN > 0)
            s |= Strike;
        return s;
    }
    juce::String curHref() const { return hrefStack.empty() ? juce::String() : hrefStack.back(); }

    void beginLeaf(BlockType t)
    {
        Block b;
        b.type = t;
        b.quoteDepth = quoteDepth;
        b.indentLevel = (int)lists.size();
        b.listGroup = lists.empty() ? 0 : currentListGroup;
        if (pendingMarker)
        {
            b.marker = pendingMarkerText;
            pendingMarker = false;
        }
        blocks.push_back(std::move(b));
        current = &blocks.back();
        runSink = &current->runs;
    }

    // Finish the current inline leaf. In tight lists md4c does not wrap item
    // text in a paragraph, so a leaf we opened lazily has no leave_block(P) to
    // close it; sibling items and nested lists close it instead.
    void closeLeaf()
    {
        current = nullptr;
        runSink = nullptr;
    }

    // Inline content (text or a span) arrived. If no leaf is open — which
    // happens for tight list items where there is no MD_BLOCK_P — start a
    // paragraph so the content (and any pending list marker) has somewhere to go.
    void ensureInlineSink()
    {
        if (runSink == nullptr)
            beginLeaf(BlockType::Paragraph);
    }
};

int cbEnterBlock(MD_BLOCKTYPE type, void *detail, void *ud)
{
    auto *c = (ParseCtx *)ud;
    switch (type)
    {
    case MD_BLOCK_DOC:
        break;
    case MD_BLOCK_QUOTE:
        c->quoteDepth++;
        break;
    case MD_BLOCK_UL:
        c->closeLeaf();           // bound a preceding tight item's paragraph
        c->pendingMarker = false; // don't leak a parent bullet into a nested list
        if (c->lists.empty())
            c->currentListGroup = ++c->listGroupCounter; // a new top-level list
        c->lists.push_back({false, 1});
        break;
    case MD_BLOCK_OL:
    {
        auto *d = (MD_BLOCK_OL_DETAIL *)detail;
        c->closeLeaf();
        c->pendingMarker = false;
        if (c->lists.empty())
            c->currentListGroup = ++c->listGroupCounter;
        c->lists.push_back({true, d ? (int)d->start : 1});
        break;
    }
    case MD_BLOCK_LI:
    {
        auto *d = (MD_BLOCK_LI_DETAIL *)detail;
        c->closeLeaf(); // bound the previous sibling item's paragraph
        c->pendingMarker = true;
        if (d && d->is_task)
        {
            bool checked = (d->task_mark == 'x' || d->task_mark == 'X');
            c->pendingMarkerText = checked ? "[x]" : "[ ]";
        }
        else if (!c->lists.empty() && c->lists.back().ordered)
        {
            c->pendingMarkerText = juce::String(c->lists.back().nextIndex) + ".";
            c->lists.back().nextIndex++;
        }
        else
        {
            c->pendingMarkerText = juce::String::fromUTF8("\xE2\x80\xA2"); // bullet
        }
        break;
    }
    case MD_BLOCK_HR:
    {
        Block b;
        b.type = BlockType::Rule;
        b.quoteDepth = c->quoteDepth;
        c->blocks.push_back(std::move(b));
        break;
    }
    case MD_BLOCK_H:
    {
        auto *d = (MD_BLOCK_H_DETAIL *)detail;
        c->beginLeaf(BlockType::Heading);
        c->current->headingLevel = d ? (int)d->level : 1;
        break;
    }
    case MD_BLOCK_CODE:
        c->beginLeaf(BlockType::CodeBlock);
        break;
    case MD_BLOCK_P:
        c->beginLeaf(BlockType::Paragraph);
        break;
    case MD_BLOCK_TABLE:
        c->tableBlk = Block{};
        c->tableBlk.type = BlockType::Table;
        c->tableBlk.quoteDepth = c->quoteDepth;
        c->tableBlk.indentLevel = (int)c->lists.size();
        c->headerSection = false;
        break;
    case MD_BLOCK_THEAD:
        c->headerSection = true;
        break;
    case MD_BLOCK_TBODY:
        c->headerSection = false;
        break;
    case MD_BLOCK_TR:
        c->curRow = TableRow{};
        c->curRow.header = c->headerSection;
        break;
    case MD_BLOCK_TH:
    case MD_BLOCK_TD:
    {
        auto *d = (MD_BLOCK_TD_DETAIL *)detail;
        c->curCell = TableCell{};
        c->curCell.header = (type == MD_BLOCK_TH);
        c->curCell.align = d ? (int)d->align : 0;
        c->runSink = &c->curCell.runs;
        break;
    }
    case MD_BLOCK_HTML:
        break;
    }
    return 0;
}

int cbLeaveBlock(MD_BLOCKTYPE type, void *, void *ud)
{
    auto *c = (ParseCtx *)ud;
    switch (type)
    {
    case MD_BLOCK_QUOTE:
        c->quoteDepth--;
        break;
    case MD_BLOCK_UL:
    case MD_BLOCK_OL:
        if (!c->lists.empty())
            c->lists.pop_back();
        break;
    case MD_BLOCK_LI:
        c->closeLeaf();           // end this item's (possibly lazy) paragraph
        c->pendingMarker = false; // item had no content to attach the marker to
        break;
    case MD_BLOCK_H:
    case MD_BLOCK_CODE:
    case MD_BLOCK_P:
        c->current = nullptr;
        c->runSink = nullptr;
        break;
    case MD_BLOCK_TH:
    case MD_BLOCK_TD:
        c->curRow.cells.push_back(std::move(c->curCell));
        c->runSink = nullptr;
        break;
    case MD_BLOCK_TR:
        c->tableBlk.tableRows.push_back(std::move(c->curRow));
        break;
    case MD_BLOCK_TABLE:
        c->blocks.push_back(std::move(c->tableBlk));
        break;
    default:
        break;
    }
    return 0;
}

int cbEnterSpan(MD_SPANTYPE type, void *detail, void *ud)
{
    auto *c = (ParseCtx *)ud;
    c->ensureInlineSink(); // a span may be the first content of a tight list item
    switch (type)
    {
    case MD_SPAN_EM:
        c->italicN++;
        break;
    case MD_SPAN_STRONG:
        c->boldN++;
        break;
    case MD_SPAN_CODE:
        c->codeN++;
        break;
    case MD_SPAN_DEL:
        c->strikeN++;
        break;
    case MD_SPAN_A:
    {
        auto *d = (MD_SPAN_A_DETAIL *)detail;
        c->hrefStack.push_back(d ? attrToString(d->href) : juce::String());
        break;
    }
    case MD_SPAN_IMG:
        // Render alt text as normal flow; ignore the source. Push an empty href
        // so leave_span pops symmetrically.
        c->hrefStack.push_back({});
        break;
    default:
        break;
    }
    return 0;
}

int cbLeaveSpan(MD_SPANTYPE type, void *, void *ud)
{
    auto *c = (ParseCtx *)ud;
    switch (type)
    {
    case MD_SPAN_EM:
        c->italicN = std::max(0, c->italicN - 1);
        break;
    case MD_SPAN_STRONG:
        c->boldN = std::max(0, c->boldN - 1);
        break;
    case MD_SPAN_CODE:
        c->codeN = std::max(0, c->codeN - 1);
        break;
    case MD_SPAN_DEL:
        c->strikeN = std::max(0, c->strikeN - 1);
        break;
    case MD_SPAN_A:
    case MD_SPAN_IMG:
        if (!c->hrefStack.empty())
            c->hrefStack.pop_back();
        break;
    default:
        break;
    }
    return 0;
}

int cbText(MD_TEXTTYPE type, const MD_CHAR *s, MD_SIZE n, void *ud)
{
    auto *c = (ParseCtx *)ud;

    if (c->current && c->current->type == BlockType::CodeBlock)
    {
        // Code block text arrives verbatim, including '\n'.
        c->current->codeText += juce::String::fromUTF8(s, (int)n);
        return 0;
    }

    c->ensureInlineSink(); // first text of a tight list item starts its paragraph

    if (c->runSink == nullptr)
        return 0;

    auto pushRun = [&](const juce::String &t) {
        InlineRun r;
        r.text = t;
        r.style = c->styleBits();
        r.href = c->curHref();
        c->runSink->push_back(std::move(r));
    };

    switch (type)
    {
    case MD_TEXT_NORMAL:
    case MD_TEXT_CODE:
    case MD_TEXT_LATEXMATH:
        pushRun(juce::String::fromUTF8(s, (int)n));
        break;
    case MD_TEXT_ENTITY:
        pushRun(decodeEntity(juce::String::fromUTF8(s, (int)n)));
        break;
    case MD_TEXT_NULLCHAR:
        pushRun(juce::String::charToString((juce::juce_wchar)0xFFFD));
        break;
    case MD_TEXT_SOFTBR:
    {
        InlineRun r;
        r.text = " ";
        c->runSink->push_back(r);
        break;
    }
    case MD_TEXT_BR:
    {
        InlineRun r;
        r.hardBreak = true;
        c->runSink->push_back(r);
        break;
    }
    case MD_TEXT_HTML:
        break; // skip inline HTML
    }
    return 0;
}

std::vector<Block> parseMarkdown(const std::string &md)
{
    ParseCtx ctx;
    MD_PARSER p{};
    p.abi_version = 0;
    p.flags = MD_FLAG_COLLAPSEWHITESPACE | MD_FLAG_STRIKETHROUGH | MD_FLAG_TABLES |
              MD_FLAG_TASKLISTS | MD_FLAG_PERMISSIVEAUTOLINKS;
    p.enter_block = cbEnterBlock;
    p.leave_block = cbLeaveBlock;
    p.enter_span = cbEnterSpan;
    p.leave_span = cbLeaveSpan;
    p.text = cbText;
    md_parse(md.data(), (MD_SIZE)md.size(), &p, &ctx);
    return std::move(ctx.blocks);
}

// ============================================================================
// Stage 2: layout into per-colour GlyphArrangements + decoration lists.
// ============================================================================
struct Theme
{
    juce::Font body{SST_JUCE_FONT_CTOR(13)};
    juce::Font mono{SST_JUCE_FONT_CTOR(13)};
    float bodyHeight{13};
    juce::Colour text, codeText, codeBg, link, heading, quoteBar, rule, background;
};

constexpr float kOuterPad = 12.0f;
constexpr float kParagraphGap = 8.0f;
constexpr float kListItemGap = 2.0f; // between items of one tight list
constexpr float kHeadingGapAbove = 12.0f;
constexpr float kHeadingGapBelow = 4.0f;
constexpr float kListIndent = 22.0f;
constexpr float kMarkerGutter = 22.0f;
constexpr float kQuoteIndent = 16.0f;
constexpr float kQuoteBarWidth = 3.0f;
constexpr float kCodePad = 8.0f;
constexpr float kCodeGap = 8.0f;
constexpr float kLineLeading = 3.0f;
constexpr float kRuleGap = 10.0f;

float headingHeight(const Theme &t, int level)
{
    static const float f[6] = {1.9f, 1.55f, 1.3f, 1.15f, 1.0f, 0.9f};
    return t.bodyHeight * f[juce::jlimit(1, 6, level) - 1];
}

juce::Font fontForRun(uint8_t style, int headingLevel, const Theme &t)
{
    float h = headingLevel > 0 ? headingHeight(t, headingLevel) : t.bodyHeight;
    juce::Font f = (style & Code) ? t.mono : t.body;
    f = f.withHeight(h);
    int st = juce::Font::plain;
    if ((style & Bold) || headingLevel > 0)
        st |= juce::Font::bold;
    if (style & Italic)
        st |= juce::Font::italic;
    if (st != juce::Font::plain)
        f = f.withStyle(st);
    return f;
}

juce::Colour colourForRun(const InlineRun &r, int headingLevel, const Theme &t)
{
    if (r.href.isNotEmpty())
        return t.link;
    if (r.style & Code)
        return t.codeText;
    if (headingLevel > 0)
        return t.heading;
    return t.text;
}

struct LinkRect
{
    juce::Rectangle<float> r;
    juce::String href;
};

struct ColourGlyphs
{
    juce::Colour colour;
    juce::GlyphArrangement glyphs;
};

// A selectable character: its glyph and the x-span it occupies. Inter-word
// spaces get a zero/gap-width entry so they can be selected and copied too.
struct SelChar
{
    juce::juce_wchar ch;
    float x0, x1;
};

// One visual line of selectable text, in reading order. softWrapFromPrev marks
// a soft wrap continuing the previous line's paragraph (copied as a space);
// otherwise the join to the previous line is a newline.
struct SelLine
{
    float top, bottom;
    bool softWrapFromPrev{false};
    std::vector<SelChar> chars;
};

// A caret position: a boundary between characters (idx) on a visual line.
struct SelPosition
{
    int line{-1};
    int idx{0};
};
inline bool operator<(const SelPosition &a, const SelPosition &b)
{
    return a.line < b.line || (a.line == b.line && a.idx < b.idx);
}
inline bool operator==(const SelPosition &a, const SelPosition &b)
{
    return a.line == b.line && a.idx == b.idx;
}

struct Layout
{
    std::vector<ColourGlyphs> buckets;
    std::vector<juce::Rectangle<float>> codeSpanBg;
    std::vector<juce::Rectangle<float>> codeBlockBg;
    std::vector<juce::Rectangle<float>> strikes; // segment boxes; line drawn through centre
    std::vector<juce::Rectangle<float>> rules;
    std::vector<juce::Rectangle<float>> quoteBars;
    std::vector<juce::Rectangle<float>> linkUnderlines;
    std::vector<juce::Rectangle<float>> tableHeaderBg;
    std::vector<juce::Rectangle<float>> tableGrid;
    std::vector<LinkRect> links;
    std::vector<SelLine> selLines;
    float contentHeight{0};
    int laidOutWidth{-1};

    juce::GlyphArrangement &bucketFor(juce::Colour c)
    {
        for (auto &b : buckets)
            if (b.colour == c)
                return b.glyphs;
        buckets.push_back({c, {}});
        return buckets.back().glyphs;
    }
    void clear() { *this = Layout{}; }
};

// A visual word: whitespace-delimited, but may carry several style segments
// (e.g. "**bo**ld" is one word, two segments).
struct Seg
{
    juce::String text;
    juce::Font font{SST_JUCE_FONT_CTOR(1)};
    juce::Colour colour;
    uint8_t style;
    juce::String href;
    float width;
};
struct FlowWord
{
    std::vector<Seg> segs;
    float width{0};
    bool breakAfter{false};
};

std::vector<FlowWord> tokenize(const std::vector<InlineRun> &runs, int headingLevel, const Theme &t)
{
    std::vector<FlowWord> words;
    FlowWord cur;
    auto flush = [&]() {
        if (!cur.segs.empty())
        {
            words.push_back(std::move(cur));
            cur = FlowWord{};
        }
    };
    for (auto &r : runs)
    {
        if (r.hardBreak)
        {
            flush();
            FlowWord br;
            br.breakAfter = true;
            words.push_back(std::move(br));
            continue;
        }
        auto font = fontForRun(r.style, headingLevel, t);
        auto colour = colourForRun(r, headingLevel, t);
        const auto &s = r.text;
        int i = 0, len = s.length();
        while (i < len)
        {
            int start = i;
            while (i < len && !juce::CharacterFunctions::isWhitespace(s[i]))
                ++i;
            if (i > start)
            {
                Seg sg;
                sg.text = s.substring(start, i);
                sg.font = font;
                sg.colour = colour;
                sg.style = r.style;
                sg.href = r.href;
                sg.width = SST_STRING_WIDTH_FLOAT(font, sg.text);
                cur.segs.push_back(std::move(sg));
            }
            if (i < len && juce::CharacterFunctions::isWhitespace(s[i]))
            {
                flush();
                while (i < len && juce::CharacterFunctions::isWhitespace(s[i]))
                    ++i;
            }
        }
    }
    flush();
    for (auto &w : words)
    {
        w.width = 0;
        for (auto &sg : w.segs)
            w.width += sg.width;
    }
    return words;
}

// Greedy wrap + emit at absolute coordinates. Returns the y below the block.
float emitFlow(Layout &out, const std::vector<FlowWord> &words, float x0, float maxWidth,
               float startY, const Theme &t)
{
    const float spaceW = SST_STRING_WIDTH_FLOAT(t.body, " ");

    std::vector<std::vector<const FlowWord *>> lines;
    std::vector<bool> lineSoft; // parallel: is line[i] a soft wrap of line[i-1]?
    {
        std::vector<const FlowWord *> ln;
        float w = 0;
        bool curSoft = false;
        auto pushLine = [&](bool nextSoft) {
            lines.push_back(ln);
            lineSoft.push_back(curSoft);
            ln.clear();
            w = 0;
            curSoft = nextSoft;
        };
        for (auto &word : words)
        {
            if (word.breakAfter)
            {
                pushLine(false); // a hard break: the next line starts fresh
                continue;
            }
            float add = (ln.empty() ? 0 : spaceW) + word.width;
            if (!ln.empty() && w + add > maxWidth)
            {
                pushLine(true); // width wrap: the next line continues this paragraph
                add = word.width;
            }
            ln.push_back(&word);
            w += add;
        }
        if (!ln.empty() || lines.empty())
            pushLine(false);
    }

    float y = startY;
    for (size_t li = 0; li < lines.size(); ++li)
    {
        auto &line = lines[li];
        float ascent = 0, descent = 0;
        for (auto *word : line)
            for (auto &sg : word->segs)
            {
                ascent = std::max(ascent, sg.font.getAscent());
                descent = std::max(descent, sg.font.getDescent());
            }
        if (ascent == 0 && descent == 0)
        {
            ascent = t.body.getAscent();
            descent = t.body.getDescent();
        }
        const float baseline = y + ascent;

        SelLine sl;
        sl.top = y;
        sl.bottom = y + ascent + descent;
        sl.softWrapFromPrev = lineSoft[li];

        float penX = x0;
        for (size_t wi = 0; wi < line.size(); ++wi)
        {
            if (wi)
            {
                float gapStart = penX;
                penX += spaceW;
                sl.chars.push_back({(juce::juce_wchar)' ', gapStart, penX});
            }
            for (auto &sg : line[wi]->segs)
            {
                out.bucketFor(sg.colour).addLineOfText(sg.font, sg.text, penX, baseline);
                juce::Rectangle<float> box(penX, y, sg.width, ascent + descent);
                if (sg.style & Code)
                    out.codeSpanBg.push_back(box);
                if (sg.style & Strike)
                    out.strikes.push_back(box);
                if (sg.href.isNotEmpty())
                {
                    out.links.push_back({box, sg.href});
                    out.linkUnderlines.push_back({penX, baseline + 1.5f, sg.width, 1.0f});
                }
                float prev = 0;
                for (int k = 0; k < sg.text.length(); ++k)
                {
                    float next = SST_STRING_WIDTH_FLOAT(sg.font, sg.text.substring(0, k + 1));
                    sl.chars.push_back({sg.text[k], penX + prev, penX + next});
                    prev = next;
                }
                penX += sg.width;
            }
        }
        out.selLines.push_back(std::move(sl));
        y += ascent + descent + kLineLeading;
    }
    return y;
}

// One block's vertical extent and its blockquote nesting depth. Collected while
// laying out, then merged so a quote bar spans every contiguous block at (or
// deeper than) its level — that is what makes nested quotes read as nested
// rather than as separate one-line quotes.
struct QSpan
{
    float top, bottom;
    int depth;
};

void generateQuoteBars(Layout &out, const std::vector<QSpan> &spans)
{
    int maxDepth = 0;
    for (auto &s : spans)
        maxDepth = std::max(maxDepth, s.depth);

    for (int level = 1; level <= maxDepth; ++level)
    {
        size_t i = 0;
        while (i < spans.size())
        {
            if (spans[i].depth >= level)
            {
                float top = spans[i].top;
                float bottom = spans[i].bottom;
                size_t j = i;
                while (j + 1 < spans.size() && spans[j + 1].depth >= level)
                    bottom = spans[++j].bottom;
                float x = kOuterPad + (level - 1) * kQuoteIndent;
                out.quoteBars.push_back({x, top, kQuoteBarWidth, bottom - top});
                i = j + 1;
            }
            else
            {
                ++i;
            }
        }
    }
}

// Natural (unwrapped) width of a run of inline content on a single line.
float measureRuns(const std::vector<InlineRun> &runs, bool bold, const Theme &t)
{
    auto rr = runs;
    if (bold)
        for (auto &r : rr)
            r.style |= Bold;
    auto words = tokenize(rr, 0, t);
    const float spaceW = SST_STRING_WIDTH_FLOAT(t.body, " ");
    float w = 0;
    int n = 0;
    for (auto &wd : words)
    {
        if (wd.breakAfter)
            continue;
        w += wd.width;
        ++n;
    }
    if (n > 1)
        w += spaceW * (n - 1);
    return w;
}

// Lay out a GFM table as an aligned grid: measure natural column widths, shrink
// to fit the available width (cells then wrap), place cells with per-column
// alignment, and record a shaded header plus grid lines.
float layoutTable(Layout &out, const Block &b, float totalW, const Theme &t, float startY)
{
    constexpr float cellPadX = 7.0f;
    constexpr float cellPadY = 4.0f;

    const float x0 = kOuterPad + b.quoteDepth * kQuoteIndent + b.indentLevel * kListIndent;
    const float avail = std::max(40.0f, totalW - x0 - kOuterPad);

    size_t ncols = 0;
    for (auto &row : b.tableRows)
        ncols = std::max(ncols, row.cells.size());
    if (ncols == 0)
        return startY;

    std::vector<float> colW(ncols, 0.0f);
    for (auto &row : b.tableRows)
        for (size_t c = 0; c < row.cells.size(); ++c)
        {
            float w = measureRuns(row.cells[c].runs, row.cells[c].header, t) + 2 * cellPadX;
            colW[c] = std::max(colW[c], w);
        }

    float sum = 0;
    for (auto w : colW)
        sum += w;
    if (sum > avail && sum > 0)
    {
        float factor = avail / sum;
        for (auto &w : colW)
            w *= factor;
    }

    std::vector<float> colX(ncols + 1, 0.0f);
    colX[0] = x0;
    for (size_t c = 0; c < ncols; ++c)
        colX[c + 1] = colX[c] + colW[c];
    const float tableRight = colX[ncols];

    std::vector<float> rowEdges;
    float y = startY + kParagraphGap;
    const float tableTop = y;
    rowEdges.push_back(tableTop);

    for (auto &row : b.tableRows)
    {
        const float rowTop = y;
        float maxBottom = rowTop + t.body.getHeight() + 2 * cellPadY;
        for (size_t c = 0; c < ncols; ++c)
        {
            if (c >= row.cells.size() || row.cells[c].runs.empty())
                continue;
            const auto &cell = row.cells[c];
            float innerW = std::max(10.0f, colW[c] - 2 * cellPadX);
            float emitX = colX[c] + cellPadX;
            float emitW = innerW;

            float naturalW = measureRuns(cell.runs, cell.header, t);
            if (naturalW <= innerW && cell.align >= 2) // fits on one line; center/right
            {
                if (cell.align == 2)
                    emitX += (innerW - naturalW) * 0.5f;
                else
                    emitX += (innerW - naturalW);
                emitW = naturalW + 1.0f;
            }

            auto rr = cell.runs;
            if (cell.header)
                for (auto &r : rr)
                    r.style |= Bold;
            auto words = tokenize(rr, 0, t);
            float cellBottom = emitFlow(out, words, emitX, emitW, rowTop + cellPadY, t) + cellPadY;
            maxBottom = std::max(maxBottom, cellBottom);
        }
        if (row.header)
            out.tableHeaderBg.push_back({x0, rowTop, tableRight - x0, maxBottom - rowTop});
        y = maxBottom;
        rowEdges.push_back(y);
    }
    const float tableBottom = y;

    for (float e : rowEdges)
        out.tableGrid.push_back({x0, e, tableRight - x0, 1.0f});
    for (size_t c = 0; c <= ncols; ++c)
        out.tableGrid.push_back({colX[c], tableTop, 1.0f, tableBottom - tableTop});

    return tableBottom + kParagraphGap;
}

void layoutDocument(Layout &out, const std::vector<Block> &doc, int width, const Theme &t)
{
    out.clear();
    const float totalW = (float)width;
    const float rightPad = kOuterPad;
    float y = kOuterPad;

    std::vector<QSpan> quoteSpans;

    for (size_t i = 0; i < doc.size(); ++i)
    {
        const auto &b = doc[i];
        switch (b.type)
        {
        case BlockType::Rule:
        {
            float top = y;
            y += kRuleGap;
            float qx = kOuterPad + b.quoteDepth * kQuoteIndent;
            out.rules.push_back({qx, y, std::max(1.0f, totalW - qx - rightPad), 1.0f});
            y += kRuleGap;
            quoteSpans.push_back({top, y, b.quoteDepth});
            break;
        }
        case BlockType::Heading:
        {
            y += kHeadingGapAbove;
            float x0 = kOuterPad + b.quoteDepth * kQuoteIndent;
            float mw = std::max(20.0f, totalW - x0 - rightPad);
            auto words = tokenize(b.runs, b.headingLevel, t);
            float y1 = emitFlow(out, words, x0, mw, y, t);
            quoteSpans.push_back({y, y1, b.quoteDepth});
            y = y1 + kHeadingGapBelow;
            break;
        }
        case BlockType::Table:
        {
            float top = y;
            y = layoutTable(out, b, totalW, t, y);
            quoteSpans.push_back({top, y, b.quoteDepth});
            break;
        }
        case BlockType::Paragraph:
        {
            float baseIndent =
                kOuterPad + b.quoteDepth * kQuoteIndent + b.indentLevel * kListIndent;
            float x0 = baseIndent;
            if (b.marker.isNotEmpty())
                x0 = baseIndent + kMarkerGutter;
            float mw = std::max(20.0f, totalW - x0 - rightPad);

            auto words = tokenize(b.runs, 0, t);
            float yStart = y;
            float y1 = emitFlow(out, words, x0, mw, y, t);

            if (b.marker.isNotEmpty())
            {
                float baseline = yStart + t.body.getAscent();
                out.bucketFor(t.text).addLineOfText(t.body, b.marker, baseIndent, baseline);
            }

            // Items of the same tight list pack together; anything else (end of
            // the list, a new list, a following paragraph) gets a full gap.
            float gapAfter = kParagraphGap;
            if (b.listGroup != 0 && i + 1 < doc.size() && doc[i + 1].listGroup == b.listGroup)
                gapAfter = kListItemGap;
            y = y1 + gapAfter;
            quoteSpans.push_back({yStart, y, b.quoteDepth});
            break;
        }
        case BlockType::CodeBlock:
        {
            y += kCodeGap;
            float x0 = kOuterPad + b.quoteDepth * kQuoteIndent;
            float blockW = std::max(20.0f, totalW - x0 - rightPad);

            auto code = b.codeText;
            if (code.endsWithChar('\n'))
                code = code.dropLastCharacters(1);
            juce::StringArray codeLines;
            codeLines.addLines(code);

            auto cf = t.mono.withHeight(t.bodyHeight);
            float lineH = cf.getHeight() + kLineLeading;
            float top = y;
            float yy = top + kCodePad;
            for (auto &lstr : codeLines)
            {
                float baseline = yy + cf.getAscent();
                float lx = x0 + kCodePad;
                out.bucketFor(t.codeText).addLineOfText(cf, lstr, lx, baseline);

                SelLine sl;
                sl.top = yy;
                sl.bottom = yy + cf.getHeight();
                sl.softWrapFromPrev = false; // code lines are hard newlines
                float prev = 0;
                for (int k = 0; k < lstr.length(); ++k)
                {
                    float next = SST_STRING_WIDTH_FLOAT(cf, lstr.substring(0, k + 1));
                    sl.chars.push_back({lstr[k], lx + prev, lx + next});
                    prev = next;
                }
                out.selLines.push_back(std::move(sl));
                yy += lineH;
            }
            float bottom = yy - kLineLeading + kCodePad;
            out.codeBlockBg.push_back({x0, top, blockW, bottom - top});
            y = bottom + kCodeGap;
            quoteSpans.push_back({top, y, b.quoteDepth});
            break;
        }
        }
    }

    generateQuoteBars(out, quoteSpans);
    out.contentHeight = y + kOuterPad;
    out.laidOutWidth = width;
}

// ---------------------------------------------------------------------------
// Selection over the built SelLines.
// ---------------------------------------------------------------------------
SelPosition selHitTest(const std::vector<SelLine> &lines, juce::Point<float> p)
{
    if (lines.empty())
        return {};

    int li = -1;
    for (int i = 0; i < (int)lines.size(); ++i)
        if (p.y < lines[i].bottom)
        {
            li = i;
            break;
        }
    if (li < 0)
        li = (int)lines.size() - 1;

    const auto &ln = lines[li];
    int idx = (int)ln.chars.size();
    for (int k = 0; k < (int)ln.chars.size(); ++k)
    {
        float mid = (ln.chars[k].x0 + ln.chars[k].x1) * 0.5f;
        if (p.x < mid)
        {
            idx = k;
            break;
        }
    }
    return {li, idx};
}

SelPosition wordStart(const std::vector<SelLine> &lines, SelPosition pos)
{
    const auto &ch = lines[pos.line].chars;
    int i = juce::jlimit(0, (int)ch.size() - 1, pos.idx);
    while (i > 0 && !juce::CharacterFunctions::isWhitespace(ch[i - 1].ch))
        --i;
    return {pos.line, i};
}

SelPosition wordEnd(const std::vector<SelLine> &lines, SelPosition pos)
{
    const auto &ch = lines[pos.line].chars;
    int i = juce::jlimit(0, (int)ch.size(), pos.idx);
    while (i < (int)ch.size() && !juce::CharacterFunctions::isWhitespace(ch[i].ch))
        ++i;
    return {pos.line, i};
}

juce::String selectionText(const std::vector<SelLine> &lines, SelPosition a, SelPosition b)
{
    juce::String out;
    for (int li = a.line; li <= b.line; ++li)
    {
        const auto &ln = lines[li];
        int from = (li == a.line) ? a.idx : 0;
        int to = (li == b.line) ? b.idx : (int)ln.chars.size();
        if (li > a.line)
            out += ln.softWrapFromPrev ? " " : "\n";
        for (int k = from; k < to && k < (int)ln.chars.size(); ++k)
            out += juce::String::charToString(ln.chars[k].ch);
    }
    return out;
}

std::vector<juce::Rectangle<float>> selectionRects(const std::vector<SelLine> &lines, SelPosition a,
                                                   SelPosition b)
{
    std::vector<juce::Rectangle<float>> rects;
    for (int li = a.line; li <= b.line; ++li)
    {
        const auto &ln = lines[li];
        if (ln.chars.empty())
            continue;
        int from = (li == a.line) ? a.idx : 0;
        int to = (li == b.line) ? b.idx : (int)ln.chars.size();
        from = juce::jlimit(0, (int)ln.chars.size(), from);
        to = juce::jlimit(0, (int)ln.chars.size(), to);
        if (from >= to)
            continue;
        float left = ln.chars[from].x0;
        float right = ln.chars[to - 1].x1;
        rects.push_back({left, ln.top, right - left, ln.bottom - ln.top});
    }
    return rects;
}
} // namespace

// ============================================================================
// Component
// ============================================================================
struct MarkdownComponent::Impl
{
    std::vector<Block> doc;
    Layout layout;
    Theme theme;
    bool haveTheme{false};

    // Selection: anchor/caret are boundaries into layout.selLines. A selection
    // is active when anchor.line >= 0 and anchor != caret.
    SelPosition anchor, caret;
    bool dragging{false};
    bool downOnLink{false};
    juce::String downLinkHref;

    bool hasSelection() const { return anchor.line >= 0 && !(anchor == caret); }
    void clearSelection()
    {
        anchor = {};
        caret = {};
        dragging = false;
    }
    // anchor/caret ordered low..high
    SelPosition selLow() const { return (caret < anchor) ? caret : anchor; }
    SelPosition selHigh() const { return (caret < anchor) ? anchor : caret; }
};

namespace
{
Theme resolveTheme(MarkdownComponent &c)
{
    using S = MarkdownComponent::Styles;
    Theme t;
    t.body = c.getFont(S::labelfont);
    t.bodyHeight = t.body.getHeight();
    t.mono = c.getFont(S::codefont);
    t.text = c.getColour(S::labelcolor);
    t.codeText = c.getColour(S::codetext);
    t.codeBg = c.getColour(S::codebackground);
    t.link = c.getColour(S::link);
    t.heading = c.getColour(S::heading);
    t.quoteBar = c.getColour(S::quotebar);
    t.rule = c.getColour(S::brightoutline);
    t.background = c.getColour(S::background);
    return t;
}
} // namespace

MarkdownComponent::MarkdownComponent() : style::StyleConsumer(Styles::styleClass)
{
    impl = std::make_unique<Impl>();
    setAccessible(true);
    setTitle("Markdown");
    setWantsKeyboardFocus(true); // so Cmd/Ctrl-C and Cmd/Ctrl-A reach us
}

MarkdownComponent::~MarkdownComponent() = default;

void MarkdownComponent::setMarkdown(const std::string &markdown)
{
    rawMarkdown = markdown;
    impl->doc = parseMarkdown(markdown);
    layoutDirty = true;
    relayoutIfNeeded();
    repaint();
}

void MarkdownComponent::doLayout(int forWidth)
{
    if (forWidth <= 0)
        return;
    if (!style())
        return; // fonts/colours aren't resolvable yet

    impl->theme = resolveTheme(*this);
    impl->haveTheme = true;
    layoutDocument(impl->layout, impl->doc, forWidth, impl->theme);
    impl->clearSelection(); // positions index into selLines, which just changed
    layoutDirty = false;
}

void MarkdownComponent::relayoutIfNeeded()
{
    if (getWidth() <= 0)
        return;
    if (layoutDirty || impl->layout.laidOutWidth != getWidth())
        doLayout(getWidth());
    if (!impl->haveTheme)
        return;
    if (autoSizeHeight)
    {
        int h = (int)std::ceil(impl->layout.contentHeight);
        if (h > 0 && h != getHeight())
        {
            setSize(getWidth(), h); // triggers resized(); layout is cached for this width
            return;
        }
    }
}

int MarkdownComponent::getHeightForWidth(int width)
{
    if (width <= 0)
        return 0;
    if (layoutDirty || impl->layout.laidOutWidth != width)
        doLayout(width);
    return (int)std::ceil(impl->layout.contentHeight);
}

void MarkdownComponent::resized() { relayoutIfNeeded(); }

void MarkdownComponent::onStyleChanged()
{
    layoutDirty = true;
    relayoutIfNeeded();
    repaint();
}

void MarkdownComponent::paint(juce::Graphics &g)
{
    juce::Colour bg = impl->haveTheme ? impl->theme.background : getColour(Styles::background);
    g.fillAll(bg);
    if (!impl->haveTheme)
        return;

    auto &L = impl->layout;
    auto &t = impl->theme;

    g.setColour(t.codeBg);
    for (auto &r : L.codeBlockBg)
        g.fillRoundedRectangle(r, 4.0f);

    g.setColour(t.quoteBar);
    for (auto &r : L.quoteBars)
        g.fillRect(r);

    g.setColour(t.codeBg);
    for (auto &r : L.codeSpanBg)
        g.fillRoundedRectangle(r.expanded(2.0f, 0.0f), 3.0f);

    g.setColour(t.codeBg);
    for (auto &r : L.tableHeaderBg)
        g.fillRect(r);

    if (impl->hasSelection())
    {
        g.setColour(t.link.withAlpha(0.30f));
        for (auto &r : selectionRects(L.selLines, impl->selLow(), impl->selHigh()))
            g.fillRect(r);
    }

    for (auto &b : L.buckets)
    {
        g.setColour(b.colour);
        b.glyphs.draw(g);
    }

    g.setColour(t.rule);
    for (auto &r : L.tableGrid)
        g.fillRect(r);

    g.setColour(t.text);
    for (auto &r : L.strikes)
    {
        float cy = r.getCentreY();
        g.drawLine(r.getX(), cy, r.getRight(), cy, 1.0f);
    }

    g.setColour(t.link);
    for (auto &r : L.linkUnderlines)
        g.fillRect(r);

    g.setColour(t.rule);
    for (auto &r : L.rules)
        g.fillRect(r);
}

const juce::String *MarkdownComponent::linkHrefAt(juce::Point<float> p) const
{
    for (auto &l : impl->layout.links)
        if (l.r.contains(p))
            return &l.href;
    return nullptr;
}

bool MarkdownComponent::pointIsOnText(juce::Point<float> p) const
{
    for (auto &ln : impl->layout.selLines)
        if (p.y >= ln.top && p.y < ln.bottom && !ln.chars.empty())
            return p.x >= ln.chars.front().x0 && p.x <= ln.chars.back().x1;
    return false;
}

void MarkdownComponent::mouseMove(const juce::MouseEvent &e)
{
    auto p = e.position;
    if (linkHrefAt(p))
        setMouseCursor(juce::MouseCursor::PointingHandCursor);
    else if (pointIsOnText(p))
        setMouseCursor(juce::MouseCursor::IBeamCursor);
    else
        setMouseCursor(juce::MouseCursor::NormalCursor);
}

void MarkdownComponent::mouseExit(const juce::MouseEvent &)
{
    setMouseCursor(juce::MouseCursor::NormalCursor);
}

void MarkdownComponent::mouseDown(const juce::MouseEvent &e)
{
    grabKeyboardFocus();
    auto p = e.position;

    auto *href = linkHrefAt(p);
    impl->downOnLink = href != nullptr;
    impl->downLinkHref = href ? *href : juce::String();
    impl->dragging = false;

    auto pos = selHitTest(impl->layout.selLines, p);
    if (pos.line < 0)
    {
        impl->clearSelection();
        repaint();
        return;
    }

    if (e.getNumberOfClicks() >= 3)
    {
        impl->anchor = {pos.line, 0};
        impl->caret = {pos.line, (int)impl->layout.selLines[pos.line].chars.size()};
    }
    else if (e.getNumberOfClicks() == 2)
    {
        impl->anchor = wordStart(impl->layout.selLines, pos);
        impl->caret = wordEnd(impl->layout.selLines, pos);
    }
    else
    {
        impl->anchor = pos;
        impl->caret = pos;
    }
    repaint();
}

void MarkdownComponent::mouseDrag(const juce::MouseEvent &e)
{
    if (impl->layout.selLines.empty())
        return;
    impl->dragging = true;
    impl->caret = selHitTest(impl->layout.selLines, e.position);
    repaint();
}

void MarkdownComponent::mouseUp(const juce::MouseEvent &)
{
    // A plain click (no drag) on a link follows the link and drops any caret.
    if (!impl->dragging && impl->downOnLink)
    {
        auto href = impl->downLinkHref;
        impl->clearSelection();
        repaint();
        if (onLinkClick)
            onLinkClick(href.toStdString());
        else if (href.startsWithIgnoreCase("http"))
            juce::URL(href).launchInDefaultBrowser();
    }
    impl->dragging = false;
}

bool MarkdownComponent::keyPressed(const juce::KeyPress &key)
{
    if (key.getModifiers().isCommandDown())
    {
        auto code = key.getKeyCode();
        if (code == 'C' || code == 'c')
        {
            copySelectionToClipboard();
            return true;
        }
        if (code == 'A' || code == 'a')
        {
            selectAll();
            return true;
        }
    }
    return false;
}

void MarkdownComponent::selectAll()
{
    auto &lines = impl->layout.selLines;
    if (lines.empty())
        return;
    impl->anchor = {0, 0};
    impl->caret = {(int)lines.size() - 1, (int)lines.back().chars.size()};
    repaint();
}

std::string MarkdownComponent::getSelectedText() const
{
    if (!impl->hasSelection())
        return {};
    return selectionText(impl->layout.selLines, impl->selLow(), impl->selHigh()).toStdString();
}

void MarkdownComponent::copySelectionToClipboard()
{
    auto txt = getSelectedText();
    if (!txt.empty())
        juce::SystemClipboard::copyTextToClipboard(txt);
}

} // namespace sst::jucegui::markdown
