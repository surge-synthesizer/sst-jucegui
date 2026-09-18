# Vendored md4c

This directory contains a vendored copy of **md4c**, the Markdown parser used by
`sst::jucegui::markdown::MarkdownComponent`.

- Upstream: https://github.com/mity/md4c
- Version: release-0.5.2
- License: MIT (see `LICENSE.md` in this directory)

md4c is a single C source file (`md4c.c`) plus header (`md4c.h`). It is
CommonMark compliant with optional GitHub-flavoured extensions behind flags.
It is MIT licensed, so unlike JUCE itself it carries no GPL friction when
statically linked.

## Local modifications

None. The files are dropped in verbatim from the upstream `src/` directory.

## Updating

Replace `md4c.c` and `md4c.h` with the newer upstream `src/md4c.c` and
`src/md4c.h`, refresh `LICENSE.md`, and bump the version noted above. The only
consumer is `src/sst/jucegui/markdown/MarkdownComponent.cpp`, which uses the
public SAX-style callback API (`md_parse`).
