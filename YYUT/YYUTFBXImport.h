#ifndef __YYUTFBXIMPORT_H__
#define __YYUTFBXIMPORT_H__
#pragma once
#include "stdafx.h"
#include "YYUT.h"
#include "YYUTObject.h"
#include <fbxsdk.h>
#include <memory>
namespace YYUT
{

	class YYUTFBXImport:public YYUTObjectImportBase
	{
	public:
		YYUTFBXImport();
		YYUTFBXImport(string file_name="");
		YYUTObjectImportBase::ModelType GetType() const;
		bool Load(string file_name);
		~YYUTFBXImport();
	private:
		std::shared_ptr<FbxScene> scene_;
		string file_name_;
		bool Parse();
	};
}
#endif