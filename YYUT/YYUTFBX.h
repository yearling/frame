#ifndef __YYUTFBX_H__
#define __YYUTFBX_H__
#pragma once
#include "stdafx.h"
#include "YYUT.h"
#include <fbxsdk.h>
namespace YYUT
{
	struct YYUTFBXException:YYUTException{};
	//��FBX v2014�棬fbx sdk��֧�ֶ��̣߳�����û�в�������
	class YYUTFBXManager
	{
	public:
		YYUTFBXManager();
		~YYUTFBXManager();
		void Init();
		static YYUTFBXManager &GetInstance() { return instance_;}
		std::shared_ptr<FbxScene> CreateScence(string file_path,string scene_name);
		static YYUTFBXManager instance_;
		bool GetInitialed() const{ return initialised_;}
	private:
		FbxManager *manger_;
		FbxImporter *importer_;
		FbxIOSettings *io_setting_;
		bool      initialised_;
	};
}
#endif