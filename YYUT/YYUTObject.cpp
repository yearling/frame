#include "stdafx.h"
#include "YYUTObject.h"

namespace YYUT
{

	void YYUTObjectX::LoadObject(wstring file_name)
	{
		HRESULT hr;
		CComPtr<ID3DXBuffer> material_buf;
		DWORD num_material=0;
		hr=D3DXLoadMeshFromX(file_name.c_str(),D3DXMESH_MANAGED,d3d_dev_,nullptr,&material_buf,nullptr,&num_material,&mesh_);
		if(FAILED(hr))
			BOOST_THROW_EXCEPTION(YYUTObjectException()<<err_str("Create Mesh Faild! mesh name is "+string(file_name.begin(),file_name.end()))<<err_hr(hr));
		if(material_buf && num_material!=0)
		{
			D3DXMATERIAL *mtrls=static_cast<D3DXMATERIAL*>(material_buf->GetBufferPointer());
			for(int i=0;i<num_material;++i)
			{
				mtrls[i].MatD3D.Ambient=mtrls[i].MatD3D.Diffuse;
				CComPtr<IDirect3DTexture9> tex;
				if(mtrls[i].pTextureFilename!=nullptr)
				{
					hr=D3DXCreateTextureFromFileA(d3d_dev_,mtrls[i].pTextureFilename,&tex);
					if(FAILED(hr))
						BOOST_THROW_EXCEPTION(YYUTObjectException()<<err_str("Create Mesh Faild! mesh name is "+string(mtrls[i].pTextureFilename))<<err_hr(hr));
				}
				material_texture_combine_.push_back(make_pair(mtrls[i].MatD3D,tex));
			}
		}
	}

	void YYUTObjectX::Init(CComPtr<IDirect3DDevice9> d3d_dev)
	{
		d3d_dev_=d3d_dev;
	}

	void YYUTObjectX::Draw(int id)
	{
		mesh_->DrawSubset(id);
	}

}
