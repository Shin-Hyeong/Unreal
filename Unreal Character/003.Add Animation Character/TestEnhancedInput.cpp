#include "TestEnhancedInput.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "InputAction.h"
#include "InputMappingContext.h"
/*AddOnScreenDebugMessage*/
#include "Engine/Engine.h"

// Sets default values
ATestEnhancedInput::ATestEnhancedInput()
{	
	/*Variable Setting*/
	ZoomVariable = 15.0f;

	//PrimaryActorTick.bCanEverTick = true;
	AutoPossessPlayer = EAutoReceiveInput::Player0;

	/*Character Setting*/
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 640.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	/*Found InputMappingContext*/
	static ConstructorHelpers::FObjectFinder<UInputMappingContext> MappingContext(TEXT("/Game/Input/IMC_Test"));
	if (MappingContext.Succeeded()){ TestMappingContext = MappingContext.Object; }

	/*Found InputAction*/
	static ConstructorHelpers::FObjectFinder<UInputAction> IA_Jump(TEXT("/Game/Input/Action/IA_Jump"));
	if (IA_Jump.Succeeded()){ JumpAction = IA_Jump.Object; }
	static ConstructorHelpers::FObjectFinder<UInputAction> IA_Move(TEXT("/Game/Input/Action/IA_Move"));
	if (IA_Move.Succeeded()){ MoveAction = IA_Move.Object; }
	static ConstructorHelpers::FObjectFinder<UInputAction> IA_Look(TEXT("/Game/Input/Action/IA_Look"));
	if (IA_Look.Succeeded()){ LookAction = IA_Look.Object; }
	static ConstructorHelpers::FObjectFinder<UInputAction> IA_Zoom(TEXT("/Game/Input/Action/IA_Zoom"));
	if (IA_Zoom.Succeeded()){ ZoomAction = IA_Zoom.Object; }

	/*CameraBoom*/
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 500.0f;
	CameraBoom->bUsePawnControlRotation = true;

	/*Camera*/
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	/*SkeletalMesh*/
	// SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("USkeletalMeshComponent"));
	// SkeletalMesh->SetupAttachment(RootComponent);
	//static ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshAsset(TEXT("/Game/Character/StaticMesh/Skeletal/X_Bot"));
	// if (MeshAsset.Succeeded() && MeshAsset.Object != NULL){
	// 	SkeletalMesh->SetSkeletalMesh(MeshAsset.Object);
	// 	SkeletalMesh->SetRelativeLocation(FVector(0.0f, 0.0f, -89.0f));
	// 	SkeletalMesh->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	// 	SkeletalMesh->SetWorldScale3D(FVector(1.0f));
	// }

	/*Found AnimClass(Anim BluePrint)*/
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimInstance(TEXT("/Game/Character/Animation/ABP_Xbot"));
	if (AnimInstance.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(AnimInstance.Class);
	}
}

void ATestEnhancedInput::BeginPlay()
{
	Super::BeginPlay();
	/*Controller Cast PlayerController*/
	if (APlayerController *PlayerController = Cast<APlayerController>(GetController())){
		if (UEnhancedInputLocalPlayerSubsystem *Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer())){
			Subsystem->AddMappingContext(TestMappingContext, 0);
		}
	}
}

//void ATestEnhancedInput::Tick(float DeltaTime){
//	Super::Tick(DeltaTime);
//}

void ATestEnhancedInput::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	/*BindAction*/
	if (UEnhancedInputComponent *EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)){
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ATestEnhancedInput::Jump);
		//EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ATestEnhancedInput::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ATestEnhancedInput::Look);
		EnhancedInputComponent->BindAction(ZoomAction, ETriggerEvent::Triggered, this, &ATestEnhancedInput::Zoom);
	}
}

/*Character Move*/
void ATestEnhancedInput::Move(const FInputActionValue &Value)
{
	/*MoveAction이 Bool형식임으로 MoveAction 값을 CurrentValue에 넣음.*/
	const FVector2D MovementValue = Value.Get<FVector2D>();
	if (Controller != nullptr){
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(ForwardDirection, MovementValue.Y);
		AddMovementInput(RightDirection, MovementValue.X);

		if (MovementValue.Y > 0) GEngine->AddOnScreenDebugMessage(-1, 0.005f, FColor::Yellow, TEXT("Forward"));
		else if (MovementValue.Y < 0) GEngine->AddOnScreenDebugMessage(-1, 0.005f, FColor::Yellow, TEXT("Back"));
		if (MovementValue.X > 0) GEngine->AddOnScreenDebugMessage(-1, 0.005f, FColor::Yellow, TEXT("Right"));
		else if (MovementValue.X < 0) GEngine->AddOnScreenDebugMessage(-1, 0.005f, FColor::Yellow, TEXT("Left"));
	}
}

/*Camera Movement*/
void ATestEnhancedInput::Look(const FInputActionValue &Value){
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr){
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);

		if (LookAxisVector.Y > 0) GEngine->AddOnScreenDebugMessage(-1, 0.005f, FColor::Yellow, TEXT("LookUp"));
		else if (LookAxisVector.Y < 0) GEngine->AddOnScreenDebugMessage(-1, 0.005f, FColor::Yellow, TEXT("LookDown"));
		if (LookAxisVector.X > 0) GEngine->AddOnScreenDebugMessage(-1, 0.005f, FColor::Yellow, TEXT("Turn Right"));
		else if (LookAxisVector.X < 0) GEngine->AddOnScreenDebugMessage(-1, 0.005f, FColor::Yellow, TEXT("Turn Left"));

	}
}

/*Character Jump*/
void ATestEnhancedInput::Jump(){
	Super::Jump();
	
	GEngine->AddOnScreenDebugMessage(-1, 0.005f, FColor::Yellow, TEXT("Jump"));
}

/*Caemra Zoon In and Out*/
void ATestEnhancedInput::Zoom(const FInputActionValue &Value)
{
	float ZoomValue = Value.Get<float>();

	if (ZoomValue > 0 && CameraBoom->TargetArmLength > 250)
	{
		CameraBoom->TargetArmLength -= ZoomVariable;
		GEngine->AddOnScreenDebugMessage(-1, 0.005f, FColor::Yellow, TEXT("Zoom In"));
	}
	else if (ZoomValue < 0 && CameraBoom->TargetArmLength < 700)
	{
		CameraBoom->TargetArmLength += ZoomVariable;
		GEngine->AddOnScreenDebugMessage(-1, 0.005f, FColor::Yellow, TEXT("Zoom Out"));
	}
}