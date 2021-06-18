#include "draw.h"
#include "../DirectX/Include/d3dx9core.h"
#include "../DirectX/Include/d3dx9math.h"
#include "../DirectX/Include/DxErr.h"
#include <vector>

IDirect3DDevice9* pDevice = NULL;
LPDIRECT3DVERTEXBUFFER9 g_pVB;    // Buffer to hold vertices
LPDIRECT3DINDEXBUFFER9  g_pIB;    // Buffer to hold indices


enum circle_type {full, half, quarter};
enum text_alignment {lefted, centered, righted};

void setDevice(IDirect3DDevice9* g_pD3DDevice)
{
	pDevice = g_pD3DDevice;
}

struct vertex
{
    FLOAT x, y, z, rhw;
    DWORD color;
};

void Line(float x1, float y1, float x2, float y2, float width, bool antialias, DWORD color)
{
   ID3DXLine *m_Line;
 
   D3DXCreateLine(pDevice, &m_Line);
   D3DXVECTOR2 line[] = {D3DXVECTOR2(x1, y1), D3DXVECTOR2(x2, y2)};
   m_Line->SetWidth(width);
   if(antialias) m_Line->SetAntialias(1);
   m_Line->Begin();
   m_Line->Draw(line, 2, color);
   m_Line->End();
   m_Line->Release();
}
 
void Box(float x, float y, float w, float h, float linewidth, DWORD color)
{
   if(linewidth == 0 || linewidth == 1)
   {
      BoxFilled(x, y, w, 1, color);             // Top
      BoxFilled(x, y+h-1, w, 1, color);         // Bottom
      BoxFilled(x, y+1, 1, h-2*1, color);       // Left
      BoxFilled(x+w-1, y+1, 1, h-2*1, color);   // Right
   }
   else
   {
      BoxFilled(x, y, w, linewidth, color);                                     // Top
      BoxFilled(x, y+h-linewidth, w, linewidth, color);                         // Bottom
      BoxFilled(x, y+linewidth, linewidth, h-2*linewidth, color);               // Left
      BoxFilled(x+w-linewidth, y+linewidth, linewidth, h-2*linewidth, color);   // Right
   }
}

void BoxFilled(float x, float y, float w, float h, DWORD color)
{
   vertex V[4];
 
   V[0].color = V[1].color = V[2].color = V[3].color = color;
 
   V[0].z = V[1].z = V[2].z = V[3].z = 0;
   V[0].rhw = V[1].rhw = V[2].rhw = V[3].rhw = 0;
 
   V[0].x = x;
   V[0].y = y;
   V[1].x = x+w;
   V[1].y = y;
   V[2].x = x+w;
   V[2].y = y+h;
   V[3].x = x;
   V[3].y = y+h;
 
   unsigned short indexes[] = {0, 1, 3, 1, 2, 3};
 
   pDevice->CreateVertexBuffer(4*sizeof(vertex), D3DUSAGE_WRITEONLY, D3DFVF_XYZRHW|D3DFVF_DIFFUSE, D3DPOOL_DEFAULT, &g_pVB, NULL);
   pDevice->CreateIndexBuffer(2*sizeof(short), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &g_pIB, NULL);
 
   VOID* pVertices;
   g_pVB->Lock(0, sizeof(V), (void**)&pVertices, 0);
   memcpy(pVertices, V, sizeof(V));
   g_pVB->Unlock();
 
   VOID* pIndex;
   g_pIB->Lock(0, sizeof(indexes), (void**)&pIndex, 0);
   memcpy(pIndex, indexes, sizeof(indexes));
   g_pIB->Unlock();
 
   pDevice->SetTexture(0, NULL);
   pDevice->SetPixelShader(NULL);
   pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
   pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
   pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
 
   pDevice->SetStreamSource(0, g_pVB, 0, sizeof(vertex));
   pDevice->SetFVF(D3DFVF_XYZRHW|D3DFVF_DIFFUSE);
   pDevice->SetIndices(g_pIB);
 
   pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2);
 
   g_pVB->Release();
   g_pIB->Release();
}

