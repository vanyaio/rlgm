 #ifndef img_h
 #define img_h

 #include<string>
 #include<fstream>
 using namespace std;

 class pixel
 {
 public:
    char sign;
    int color;
    pixel();
    pixel(char _sing, int _color);
 };

 class img
 {
 public:
    pixel** arr;
    int x;
    int y;
    img();
    ~img();
    img(const img& a);
    img& operator=(const img& a);

    img(string img_path);
 };

 #endif // img_h
