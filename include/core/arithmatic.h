//https://www.codementor.io/a_hathon/building-and-using-dlls-in-c-d7rrd4caz

#define EXPORT __declspec(dllexport)

EXPORT int window();
EXPORT void connect();
EXPORT int randNum();
EXPORT int addNum(int a, int b);