void BoxBordered(float x, float y, float w, float h, float border_width, DWORD color, DWORD color_border)
{
   BoxFilled(x, y, w, h, color);
   Box(x-border_width, y-border_width, w+2*border_width, h+border_width, border_width, color_border);
}

void BoxRounded(float x, float y, float w, float h,float radius, bool smoothing, DWORD color, DWORD bcolor)
{
   BoxFilled(x+radius, y+radius, w-2*radius-1, h-2*radius-1, color);   // Center rect.
   BoxFilled(x+radius, y+1, w-2*radius-1, radius-1, color);            // Top rect.
   BoxFilled(x+radius, y+h-radius-1, w-2*radius-1, radius, color);     // Bottom rect.
   BoxFilled(x+1, y+radius, radius-1, h-2*radius-1, color);            // Left rect.
   BoxFilled(x+w-radius-1, y+radius, radius, h-2*radius-1, color);     // Right rect.
 
   // Smoothing method
   if (smoothing)
   {
      CircleFilled(x+radius, y+radius, radius-1, 0, quarter, 16, color);             // Top-left corner
      CircleFilled(x+w-radius-1, y+radius, radius-1, 90, quarter, 16, color);        // Top-right corner
      CircleFilled(x+w-radius-1, y+h-radius-1, radius-1, 180, quarter, 16, color);   // Bottom-right corner
      CircleFilled(x+radius, y+h-radius-1, radius-1, 270, quarter, 16, color);       // Bottom-left corner
 
      Circle(x+radius+1, y+radius+1, radius, 0, quarter, true, 16, bcolor);          // Top-left corner
      Circle(x+w-radius-2, y+radius+1, radius, 90, quarter, true, 16, bcolor);       // Top-right corner
      Circle(x+w-radius-2, y+h-radius-2, radius, 180, quarter, true, 16, bcolor);    // Bottom-right corner
      Circle(x+radius+1, y+h-radius-2, radius, 270, quarter, true, 16, bcolor);      // Bottom-left corner
 
      Line(x+radius, y+1, x+w-radius-1, y+1, 1, false, bcolor);       // Top line
      Line(x+radius, y+h-2, x+w-radius-1, y+h-2, 1, false, bcolor);   // Bottom line
      Line(x+1, y+radius, x+1, y+h-radius-1, 1, false, bcolor);       // Left line
      Line(x+w-2, y+radius, x+w-2, y+h-radius-1, 1, false, bcolor);   // Right line
   }
   else
   {
      CircleFilled(x+radius, y+radius, radius, 0, quarter, 16, color);             // Top-left corner
      CircleFilled(x+w-radius-1, y+radius, radius, 90, quarter, 16, color);        // Top-right corner
      CircleFilled(x+w-radius-1, y+h-radius-1, radius, 180, quarter, 16, color);   // Bottom-right corner
      CircleFilled(x+radius, y+h-radius-1, radius, 270, quarter, 16, color);       // Bottom-left corner
   }  
}
 
