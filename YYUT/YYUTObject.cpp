#include "stdafx.h"
#include "YYUTObject.h"
#include "YYUTUtility.h"
#include <utility>
namespace YYUT
{

	void YYUTObjectX::LoadObject(wstring file_name)
	{
		HRESULT hr;
		CComPtr<ID3DXBuffer> material_buf;
		DWORD num_material=0;
		//wstring pre_path=file_name.substr(0,file_name.rfind(L'\\'));
		wstring pre_path;
		auto pos=file_name.rfind(L'\\');
		if(pos!=file_name.npos)
			pre_path=file_name.substr(0,pos+1);
		string pre_path_A(pre_path.begin(),pre_path.end());
		string texture_path;
		hr=D3DXLoadMeshFromX(file_name.c_str(),D3DXMESH_MANAGED,d3d_dev_,nullptr,&material_buf,nullptr,&num_material,&mesh_);
		if(FAILED(hr))
			BOOST_THROW_EXCEPTION(YYUTObjectException()<<err_str("Create Mesh Faild! mesh name is "+string(file_name.begin(),file_name.end()))<<err_hr(hr));
		if(material_buf && num_material!=0)
		{
			D3DXMATERIAL *mtrls=static_cast<D3DXMATERIAL*>(material_buf->GetBufferPointer());
			for(DWORD i=0;i<num_material;++i)
			{
				//mtrls[i].MatD3D.Ambient=mtrls[i].MatD3D.Diffuse;
				CComPtr<IDirect3DTexture9> tex;
				if(mtrls[i].pTextureFilename!=nullptr)
				{
					texture_path=pre_path_A+mtrls[i].pTextureFilename;
					hr=D3DXCreateTextureFromFileA(d3d_dev_,texture_path.c_str(),&tex);
					if(FAILED(hr))
						BOOST_THROW_EXCEPTION(YYUTObjectException()<<err_str("Create texture Faild! mesh name is "+string(mtrls[i].pTextureFilename))<<err_hr(hr));
				}
				material_texture_combine_.push_back(std::make_pair(mtrls[i].MatD3D,tex));
			}
		}
	}

	void YYUTObjectX::Init(CComPtr<IDirect3DDevice9> d3d_dev)
	{
		d3d_dev_=d3d_dev;
	}

	void YYUTObjectX::Draw()
	{
		for(DWORD i=0;i<material_texture_combine_.size();++i)
		{
			d3d_dev_->SetMaterial(&material_texture_combine_[i].first);
			d3d_dev_->SetTexture(0,material_texture_combine_[i].second);
			mesh_->DrawSubset(i);
		}
	}

	YYUTObjectX::~YYUTObjectX()
	{

	}


	void YYUTHLSLRenderObject::OnLostDevice()
	{

	}

	void YYUTHLSLRenderObject::OnFrameMove()
	{
		if(frame_move_event_)
		{
			frame_move_event_(this);
		}
	}

	void YYUTHLSLRenderObject::OnResetDevice()
	{
		if(frame_reset_event_)
		{
			frame_reset_event_(this);
		}
	}

