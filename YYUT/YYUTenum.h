#pragma once
#include "stdafx.h"
#include <vector>
#include <functional>
#include <set>
#include "YYUT.h"
using namespace std;
using std::set;
using  std::tr1::function;
class D3D9EnumDeviceInfo;
class D3D9EnumAdapterInfo;

struct D3D9EnumDSMSConflict
{
	D3DFORMAT DSFormat;
	D3DMULTISAMPLE_TYPE MSType;
};

struct D3D9EnumDeviceSettingsCombo
{
	UINT AdapterOrdinal;
	D3DDEVTYPE DeviceType;
	D3DFORMAT AdapterFormat;
	D3DFORMAT BackBufferFormat;
	BOOL Windowed;

	vector <D3DFORMAT> depthStencilFormatList; // List of D3DFORMATs
	vector <D3DMULTISAMPLE_TYPE> multiSampleTypeList; // List of D3DMULTISAMPLE_TYPEs
	vector <DWORD> multiSampleQualityList; // List of number of quality levels for each multisample type
	vector <UINT> presentIntervalList; // List of D3DPRESENT flags
	vector <D3D9EnumDSMSConflict> DSMSConflictList; // List of CD3D9EnumDSMSConflict

	D3D9EnumAdapterInfo* adapter_info;
	D3D9EnumDeviceInfo* device_info;
};
class D3D9EnumDeviceInfo
{
public:
	~D3D9EnumDeviceInfo();
	UINT adapter_Ordinal;
	D3DDEVTYPE device_type;
	D3DCAPS9 caps;
	vector<D3D9EnumDeviceSettingsCombo*> device_settings_comboList;
};

class D3D9EnumAdapterInfo
{
public:
	~D3D9EnumAdapterInfo();
	UINT adapter_ordianl;
	D3DADAPTER_IDENTIFIER9 adatper_identifier;
	WCHAR unique_description[256];
	vector<D3DDISPLAYMODE> display_mode_list;
	vector<D3D9EnumDeviceInfo*> device_info_list;
};
class YYUTenum
{
public:
	typedef std::tr1::function<bool(D3DCAPS9*,D3DFORMAT,D3DFORMAT,bool,void*)> IsDeviceAcceptableCallBack;
	typedef bool    (CALLBACK *LPDXUTCALLBACKISD3D9DEVICEACCEPTABLE)( D3DCAPS9* pCaps, D3DFORMAT AdapterFormat, D3DFORMAT BackBufferFormat, bool bWindowed, void* pUserContext );
	~YYUTenum(void);
	
public:
	void SetRequirPostPixelShaderBlending(bool require)
	{
		require_post_pixel_shader_blending=require;
	}
	void SetD3D9(IDirect3D9 *d3d);
	void SetHWND(HWND h) {hwnd_=h;}
	void SetIsDeviceAcceptable(IsDeviceAcceptableCallBack fuc){is_device_acceptable_callback=fuc;}
	void SetResolutionMinMax(UINT min_width,UINT min_height,UINT max_wid,UINT max_height);
	void SetRefreshMinMax(UINT min,UINT max);
	void SetMultisapleQualityMax(UINT max);
	void GetPossibleVertexProcessingList(bool *software_vp,bool *hardware_vp,bool *pure_hardware_vp,bool *minxed_vp);
	void SetPossibleVertexProcessingList(bool software_vp,bool hardware_vp,bool pure_hardware_vp,bool minxed_vp);
	vector<D3DFORMAT> *GetPossibleDepthStencilFormatList();
	vector<D3DMULTISAMPLE_TYPE> * GetPossibleMultisampleTypeList();
	vector<UINT> *GetPossiblePresentIntervalList();
	void ResetPossibleDepthStencilFormats();
	void ResetPossibleMutisampleTypelist();
	void ResetPossiblePresentIntervalList();
	bool HasEnumerated() { return has_enumerated;}
	HRESULT Enumerate();
	vector<D3D9EnumAdapterInfo*> &GetAdapterInfoList();
	void PrintAllValidType();
	D3D9EnumAdapterInfo *GetAdapterInfo(UINT adapter_ordinal);
	D3D9EnumDeviceInfo *GetDeviceInfo(UINT adapter_ordinal,D3DDEVTYPE device_type);
	bool CheckD3D9DeviceSettings(YYUTD3D9DeviceSettings *d3d_device_settings) ;
	bool FindPropertySettings(YYUTD3D9DeviceSettings *d3d_device_settings);
	D3D9EnumDeviceSettingsCombo * GetDeviceSettingsCombo(YYUTD3D9DeviceSettings *d3d9_device_settings)
	{
		return GetDeviceSettingsCombo(d3d9_device_settings->adapter_ordinal,d3d9_device_settings->device_type,d3d9_device_settings->adapter_format,d3d9_device_settings->pp.BackBufferFormat,d3d9_device_settings->pp.Windowed);
	}
	D3D9EnumDeviceSettingsCombo* GetDeviceSettingsCombo( UINT adapter_ordinal, D3DDEVTYPE device_type,
		D3DFORMAT adapter_format, D3DFORMAT backBuffer_format,
		BOOL windowed );
	static YYUTenum & GetInstance();
private:
	static YYUTenum instance;
	YYUTenum(void);
	bool require_post_pixel_shader_blending;
	bool has_enumerated;
	IDirect3D9 * d3d_;
	IsDeviceAcceptableCallBack   is_device_acceptable_callback;
	UINT min_width_;
	UINT min_height_;
	UINT max_width_;
	UINT max_height_;
	UINT refresh_rate_min_;
	UINT refresh_rate_max_;
	UINT multisample_quality_max_;
	vector<D3DFORMAT> depth_stencil_possible_list;
	vector<D3DMULTISAMPLE_TYPE> multisample_type_list;
	vector<D3D9EnumAdapterInfo*> adapter_info_list;
	vector<UINT> present_interval_list;
	bool software_vp_;
	bool hardware_vp_;
	bool pure_hareware_vp_;
	bool mixed_vp_;
	HWND hwnd_;
	void ClearAdapterInfoList();
	HRESULT EnumerateDevices(D3D9EnumAdapterInfo *adapter_info,set<D3DFORMAT> * adapter_foramt_list);
	HRESULT EnumerateDevicesCombos(D3D9EnumAdapterInfo *adapter_info,D3D9EnumDeviceInfo * device_ifno, set<D3DFORMAT> * adapter_foramt_list);
	void BuildDepthStencilFormatList(D3D9EnumDeviceSettingsCombo *dev_combo);
	void BuildMultiSampleTypeList(D3D9EnumDeviceSettingsCombo *dev_combo);
	void BuildDSMSConflictList(D3D9EnumDeviceSettingsCombo *dev_combo);
	void BuildPresetIntervalList(D3D9EnumDeviceSettingsCombo *dev_combo,D3D9EnumDeviceInfo *dev_info);
	static wstring Format2String(D3DFORMAT f);
	static wstring Device2String(D3DDEVTYPE dev);
	static wstring Windowed2String(bool windowed);
};

