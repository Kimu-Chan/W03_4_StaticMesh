#include "LocalGizmoComponent.h"
#include "ArrowComp.h"
#include "Define.h"
#include "ObjectFactory.h"

LocalGizmoComponent::LocalGizmoComponent()
{
	UObject* obj = FObjectFactory::ConstructObject<UArrowComp>();
	UArrowComp* ArrowX = static_cast<UArrowComp*>(obj);
	ArrowX->SetParent(this);
	ArrowX->SetRotation(FVector(0.0f, 90.0f, 0.0f));
	AttachChildren.push_back(ArrowX);
	GetWorld()->GetObjectArr().push_back(ArrowX);

	obj = FObjectFactory::ConstructObject<UArrowComp>();
	UArrowComp* ArrowY = static_cast<UArrowComp*>(obj);
	ArrowY->SetParent(this);
	ArrowY->SetRotation(FVector(90.0f, 0.0, 0.0f));
	AttachChildren.push_back(ArrowY);
	GetWorld()->GetObjectArr().push_back(ArrowY);


	obj = FObjectFactory::ConstructObject<UArrowComp>();
	UArrowComp* ArrowZ = static_cast<UArrowComp*>(obj);
	ArrowZ->SetParent(this);
	ArrowZ->SetRotation(FVector(0.0f, 0.0, 0.0f));
	AttachChildren.push_back(ArrowZ);
	GetWorld()->GetObjectArr().push_back(ArrowZ);

}

LocalGizmoComponent::~LocalGizmoComponent()
{
}

void LocalGizmoComponent::Initialize()
{
	Super::Initialize();
}

void LocalGizmoComponent::Update(double deltaTime)
{
	Super::Update(deltaTime);
}

void LocalGizmoComponent::Release()
{
}
