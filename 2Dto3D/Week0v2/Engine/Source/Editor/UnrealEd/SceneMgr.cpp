﻿#include "UnrealEd/SceneMgr.h"
#include "JSON/json.hpp"
#include "UObject/Object.h"
#include "Components/SphereComp.h"
#include "Components/CubeComp.h"
#include "BaseGizmos/GizmoArrowComponent.h"
#include "UObject/ObjectFactory.h"
#include <fstream>
#include "Components/UBillboardComponent.h"
#include "Components/LightComponent.h"
#include "Components/SkySphereComponent.h"

using json = nlohmann::json;

SceneData FSceneMgr::ParseSceneData(const std::string& jsonStr)
{
    SceneData sceneData;

    try {
        json j = json::parse(jsonStr);

        // 버전과 NextUUID 읽기
        sceneData.Version = j["Version"].get<int>();
        sceneData.NextUUID = j["NextUUID"].get<int>();

        // Primitives 처리 (C++14 스타일)
        auto primitives = j["Primitives"];
        for (auto it = primitives.begin(); it != primitives.end(); ++it) {
            int id = std::stoi(it.key());  // Key는 문자열, 숫자로 변환
            const json& value = it.value();
            UObject* obj = nullptr;
            if (value.contains("Type"))
            {
                if (value["Type"].get<FString>() == "Sphere")
                {
                    obj = FObjectFactory::ConstructObject<USphereComp>();
                }
                else if (value["Type"].get<FString>() == "Cube")
                {
                    obj = FObjectFactory::ConstructObject<UCubeComp>();
                }
                else if (value["Type"].get<FString>() == "Arrow")
                {
                    obj = FObjectFactory::ConstructObject<UGizmoArrowComponent>();
                }
                else if (value["Type"].get<FString>() == "Quad")
                {
                    obj = FObjectFactory::ConstructObject<UBillboardComponent>();
                }  
                else if (value["Type"].get<FString>() == "SpotLight")
                {
                    obj = FObjectFactory::ConstructObject<ULightComponentBase>();
                }
                else if (value["Type"].get<FString>() == "SkySphere") {

                    obj = FObjectFactory::ConstructObject<USkySphereComponent>();
                    USkySphereComponent* skySphere = static_cast<USkySphereComponent*>(obj);
                    skySphere->SetTexture(L"Assets/Texture/ocean_sky.jpg");
                    skySphere->SetScale(FVector(-300.0f, -300.0f, -300.0f));
                    skySphere->SetRotation(FVector(-167.0f, 25.0f, -135.0f));
                }
               
            }
            USceneComponent* sceneComp = static_cast<USceneComponent*>(obj);
            if (value.contains("Location")) sceneComp->SetLocation(FVector(value["Location"].get<TArray<float>>()[0],
                value["Location"].get<TArray<float>>()[1],
                value["Location"].get<TArray<float>>()[2]));
            if (value.contains("Rotation")) sceneComp->SetRotation(FVector(value["Rotation"].get<TArray<float>>()[0],
                value["Rotation"].get<TArray<float>>()[1],
                value["Rotation"].get<TArray<float>>()[2]));
            if (value.contains("Scale")) sceneComp->SetScale(FVector(value["Scale"].get<TArray<float>>()[0],
                value["Scale"].get<TArray<float>>()[1],
                value["Scale"].get<TArray<float>>()[2]));
            if (value.contains("Type")) {
                UPrimitiveComponent* primitiveComp = dynamic_cast<UPrimitiveComponent*>(sceneComp);
                if (primitiveComp) {
                    primitiveComp->SetType(value["Type"].get<FString>());
                }
                else {
                    FString name = value["Type"].get<FString>();
                    sceneComp->SetName(name);
                }
            }
            sceneData.Primitives[id] = sceneComp;
        }
    }
    catch (const std::exception& e) {
        FString errorMessage = "Error parsing JSON: ";
        errorMessage += e.what();

        MessageBoxA(NULL, errorMessage.c_str(), "Error", MB_OK | MB_ICONERROR);
    }

    return sceneData;
}

FString FSceneMgr::LoadSceneFromFile(const std::string& filename)
{
    std::ifstream inFile(filename);
    if (!inFile) {
        UE_LOG(LogLevel::Error, "Failed to open file for reading: %s", filename.c_str());
        return FString();
    }

    json j;
    try {
        inFile >> j; // JSON 파일 읽기
    }
    catch (const std::exception& e) {
        UE_LOG(LogLevel::Error, "Error parsing JSON: %s", e.what());
        return FString();
    }

    inFile.close();

    return j.dump(4);
}

std::string FSceneMgr::SerializeSceneData(const SceneData& sceneData)
{
    json j;

    // Version과 NextUUID 저장
    j["Version"] = sceneData.Version;
    j["NextUUID"] = sceneData.NextUUID;

    // Primitives 처리 (C++14 스타일)
    
    for (auto it = sceneData.Primitives.begin(); it != sceneData.Primitives.end(); ++it) {
        int id = it->first;
        USceneComponent* primitive = static_cast<USceneComponent*>(it->second);
        TArray<float> Location = { primitive->GetWorldLocation().x,primitive->GetWorldLocation().y,primitive->GetWorldLocation().z };
        TArray<float> Rotation = { primitive->GetWorldRotation().x,primitive->GetWorldRotation().y,primitive->GetWorldRotation().z };
        TArray<float> Scale = { primitive->GetWorldScale().x,primitive->GetWorldScale().y,primitive->GetWorldScale().z };

        FString primitiveName = static_cast<USceneComponent*>(primitive)->GetName().ToString();
         size_t pos = primitiveName.rfind('_');
        if (pos != std::string::npos) {
            primitiveName = primitiveName.substr(0, pos);
        }
        j["Primitives"][std::to_string(id)] = {
            {"Location", Location},
            {"Rotation", Rotation},
            {"Scale", Scale},
            {"Type",primitiveName}
        };
    }

    return j.dump(4); // 4는 들여쓰기 수준
}

bool FSceneMgr::SaveSceneToFile(const std::string& filename, const SceneData& sceneData)
{
    std::ofstream outFile(filename);
    if (!outFile) {
        FString errorMessage = "Failed to open file for writing: ";
        MessageBoxA(NULL, errorMessage.c_str(), "Error", MB_OK | MB_ICONERROR);
        return false;
    }

    std::string jsonData = SerializeSceneData(sceneData);
    outFile << jsonData;
    outFile.close();

    return true;
}

