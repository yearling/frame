#pragma once
#include "stdafx.h"
#include <memory>
#include "YYUTApplication.h"
struct yyd3dException:std::exception
{

};

class MyAPP:public YYUTApplication
{
public:
	MyAPP();
	virtual bool  Initial();
	virtual int Exit();
	virtual void GameMain(DWORD timespan);
	~MyAPP();
protected:
	virtual LRESULT MyProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	virtual bool GameInit();

	virtual void GameExit();
	bool InitResource();
protected:
	CComPtr<IDirect3D9>  d3d9;
	CComPtr<IDirect3DDevice9> d3d_device;
	CComPtr<IDirect3DVertexBuffer9> d3d_vertex_buffer;
	CComPtr<IDirect3DTexture9> background_texture;
	D3DXVECTOR3 robot_vec,robot_last_vec;
	float mesh_radius;
};