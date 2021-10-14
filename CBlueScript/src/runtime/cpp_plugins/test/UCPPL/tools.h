/*
    Author: May Draskovics
    Date: 9/6/2021
*/
#pragma once

#ifndef TOOLS_H
#define TOOLS_H

#define MAX_STRING_SIZE 256

#if defined _WIN32
    #define CURRENT_OS "WIN32"
    #define GETCHAR() _getch
    #include <conio.h>

#elif defined (__LINUX__) || defined(__gnu_linux__) || defined(__linux__)
    #define CURRENT_OS "LINUX"
    #define GETCHAR() getch
#elif defined(__APPLE__)
    #define CURRENT_OS "APPLE"
#endif

#include <iostream>
#include <stdarg.h>
#include <fstream>
#include <vector>
#include <string>
#include <locale>

using namespace std;


template <typename T>
inline void removeByIndex(vector<T>& vec, size_t pos)
{
    typename vector<T>::iterator it = vec.begin();
    advance(it, pos);
    vec.erase(it);
}

// trim from left
inline string& ltrim(string& s, const char* t = " \t\n\r\f\v")
{
    s.erase(0, s.find_first_not_of(t));
    return s;
}

// trim from right
inline string& rtrim(string& s, const char* t = " \t\n\r\f\v")
{
    s.erase(s.find_last_not_of(t) + 1);
    return s;
}

// trim from left & right
inline string& trim(string& s, const char* t = " \t\n\r\f\v")
{
    return ltrim(rtrim(s, t), t);
}

inline void eraseSubStr(string &mainStr, const string& toErase)
{
    size_t pos = mainStr.find(toErase);
    
    if (pos != string::npos)
        mainStr.erase(pos, toErase.length());
}

inline bool is_number(string s)
{
    eraseSubStr(s, ".");
    string::const_iterator it = s.begin();
    while(it != s.end() && isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}

inline bool string_contains(const string& str, const char lookfor)
{
    for (auto it = str.cbegin(); it != str.cend(); it++)
        if (*it == lookfor)
            return true;

    return false;
}

inline void termClear( void )
{
    if (CURRENT_OS == "WIN32") { system("cls"); }
    else if (CURRENT_OS == "LINUX" || CURRENT_OS == "APPLE") { system("clear"); }
}

#ifdef _WIN32
inline string getPass( const string prompt=">", int maxStringSize = MAX_STRING_SIZE)
{
    string pass = "";
    char ch;
    int size = 0;
    cout << prompt;

    ch = _getch();
    while (ch != 13 || size > maxStringSize)
    {
        pass.push_back(ch);
        cout << "*";
        ch = _getch();
        size++;
    }

    return pass;
}
#endif

template<typename lookType>
inline bool anyInVector(const lookType& lookfor, const vector<lookType>& lookIn)
{
    for (lookType l : lookIn)
    {
        if (l == lookfor)
            return true;
    }
    return false;
}

inline bool replace(string& str, const string& from, const string& to)
{
    size_t start_pos = str.find(from);
    if (start_pos == string::npos)
        return false;
    str.replace(start_pos, from.length(), to);
    return true;
}

inline string toLower( const string inpt)
{
    locale loc;
    string retStr = "";
    for (string::size_type i = 0; i < inpt.size(); i++)
        retStr += tolower(inpt[i], loc);

    return retStr;
}

inline string strToHex( const string inpt)
{
    const char* cStr = inpt.c_str();
    string retStr = "";

    for(int i = 0; i < inpt.size(); i++)
    {
        cout << cStr[i] << " " <<(int) cStr[i] << endl;
    }

    return retStr;
}

inline string hexToString( const string hexStr)
{
    const char* cHexStr = hexStr.c_str();
    string retStr = "";

    return retStr;
}

inline string toUpper( const string inpt)
{
    locale loc;
    string retStr = "";
    for (string::size_type i = 0; i < inpt.size(); i++)
        retStr += toupper(inpt[i], loc);
    
    return retStr;
}

// split string by delim x times (set count to 99999 as i doubt you'll split that many times)
inline vector<string> split(string str, string delim, bool oneSplit=false)
{
    vector<string> split_str;
    string token;
    size_t pos = 0;

    while((pos = str.find(delim)) != string::npos)
    {
        token = str.substr(0, pos);
        split_str.push_back(token);
        str.erase(0, pos + delim.length());
        
        if (oneSplit == true)
            break;
    }
    split_str.push_back(str);

    return split_str;
}

inline int writeFile(string filename, vector<string> data, bool autoNewLine)
{
    ofstream _file(filename);

    if (_file.is_open())
    {
        for (string& d : data)
        {
            if (!autoNewLine)
                _file << d;
            else
                _file << d << endl;
        }

        _file.close();
        return 0;
    }

    return 1;
}

inline string uInput(string prompt=">")
{
    string _userinp = "";
    cout << prompt;
    getline(cin, _userinp);
    return _userinp;
}

inline vector<string> readFile(string Filename)
{
    vector<string> lines;
    ifstream file_to_read(Filename);

    if (file_to_read.is_open())
    {
        string line;
        while(getline(file_to_read, line))
        {
            if (line == "") { continue; }
            lines.push_back(line);
        }
        file_to_read.close();
        return lines;
    }

    return lines;
}
#endif