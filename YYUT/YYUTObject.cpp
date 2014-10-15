#include "stdafx.h"
#include "YYUTObject.h"
#include <utility>
#include <fstream>
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
				mtrls[i].MatD3D.Ambient=mtrls[i].MatD3D.Diffuse;
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


	void YYUTObjectLP::LoadObject(std::wstring file_name)
	{
		string file(file_name.begin(),file_name.end());
		std::fstream fs;
		fs.open(file,std::ios::in);
		fs>>num_vertex_;
		fs>>num_faces_;
		int nop;
		fs>>nop;
		vertex_.reserve(4*10000);
		for(int i=0;i<num_vertex_;++i)
		{
			fs>>nop;
			assert(nop==i);
			Vertex v;
			fs>>v.x_>>v.y_>>v.z_;
			v.z_=-v.z_;
			vertex_.push_back(v);
		}
		vertex_index_.reserve(4*10000);
		for(int i=0;i<num_faces_;++i)
		{
			fs>>nop;
			assert(nop==i);
			for(int i=0;i<3;++i)
			{
				fs>>nop;
				vertex_index_.push_back(nop);
			}
		}
		HRESULT hr;
		if(FAILED(hr=d3d_dev_->CreateVertexBuffer(num_vertex_*sizeof(Vertex),D3DUSAGE_WRITEONLY,Vertex::FVF,D3DPOOL_MANAGED,&vertex_buf_,nullptr)))
		{
			BOOST_THROW_EXCEPTION(YYUTObjectException()<<err_str("create vertex buffer failed!")<<err_hr(hr));
		}
		if(FAILED(hr=d3d_dev_->CreateIndexBuffer(num_faces_*3*sizeof(int),D3DUSAGE_WRITEONLY,D3DFMT_INDEX32,D3DPOOL_MANAGED,&index_buf_,nullptr)))
		{
			BOOST_THROW_EXCEPTION(YYUTObjectException()<<err_str("create index buffer failed!")<<err_hr(hr));
		}
		Vertex *vec_tmp=0;
		vertex_buf_->Lock(0,0,(void**)&vec_tmp,0);
		memcpy(vec_tmp,&vertex_[0],vertex_.size()*sizeof(Vertex));
		vertex_buf_->Unlock();
		cout<<"load vertex"<<endl;
		int *index_tmp=0;
		index_buf_->Lock(0,0,(void**)&index_tmp,0);
		memcpy(index_tmp,&vertex_index_[0],sizeof(int)*3*num_faces_);
		index_buf_->Unlock();
		cout<<"load index"<<endl;
	}

	void YYUTObjectLP::Draw(int id)
	{
		d3d_dev_->SetRenderState(D3DRS_CLIPPING,FALSE);
		d3d_dev_->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);
		d3d_dev_->SetStreamSource(0,vertex_buf_,0,sizeof(Vertex));
		d3d_dev_->SetIndices(index_buf_);
		d3d_dev_->SetFVF(Vertex::FVF);
		d3d_dev_->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0,0,num_vertex_,0,num_faces_);
	}

	YYUTObjectLP::YYUTObjectLP():num_vertex_(0),num_faces_(0)
	{

	}

	void YYUTObjectLP::Init(CComPtr<IDirect3DDevice9> d3d_dev)
	{
		d3d_dev_=d3d_dev;
	}


	const DWORD YYUTObjectLP::Vertex::FVF=D3DFVF_XYZ;

}
