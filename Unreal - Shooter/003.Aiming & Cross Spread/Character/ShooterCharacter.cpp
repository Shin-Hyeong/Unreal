#include "Character/ShooterCharacter.h"

// Input
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

// Movement
#include "GameFramework/CharacterMovementComponent.h"

// Camera
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

// Sound && Particle
#include "Kismet/GameplayStatics.h"

// Socket && Particle
#include "Engine/SkeletalMeshSocket.h"

// Particle
#include "Particles/ParticleSystemComponent.h"

// AnimMontage
#include "Animation/AnimMontage.h"

//Debug
#include "DrawDebugHelpers.h"


AShooterCharacter::AShooterCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f);
	GetCharacterMovement()->JumpZVelocity = JumpVelocity;
	// 공중에서 이동할 수 있는 비율(0 : 이동 못함. 1: 지상의 이동속도 동일)
	GetCharacterMovement()->AirControl = AirControlRate;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 180.f;
	// 컨트롤러에 의해 회전 허용
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->SocketOffset = FVector(0.f, 75.f, 50.f);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	// USpringArmComponent::SocketName : SpringArm의 시작 부분
	Camera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	// 컨트롤러에 의해 회전이 아닌 카메라붐이 움직이면서 같이 움직여야 함.
	Camera->bUsePawnControlRotation = false;
}

void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 카메라 부드럽게 줌 인&아웃
	CameraInterpZoom(DeltaTime);

	// 조준 시 마우스 감도 변경
	SetLookRate();

	// Crosshair 퍼짐 계산
	CalculateCrosshiarSpread(DeltaTime);
}

void AShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	check(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AShooterCharacter::Move);
		EnhancedInputComponent->BindAction(GamepadLookAction, ETriggerEvent::Triggered, this, &AShooterCharacter::GamepadLook);
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &AShooterCharacter::MouseLook);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AShooterCharacter::Jump);
		EnhancedInputComponent->BindAction(FireWeaponAction, ETriggerEvent::Started, this, &AShooterCharacter::FireWeapon);
		EnhancedInputComponent->BindAction(AimingAction, ETriggerEvent::Started, this, &AShooterCharacter::AimingButtonPressed);
		EnhancedInputComponent->BindAction(AimingAction, ETriggerEvent::Completed, this, &AShooterCharacter::AimingButtonReleased);
	}
}

float AShooterCharacter::GetCrosshairSpreadMultiplier() const
{
	return CrosshairSpreadMultiplier;
}

void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();
	PlayerController = Cast<APlayerController>(GetController());

	if (PlayerController)
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(IMC, 0);
		}
	}

	AnimInstance = GetMesh()->GetAnimInstance();
	
	if (Camera)
	{
		CameraDefaultFOV = GetCamera()->FieldOfView;
		CameraCurrentFOV = CameraDefaultFOV;
	}
}

/** Input */
void AShooterCharacter::Move(const FInputActionValue& value)
{
	const FVector2D MovementValue = value.Get<FVector2D>();
	// 게임패드 스틱 쏠림 현상 방지
	bool bInputX = MovementValue.X > GamepadSticklean || MovementValue.X < -GamepadSticklean;
	bool bInputY = MovementValue.Y > GamepadSticklean || MovementValue.Y < -GamepadSticklean;

	if (Controller != nullptr && (bInputX || bInputY))
	{
		const FRotator ControlRotation = GetControlRotation();
		const FRotator YawRotation(0.f, ControlRotation.Yaw, 0.f);
		
		const FVector ForwardDir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(ForwardDir, MovementValue.Y);
		AddMovementInput(RightDir, MovementValue.X);
	}
}

/** Input */
void AShooterCharacter::Jump()
{
	if (Controller != nullptr)
	{
		ACharacter::Jump();
	}
}

/** input */
void AShooterCharacter::MouseLook(const FInputActionValue& value)
{
	FVector2D LookAxisVector = value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		AddControllerYawInput(LookAxisVector.X * MouseTurnRate);
		AddControllerPitchInput(LookAxisVector.Y * MouseLookUpRate );
	}
}

/** Input */
void AShooterCharacter::GamepadLook(const FInputActionValue& value)
{
	FVector2D LookAxisVector = value.Get<FVector2D>();

	// 게임패드 스틱 쏠림 현상 방지
	bool bInputX = LookAxisVector.X > GamepadSticklean || LookAxisVector.X < -GamepadSticklean;
	bool bInputY = LookAxisVector.Y > GamepadSticklean || LookAxisVector.Y < -GamepadSticklean;

	if (Controller != nullptr && (bInputX || bInputY))
	{
		AddControllerYawInput(LookAxisVector.X * GamepadTurnRate * GetWorld()->GetDeltaSeconds());
		AddControllerPitchInput(LookAxisVector.Y * GamepadLookUpRate * GetWorld()->GetDeltaSeconds());
	}
}

