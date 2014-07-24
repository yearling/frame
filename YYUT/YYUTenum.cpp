#include "stdafx.h"
#include "YYUTenum.h"
#include <algorithm>
#include <iostream>
#include <set>
#include <iomanip>
using namespace std;
YYUTenum::YYUTenum()
{
	has_enumerated=false;
	d3d_=nullptr;
	is_device_acceptable_callback=nullptr;
	require_post_pixel_shader_blending=true;
	min_width_=640;
	min_height_=480;
	max_width_=UINT_MAX;
	max_height_=UINT_MAX;
	refresh_rate_min_=0;
	refresh_rate_max_=UINT_MAX;
	multisample_quality_max_=0xFFFF;
	ResetPossibleDepthStencilFormats();
	ResetPossibleMutisampleTypelist();
	ResetPossiblePresentIntervalList();
	SetPossibleVertexProcessingList(true,true,true,false);
}


YYUTenum::~YYUTenum(void)
{
	ClearAdapterInfoList();
}


YYUTenum &YYUTenum::GetInstance()
{
	return instance;
}

void YYUTenum::SetResolutionMinMax(UINT min_width,UINT min_height,UINT max_wid,UINT max_height)
{
	min_width_=min_width;
	min_height_=min_height;
}

void YYUTenum::SetRefreshMinMax(UINT min,UINT max)
{
	refresh_rate_min_=min;
	refresh_rate_max_=max;
}

void YYUTenum::SetMultisapleQualityMax(UINT max)
{
	if(max>0xffff)
		max=0xffff;
	multisample_quality_max_=max;
}

void YYUTenum::GetPossibleVertexProcessingList(bool *software_vp,bool *hardware_vp,bool *pure_hardware_vp,bool *minxed_vp)
{

}

void YYUTenum::SetPossibleVertexProcessingList(bool software_vp,bool hardware_vp,bool pure_hardware_vp,bool minxed_vp)
{
	software_vp_=software_vp;
	hardware_vp_=hardware_vp;
	pure_hareware_vp_=pure_hardware_vp;
	mixed_vp_=minxed_vp;
}

vector<D3DFORMAT> * YYUTenum::GetPossibleDepthStencilFormatList()
{
	return &depth_stencil_possible_list;
}

vector<D3DMULTISAMPLE_TYPE> * YYUTenum::GetPossibleMultisampleTypeList()
{
	return &multisample_type_list;
}

vector<UINT> * YYUTenum::GetPossiblePresentIntervalList()
{
	return &present_interval_list;
}

void YYUTenum::ResetPossibleDepthStencilFormats()
{
	depth_stencil_possible_list.clear();
	depth_stencil_possible_list.push_back(D3DFMT_D16);
	depth_stencil_possible_list.push_back(D3DFMT_D15S1);
	depth_stencil_possible_list.push_back(D3DFMT_D24X8);
	depth_stencil_possible_list.push_back(D3DFMT_D24S8);
	depth_stencil_possible_list.push_back(D3DFMT_D24X4S4);
	depth_stencil_possible_list.push_back(D3DFMT_D32);
}

void YYUTenum::ResetPossibleMutisampleTypelist()
{
	multisample_type_list.clear();
	multisample_type_list.push_back( D3DMULTISAMPLE_NONE );
	multisample_type_list.push_back( D3DMULTISAMPLE_NONMASKABLE );
	multisample_type_list.push_back( D3DMULTISAMPLE_2_SAMPLES );
	multisample_type_list.push_back( D3DMULTISAMPLE_3_SAMPLES );
	multisample_type_list.push_back( D3DMULTISAMPLE_4_SAMPLES );
	multisample_type_list.push_back( D3DMULTISAMPLE_5_SAMPLES );
	multisample_type_list.push_back( D3DMULTISAMPLE_6_SAMPLES );
	multisample_type_list.push_back( D3DMULTISAMPLE_7_SAMPLES );
	multisample_type_list.push_back( D3DMULTISAMPLE_8_SAMPLES );
	multisample_type_list.push_back( D3DMULTISAMPLE_9_SAMPLES );
	multisample_type_list.push_back( D3DMULTISAMPLE_10_SAMPLES );
	multisample_type_list.push_back( D3DMULTISAMPLE_11_SAMPLES );
	multisample_type_list.push_back( D3DMULTISAMPLE_12_SAMPLES );
	multisample_type_list.push_back( D3DMULTISAMPLE_13_SAMPLES );
	multisample_type_list.push_back( D3DMULTISAMPLE_14_SAMPLES );
	multisample_type_list.push_back( D3DMULTISAMPLE_15_SAMPLES );
	multisample_type_list.push_back( D3DMULTISAMPLE_16_SAMPLES );
}

