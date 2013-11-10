ramen
=====

Game? Engine? Nobody knows yet!

FBX SDK: http://www.autodesk.com/products/fbx/overview  
SDL: http://www.libsdl.org/  
GLM: http://glm.g-truc.net/0.9.4/index.html  
Boost: http://www.boost.org/  
(At least 1.54 is required, if you want to use an earlier version you will need to compile log
from the original sourceforge repository)


For windows builds, binaries for VS2012EEE are provided in build/win
Please extract external.7z at the solution location.
For GLES/EGL emulation, angleproject was used during developement.

Resources data is provided in resource.7z, this is a temporary design.

Angleproject: https://code.google.com/p/angleproject/


Some features about the framework:
- SDL for window and context creation. Also for input handling
  Note that on windows, SDL is only used to create the windows. EGL context is then created manually.
- Fonts drawing with Freetype. A font atlas texture is generated and text is drawn with a single VBO
- Simple profiler
- FPS, CPU, memory usage monitor
- Job system

TODO:
-----

GENERAL:
- Improve profiler with many level of hierarchy
- Loops not to take 100% cpu

GRAPHICS:
- Handle resize on windows
- Improve shader system to be easier to use

FONTS:
- Improve font packing with an algorithm like http://clb.demon.fi/files/RectangleBinPack.pdf
- Font rendering probably needs some optimization

IO
- Async file loading
- Use a ressource file instead of exposing resource folder
