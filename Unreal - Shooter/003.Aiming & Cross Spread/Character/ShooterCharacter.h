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
	
	FORCEINLINE UCameraComponent* GetCamera() const { return Camera; }
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE bool GetAiming() const { return bAiming; }

protected:
	/* <AActor>*/
	virtual void BeginPlay() override;
	/* </AActor>*/

private:
	/** Input to Controller */
	void Move(const FInputActionValue& value);
	void Jump();
	void MouseLook(const FInputActionValue& value);
	void GamepadLook(const FInputActionValue& value);
	void FireWeapon();
	void AimingButtonPressed();
	void AimingButtonReleased();

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
		LineTrace
	*/
	// OutBeamLocation : Output
	bool GetBeamEndLocation(const FVector& MuzzleSocketLocation, FVector& OutBeamLocation, bool& bHitObject);

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

	// 총 사정거리
	UPROPERTY(EditAnywhere, Category = "Combat | Weapon")
	float WeaponRange = 50000.f;

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
	// Crosshair 범위
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Crosshiar", meta = (AllowPrivateAccess = "true"))
	float CrosshairSpreadMultiplier{ 0.0f };

	// Crosshair 퍼짐 속도
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Crosshiar", meta = (AllowPrivateAccess = "true"))
	float CrosshairVelocityFactor{ 0.0f };

	// Crosshair 점프 중 퍼짐
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Crosshiar", meta = (AllowPrivateAccess = "true"))
	float CrosshairInAirFactor{ 0.0f };

	// Crosshair 조준 중 퍼짐
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Crosshiar", meta = (AllowPrivateAccess = "true"))
	float CrosshairAimFactor{ 0.0f };

	// Crosshair 사격중 퍼짐
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Crosshiar", meta = (AllowPrivateAccess = "true"))
	float CrosshairShooingFactor{ 0.0f };

	/**
		Fire
	*/
	FTimerHandle CrosshairShootTimer;

	UPROPERTY(EditAnywhere, Category = "Crosshair", meta = (AllowPrivateAccess = "true"))
	float ShootTimeDuration{ 0.05f };

	bool bFiringBullet{ false };
};
