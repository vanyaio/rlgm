#include "build.h"
using namespace std;

void read_directory(const string& name, vector<string>& v)
{
    DIR* dirp = opendir(name.c_str());
    struct dirent * dp;
    while ((dp = readdir(dirp)) != NULL)
    {
        v.push_back(dp->d_name);
    }
    closedir(dirp);
}
double directory_delete(const char *pathname)
{
    string str(pathname);
    if (!str.empty())
    {
        while (*str.rbegin() == '\\' || *str.rbegin() == '/')
        {
            str.erase(str.size()-1);
        }
    }
    replace(str.begin(),str.end(),'/','\\');

    struct stat sb;
    if (stat((char *)str.c_str(),&sb) == 0 &&
            S_ISDIR(sb.st_mode))
    {
        HANDLE hFind;
        WIN32_FIND_DATA FindFileData;

        TCHAR DirPath[MAX_PATH];
        TCHAR FileName[MAX_PATH];

        _tcscpy(DirPath,(char *)str.c_str());
        _tcscat(DirPath,"\\*");
        _tcscpy(FileName,(char *)str.c_str());
        _tcscat(FileName,"\\");

        hFind = FindFirstFile(DirPath,&FindFileData);
        if (hFind == INVALID_HANDLE_VALUE) return 0;
        _tcscpy(DirPath,FileName);

        bool bSearch = true;
        while (bSearch)
        {
            if (FindNextFile(hFind,&FindFileData))
            {
                if (!(_tcscmp(FindFileData.cFileName,".") &&
                        _tcscmp(FindFileData.cFileName,".."))) continue;
                _tcscat(FileName,FindFileData.cFileName);
                if ((FindFileData.dwFileAttributes &
                        FILE_ATTRIBUTE_DIRECTORY))
                {
                    if (!directory_delete(FileName))
                    {
                        FindClose(hFind);
                        return 0;
                    }
                    RemoveDirectory(FileName);
                    _tcscpy(FileName,DirPath);
                }
                else
                {
                    if (FindFileData.dwFileAttributes &
                            FILE_ATTRIBUTE_READONLY)
                        _chmod(FileName, _S_IWRITE);

                    if (!DeleteFile(FileName))
                    {
                        FindClose(hFind);
                        return 0;
                    }
                    _tcscpy(FileName,DirPath);
                }
            }
            else
            {
                if (GetLastError() == ERROR_NO_MORE_FILES)
                    bSearch = false;
                else
                {
                    FindClose(hFind);
                    return 0;
                }
            }
        }
        FindClose(hFind);

        return (double)(RemoveDirectory((char *)str.c_str()) == true);
    }
    else
    {
        return 0;
    }
}

void build(string& proj_path)
{
    string built_proj = proj_path + "\\built_project";

    directory_delete(built_proj.c_str());
    CreateDirectory(built_proj.c_str(), nullptr);

    build_objs(proj_path);
    build_lvls(proj_path);
    build_scripts(proj_path);
    build_vars(proj_path);
    build_stl(proj_path);
    build_imgs(proj_path);
}

void build_objs(string& proj_path)
{
    string eng_files = proj_path + "\\engine_files";
    string built_proj = proj_path + "\\built_project";

    CreateDirectory((built_proj + "\\objs").c_str(), nullptr);
    CreateFile((built_proj + "\\objs.h").c_str(),
               GENERIC_WRITE | GENERIC_READ,
               FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE,
               0,
               OPEN_ALWAYS,
               FILE_ATTRIBUTE_NORMAL,
               0);

    //create file with base class object
    //below are problems, need to better understand inheritance
    CreateFile((built_proj + "\\obj.h").c_str(),
               GENERIC_WRITE | GENERIC_READ,
               FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE,
               0,
               OPEN_ALWAYS,
               FILE_ATTRIBUTE_NORMAL,
               0);
    ofstream fout_obj_h;
    fout_obj_h.open((built_proj + "\\obj.h").c_str());
    fout_obj_h << "#ifndef obj_h" << endl;
    fout_obj_h << "#define obj_h" << endl;
    fout_obj_h << "class obj{" << endl;
    fout_obj_h << "public:" << endl;
    fout_obj_h << "}" << endl;
    fout_obj_h << "#endif" << endl;

    CreateFile((built_proj + "\\obj.cpp").c_str(),
               GENERIC_WRITE | GENERIC_READ,
               FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE,
               0,
               OPEN_ALWAYS,
               FILE_ATTRIBUTE_NORMAL,
               0);
    //

    vector<string> objs_list;
    read_directory((eng_files + "\\objs\\").c_str(), objs_list);
    for (auto obj_folder : objs_list)
    {
        if (obj_folder == "." || obj_folder == "..")
            continue;

        string obj_path_bp = built_proj + "\\objs\\" + obj_folder;
        string obj_path_ef = eng_files + "\\objs\\" + obj_folder;
        CreateDirectory(obj_path_bp.c_str(), nullptr);

        build_obj(proj_path, obj_path_bp, obj_path_ef, obj_folder);
    }
}

