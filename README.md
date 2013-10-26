ramen
=====

Game? Engine? Nobody knows yet!

SDL: http://www.libsdl.org/
GLM: http://glm.g-truc.net/0.9.4/index.html

Boost: http://www.boost.org/ 
At least 1.54 is required, if you want to use an earlier version you will need to compile log
from the original sourceforge repository


For windows builds, binaries for VS2012EEE are provided in build/win
Please extract external.7z at the solution location.
For GLES/EGL emulation, angleproject was used during developement.

Angleproject: https://code.google.com/p/angleproject/

Some features about the framework:
- Fonts with Freetype
- SDL for window and context creation. Also for input handling
  Note that on windows, SDL is only used to create the windows. EGL context is then created manually.


TODO:
-----

GENERAL:
- Improve profiler with many level of hierarchy
- Loop to not take 100% cpu

GRAPHICS:
- Handle resize on windows

FONTS:
- Improve font packing with an algorithm like http://clb.demon.fi/files/RectangleBinPack.pdf
- Font rendering probably needs some optimization
