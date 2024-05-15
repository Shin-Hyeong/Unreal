#pragma once

#include "CoreMinimal.h"
#include "Items/BaseItem.h"
#include "PickupItem.generated.h"


UCLASS()
class SOULSLIKE__API APickupItem : public ABaseItem
{
	GENERATED_BODY()

public:
	APickupItem();

protected:
	virtual void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:
	UPROPERTY(EditAnywhere, Category = "PickUp Properties")
	int32 Exp;
};