void build_obj(string& proj_path, string& obj_path_bp, string& obj_path_ef, string& name)
{
    string eng_files = proj_path + "\\engine_files";
    string built_proj = proj_path + "\\built_project";

    ofstream fout_objs_h;
    fout_objs_h.open((built_proj + "\\objs.h").c_str(), ofstream::app);//*NEED TO ADD EXTENSION FLAG!!!
    fout_objs_h << "#include\".\\objs\\" << name << "\\" << name << ".h" << "\"" << endl;//*include".\\objs\\name\\name.h"
    fout_objs_h.close();

    string obj_h = obj_path_bp + "\\" + name + ".h";//*check for // in path
    CreateFile(obj_h.c_str(),
               GENERIC_WRITE | GENERIC_READ,
               FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE,
               0,
               OPEN_ALWAYS,
               FILE_ATTRIBUTE_NORMAL,
               0);
    string obj_cpp = obj_path_bp + "\\" + name + ".cpp";
    CreateFile(obj_cpp.c_str(),
               GENERIC_WRITE | GENERIC_READ,
               FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE,
               0,
               OPEN_ALWAYS,
               FILE_ATTRIBUTE_NORMAL,
               0);

    ofstream fout_h;
    fout_h.open(obj_h.c_str());
    ofstream fout_cpp;
    fout_cpp.open(obj_cpp.c_str());

    fout_h << "#ifndef " << name << "_h" << endl;
    fout_h << "#define " << name << "_h" << endl << endl;

    fout_h << "#include\".\\..\\..\\obj.h\"" << endl;//include".\\..\\..\\obj.h"
    fout_h << "class " << name << ";" << endl;

    fout_h << "#include\".\\..\\..\\objs.h\"" << endl;
    fout_h << "#include\".\\..\\..\\lvls.h\"" << endl;
    fout_h << "#include\".\\..\\..\\imgs.h\"" << endl;
    fout_h << "#include\".\\..\\..\\stl.h\"" << endl;

    fout_h << "class " << name << " : public obj" << endl;
    fout_h << "{" << endl;
    fout_h << "public:" << endl;

    fout_cpp << "#include\"" << name << ".h" << "\"" << endl;
    fout_cpp << "#include\".\\..\\..\\vars.h\"" << endl;
    fout_cpp << "#include\".\\..\\..\\scripts.h\"" << endl;

    ifstream field_in;
    field_in.open((obj_path_ef + "\\fields.cpp").c_str());
    while (!field_in.eof())
    {
        char c = field_in.get();
        cout << c;
        if ((int)c != -1)
            fout_h << c;
    }
    fout_h << endl;
    field_in.close();

    vector<string> obj_def;
    read_directory(obj_path_ef, obj_def);
    for (auto obj_method : obj_def)
    {
        if (obj_method == "." || obj_method == ".." || obj_method == "fields.cpp")
            continue;

        ifstream method_in;
        method_in.open(obj_path_ef + "\\" + obj_method);

        bool endl_met = false;
        while ( !endl_met)
        {
            char x = method_in.get();

            if (endl_met == false && x == '\n')
                endl_met = true;
            if (!endl_met)
                fout_h << x;
        }
        //CAREFUL!! FILE MAY CONTAIN ENDL IN BEGIN
        //THERE ARE PROBLEMS WITH NAMESPACES
        while( !method_in.eof() )
        {
            char x = method_in.get();
            if ((int)x != -1)
                fout_cpp << x;
        }
    }

    fout_h << "};" << endl;
    fout_h << "#endif" << endl;
    fout_h.close();
    fout_cpp.close();
}

