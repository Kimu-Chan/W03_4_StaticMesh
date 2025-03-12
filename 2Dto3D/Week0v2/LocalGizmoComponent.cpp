#include "LocalGizmoComponent.h"
#include "ArrowComp.h"
#include "Define.h"
#include "ObjectFactory.h"

ULocalGizmoComponent::ULocalGizmoComponent()
{
	UObject* obj = FObjectFactory::ConstructObject<UArrowComp>();
	UArrowComp* ArrowX = static_cast<UArrowComp*>(obj);
	ArrowX->SetType("ArrowX");
	ArrowX->SetParent(this);
	AttachChildren.push_back(ArrowX);
	GetWorld()->GetObjectArr().push_back(ArrowX);
	ArrowArr.push_back(ArrowX);

	obj = FObjectFactory::ConstructObject<UArrowComp>();
	UArrowComp* ArrowY = static_cast<UArrowComp*>(obj);
	ArrowY->SetType("ArrowY");

	ArrowY->SetParent(this);
	ArrowY->SetDir(ARROW_DIR::AD_Y);
	AttachChildren.push_back(ArrowY);
	GetWorld()->GetObjectArr().push_back(ArrowY);
	ArrowArr.push_back(ArrowY);


	obj = FObjectFactory::ConstructObject<UArrowComp>();
	UArrowComp* ArrowZ = static_cast<UArrowComp*>(obj);
	ArrowZ->SetType("ArrowZ");
	ArrowZ->SetParent(this);
	ArrowZ->SetDir(ARROW_DIR::AD_Z);

	AttachChildren.push_back(ArrowZ);
	GetWorld()->GetObjectArr().push_back(ArrowZ);
	ArrowArr.push_back(ArrowZ);

}

ULocalGizmoComponent::~ULocalGizmoComponent()
{
}

void ULocalGizmoComponent::Initialize()
{
	Super::Initialize();
}

void ULocalGizmoComponent::Update(double deltaTime)
{
	Super::Update(deltaTime);
}

void ULocalGizmoComponent::Release()
{
}
