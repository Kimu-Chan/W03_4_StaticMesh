#include "PrimitiveComponent.h"

UPrimitiveComponent::UPrimitiveComponent()
{
}

UPrimitiveComponent::UPrimitiveComponent(FString _Type) : m_Type(_Type)
{
}

UPrimitiveComponent::~UPrimitiveComponent()
{
}

void UPrimitiveComponent::Initialize()
{
	Super::Initialize();
}

void UPrimitiveComponent::Update(double deltaTime)
{
	Super::Update(deltaTime);
}

void UPrimitiveComponent::Release()
{
}

void UPrimitiveComponent::Render()
{
}
