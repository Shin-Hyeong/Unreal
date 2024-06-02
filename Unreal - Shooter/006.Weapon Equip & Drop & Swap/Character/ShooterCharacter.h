#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "GameFramework/Character.h"
#include "ShooterCharacter.generated.h"

class AnimInstance;
class UInputAction;
class UInputMappingContext;
class UCameraComponent;
class USpringArmComponent;
class USoundBase;
class UParticleSystem;
class UAnimMontage; 
class AItem;
class AWeapon;

UCLASS()
class SHOOTER_API AShooterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AShooterCharacter();

	/* <AActor>*/
	virtual void Tick(float DeltaTime) override;
	/* </AActor>*/

	/* <ACharacter>*/
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	/* </ACharacter>*/

	UFUNCTION(BlueprintCallable)
	float GetCrosshairSpreadMultiplier() const;
	
	/**
		OverlapItems
	*/
	FORCEINLINE int8 GetOverlappedItemCount() const { return OverlappedItemCount; }
	// OverlappedItemCount를 더하거나 빼서 0이 되면 bShouldTraceForItems를 false로 변경한다
	void IncrementOverlappedItemCount(int8 Amount);

	FORCEINLINE UCameraComponent* GetCamera() const { return Camera; }
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE bool GetAiming() const { return bAiming; }

protected:
	/* <AActor>*/
	virtual void BeginPlay() override;
	/* </AActor>*/

	/**
		Weapon
	*/
	// EquippedWeapon를 버리고 TraceHitItem를 대신 장착함.
	void SwapWeapon(AWeapon* WeaponToSwap);

private:
	/** Input to Controller */
	void Move(const FInputActionValue& value);
	void Jump();
	void MouseLook(const FInputActionValue& value);
	void GamepadLook(const FInputActionValue& value);
	void FireWeapon();
	void AimingButtonPressed();
	void AimingButtonReleased();
	void SelectButtonPressed();
	void SelectButtonReleased();

	UFUNCTION(BlueprintCallable)
	void FireBullet();

	/**
		Montage
	*/
	void PlayMontage(UAnimMontage* Montage, const FName& SectionName);

	/**
		Camera
	*/
	// 카메라 부르럽게 줌 인/아웃
	void CameraInterpZoom(float DeltaTime);
	// 카메라 회전율 변경
	void SetLookRate(); 

	/**
		Weapon
	*/
	// 기본 무기 생성
	AWeapon* SpawnDefaultWeapon();

	// 무기 장착
	void EquipWeapon(AWeapon* WeaponToEquip);

	// 무기 버리기
	void DropWeapon();

	/**
		Particle
	*/
	// Location by Transform
	UParticleSystemComponent* SpawnParticle(UParticleSystem* ParticleName, const FTransform& SpawnTransform);

	// Location by FVector
	UParticleSystemComponent* SpawnParticle(UParticleSystem* ParticleName, const FVector& SpawnVector);

	// 총알 궤도 표시
	void BulletBeam(const FTransform& SocketTransform, const FVector& BeamEndPoint);

	/**
		Sound
	*/
	void PlayFireSound(USoundBase* SoundName);

	/**
		Trace
	*/
	// OutBeamLocation : Output
	bool GetBeamEndLocation(const FVector& MuzzleSocketLocation, FVector& OutBeamLocation, bool& bHitObject);

	// Crosshair 밑에 Item이 있는지 감지
	// OutHitResult, OutHitLocation, bShot : Output
	bool TraceUnderCrosshairs(FHitResult& OutHitResult, FVector& OutHitLocation, bool& bShot);

	// Tick에서 사용하는 Item Widget를 보여주는 함수
	// Item의 AreaSphere에 Overlap 될 때만 보여줌.
	void TraceForItems();

	/**
		Crosshair
	*/
	// Crosshair 퍼짐 계산
	void CalculateCrosshiarSpread(float DeltaTime);

	/**
		Fire
	*/
	void StartCrosshairBulletFire();

	UFUNCTION()
	void FinishCrosshairBulletFire();

	// 발사 버튼 누르면 호출
	void FireButtonPressed();

	// 발사 버튼 때면 호출
	void FireButtonReleased();

	// 발사 Timer -> AutoFireRest 호출
	void StartFireTimer();

	// AutoFireRest -> StartFireTimer 호출 or 그냥 끝
	UFUNCTION()
	void AutoFireReset();

