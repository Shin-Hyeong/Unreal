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

// AItem
#include "Items/Item.h"

// Widget
#include "Components/WidgetComponent.h"

// Debug
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
	
	// OverlappedCount를 확인해서 Item를 LineTrace로 감지함
	TraceForItems();
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
		EnhancedInputComponent->BindAction(FireWeaponAction, ETriggerEvent::Started, this, &AShooterCharacter::FireButtonPressed);
		EnhancedInputComponent->BindAction(FireWeaponAction, ETriggerEvent::Completed, this, &AShooterCharacter::FireButtonReleased);
		EnhancedInputComponent->BindAction(AimingAction, ETriggerEvent::Started, this, &AShooterCharacter::AimingButtonPressed);
		EnhancedInputComponent->BindAction(AimingAction, ETriggerEvent::Completed, this, &AShooterCharacter::AimingButtonReleased);
	}
}

float AShooterCharacter::GetCrosshairSpreadMultiplier() const
{
	return CrosshairSpreadMultiplier;
}

void AShooterCharacter::IncrementOverlappedItemCount(int8 Amount)
{
	// OverlappedItemCount가 음수가 되지 않도록 함
	if (OverlappedItemCount + Amount <= 0)
	{
		OverlappedItemCount = 0;
		bShouldTraceForItems = false;
	}
	else 
	{
		OverlappedItemCount += Amount;
		bShouldTraceForItems = true;
	}
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
		bool bHitObject{ false };
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
	// Check for crosshair trace hit
	FHitResult CrosshairHitResult;
	bool bShot{ false };
	bool bCrosshairHit = TraceUnderCrosshairs(CrosshairHitResult, OutBeamLocation, bShot);

	if (bCrosshairHit)
	{
		// Tentative beam location - still need to trace from gun
		OutBeamLocation = CrosshairHitResult.Location;
		bHitObject = true;
	}
	else // no crosshair trace hit
	{
		// OutBeamLocation is the End Location for the line trace
	}

	/** Line Trace by Gun Barrel */
		// Perform a second Trace, this time from the gun barrel
	FHitResult WeaponTraceHit;
	const FVector WeaponTraceStart{ MuzzleSocketLocation };
	// Start To End : 총구부터 CrosshairLineTrace에서 충돌 지점까지의 거리
	const FVector StartToEnd{ OutBeamLocation - MuzzleSocketLocation };

	// MuzzleSocketLocation에서 OutBeamLocation으로 LineTrace를 하면 
	// 가끔씩 거리가 부족하여 충돌이 일어나지 않을 수 있음

	// 총구에서 StartToEnd의 1.25배를 더하여 확실하게 해당 물체에 닺도록 함
	const FVector WeaponTraceEnd{ MuzzleSocketLocation + StartToEnd * 1.25f };
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

	if (bShot)
	{
		return true;
	}

	return false;

}

bool AShooterCharacter::TraceUnderCrosshairs(FHitResult& OutHitResult, FVector& OutHitLocation, bool& bShot)
{
	// Get Viewport Size
	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}

	// Get Screen space location of crosshair
	FVector2D CrosshairLocation{ ViewportSize.X / 2.f, ViewportSize.Y / 2.f };
	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;


	// Get World positon and Direction of Crosshairs
	// WorldPosition, WorldDirection : Output
	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld( 
		PlayerController,
		CrosshairLocation,
		CrosshairWorldPosition, // Output
		CrosshairWorldDirection // Output
	);

	// LineTrace
	if (bScreenToWorld)
	{
		const FVector Start{ CrosshairWorldPosition };
		const FVector End{ Start + CrosshairWorldDirection * WeaponRange };
		OutHitLocation = End;
		GetWorld()->LineTraceSingleByChannel(
			OutHitResult,
			Start,
			End,
			ECollisionChannel::ECC_Visibility
		);

		bShot = true;

		if (OutHitResult.bBlockingHit)
		{
			OutHitLocation = OutHitResult.Location;
			return true;
		}
	}

	return false;
}

void AShooterCharacter::TraceForItems()
{
	if (bShouldTraceForItems)
	{
		// 아이템을 바라보면 Widget이 보이도록 함
		FHitResult ItemTraceResult;
		FVector HitLocation;
		bool bShot;
		TraceUnderCrosshairs(ItemTraceResult, HitLocation, bShot);
		if (ItemTraceResult.bBlockingHit)
		{
			AItem* HitItem = Cast<AItem>(ItemTraceResult.GetActor());
			if (HitItem && HitItem->GetPickupWidget())
			{
				// Show Item's piuk up Widget
				HitItem->SetWidgetVisibility(true);
			}

			// 이전 frame에서 item를 감지한 상태이면
			if (TraceHitItemLastFrame)
			{
				// 현재 frame과 이전 frame과 감지하고 있는 대상이 다르면
				if (HitItem != TraceHitItemLastFrame)
				{
					// 이전 frame에서 감지했던 Item의 Widget를 비활성화 함.
					// 혹은 현재 frame에 감지 실패하여 HitItem이 Null이면
					TraceHitItemLastFrame->GetPickupWidget()->SetVisibility(false);
				}
			}

			// Cast가 실패하면 HitItem에 Null이 들어감
			// Cast가 성공해서 item이 들어가면 다음 Frame과 비교하기 위해서 저장
			TraceHitItemLastFrame = HitItem;
		}
	}
	// Item이 감지된 상태에서 AreaSphere에서 벗어나면
	else if (TraceHitItemLastFrame)
	{
		TraceHitItemLastFrame->GetPickupWidget()->SetVisibility(false);
	}
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

void AShooterCharacter::FireButtonPressed()
{
	bFireButtonPressed = true;
	StartFireTimer();
}

void AShooterCharacter::FireButtonReleased()
{
	bFireButtonPressed = false;
}

void AShooterCharacter::StartFireTimer()
{
	if (bShouldFire)
	{
		FireWeapon();
		bShouldFire = false;
		GetWorldTimerManager().SetTimer(
			AutoFireTimer, 
			this, 
			&AShooterCharacter::AutoFireReset,
			AutomaticFireRate
		);
	}
}

void AShooterCharacter::AutoFireReset()
{
	bShouldFire = true;
	if (bFireButtonPressed)
	{
		StartFireTimer(); 
	}
}
