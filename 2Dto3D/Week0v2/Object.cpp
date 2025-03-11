#include "Object.h"
#include "JungleMath.h"

UObject::UObject()
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
//
//FVector UObject::GetForwardVector()
//{
//	FVector Forward = FVector(0.f,0.f,1.0f);
//	Forward = JungleMath::FVectorRotate(Forward, RelativeRotation);
//	return Forward;
//}
//
//FVector UObject::GetRightVector()
//{
//	FVector Right = FVector(1.f, 0.f, 0.0f);
//	Right = JungleMath::FVectorRotate(Right, RelativeRotation);
//	return Right;
//}
//
//
//FVector UObject::GetUpVector()
//{
//	FVector Up = FVector(0.f, 1.f, 0.0f);
//	Up = JungleMath::FVectorRotate(Up, RelativeRotation);
//	return Up;
//}
//
//FVector UObject::GetLocation()
//{
//    return RelativeLocation;
//}
//
//void UObject::SetLocation(FVector _newLoc)
//{
//    RelativeLocation = _newLoc;
//}
//
//void UObject::AddLocation(FVector _added)
//{
//	RelativeLocation = RelativeLocation + _added;
//}
//
//void UObject::AddRotation(FVector _added)
//{
//	RelativeRotation = RelativeRotation + _added;
//}
//
//void UObject::AddScale(FVector _added)
//{
//	RelativeScale3D = RelativeScale3D + _added;
//}

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
