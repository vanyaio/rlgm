#ifndef BUILD_H
#define BUILD_H

#include<windows.h>
#include<iostream>
#include<fstream>
#include<string>
#include<sys/types.h>
#include<dirent.h>
#include<vector>
#include<sys/stat.h>
#include<tchar.h>
#include<algorithm>

using namespace std;

void read_directory(const string& name, vector<string>& v);
double directory_delete(const char *pathname);

void build(string& proj_path);

void build_objs(string& proj_path);
void build_obj(string& proj_path, string& obj_path_bp, string& obj_path_ef, string& name);

void build_lvls(string& proj_path);
void build_lvl(string& proj_path, string& lvl_path_bp, string& lvl_path_ef, string& name);

void build_scripts(string& proj_path);
void build_script(string& proj_path, string& script_path_bp, string& script_path_ef, string& name_cpp);

void build_vars(string& proj_path);
void build_stl(string& proj_path);
void build_imgs(string& proj_path);

#endif // BUILD_H
