#include "stdafx.h"
#include "YYUTFBX.h"
#include <sstream>
#include <iosfwd>


namespace YYUT
{


	YYUTFBXManager::YYUTFBXManager():manger_(nullptr),importer_(nullptr),io_setting_(nullptr),initialised_(false)
	{
	}

	void YYUTFBXManager::Init()
	{
		manger_=FbxManager::Create();
		io_setting_=FbxIOSettings::Create(manger_,IOSROOT);
		manger_->SetIOSettings(io_setting_);
		importer_=FbxImporter::Create(manger_,"");
		initialised_=true;
	}

	std::shared_ptr<FbxScene>  YYUTFBXManager::CreateScence(string file_path,string scene_name)
	{
		
		if(!GetInitialed())
			Init();
		//assert(manger_&&io_setting_&&importer_&&"用之前要Init");
		if(!importer_->Initialize(file_path.c_str(), -1, io_setting_)) { 
			std::stringstream ss;	
			ss<<"Call to FbxImporter::Initialize() failed.\n"; 
			ss<<"file_path:"<<file_path<<"scene_name:"<<scene_name<<endl;
			ss<<"Error resturn :"<<importer_->GetStatus().GetErrorString()<<std::endl;
			string tmp;
			ss>>tmp;
			BOOST_THROW_EXCEPTION(YYUTFBXException()<<err_str(tmp));
		}
		FbxScene *scene=FbxScene::Create(manger_,scene_name.c_str());
		if(scene==nullptr)
		{
			std::stringstream ss;
			ss<<"Create fbxscene failed!"<<std::endl;
			ss<<"file_path:"<<file_path<<"scene_name:"<<scene_name<<endl;
			string tmp;
			ss>>tmp;
			BOOST_THROW_EXCEPTION(YYUTFBXException()<<err_str(tmp));
		}
		if(!importer_->Import(scene))
		{
			std::stringstream ss;
			ss<<"import fbxscene failed!"<<std::endl;
			ss<<"file_path:"<<file_path<<"scene_name:"<<scene_name<<endl;
			string tmp;
			ss>>tmp;
			BOOST_THROW_EXCEPTION(YYUTFBXException()<<err_str(tmp));
		}
		
		return std::shared_ptr<FbxScene>(scene,[&](FbxScene *s){if(s) s->Destroy();});
	}

	YYUTFBXManager::~YYUTFBXManager()
	{
		if(importer_)
		importer_->Destroy();
		if(io_setting_)
		io_setting_->Destroy();
		if(manger_)
		manger_->Destroy();
	}

	YYUT::YYUTFBXManager YYUTFBXManager::instance_;

}