#ifndef __YYUTMESH_H__
#define __YYUTMESH_H__
#pragma once 
#include "stdafx.h"
#include "YYUTObject.h"
#include <atlcomcli.h>
namespace YYUT
{
	struct YYUTMESHException:YYUTException{};
	class YYUTMesh:public YYUTNodeDataBase
	{
	public:
		typedef std::vector<std::pair<D3DMATERIAL9,CComPtr<IDirect3DTexture9> > > MaterialTexture;
		YYUTMesh();
		virtual YYUTNodeDataBase::NodeType GetType() const;
		MaterialTexture& GetMaterialTextureCombine()  { return material_texture_combine_;}
		CComPtr<IDirect3DVertexBuffer9> GetVertexBuffer() const  { return vertex_buffer_;}
		CComPtr<IDirect3DIndexBuffer9>  GetIndexBuffer() const{return index_buffer_;}
	protected:
		CComPtr<IDirect3DDevice9> d3d_dev_;
		MaterialTexture material_texture_combine_;
		CComPtr<IDirect3DVertexBuffer9> vertex_buffer_;
		CComPtr<IDirect3DIndexBuffer9> index_buffer_;
		DWORD               num_indices_;
		DWORD				num_vertices_;
		CComPtr<IDirect3DVertexDeclaration9> vertex_decl_;
	};
}
#endif