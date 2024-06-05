#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Weapon.generated.h"

UCLASS()
class SHOOTER_API AWeapon : public AItem
{
	GENERATED_BODY()
public:
	AWeapon();

	/** <AActor> */
	virtual void Tick(float DeltaTime) override;
	/** </AActor> */

	// Waepon에 충동시킴
	void ThrowWeapon();

protected:
	void StopFalling();

private:
	FTimerHandle ThrowWeaponTimer;
	float ThrowWeaponTime{ 0.4f };
	bool bFalling{ false };
};
