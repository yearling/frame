#ifndef __YYUT_OBJECT_H__
#define __YYUT_OBJECT_H__
#pragma  once
#include "stdafx.h"
#include "YYUT.h"
#include "YYUTEffectManager.h"
#include <utility>
#include <string>
#include <functional>
namespace YYUT{
	struct YYUTObjectException:virtual YYUTException{};

	class YYUTObjectBase
	{
	public:
		virtual void LoadObject(std::wstring file_name)=0;
		virtual void Draw()=0;
		virtual ~YYUTObjectBase()=0;
	};
	class YYUTObjectX:public YYUTObjectBase
	{
	public:
		YYUTObjectX():radius_(0.0f){};
		virtual void LoadObject(std::wstring file_name);
		virtual void Draw();
		float GetRadius() { return radius_;}
		virtual ~YYUTObjectX();
		void Init(CComPtr<IDirect3DDevice9> d3d_dev);
		CComPtr<ID3DXMesh> GetMesh(){ return mesh_;}
		typedef std::vector<std::pair<D3DMATERIAL9,CComPtr<IDirect3DTexture9> > > MaterialTexture;
		const MaterialTexture& GetMaterialTextureCombine() const { return material_texture_combine_;}
	protected:
		float			    radius_;
		D3DXVECTOR3			center_;
		CComPtr<ID3DXMesh> mesh_;
		CComPtr<IDirect3DDevice9> d3d_dev_;
		MaterialTexture material_texture_combine_;
	};
	class YYUTHLSLRenderObject:public YYUTObjectX
	{
	public:
		typedef std::function<void(YYUTHLSLRenderObject*)> EffectEvent;
		YYUTHLSLRenderObject();
		~YYUTHLSLRenderObject();
		void LoadObject(std::wstring file_name,string effect_id,EffectEvent frame_event=NULL,EffectEvent reset=NULL);
		void SetFrameMoveEvent(EffectEvent frame_event);
		void SetFrameResetEvent(EffectEvent frame_reset);
		void Draw();
		void OnResetDevice();
		void OnLostDevice();
		void OnFrameMove();
		CComPtr<ID3DXEffect> GetEffect() const{ return effect_;}
		struct SelfFVF
		{
			D3DXVECTOR3 position;
			D3DXVECTOR3 normal;
			float u,v;
			static int FVFSize;
		};
	protected:
	private:
		CComPtr<IDirect3DVertexBuffer9> vertex_buffer_;
		CComPtr<IDirect3DIndexBuffer9> index_buffer_;
		DWORD               num_indices_;
		DWORD				num_vertices_;
		CComPtr<IDirect3DVertexDeclaration9> vertex_decl_;
		int                 num_resources_;
		CComPtr<IDirect3DVertexDeclaration9> decl_;
		CComPtr<ID3DXMesh>  hlsl_mesh_;
		string effect_id_;
		CComPtr<ID3DXEffect> effect_;
		EffectEvent frame_move_event_;
		EffectEvent frame_reset_event_;
	};
}
#endif