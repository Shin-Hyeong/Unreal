#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Character/HitInterface.h"
#include "Breakalbe.generated.h"

class UCapsuleComponent;
class UGeometryCollectionComponent;
class USoundBase;

UCLASS()
class SOULSLIKE__API ABreakalbe : public AActor, public IHitInterface
{
	GENERATED_BODY()
	
public:	

	ABreakalbe();
	/** <AActor> */
	virtual void Tick(float DeltaTime) override;
	/** </AActor> */

	/** <IHitInterface> */
	virtual void GetHit(AActor* GetHitter, const FVector& ImpactPoint, AActor* Hitter, int32 DamageType) override;
	/** </IHitInterface> */

protected:
	/** <AActor> */
	virtual void BeginPlay() override;
	/** </AActor> */

private:
	void PlayBreakSound();

	UFUNCTION(BlueprintCallable)
	void DisableCapsuleCollision();

	void SpawnDropExp();
	void SpawnDropWeapon();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Break Properties")
	float DropItemLocationZ = 75.f;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UCapsuleComponent* Capsule;

private:
	UPROPERTY(VisibleAnywhere)
	UGeometryCollectionComponent* GeometryCollection;

	UPROPERTY(EditAnywhere, Category = "Break Properties")
	TArray<TSubclassOf<class AExp>> ExpClasses;

	UPROPERTY(EditAnywhere, Category = "Break Properties")
	TArray<TSubclassOf<class AWeapon>> WeaponClasses;

	UPROPERTY(EditAnywhere, Category = "Break Properties")
	USoundBase* BreakSound;

	UPROPERTY(VisibleAnywhere)
	bool bBroken = false;

	UPROPERTY(EditAnywhere)
	bool SpawnWeapon = false;
};
