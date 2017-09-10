\page plugin Wacrana Plugins


Wacrana supports a plugin system to allow developers to extend its
functionality very easily. Most optional features of Wacrana is supported
by plugins, so that they can be removed when the user does not need them,
keeping Wacrana small and fast. 

All plugins must implement the wacrana::V1::Plugin interface. See its
documentations for more details. All interfaces in the wacrana::V1
namespace are stable. Plugins that solely uses these interfaces will
work with all versions of the browser applications that supports them
without compilation. For example, wacrana::V1 interfaces are supported
by all V1.x version of the browser.

Once a major version of the browser is released, only the most previous
version of interface will be supported. All older interfaces will not be
supported. For example, when wacrana 2.0 is released, the wacrana::V1
interfaces will still be supported. However, when 3.0 is released, the
wacrana::V1 interfaces will not be supported anymore.

By convention, all interface headers files have `.hpp` extension. Plugins
should only include these headers to ensure compatibility between browser
versions.  

# Plugin Types

# GCC Visibility for Plugin

By default, all symbols in the Wacrana plugin DSO in Linux are marked as "hidden". It is
specified in CMakeLists.txt via the CXX_VISIBILITY_PRESET variable.

# Configuration for Plugins

Some doc here
