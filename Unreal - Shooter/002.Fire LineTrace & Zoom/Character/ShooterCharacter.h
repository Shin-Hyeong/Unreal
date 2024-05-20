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
	void Look(const FInputActionValue& value);
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
	void CameraInterpZoom(float DeltaTime);

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

private:
	/**
		Properties
	*/
	UPROPERTY()
	APlayerController* PlayerController;

	UPROPERTY()
	UAnimInstance* AnimInstance;

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
	UInputAction* LookAction;

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

	// X 감도
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float BaseTurnRate{1.0f};

	// Y 감도
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float BaseLookUpRate{1.0f};
	
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

	// 줌 인/아웃 속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat | Camera", meta = (AllowPrivateAccess = "true"))
	float ZoomSpeed{ 20.f };

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
};