/** Input */
void AShooterCharacter::FireWeapon()
{
	PlayMontage(HipFireMontage, FName("StartFire"));

	// 발사로 인해 Crosshair 퍼짐 확장
	StartCrosshairBulletFire();

	/* Call in AnimBlueprint */
	// FireBullet();
}

void AShooterCharacter::AimingButtonPressed()
{
	bAiming = true;
}

void AShooterCharacter::AimingButtonReleased()
{
	bAiming = false;
}

/** Function can call in Blueprint */
void AShooterCharacter::FireBullet()
{
	PlayFireSound(FireSound);

	const USkeletalMeshSocket* BarrelSocket = GetMesh()->GetSocketByName("BarrelSocket");
	if (BarrelSocket && MuzzleFlash)
	{
		const FTransform SocketTransform = BarrelSocket->GetSocketTransform(GetMesh());

		SpawnParticle(MuzzleFlash, SocketTransform);

		FVector BeamEnd;
		bool bHitObject(false);
		// BeamEnd set in Function GetBeamEndLocation
		bool bBeamEnd = GetBeamEndLocation(SocketTransform.GetLocation(), BeamEnd, bHitObject);

		if (bBeamEnd)
		{
			// Bullet Trajectory Tail
			BulletBeam(SocketTransform, BeamEnd); 

			// Bullet Hit Target Object Impact
			if (bHitObject)
				SpawnParticle(ImpactParticles, BeamEnd);
		}
	}
}

/** Play Montage */
void AShooterCharacter::PlayMontage(UAnimMontage* Montage, const FName& SectionName)
{
	if (AnimInstance && Montage)
	{
		AnimInstance->Montage_Play(Montage);
		AnimInstance->Montage_JumpToSection(SectionName);
	}
}

/** Camera smoothly Zoom In/Out  */
void AShooterCharacter::CameraInterpZoom(float DeltaTime)
{
	if (bAiming)
	{
		// FInterpTo(A, B, DeltaTime, Speed) : A에서 B까지의 사이값 내에서 DeltaTime * Speed 백분율 만큼 이동한 값
		CameraCurrentFOV = FMath::FInterpTo(CameraCurrentFOV, CameraZoomedFOV, DeltaTime, ZoomSpeed);
	}
	else
	{
		CameraCurrentFOV = FMath::FInterpTo(CameraCurrentFOV, CameraDefaultFOV, DeltaTime, ZoomSpeed);
	}
	GetCamera()->SetFieldOfView(CameraCurrentFOV);
}

/** Set BaseTurnRate and BaseLookUpRate based on Aiming */
void AShooterCharacter::SetLookRate()
{
	if (bAiming)
	{
		GamepadTurnRate = GamepadAimingTurnRate;
		GamepadLookUpRate = GamepadAimingLookUpRate;

		MouseTurnRate = MouseAimingTurnRate;
		MouseLookUpRate = MouseAimingLookUpRate;
	}
	else
	{
		GamepadTurnRate = GamepadHipTurnRate;
		GamepadLookUpRate = GamepadHipLookUpRate;

		MouseTurnRate = MouseHipTurnRate;
		MouseLookUpRate = MouseHipLookUpRate;
	}
}

/** Spawn Particle by Transform */
UParticleSystemComponent* AShooterCharacter::SpawnParticle(UParticleSystem* ParticleName, const FTransform& SpawnTransform)
{
	if (ParticleName)
	{
		return UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ParticleName, SpawnTransform);
	}
	return nullptr;
}

/** Spawn Particle by FVector*/
UParticleSystemComponent* AShooterCharacter::SpawnParticle(UParticleSystem* ParticleName, const FVector& SpawnVector)
{
	if (ParticleName)
	{
		return UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ParticleName, SpawnVector);
	}
	return nullptr;
}

/** Bullet Trajectory*/
void AShooterCharacter::BulletBeam(const FTransform& SocketTransform, const FVector& BeamEndPoint)
{
	UParticleSystemComponent* Beam = SpawnParticle(BeamParticles, SocketTransform);
	if (Beam)
	{
		// BeamParticle에 있는 Target FVector변수를 BeamEndPoint로 업데이트함.
		Beam->SetVectorParameter(FName("Target"), BeamEndPoint);
	}
}

/** Play Sound */
void AShooterCharacter::PlayFireSound(USoundBase* SoundName)
{
	if (SoundName)
	{
		UGameplayStatics::PlaySoundAtLocation(this, SoundName, GetActorLocation());
	}
}

