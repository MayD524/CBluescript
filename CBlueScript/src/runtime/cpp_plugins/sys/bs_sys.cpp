#define BUILD_DLL
#include "bs_sys.h"

// get current directory
string getWorkingDirectory( void )
{
    #ifdef _WIN32
        // windows get current directory name
        char buffer[MAX_PATH];
        GetCurrentDirectoryA(MAX_PATH, buffer);
        return string(buffer);
    #else
        // linux get current directory name
        char buffer[PATH_MAX];
        getcwd(buffer, PATH_MAX);
        return string(buffer);
    #endif
}

HEADER getCWD(bs_memory* mem, const string& args)
{
    /*
        cbs: cwd retvar
    */
    
    mem->bsMovCmd(args, getWorkingDirectory());
}

HEADER listDir(bs_memory* mem, const string& args)
{
    /*
      cbs: lsdir (%)path,retvar
      returns array
    */
    vector<string> args_split = split(args, ",");
    vector<string> files;
    bsMemoryObject tmp = mem->bs_NULL();

    string dir = args_split[0];
    string retVar = args_split[1];
    
    if (dir.rfind("%", 0) == 0)
    {
        dir = split(dir, "%")[1];
        if (mem->varExists(dir))
            tmp = mem->getVar(dir);
        

        dir = tmp.__str_reper__;
    }

    if (retVar.rfind("%", 0) == 0)
    {
        retVar = split(retVar, "%")[1];
        if (mem->varExists(retVar))
            tmp = mem->getVar(retVar);
        

        retVar = tmp.__str_reper__;
    }
    
    if (dir.empty() || dir.compare("./") == 0)
        dir = getWorkingDirectory();

    // this is only because g++ is stupid
    // and wont let me use the std::filesystem
    #ifdef _WIN32
        dir = dir + "\\*"; // we need to ask for dir content
        WIN32_FIND_DATA data;
        // windows is stupid
        HANDLE hFind = FindFirstFile(dir.c_str(), &data);
        if ( hFind != INVALID_HANDLE_VALUE )
        {
            do
            {
                // convert to string
                files.push_back(split(dir, "*")[0] + string(data.cFileName));
            }
            while (FindNextFile(hFind, &data));
            FindClose(hFind);
        }

    #else
        #include <filesystem>
        for (auto& p : std::filesystem::directory_iterator(dir))
            files.push_back(p.path().filename().string());
    #endif

    for (int i = 0; i < files.size(); i++)
    {
        bsMemoryObject file = mem->createObject(files[i]);
        mem->stackPush(file);
        //mem->arrayAppend(retVar, i, mem->createObject(files[i]));
    }

    bsMemoryObject ret = mem->createObject(to_string(files.size()));
    mem->putObjInMemory(retVar, ret);
}


HEADER load_lib(map<string, void(*)(bs_memory* mem, const string& args)>* rt)
{
    map<string, void(*)(bs_memory* mem, const string& args)> retmap;
    
    retmap["lsdir"] = &listDir;
    retmap["cwd"] = &getCWD;

    rt->insert(retmap.begin(), retmap.end());

}
