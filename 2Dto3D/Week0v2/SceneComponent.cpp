#include "SceneComponent.h"
#include "World.h"
#include "JungleMath.h"
USceneComponent::USceneComponent() :RelativeLocation(FVector(0.f, 0.f, 0.f)), RelativeRotation(FVector(0.f, 0.f, 0.f)), RelativeScale3D(FVector(1.f, 1.f, 1.f))
{
}

USceneComponent::~USceneComponent()
{
}
void USceneComponent::Initialize()
{
	Super::Initialize();
}

void USceneComponent::Update(double deltaTime)
{
}


void USceneComponent::Release()
{
}

void USceneComponent::Render()
{
}

FVector USceneComponent::GetForwardVector()
{
	FVector Forward = FVector(0.f, 0.f, 1.0f);
	Forward = JungleMath::FVectorRotate(Forward, RelativeRotation);
	return Forward;
}

FVector USceneComponent::GetRightVector()
{
	FVector Right = FVector(1.f, 0.f, 0.0f);
	Right = JungleMath::FVectorRotate(Right, RelativeRotation);
	return Right;
}

FVector USceneComponent::GetUpVector()
{
	FVector Up = FVector(0.f, 1.f, 0.0f);
	Up = JungleMath::FVectorRotate(Up, RelativeRotation);
	return Up;
}

FVector USceneComponent::GetLocation()
{
	return RelativeLocation;
}

void USceneComponent::SetLocation(FVector _newLoc)
{
	RelativeLocation = _newLoc;
}

void USceneComponent::AddLocation(FVector _added)
{
	RelativeLocation = RelativeLocation + _added;

}

void USceneComponent::AddRotation(FVector _added)
{
	RelativeRotation = RelativeRotation + _added;

}

void USceneComponent::AddScale(FVector _added)
{
	RelativeScale3D = RelativeScale3D + _added;

}
