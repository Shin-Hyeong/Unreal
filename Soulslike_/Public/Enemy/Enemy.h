#pragma once

#include "CoreMinimal.h"
#include "Character/BaseCharacter.h"
#include "Character/CharacterType.h"
#include "Enemy.generated.h"

class UHealthBarComponent;
class AAIController;
class AWeapon;
class UPawnSensingComponent;

UCLASS()
class SOULSLIKE__API AEnemy : public ABaseCharacter
{
	GENERATED_BODY()

public:
	AEnemy();

	/** <AActor>*/
	virtual void Tick(float DeltaTime) override;
	virtual void Destroyed() override;
	/** </AActor>*/

	/** <HitInterface>*/
	// DamageType : 0 = 일반 공격 / 1 = 강공격
	virtual void GetHit(AActor* GetHitter, const FVector& ImpactPoint, AActor* Hitter, int32 DamageType) override;
	/** </HitInterface>*/

	/** Combat */
	float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser);

	/** HUD */
	void ShowHealthBar();
	void HideHealthBar();

	/** Check */
	bool IsDead();

	FORCEINLINE EEnemyState GetEnemyState() const { return EnemyState; }

protected:
	/** <AActor>*/
	virtual void BeginPlay() override;
	/** </AActor>*/

	/** <BaseCharacter>*/
	virtual void Attack() override;
	virtual bool CanAttack() override;
	virtual void AttackEnd() override;
	virtual void Die(const FName& HitDirection) override;
	virtual void HandleDamage(float DamageAmount) override;
	/** </BaseCharacter>*/

private:
	/**
		AI Behavior
	*/
	/** Patrol */
	// 게임이 처음 시작할 때 Patrol 목적지로 향해 이동하도록 함.
	void StartPatrolling();
	// Patrol 목적지에 도착하면 일정시간 지나고 새로운 목적지를 지정하고 이동시킴
	void CheckPatrolTarget();
	// Patrol 목적지로 이동 명령함.
	void PatrolTimerFinished();
	// Patrol 목적지를 이동 명령하는 Timer를 초기화 시킴.
	void ClearPatrolTimer();
	// Target으로 이동하도록 함.
	void MoveToTarget(AActor* Target);
	// Partrols 리스트에서 Target를 골름.
	AActor* ChoosePatrolTarget();

	///** Chasing */
	void CheckCombatTarget();
	void LoseInterest();
	void ChaseTarget();

	/** Combat */
	void StartAttackTimer();
	void ClearAttackTimer();
	void SpawnExp();

	/** <PawnSensingComponent> */
	UFUNCTION()
	void PawnSeen(APawn* SeenPawn);
	/** </PawnSensingComponent> */

	/** Check */
	bool IsOutsideCombatRadius();
	bool IsOutsideAttackRadius();
	bool IsInsideAttackRadius();
	bool IsChasing();
	bool IsAttacking();
	bool IsEngaged();

	bool InTargetRange(AActor* Target, double Radius);
	void SpawnEnemyWeapon();

protected:


private:
	/**
	*	Navigation
	*/

	UPROPERTY()
	AAIController* EnemyController;

	UPROPERTY(EditAnywhere, Category = "Drop Item")
	TSubclassOf<class AExp> ExpClass;

	// Exp 스폰될 높이
	UPROPERTY(EditAnywhere, Category = "Drop Item")
	float ExpLocationZ = 0.f;

	/** Patrol */
	FTimerHandle PatrolTimer;

	UPROPERTY(EditAnywhere, Category = "AI Navigation | Patrol")
	AActor* PatrolTarget; 

	UPROPERTY(EditAnywhere, Category = "AI Navigation | Patrol")
	TArray<AActor*> PatrolTargets; 

	// Target근처에 도착하여 다음 목적지를 지정하고 이동하기 위함.
	UPROPERTY(EditAnywhere, Category = "AI Navigation | Patrol")
	double PatrolRadius = 200.f;
	
	// 목적지에 도착하고 다음 목적지로 출발하기 까지 최소시간
	UPROPERTY(EditAnywhere, Category = "AI Navigation | Patrol")
	float PatrolWaitMin = 3.0f;

	// 목적지에 도착하고 다음 목적지로 출발하기 까지 최대시간
	UPROPERTY(EditAnywhere, Category = "AI Navigation | Patrol")
	float PatrolWaitMax = 6.0f;

	// Patrol중일 떄 Enemy의 이동속도
	UPROPERTY(EditAnywhere, Category = "AI Navigation | Patrol")
	float PatrollingSpeed = 150.f;

	// MoveTo를 의해 이동하는데 도착했음을 인정하는 범위
	UPROPERTY(EditAnywhere, Category = Combat)
	double AcceptanceRadius = 50.f;

	/** Chasing  */

	// 추격중일 떄 Enemy 이동속도
	UPROPERTY(EditAnywhere, Category = "AI Navigation | Chase")
	float ChasingSpeed = 400.f;

	/**
		Combat
	*/
	FTimerHandle AttackTimer;

	// Enemy Weapon
	UPROPERTY(EditAnywhere,  Category = "AI Navigation | Combat")
	TSubclassOf<AWeapon> WeaponClass;

	// Enemy가 CombatTarget를 추격하는 범위
	UPROPERTY(EditAnywhere, Category = "AI Navigation | Combat")
	double CombatRadius = 750.f;

	// Enemy가 CombatTarget를 공격할 수 있는 범위
	UPROPERTY(EditAnywhere, Category = "AI Navigation | Combat")
	double AttackRadius = 150.f;

	// Enemy의 최소 공격 쿨타임
	UPROPERTY(EditAnywhere, Category = "AI Navigation | Combat")
	float AttackMin = 0.2f;

	// Enmey의 최대 공격 쿨타임
	UPROPERTY(EditAnywhere, Category = "AI Navigation | Combat")
	float AttackMax = 0.5f;

	// 사망 후 Enemy 제거
	UPROPERTY(EditAnywhere, Category = "AI Navigation | Combat")
	float DeathLifeSpan = 5.0f;

	// 플레이어로 부터 받은 일반 / 강공격에 따른 다른 HitReact를 위함.
	UPROPERTY(VisibleAnywhere, Category = "Ai Navigation | Combat")
	int32 HitReactType = 0;

	/**
		Enemy State
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	EEnemyState EnemyState = EEnemyState::EES_Patrolling;

	/**
		Components
	*/
	UPROPERTY(EditAnywhere)
	bool bShowStartHUD = false;

	UPROPERTY(VisibleAnywhere)
	UHealthBarComponent* HealthBarWidget;
	
	UPROPERTY(VisibleAnywhere)
	UPawnSensingComponent* PawnSensing;
};
