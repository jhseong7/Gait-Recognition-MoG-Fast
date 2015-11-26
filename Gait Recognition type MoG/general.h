#ifndef GENERAL_H
#define GENERAL_H

#include <d3d9.h>
#include <d3dx9.h>
#include <string>
#include <sstream>


// Define vertex data structure.
struct VERTEX_TYPE1
{
	FLOAT x, y, z, rhw;
	FLOAT u1, v1;
	enum
	{
		FVF = D3DFVF_XYZRHW | D3DFVF_TEX1
	};
};

struct VERTEX_TYPE2
{
	FLOAT x, y, z, w;
	FLOAT u1, v1;
	enum
	{
		FVF = D3DFVF_XYZW | D3DFVF_TEX1
	};
};

struct VERTEX_TYPE3
{
	FLOAT x, y, z, w;
	DWORD clr;
	FLOAT u1, v1;
	enum
	{
		FVF = D3DFVF_XYZW | D3DFVF_DIFFUSE | D3DFVF_TEX1
	};
};

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }
#endif //SAFE_RELEASE

typedef unsigned char BYTE;
typedef int INT4V[4];

class COLOR
{
public:
	union {
		int m_nData;
		struct {
			BYTE m_nB, m_nG, m_nR, m_nA;
		} m_Channel;
	};
};

#undef	INT_HUGE
#define	INT_HUGE	2000000000	//2147483648

#define MAX_STEREO_PAIR 4
#define MAX_CAMERA_VIEW 10
#define MAX_TEXTURE_SIZE 4096

class DataStereo;
class DataMultiFrame;
class DataMultiView;
class DataCamera;
class DataRendering;
class DataMotion;

extern LPDIRECT3DDEVICE9 g_pd3dDevice;

#endif
