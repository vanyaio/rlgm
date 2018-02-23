#include <iostream>
#include <fstream>
#include "build.h"
using namespace std;

int main()
{
    string s1 = ".\\projects\\test";
    string s2 = ".\\projects\\test\\built_project";
    string s3 = ".\\projects\\test\\engine_files";
    string s4 = "obj1";
    //build_obj(s1, s2, s3, s4);

    string built_proj = s1 + "\\built_project";

    directory_delete(built_proj.c_str());
    CreateDirectory(built_proj.c_str(), nullptr);
    //build_lvls(s1);
}
