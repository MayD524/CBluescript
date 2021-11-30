ECHO OFF
SET currentdir=%CD%
ECHO Current directory is %currentdir%
CD C:\Users\Cross\Desktop\CBluescript_repo\CBlueScript\src\runtime
ECHO Current directory is %CD%
ECHO Deleting old files...
DEL *.exe
ECHO Deleted old files.
ECHO Compiling...
g++ -std=c++17 -static -o3 -o bluescript.exe *.cpp
ECHO Compiled.
ECHO Returning to original directory...
CD %currentdir%
ECHO Finished happy coding!
COPY bluescript.exe C:\Users\Cross\Desktop\bs_release\bluescript.exe