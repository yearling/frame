#include "stdafx.h"
#include "YYUTFBX.h"



namespace YYUT
{


YYUTFBXManager::YYUTFBXManager()
{

}

void YYUTFBXManager::Init()
{
	manger_=FbxManager::Create();
	io_setting_=FbxIOSettings::Create(manger_,IOSROOT);
	manger_->SetIOSettings(io_setting_);
	importer_=FbxImporter::Create(manger_,"");
}

}