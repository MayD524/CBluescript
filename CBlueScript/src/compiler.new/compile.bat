@ECHO OFF
@SETLOCAL

set start=%time%

ECHO Cleaning up old files...
DEL cbs.exe
ECHO Compiling...
g++ -std=c++17 -static -O0 -o cbs.exe *.cpp