#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "HitInterface.h"
#include "CharacterType.h"
#include "BaseCharacter.generated.h"

class UAnimMontage;
class ABaseItem;
class AWeapon;
class UParticleSystem;
class USoundBase;
class UAttributesComponent;

UCLASS()
class SOULSLIKE__API ABaseCharacter : public ACharacter, public IHitInterface
{
	GENERATED_BODY()

public:
	ABaseCharacter();

	/** <AActor> */
	virtual void Tick(float DeltaTime) override;
	/** </AActor> */
	FORCEINLINE TEnumAsByte<EDeathPose> GetDeathPose() const { return DeathPose; }
	FORCEINLINE int32 GetDamageType() const { return PlayerDamageType; }

protected:
	/** <AActor> */
	virtual void BeginPlay() override;
	/** </AActor> */

	/** <HitInterface>*/
	// DamageType : 0 = 일반 공격 / 1 = 강공격
	virtual void GetHit(AActor* GetHitter, const FVector& ImpactPoint, AActor* Hitter, int32 DamageType) override;
	/** </HitInterface>*/

	/** Combat */
	virtual void Attack();
	virtual bool CanAttack();
	virtual void HandleDamage(float DamageAmount);

	UFUNCTION(BlueprintCallable)
	void SetWeaponBoxCollision(ECollisionEnabled::Type CollisionEnabled);

	FName GetHitDirectional(const FVector& ImpactPoint);
	bool IsAlive();
	virtual void Die(const FName& HitDirection);

	void PlayHitSound(const FVector& ImpactPoint);
	void SpawnHitParticles(const FVector& ImpactPoint);
	void DisableCapusle();
	void DisableMeshCollision();

	/** Montage */
	void PlayMontageSection(UAnimMontage* Montage, const FName& SectionName);
	void StopMontage(UAnimMontage* Montage);

	// Enemy만 사용함.
	int32 PlayAttackMontage();
	// Player만 사용함.
	int32 PlayRMAttackMontage();
	UFUNCTION(BlueprintCallable)
	virtual void AttackEnd();

	int32 PlayDeathMontage(const FName& HitDirection);
	int32 GetDeathPoseSection(const FName& HitDirection);
	int32 PlayRandomMontageSection(UAnimMontage* Montage, const TArray<FName>& SectionName);

	UFUNCTION(BlueprintCallable) 
	FVector GetTranslationWarpTarget();
	UFUNCTION(BlueprintCallable) 
	FVector GetRotationWarpTarget();

	/** Check */
	bool IsJumping();

	/** Debug */
	void DirectionalDebugArrow(const FVector& CrossProduct, const FVector& Forward, const FVector& ToHit);


protected:
	UPROPERTY(BlueprintReadOnly)
	TEnumAsByte<EDeathPose> DeathPose;

	UPROPERTY(EditAnywhere, Category = "Move")
	float WalkSpeed = 400.f;

	UPROPERTY(EditAnywhere, Category = "Move")
	float RunSpeed = 750.f;

	/** Components */
	UPROPERTY(VisibleAnywhere)
	UAttributesComponent* Attributes;

	/** Combat */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	AActor* CombatTarget;

	UPROPERTY(VisibleAnywhere, Category = "Combat")
	ABaseItem* OverlappingItem;

	UPROPERTY(VisibleAnywhere, Category = "Combat")
	AWeapon* EquippedWeapon;

	UPROPERTY(EditAnywhere, Category = "Combat")
	UParticleSystem* HitParticles;

	UPROPERTY(EditAnywhere, Category = "Combat")
	USoundBase* HitSound;

	// 0 = 일반 공격 / 1 = 강 공격
	// Enemy의 공격은 무조건 0
	UPROPERTY(VisibleAnywhere, Category = Weapon) 
	int32 PlayerDamageType = 0; 

	/** Montage */
	UPROPERTY(EditAnywhere, Category = Montage)
	UAnimMontage* AttackMontage;

	UPROPERTY(EditAnywhere, Category = Montage)
	UAnimMontage* DeathMontage;

	// 플레이어 전용 Montage
	UPROPERTY(EditAnywhere, Category = Montage)
	UAnimMontage* RMAttackMontage;

	UPROPERTY(EditAnywhere, Category = Montage)
	UAnimMontage* HitSmallReactMontage;

	// Enemy 전용 Montage
	UPROPERTY(EditAnywhere, Category = Montage)
	UAnimMontage* HitLargeReactMontage;

private:
	UAnimInstance* AnimInstance;

	// Enemy가 Player를 공격할 때의 거리
	UPROPERTY(EditAnywhere, Category = "Combat")
	float WarpTargetDistance = 75.f;

	UPROPERTY(EditAnywhere, Category = Debug)
	bool bShowDirectionalDebugArrow = true;

	// AttackMontage에 있는 Sections 이름
	UPROPERTY(EditAnywhere, Category = Combat)
	TArray<FName> AttackMontageSections;
};
