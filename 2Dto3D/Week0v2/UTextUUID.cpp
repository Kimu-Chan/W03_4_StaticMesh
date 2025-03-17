#include "UTextUUID.h"

UTextUUID::UTextUUID()
{
}

UTextUUID::~UTextUUID()
{
}

void UTextUUID::Render()
{
	//플래그 처리

	Super::Render();
}

int UTextUUID::CheckRayIntersection(FVector& rayOrigin, FVector& rayDirection, float& pfNearHitDistance)
{
	return 0;
}

void UTextUUID::SetUUID(uint32 UUID)
{
	SetText(std::to_wstring(UUID));
}


