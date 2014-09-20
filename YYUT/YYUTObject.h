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

}
#endif