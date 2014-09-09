#include "stdafx.h"
#include "YYUTObject.h"

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
				material_texture_combine_.push_back(make_pair(mtrls[i].MatD3D,tex));
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

}
