#include "stdafx.h"
#include "YYUTFBXImport.h"
#include "YYUTMesh.h"
#include <assert.h>
#include "YYUTFBX.h"


namespace YYUT
{
	YYUTFBXImport::YYUTFBXImport()
	{
		//assert(YYUTFBXManager::GetInstance().GetInitialed() && "YYUTFBXManager is not initialise");
		if(!YYUTFBXManager::GetInstance().GetInitialed())
			YYUTFBXManager::GetInstance().Init();
	}


	YYUTFBXImport::YYUTFBXImport(string file_name):file_name_(file_name)
	{
		if(!YYUTFBXManager::GetInstance().GetInitialed())
			YYUTFBXManager::GetInstance().Init();
	}

	bool YYUTFBXImport::Load(string file_name)
	{
		if(file_name.empty())
		{
			file_name=file_name_;
		}
		else
			file_name_=file_name;
		if(file_name.empty())
			return false;
		assert(YYUTFBXManager::GetInstance().GetInitialed());
		scene_=YYUTFBXManager::GetInstance().CreateScence(file_name,"");

	}

	YYUTFBXImport::~YYUTFBXImport()
	{

	}

	YYUTObjectImportBase::ModelType YYUTFBXImport::GetType() const 
	{
		return YYUTObjectImportBase::fbx;
	}	

	bool YYUTFBXImport::Parse()
	{
		assert(scene_);

	}

}