#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "HitInterface.generated.h"

UINTERFACE(MinimalAPI)
class UHitInterface : public UInterface
{
	GENERATED_BODY()
};

class SOULSLIKE__API IHitInterface
{
	GENERATED_BODY()


public:
	UFUNCTION()
	virtual void GetHit(AActor* GetHitter, const FVector& ImpactPoint, AActor* Hitter, int32 DamageType) = 0;
};
