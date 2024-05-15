#pragma once

#include "CoreMinimal.h"
#include "Character/BaseCharacter.h"
#include "Character/AcquisitionInterface.h"
#include "InputActionValue.h"
#include "PlayableCharacter.generated.h"

class UInputAction;
class UEnhancedInputComponent;
class UInputMappingContext;
class UCameraComponent;
class USpringArmComponent;
class ABaseItem;
class AWeapon;
class AEnemy;
class UPlayerOverlay;
class USettingUI;
class UGameOver;
class APlayerCameraManager;

UCLASS()
class SOULSLIKE__API APlayableCharacter : public ABaseCharacter, public IAcquisitionInterface
{
	GENERATED_BODY()

public:
	APlayableCharacter();

	/** <AActor> */
	virtual void Tick(float DeltaTime) override;
	/** </AActor> */

	/** <ACharacter> */
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void Die(const FName& HitDirection) override;
	/** </ACharacter> */

	/** <HitInterface> */
	virtual void GetHit(AActor* GetHitter, const FVector& ImpactPoint, AActor* Hitter, int32 DamageType) override;
	/** </HitInterface> */

	/** <AcquisitionInterface> */
	virtual void SetOverlappingItem(ABaseItem* Item) override;
	virtual void AddExp(class AExp* Exp) override;
	/** </AcquisitionInterface> */

	/** SettingUI */
	void SetSettingValue(float CameraValueX, float CameraValueY, bool RunToggle);
	void SetCameraArmLength(float Length);

	/** Combat */
	float TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AController* EventInstigator, AActor* DamageCauser);

	// Enemy가 죽으면 OverlayTarget & TargetEnemy가 죽은 Enmey이면 nullptr 시킴.
	void EnemyIsDead(AEnemy* Enemy);

	/** Input */
	void Walk();

	FORCEINLINE ECharacterState GetCharacterState() const { return CharacterWeaponState; }
	FORCEINLINE EPlayerState GetPlayerState() const { return CharacterState; }
	FORCEINLINE FVector2D GetMovementValue() const { return MovementValue; }

protected:
	/** <AActor> */
	virtual void BeginPlay() override;
	/** </AActor> */

private:
	/** Input */
	void Move(const FInputActionValue& value);
	void Look(const FInputActionValue& value);
	void Jump();
	void Zoom(const FInputActionValue& value);
	void Interaction();
	void Run();
	void Dodge();
	void Attack();
	void RMAttack();

	/** 
		Camera
	*/
	void LockOn();

	// 타겟 감지
	UFUNCTION()
	void CameraTrace();

	UFUNCTION()
	void EnemyTargeting();

	// 카메라와 캐릭터를 Target을 바라보도록 함
	void LockOnTarget();

	// 캐릭터만 Target을 바라보돌고 함
	void LookAtTarget(FRotator Look);

	// Start 지점에서 TargetEnemy과의 방향(Rotation)
	FRotator RotationToTarget(FVector Start);

	// Lock상태 On 변환
	void LockOnState();

	// Lock상태 Off 변환
	void LockOffState();

	/** 
		Montage
	*/
	void PlayWakeUpMontage();
	void PlayBoringMontage(float DeltaTime);
	void PlayComboAttackMontage();
	void ChangeStateToWalk();

	UFUNCTION(BlueprintCallable)
	void WakeUpEnd();

	// Boring이 끝나고 수치 초기화, 무기 생성
	UFUNCTION(BlueprintCallable)
	void BoringEnd();

	UFUNCTION()
	void DodgeDirection();

	UFUNCTION(BlueprintCallable)
	void DodgeEnd();

	UFUNCTION(BlueprintCallable)
	void ReactEnd();

	// Boring Montage 종료
	UFUNCTION(BlueprintCallable)
	void StopBoring();

	/** <BaseCharacter> */
	virtual void AttackEnd() override;
	/** </BaseCharacter> */

	/** Attributes & UI*/
	void InitializePlayerUI();
	void RegenAttributes(float DeltaTime);
	// HUD의 HealthBar Percent 변경
	void SetHUDHealth();
	// HUD의 StaminaBar Percent 변경
	void SetHUDStamina();
	void UseStaminaCost(float Cost);
	void RegenHealth(float DeltaTime);
	void RegenStamina(float DeltaTime);

	void OnSettingUI();
	void PowerAttackCoolDown(float DeltaTime);

	UFUNCTION(BlueprintCallable)
	void VisibleGameOverUI();

	/** Check */
	void PrintFPS(float DeltaTime);
	bool IsUnoccupied();
	bool IsEquipped();
	bool CanAttack(float AttackType);
	bool CanDodge();
	void SetEquippedState();
	bool IsDead();
	bool CanLockOnEnemy(AEnemy* LookTarget);