void YYUTenum::ResetPossiblePresentIntervalList()
{
	present_interval_list.clear();
	present_interval_list.push_back(D3DPRESENT_INTERVAL_IMMEDIATE);
	present_interval_list.push_back(D3DPRESENT_INTERVAL_DEFAULT);
	present_interval_list.push_back(D3DPRESENT_INTERVAL_ONE);
	present_interval_list.push_back(D3DPRESENT_INTERVAL_TWO);
	present_interval_list.push_back(D3DPRESENT_INTERVAL_THREE);
	present_interval_list.push_back(D3DPRESENT_INTERVAL_FOUR);
}
bool operator < (const D3DDISPLAYMODE & d1,const D3DDISPLAYMODE &d2)
{
	if(d1.Width<d2.Width)
		return true;
	if(d1.Width>d2.Width)
		return false;
	if(d1.Height<d2.Height)
		return true;
	if(d1.Height>d2.Height)
		return false;
	if((UINT)d1.Format<(UINT)d2.Format)
		return true;
	if((UINT)d1.Format>(UINT)d2.Format)
		return false;
	if((UINT)d1.RefreshRate<(UINT)d2.RefreshRate)
		return true;
	if((UINT)d1.RefreshRate>(UINT)d2.RefreshRate)
		return false;
	return false;
}
HRESULT YYUTenum::Enumerate()
{
	has_enumerated=true;
	HRESULT hr;
	ClearAdapterInfoList();
	set<D3DFORMAT> adapter_format_set;
	//BackBuffer or Display Formats
	//These formats are the only valid formats for a back buffer or a display.
	/*
		Format        Back buffer   Display 
		A2R10G10B10   x				x (full-screen mode only) 
		A8R8G8B8	  x  
		X8R8G8B8      x				x 
		A1R5G5B5	  x  
		X1R5G5B5      x             x 
		R5G6B5        x             x  
	*/
	//也就是说这里的display对应的就是adapter
	const D3DFORMAT allowed_adapter_format[] =
	{
		D3DFMT_X8R8G8B8,
		D3DFMT_X1R5G5B5,
		D3DFMT_R5G6B5,
		D3DFMT_A2R10G10B10
	};
	const UINT allowed_adapter_format_count=_countof(allowed_adapter_format);
	//得到有多个adapter,貌似是一个屏幕对应一个
	UINT num_adapters=d3d_->GetAdapterCount();
	for(UINT adapter_ordinal=0;adapter_ordinal<num_adapters;adapter_ordinal++)
	{
		D3D9EnumAdapterInfo * adapter_info=new D3D9EnumAdapterInfo();
		if(adapter_info==nullptr)
			throw bad_alloc();
		adapter_info->adapter_ordianl=adapter_ordinal;
		d3d_->GetAdapterIdentifier(adapter_ordinal,0,&adapter_info->adatper_identifier);
		adapter_format_set.clear();
		for(UINT i_format=0;i_format<allowed_adapter_format_count;i_format++)
		{
			D3DFORMAT tmp_format=allowed_adapter_format[i_format];
			//得到对应D3DFormat的模式，比如分辨率，大小，刷新率
			UINT num_adapter_modes=d3d_->GetAdapterModeCount(adapter_ordinal,tmp_format);
			for(UINT mode=0;mode<num_adapter_modes;mode++)
			{
				D3DDISPLAYMODE display_mode;
				d3d_->EnumAdapterModes(adapter_ordinal,tmp_format,mode,&display_mode);
				adapter_info->display_mode_list.push_back(display_mode);
				adapter_format_set.insert(display_mode.Format);
			}
		}
		D3DDISPLAYMODE display_mode;
		d3d_->GetAdapterDisplayMode(adapter_ordinal,&display_mode);
		adapter_format_set.insert(display_mode.Format);
		std::sort(adapter_info->display_mode_list.begin(),adapter_info->display_mode_list.end());
		if(FAILED(EnumerateDevices(adapter_info,&adapter_format_set)))
		{
			delete adapter_info;
			continue;
		}
		if(adapter_info->device_info_list.empty())
		{
			delete adapter_info;
		}
		else
		{
			adapter_info_list.push_back(adapter_info);
		}
	}
	return S_OK;
}

