call "C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\vcvarsall.bat" x86_amd64
IF NOT EXIST build mkdir build
cd build
qmake.exe -spec win32-msvc2013 "CONFIG += x86_64" -r ../RtspPlayer/RtspPlayer.pro
nmake
