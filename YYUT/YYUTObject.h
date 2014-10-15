#ifndef __YYUT_OBJECT_H__
#define __YYUT_OBJECT_H__
#pragma  once
#include "stdafx.h"
#include "YYUT.h"
#include <utility>
#include <string>
namespace YYUT{
	struct YYUTObjectException:virtual YYUTException{};

	class YYUTObjectBase
	{
	public:
		virtual void LoadObject(std::wstring file_name)=0;
		virtual void Draw(int id)=0;
	};
	class YYUTObjectX
	{
	public:
		virtual void LoadObject(std::wstring file_name);
		virtual void Draw();
		void Init(CComPtr<IDirect3DDevice9> d3d_dev);
		CComPtr<ID3DXMesh> GetMesh(){ return mesh_;}
	protected:
		CComPtr<ID3DXMesh> mesh_;
		CComPtr<IDirect3DDevice9> d3d_dev_;
		std::vector<std::pair<D3DMATERIAL9,CComPtr<IDirect3DTexture9> > > material_texture_combine_;
	};
	class YYUTObjectLP
	{
	public:
		YYUTObjectLP();	
		virtual void LoadObject(std::wstring file_name);
		virtual void Draw(int id);
		void Init(CComPtr<IDirect3DDevice9> d3d_dev);
		struct Vertex
		{
			Vertex():x_(0),y_(0),z_(0){};
			Vertex(float x,float y,float z):x_(x),y_(y),z_(z){}
			float x_,y_,z_;
			static const DWORD FVF;
		};
	private:
		int num_vertex_;
		int num_faces_;
		std::vector<Vertex> vertex_;
		std::vector<int> vertex_index_;
		CComPtr<IDirect3DDevice9> d3d_dev_;
		CComPtr<IDirect3DVertexBuffer9> vertex_buf_;
		CComPtr<IDirect3DIndexBuffer9> index_buf_;
	};
}
#endif