private:
	/**
		Properties
	*/
	UPROPERTY()
	APlayerController* PlayerController;

	UPROPERTY()
	UAnimInstance* AnimInstance;

	UPROPERTY(EditAnywhere)
	float GamepadSticklean = 0.1f;

	// 점프 높이
	UPROPERTY(EditAnywhere, Category = "Properties | Move")
	float JumpVelocity = 600.f;

	// 공중에서 제어 속도
	UPROPERTY(EditAnywhere, Category = "Properties | Move")
	float AirControlRate = 0.2;

	/**
		Input
	*/
	UPROPERTY(EditAnywhere, Category = "Input | InputAction")
	UInputMappingContext* IMC;

	UPROPERTY(EditAnywhere, Category = "Input | InputAction")
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, Category = "Input | InputAction")
	UInputAction* GamepadLookAction;

	UPROPERTY(EditAnywhere, Category = "Input | InputAction")
	UInputAction* MouseLookAction;

	UPROPERTY(EditAnywhere, Category = "Input | InputAction")
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, Category = "Input | InputAction")
	UInputAction* FireWeaponAction;

	UPROPERTY(EditAnywhere, Category = "Input | InputAction")
	UInputAction* AimingAction;

	UPROPERTY(EditAnywhere, Category = "Input | InputAction")
	UInputAction* SelectAction;

	/**
		Camera
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	// 게임패드 기본 X 감도
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float GamepadTurnRate{ 45.0f };
	// 게임패드 기본 Y 감도
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float GamepadLookUpRate{ 45.0f };

	// 마우스 기본 X감도
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float MouseTurnRate{ 1.0f };
	// 마우스 기본 Y감도
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float MouseLookUpRate{ 1.0f };

	// 줌 인/아웃 속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat | Camera", meta = (AllowPrivateAccess = "true"))
	float ZoomSpeed{ 20.f };

	// 게임패드 줌 아웃 상태 X 감도
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float GamepadHipTurnRate{ 90.f };
	// 게임패드 줌 아웃 상태 Y 감도
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float GamepadHipLookUpRate{ 90.f };

	// 게임패드 줌 인 상태 X 감도
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float GamepadAimingTurnRate{ 20.f };
	// 게임패드 줌 인 상태 Y 감도
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float GamepadAimingLookUpRate{ 20.f };

	// 마우스 줌 아웃 상태 X 감도
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float MouseHipTurnRate{ 1.0f };
	// 마우스 줌 아웃 상태 Y 감도
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float MouseHipLookUpRate{ 1.0f };

	// 마우스 줌 인 상태 X 감도
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float MouseAimingLookUpRate{ 0.2 };
	// 마우스 줌 인 상태 Y 감도
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float MouseAimingTurnRate{ 0.2 };
	
	// 줌을 하고 있는지
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat | Camera", meta = (AllowPrivateAccess = "true"))
	bool bAiming{false};

	// 기본 카메라 FOV 값
	UPROPERTY()
	float CameraDefaultFOV{ 0.f };

	// 줌 했을 때 카메라 FOV값
	UPROPERTY(EditAnywhere, Category = "Combat | Camera")
	float CameraZoomedFOV{ 45.f };

	// 현재 프레임의 FOV값
	UPROPERTY()
	float CameraCurrentFOV{ 0.f };

	/**
		Weapon
	*/
	// 현재 장착한 무기
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat | Weapon", meta = (AllowPrivateAccess = "true"))
	AWeapon* EquippedWeapon;

	// 기본 무기
	// 스폰하기 위해선 UClass 타입으로 필요함
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat | Weapon", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AWeapon> DefaultWeaponClass;

	// TraceForItems를 통해서 감지된 마지막 Item(Null일 수도 있음)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat | Weapon", meta = (AllowPrivateAccess = "true"))
	AItem* TraceHitItem;

	// 총 사정거리
	UPROPERTY(EditAnywhere, Category = "Combat | Weapon")
	float WeaponRange = 50000.f;

	/**
		AnimMontage
	*/
	// 발사 모션 Montage
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat | Montage", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* HipFireMontage;

	/**
		Sounds 
	*/
	// 발사 소리 Sound
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat | Sounds", meta =(AllowPrivateAccess = "true"))
	USoundBase* FireSound;

	/**
		Particles
	*/
	// 총구에서 빛나는 Particle
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat | Particle", meta = (AllowPrivateAccess = "true"))
	UParticleSystem* MuzzleFlash;

	// 총알 날라기는 궤도를 연기처럼 보여주는 Particle
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat | Particle", meta = (AllowPrivateAccess = "true"))
	UParticleSystem* BeamParticles;

	// 총알 탄착 지점 Particle
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat | Particle", meta = (AllowPrivateAccess = "true"))
	UParticleSystem* ImpactParticles;
	
	/**
		Crosshair
	*/
	// Crosshair 퍼짐 범위
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat | Crosshiar", meta = (AllowPrivateAccess = "true"))
	float CrosshairSpreadMultiplier{ 0.0f };

	// Crosshair 이동중 퍼짐 정도
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat | Crosshiar", meta = (AllowPrivateAccess = "true"))
	float CrosshairVelocityFactor{ 0.0f };

	// Crosshair 점프 중 퍼짐 정도
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat | Crosshiar", meta = (AllowPrivateAccess = "true"))
	float CrosshairInAirFactor{ 0.0f };

	// Crosshair 조준 중 퍼짐 정도
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat | Crosshiar", meta = (AllowPrivateAccess = "true"))
	float CrosshairAimFactor{ 0.0f };

	// Crosshair 사격중 퍼짐 정도
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat | Crosshiar", meta = (AllowPrivateAccess = "true"))
	float CrosshairShooingFactor{ 0.0f };

	/**
		Fire
	*/
	// 사격시 Crosshair 퍼지고 줄어드는걸 제어하는 Timer
	FTimerHandle CrosshairShootTimer;

	// 사격후 다시 줄어들 시간
	UPROPERTY(EditAnywhere, Category = "Combat | Crosshair", meta = (AllowPrivateAccess = "true"))
	float ShootTimeDuration{ 0.05f };

	// 자동 발사 제어 타이머
	FTimerHandle AutoFireTimer;

	bool bFiringBullet{ false };

	// 발사 버튼 누르고 있는지
	bool bFireButtonPressed{ false };

	// 발사 할지 말지, true : 발사 가능, false : 발사 불가능
	bool bShouldFire{ true };

	// 발사 속도, 
	// ShootTimerDuration 보다 커야 Crosshair가 퍼짐 축소가 일어나고 다시 발사함
	UPROPERTY(EditAnywhere, Category = "Combat | Fire", meta = (AllowPrivateAccess = "true"))
	float AutomaticFireRate{ 0.1f };

	/**
		Trace
	*/
	// 참이면 모든 프레임때 Item를 감지하는 LineTrace를 사용
	bool bShouldTraceForItems{ false };

	// 겹친 Items 수
	int8 OverlappedItemCount;

	// 마지막 Frame에 감지된 Item
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Items", meta = (AllowPrivateAccess = "true"))
	AItem* TraceHitItemLastFrame;


};
