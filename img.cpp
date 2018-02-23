#include"img.h"
#include<iostream>
using namespace std;

pixel::pixel()
{
    sign = ' ';
    color = -1;
};

pixel::pixel(char _sign, int _color = -1)
{
    sign = _sign;
    color = _color;
}

img::img()
{
    x = 0;
    y = 0;
    arr = nullptr;
}
img::~img()
{
    if (arr == nullptr)
        return;
    for (int i = 0; i < x; i++)
        delete[] arr[i];
    delete[] arr;
}
img::img(const img& a)
{
    x = a.x;
    y = a.y;
    arr = new pixel*[x];
    for (int i = 0; i < x; i++)
        arr[x] = new pixel[y];
    for (int i = 0; i < x; i++)
        for (int j = 0; j < y; j++)
            arr[i][j] = a.arr[i][j];
}
img& img::operator=(const img& a)
{
    if (arr != nullptr)
    {
        for (int i = 0; i < x; i++)
            delete[] arr[i];
        delete[] arr;
    }

    x = a.x;
    y = a.y;
    arr = new pixel*[x];
    for (int i = 0; i < x; i++)
        arr[i] = new pixel[y];
    for (int i = 0; i < x; i++)
        for (int j = 0; j < y; j++)
            arr[i][j] = a.arr[i][j];
    return *this;
}
img::img(string img_path)
{
    ifstream img_in;
    img_in.open((img_path).c_str());

    int _x, _y;
    img_in >> _x >> _y;
    x = _y;
    y = _y;

    arr = new pixel*[x];
    for (int i = 0; i < x; i++)
        arr[i] = new pixel[y];

    int cnt;
    img_in >> cnt;
    for(int i = 0; i < cnt; i++)
    {
        int _x, _y;
        char _sign;
        int _color;
        img_in >> _x >>_y >> _sign >> _color;
        arr[_x][_y] = pixel(_sign, _color);
    }
}

