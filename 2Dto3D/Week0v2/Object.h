#pragma once
#include "Define.h"
#include "EngineLoop.h"
#include "World.h"
#include "UClass.h"

extern FEngineLoop GEngineLoop;

class UClass;
class UWorld;
class UObject
{
public:
						UObject();
	virtual				~UObject();

	virtual void		Initialize();
	virtual void		Update(double deltaTime);
	virtual void		Release();
	virtual void		Render();
	FVector				GetForwardVector();
	FVector				GetRightVector();
	FVector				GetUpVector();
	FVector				GetLocation();
	void				SetLocation(FVector _newLoc);
	void				AddLocation(FVector _added);
	void				AddRotation(FVector _added);
	void				AddScale(FVector _added);

protected:
	FVector				m_Location;
	FVector				m_Rotation;
	FVector				m_Scale;
	UWorld*				m_World;
public:
	inline FVector				GetRotation() { return m_Rotation; }
	inline FVector				GetScale()    {	return m_Scale;    }
	inline void					SetRotation(FVector _newRot) { m_Rotation = _newRot; }
	inline void					SetScale(FVector _newScale) { m_Scale = _newScale; }
	inline UWorld*				GetWorld() { return GEngineLoop.GetWorld(); }
	inline FEngineLoop&			GetEngine() { return GEngineLoop; }

public:
	uint32 UUID;
	uint32 InternalIndex; // Index of GUObjectArray

public:
	static void* operator new(size_t size) {
		UE_LOG(LogLevel::Display, "UObject Created : %d", size);
		FEngineLoop::TotalAllocationBytes+=size;
		FEngineLoop::TotalAllocationCount++;

		UE_LOG(LogLevel::Display, "TotalAllocationBytes : %d, TotalAllocationCount : %d", FEngineLoop::TotalAllocationBytes, FEngineLoop::TotalAllocationCount);
		return std::malloc(size);
	}
	static void operator delete(void* ptr) {
		std::free(ptr);
	}
	virtual UClass* GetClass() const = 0;

	bool IsA(UClass* TargetClass) const; 
	static UClass* StaticClass() {
		static UClass ClassInfo("UObject", nullptr);
		return &ClassInfo;
	}
};	

