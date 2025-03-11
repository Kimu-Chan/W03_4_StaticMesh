#pragma once
#include "Object.h"
#include "EngineStatics.h"
class FObjectFactory
{
public:
	template<class T> 
	static UObject* ConstructObject()
	{
		UObject* obj = new T;
		obj->Initialize();
		obj->UUID = UEngineStatics::GenUUID();

		return obj;
	}

};

