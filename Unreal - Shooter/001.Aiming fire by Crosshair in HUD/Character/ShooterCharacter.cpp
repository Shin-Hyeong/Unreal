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
	CameraBoom->TargetArmLength = 300.f;
	// 컨트롤러에 의해 회전 허용
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->SocketOffset = FVector(0.f, 50.f, 50.f);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	// USpringArmComponent::SocketName : SpringArm의 시작 부분
	Camera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	// 컨트롤러에 의해 회전이 아닌 카메라붐이 움직이면서 같이 움직여야 함.
	Camera->bUsePawnControlRotation = false;
}

void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	check(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AShooterCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AShooterCharacter::Look);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AShooterCharacter::Jump);
		EnhancedInputComponent->BindAction(FireWeaponAction, ETriggerEvent::Started, this, &AShooterCharacter::FireWeapon);
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
}

/** Input */
void AShooterCharacter::Move(const FInputActionValue& value)
{
	const FVector2D MovementValue = value.Get<FVector2D>();

	if (Controller != nullptr)
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

/** Input */
void AShooterCharacter::Look(const FInputActionValue& value)
{
	FVector2D LookAxisVector = value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		AddControllerYawInput(LookAxisVector.X * BaseTurnRate);
		AddControllerPitchInput(LookAxisVector.Y * BaseLookUpRate);
	}
}

/** Input */
void AShooterCharacter::FireWeapon()
{
	PlayMontage(HipFireMontage, FName("StartFire"));

	/* Call in AnimBlueprint */
	// FireBullet();
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

		LineTraceScreenToWorld(SocketTransform);
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

/** Line Trace, Screen To World*/
void AShooterCharacter::LineTraceScreenToWorld(const FTransform& SocketTransform)
{
	// Get Current size of the viewport
	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}

	// Get Screen space location of crosshair
	FVector2D CrosshairLocation{ ViewportSize.X / 2.f, ViewportSize.Y / 2.f };
	// HUD에서 Crosshair를 어깨 높이로 올리기 위해서 Y에서 50 뺌.
	CrosshairLocation.Y -= 50.f;
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
		FHitResult ScreenTraceHit;
		const FVector Start{ CrosshairWorldPosition };
		const FVector End{ CrosshairWorldPosition + CrosshairWorldDirection * WeaponRange };

		FVector BeamEndPoint{ End };
		GetWorld()->LineTraceSingleByChannel(ScreenTraceHit,
			Start,
			End,
			ECollisionChannel::ECC_Visibility
		);

		if (ScreenTraceHit.bBlockingHit)
		{
			BeamEndPoint = ScreenTraceHit.Location;
			SpawnParticle(ImpactParticles, ScreenTraceHit.Location);
		}

		BulletBeam(SocketTransform, BeamEndPoint);
	}
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