/** Line Trace, Screen To World*/
bool AShooterCharacter::GetBeamEndLocation(const FVector& MuzzleSocketLocation, FVector& OutBeamLocation, bool& bHitObject)
{
	// Get Current size of the viewport
	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}
	bHitObject = false;

	// Get Screen space location of crosshair
	FVector2D CrosshairLocation{ ViewportSize.X / 2.f, ViewportSize.Y / 2.f };
	// HUD에서 Crosshair를 어깨 높이로 올리기 위해서 Y에서 50 뺌.
	// CrosshairLocation.Y -= 50.f;
	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;


	// Get World positon and Direction of Crosshairs
	// WorldPosition, WorldDirection : Output
	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(
		PlayerController,
		CrosshairLocation,
		CrosshairWorldPosition,
		CrosshairWorldDirection
	);

	// Was Deprojection Successful?
	if (bScreenToWorld)
	{
		/** Line Trace by Crosshair */
		FHitResult ScreenTraceHit;
		const FVector Start{ CrosshairWorldPosition };
		const FVector End{ CrosshairWorldPosition + CrosshairWorldDirection * WeaponRange };

		OutBeamLocation = End;

		GetWorld()->LineTraceSingleByChannel(
			ScreenTraceHit,
			Start,
			End,
			ECollisionChannel::ECC_Visibility
		);

		if (ScreenTraceHit.bBlockingHit)
		{
			OutBeamLocation = ScreenTraceHit.Location;
			bHitObject = true;
		}

		/** Line Trace by Gun Barrel */
		// Perform a second Trace, this time from the gun barrel
		FHitResult WeaponTraceHit;
		const FVector WeaponTraceStart{ MuzzleSocketLocation };
		const FVector WeaponTraceEnd{ OutBeamLocation };
		GetWorld()->LineTraceSingleByChannel(
			WeaponTraceHit,
			WeaponTraceStart,
			WeaponTraceEnd,
			ECollisionChannel::ECC_Visibility
		);

		if (WeaponTraceHit.bBlockingHit)
		{
			OutBeamLocation = WeaponTraceHit.Location;
		}

		return true;
	}

	return false;
}

void AShooterCharacter::CalculateCrosshiarSpread(float DeltaTime)
{
	FVector2D WalkSpeedRange{ 0.0f, GetCharacterMovement()->MaxWalkSpeed};
	FVector2D VelocityMultiplierRange{ 0.0f, 1.0f };
	FVector Velocity{ GetVelocity() };
	Velocity.Z = 0.f;

	// WalkSpeedRange(0 ~ 600) Clamp Range (0 ~ 1)
	// Calculate Crosshair velocity factor
	CrosshairVelocityFactor = FMath::GetMappedRangeValueClamped(WalkSpeedRange, VelocityMultiplierRange, Velocity.Size());

	// Calculate Crosshair in air factor
	// CrosshairInAirFactor를 Target까지 DeltaTime * InterpSpead 만큼 변화
	if (GetCharacterMovement()->IsFalling())
	{
		// Spread the crosshairs slowly while in air
		CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 2.25f, DeltaTime, 2.25f);
	}
	else // Character is on the ground
	{
		CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 0.f, DeltaTime, 30.f);
	}

	// Calculate crosshair Aim factor
	if (bAiming)
	{
		CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 0.6f, DeltaTime, 30.f);
	}
	else // Not Aiming
	{
		CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 0.0f, DeltaTime, 30.f);
	}

	// True ShootTimeDuration(0.05s) after firing
	if (bFiringBullet)
	{
		CrosshairShooingFactor = FMath::FInterpTo(CrosshairShooingFactor, 0.3f, DeltaTime, 60.f);
	}
	else
	{
		CrosshairShooingFactor = FMath::FInterpTo(CrosshairShooingFactor, 0.0f, DeltaTime, 60.f);
	}

	// 0.5f : 기본 Crosshair 퍼짐정도
	// CrosshairSpreadMultiplier : 0.5f + (0 ~ 1.0f) + (0 ~ 2.25f) - (0 ~ 0.6f)
	CrosshairSpreadMultiplier = 0.5f
		+ CrosshairVelocityFactor
		+ CrosshairInAirFactor
		- CrosshairAimFactor
		+ CrosshairShooingFactor;
}

void AShooterCharacter::StartCrosshairBulletFire()
{
	bFiringBullet = true;

	// ShootTimeDuration(0.05s) 이후 FinishCrossHairBulletFire()를 1번 호출 함
	GetWorldTimerManager().SetTimer(CrosshairShootTimer, 
		this, 
		&AShooterCharacter::FinishCrosshairBulletFire, 
		ShootTimeDuration
	);
}

void AShooterCharacter::FinishCrosshairBulletFire()
{
	bFiringBullet = false;
}
