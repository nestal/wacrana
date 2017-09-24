# Wacrana

This is Wacrana: a simple but modular web browser base on QtWebEngine.

Wacrana aims to be a neat and fast browser. Optional features like bookmarks
are implemented as [plugins](@ref plugin). The user can disable these
features if they don't need them, saving memory and CPU time.

## Plugins Support

To keep the memory footprint of the browser small, most optional features
are implemented as plugins. Unlike "add-ons" from other browser, these
plugins are written in C++, using a well-defined stable interface provided
by the browser.