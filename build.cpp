#include "build.h"
using namespace std;

void read_directory(const string& name, vector<string>& v)
{
    DIR* dirp = opendir(name.c_str());
    struct dirent * dp;
    while ((dp = readdir(dirp)) != NULL) {
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
    cout << proj_path << endl;
    cout << obj_path_bp << endl;
    cout << obj_path_ef << endl;
    cout << name << endl;

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
    fout_h << "class " << name << " : public obj;" << endl;

    fout_h << "#include\".\\..\\..\\objs.h\"" << endl;
    fout_h << "#include\".\\..\\..\\lvls.h\"" << endl;
    fout_h << "#include\".\\..\\..\\scripts.h\"" << endl;
    fout_h << "#include\".\\..\\..\\vars.h\"" << endl;
    fout_h << "#include\".\\..\\..\\imgs.h\"" << endl;
    fout_h << "#include\".\\..\\..\\stl.h\"" << endl;

    fout_h << "class " << name << " : public obj" << endl;
    fout_h << "{" << endl;
    fout_h << "public:" << endl;

    fout_cpp << "#include\"" << name << ".h" << "\"" << endl;

    ifstream field_in;
    field_in.open((obj_path_ef + "\\fields.cpp").c_str());
    if (field_in.is_open())
        cout << 33 << endl;
    cout << 3 << endl;
    while (!field_in.eof())
    {
        char c = field_in.get();
        cout << c;
        if ((int)c != -1)
            fout_h << c;
    }
    cout << 4 << endl;
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

void build_lvls(string& proj_path){};
void build_scripts(string& proj_path){};
void build_vars(string& proj_path){};
void build_stl(string& proj_path){};
void build_imgs(string& proj_path){};