void Circle(float x, float y, float radius, int rotate, int type, bool smoothing, int resolution, DWORD color)
{
   std::vector<vertex> circle(resolution + 2);   
   float angle = rotate*D3DX_PI/180;   
   float pi;
 
   if(type == full) pi = D3DX_PI;        // Full circle
   if(type == half) pi = D3DX_PI/2;      // 1/2 circle
   if(type == quarter) pi = D3DX_PI/4;   // 1/4 circle
 
   for(int i = 0; i < resolution + 2; i++)
   {
      circle[i].x = (float)(x - radius*cos(i*(2*pi/resolution)));
      circle[i].y = (float)(y - radius*sin(i*(2*pi/resolution)));
      circle[i].z = 0;
      circle[i].rhw = 1;
      circle[i].color = color;
   }
   
   // Rotate matrix
   int _res = resolution + 2;
   for(int i = 0; i < _res; i++)
   {
      circle[i].x = x + cos(angle)*(circle[i].x - x) - sin(angle)*(circle[i].y - y);
      circle[i].y = y + sin(angle)*(circle[i].x - x) + cos(angle)*(circle[i].y - y);
   }
 
   pDevice->CreateVertexBuffer((resolution + 2) * sizeof(vertex), D3DUSAGE_WRITEONLY, D3DFVF_XYZRHW|D3DFVF_DIFFUSE, D3DPOOL_DEFAULT, &g_pVB, NULL);
   
   VOID* pVertices;
   g_pVB->Lock(0, (resolution + 2) * sizeof(vertex), (void**)&pVertices, 0);
   memcpy(pVertices, &circle[0], (resolution + 2) * sizeof(vertex));
   g_pVB->Unlock();
 
 
   pDevice->SetTexture(0, NULL);
   pDevice->SetPixelShader(NULL);
   if (smoothing)
   {
      pDevice->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, TRUE);
      pDevice->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, TRUE);
   }
   pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
   pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
   pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
 
   pDevice->SetStreamSource(0, g_pVB, 0, sizeof(vertex));
   pDevice->SetFVF(D3DFVF_XYZRHW|D3DFVF_DIFFUSE);
 
   pDevice->DrawPrimitive(D3DPT_LINESTRIP, 0, resolution);
   if(g_pVB != NULL) g_pVB->Release();
}

void CircleFilled(float x, float y, float rad, float rotate, int type, int resolution, DWORD color)
{
   std::vector<vertex> circle(resolution + 2);
   float angle = rotate*D3DX_PI/180;
   float pi;
 
   if(type == full) pi = D3DX_PI;        // Full circle
   if(type == half) pi = D3DX_PI/2;      // 1/2 circle
   if(type == quarter) pi = D3DX_PI/4;   // 1/4 circle
 
   circle[0].x = x;
   circle[0].y = y;
   circle[0].z = 0;
   circle[0].rhw = 1;
   circle[0].color = color;   
   
   for(int i = 1; i < resolution + 2; i++)
   {
      circle[i].x = (float)(x - rad*cos(pi*((i-1)/(resolution/2.0f))));
      circle[i].y = (float)(y - rad*sin(pi*((i-1)/(resolution/2.0f))));
      circle[i].z = 0;
      circle[i].rhw = 1;
      circle[i].color = color;
   }
   
   // Rotate matrix
   int _res = resolution + 2;
   for(int i = 0; i < _res; i++)
   {
      circle[i].x = x + cos(angle)*(circle[i].x - x) - sin(angle)*(circle[i].y - y);
      circle[i].y = y + sin(angle)*(circle[i].x - x) + cos(angle)*(circle[i].y - y);
   }
 
   pDevice->CreateVertexBuffer((resolution + 2) * sizeof(vertex), D3DUSAGE_WRITEONLY, D3DFVF_XYZRHW|D3DFVF_DIFFUSE, D3DPOOL_DEFAULT, &g_pVB, NULL );
 
   VOID* pVertices;
   g_pVB->Lock(0, (resolution + 2) * sizeof(vertex), (void**)&pVertices, 0);
   memcpy(pVertices, &circle[0], (resolution + 2) * sizeof(vertex));
   g_pVB->Unlock();
 
   pDevice->SetTexture(0, NULL);
   pDevice->SetPixelShader(NULL);
   pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
   pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
   pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
 
   pDevice->SetStreamSource(0, g_pVB, 0, sizeof(vertex));
   pDevice->SetFVF(D3DFVF_XYZRHW|D3DFVF_DIFFUSE);
   pDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, resolution);
   if(g_pVB != NULL) g_pVB->Release();
}
