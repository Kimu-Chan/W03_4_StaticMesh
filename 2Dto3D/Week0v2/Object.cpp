#include "Object.h"
#include "JungleMath.h"



UObject::UObject() : Name(FName("DefaultObjectName"))
{
}

UObject::UObject(FString& name) : Name(FName(name))
{
}

UObject::UObject(FString& name, uint32 uuid) : Name(FName(name)), UUID(uuid)
{
}

UObject::~UObject()
{
}

void UObject::Initialize()
{
	InternalIndex = GetWorld()->GetObjectArr().size() - 1;
}

void UObject::Update(double deltaTime)
{
}
void UObject::Release()
{
}
void UObject::Render()
{
}
bool UObject::IsA(UClass* TargetClass) const
{
	UClass* CurrentClass = GetClass();
	while (CurrentClass) {
		if (CurrentClass == TargetClass)
			return true;
		CurrentClass = CurrentClass->ParentClass;
	}
	return false;
}
