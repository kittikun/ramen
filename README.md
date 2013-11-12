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

Priority levels:
P0 Absolute must / Breaking change
P1 High / Consequent design change
P2 When possible
P3 Maybe later

DATABASE:
- (P1) Replace global lock with clever system

GENERAL:
- (P3) Improve profiler with many level of hierarchy
- (P2) Loops not to take 100% cpu
- (P3) Refacctorize Perfmon

GRAPHICS:
- (P1) Check GL extensions and max GL features
- (P1) Improve shader system to be easier to use
- (P3) Handle resize on windows
- Explore triangle vs tri-strips
- Explore interleaving vs normal

MESH:
- (P0) Interleave buffer data

FBX:
- (P2) Move loading to Job system

FONTS:
- (P3) Improve font packing with an algorithm like http://clb.demon.fi/files/RectangleBinPack.pdf
- (P2) Font rendering probably needs some optimization (pack all draw with same font, ...)

IO
- (P1) Async file loading
- (P3) Use a ressource file instead of exposing resource folder
