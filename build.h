#ifndef BUILD_H
#define BUILD_H

#include <windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sys/types.h>
#include <dirent.h>
#include <vector>

void read_directory(const string& name, vector<string>& v);

void build_objs(string& proj_path);
void build_obj(string& proj_path, string& obj_path_bp, string& obj_path_ef, string& name);

void build_lvls(string& proj_path);
void build_scripts(string& proj_path);
void build_vars(string& proj_path);
void build_stl(string& proj_path);
void build_imgs(string& proj_path);

void build(string& proj_path);

#endif // BUILD_H