D3D9EnumAdapterInfo * YYUTenum::GetAdapterInfo(UINT adapter_ordinal)
{
	return nullptr;
}

D3D9EnumDeviceInfo * YYUTenum::GetDeviceInfo(UINT adapter_ordinal,D3DDEVTYPE device_type)
{
	return nullptr;
}

D3D9EnumDeviceSettingsCombo* YYUTenum::GetDeviceSettingsCombo(UINT adapter_ordinal, D3DDEVTYPE device_type, D3DFORMAT adapter_format, D3DFORMAT backBuffer_format, BOOL windowed)
{
	return nullptr;
}

void YYUTenum::ClearAdapterInfoList()
{
	adapter_info_list.clear();
}

void YYUTenum::SetD3D9(IDirect3D9 *d3d)
{
	d3d_=d3d;
}

HRESULT YYUTenum::EnumerateDevices(D3D9EnumAdapterInfo *adapter_info,set<D3DFORMAT> * adapter_foramt_set)
{
	HRESULT hr;
	const D3DDEVTYPE dev_type[]=
	{
		D3DDEVTYPE_HAL,
		D3DDEVTYPE_SW,
		D3DDEVTYPE_REF
	};
	const UINT dev_type_count=_countof(dev_type);
	for(UINT i=0;i<dev_type_count;i++)
	{
		D3D9EnumDeviceInfo *dev_info=new D3D9EnumDeviceInfo;
		if(dev_info==nullptr)
			return E_OUTOFMEMORY;
		dev_info->adapter_Ordinal=adapter_info->adapter_ordianl;
		dev_info->device_type=dev_type[i];
		if(FAILED(hr=d3d_->GetDeviceCaps(adapter_info->adapter_ordianl,dev_info->device_type,&dev_info->caps)))
		{
			delete dev_info;
			continue;
		}
		if(dev_info->device_type!=D3DDEVTYPE_HAL)
		{
			//create a temp device to verify that it is really can create a REF device
			D3DDISPLAYMODE mode;
			d3d_->GetAdapterDisplayMode(adapter_info->adapter_ordianl,&mode);
			D3DPRESENT_PARAMETERS pp;
			ZeroMemory(&pp,sizeof(pp));
			pp.BackBufferWidth=1;
			pp.BackBufferHeight=1;
			pp.BackBufferFormat=mode.Format;
			pp.BackBufferCount=1;
			pp.SwapEffect=D3DSWAPEFFECT_COPY;
			pp.Windowed=true;
			pp.hDeviceWindow=hwnd_;
			IDirect3DDevice9 * dev=nullptr;
			if(FAILED(hr=d3d_->CreateDevice(adapter_info->adapter_ordianl,dev_info->device_type,hwnd_,D3DCREATE_HARDWARE_VERTEXPROCESSING|D3DCREATE_FPU_PRESERVE,&pp,&dev)))
			{
				delete dev_info;
				continue;
			}
			dev->Release();
			dev=nullptr;
		}
		if(FAILED(hr=EnumerateDevicesCombos(adapter_info,dev_info,adapter_foramt_set)))
		{
			delete dev_info;
			continue;
		}
		if(dev_info->device_settings_comboList.empty())
			delete dev_info;
		else
			adapter_info->device_info_list.push_back(dev_info);
	}
	return S_OK;
}

