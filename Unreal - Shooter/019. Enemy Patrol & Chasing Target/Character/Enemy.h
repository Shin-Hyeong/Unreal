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
	virtual void BulletHit_Implementation(const FHitResult& HitResult) override;
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
		Agro
	*/
	// AgroSphere에 Overlap되면 호출
	UFUNCTION()
	void AgroSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/**
		Animations
	*/
	void PlayHitMontage(const FName& Section, float PlayRate = 1.f);

	void ResetHitReactTimer();

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

private:
	void SetPartolPoint(); 

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
	
	// HeadShot 판정 Bone
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat | Properties", meta = (AllowPrivateAccess = "true"))
	FString HeadBone;

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
		Agro
	*/
	// Sphere에 Overlap되면 Player에게 어그로가 끌림
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat | Agros", meta = (AllowPrivateAccess = "true"))
	USphereComponent* AgroSphere;

	/**
		Animation
	*/
	// 피격과 죽는 모션
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat | Animation", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* HitMontage;

	FTimerHandle HitReactTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat | Animation", meta = (AllowPrivateAccess = "true"))
	float HitReactTimeMin{ 0.75f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat | Animation", meta = (AllowPrivateAccess = "true"))
	float HitReactTimeMax{ 1.5f };

	bool bCanHitReact{ true };

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
};
