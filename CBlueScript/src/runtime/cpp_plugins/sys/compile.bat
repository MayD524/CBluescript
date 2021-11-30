ECHO OFF
DEL *.o
DEL bs_sys.dll
DEL C:\Users\Cross\Desktop\bs_release\bs_sys.dll
g++ -std=c++17 -c *.cpp
g++ -shared -static -o3 -o bs_sys.dll *.o 
COPY bs_sys.dll C:\Users\Cross\Desktop\bs_release\bs_sys.dll