HRESULT YYUTenum::EnumerateDevicesCombos(D3D9EnumAdapterInfo *adapter_info,D3D9EnumDeviceInfo * device_ifno, set<D3DFORMAT> * adapter_foramt_set)
{
	const D3DFORMAT back_buffer_format_arr[] =
	{
		D3DFMT_A8R8G8B8,
		D3DFMT_X8R8G8B8,
		D3DFMT_A2R10G10B10,
		D3DFMT_R5G6B5,
		D3DFMT_A1R5G5B5,
		D3DFMT_X1R5G5B5
	};
	const UINT back_buffer_count=_countof(back_buffer_format_arr);
	for(auto i_format=adapter_foramt_set->begin();i_format!=adapter_foramt_set->end();i_format++)
	{
		D3DFORMAT adapter_format=*i_format;
		for(UINT i_back_buffer_format=0;i_back_buffer_format<back_buffer_count;i_back_buffer_format++)
		{
			D3DFORMAT back_buffer_format=back_buffer_format_arr[i_back_buffer_format];
			for(int windowed=0;windowed<2;windowed++)
			{
				if(!windowed && adapter_info->display_mode_list.size()==0)
					continue;
				if(FAILED(d3d_->CheckDeviceType(adapter_info->adapter_ordianl,device_ifno->device_type,adapter_format,back_buffer_format,windowed)))
				{
					continue;
				}
				if(require_post_pixel_shader_blending)
				{
					if(FAILED(d3d_->CheckDeviceFormat(adapter_info->adapter_ordianl,device_ifno->device_type,adapter_format,D3DUSAGE_QUERY_POSTPIXELSHADER_BLENDING,D3DRTYPE_TEXTURE,back_buffer_format)))
					{
						continue;
					}
				}
				if(is_device_acceptable_callback)
				{
					if(!is_device_acceptable_callback(&device_ifno->caps,adapter_format,back_buffer_format,FALSE!=windowed,nullptr))
						continue;
				}
				//这时候我们已经找到了合适的adapter device adpterformat backbufferformat iswindows 的组合
				//接下来找出depth/stencil buffer format multisaple type and present interval
				D3D9EnumDeviceSettingsCombo *dev_combo=new D3D9EnumDeviceSettingsCombo;
				if(!dev_combo)
					return E_OUTOFMEMORY;
				dev_combo->AdapterOrdinal=adapter_info->adapter_ordianl;
				dev_combo->DeviceType=device_ifno->device_type;
				dev_combo->AdapterFormat=adapter_format;
				dev_combo->BackBufferFormat=back_buffer_format;
				dev_combo->Windowed=(windowed!=0);
				BuildDepthStencilFormatList(dev_combo);
				BuildMultiSampleTypeList(dev_combo);
				if(dev_combo->multiSampleTypeList.empty())
				{
					delete dev_combo;
					continue;
				}
				BuildDSMSConflictList(dev_combo);
				BuildPresetIntervalList(dev_combo,device_ifno);
				dev_combo->adapter_info=adapter_info;
				dev_combo->device_info=device_ifno;
				device_ifno->device_settings_comboList.push_back(dev_combo);
			}
		}
	}
	return S_OK;
}

void YYUTenum::BuildDepthStencilFormatList(D3D9EnumDeviceSettingsCombo *dev_combo)
{
	D3DFORMAT depth_stencil_format;
	for(auto i=depth_stencil_possible_list.begin();i!=depth_stencil_possible_list.end();++i)
	{
		depth_stencil_format=*i;
		if(SUCCEEDED(d3d_->CheckDeviceFormat(dev_combo->AdapterOrdinal,dev_combo->DeviceType,
			dev_combo->AdapterFormat,D3DUSAGE_DEPTHSTENCIL,D3DRTYPE_SURFACE,depth_stencil_format)))
		{
			if(SUCCEEDED(d3d_->CheckDepthStencilMatch(dev_combo->AdapterOrdinal,dev_combo->DeviceType,
				dev_combo->AdapterFormat,dev_combo->BackBufferFormat,depth_stencil_format)))
			{
				dev_combo->depthStencilFormatList.push_back(depth_stencil_format);
			}
		}
	}
}

void YYUTenum::BuildMultiSampleTypeList(D3D9EnumDeviceSettingsCombo *dev_combo)
{
	D3DMULTISAMPLE_TYPE ms_type;
	DWORD ms_quality;
	for(auto i=multisample_type_list.begin();i!=multisample_type_list.end();++i)
	{
		ms_type=*i;
		if(SUCCEEDED(d3d_->CheckDeviceMultiSampleType(dev_combo->AdapterOrdinal,dev_combo->DeviceType,dev_combo->BackBufferFormat,dev_combo->Windowed,ms_type,&ms_quality)))
		{
			dev_combo->multiSampleTypeList.push_back(ms_type);
			if(ms_quality> multisample_quality_max_+1)
			{
				ms_quality=multisample_quality_max_+1;
				
			}
			dev_combo->multiSampleQualityList.push_back(ms_quality);
		}
	}
}

