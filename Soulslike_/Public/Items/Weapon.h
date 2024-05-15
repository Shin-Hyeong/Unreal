#pragma once

#include "CoreMinimal.h"
#include "Items/BaseItem.h"
#include "Weapon.generated.h"

class UBoxComponent;
class UBaseSound;
class ABaseCharacter;

UCLASS()
class SOULSLIKE__API AWeapon : public ABaseItem
{
	GENERATED_BODY()

public:
	AWeapon();

	void Equip(USceneComponent* InParent, FName InSocketName, AActor* NewOwner, APawn* NewInstigator);
	void PlayEquipSound();
	void AttachMeshToSocket(USceneComponent* InParent, const FName& InSocketName);

	FORCEINLINE UBoxComponent* GetWeaponBoxComponent() const { return WeaponBox; }
	FORCEINLINE int32 GetWeaponNumber() { return WeaponNumber; }

protected:
	/** <AActor> */
	virtual void BeginPlay() override;
	/** </AActor> */

	UFUNCTION()
	void OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintImplementableEvent)
	void CreateFields(const FVector& FieldLocation);

	void GetHit(FHitResult& BoxHit);
private:
	void BoxTrace(FHitResult& BoxHit);

	/** Check */
	bool ActorIsEnemyType(AActor* OtherActor);
	bool IsOwner(AActor* OtherActor);

public:
	TArray<AActor*> IgnoreActors;

private:
	// 1 : Sword, 2 : Hammer
	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	int32 WeaponNumber;

	// 일반 공격 / 강 공격을 분리하기 위해
	UPROPERTY(VisibleAnywhere)
	ABaseCharacter* WeaponOwner;

	/** Sounds */
	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	USoundBase* EquipSound;

	/** BoxTrace */
	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	UBoxComponent* WeaponBox;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	USceneComponent* BoxTraceStart;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	USceneComponent* BoxTraceEnd;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	FVector BoxTraceExtent = FVector(5.0f);

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	bool bShowBoxDebug = false;

	/** Combat */
	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	TArray<float> Damage;
};
