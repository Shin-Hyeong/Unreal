#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
// HitInterface를 상속시키기 위함
#include "Interfaces/HitInterface.h"
// Enemy의 상태(Alive/Dead)
#include "Player/CharacterType.h"

#include "Enemy.generated.h"

class UAnimMontage;
class USoundBase;
class UAttributeComponent;
class UHealthBarComponent;


UCLASS()
class UDEMYCPP_API AEnemy : public ACharacter, public IHitInterface
{
	GENERATED_BODY()

public:

private:

	// Actor Attributes(Health) 컴포넌트 추가
	UPROPERTY(VisibleAnywhere)
	UAttributeComponent* Attributes;

	// 체력 표시하는 컴포넌트 추가
	UPROPERTY(VisibleAnywhere)
	UHealthBarComponent* HealthBarWidget;

	/*
	* Animation Montage
	*/
	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* HitReactMontage;

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* DeathMontage;
	
	UPROPERTY(EditAnywhere, Category = Sounds)
	USoundBase* HitSound;

	// UParticleSystem은 기본으로 지원됨. 따로 class UParticleSystem 선언 불필요
	UPROPERTY(EditAnywhere, Category = VisualEffects)
	UParticleSystem* HitParticles;

	// 공격 받고 있는 타겟
	UPROPERTY(VisibleAnywhere, Category = Target)
	AActor* CombatTarget;

	// 체력바를 표시할 원의 반지름
	UPROPERTY(EditAnywhere, Category = Target)
	double CombatRadius = 500.f;

protected:
	UPROPERTY(BlueprintReadOnly)
	EDeathPose DeathPose = EDeathPose::EDP_Alive;

public:	
	AEnemy();

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// HitInterface
	// _Implementation 붙여 Native Function으로 만듦.
	virtual void GetHit_Implementation(const FVector& ImpactPoint);

	// AActor에 내장된 TakeDamage 기능
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;


protected:
	virtual void BeginPlay() override;

	/*
	*  Play Montage functions
	*/
	// 공격 모션 재생
	void PlayHitReactMontage(const FName& SectionName);

	void Die();

private:
	// 공격 받는 방향을 구하는 함수
	void DirectionalHitReact(const FVector& ImpactPoint);
	// 공격 받는 벡터를 Debug Arrow 하는 함수
	void DirectionalDebugArrow(const FVector& CrossProduct, const FVector& Forward, const FVector& ToHit);
};