void YYUTenum::BuildDSMSConflictList(D3D9EnumDeviceSettingsCombo *dev_combo)
{
	D3D9EnumDSMSConflict ds_conflict;
	for(auto i=dev_combo->depthStencilFormatList.begin();i!=dev_combo->depthStencilFormatList.end();++i)
	{
		D3DFORMAT ds_fmt=*i;
		for(auto j=dev_combo->multiSampleTypeList.begin();j!=dev_combo->multiSampleTypeList.end();++j)
		{
			D3DMULTISAMPLE_TYPE ms_type=*j;
			if(FAILED(d3d_->CheckDeviceMultiSampleType(dev_combo->AdapterOrdinal,dev_combo->DeviceType,
				ds_fmt,dev_combo->Windowed,ms_type,nullptr)))
			{
				ds_conflict.DSFormat=ds_fmt;
				ds_conflict.MSType=ms_type;
				dev_combo->DSMSConflictList.push_back(ds_conflict);
			}
		}
	}
}

void YYUTenum::BuildPresetIntervalList(D3D9EnumDeviceSettingsCombo *dev_combo,D3D9EnumDeviceInfo *dev_info)
{
	UINT pi;
	for(auto i=present_interval_list.begin();i!=present_interval_list.end();++i)
	{
		pi=*i;
		if(dev_combo->Windowed)
		{
			if( pi == D3DPRESENT_INTERVAL_TWO ||
				pi == D3DPRESENT_INTERVAL_THREE ||
				pi == D3DPRESENT_INTERVAL_FOUR )
			{
				// These intervals are not supported in windowed mode.
				continue;
			}
		}
		// Note that D3DPRESENT_INTERVAL_DEFAULT is zero, so you
		// can't do a caps check for it -- it is always available.
		if(pi==D3DPRESENT_INTERVAL_DEFAULT || (dev_info->caps.PresentationIntervals & pi))
		{
			dev_combo->presentIntervalList.push_back(pi);
		}
	}
}

vector<D3D9EnumAdapterInfo*> &YYUTenum::GetAdapterInfoList()
{
	return adapter_info_list;
}

void YYUTenum::PrintAllValidType()
{
	for(auto i_adapter=adapter_info_list.begin();
		i_adapter!=adapter_info_list.end();++i_adapter)
	{
		cout<<"-----------------------------------"<<endl;
		cout<<"Adapter info:"<<endl;
		cout<<"adapter ordinal: "<<(*i_adapter)->adapter_ordianl<<endl;
		cout<<"adapter name"<<(*i_adapter)->adatper_identifier.DeviceName<<endl;
		int display_mode_count=0;
		wcout<<_T("              ")<<_T("分辨率")<<"  "<<_T("刷新率")
			 <<"   "<<_T("adapter Format")<<endl;
		for_each((*i_adapter)->display_mode_list.begin(),
				 (*i_adapter)->display_mode_list.end(),
			     [&](D3DDISPLAYMODE& d){
				 wcout<<_T("mode[")<<setw(2)<<display_mode_count++<<_T("]:")<<
				 setw(5)<<d.Width<<_T("*")<<setw(5)<<d.Height<<_T("   ")<<
				 setw(3)<<d.RefreshRate<<_T("   ")<<Format2String(d.Format)<<endl;
				});

		for(auto i_dev=(*i_adapter)->device_info_list.begin();
			i_dev!=(*i_adapter)->device_info_list.end();++i_dev)
		{
			wcout<<_T("----------------dev")
				 <<Device2String((*i_dev)->device_type)<<_T("------------")<<endl;
			wcout<<_T("显卡  ")<<_T("dev_type  ")<<_T("adapter_buf  ")
				 <<_T("back_buffer    ")<<_T("windowed   ")<<endl;
			for(auto i_com=(*i_dev)->device_settings_comboList.begin();
				i_com!=(*i_dev)->device_settings_comboList.end();++i_com)
			{
				D3D9EnumDeviceSettingsCombo *com=*i_com;
				wcout<<left<<com->AdapterOrdinal<<"     "
					<<left<<setw(10)<<Device2String(com->DeviceType)
					<<left<<setw(10)<<Format2String(com->AdapterFormat)<<_T("   ")
					<<left<<setw(15)<<Format2String(com->BackBufferFormat)
					<<left<<setw(10)<<Windowed2String(com->Windowed)<<endl;
			}
		}
	}
}