private:
	UPROPERTY(EditAnywhere, Category = "FPS")
	bool bShowFPS = true;

	UPROPERTY()
	APlayerController* PlayerController;

	UPROPERTY()
	UEnhancedInputComponent* EnhancedInputComponent;

	/**
		Input Action
	*/
	UPROPERTY(EditDefaultsOnly, Category = "Input Action")
	UInputAction* MoveAction;

	UPROPERTY(VisibleAnywhere, Category = "Input Action")
	FVector2D MovementValue;

	UPROPERTY(EditDefaultsOnly, Category = "Input Action")
	UInputAction* LookAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input Action")
	UInputAction* JumpAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input Action")
	UInputAction* ZoomAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input Action")
	UInputAction* InteractionAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input Action")
	UInputAction* DodgeAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input Action")
	UInputAction* AttackAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input Action")
	UInputAction* RMAttackAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input Action")
	UInputAction* RunAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input Action")
	UInputAction* TargetAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input Action")
	UInputAction* SettingAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input Action")
	UInputAction* ComboAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input Action")
	UInputMappingContext* IMC;

	/** 
		Camera
	*/
	UPROPERTY(VisibleAnywhere, Category = "Camera")
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, Category = "Camera")
	UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere)
	APlayerCameraManager* CameraManager;

	UPROPERTY(VisibleAnywhere, Category = "Camera | LockOn")
	bool bIsLockOnTarget = false;

	UPROPERTY(EditAnywhere, Category = "Camera | LockOn")
	bool bShowCameraTrace = false;

	UPROPERTY(EditAnywhere, Category = "Camera | LockOn")
	FVector BoxTraceExtent = FVector(5.0f, 5.0f, 5.0f);

	UPROPERTY(VisibleAnywhere, Category = "Camera | LockOn")
	AEnemy* OverlapEnemy;

	UPROPERTY(VisibleAnywhere, Category = "Camera | LockOn")
	AEnemy* TargetEnemy;

	// Enemy 탐지 최대 거리
	UPROPERTY(VisibleAnywhere, Category = "Camera | LockOn")
	float TargetingLenght = 3000.f;

	// Target과의 거리
	UPROPERTY(VisibleAnywhere, Category = "Camera | LockOn")
	float DistanceToTarget;

	// Camera Arm 길이
	UPROPERTY(EditDefaultsOnly, Category = "Input Action")
	float CameraArmLength = 330.f;

	// 줌 속도
	UPROPERTY(EditDefaultsOnly, Category = "Input Action")
	float ZoomSpeed = 10.f;

	// 최대 Camera Arm 길이
	UPROPERTY(EditDefaultsOnly, Category = "Input Action")
	float MaxCameraArmLength = 400.f;

	// 최소 Camera Arm 길이
	UPROPERTY(EditDefaultsOnly, Category = "Input Action")
	float MinCameraArmLength = 250.f;

	/** Character Component */
	UPROPERTY(EditAnywhere, Category = "Move")
	float EquippedWalkSpeed = 400.f;

	/**
		PlayerState
	*/
	UPROPERTY(VisibleAnywhere)
	ECharacterState CharacterWeaponState = ECharacterState::ECS_Unequipped;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	EPlayerState CharacterState = EPlayerState::EPS_Unoccupied;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	EEquippState EquipState = EEquippState::EES_Equipped;

	/** Montage */
	UPROPERTY(VisibleAnywhere, Category = Montage)
	bool bRunning = false;

	UPROPERTY(EditAnywhere, Category = Montage)
	UAnimMontage* StartWakeUpMontage;

	UPROPERTY(EditAnywhere, Category = Montage)
	bool bPlayWakeUp = false;

	UPROPERTY(EditAnywhere, Category = Montage)
	UAnimMontage* BoringMontage;

	UPROPERTY(EditAnywhere, Category = Montage)
	float BoringTime = 0.f;

	UPROPERTY(EditAnywhere, Category = Montage)
	float MaxBoringTime = 10.f;

	UPROPERTY(VisibleAnywhere, Category = Montage)
	bool bPlayingBoringMontage = false;

	UPROPERTY(EditAnywhere, Category = Montage)
	UAnimMontage* DodgeMontage;

	UPROPERTY(EditAnywhere, Category = Montage)
	UAnimMontage* ComboAttackMontage;

	/** Attributes & UI */
	UPROPERTY()
	UPlayerOverlay* PlayerOverlay;

	UPROPERTY(VisibleAnywhere, Category = "Attributes")
	bool bCanRegenHealth = true;

	UPROPERTY(VisibleAnywhere, Category = "Attributes")
	bool bCanRegenStamina = true;

	UPROPERTY(EditAnywhere)
	USettingUI* SettingUI;

	UPROPERTY(EditAnywhere)
	UGameOver* GameOverUI;

	UPROPERTY(EditAnywhere, Category = "Camera | Sensitivity")
	float CameraSensitivityX = 1.f;

	UPROPERTY(EditAnywhere, Category = "Camera | Sensitivity")
	float CameraSensitivityY = 1.f;

	UPROPERTY(EditAnywhere, Category = "Input Action | Setting")
	bool bRunToggle = false;

	UPROPERTY(EditAnywhere, Category = "Combat | Spell CoolDown")
	float PowerAttackMaxCool = 5.0f;

	UPROPERTY(VisibleAnywhere, Category = "Combat | Spell CoolDown")
	float PowerAttackCool = 5.0f;

	UPROPERTY(VisibleAnywhere, Category = "UI | FPS")
	float FPS;

	/** Combat */
	// Boring 이후 들고 있는 무기 다시 생성하기 위함
	UPROPERTY(EditAnywhere, Category = Weapon)
	TArray<TSubclassOf<AWeapon>> WeaponClass;

	// 무기마다 강공격 모션을 다르게 하기 위함
	UPROPERTY(EditAnywhere, Category = Weapon)
	TArray<FName> WeaponName;

	// Death React와 Death pose를 같도록 하기 위해
	UPROPERTY(EditAnywhere, Category = Weapon)
	TArray<FName> DeathName;

	UPROPERTY(EditAnywhere, Category = "Combat | Combo")
	TArray<FName> SwordCombo;

	UPROPERTY(EditAnywhere, Category = "Combat | Combo")
	TArray<FName> HammerCombo;
};
