#include "Player/PlayerCharacter.h"

#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	// 캐릭터의 움직일때 이동 방향을 바라보도록 함.
	GetCharacterMovement()->bOrientRotationToMovement = true;
	// 캐릭터가 회전할 때 속도 (Pitch, Yaw, Roll)
	GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);

	// RootComponent가 ControllerRotation의 입력으로 회전하지 않도록 함.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 500.f;

	// SpringArm이 ControllerRotation의 입력값을 받을 수 있도록함.
	// -> RootComponent는 가만히 있지만 SpringArm은 회전할 수 있음.
	CameraBoom->bUsePawnControlRotation = true;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(CameraBoom);
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(IMC, 0);
		}
	}
}


void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Look);
	}

}

void APlayerCharacter::Move(const FInputActionValue& value)
{
	const FVector2D MovementValue = value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// 카메라 시점 기준으로 앞/뒤, 왼/오를 계산하지 않고 객체의 기준으로 이동함.
		// FVector Forward = GetActorForwardVector();
		// FVector Right = GetActorRightVector();

		// Controller가 바라보는 방향을 가져옴.
		const FRotator ControlRotation = GetControlRotation();
		// Pitch, Yaw, Roll
		// 캐릭터의 방향을 영향을 주는 것은 Yaw만 영향을 줌.
		const FRotator YawRotation(0.f, ControlRotation.Yaw, 0.f);
		
		// GetUnitAxis.X : X -> 앞으로 바라보는 방향으로 단위 벡터를 구함.
		const FVector ForwardDir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(ForwardDir, MovementValue.Y);
		AddMovementInput(RightDir, MovementValue.X);

	}
}

void APlayerCharacter::Look(const FInputActionValue& value)
{
	FVector2D LookAxisVector = value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}