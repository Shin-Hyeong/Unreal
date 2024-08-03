#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BulletHitInterface.h"
#include "Enemy.generated.h"

class UParticleSystem;
class USoundBase;
class UBehaviorTree;
class AEnemyController;
class USphereComponent;
class UBoxComponent;
class AShooterCharacter;

UCLASS()
class SHOOTER_API AEnemy : public ACharacter, public IBulletHitInterface
{
	GENERATED_BODY()

public:
	AEnemy();

	/** <AActor>*/
	virtual void Tick(float DeltaTime) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EvenetInstigator, AActor* DamageCauser) override;
	/** </AActor>*/

	/** <ACharacter>*/
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	/** </ACharacter>*/

	/** </IBulletHitInterface>*/
	virtual void BulletHit_Implementation(const FHitResult& HitResult, AActor* Shooter, AController* ShooterController) override;
	/** <IBulletHitInterface>*/

	/**
		UI
	*/
	UFUNCTION(BlueprintImplementableEvent)
	void ShowHitNumber(int32 Damage, FVector HitLocation, bool bHeadShot);

	UFUNCTION()
	void DestroyHitNumber(UUserWidget* HitNumber);

	void UpdateHitNumbers();

protected:
	/** <AActor>*/
	virtual void BeginPlay() override;
	/** </AActor>*/

	/**
		Default
	*/
	void Die();

	/**
		Commbat
	*/
	// 변경된 Stunned 값을 Blackboard에 전달하는 함수
	UFUNCTION(BlueprintCallable)
	void SetStunned(bool Stunned);

	// 변경된 InAttackRange 값을 Blackboard에 전달함.
	UFUNCTION(BlueprintCallable)
	void SetInAttackRange(bool InAttackRange);

	// 공격한 이후 일정 시간 이후 다시 공격 할 수 있도록 함
	UFUNCTION(BlueprintCallable)
	void ResetCanAttack();
	
	// 무기 Collision 활성화/비활성화
	UFUNCTION(BlueprintCallable)
	void ActivateLeftWeapon();
	UFUNCTION(BlueprintCallable)
	void DeactivateLeftWeapon();
	UFUNCTION(BlueprintCallable)
	void ActivateRightWeapon();
	UFUNCTION(BlueprintCallable)
	void DeactivateRightWeapon();

	/**
		Animations
	*/
	void PlayHitMontage(const FName& Section, float PlayRate = 1.f);

	void ResetHitReactTimer();

	UFUNCTION(BlueprintCallable)
	void PlayAttackMontage(const FName& Section, float PlayRate = 1.f);

	// BlueprintPure : BP에서 실행 핀이 없음
	UFUNCTION(BlueprintPure)
	FName GetAttackSectionName();

	UFUNCTION(BlueprintCallable)
	void FinishDeath();

	UFUNCTION()
	void DestroyEnemy();

	/**
		UI
	*/
	// BP에서 Override가능하도록 함.
	UFUNCTION(BlueprintNativeEvent)
	void ShowHealthBar();
	// BP에서 호출해서 사용함.
	void ShowHealthBar_Implementation();

	// BP에서 구현되도록 함.
	UFUNCTION(BlueprintImplementableEvent)
	void HideHealthBar();

	// TMap에 HitNumber의 위치를 저장함.
	UFUNCTION(BlueprintCallable)
	void StoreHitNumber(UUserWidget* HitNumber, FVector Location);

	// Overlap
	// 왼쪽 무기에 Overlap 되었을 때
	UFUNCTION()
	void OnLeftWeaponOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// 오른쪽 무기에 Overlap 되었을 때
	UFUNCTION()
	void OnRightWeaponOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// AgroSphere에 Overlap되면 호출
	UFUNCTION()
	void AgroSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// CombatRangeSphere에 BeginOverlap 되면 호출
	UFUNCTION()
	void CombatRangeOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// CombatRangeSphere에 EndOveralp 되면 호출
	UFUNCTION()
	void CombatRangeEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


private:
	// Enemy의 순찰 지역 설정
	void SetPartolPoint(); 

	// ShooterCharacter에게 데미지 줌
	void DoDamage(AShooterCharacter* Victim);

	// ShooterCharacter를 일정 확률로 기절시킴.
	void StunCharacter(AShooterCharacter* Victim);

