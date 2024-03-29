#pragma once

#include "CoreMinimal.h"
#include "Actor/Item.h"
#include "Treasure.generated.h"

class USoundBase;

UCLASS()
class UDEMYCPP_API ATreasure : public AItem
{
	GENERATED_BODY()
public:
	FORCEINLINE int32 GetGold() const { return Gold; }

protected:
	// OverlapBegin
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	// OverlapEnd
	virtual void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;
private:
	UPROPERTY(EditAnywhere, Category = "Treasure Properties")
	int32 Gold;
};
