ECHO OFF
ECHO Cleaning up old files...
DEL cbs.exe
ECHO Compiling...
g++ -std=c++17 -static -o3 -o cbs.exe *.cpp