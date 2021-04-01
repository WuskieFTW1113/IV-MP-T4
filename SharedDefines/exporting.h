#ifndef exporting
#define exporting
#define linux 0
#if linux
#define DLL
#else
#define DLL __declspec(dllexport)
#endif
#endif