@echo off
::cls
cd .\build
::mingw32-make clean
cmake -G "MinGW Makefiles" ..
mingw32-make
echo ===demo running===
.\demo.exe
cd ..