std::wstring YYUTenum::Format2String(D3DFORMAT f)
{
	switch(f)
	{
	case D3DFMT_UNKNOWN:
		return _T("UNKNOWN");
		break;
	case D3DFMT_A8R8G8B8:
		return _T("A8R8G8B8");
		break;
	case D3DFMT_X8R8G8B8:
		return _T("X8R8G8B8");
		break;
	case D3DFMT_A2R10G10B10:
		return _T("A2R10G10B10");
		break;
	case D3DFMT_R5G6B5:
		return _T("R5G6B5");
		break;
	case D3DFMT_A1R5G5B5:
		return _T("A1R5G5B5");
		break;
	case D3DFMT_X1R5G5B5:
		return _T("X1R5G5B5");
		break;
	default:
		return _T("UNKNOWN");
	}
}

std::wstring YYUTenum::Device2String(D3DDEVTYPE dev)
{
	switch(dev)
	{
	case D3DDEVTYPE_HAL:
		return _T("HAL");
		break;
	case D3DDEVTYPE_REF:
		return _T("REF");
		break;
	case D3DDEVTYPE_SW:
		return _T("SW");
		break;
	case D3DDEVTYPE_NULLREF:
		return _T("NULLREF");
		break;
	default:
		return _T("UNKNOW");
		break;
	}
}

std::wstring YYUTenum::Windowed2String(bool windowed)
{
	if(windowed)
		return _T("windowed");
	else
		return _T("full screen");
}


bool YYUTenum::CheckD3D9DeviceSettings(YYUTD3D9DeviceSettings *d3d_device_settings)
{
	bool flag=false;
	D3DDISPLAYMODE display_mode;
	d3d_->GetAdapterDisplayMode(d3d_device_settings->adapter_ordinal,&display_mode);	
	if(d3d_device_settings->pp.Windowed)
	{
		//如果是非全屏的，检查back_buffer是否一样。
		/*if(d3d_device_settings->pp.BackBufferFormat==display_mode.Format)
		{
			flag=true;
		}*/
		if(FAILED(d3d_->CheckDeviceType(d3d_device_settings->adapter_ordinal,d3d_device_settings->device_type,
			display_mode.Format,d3d_device_settings->pp.BackBufferFormat,true)))
			return false;
		if(d3d_device_settings->pp.FullScreen_RefreshRateInHz!=0)
			return false;
			return true;
	}
	else
	{
		auto i_adapter=std::find_if(adapter_info_list.begin(),adapter_info_list.end(),
			[&](D3D9EnumAdapterInfo*p){
				return p->adapter_ordianl==d3d_device_settings->adapter_ordinal;
				return true;
		});
		if(i_adapter==adapter_info_list.end())
			return flag;
		D3D9EnumAdapterInfo* adapter=*i_adapter;
		auto i_mode=std::find_if(adapter->display_mode_list.begin(),
							  adapter->display_mode_list.end(),
							  [&](D3DDISPLAYMODE &d){
							return d3d_device_settings->adapter_format==d.Format &&
								d3d_device_settings->pp.BackBufferWidth==d.Width&&
								d3d_device_settings->pp.BackBufferHeight==d.Height&&
								d3d_device_settings->pp.FullScreen_RefreshRateInHz==d.RefreshRate;
							});
		if(i_mode==adapter->display_mode_list.end())
			return flag;
		auto i_dev=std::find_if(adapter->device_info_list.begin(),adapter->device_info_list.end(),
			[&](D3D9EnumDeviceInfo* d){
				return d->device_type==d3d_device_settings->device_type;
			});
		if(i_dev==adapter->device_info_list.end())
			return flag;
		D3D9EnumDeviceInfo *dev_info=*i_dev;
		auto i_com=std::find_if(dev_info->device_settings_comboList.begin(),dev_info->device_settings_comboList.end(),
			[&](D3D9EnumDeviceSettingsCombo *com){
					return com->BackBufferFormat==d3d_device_settings->pp.BackBufferFormat;
				});
		if(i_com==dev_info->device_settings_comboList.end())
			return flag;
		D3D9EnumDeviceSettingsCombo *com=*i_com;
			
		auto i_ds=std::find_if(com->depthStencilFormatList.begin(),com->depthStencilFormatList.end(),
					[&](D3DFORMAT & format){
						return format==d3d_device_settings->pp.AutoDepthStencilFormat;
					});
		if(i_ds==com->depthStencilFormatList.end())
			return flag;
		auto i_mt=std::find_if(com->multiSampleTypeList.begin(),com->multiSampleTypeList.end(),
			[&](D3DMULTISAMPLE_TYPE &mt){
				return mt==d3d_device_settings->pp.MultiSampleType;
			});
		if(i_mt==com->multiSampleTypeList.end())
			return flag;
		auto i_present=std::find_if(com->presentIntervalList.begin(),com->presentIntervalList.end(),
			[&](UINT u){
					return u==d3d_device_settings->pp.PresentationInterval;
			});
			
		flag=true;
	}
	return flag;	
}

