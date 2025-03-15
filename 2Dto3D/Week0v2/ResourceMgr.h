#pragma once
#include <Windows.h>
#include <unordered_map>
#include <memory>
#include "StaticMesh.h"
#include "Define.h"

class FRenderer;
class FResourceMgr
{

public:
	void Initialize(FRenderer* renderer);
	void Release(FRenderer* renderer);
	void LoadObjAsset(FRenderer* renderer, const FString& meshName, const FWString& filepath);
	void LoadObjNormalAsset(FRenderer* renderer, const FString& meshName, const FWString& filepath);
	void RegisterMesh(FRenderer* renderer, const std::string& name, FVertexSimple* vertices, uint32 vCount, uint32* indices, uint32 iCount);
	void RegisterMesh(FRenderer* renderer, const std::string& name, TArray<FVertexSimple>& vertices, uint32 vCount, TArray<uint32>& indices, uint32 iCount);
	void RegisterMesh(FRenderer* renderer, const std::string& name, TArray<FVertexTexture>& vertices, uint32 vCount, TArray<uint32>& indices, uint32 iCount);
	std::shared_ptr<FStaticMesh> GetMesh(const FString& name) const;

private:
	std::unordered_map<std::string, std::shared_ptr<FStaticMesh>> meshMap;

};

