#include "LocalGizmoComponent.h"
#include "ArrowComp.h"
#include "Define.h"
#include "ObjectFactory.h"
#include "DiscHellowComponent.h"
#include "Player.h"
#include "ScaleGizmoComponent.h"
ULocalGizmoComponent::ULocalGizmoComponent()
{
	UObject* obj = FObjectFactory::ConstructObject<UArrowComp>();
	UArrowComp* ArrowX = static_cast<UArrowComp*>(obj);
	ArrowX->SetType("ArrowX");
	ArrowX->SetParent(this);
	AttachChildren.push_back(ArrowX);
	//GetWorld()->GetObjectArr().push_back(ArrowX);
	ArrowArr.push_back(ArrowX);

	obj = FObjectFactory::ConstructObject<UArrowComp>();
	UArrowComp* ArrowY = static_cast<UArrowComp*>(obj);
	ArrowY->SetType("ArrowY");

	ArrowY->SetParent(this);
	ArrowY->SetDir(ARROW_DIR::AD_Y);
	AttachChildren.push_back(ArrowY);
	//GetWorld()->GetObjectArr().push_back(ArrowY);
	ArrowArr.push_back(ArrowY);


	obj = FObjectFactory::ConstructObject<UArrowComp>();
	UArrowComp* ArrowZ = static_cast<UArrowComp*>(obj);
	AttachChildren.push_back(ArrowZ);
	ArrowZ->SetType("ArrowZ");
	ArrowZ->SetParent(this);
	ArrowZ->SetDir(ARROW_DIR::AD_Z);
	AttachChildren.push_back(ArrowZ);
	//GetWorld()->GetObjectArr().push_back(ArrowZ);
	ArrowArr.push_back(ArrowZ);

	UDiscHollowComponent* disc = new UDiscHollowComponent(EPrimitiveColor::RED_X, 0.90, "DiscX");;
	disc->SetType("DiscX");
	disc->SetRotation(FVector(0.0f,0.0f,90.0f));
	disc->SetParent(this);
	AttachChildren.push_back(disc);
	//GetWorld()->GetObjectArr().push_back(disc);
	HollowDiscArr.push_back(disc);

	disc = new UDiscHollowComponent(EPrimitiveColor::GREEN_Y, 0.90, "DiscY");
	disc->SetType("DiscY");
	disc->SetParent(this);
	AttachChildren.push_back(disc);
	//GetWorld()->GetObjectArr().push_back(disc);
	HollowDiscArr.push_back(disc);


	disc = new UDiscHollowComponent(EPrimitiveColor::BLUE_Z, 0.9, "DiscZ");
	disc->SetType("DiscZ");
	disc->SetParent(this);
	disc->SetRotation(FVector(90.0f,0.0f,0.0f));
	AttachChildren.push_back(disc);
	//GetWorld()->GetObjectArr().push_back(disc);
	HollowDiscArr.push_back(disc);

	for (auto i : HollowDiscArr)
	{
		i->SetScale({ 2.5f,2.5f,2.5f });
	}

	obj = FObjectFactory::ConstructObject<UScaleGizmoComponent>();
	UScaleGizmoComponent* ScaleX = static_cast<UScaleGizmoComponent*>(obj);
	AttachChildren.push_back(ScaleX);
	ScaleX->SetType("ScaleX");
	ScaleX->SetParent(this);
	AttachChildren.push_back(ScaleX);
	//GetWorld()->GetObjectArr().push_back(ScaleX);
	ScaleArr.push_back(ScaleX);

	obj = FObjectFactory::ConstructObject<UScaleGizmoComponent>();
	UScaleGizmoComponent* ScaleY = static_cast<UScaleGizmoComponent*>(obj);
	AttachChildren.push_back(ScaleY);
	ScaleY->SetType("ScaleY");
	ScaleY->SetParent(this);
	AttachChildren.push_back(ScaleY);
	//GetWorld()->GetObjectArr().push_back(ScaleY);
	ScaleArr.push_back(ScaleY);

	obj = FObjectFactory::ConstructObject<UScaleGizmoComponent>();
	UScaleGizmoComponent* ScaleZ = static_cast<UScaleGizmoComponent*>(obj);
	AttachChildren.push_back(ScaleZ);
	ScaleZ->SetType("ScaleZ");
	ScaleZ->SetParent(this);
	AttachChildren.push_back(ScaleZ);
	//GetWorld()->GetObjectArr().push_back(ScaleZ);
	ScaleArr.push_back(ScaleZ);



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
	if (GetWorld()->GetPickingObj()) {
		SetLocation(GetWorld()->GetPickingObj()->GetWorldLocation());
	if(GetWorld()->GetPlayer()->GetCoordiMode() == CoordiMode::CDM_LOCAL)
			SetRotation(GetWorld()->GetPickingObj()->GetWorldRotation());
	else if (GetWorld()->GetPlayer()->GetCoordiMode() == CoordiMode::CDM_WORLD)
			SetRotation(FVector(0.0f,0.0f,0.0f));
	}
	for (int i = 0;i < 3;i++)
	{
		ArrowArr[i]->Update(deltaTime);
		HollowDiscArr[i]->Update(deltaTime);
		ScaleArr[i]->Update(deltaTime);
	}
}

void ULocalGizmoComponent::Release()
{
}

void ULocalGizmoComponent::Render()
{
	for (int i = 0;i < 3;i++)
	{
		ArrowArr[i]->Render();
		HollowDiscArr[i]->Render();
		ScaleArr[i]->Render();
	}
}
