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
    build_obj(s1, s2, s3, s4);
}
