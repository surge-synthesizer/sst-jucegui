# Markdown Component

A feature test for `sst::jucegui::markdown::MarkdownComponent`. Everything below
is rendered by the component you are looking at, styled from the active
stylesheet (try the Light and Dark windows side by side).

## Inline styles

Body text supports **bold**, *italic*, ***bold italic***, `inline code`, and
~~strikethrough~~. Links are hit-tested and open in the browser — for example
[md4c](https://github.com/mity/md4c), the MIT-licensed parser doing stage one.
Emphasis can **nest `code` and *italics* together** on a single wrapped line.

## Headings

### Heading level 3
#### Heading level 4

Headings scale from the body font and share the heading colour.

## Lists

- Unordered item
- Item with a deliberately long line so the greedy word wrap has something to do
  across whatever width the viewport happens to be at right now
  - Nested item one
  - Nested item two
- Third item

1. Ordered one
2. Ordered two
3. Ordered three

- [x] Task list: done
- [ ] Task list: not done

## Block quote

> A block quote gets a left rule and indent.
>
> > And quotes can nest, tracking depth for each bar.

## Code block

```cpp
juce::Font fontFor(uint8_t style, const Theme &t)
{
    juce::Font f = (style & Code) ? t.mono : t.body;
    if (style & Bold)   f = f.withStyle(juce::Font::bold);
    if (style & Italic) f = f.withStyle(juce::Font::italic);
    return f;
}
```

## Tables

Tables are a GFM extension. Columns are measured, shrunk to fit, and honour the
alignment markers in the header separator (left, centre, right):

| Feature      | Status  |                    Notes |
| :----------- | :-----: | -----------------------: |
| Headings     |  done   |      `#` through `######` |
| Inline code  |  done   |        rounded background |
| Lists        |  done   |  ordered, nested, task    |
| Block quotes |  done   |         left rule, nested |
| Links        |  done   | hit-tested, cursor change |
| Tables       |  done   |    aligned grid, wrapping |

Cells carry full inline styling too — **bold**, `code`, and
[links](https://surge-synthesizer.github.io) all work inside a table cell, and
long cell text wraps within its column just like a paragraph does.

---

That's the whole feature set. See `juce-markdown-renderer-DESIGN.md` at the repo
root for the design rationale behind the two-stage parse-then-layout approach.
