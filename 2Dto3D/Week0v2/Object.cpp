#include "Object.h"
#include "JungleMath.h"

UObject::UObject():m_Location(FVector(0.f,0.f,0.f)),m_Rotation(FVector(0.f,0.f,0.f)),m_Scale(FVector(1.f,1.f,1.f))
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
FVector UObject::GetForwardVector()
{
	FVector Forward = FVector(0.f,0.f,1.0f);
	Forward = JungleMath::FVectorRotate(Forward, m_Rotation);
	return Forward;
}

FVector UObject::GetRightVector()
{
	FVector Right = FVector(1.f, 0.f, 0.0f);
	Right = JungleMath::FVectorRotate(Right, m_Rotation);
	return Right;
}


FVector UObject::GetUpVector()
{
	FVector Up = FVector(0.f, 1.f, 0.0f);
	Up = JungleMath::FVectorRotate(Up, m_Rotation);
	return Up;
}

FVector UObject::GetLocation()
{
    return m_Location;
}

void UObject::SetLocation(FVector _newLoc)
{
    m_Location = _newLoc;
}

void UObject::AddLocation(FVector _added)
{
	m_Location = m_Location + _added;
}

void UObject::AddRotation(FVector _added)
{
	m_Rotation = m_Rotation + _added;
}

void UObject::AddScale(FVector _added)
{
	m_Scale = m_Scale + _added;
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
