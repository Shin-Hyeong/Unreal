#include "Pawn/Bird.h"

#include "Components/CapsuleComponent.h"
#include "components/SkeletalMeshComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"


ABird::ABird()
{
	PrimaryActorTick.bCanEverTick = true;

	// Capsule Collision 객체 생성
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

	// SpringArm 객체 생성
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	// RootComponent(Capsule)에 종속 시킴
	CameraBoom->SetupAttachment(GetRootComponent());
	// SpringArm의 길이 조절
	CameraBoom->TargetArmLength = 300.f;
	
	// Camera 객체 생성
	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));
	// SpringArm에 Camera 종속시킴.
	ViewCamera->SetupAttachment(CameraBoom);

	// BP에서 bUseControllerPitch(컨트롤러 회전 Pitch 사용), bUseControllerYaw(컨트롤러 회전 Yaw 사용)를 true로 해주어야 회전 됨.

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
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ABird::Look);
	}
}

void ABird::Move(const FInputActionValue &Value)
{
	const float MovementValue = Value.Get<float>();
	if (Controller != nullptr)
	{
		// Pawn의 Forward Fector 구함.
		FVector Forward = GetActorForwardVector();

		// 입력 값에 따라 이동
		AddMovementInput(Forward, MovementValue);
	}
}

void ABird::Look(const FInputActionValue& Value)
{
	// LookAction 입력값을 FVector2D 변수 값으로 받아옴.
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// 입력 값에 따라 이동
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}
