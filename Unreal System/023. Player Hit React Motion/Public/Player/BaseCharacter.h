#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
// HitInterface를 상속시키기 위함
#include "Interfaces/HitInterface.h"
#include "BaseCharacter.generated.h"

class AWeapon;
// 공격 모션을 사용하기 위한 AnimMontage
class UAnimMontage;
class UAttributeComponent;
class USoundBase;


UCLASS()
class UDEMYCPP_API ABaseCharacter : public ACharacter, public IHitInterface
{
	GENERATED_BODY()

public:
	ABaseCharacter();

	/** <AActor> */
	virtual void Tick(float DeltaTime) override;
	/** </AActor> */

protected:
	/** <AActor> */
	virtual void BeginPlay() override;	
	/** </AActor> */

	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter);
	virtual void Attack();
	virtual bool CanAttack(); 
	bool IsAlive(); 
	virtual void Die(); 
	void DirectionalHitReact(const FVector& ImpactPoint); 
	virtual void HandleDamage(float DamageAmount); 
	void PlayHitSound(const FVector& ImpactPoint); 
	void SpawnHitParticles(const FVector& ImpactPoint); 
	void DisableCapsule();


	/**  Play Montage functions */
	void PlayHitReactMontage(const FName& SectionName); 
	virtual int32 PlayAttackMontage();
	virtual int32 PlayDeathMontage();

	/** Debug */
	// 공격 받는 벡터를 Debug Arrow 하는 함수
	void DirectionalDebugArrow(const FVector& CrossProduct, const FVector& Forward, const FVector& ToHit);

	UFUNCTION(BlueprintCallable) 
	virtual void AttackEnd(); 

	UFUNCTION(BlueprintCallable)
	void SetWeaponCollision(ECollisionEnabled::Type CollisionEnabled);

private:
	void PlayMontageSection(UAnimMontage* Montage, const FName& SectionName); 
	int32 PlayRandomMontageSection(UAnimMontage* Montage, const TArray<FName>& SectionName); 

protected:
	// 장착한 무기 변수
	UPROPERTY(VisibleAnywhere, Category = Weapon)
	AWeapon* EquippedWeapon;

	/**  Component */

	// Actor Attributes(Health) 컴포넌트 추가
	UPROPERTY(VisibleAnywhere)
	UAttributeComponent* Attributes;

private:
	UPROPERTY(EditAnywhere, Category = Combat)
	USoundBase* HitSound;

	// UParticleSystem은 기본으로 지원됨. 따로 class UParticleSystem 선언 불필요
	UPROPERTY(EditAnywhere, Category = Combat)
	UParticleSystem* HitParticles;

	/** Animation Montage */
	UPROPERTY(EditDefaultsOnly, Category = Combat)
	UAnimMontage* AttackMontage;

	UPROPERTY(EditDefaultsOnly, Category = Combat)
	UAnimMontage* HitReactMontage;

	UPROPERTY(EditDefaultsOnly, Category = Combat)
	UAnimMontage* DeathMontage;

	UPROPERTY(EditAnywhere, Category = Montages)
	TArray<FName> AttackMontageSections;

	UPROPERTY(EditAnywhere, Category = Montages)
	TArray<FName> DeathMontageSections;
};
