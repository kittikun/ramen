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

TODO:

- Improve profiler with many level of hierarchy
- Handle resize on windows
- Loop to not take 100% cpu
- Improve font packing with an algorithm like http://clb.demon.fi/files/RectangleBinPack.pdf
