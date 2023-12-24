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
class UPawnSensingComponent;

UCLASS()
class UDEMYCPP_API AEnemy : public ACharacter, public IHitInterface
{
	GENERATED_BODY()

public:

protected:
	UPROPERTY(BlueprintReadOnly)
	EDeathPose DeathPose = EDeathPose::EDP_Alive;

private:
	/*
	*  Component
	*/

	// Actor Attributes(Health) 컴포넌트 추가
	UPROPERTY(VisibleAnywhere)
	UAttributeComponent* Attributes;

	// 체력 표시하는 컴포넌트 추가
	UPROPERTY(VisibleAnywhere)
	UHealthBarComponent* HealthBarWidget;

	// 특정 범위 내의 Pawn를 감지하는 컴포넌트
	UPROPERTY(VisibleAnywhere)
	UPawnSensingComponent* PawnSensing;

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
	double CombatRadius = 750.f;

	// 공격을 가할 범위을 나타내는 원의 반지름
	UPROPERTY(EditAnywhere, Category = Target)
	double AttackRadius = 150.f;

	/*
	*  Navigation
	*/

	// Ai Controller를 사용하고 있는지 확인하기 위한 변수
	// 여러번 AIController를 Cast하는 것을 막기 위해 변수화 시킴
	UPROPERTY()
	class AAIController* EnemyController;

	// 현재 순찰할 목적지
	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	AActor* PatrolTarget;

	// 순찰할 목적지 배열
	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	TArray<AActor*> PatrolTargets;

	// 목적지에의 수용범위
	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	double PatrolRadius = 200.f;

	// 타이머를 제어하는 TimerHandle
	FTimerHandle PatrolTimer;

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float WaitMin = 3.0f;
	
	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float WaitMax = 6.0f;

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float ChasingSpeed = 300.f;

	/*
	*  State
	*/

	// Enemy의 상태를 나타내는 변수 추가
	UPROPERTY(VisibleAnywhere)
	EEnemyState EnemyState = EEnemyState::EES_Patrolling;

public:	
	AEnemy();

	virtual void Tick(float DeltaTime) override;

	void CheckPatrolTarget();

	void CheckCombatTarget();

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

	// Target과의 거리가 Radius보다 가까운지에 따라 bool값 반환
	bool InTargetRange(AActor* Target, double Radius);

	// Target을 향해 이동하게 하는 함수
	void MoveToTarget(AActor* Target);

	// 다른 Target을 반환해주는 함수
	AActor* ChoosePatrolTarget();

	// PawnSensingComponent 안에서 Pawn을 감지하는 함수
	UFUNCTION()
	void PawnSeen(APawn* SeenPawn);

private:
	// 공격 받는 방향을 구하는 함수
	void DirectionalHitReact(const FVector& ImpactPoint);
	// 공격 받는 벡터를 Debug Arrow 하는 함수
	void DirectionalDebugArrow(const FVector& CrossProduct, const FVector& Forward, const FVector& ToHit);

	// 타이머가 끝나면 콜백하는 함수
	void PatrolTimerFinished();
};
