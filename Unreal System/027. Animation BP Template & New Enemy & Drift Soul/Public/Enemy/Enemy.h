#pragma once

#include "CoreMinimal.h"
// Enemy의 상태(Alive/Dead)
#include "Player/CharacterType.h"
// Player와 Enemy의 공통된 기능을 사용하기 위해
#include "Player/BaseCharacter.h"

#include "Enemy.generated.h"

class UHealthBarComponent;
class UPawnSensingComponent;

UCLASS()
class UDEMYCPP_API AEnemy : public ABaseCharacter
{
	GENERATED_BODY()

public:
	AEnemy();

	/** <AActor> */
	virtual void Tick(float DeltaTime) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	// Enemy가 삭제될 때 자동으로 호출됨.
	virtual void Destroyed() override;
	/** </AActor> */

	/** <IHitInterface> */
	// _Implementation 붙여 Native Function으로 만듦.
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;
	/** </IHitInterface> */

protected:
	/** <AActor> */
	virtual void BeginPlay() override;
	
	/** </AActor> */

	/** <ABaseCharacter> */
	virtual void Die_Implementation() override;
	virtual void Attack() override;
	virtual bool CanAttack() override;
	virtual void AttackEnd() override;
	// 피격 데미지 계산
	virtual void HandleDamage(float DamageAmount) override; 
	/** </ABaseCharacter> */

private:
	/** AI Behavior */
	void CheckPatrolTarget();
	void CheckCombatTarget();
	void PatrolTimerFinished();
	void HideHealthBar();
	void ShowHealthBar();
	void LoseInterest();
	void StartPatrolling();
	void ChaseTarget();
	bool IsOutsideCombatRadius();
	bool IsOutsideAttackRadius();
	bool IsInsideAttackRadius();
	bool IsChasing();
	bool IsAttacking();
	bool IsDead();
	bool IsEngaged();
	void ClearPatrolTimer(); 
	bool InTargetRange(AActor* Target, double Radius);
	void MoveToTarget(AActor* Target);
	AActor* ChoosePatrolTarget(); 
	void SpawnDefaultWeapon();

	/** <PawnSensingComponent> */
	// PawnSensingComponent 안에서 Pawn을 감지하는 함수
	UFUNCTION()
	void PawnSeen(APawn* SeenPawn);
	/** </PawnSensingComponent> */

	/**  Combat */
	void StartAttackTimer();
	void CleaerAttackTimer();
	void SpawnSoul();

protected:
	/**  State */
	// Enemy의 상태를 나타내는 변수 추가
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	EEnemyState EnemyState = EEnemyState::EES_Patrolling;

private:
	// 장착할 무기
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AWeapon> WeaponClass;

	/**  Component */
	// 체력 표시하는 컴포넌트 추가
	UPROPERTY(VisibleAnywhere)
	UHealthBarComponent* HealthBarWidget;

	// 특정 범위 내의 Pawn를 감지하는 컴포넌트
	UPROPERTY(VisibleAnywhere)
	UPawnSensingComponent* PawnSensing;

	/**  Combat */
	FTimerHandle AttackTimer; 

	// 체력바를 표시할 원의 반지름
	UPROPERTY(EditAnywhere, Category = Combat)
	double CombatRadius = 750.f;

	// 공격을 가할 범위을 나타내는 원의 반지름
	UPROPERTY(EditAnywhere, Category = Combat)
	double AttackRadius = 150.f;

	// Target에 도착 수용 범위
	UPROPERTY(EditAnywhere, Category = Combat)
	double AcceptanceRadius = 50.f;

	UPROPERTY(EditAnywhere, Category = Combat)
	float AttackMin = 0.2f; 

	UPROPERTY(EditAnywhere, Category = Combat)
	float AttackMax = 0.5f;

	UPROPERTY(EditAnywhere, Category = Combat)
	float DeathLifeSpan = 5.0f;

	UPROPERTY(EditAnywhere, Category = Combat)
	float SoulLocationZ = 0.f;

	UPROPERTY(EditAnywhere, Category = Combat)
	TSubclassOf<class ASoul> SoulClass;

	/**  Navigation */
	// Ai Controller를 사용하고 있는지 확인하기 위한 변수
	// 여러번 AIController를 Cast하는 것을 막기 위해 변수화 시킴
	UPROPERTY()
	class AAIController* EnemyController;

	// 타이머를 제어하는 TimerHandle
	FTimerHandle PatrolTimer; 

	// 현재 순찰할 목적지
	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	AActor* PatrolTarget;

	// 순찰할 목적지 배열
	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	TArray<AActor*> PatrolTargets;

	// 목적지에의 수용범위
	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	double PatrolRadius = 200.f;

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float PatrolWaitMin = 3.0f;
	
	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float PatrolWaitMax = 6.0f;

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float PatrollingSpeed = 125.f;

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float ChasingSpeed = 300.f;

	UPROPERTY(EditAnywhere, Category = "Ai Navigation")
	bool VisibilityTarget = false;
};
