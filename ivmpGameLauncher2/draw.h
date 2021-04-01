#include <d3d9.h>

void setDevice(IDirect3DDevice9* g_pD3DDevice);
void Line(float x1, float y1, float x2, float y2, float width, bool antialias, DWORD color);
 
void Box(float x, float y, float w, float h, float linewidth, DWORD color);
void BoxFilled(float x, float y, float w, float h, DWORD color);
void BoxBordered(float x, float y, float w, float h, float border_width, DWORD color, DWORD color_border);
void BoxRounded(float x, float y, float w, float h,float radius, bool smoothing, DWORD color, DWORD bcolor);
 
void Circle(float x, float y, float radius, int rotate, int type, bool smoothing, int resolution, DWORD color);
void CircleFilled(float x, float y, float rad, float rotate, int type, int resolution, DWORD color);

void Panel(RECT r);