void build_lvls(string& proj_path)
{
    string eng_files = proj_path + "\\engine_files";
    string built_proj = proj_path + "\\built_project";

    CreateDirectory((built_proj + "\\lvls").c_str(), nullptr);
    CreateFile((built_proj + "\\lvls.h").c_str(),
               GENERIC_WRITE | GENERIC_READ,
               FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE,
               0,
               OPEN_ALWAYS,
               FILE_ATTRIBUTE_NORMAL,
               0);

    CreateFile((built_proj + "\\lvl.h").c_str(),
               GENERIC_WRITE | GENERIC_READ,
               FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE,
               0,
               OPEN_ALWAYS,
               FILE_ATTRIBUTE_NORMAL,
               0);
    ofstream fout_lvl_h;
    fout_lvl_h.open((built_proj + "\\lvl.h").c_str());
    fout_lvl_h << "#ifndef lvl_h" << endl;
    fout_lvl_h << "#define lvl_h" << endl;
    fout_lvl_h << "class lvl{" << endl;
    fout_lvl_h << "public:" << endl;
    fout_lvl_h << "}" << endl;
    fout_lvl_h << "#endif" << endl;

    CreateFile((built_proj + "\\lvl.cpp").c_str(),
               GENERIC_WRITE | GENERIC_READ,
               FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE,
               0,
               OPEN_ALWAYS,
               FILE_ATTRIBUTE_NORMAL,
               0);
    //

    vector<string> lvls_list;
    read_directory((eng_files + "\\lvls\\").c_str(), lvls_list);
    for (auto lvl_folder : lvls_list)
    {
        if (lvl_folder == "." || lvl_folder == "..")
            continue;

        string lvl_path_bp = built_proj + "\\lvls\\" + lvl_folder;
        string lvl_path_ef = eng_files + "\\lvls\\" + lvl_folder;
        CreateDirectory(lvl_path_bp.c_str(), nullptr);

        build_lvl(proj_path, lvl_path_bp, lvl_path_ef, lvl_folder);
    }
};


