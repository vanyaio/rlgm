#include "build.h"

void read_directory(const string& name, vector<string>& v)
{
    DIR* dirp = opendir(name.c_str());
    struct dirent * dp;
    while ((dp = readdir(dirp)) != NULL) {
        v.push_back(dp->d_name);
    }
    closedir(dirp);
}

void build(string& proj_path)
{
    string eng_files = proj_path + "\\engine_files";
    string built_proj = proj_path + "\\built_project";

    RemoveDirectory(built_proj.c_str());//what if directory does not exist??? maybe create new every time??
    CreateDirectory(built_proj.c_str(), nullptr);

    void build_objs(proj_path);
    void build_lvls(proj_path);
    void build_scripts(proj_path);
    void build_vars(proj_path);
    void build_stl(proj_path);
    void build_imgs(proj_path);
}

void build_objs(string& proj_path);
{
    string eng_files = proj_path + "\\engine_files";
    string built_proj = proj_path + "\\built_project";

    CreateDirectory((built_proj + "\\objs").c_str(), nullptr);
    CreateFile((built_proj + "\\objs.h").c_str(),
                          GENERIC_WRITE | GENERIC_READ,
                          FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE,             // sharing mode, none in this case FILE_SHARE_DELETE
                          0,
                          OPEN_ALWAYS,
                          FILE_ATTRIBUTE_NORMAL,
                          0);
    //create file with base class object

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

void build_obj(string& proj_path, string& obj_path_bp, string& obj_path_ef, string& name);
{
    string eng_files = proj_path + "\\engine_files";
    string built_proj = proj_path + "\\built_project";

    ofstream fout_objs_h;
    fout_objs_h.open((built_proj + "\\objs.h").c_str());//NEED TO ADD EXTENSION FLAG!!!
    fout_objs_h << "#include<" << name << ">" << endl;
    fout_objs_h.close();

    string obj_h_path = obj_path_bp + name + ".h";//check for // in path
    CreateFile(obj_h_path.c_str(),
                          GENERIC_WRITE | GENERIC_READ,
                          FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE,             // sharing mode, none in this case FILE_SHARE_DELETE
                          0,
                          OPEN_ALWAYS,
                          FILE_ATTRIBUTE_NORMAL,
                          0);
    string obj_cpp_path = obj_path_bp + name + ".cpp";
    CreateFile(obj_cpp_path.c_str(),
                          GENERIC_WRITE | GENERIC_READ,
                          FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE,             // sharing mode, none in this case FILE_SHARE_DELETE
                          0,
                          OPEN_ALWAYS,
                          FILE_ATTRIBUTE_NORMAL,
                          0);

    ofstream fout_h;//don't forget to close
    fout_h.open(obj_h_path);//.c_str()?
    ofstream fout_cpp;
    fout_cpp.open(obj_cpp_path);//.c_str()?

    fout_h << "#ifndef " << name << "_h" << endl;
    fout_h << "#define " << name << "_h" << endl << endl;
    fout_h << "class " << name

    vector<string> obj_def;
    read_directory(obj_path_ef, obj_def);
    for (auto obj_method : obj_def)
    {
        if (obj_method == "." || obj_method == ".." || obj_method == "fields.cpp")
            continue;


    }
}
