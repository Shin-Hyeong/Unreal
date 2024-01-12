#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
// HitInterface�� ��ӽ�Ű�� ����
#include "Interfaces/HitInterface.h"
// DeathPose�� �����ϱ� ����
#include "Player/CharacterType.h"
#include "BaseCharacter.generated.h"

class AWeapon;
// ���� ����� ����ϱ� ���� AnimMontage
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
	void DisableMehsCollision();


	/**  Play Montage functions */
	void PlayHitReactMontage(const FName& SectionName); 
	virtual int32 PlayAttackMontage();
	virtual int32 PlayDeathMontage();
	void StopAttackMontage();

	// Root Motion���� �̵��ϱ� ������ CombatTarget�� ���� ��ġ ����
	UFUNCTION(BlueprintCallable)
	FVector GetTranslationWarpTarget();

	// Warp Target�� �ٶ󺸰� �ִ� ����, �ٶ󺸱� ���ؼ�
	UFUNCTION(BlueprintCallable)
	FVector GetRotationWarpTarget();

	UFUNCTION(BlueprintCallable) 
	virtual void AttackEnd(); 

	UFUNCTION(BlueprintCallable)
	void SetWeaponCollision(ECollisionEnabled::Type CollisionEnabled);

	/** Debug */
	// ���� �޴� ���͸� Debug Arrow �ϴ� �Լ�
	void DirectionalDebugArrow(const FVector& CrossProduct, const FVector& Forward, const FVector& ToHit);

private:
	void PlayMontageSection(UAnimMontage* Montage, const FName& SectionName); 
	int32 PlayRandomMontageSection(UAnimMontage* Montage, const TArray<FName>& SectionName); 

protected:
	/** Combat */
	// ������ ���� ����
	UPROPERTY(VisibleAnywhere, Category = Combat)
	AWeapon* EquippedWeapon;

	// ���� �ް� �ִ� Ÿ��
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = Combat) 
	AActor* CombatTarget; 

	// WarpTarget���� �Ÿ�
	UPROPERTY(EditAnywhere, Category = Combat)
	double WarpTargetDistance = 100.0f;

	// Enemy�� ������� �� ��� ���ϴ� ���� �߰�
	UPROPERTY(BlueprintReadOnly)
	TEnumAsByte<EDeathPose> DeathPose;

	/**  Component */

	// Actor Attributes(Health) ������Ʈ �߰�
	UPROPERTY(VisibleAnywhere)
	UAttributeComponent* Attributes;

private:
	UPROPERTY(EditAnywhere, Category = Combat)
	USoundBase* HitSound;

	// UParticleSystem�� �⺻���� ������. ���� class UParticleSystem ���� ���ʿ�
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