void build_lvl(string& proj_path, string& lvl_path_bp, string& lvl_path_ef, string& name)
{
    string eng_files = proj_path + "\\engine_files";
    string built_proj = proj_path + "\\built_project";

    ofstream fout_lvls_h;
    fout_lvls_h.open((built_proj + "\\lvls.h").c_str(), ofstream::app);
    fout_lvls_h << "#include\".\\lvls\\" << name << "\\" << name << ".h" << "\"" << endl;//*include".\\lvls\\name\\name.h"
    fout_lvls_h.close();

    string lvl_h = lvl_path_bp + "\\" + name + ".h";//*check for // in path
    CreateFile(lvl_h.c_str(),
               GENERIC_WRITE | GENERIC_READ,
               FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE,
               0,
               OPEN_ALWAYS,
               FILE_ATTRIBUTE_NORMAL,
               0);
    string lvl_cpp = lvl_path_bp + "\\" + name + ".cpp";
    CreateFile(lvl_cpp.c_str(),
               GENERIC_WRITE | GENERIC_READ,
               FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE,
               0,
               OPEN_ALWAYS,
               FILE_ATTRIBUTE_NORMAL,
               0);

    ofstream fout_h;
    fout_h.open(lvl_h.c_str());
    ofstream fout_cpp;
    fout_cpp.open(lvl_cpp.c_str());

    fout_h << "#ifndef " << name << "_h" << endl;
    fout_h << "#define " << name << "_h" << endl << endl;

    fout_h << "#include\".\\..\\..\\lvl.h\"" << endl;//include".\\..\\..\\lvl.h"
    fout_h << "class " << name << ";" << endl;

    fout_h << "#include\".\\..\\..\\objs.h\"" << endl;
    fout_h << "#include\".\\..\\..\\lvls.h\"" << endl;
    fout_h << "#include\".\\..\\..\\imgs.h\"" << endl;
    fout_h << "#include\".\\..\\..\\stl.h\"" << endl;

    fout_h << "class " << name << " : public lvl" << endl;
    fout_h << "{" << endl;
    fout_h << "public:" << endl;

    fout_cpp << "#include\"" << name << ".h" << "\"" << endl;
    fout_cpp << "#include\".\\..\\..\\vars.h\"" << endl;
    fout_cpp << "#include\".\\..\\..\\scripts.h\"" << endl;

    ifstream field_in;
    field_in.open((lvl_path_ef + "\\fields.cpp").c_str());
    while (!field_in.eof())
    {
        char c = field_in.get();
        cout << c;
        if ((int)c != -1)
            fout_h << c;
    }
    fout_h << endl;
    field_in.close();

    vector<string> lvl_def;
    read_directory(lvl_path_ef, lvl_def);
    for (auto lvl_method : lvl_def)
    {
        if (lvl_method == "." || lvl_method == ".." || lvl_method == "fields.cpp")
            continue;

        ifstream method_in;
        method_in.open(lvl_path_ef + "\\" + lvl_method);

        bool endl_met = false;//CAREFUL!! FILE MAY CONTAIN ENDL IN BEGIN
        //THERE ARE PROBLEMS WITH NAMESPACES
        while( !method_in.eof() )
        {
            char x = method_in.get();

            if (endl_met == false && x == '\n')
                endl_met = true;
            if (!endl_met)
                fout_h << x;
            if ((int)x != -1)
                fout_cpp << x;
        }
        fout_h << ";" << endl;
    }

    fout_h << "};" << endl;
    fout_h << "#endif" << endl;
    fout_h.close();
    fout_cpp.close();
}
void build_scripts(string& proj_path)
{
    string eng_files = proj_path + "\\engine_files";
    string built_proj = proj_path + "\\built_project";

    CreateDirectory((built_proj + "\\scripts").c_str(), nullptr);
    CreateFile((built_proj + "\\scripts.h").c_str(),
               GENERIC_WRITE | GENERIC_READ,
               FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE,
               0,
               OPEN_ALWAYS,
               FILE_ATTRIBUTE_NORMAL,
               0);

    vector<string> scripts_list;
    read_directory((eng_files + "\\scripts\\").c_str(), scripts_list);
    for (auto script : scripts_list)
    {
        if (script == "." || script == "..")
            continue;

        string script_path_bp = built_proj + "\\scripts";
        string script_path_ef = eng_files + "\\scripts";

        build_script(proj_path, script_path_bp, script_path_ef, script);
    }
};

void build_script(string& proj_path, string& script_path_bp, string& script_path_ef, string& name_cpp)
{
    string eng_files = proj_path + "\\engine_files";
    string built_proj = proj_path + "\\built_project";

    string name = "";
    for (int i = 0; i < name_cpp.length() - 4; i++)
        name = name + (name_cpp[i]);

    ofstream fout_scripts_h;
    fout_scripts_h.open((built_proj + "\\scripts.h").c_str(), ofstream::app);
    fout_scripts_h << "#include\".\\scripts\\" << name << ".h" << "\"" << endl;
    fout_scripts_h.close();

    string script_h = script_path_bp + "\\" + name + ".h";//*check for // in path
    CreateFile(script_h.c_str(),
               GENERIC_WRITE | GENERIC_READ,
               FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE,
               0,
               OPEN_ALWAYS,
               FILE_ATTRIBUTE_NORMAL,
               0);
    string script_cpp = script_path_bp + "\\" + name + ".cpp";
    CreateFile(script_cpp.c_str(),
               GENERIC_WRITE | GENERIC_READ,
               FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE,
               0,
               OPEN_ALWAYS,
               FILE_ATTRIBUTE_NORMAL,
               0);

    ofstream fout_h;
    fout_h.open(script_h.c_str());
    ofstream fout_cpp;
    fout_cpp.open(script_cpp.c_str());

    fout_h << "#ifndef " << name << "_h" << endl;
    fout_h << "#define " << name << "_h" << endl << endl;

    fout_h << "#include\".\\..\\objs.h\"" << endl;
    fout_h << "#include\".\\..\\lvls.h\"" << endl;
    fout_h << "#include\".\\..\\imgs.h\"" << endl;
    fout_h << "#include\".\\..\\stl.h\"" << endl;

    fout_cpp << "#include\"" << name << ".h" << "\"" << endl;
    fout_cpp << "#include\".\\..\\vars.h\"" << endl;
    fout_cpp << "#include\".\\..\\scripts.h\"" << endl;

    ifstream script_in;
    script_in.open(script_path_ef + "\\" + name_cpp);

    bool endl_met = false;//CAREFUL!! FILE MAY CONTAIN ENDL IN BEGIN
    while(!script_in.eof())
    {
        char x = script_in.get();

        if (endl_met == false && x == '\n')
            endl_met = true;
        if (!endl_met)
            fout_h << x;
        if ((int)x != -1)
            fout_cpp << x;
    }
    fout_h << ";" << endl;

    fout_h << "#endif" << endl;
    fout_h.close();
    fout_cpp.close();
}

