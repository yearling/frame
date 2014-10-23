#ifndef __YYUTFBX_H__
#define __YYUTFBX_H__
#pragma once
#include "stdafx.h"
#include "YYUT.h"
#include <fbxsdk.h>
namespace YYUT
{
	struct YYUTFBXException:YYUTException{};
	class YYUTFBXManager
	{
	public:
		YYUTFBXManager();
		void Init();
		YYUTFBXManager &GetInstance() const{ return &instance_;}
		FbxScene *CreateScence(string file_path,string scene_name);
		static YYUTFBXManager instance_;
	private:
		FbxManager *manger_;
		FbxImporter *importer_;
		FbxIOSettings *io_setting_;
	};
}
#endif