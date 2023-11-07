#include "Pawn/Bird.h"
#include "Components/CapsuleComponent.h"
#include "components/SkeletalMeshComponent.h"
#include "Components/InputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"


ABird::ABird()
{
	PrimaryActorTick.bCanEverTick = true;

	Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	// Y축(세로) 길이 반절 설정
	Capsule->SetCapsuleHalfHeight(20.f);
	// 원의 반지름(가로) 길이 설정.
	Capsule->SetCapsuleRadius(15.f);
	// RootComponent = Capsule
	SetRootComponent(Capsule);

	// SkeletalMesh 생성
	BirdMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BirdMesh"));
	BirdMesh->SetupAttachment(GetRootComponent());

	// Pawn에 Controller와 연결
	AutoPossessPlayer = EAutoReceiveInput::Player0;
}


void ABird::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem *Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(MappingContext, 0);
			UE_LOG(LogTemp, Warning, TEXT("MappingContext!"));
		}
	}
	
}

void ABird::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABird::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent *EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Binding
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ABird::Move);
	}
}

void ABird::Move(const FInputActionValue &Value)
{
	const FVector2D MovementValue = Value.Get<FVector2D>();
	if (Controller != nullptr)
	{
		// Rotator 가져옴.
		const FRotator Rotation = Controller->GetControlRotation();
		// Rotator의 Yaw만 가져옴.
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// YawRotator의 X 축 변수
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		// YawRotator의 Y 축 변수
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(ForwardDirection, MovementValue.Y);
		AddMovementInput(RightDirection, MovementValue.X);

		// Debuging
		if (MovementValue.Y > 0)
			GEngine->AddOnScreenDebugMessage(1, 2.0f, FColor::Yellow, TEXT("Forward"));
		else if (MovementValue.Y < 0)
			GEngine->AddOnScreenDebugMessage(1, 2.0f, FColor::Yellow, TEXT("Back"));
		if (MovementValue.X > 0)
			GEngine->AddOnScreenDebugMessage(1, 2.0f, FColor::Yellow, TEXT("Right"));
		else if (MovementValue.X < 0)
			GEngine->AddOnScreenDebugMessage(1, 2.0f, FColor::Yellow, TEXT("Left"));
	}
}