void build_vars(string& proj_path)
{
    string eng_files = proj_path + "\\engine_files";
    string built_proj = proj_path + "\\built_project";

    CreateFile((built_proj + "\\vars.h").c_str(),
               GENERIC_WRITE | GENERIC_READ,
               FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE,
               0,
               OPEN_ALWAYS,
               FILE_ATTRIBUTE_NORMAL,
               0);
    ofstream fout_h;
    fout_h.open((built_proj + "\\vars.h").c_str());
    fout_h << "#ifndef vars_h" << endl;
    fout_h << "#define vars_h" << endl;
    fout_h << "#include\"objs.h\"" << endl;
    fout_h << "#include\"lvls.h\"" << endl;
    fout_h << "#include\"imgs.h\"" << endl;
    fout_h << "#include\"stl.h\"" << endl;

    CreateFile((built_proj + "\\vars.cpp").c_str(),
               GENERIC_WRITE | GENERIC_READ,
               FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE,
               0,
               OPEN_ALWAYS,
               FILE_ATTRIBUTE_NORMAL,
               0);
    ofstream fout_cpp;
    fout_cpp.open((built_proj + "\\vars.cpp").c_str());
    fout_cpp << "#include\"vars.h\"" << endl;
    fout_cpp << "#include\"scripts.h\"" << endl;

    ifstream vars_in;
    vars_in.open(eng_files + "\\vars\\vars.cpp");
    //
    //fout_h << extern type varname; fout_cpp << type varname = ...;
    //
    if (vars_in.is_open())
        cout << "hey" << endl;
    bool in_process = true;
    bool newline = true;
    while (!vars_in.eof())
    {
        cout << "hello" << endl;
        char x = vars_in.get();
        if ((int)x == -1)
            break;

        fout_cpp << x;
        if (x == '\n')
        {
            newline = true;
            in_process = true;
            continue;
        }
        if (newline)
        {
            newline = false;
            fout_h << "extern " << x;
            continue;
        }
        if (in_process)
        {
            if (x == '=' || x == '(' || x == ';')
            {
                fout_h << ";" << endl;
                in_process = false;
            }
            else
                fout_h << x;
        }
    }

    fout_h << "#endif" << endl;
};

void build_imgs(string& proj_path)//test
{
    string eng_files = proj_path + "\\engine_files";
    string built_proj = proj_path + "\\built_project";

    CreateDirectory((built_proj + "\\imgs").c_str(), nullptr);

    vector<string> imgs_list;
    read_directory((eng_files + "\\imgs\\").c_str(), imgs_list);
    for (auto img : imgs_list)
    {
        if (img == "." || img == "..")
            continue;

        ifstream img_in;
        img_in.open((eng_files + "\\imgs\\" + img).c_str());
        ofstream img_out;
        if (img != "img.h" && img != "img.cpp")
        {
            CreateFile((built_proj + "\\imgs\\" + img).c_str(),
                   GENERIC_WRITE | GENERIC_READ,
                   FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE,
                   0,
                   OPEN_ALWAYS,
                   FILE_ATTRIBUTE_NORMAL,
                   0);

            img_out.open((built_proj + "\\imgs\\" + img).c_str());
        }
        else
        {
            CreateFile((built_proj + "\\" + img).c_str(),
                   GENERIC_WRITE | GENERIC_READ,
                   FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE,
                   0,
                   OPEN_ALWAYS,
                   FILE_ATTRIBUTE_NORMAL,
                   0);

            img_out.open((built_proj + "\\" + img).c_str());
        }

        while( !img_in.eof() )
        {
            char x = img_in.get();
            if ((int)x != -1)
                img_out << x;
        }

        img_in.close();
        img_out.close();
    }


};
void build_stl(string& proj_path) {};
