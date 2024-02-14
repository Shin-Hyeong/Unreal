#pragma once

#include "CoreMinimal.h"
#include "Actor/Item.h"
#include "Soul.generated.h"

UCLASS()
class UDEMYCPP_API ASoul : public AItem
{
	GENERATED_BODY()
public:
	/** <AActor> */
	virtual void Tick(float DeltaTime) override;
	/** </AActor> */

	FORCEINLINE int32 GetSouls() const { return Souls; }
	FORCEINLINE void SetSouls(int32 NumberOfSouls) { Souls = NumberOfSouls; }

protected:
	/** <AActor> */
	virtual void BeginPlay() override;
	/** </AActor> */

	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

private:
	UPROPERTY(EditAnywhere, Category = "Soul Properties")
	int32 Souls;

	UPROPERTY(EditAnywhere, Category = "Soul Properties")
	bool ShowLineTrace = false;

	UPROPERTY(EditAnywhere, Category = "Soul Properties")
	float SoulLocationZ = 50.f;

	UPROPERTY(VisibleAnywhere, Category = "Soul Properties")
	double DesiredZ;

	UPROPERTY(EditAnywhere, Category = "Soul Properties")
	float DriftRate = -30.f;
};