	void YYUTHLSLRenderObject::Draw()
	{
		HRESULT hr=S_OK;
		d3d_dev_->SetVertexDeclaration(decl_);
		d3d_dev_->SetStreamSource(0,vertex_buffer_,0,SelfFVF::FVFSize);
		d3d_dev_->SetIndices(index_buffer_);
		UINT cpass;
		effect_->Begin(&cpass,0);
		for(UINT i=0;i<cpass;i++)
		{
			effect_->BeginPass(i);
			d3d_dev_->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0,0,num_vertices_,0,num_indices_);
			effect_->EndPass();
		}
		effect_->End();
	}

	void YYUTHLSLRenderObject::LoadObject(std::wstring file_name,string effect_id,EffectEvent frame_event,EffectEvent reset)
	try{
		YYUTObjectX::LoadObject(file_name);
		D3DVERTEXELEMENT9 decl[]=
		{
			{0,0,D3DDECLTYPE_FLOAT3,D3DDECLMETHOD_DEFAULT,D3DDECLUSAGE_POSITION,0},
			{0,12,D3DDECLTYPE_FLOAT3,D3DDECLMETHOD_DEFAULT,D3DDECLUSAGE_NORMAL,0},
			{0,24,D3DDECLTYPE_FLOAT2,D3DDECLMETHOD_DEFAULT,D3DDECLUSAGE_TEXCOORD,0},
			D3DDECL_END()
		};
		HRESULT hr=S_OK;
		if(FAILED(hr=d3d_dev_->CreateVertexDeclaration(decl,&decl_)))
			BOOST_THROW_EXCEPTION(YYUTObjectException()<<err_str("Create vertex declaration failed!")<<err_hr(hr));
		if(FAILED(hr=mesh_->CloneMesh(D3DXMESH_MANAGED,decl,d3d_dev_,&hlsl_mesh_)))
			BOOST_THROW_EXCEPTION(YYUTObjectException()<<err_str("CloneMesh failed!")<<err_hr(hr));
		if(FAILED(hr=hlsl_mesh_->GetVertexBuffer(&vertex_buffer_)))
			BOOST_THROW_EXCEPTION(YYUTObjectException()<<err_str("Get vertex buffer failed!")<<err_hr(hr));
		if(FAILED(hr=hlsl_mesh_->GetIndexBuffer(&index_buffer_)))
			BOOST_THROW_EXCEPTION(YYUTObjectException()<<err_str("Get index buffer failed!")<<err_hr(hr));
		num_indices_=hlsl_mesh_->GetNumFaces();
		num_vertices_=hlsl_mesh_->GetNumVertices();
		D3DXVECTOR3 *data;
		if(FAILED(hr=hlsl_mesh_->LockVertexBuffer(0,(LPVOID*)&data)))
			BOOST_THROW_EXCEPTION(YYUTObjectException()<<err_str("Lock Vertex buffer failed!")<<err_hr(hr));
		if(FAILED(hr=D3DXComputeBoundingSphere(data,hlsl_mesh_->GetNumVertices(),D3DXGetFVFVertexSize(hlsl_mesh_->GetFVF()),&center_,&radius_)))
			BOOST_THROW_EXCEPTION(YYUTObjectException()<<err_str("compute bouding sphere failed!")<<err_hr(hr));
		if(FAILED(hr=hlsl_mesh_->UnlockVertexBuffer()))
			BOOST_THROW_EXCEPTION(YYUTObjectException()<<err_str("unlock vertex buffer error")<<err_hr(hr));
		
		effect_id_=effect_id;
		effect_=YYUTEffectManager::GetInstance().AddEffect(effect_id_);

		frame_move_event_=frame_event;
		frame_reset_event_=reset;
	}
	catch(YYUTException &e)
	{
		ExceptionTraceInformation(e);
		string file_name_char(file_name.begin(),file_name.end());
		BOOST_THROW_EXCEPTION(e<<err_str("load object failed! file_name is "+file_name_char+";effect_id "+effect_id));
	}

	YYUTHLSLRenderObject::YYUTHLSLRenderObject()
	{
		
	}

	YYUTHLSLRenderObject::~YYUTHLSLRenderObject()
	{

	}

	void YYUTHLSLRenderObject::SetFrameMoveEvent(EffectEvent frame_event)
	{
		frame_move_event_=frame_event;
	}

	void YYUTHLSLRenderObject::SetFrameResetEvent(EffectEvent frame_reset)
	{
		frame_reset_event_=frame_reset;
	}


	YYUTObjectBase::~YYUTObjectBase()
	{

	}


	int YYUTHLSLRenderObject::SelfFVF::FVFSize=sizeof(SelfFVF);

}