private:
	/**
		Properties
	*/
	// 현재 HP
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat | Properties", meta = (AllowPrivateAccess = "true"))
	float Health{ 100.f };

	// 최대 HP
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat | Properties", meta = (AllowPrivateAccess = "true"))
	float MaxHealth{ 100.f };

	// 방어력
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat | Properties", meta = (AllowPrivateAccess = "true"))
	float Defensive{ 0.f };

	// 기본 공격력
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat | Properties", meta = (AllowPrivateAccess = "true"))
	float BaseDamage{ 20.f };
	
	// HeadShot 판정 Bone
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat | Properties", meta = (AllowPrivateAccess = "true"))
	FString HeadBone;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat | Properties", meta = (AllowPrivateAccess = "true"))
	bool bDead{ false };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat | Properties", meta = (AllowPrivateAccess = "true"))
	int32 Coin{ 10 };

	/**
		AI
	*/
	AEnemyController* EnemyController;

	// AI Controller에 의한 Enemy 행동 패턴
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI | Behavior", meta = (AllowPrivateAccess = "true"))
	UBehaviorTree* BehaviorTree;

	// Enemy가 이동할 PatrolPoint
	// MakeEditWidget : ViewPort상에서 이동 가능한 Widget으로 보여줌. Local space임
	//					Transform, FVector, FRotator에서 사용 가능
	UPROPERTY(EditAnywhere, Category = "AI | Behavior", meta = (AllowPrivateAccess = "true", MakeEditWidget = "true"))
	FVector PatrolPoint;

	UPROPERTY(EditAnywhere, Category = "AI | Behavior", meta = (AllowPrivateAccess = "true", MakeEditWidget = "true"))
	FVector PatrolPoint2;

	/**
		Combat
	*/
	// 총에 맞아서 기절 중이면 true
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite ,Category = "Combat | Stun", meta = (AllowPrivateAccess = "true", MakeEditWidget = "true"))
	bool bStunned{ false };

	// 기절이 걸릴 확률 0 : 기절 안함, 1 : 100% 기절함
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat | Stun", meta = (AllowPrivateAccess = "true", MakeEditWidget = "true"))
	float StunChance{ 0.3f };

	// Sphere에 Overlap되면 Player에게 어그로가 끌림
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat | Agros", meta = (AllowPrivateAccess = "true"))
	USphereComponent* AgroSphere;

	// 공격 범위 Sphere
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat | Attack", meta = (AllowPrivateAccess = "true"))
	USphereComponent* CombatRangeSphere;

	// 공격 범위에 들어오면 True
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat | Attack", meta = (AllowPrivateAccess = "true"))
	bool bInAttackRange{ false };

	// 현재 공격할 수 있으면 Ture
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat | Attack", meta = (AllowPrivateAccess = "true"))
	bool bCanAttack{ true };

	// 공격 쿨타임 제어 Timer
	FTimerHandle AttackWaitTimer;

	// 공격 쿨타임 시간
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat | Attack", meta = (AllowPrivateAccess = "true"))
	float AttackWaitTime{ 1.f };

	// 왼쪽 무기 Collsion
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat | Collision", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* LeftWeaponCollision;

	// 오른쪽 무기 Collision
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat | Collision", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* RightWeaponCollision;

	/**
		Animation
	*/
	// 피격 모션
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat | Animation", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* HitMontage;

	FTimerHandle HitReactTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat | Animation", meta = (AllowPrivateAccess = "true"))
	float HitReactTimeMin{ 0.75f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat | Animation", meta = (AllowPrivateAccess = "true"))
	float HitReactTimeMax{ 1.5f };

	bool bCanHitReact{ true };

	// 공격 모션
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat | Animation", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* AttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat | Animation", meta = (AllowPrivateAccess = "true"))
	TArray<FName> AttackSectionName;

	// 죽는 모션
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat | Animation", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* DeathMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat | Animation", meta = (AllowPrivateAccess = "true"))
	TArray<FName> DeathMontageSection;

	FTimerHandle DeathTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat | Animation", meta = (AllowPrivateAccess = "true"))
	float DeathTime{ 4.0f };

	/**
		UI
	*/
	FTimerHandle HealthBarTimer;

	// 한대 맞고 UI가 N초 후 사라지게 하는 시간
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat | UI", meta = (AllowPrivateAccess = "true"))
	float HealthBarDisplayTime{ 4.0f };

	// 데미지 표시 Widget를 키 값으로 가지고 Widget의 Location을 저장
	UPROPERTY(VisibleAnywhere, Category = "Combat | UI", meta = (AllowPrivateAccess = "true"))
	TMap<UUserWidget*, FVector> HitNumbers;

	// HitNumber 유지 시간
	UPROPERTY(EditAnywhere, BlueprintReadWrite ,Category = "Combat | UI", meta = (AllowPrivateAccess = "true"))
	float HitNumberDestoryTime{ 1.5f };

	/**
		HitInterface
	*/
	// 피격시 이펙트
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat | Impact", meta = (AllowPrivateAccess = "true"))
	UParticleSystem* ImpactParticles;

	// 피격시 사운드
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat | Impact", meta = (AllowPrivateAccess = "true"))
	USoundBase* ImpactSound;

public:	
	FORCEINLINE FString GetHeadBone() const { return HeadBone; }
	FORCEINLINE UBehaviorTree* GetBehaviorTree() const { return BehaviorTree; }
	FORCEINLINE float GetDefensive() const { return Defensive; }
};