bool YYUTenum::FindPropertySettings(YYUTD3D9DeviceSettings *d3d_device_settings)
{
	if(!CheckD3D9DeviceSettings(d3d_device_settings))
	{
		D3DDISPLAYMODE display_mode;
		d3d_->GetAdapterDisplayMode(d3d_device_settings->adapter_ordinal,&display_mode);
		if(d3d_device_settings->pp.Windowed)
		{
			d3d_device_settings->adapter_format=display_mode.Format;
			d3d_device_settings->pp.BackBufferFormat=display_mode.Format;
			d3d_device_settings->device_type=D3DDEVTYPE_HAL;
			d3d_device_settings->pp.BackBufferCount=1;
			d3d_device_settings->pp.EnableAutoDepthStencil=true;
			d3d_device_settings->pp.Flags=0;
			d3d_device_settings->pp.FullScreen_RefreshRateInHz=0;
			d3d_device_settings->pp.MultiSampleQuality=0;
			d3d_device_settings->pp.MultiSampleType=D3DMULTISAMPLE_NONE;
			d3d_device_settings->pp.AutoDepthStencilFormat=D3DFMT_D24S8;
			d3d_device_settings->pp.PresentationInterval=D3DPRESENT_INTERVAL_DEFAULT;
			d3d_device_settings->pp.SwapEffect=D3DSWAPEFFECT_DISCARD;
			if(CheckD3D9DeviceSettings(d3d_device_settings))
				return true;
			else
				return false;
		}
		else
		{
			//full screen
			d3d_device_settings->pp.BackBufferWidth=display_mode.Width;
			d3d_device_settings->pp.BackBufferHeight=display_mode.Height;
			d3d_device_settings->adapter_format=display_mode.Format;
			d3d_device_settings->pp.BackBufferFormat=display_mode.Format;
			d3d_device_settings->device_type=D3DDEVTYPE_HAL;
			d3d_device_settings->pp.BackBufferCount=1;
			d3d_device_settings->pp.EnableAutoDepthStencil=true;
			d3d_device_settings->pp.Flags=0;
			d3d_device_settings->pp.FullScreen_RefreshRateInHz=display_mode.RefreshRate;
			d3d_device_settings->pp.MultiSampleQuality=0;
			d3d_device_settings->pp.MultiSampleType=D3DMULTISAMPLE_NONE;
			d3d_device_settings->pp.AutoDepthStencilFormat=D3DFMT_D24S8;
			d3d_device_settings->pp.PresentationInterval=D3DPRESENT_INTERVAL_IMMEDIATE;
			d3d_device_settings->pp.SwapEffect=D3DSWAPEFFECT_DISCARD;
			if(CheckD3D9DeviceSettings(d3d_device_settings))
				return true;
			else
				return false;
		}
	}
	else
		return true;
}

YYUTenum YYUTenum::instance;

D3D9EnumAdapterInfo::~D3D9EnumAdapterInfo()
{

}

D3D9EnumDeviceInfo::~D3D9EnumDeviceInfo()
{

}
